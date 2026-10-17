/*
| File    : params_defs.h
| Purpose : Definitions for parameter types, fileinfo and storage.
| Author  : Martin Rizzo | <martinrizzo@gmail.com>
| Date    : Oct 9, 2026
| Repo    : https://github.com/martin-rizzo/PromptExtractor
| License : MIT
|- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
|                              PromptExtractor
| A lightweight CLI tool to extract embedded prompts from AI-generated images.
\_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include "types.h"
#include "params_defs.h"

/*===========================================================================
|  ////////////////////////// PUBLIC FUNCTIONS ///////////////////////////  | 
===========================================================================*/

// Sets the optional long integer value from a string representation.
void pe_set_long(PeLong* long_, const PeUTF8* strvalue) {
    assert(long_ != NULL);
    if (strvalue) {
        char* end;
        errno = 0;
        long_->has_value = true;
        long_->value     = strtol(strvalue, &end, 10);
        long_->_strvalue = strvalue;
        long_->_converr  = (end == strvalue) || (errno == ERANGE);
    }
    else {
        long_->has_value = false;
        long_->value     = 0;
        long_->_strvalue = NULL;
        long_->_converr  = false;
    }
}

// Sets the optional floating-point value from a string representation.
void pe_set_float(PeFloat* float_, const PeUTF8* strvalue) {
    assert(float_ != NULL);
    if (strvalue) {
        char* end;
        errno = 0;
        float_->has_value = true;
        float_->value     = strtof(strvalue, &end);;
        float_->_strvalue = strvalue;
        float_->_converr  = (end == strvalue) || (errno == ERANGE);
    }
    else {
        float_->has_value = false;
        float_->value     = 0.0f;
        float_->_strvalue = NULL;
        float_->_converr  = false;
    }
}

