# Compiler options
pkglibs = sdl2 SDL2_mixer SDL2_image SDL2_ttf
CC=gcc
CFLAGS= -Wall $(shell pkg-config --cflags $(pkglibs))
LDFLAGS= $(shell pkg-config --libs-only-other $(pkglibs))
LDLIBS = $(shell pkg-config --libs-only-L --libs-only-l $(pkglibs)) -lm


EXECUTABLE_NAME= zengine

# Folders
SRC= ./code/
BIN= .
OBJ=$(BIN)/obj


# Files
SOURCE_FILES= $(wildcard $(SRC)/*.c)
SOURCE_HEADERS = $(wildcard $(SRC)/*.h)

EXECUTABLE_FILES = $(EXECUTABLE_NAME:%=$(BIN)/%.exe)
OBJECT_FILES     = $(SOURCE_FILES:%.c=$(OBJ)/%.o)


ifeq ($(OS),Windows_NT)
ifeq ($(MAKECMDGOALS),debug)
	LDFLAGS += -mconsole
endif
endif
debug: CFLAGS += -g -Og
debug: build

release: CFLAGS += -O3
release: clean
release: build

build: $(EXECUTABLE_FILES)

clean:
	rm -r $(OBJ)

.PHONY: all build debug clean

# define general rule to link .exe from .o
$(EXECUTABLE_FILES): $(OBJECT_FILES)
	@mkdir -p $(@D)
	$(CC) -o $@ $+ $(LDLIBS) $(LDFLAGS)

# define general rule to compile .c to .o + refresh all if headers changed
$(OBJECT_FILES): $(OBJ)/%.o:%.c $(SOURCE_HEADERS)
	@mkdir -p $(@D)
	$(CC) -c -o $@ $< $(CFLAGS)
