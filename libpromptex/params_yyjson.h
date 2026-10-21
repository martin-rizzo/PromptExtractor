/*
| File    : params_yyjson.h
| Purpose : Set parameters from yyjson values.
| Author  : Martin Rizzo | <martinrizzo@gmail.com>
| Date    : Oct 16, 2026
| Repo    : https://github.com/martin-rizzo/PromptExtractor
| License : MIT
|- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
|                              PromptExtractor
| A lightweight CLI tool to extract embedded prompts from AI-generated images.
\_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/
#ifndef PE__PARAMS_YYJSON_H__
#define PE__PARAMS_YYJSON_H__
#include <promptex/types.h>
#include "yyjson.h"


/**
 * Sets the optional long integer value from a yyjson_val object.
 *
 * Extracts the value associated with `key` from the provided yyjson_val
 * `object` and updates the target `PeLong` structure: `has_value` is set
 * to `true`, `value` is updated with the extracted result, and `_converr`
 * indicates whether a conversion error occurred.
 *
 * When the update is successful but `_converr` is `true`, it means the
 * value stored in `PeLong` may not be exact due to conversion limits
 * (e.g., overflow).
 *
 * If the key is not found, the structure remains unmodified.
 *
 * @param long_  Pointer to the `PeLong` structure to be updated.
 * @param object Pointer to the `yyjson_val` object containing the JSON data.
 * @param key    The key of the variable in the JSON object.
 */
void pe_set_yylong(PeLong* long_, const yyjson_val *object, const PeUTF8* key);



#endif /*PE__PARAMS_YYJSON_H__*/
