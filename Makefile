# File    : Makefile
# Purpose : Build configuration for PromptExtractor
# Author  : Martin Rizzo | <martinrizzo@gmail.com>
# Date    : Oct 2, 2026
# Repo    : https://github.com/martin-rizzo/PromptExtractor
# License : MIT
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#                              PromptExtractor
# A lightweight CLI tool to extract embedded prompts from AI-generated images.
#_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _

# ==============================================================================
# Compiler Options and Flags
# ==============================================================================
EXTRAFLAGS := -g
#EXTRAFLAGS := -O2
CC       := gcc
CFLAGS   := -std=c99 -pedantic -Wall -Wextra -Wunused-function -Iinclude -Ilibpromptex -Ilibpromptex/yyjson_0_13 $(EXTRAFLAGS)
AR       := ar
ARFLAGS  := rcs

# ==============================================================================
# Final Target Names
# ==============================================================================
LIB_NAME := libpromptex.a
BIN_NAME := promptex

# ==============================================================================
# Directory Paths
# ==============================================================================
SRC_DIR   := src
LIB_DIR   := libpromptex
BUILD_DIR := build
BIN_DIR   := bin

# ==============================================================================
# Automatic Source and Object File Detection
# ==============================================================================
# Static library source and object files (incluyendo subcarpetas)
LIB_SRCS := $(wildcard $(LIB_DIR)/*.c)               \
            $(wildcard $(LIB_DIR)/image_fmt/*.c)     \
            $(wildcard $(LIB_DIR)/prompt_fmt/*.c)    \
			$(wildcard $(LIB_DIR)/yyjson_0_13/*.c)   \
			$(wildcard $(LIB_DIR)/comfyui_nodes/*.c)
LIB_OBJS := $(patsubst $(LIB_DIR)/%.c, $(BUILD_DIR)/$(LIB_DIR)/%.o, $(LIB_SRCS))

# Executable (CLI) source and object files
CLI_SRCS := $(wildcard $(SRC_DIR)/*.c)
CLI_OBJS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/$(SRC_DIR)/%.o, $(CLI_SRCS))

# Final products with full paths
FINAL_LIB := $(BIN_DIR)/$(LIB_NAME)
FINAL_BIN := $(BIN_DIR)/$(BIN_NAME)

# ==============================================================================
# Compilation Rules (Main Rules)
# ==============================================================================

# The default target builds everything
all: $(FINAL_LIB) $(FINAL_BIN)

# Rule to link the final binary (CLI) using the static library
$(FINAL_BIN): $(CLI_OBJS) $(FINAL_LIB) | $(BIN_DIR)
	@echo "====== Linking final executable: $@ ======"
	$(CC) $(CFLAGS) $(CLI_OBJS) -L$(BIN_DIR) -lpromptex -o $@
	@echo "Build successful! Executable created at: $@"

# Rule to package the static library (.a)
$(FINAL_LIB): $(LIB_OBJS) | $(BIN_DIR)
	@echo "====== Creating static library: $@ ======"
	$(AR) $(ARFLAGS) $@ $(LIB_OBJS)

# Pattern rule to compile library objects (crea subdirectorios automáticamente)
$(BUILD_DIR)/$(LIB_DIR)/%.o: $(LIB_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "Compiling library component: $<"
	$(CC) $(CFLAGS) -c $< -o $@

# Pattern rule to compile CLI objects
$(BUILD_DIR)/$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "Compiling CLI component: $<"
	$(CC) $(CFLAGS) -c $< -o $@

# ==============================================================================
# Dynamic Directory Creation
# ==============================================================================
$(BIN_DIR):
	@mkdir -p $@

# ==============================================================================
# Project Cleanup
# ==============================================================================
clean:
	@echo "Cleaning up compilation files..."
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "Cleanup completed."

# Prevents conflicts if files named 'all' or 'clean' exist
.PHONY: all clean
