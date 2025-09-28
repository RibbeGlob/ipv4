#include "patricia_core.h"
#include "prefix_bits.h"
#include <string.h>
#include <limits.h>
#include <stdint.h>

#if PREFIX_MAX_NODES <= 32767
typedef int16_t idx_t;
#else
typedef int32_t idx_t;
#endif

#define IDX_NULL ((idx_t)-1)

typedef struct Node {
    uint32_t prefix;
    uint8_t  maskLen;
    uint8_t  has_value;
    idx_t    child[2];
} Node;

static Node  g_nodes[PREFIX_MAX_NODES];
static idx_t g_free_stack[PREFIX_MAX_NODES];
static uint32_t g_free_top = 0;   
static idx_t    g_root     = IDX_NULL;
static uint8_t  g_inited   = 0;

static void arena_init(void) {
    g_free_top = 0;
    for (int i = (int)(PREFIX_MAX_NODES - 1); i >= 0; --i) {
        g_free_stack[g_free_top] = (idx_t)i;
        g_free_top++;
    }
    g_root = IDX_NULL;
    g_inited = 1u;
}

static idx_t pool_alloc_node(void){
    if (!g_inited) arena_init();
    if (g_free_top == 0) return IDX_NULL;

    idx_t idx = g_free_stack[--g_free_top];

    Node* n = &g_nodes[idx];
    memset(n, 0, sizeof(*n));
    n->child[0] = IDX_NULL;
    n->child[1] = IDX_NULL;

    return idx;
}


static void pool_free_node(idx_t idx){
    if (idx == IDX_NULL) return;
    if (g_free_top < PREFIX_MAX_NODES){
        g_free_stack[g_free_top++] = idx;
    }
}

static idx_t new_node(uint32_t prefix, unsigned maskLen, int has_value){
    idx_t idx = pool_alloc_node();
    if (idx == IDX_NULL) return IDX_NULL;

    Node* n = &g_nodes[idx];
    n->prefix    = normalize_prefix(prefix, maskLen);
    n->maskLen   = (uint8_t)maskLen;
    n->has_value = (uint8_t)(has_value ? 1u : 0u);

    return idx;
}

static void ensure_root(void){
    if (!g_inited) arena_init();
    if (g_root == IDX_NULL){
        g_root = new_node(0u, 0u, 0);
    }
}

static int node_matches_ip(const Node* n, uint32_t ip){
    return normalize_prefix(ip, n->maskLen) == n->prefix;
}

int pat_add(uint32_t base, unsigned maskLen){
    if (maskLen > 32u) return -1;
    if (!base_is_normalized(base, maskLen)) return -1;

    ensure_root();

    if (maskLen == 0u){
        g_nodes[g_root].has_value = 1u;
        return 0;
    }

    idx_t  n_idx = g_root;
    Node*  n     = &g_nodes[n_idx];

    for (;;){
        if (n->maskLen == maskLen){
            n->has_value = 1u;
            return 0;
        }

        int   dir   = bit_at(base, n->maskLen);
        idx_t c_idx = n->child[dir];

        if (c_idx == IDX_NULL){
            idx_t leaf = new_node(base, maskLen, 1);
            if (leaf == IDX_NULL) return -1;
            n->child[dir] = leaf;
            return 0;
        }

        Node*    c   = &g_nodes[c_idx];
        unsigned lcp = common_prefix_len(base, c->prefix);

        if (maskLen <= c->maskLen && lcp >= maskLen){
            if (maskLen == c->maskLen){
                c->has_value = 1u;
                return 0;
            }
            idx_t mid_idx = new_node(base, maskLen, 1);
            if (mid_idx == IDX_NULL) return -1;

            Node* mid   = &g_nodes[mid_idx];
            int   dir_c = bit_at(c->prefix, maskLen);
            mid->child[dir_c] = c_idx;

            n->child[dir] = mid_idx;
            return 0;
        }

        if (lcp >= c->maskLen && c->maskLen < maskLen){
            n_idx = c_idx;
            n     = c;
            continue;
        }

        idx_t mid_idx = new_node(base, lcp, 0);
        if (mid_idx == IDX_NULL) return -1;

        Node* mid   = &g_nodes[mid_idx];
        int   dir_c = bit_at(c->prefix, lcp);
        mid->child[dir_c] = c_idx;

        if (lcp == maskLen){
            mid->has_value = 1u;
        } else {
            int   dir_new  = bit_at(base, lcp);
            idx_t leaf_idx = new_node(base, maskLen, 1);
            if (leaf_idx == IDX_NULL){
                pool_free_node(mid_idx);
                return -1;
            }
            mid->child[dir_new] = leaf_idx;
        }

        n->child[dir] = mid_idx;
        return 0;
    }
}


