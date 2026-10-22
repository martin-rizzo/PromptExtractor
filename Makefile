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

#============================================================================
# Compiler Options and Flags
#============================================================================

# Default build type
BUILD_TYPE ?= release

# Flags for automatic dependency generation (.d files)
DEPFLAGS := -MMD -MP

# Compiler and linker settings
AR       := ar
ARFLAGS  := rcs
CC       := gcc
CFLAGS   := -std=c99 -pedantic -Wall -Wextra -Wunused-function \
            -Iinclude -Ilibpromptex -Ilibpromptex/yyjson_0_13 \
            $(DEPFLAGS) $(EXTRA_CFLAGS)
LDFLAGS  :=

# Build type specific flags
ifeq ($(BUILD_TYPE),debug)
    CFLAGS  += -g -O0 -DDEBUG
    LDFLAGS += 
else
    CFLAGS  += -Os -s -ffunction-sections -fdata-sections -DNDEBUG
    LDFLAGS += -Wl,--gc-sections
endif

#============================================================================
# Final Target Names
#============================================================================
ifeq ($(BUILD_TYPE),debug)
    BIN_NAME := promptexd
    LIB_NAME := libpromptexd.a
else
    BIN_NAME := promptex
    LIB_NAME := libpromptex.a
endif

#============================================================================
# Directory Paths
#============================================================================
SRCDIR    := src
SRCLIBDIR := libpromptex
OBJDIR    := build/$(BUILD_TYPE)
BINDIR    := bin
LIBDIR    := lib

#============================================================================
# Automatic Source and Object File Detection
#============================================================================
# Static library source and object files (including subdirectories)
LIB_SRCS := $(wildcard $(SRCLIBDIR)/*.c)               \
            $(wildcard $(SRCLIBDIR)/image_fmt/*.c)     \
            $(wildcard $(SRCLIBDIR)/prompt_fmt/*.c)    \
            $(wildcard $(SRCLIBDIR)/yyjson_0_13/*.c)   \
            $(wildcard $(SRCLIBDIR)/comfyui_nodes/*.c)
LIB_OBJS := $(patsubst $(SRCLIBDIR)/%.c, $(OBJDIR)/$(SRCLIBDIR)/%.o, $(LIB_SRCS))

# Executable (CLI) source and object files
CLI_SRCS := $(wildcard $(SRCDIR)/*.c)
CLI_OBJS := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/$(SRCDIR)/%.o, $(CLI_SRCS))

# Track all generated dependency files
ALL_DEPS := $(LIB_OBJS:.o=.d) $(CLI_OBJS:.o=.d)

# Final products with full paths
FINAL_LIB := $(LIBDIR)/$(LIB_NAME)
FINAL_BIN := $(BINDIR)/$(BIN_NAME)

#============================================================================
# Compilation Rules (Main Rules)
#============================================================================

# The default target builds everything
all: $(FINAL_LIB) $(FINAL_BIN)

# Rule to link the final binary (CLI) using the static library
$(FINAL_BIN): $(CLI_OBJS) $(FINAL_LIB) | $(BINDIR)
	@echo "====== Linking final executable: $@ (Build: $(BUILD_TYPE)) ======"
	$(CC) $(CFLAGS) $(LDFLAGS) $(CLI_OBJS) $(FINAL_LIB) -o $@
	@echo "Build successful! Executable created at: $@"

# Rule to package the static library (.a)
$(FINAL_LIB): $(LIB_OBJS) | $(LIBDIR)
	@echo "====== Creating static library: $@ (Build: $(BUILD_TYPE)) ======"
	$(AR) $(ARFLAGS) $@ $(LIB_OBJS)

# Pattern rule to compile library objects (automatically creates subdirectories)
$(OBJDIR)/$(SRCLIBDIR)/%.o: $(SRCLIBDIR)/%.c
	@mkdir -p $(dir $@)
	@echo "Compiling library component: $< ($(BUILD_TYPE))"
	$(CC) $(CFLAGS) -c $< -o $@

# Pattern rule to compile CLI objects
$(OBJDIR)/$(SRCDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	@echo "Compiling CLI component: $< ($(BUILD_TYPE))"
	$(CC) $(CFLAGS) -c $< -o $@

#============================================================================
# Dynamic Directory Creation
#============================================================================
$(BINDIR) $(LIBDIR):
	@mkdir -p $@

#============================================================================
# Project Cleanup
#============================================================================
clean:
	@echo "Cleaning up all compilation files..."
	rm -rf build bin lib
	@echo "Cleanup completed."

# Prevents conflicts if files named 'all' or 'clean' exist
.PHONY: all clean debug release

# Specific targets for debug and release
debug:
	@$(MAKE) all BUILD_TYPE=debug

release:
	@$(MAKE) all BUILD_TYPE=release

#============================================================================
# Include Dependencies (crucial for .h changes)
#============================================================================
-include $(ALL_DEPS)
