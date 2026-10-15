/*
| File    : types.h
| Purpose : Simple types used in the project.
| Author  : Martin Rizzo | <martinrizzo@gmail.com>
| Date    : Oct 2, 2026
| Repo    : https://github.com/martin-rizzo/PromptExtractor
| License : MIT
|- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
|                              PromptExtractor
| A lightweight CLI tool to extract embedded prompts from AI-generated images.
\_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/
#ifndef PE__TYPES_H__
#define PE__TYPES_H__
#include <stdlib.h>


// Suppresses unused variable warnings.
#define PE_UNUSED(x) ((void)&x);

// Convenience macro for duplicating UTF-8 and ASCII strings.
// Can be used as a direct replacement for strdup.
#define pe_strdup(str) pe_strdup_utf8(str, 0, PE_ENCODING_UTF8)

/**
 * Maximum number of text chunks that can be analyzed from an image file.
 *
 * Defines the upper limit for the number of text contents or chunks
 * that the extractor can process from a single image file (e.g., JPEG, PNG).
 */
#define PE_MAX_TEXTCHUNK_COUNT 4

/**
 * Maximum size in bytes for a single text chunk.
 *
 * Acts as a safety limit to prevent memory allocation issues.
 * The value is 6 MB (6 * 1024 * 1024 bytes).
 */
#define PE_MAX_TEXTCHUNK_SIZE (6*1024*1024)

/**
 * Represents a basic UTF-8 character unit.
 */
typedef char PeUTF8;

/**
 * Represents a byte with a value range from 0 to 255.
 */
typedef unsigned char PeBYTE;


/*============================ STATE REPORTING ============================*/

enum PEREPORT_TYPE {
    PEREPORT_INFO,     ///< The reported message is an info message
    PEREPORT_WARNING,  ///< The reported message is a warning
    PEREPORT_ERROR     ///< The reported message is an execution error
};
typedef enum PEREPORT_TYPE PEREPORT_TYPE;
typedef void (PeReportFn)(PEREPORT_TYPE type, const PeUTF8* message);


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


#endif /*PE__TYPES_H__*/
