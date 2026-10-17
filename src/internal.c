/*
| File    : internal.c
| Purpose : Internal helper functions used across the project.
| Author  : Martin Rizzo | <martinrizzo@gmail.com>
| Date    : Oct 10, 2026
| Repo    : https://github.com/martin-rizzo/PromptExtractor
| License : MIT
|- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
|                              PromptExtractor
| A lightweight CLI tool to extract embedded prompts from AI-generated images.
\_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include "internal.h"


/** Returns true if the character is considered whitespace. */
static inline bool _is_whitespace(char c) {
    return (c <= ' ' && c > '\x08');
}


/*===========================================================================
|  ////////////////////////// PUBLIC FUNCTIONS ///////////////////////////  | 
===========================================================================*/

// Strips leading and trailing whitespace from a string in place.
PeUTF8* pe_strip_inplace(PeUTF8* str, size_t len) {
    assert(str != NULL);
    if (len==0) { len = strlen(str); }

    // trims trailing whitespace by replacing them with '\0'
    PeUTF8* ptr = &str[len-1];
    while (ptr>=str && _is_whitespace(*ptr)) { *ptr-- = '\0'; }

    // skips leading whitespace by advancing the pointer
    ptr = str;
    while (*ptr!='\0' && _is_whitespace(*ptr)) { ++ptr; }

    return ptr;

}


// Splits a raw text string by populating an array with each found text line.
int pe_split_lines(PeUTF8* raw_text,
                   PeUTF8* lines_array[],
                   int     lines_array_size
){
    if (raw_text == NULL || lines_array == NULL || lines_array_size <= 0) {
        return 0;
    }
    int     line_idx   = 0;
    PeUTF8* line_start = raw_text;
    PeUTF8* ptr        = raw_text;
    for ( ptr=raw_text ; *ptr!='\0' && line_idx<lines_array_size ; ++ptr )  {
        if (*ptr == '\n') {
            lines_array[line_idx++] = line_start;
            *ptr='\0'; line_start=(ptr+1);
        }
    }
    // always appends the last line (which does NOT end with '\n')
    // but only if there is enough space in lines_array.
    if (line_idx<lines_array_size) { lines_array[line_idx++] = line_start; }
    else                           { line_idx++; }
    return line_idx;
}
