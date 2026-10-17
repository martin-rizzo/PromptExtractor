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
#ifndef PE__PARAMS_DEFS_H__
#define PE__PARAMS_DEFS_H__
#include <stdbool.h>
#include "types.h"

/**
 * An optional long integer value.
 */
struct PeLong {
    bool          has_value; ///< Flag indicating if the integer value is present
    long          value;     ///< The long integer value
    bool          _converr;  ///< Flag indicating if hubo algun error durante la conversion (str -> int)
    const PeUTF8* _strvalue; ///< String representation that originated the value (optional, may be NULL)
};
typedef struct PeLong PeLong;

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
void pe_set_long(PeLong* long_, const PeUTF8* strvalue);


/**
 * An optional floating-point value.
 */
struct PeFloat {
    bool          has_value; ///< Flag indicating if the float value is present
    float         value;     ///< The floating-point value
    bool          _converr;  ///< Flag indicating if hubo algun error durante la conversion (str -> float)
    const PeUTF8* _strvalue; ///< String representation that originated the value (optional, may be NULL)
};
typedef struct PeFloat PeFloat;

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
void pe_set_float(PeFloat* float_, const PeUTF8* strvalue);


/**
 * An optional string value.
 */
struct PeString {
    bool          has_value; ///< Flag indicating if the string value is present
    const PeUTF8* value;     ///< The string value
};
typedef struct PeString PeString;
static inline
void pe_set_string(PeString* string, const PeUTF8* strvalue) {
    string->has_value = strvalue ? true     : false;
    string->value     = strvalue ? strvalue : NULL;
}


/**
 * Represents a text variable extracted directly from an image file (JPEG, PNG, etc.).
 */
struct PeTextContent {
    const char*   id;           ///< Identifier of the variable in uppercase format (e.g., "USER_COMMENT")
    const PeUTF8* name;         ///< Name of the variable as it appears in the file (e.g., "User Comment")
    const PeUTF8* content;      ///< The text content of the variable (e.g., "This is the embedded text")
    const size_t  content_size; ///< Size in bytes of the `content` buffer
};
typedef struct PeTextContent PeTextContent;


/**
 * Information related to the original file from which parameters were extracted.
 */
struct PeFileInfo {
    const PeUTF8* name;                          ///< File name including extension
    const PeUTF8* path;                          ///< Full path of the file
    PeTextContent texts[PE_MAX_TEXTCHUNK_COUNT]; ///< Array of found text contents (max number of elements determined by `PE_MAX_TEXTCHUNK_COUNT`)
    int           texts_count;                   ///< Number of text contents found in the file
};
typedef struct PeFileInfo PeFileInfo;


/**
 * Internal storage buffer for parameter strings and user data.
 *
 * This structure manages the underlying memory buffer for all string data
 * referenced by the `PeParams` structure (e.g., `prompt`, `negative`, etc).
 * The `ptr` member points to a contiguous memory region where these strings
 * are stored; the size of this buffer is always sufficient to store any text
 * contents from `peparams.fileinfo.texts[n].content`. Likewise, the `size`
 * member indicates the total size in bytes.
 *
 * It also provides two user data pointers (`user1`, `user2`) that can be
 * allocated by external code (e.g., `prompt_fmt` implementations) to store
 * custom data. These pointers are automatically freed when the associated
 * `PeParams` structure is destroyed.
 */
struct PeStorage {
    void*  ptr;    ///< Pointer to the internal data buffer.
    size_t size;   ///< Total size of the internal data buffer in bytes.
    void*  user1;  ///< Optional user data pointer 1, automatically freed upon `PeParams` destruction.
    void*  user2;  ///< Optional user data pointer 2, automatically freed upon `PeParams` destruction.
};
typedef struct PeStorage PeStorage;


#endif /*PE__PARAMS_DEFS_H__*/
