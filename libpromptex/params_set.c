/*
| File    : params_defs.h
| Purpose : Set parameters of types like PeString, PeLong, PeFloat, etc...
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
#include <float.h>
#include <promptex/types.h>
#include "params_set.h"

/*===========================================================================
|  ////////////////////////// PUBLIC FUNCTIONS ///////////////////////////  | 
===========================================================================*/

// Sets the optional long integer value from a string representation.
void pe_set_strlong(PeLong* long_, const PeUTF8* strvalue) {
    assert(long_ != NULL);
    if (strvalue) {
        char* end;
        errno = 0;
        long_->has_value = true;
        long_->value     = strtol(strvalue, &end, 10);
        long_->_converr  = (end == strvalue) || (errno == ERANGE);
    }
    else {
        long_->has_value = false;
    }
}

// Sets the optional long integer value from a double value.
void pe_set_long(PeLong* long_, double value) {
    assert(long_ != NULL);
    long_->has_value = true;
    long_->value     = (long)value;
    long_->_converr  = false;
}


// Sets the optional double value from a string representation.
void pe_set_strdouble(PeDouble* double_, const PeUTF8* strvalue) {
    assert(double_ != NULL);
    if (strvalue) {
        char* end;
        errno = 0;
        double_->has_value = true;
        double_->value     = strtod(strvalue, &end);
        double_->_converr  = (end == strvalue) || (errno == ERANGE);
    }
    else {
        double_->has_value = false;
    }
}


// Sets the optional double value from a double value.
void pe_set_double(PeDouble* double_, double value) {
    assert(double_ != NULL);
    double_->has_value = true;
    double_->value     = value;
    double_->_converr  = false;
}



// Sets the optional floating-point value from a string representation.
void pe_set_strfloat(PeFloat* float_, const PeUTF8* strvalue) {
    assert(float_ != NULL);
    if (strvalue) {
        char* end;
        errno = 0;
        float_->has_value = true;
        float_->value     = strtof(strvalue, &end);;
        float_->_converr  = (end == strvalue) || (errno == ERANGE);
    }
    else {
        float_->has_value = false;
    }
}

// Sets the optional floating-point value from a double value.
void pe_set_float(PeFloat* float_, double value) {
    assert(float_ != NULL);
    float_->has_value = true;
    float_->value     = (float)value;
    float_->_converr  = (value > FLT_MAX) || (value < -FLT_MAX);
}

