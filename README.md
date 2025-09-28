
---

## Table of contents
- [Polski](#polski)
- [English](#english)

---

# Polski

Lekka biblioteka C do zarządzania **zbiorem prefiksów IPv4** (baza 32-bit + maska /0…/32).  
Zapewnia wyszukiwanie **najdłuższego dopasowanego prefiksu** (LPM).  
**Bez alokacji dynamicznej** napisana z myślą o **embedded**.

## Cechy

- Struktura danych: **Patricia trie**.
- Operacje: `add`, `del`, `check`, `clear_all`.
- Usunięcie rodzica nie usuwa dzieci (mniejszych prefiksów).
- Brak `malloc`/`free` – **statyczna pula** zarządzana własną areną.
- Indeksy 16-/32-bit w zależności od `PREFIX_MAX_NODES`.

---

## Struktura

```
include/
  prefix.h
  prefix_bits.h
src/
  patricia_core.c
  patricia_core.h
  prefix_bits.c
  prefix.c
apps/
  main.c        # Przykładowe użycie
```

---

## Kompilacja

### Makefile

W katalogu głównym znajduje się plik Makefile, który automatyzuje proces budowania.
Obsługiwane są zarówno systemy Windows jak i Linux.
Domyślnie PREFIX_MAX_NODES=2048.
Możesz ustawić własną wartość przy wywołaniu make (bez edycji kodu).
Dzięki temu biblioteka zostanie skompilowana z nowym limitem węzłów.
Jeśli wartość ≤ 32767, używany jest 16-bitowy indeks (int16_t).
Dla większych wartości automatycznie przełączane jest na 32-bitowy (int32_t).

```bash
# Windows
mingw32-make PREFIX_MAX_NODES=4096

# Linux
make PREFIX_MAX_NODES=4096
```

- na Windowsie powstanie plik ipv4.exe
- na Linuxie powstanie plik ipv4

Uruchomienie:

```bash
# Windows
.\ipv4.exe

# Linux
./ipv4
```

Sprzątanie:

```bash
# Windows
mingw32-make clean

# Linux
make clean
```

### Windows manualnie

```powershell
cd ...\ipv4

# Możesz też dodać -DPREFIX_MAX_NODES=4096 aby ustawić własny rozmiar puli
gcc -DPREFIX_MAX_NODES=4096 -Iinclude -Wall -Wextra -std=c11 -c src\patricia_core.c -o src\patricia_core.o
gcc -Iinclude -Wall -Wextra -std=c11 -c src\prefix_bits.c  -o src\prefix_bits.o
gcc -Iinclude -Wall -Wextra -std=c11 -c src\prefix.c       -o src\prefix.o
gcc -Iinclude -Wall -Wextra -std=c11 -c apps\main.c        -o apps\main.o

gcc src\patricia_core.o src\prefix_bits.o src\prefix.o apps\main.o -o ipv4.exe
.\ipv4.exe
```

### Linux manualnie

```bash
cd ipv4

# Możesz też dodać -DPREFIX_MAX_NODES=4096 aby ustawić własny rozmiar puli
cc -DPREFIX_MAX_NODES=4096 -Iinclude -Wall -Wextra -std=c11 -c src/patricia_core.c -o src/patricia_core.o
cc -Iinclude -Wall -Wextra -std=c11 -c src/prefix_bits.c   -o src/prefix_bits.o
cc -Iinclude -Wall -Wextra -std=c11 -c src/prefix.c        -o src/prefix.o
cc -Iinclude -Wall -Wextra -std=c11 -c apps/main.c         -o apps/main.o

cc src/patricia_core.o src/prefix_bits.o src/prefix.o apps/main.o -o ipv4
./ipv4
```

---

## API

Nagłówek: `#include "prefix.h"`

```c
int  add(unsigned int base, char mask);
/* Dodaje prefiks (base/mask) do zbioru. */

int  del(unsigned int base, char mask);
/* Usuwa dokładnie wskazany prefiks. */

char check(unsigned int ip);
/* Zwraca maskę najdłuższego prefiksu zawierającego ip,
   lub -1, jeśli brak dopasowania. */

void clear_all(void);
/* Czyści całą strukturę */
```

---

## Przykład użycia

```c
#include <stdint.h>
#include "prefix.h"

static uint32_t ip4(unsigned a,unsigned b,unsigned c,unsigned d){
    return (a<<24)|(b<<16)|(c<<8)|d;
}

int main(void){
    clear_all();

    add(ip4(10,20,0,0), 16);
    add(ip4(10,20,1,0), 24);
    add(0u, 0);

    int m1 = (int)(signed char)check(ip4(10,20,1,5));   // 24
    int m2 = (int)(signed char)check(ip4(10,20,200,10)); // 16
    int m3 = (int)(signed char)check(ip4(1,2,3,4));      // 0

    del(ip4(10,20,0,0),16);

    return (m1==24 && m2==16 && m3==0) ? 0 : 1;
}
```

---

## Wydajność

- `add`, `del`, `check` – maksymalnie 32 kroki (po 1 bicie).
- Pamięć: określona przez `PREFIX_MAX_NODES`, bez dynamicznej alokacji.

---

## Testy

Aktualnie `apps/main.c` zawiera **prosty program demonstracyjny**:
- dodawanie kilku prefiksów,
- sprawdzanie przykładowych adresów,
- usuwanie prefiksów i ponowny odczyt `check()`.



# English

Lightweight C library for managing **IPv4 prefix sets** (32-bit base + mask /0…/32).  
Provides **longest prefix match (LPM)** search.  
**No dynamic allocation** – designed with **embedded** use in mind.

## Features

- Data structure: **Patricia trie**.
- Operations: `add`, `del`, `check`, `clear_all`.
- Deleting a parent does not remove child prefixes.
- No `malloc`/`free` – **static node pool** managed with a custom arena.
- 16-/32-bit indices depending on `PREFIX_MAX_NODES`.

---

## Project layout

```
include/
  prefix.h
  prefix_bits.h
src/
  patricia_core.c
  patricia_core.h
  prefix_bits.c
  prefix.c
apps/
  main.c        # Sample usage
```

---

## Build

### Makefile

A `Makefile` is included to automate the build on both Windows and Linux.
By default, PREFIX_MAX_NODES=2048.
You can override this at build time without editing the source.
This compiles the library with a custom maximum number of nodes.
If the value ≤ 32767, a 16-bit index (int16_t) is used.
For larger values, the code automatically switches to 32-bit (int32_t).

```bash
# Windows
mingw32-make PREFIX_MAX_NODES=4096

# Linux
make PREFIX_MAX_NODES=4096
```

- Produces `ipv4.exe` on Windows  
- Produces `ipv4` on Linux

Run:

```bash
# Windows
.\ipv4.exe

# Linux
./ipv4
```

Clean:

```bash
# Windows
mingw32-make clean

# Linux
make clean
```

### Manual build (Windows)

```powershell
cd ...\ipv4

# You can also add -DPREFIX_MAX_NODES=4096 to set a custom pool size.
gcc -DPREFIX_MAX_NODES=4096 -Iinclude -Wall -Wextra -std=c11 -c src\patricia_core.c -o src\patricia_core.o
gcc -Iinclude -Wall -Wextra -std=c11 -c src\prefix_bits.c  -o src\prefix_bits.o
gcc -Iinclude -Wall -Wextra -std=c11 -c src\prefix.c       -o src\prefix.o
gcc -Iinclude -Wall -Wextra -std=c11 -c apps\main.c        -o apps\main.o

gcc src\patricia_core.o src\prefix_bits.o src\prefix.o apps\main.o -o ipv4.exe
.\ipv4.exe
```

### Manual build (Linux)

```bash
cd ipv4

# You can also add -DPREFIX_MAX_NODES=4096 to set a custom pool size.
cc -DPREFIX_MAX_NODES=4096 -Iinclude -Wall -Wextra -std=c11 -c src/patricia_core.c -o src/patricia_core.o
cc -Iinclude -Wall -Wextra -std=c11 -c src/prefix_bits.c   -o src/prefix_bits.o
cc -Iinclude -Wall -Wextra -std=c11 -c src/prefix.c        -o src/prefix.o
cc -Iinclude -Wall -Wextra -std=c11 -c apps/main.c         -o apps/main.o

cc src/patricia_core.o src/prefix_bits.o src/prefix.o apps/main.o -o ipv4
./ipv4
```

---

## API

Header: `#include "prefix.h"`

```c
int  add(unsigned int base, char mask);
/* Add prefix (base/mask) to the set.
   Returns 0 on success, -1 for invalid arguments. */

int  del(unsigned int base, char mask);
/* Remove the exact prefix. Returns 0 even if prefix not present. */

char check(unsigned int ip);
/* Return mask length of the longest matching prefix for ip,
   or -1 if no match. */

void clear_all(void);
/* Clear the entire structure */
```

---

## Example

```c
#include <stdint.h>
#include "prefix.h"

static uint32_t ip4(unsigned a,unsigned b,unsigned c,unsigned d){
    return (a<<24)|(b<<16)|(c<<8)|d;
}

int main(void){
    clear_all();

    add(ip4(10,20,0,0), 16);
    add(ip4(10,20,1,0), 24);
    add(0u, 0);

    int m1 = (int)(signed char)check(ip4(10,20,1,5));   // 24
    int m2 = (int)(signed char)check(ip4(10,20,200,10)); // 16
    int m3 = (int)(signed char)check(ip4(1,2,3,4));      // 0

    del(ip4(10,20,0,0),16);

    return (m1==24 && m2==16 && m3==0) ? 0 : 1;
}
```

---

## Performance

- `add`, `del`, `check` – at most 32 steps (1 per bit).
- Memory: fixed, controlled by `PREFIX_MAX_NODES`. No dynamic allocations.

---

## Tests

Currently, `apps/main.c` is a **simple demo program**:
- adds a few prefixes,
- checks several sample addresses,
- deletes prefixes and shows `check()` results afterwards.

---

