/*
| File    : internal.h
| Purpose : Internal helper functions used across the project.
| Author  : Martin Rizzo | <martinrizzo@gmail.com>
| Date    : Oct 10, 2026
| Repo    : https://github.com/martin-rizzo/PromptExtractor
| License : MIT
|- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
|                              PromptExtractor
| A lightweight CLI tool to extract embedded prompts from AI-generated images.
\_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/
#ifndef PE__INTERNAL_H__
#define PE__INTERNAL_H__
#include "types.h"

/**
 * Suppresses "unused variable" warnings for function arguments.
 *
 * Explicitly indicate that it is intentionally unused, typically
 * because the function is still under development.
 */
#define unused(x) ((void)(&x))

/**
 * Strips leading and trailing whitespace from a string in place.
 *
 * Trailing whitespace characters are replaced with null terminators ('\0')
 * directly within the provided buffer. Leading whitespace is skipped by
 * advancing the returned pointer.
 *
 * @param str  The input string buffer to be stripped (will be modified in place).
 * @param len  The length of the string. If 0, the length is calculated automatically.
 * @return
 *     A pointer to the start of the stripped string.
 */
PeUTF8* pe_strip_inplace(PeUTF8* str, size_t len);


/**
 * Splits a raw text string by populating an array with each found text line.
 *
 * Iterates through `raw_text`, replacing newline characters ('\n') with
 * null terminators ('\0') to delimit each line. The starting address of
 * each line is stored in `lines_array`.
 *
 * @param raw_text         The input text string to be split (will be modified in place).
 * @param lines_array      An array of pointers that will be populated with
 *                         each extracted text line.
 * @param lines_array_size The maximum number of lines that can be stored
 *                         in `lines_array`.
 * @return The number of lines successfully added to `lines_array`.
 *         Processing stops early if the array reaches its capacity.
 */
int pe_split_lines(PeUTF8* raw_text, PeUTF8* lines_array[], int lines_array_size);


#endif /*PE__INTERNAL_H__*/
