CC      := gcc
CFLAGS  := -Iinclude -Wall -Wextra -std=c11

SRC     := src/patricia_core.c src/prefix_bits.c src/prefix.c apps/main.c
OBJ     := $(SRC:.c=.o)

ifeq ($(OS),Windows_NT)
    EXE    := .exe
    RM     := cmd /C del /Q /F
    SEP    := \\
else
    EXE    :=
    RM     := rm -f
    SEP    := /
endif

TARGET  := ipv4$(EXE)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@


.PHONY: clean
clean:
	-$(RM) src$(SEP)patricia_core.o src$(SEP)prefix_bits.o src$(SEP)prefix.o apps$(SEP)main.o
	-$(RM) $(TARGET)
