/*
| File    : termcolors.h
| Purpose : ANSI color configuration for terminal output.
| Author  : Martin Rizzo | <martinrizzo@gmail.com>
| Date    : Oct 12, 2026
| Repo    : https://github.com/martin-rizzo/PromptExtractor
| License : MIT
|- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
|                              PromptExtractor
| A lightweight CLI tool to extract embedded prompts from AI-generated images.
\_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/
#ifndef TERMCOLORS_H__
#define TERMCOLORS_H__

/**
 * Represents a set of ANSI color codes for terminal output.
 * 
 * This struct holds pointers to ANSI escape codes that define the color theme.
 * Each member corresponds to a specific color category.
 */
struct TermColors {
    const char *label,     ///< Labels color.
               *data,      ///< Color for data values.
               *string,    ///< Color for strings.
               *highlight, ///< Highlight color for emphasis.
               *success,   ///< Color for success messages.
               *error,     ///< Color for error messages.
               *info,      ///< Color for informational messages.
               *reset;     ///< ANSI code to reset color to default.
};
typedef struct TermColors TermColors;


#endif /*TERMCOLORS_H__*/
