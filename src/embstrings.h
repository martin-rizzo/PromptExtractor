/*
| File    : embstrings.h
| Purpose : Manage embedded strings and named strings.
| Author  : Martin Rizzo | <martinrizzo@gmail.com>
| Date    : Oct 4, 2026
| Repo    : https://github.com/martin-rizzo/PromptExtractor
| License : MIT
|- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
|                              PromptExtractor
| A lightweight CLI tool to extract embedded prompts from AI-generated images.
\_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/
#ifndef PE__EMBSTRINGS_H__
#define PE__EMBSTRINGS_H__
#include "types.h"


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
PeUTF8 *pe_strdup_utf8(const void* buffer, size_t bufsize, PE_ENCODING bufencoding);


#ifndef _POSIX_C_SOURCE
#define strdup(s) pe_strdup_utf8(s,0,PE_ENCODING_UTF8)
#endif


/*============================ NamedStr OBJECT ============================*/

/**
 * Represents a named string that can be chained to form a linked list.
 * @details A PeNamedStr object holds a `name` and a `value`, both of which are UTF-8 strings.
 */
struct PeNamedStr {
    const  PeUTF8*   name;  ///< The name of the string.
    const  PeUTF8*   value; ///< The string value itself.
    struct PeNamedStr* _next; ///< Pointer to the next string in the chain (NULL if last).
};
typedef struct PeNamedStr  PeNamedStr;


/**
 * Creates a new PeNamedStr structure with copies of the provided name and value.
 *
 * This function is safe as it duplicates the input strings, so the caller
 * retains ownership of the original name and value strings.
 * @relatesalso PeNamedStr
 *
 * @param name  The name string to copy.
 * @param value The value string to copy.
 * @param chain The next node in the chain.
 * @return
 *     A pointer to the newly created PeNamedStr structure.
 */
PeNamedStr* pe_create_namedstr(const PeUTF8* name,
                               const PeUTF8* value,
                               PeNamedStr*   next
                               );

/**
 * Creates a new PeNamedStr that takes ownership of the provided name and value.
 *
 * This function is slightly less safe than `pe_create_namedstr` because it
 * takes ownership of both strings passed as parameters. Therefore, both
 * should be allocated on the heap and should not be freed externally.
 * @relatesalso PeNamedStr
 *
 * @param name_in_heap  The name string (ownership transferred).
 * @param value_in_heap The value string (ownership transferred).
 * @param chain         The next node in the chain.
 * @return
 *     A pointer to the newly attached PeNamedStr structure.
 */
PeNamedStr* pe_attach_namedstr(const PeUTF8* name_in_heap,
                               const PeUTF8* value_in_heap,
                               PeNamedStr* next
                               );

/**
 * Destroys a single PeNamedStr node and frees its associated resources.
 *
 * This function frees the memory allocated for the `name` and `value`
 * strings, as well as the `PeNamedStr` node itself.
 * @attention It does not traverse the chain, it only operates on the provided node.
 * @relatesalso PeNamedStr
 *
 * @param namedstr Pointer to the PeNamedStr node to destroy.
 *                 If `NULL` the function returns `NULL` immediately.
 * @return
 *    A pointer to the next node in the chain or `NULL` if the provided
 *    node was the last in the chain (or if namedstr was NULL).
 */
const PeNamedStr* pe_destroy_namedstr(const PeNamedStr* namedstr
                                      );

/**
 * Destroys an entire chain of PeNamedStr nodes.
 *
 * Iterates through the linked list starting from the given node, destroying
 * each node sequentially. This function effectively frees all memory
 * associated with the chain.
 * @relatesalso PeNamedStr
 *
 * @param namedstr Pointer to the first node of the chain to destroy.
 *                 If `NULL`, the function does nothing.
 */
void pe_destroy_namedstr_chain(const PeNamedStr* namedstr
                               );


/*========================== PeEmbStrings STRUCT ==========================*/

/**
 * Represents a collection of PeNamedStr items.
 */
struct PeEmbStrings {
    PeNamedStr** items;  ///< Array of pointers to PeNamedStr items.
    int          count;  ///< Number of items in the array.
    PeNamedStr*  _chain; ///< Pointer to the head of the linked chain of PeNamedStr nodes.
};
typedef struct PeEmbStrings PeEmbStrings;


/**
 * Creates a new PeEmbStrings structure taking ownership of the string chain.
 *
 * This function takes ownership of the entire linked list (chain) passed as
 * the `chain_in_heap` parameter. The PeEmbStrings structure will manage the
 * lifecycle of these nodes, and they will be freed when the PeEmbStrings
 * structure is destroyed via `pe_destroy_embstrings`.
 * @relatesalso PeEmbStrings
 *
 * @param chain_in_heap Pointer to the first node of the linked chain.
 *                      The entire chain is owned by the returned PeEmbStrings
 *                      and should not be freed externally.
 * @return
 *    A pointer to the newly created PeEmbStrings structure.
 *    The caller should use `pe_destroy_embstrings` to free this structure
 *    and its associated resources.
 */
PeEmbStrings* pe_attach_embstrings(PeNamedStr* chain_in_heap);



/**
 * Destroys a PeEmbStrings structure and frees its associated resources.
 * @relatesalso PeEmbStrings
 *
 * @param ptr Pointer to the PeEmbStrings structure to destroy.
 *            If `NULL`, the function does nothing.
 */
void pe_destroy_embstrings(const PeEmbStrings* ptr);


#endif /*PE__EMBSTRINGS_H__*/
