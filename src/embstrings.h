/*
| File    : embstrings.h
| Purpose : Manage embedded strings and named strings.
| Author  : Martin Rizzo | <martinrizzo@gmail.com>
| Date    : Oct 4, 2026
| Repo    : https://github.com/martin-rizzo/ReadPrompt
| License : MIT
|- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
|                                ReadPrompt
| A lightweight CLI tool to extract embedded prompts from AI-generated images.
\_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/
#ifndef RP__EMBSTRINGS_H__
#define RP__EMBSTRINGS_H__
#include "types.h"


enum RP_ENCODING {
    RP_ENCODING_UNKNOWN,
    RP_ENCODING_ASCII,
    RP_ENCODING_UTF8,
    RP_ENCODING_UTF16BE,
    RP_ENCODING_UTF16LE,
};
typedef enum RP_ENCODING RP_ENCODING;

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
 *       - RP_ENCODING_UNKNOWN
 *       - RP_ENCODING_ASCII
 *       - RP_ENCODING_UTF8
 *       - RP_ENCODING_UTF16BE
 *       - RP_ENCODING_UTF16LE
 */
RP_ENCODING rp_detect_encoding(const void *buffer, size_t buffer_size);


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
RpUTF8 *rp_strdup_utf8(const void* buffer, size_t bufsize, RP_ENCODING bufencoding);


#ifndef _POSIX_C_SOURCE
#define strdup(s) rp_strdup_utf8(s,0,RP_ENCODING_UTF8)
#endif


/*============================ NamedStr OBJECT ============================*/

/**
 * Represents a named string that can be chained to form a linked list.
 * @details A RpNamedStr object holds a `name` and a `value`, both of which are UTF-8 strings.
 */
struct RpNamedStr {
    const  RpUTF8*   name;  ///< The name of the string.
    const  RpUTF8*   value; ///< The string value itself.
    struct RpNamedStr* _next; ///< Pointer to the next string in the chain (NULL if last).
};
typedef struct RpNamedStr  RpNamedStr;


/**
 * Creates a new RpNamedStr structure with copies of the provided name and value.
 *
 * This function is safe as it duplicates the input strings, so the caller
 * retains ownership of the original name and value strings.
 * @relatesalso RpNamedStr
 *
 * @param name  The name string to copy.
 * @param value The value string to copy.
 * @param chain The next node in the chain.
 * @return
 *     A pointer to the newly created RpNamedStr structure.
 */
RpNamedStr* rp_create_namedstr(const RpUTF8* name,
                               const RpUTF8* value,
                               RpNamedStr*   next
                               );

/**
 * Creates a new RpNamedStr that takes ownership of the provided name and value.
 *
 * This function is slightly less safe than `rp_create_namedstr` because it
 * takes ownership of both strings passed as parameters. Therefore, both
 * should be allocated on the heap and should not be freed externally.
 * @relatesalso RpNamedStr
 *
 * @param name_in_heap  The name string (ownership transferred).
 * @param value_in_heap The value string (ownership transferred).
 * @param chain         The next node in the chain.
 * @return
 *     A pointer to the newly attached RpNamedStr structure.
 */
RpNamedStr* rp_attach_namedstr(const RpUTF8* name_in_heap,
                               const RpUTF8* value_in_heap,
                               RpNamedStr* next
                               );

/**
 * Destroys a single RpNamedStr node and frees its associated resources.
 *
 * This function frees the memory allocated for the `name` and `value`
 * strings, as well as the `RpNamedStr` node itself.
 * @attention It does not traverse the chain, it only operates on the provided node.
 * @relatesalso RpNamedStr
 *
 * @param namedstr Pointer to the RpNamedStr node to destroy.
 *                 If `NULL` the function returns `NULL` immediately.
 * @return
 *    A pointer to the next node in the chain or `NULL` if the provided
 *    node was the last in the chain (or if namedstr was NULL).
 */
const RpNamedStr* rp_destroy_namedstr(const RpNamedStr* namedstr
                                      );

/**
 * Destroys an entire chain of RpNamedStr nodes.
 *
 * Iterates through the linked list starting from the given node, destroying
 * each node sequentially. This function effectively frees all memory
 * associated with the chain.
 * @relatesalso RpNamedStr
 *
 * @param namedstr Pointer to the first node of the chain to destroy.
 *                 If `NULL`, the function does nothing.
 */
void rp_destroy_namedstr_chain(const RpNamedStr* namedstr
                               );


/*========================== RpEmbStrings STRUCT ==========================*/

/**
 * Represents a collection of RpNamedStr items.
 */
struct RpEmbStrings {
    RpNamedStr** items;  ///< Array of pointers to RpNamedStr items.
    int          count;  ///< Number of items in the array.
    RpNamedStr*  _chain; ///< Pointer to the head of the linked chain of RpNamedStr nodes.
};
typedef struct RpEmbStrings RpEmbStrings;


/**
 * Creates a new RpEmbStrings structure taking ownership of the string chain.
 *
 * This function takes ownership of the entire linked list (chain) passed as
 * the `chain_in_heap` parameter. The RpEmbStrings structure will manage the
 * lifecycle of these nodes, and they will be freed when the RpEmbStrings
 * structure is destroyed via `rp_destroy_embstrings`.
 * @relatesalso RpEmbStrings
 *
 * @param chain_in_heap Pointer to the first node of the linked chain.
 *                      The entire chain is owned by the returned RpEmbStrings
 *                      and should not be freed externally.
 * @return
 *    A pointer to the newly created RpEmbStrings structure.
 *    The caller should use `rp_destroy_embstrings` to free this structure
 *    and its associated resources.
 */
RpEmbStrings* rp_attach_embstrings(RpNamedStr* chain_in_heap);



/**
 * Destroys an RpEmbStrings structure and frees its associated resources.
 * @relatesalso RpEmbStrings
 *
 * @param ptr Pointer to the RpEmbStrings structure to destroy.
 *            If `NULL`, the function does nothing.
 */
void rp_destroy_embstrings(const RpEmbStrings* ptr);


#endif /*RP__EMBSTRINGS_H__*/
