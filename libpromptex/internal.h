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
#include <stdlib.h>
#include <promptex/types.h>

/**
 * Suppresses "unused variable" warnings for function arguments.
 *
 * Explicitly indicate that it is intentionally unused, typically
 * because the function is still under development.
 */
#define unused(x) ((void)(&x))


// Representa un tamaño nulo de imagen, generalmente retornado cuando el calculo da error.
extern const PeImageSize _peEmptyImageSize;

// ???
#define EMPTY_IMAGE_SIZE _peEmptyImageSize;


/*============================= TEXT HELPERS ==============================*/

/** Returns `true` if both UTF8 strings are equal. */
bool PeUTF8_equals(const PeUTF8* str1, const PeUTF8* str2);


/** ??? */
bool PeUTF8_startswith(const PeUTF8* str, const PeUTF8* prefix);


/** Returns true if the character is considered whitespace. */
static inline bool pe_isspace(PeUTF8 c) {
    return ('\x09' <= c && c <= ' ');
}

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


/*============================= TEXT ENCODING =============================*/

enum PE_ENCODING {
    PE_ENCODING_UNKNOWN,
    PE_ENCODING_ASCII,
    PE_ENCODING_UTF8,
    PE_ENCODING_UTF16BE,
    PE_ENCODING_UTF16LE,
};
typedef enum PE_ENCODING PE_ENCODING;


/**
 * Detects the encoding of a string.
 *
 * This function analyzes a buffer to determine the encoding of a string.
 * It counts the null characters in even and odd positions and deduces if
 * the string is ASCII, UTF-8, UTF-16BE, or UTF-16LE encoded.
 *
 * @param buffer       Pointer to the buffer containing the string to be analyzed.
 * @param buffer_size  Size of the buffer in bytes.
 * @return
 *     The encoding of the string. Possible values are:
 *       - PE_ENCODING_UNKNOWN
 *       - PE_ENCODING_ASCII
 *       - PE_ENCODING_UTF8
 *       - PE_ENCODING_UTF16BE
 *       - PE_ENCODING_UTF16LE
 */
PE_ENCODING pe_detect_encoding(const void *buffer, size_t buffer_size);


/**
 * Converts a source character buffer into a newly allocated UTF-8 string.
 *
 * Decodes the input buffer according to the specified encoding and returns
 * a newly allocated, null-terminated UTF-8 string. The caller is responsible
 * for freeing the returned pointer.
 *
 * @param buffer  Pointer to the source buffer. If NULL, returns NULL.
 * @param bufsize Size of the source buffer in bytes.
 *                If 0, the function automatically determines the size.
 * @param bufencoding The encoding format of the source buffer. Supported
 *                    encodings include ASCII, UTF-8, UTF-16BE, UTF-16LE,
 *                    and UNKNOWN.
 * @return A pointer to the newly allocated UTF-8 string, or NULL if allocation
 *         fails. For unknown encodings, returns a newly allocated empty string.
 */
PeUTF8* pe_strdup_utf8(const void* buffer, size_t bufsize, PE_ENCODING bufencoding);


/**
 * Converts an ASCII string buffer into a newly allocated uppercase string.
 *
 * Transforms the input buffer into a new string where all alphabetic characters
 * are converted to uppercase, digits are preserved, and any other characters
 * are replaced with an underscore ('_'). The resulting string mimics the
 * naming convention of C99 constants (e.g., "User Comment" becomes "USER_COMMENT").
 *
 * The function supports only ASCII encoding. If the buffer encoding is not
 * ASCII, it returns a newly allocated empty string.
 *
 * @param buffer      Pointer to the source buffer. If NULL, returns NULL.
 * @param bufsize     Size of the source buffer in bytes. If 0, the function
 *                    automatically determines the size using `strlen`.
 * @param bufencoding The encoding format of the source buffer. Only
 *                    `PE_ENCODING_ASCII` is fully supported; other encodings
 *                    result in an empty string.
 * @return A pointer to the newly allocated uppercase string, or NULL if
 *         allocation fails. For unsupported encodings, returns a newly
 *         allocated empty string.
 */
char* pe_strdup_UPPER_ASCII(const void* buffer, size_t bufsize, PE_ENCODING bufencoding);



// Convenience function for duplicating UTF-8 and ASCII strings.
// Can be used as a direct replacement for strdup.
static inline PeUTF8* pe_strdup(const PeUTF8* str) {
    return pe_strdup_utf8(str, 0, PE_ENCODING_UTF8);
}


#endif /*PE__INTERNAL_H__*/
