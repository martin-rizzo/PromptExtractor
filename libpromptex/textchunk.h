/*
| File    : textchunk.h
| Purpose : Handling of raw text extracted directly from the read files.
| Author  : Martin Rizzo | <martinrizzo@gmail.com>
| Date    : Oct 6, 2026
| Repo    : https://github.com/martin-rizzo/PromptExtractor
| License : MIT
|- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
|                              PromptExtractor
| A lightweight CLI tool to extract embedded prompts from AI-generated images.
\_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/
#ifndef PE__TEXTCHUNK_H__
#define PE__TEXTCHUNK_H__
#include <promptex/types.h>


/*=========================== TextChunk STRUCT ============================*/

/**
 * Represents a text extracted from a file, it can be chained to form a linked list.
 * 
 * Forms a singly linked list where each node holds an identifier, a name,
 * and a content. The `_next` pointer links to the subsequent chunk.
 */
struct PeTextChunk {
    const  char*        id;      ///< Constant uppercase C-style identifier (e.g., "USER_COMMENT")
    const  PeUTF8*      name;    ///< Chunk name as it appears in the file (e.g., "User Comment")
    const  PeUTF8*      content; ///< Text stored in the chunk (e.g., "This is the embedded text")
    struct PeTextChunk* _next;   ///< Internal pointer to the next text in the chain (NULL if last).
};
typedef struct PeTextChunk  PeTextChunk;


/**
 * Creates a new PeTextChunk structure with copies of the provided id, name, and content.
 *
 * This function is safe as it duplicates the input strings, so the caller
 * retains ownership of the original id, name, and content strings.
 * @relatesalso PeTextChunk
 *
 * @param id      The identifier string to copy.
 *                It should be the chunk identifier in C constant style (all uppercase and underscore).
 * @param name    The chunk name string to copy.
 *                It should be the name of the text chunk exactly as it appears in the file.
 * @param content The chunk content string to copy.
 * @param next    The next node in the chain.
 * @return
 *     A pointer to the newly created PeTextChunk structure.
 */
PeTextChunk* pe_create_textchunk(const char*   id,
                                 const PeUTF8* name,
                                 const PeUTF8* content,
                                 PeTextChunk*  next
                                 );


/**
 * Creates a new PeTextChunk that takes ownership of the provided id, name, and content.
 *
 * This function is slightly less safe than `pe_create_textchunk` because it
 * takes ownership of all strings passed as parameters. Therefore, both
 * should be allocated on the heap and should not be freed externally.
 * @relatesalso PeTextChunk
 *
 * @param id_in_heap      The chunk id string (ownership transferred).
 * @param name_in_heap    The chunk name string (ownership transferred).
 * @param content_in_heap The chunk content string (ownership transferred).
 * @param next            The next node in the chain.
 * @return
 *     A pointer to the newly attached PeTextChunk structure.
 */
PeTextChunk* pe_attach_textchunk(const char*   id_in_heap,
                                 const PeUTF8* name_in_heap,
                                 const PeUTF8* content_in_heap,
                                 PeTextChunk*  next
                                 );


/**
 * Compacts the fields of a PeTextChunk into a contiguous buffer.
 *
 * This function copies the `id`, `name`, and `content` fields from the given
 * `textchunk` into the provided `destbuf` sequentially. It also makes a second
 * copy of the `content` field. If `destbuf` is NULL, no data is copied, but
 * the function still returns the total number of bytes required to hold all
 * fields in the buffer.
 *
 * The output pointers (`out_id_ptr`, `out_name_ptr`, `out_content_ptr`, and
 * `out_content2_ptr`) are set to point to the respective copied strings within
 * the buffer. Each string is null-terminated.
 *
 * @param textchunk        Pointer to the PeTextChunk structure containing the data to compact.
 * @param destbuf          Destination buffer to store the compacted strings.
 *                         Can be NULL to calculate the required size without copying.
 * @param out_id_ptr       Output pointer that will be set to the copied `id` string in the buffer.
 * @param out_name_ptr     Output pointer that will be set to the copied `name` string in the buffer.
 * @param out_content_ptr  Output pointer that will be set to the first copy of the `content` string in the buffer.
 * @param out_content_size Output pointer that will be set to the size of the `content` string (including the null terminator).
 * @return
 *   The total number of bytes used in the buffer (including null terminators).
 */
size_t pe_compact_textchunk(const PeTextChunk* textchunk,
                            PeBYTE*            destbuf,
                            const char*        *out_id_ptr,
                            const PeUTF8*      *out_name_ptr,
                            const PeUTF8*      *out_content_ptr,
                            size_t             *out_content_size
                            );


/**
 * Destroys a single PeTextChunk node and frees its associated resources.
 *
 * This function frees the memory allocated for the strings, as well as
 * the `PeTextChunk` node itself.
 * @attention It does not traverse the chain, it only operates on the provided node.
 * @relatesalso PeTextChunk
 *
 * @param textchunk Pointer to the PeTextChunk node to destroy.
 *                  If `NULL`, the function returns `NULL` immediately.
 * @return
 *    A pointer to the next node in the chain or `NULL` if the provided
 *    node was the last in the chain (or if textchunk was NULL).
 */
const PeTextChunk* pe_destroy_textchunk(const PeTextChunk* textchunk
                                        );


/**
 * Destroys an entire chain of PeTextChunk nodes.
 *
 * Iterates through the linked list starting from the given node, destroying
 * each node sequentially. This function effectively frees all memory
 * associated with the chain.
 * @relatesalso PeTextChunk
 *
 * @param textchunk Pointer to the first node of the chain to destroy.
 *                  If `NULL`, the function does nothing.
 */
void pe_destroy_textchunk_chain(const PeTextChunk* textchunk
                                );



#endif /*PE__TEXTCHUNK_H__*/
