# Standard Makefile for a C project with sources in src/
# Default installation path is $(HOME)/.local/bin

CC         = gcc
CFLAGS     = -std=c99 -pedantic -Wall -Wextra -Wunused-function -Isrc -Isrc/image_fmt -Ideps -g
LDFLAGS    = 
TARGET_DIR = bin
TARGET     = $(TARGET_DIR)/readprompt
HEADERS    = $(wildcard src/*.h) $(wildcard src/image_fmt/*.h)
SRCS       = $(wildcard src/*.c) $(wildcard src/image_fmt/*.c)
OBJS       = $(SRCS:.c=.o)

# Define default installation prefix
ifndef PREFIX
PREFIX = $(HOME)/.local
endif

.PHONY: all clean run install uninstall

all: $(TARGET)

$(TARGET): $(TARGET_DIR) $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

run: $(TARGET)
	./$(TARGET) test.*

install: $(TARGET)
	install -d $(PREFIX)/bin
	install -s $(TARGET) $(PREFIX)/bin/

uninstall:
	rm -f $(PREFIX)/bin/$(TARGET)

distclean: clean
	rm -f Makefile

$(TARGET_DIR):
	mkdir -p $(TARGET_DIR)
