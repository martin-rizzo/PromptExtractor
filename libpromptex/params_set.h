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
#ifndef PE__PARAMS_DEFS_H__
#define PE__PARAMS_DEFS_H__
#include <stdlib.h>
#include <stdbool.h>
#include <promptex/types.h>


/**
 * Sets the optional string value.
 *
 * Assigns the provided `strvalue` to the target `PeString` structure.
 * If `strvalue` is NULL, the structure is reset to an empty state.
 *
 * @param string   Pointer to the `PeString` structure to be updated.
 * @param strvalue The string value to be assigned. Can be NULL to unset
 *                 the structure.
 */
static inline
void pe_set_string(PeString* string, const PeUTF8* strvalue) {
    string->has_value = strvalue ? true     : false;
    string->value     = strvalue ? strvalue : NULL;
}


/**
 * Sets the optional long integer value from a string representation.
 *
 * Parses the provided string `strvalue` into a long integer and updates
 * the target `PeLong` structure, `has_value` is set to true, `value` is
 * updated with the parsed result, and `_converr` indicates whether a
 * conversion error occurred.
 * 
 * If `strvalue` is NULL, the structure is reset to an empty state.
 *
 * @param long_    Pointer to the `PeLong` structure to be updated.
 * @param strvalue The string representation of the long integer (e.g., "42").
 *                 Can be NULL to unset the structure.
 */
void pe_set_strlong(PeLong* long_, const PeUTF8* strvalue);


/**
 * Sets the optional long integer value from a double value.
 *
 * Converts the provided double `value` into a long integer and updates
 * the target `PeLong` structure, `has_value` is set to true, `value` is
 * updated with the converted result, and `_converr` indicates whether a
 * conversion error occurred.
 *
 * @param long_ Pointer to the `PeLong` structure to be updated.
 * @param value The double value to be converted to a long integer.
 */
void pe_set_long(PeLong* long_, double value);


/**
 * Sets the optional double precision floating-point value from a string representation.
 *
 * Parses the provided string `strvalue` into a double precision floating-point
 * number and updates the target `PeDouble` structure, `has_value` is set to
 * true, `value` is* updated with the parsed result, and `_converr` indicates
 * whether a conversion error occurred.
 * 
 * If `strvalue` is NULL, the structure is reset to an empty state.
 *
 * @param double_  Pointer to the `PeDouble` structure to be updated.
 * @param strvalue The string representation of the double-precision floating-point number
 *                 (e.g., "3.14159"). Can be NULL to unset the structure.
 */
void pe_set_strdouble(PeDouble* double_, const PeUTF8* strvalue);


/**
 * Sets the optional double precision floating-point value from a double value.
 *
 * Assigns the provided double `value` to the target `PeDouble` structure,
 * `has_value` is set to true, `value` is updated with the assigned result,
 * and `_converr` is set to false.
 *
 * @param double_ Pointer to the `PeDouble` structure to be updated.
 * @param value   The double-precision floating-point value to be assigned.
 */
void pe_set_double(PeDouble* double_, double value);


/**
 * Sets the optional floating-point value from a string representation.
 *
 * Parses the provided string `strvalue` into a floating-point number and
 * updates the target `PeFloat` structure, `has_value` is set to true, `value`
 * is updated with the parsed result, and `_converr` indicates whether a
 * conversion error occurred.
 * 
 * If `strvalue` is NULL, the structure is reset to an empty state.
 *
 * @param float_   Pointer to the `PeFloat` structure to be updated.
 * @param strvalue The string representation of the floating-point number
 *                 (e.g., "3.14"). Can be NULL to unset the structure.
 */
void pe_set_strfloat(PeFloat* float_, const PeUTF8* strvalue);


/**
 * Sets the optional floating-point value from a double value.
 *
 * Converts the provided double `value` into a single precision floating-point
 * number and updates the target `PeFloat` structure, `has_value` is set to
 * true, `value` is updated with the converted result, and `_converr` indicates
 * whether a conversion error occurred (overflow/underflow).
 *
 * @param float_ Pointer to the `PeFloat` structure to be updated.
 * @param value  The double value to be converted to a single-precision floating-point number.
 */
void pe_set_float(PeFloat* float_, double value);



#endif /*PE__PARAMS_DEFS_H__*/