int pat_check(uint32_t ip){
    if (g_root == IDX_NULL) return -1;

    idx_t n_idx = g_root;
    Node* n     = &g_nodes[n_idx];

    int best = n->has_value ? 0 : -1;

    for(;;){
        if (n->maskLen >= 32u) break;

        int   dir   = bit_at(ip, n->maskLen);
        idx_t c_idx = n->child[dir];
        if (c_idx == IDX_NULL) break;

        Node* c = &g_nodes[c_idx];
        if (!node_matches_ip(c, ip)) break;

        n_idx = c_idx;
        n     = c;

        if (n->has_value) best = (int)n->maskLen;

        if (n->maskLen == 32u) break;
    }
    return best;
}

static int has_children(const Node* n){
    return (n->child[0] != IDX_NULL) || (n->child[1] != IDX_NULL);
}

static idx_t compress_if_possible(idx_t idx){
    if (idx == IDX_NULL) return IDX_NULL;
    Node* n = &g_nodes[idx];

    if (n->has_value) return idx;
    int c0 = (n->child[0] != IDX_NULL);
    int c1 = (n->child[1] != IDX_NULL);

    if (c0 && c1) return idx;

    if (!c0 && !c1){
        pool_free_node(idx);
        return IDX_NULL;
    }

    idx_t only = n->child[c1 ? 1 : 0];
    pool_free_node(idx);
    return only;
}

int pat_del(uint32_t base, unsigned maskLen){
    if (maskLen > 32u) return -1;
    if (!base_is_normalized(base, maskLen)) return -1;
    if (g_root == IDX_NULL) return 0;

    idx_t path[40];  int8_t dirp[40];  int depth = 0;

    path[depth] = IDX_NULL; dirp[depth] = -1; depth++;
    path[depth] = g_root;   dirp[depth] = -1; depth++;

    idx_t n_idx = g_root;                                     
    Node* n     = &g_nodes[n_idx];

    if (maskLen == 0u){
        n->has_value = 0u;
        if (!has_children(n) && !n->has_value){
            pool_free_node(g_root);
            g_root = IDX_NULL;
        }
        return 0;
    }

    while (n->maskLen < maskLen){
        int   dir   = bit_at(base, n->maskLen);
        idx_t c_idx = n->child[dir];
        if (c_idx == IDX_NULL) return 0;

        Node* c = &g_nodes[c_idx];
        unsigned lcp      = common_prefix_len(base, c->prefix);
        unsigned min_need = (maskLen < c->maskLen) ? maskLen : c->maskLen;

        if (lcp < min_need) return 0;

        n_idx = c_idx; n = c;
        path[depth] = n_idx; dirp[depth] = (int8_t)dir; depth++;
        if (depth >= 39) break;
    }

    if (n->maskLen != maskLen) return 0;
    if (!n->has_value)        return 0;

    n->has_value = 0u;

    for (int i = depth - 1; i >= 2; --i){
        idx_t cur = path[i];
        idx_t par = path[i - 1];
        int8_t pd = dirp[i];
        if (par == IDX_NULL) continue;

        Node* p = &g_nodes[par];
        idx_t repl = compress_if_possible(cur);
        p->child[pd] = repl;
    }

    if (g_root != IDX_NULL){
        Node* r = &g_nodes[g_root];
        if (!r->has_value && r->child[0] == IDX_NULL && r->child[1] == IDX_NULL){
            pool_free_node(g_root);
            g_root = IDX_NULL;
        }
    }
    return 0;
}


void pat_clear(void){
    arena_init();
}