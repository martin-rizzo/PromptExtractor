/*
| File    : textchunk.c
| Purpose : Handling of raw text extracted directly from the read files.
| Author  : Martin Rizzo | <martinrizzo@gmail.com>
| Date    : Oct 6, 2026
| Repo    : https://github.com/martin-rizzo/PromptExtractor
| License : MIT
|- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
|                              PromptExtractor
| A lightweight CLI tool to extract embedded prompts from AI-generated images.
\_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/
#include <stdlib.h>
#include <string.h>
#include "internal.h"
#include "textchunk.h"

/**
 * Represents the CPU preferred alignment in bytes.
 */
#define CPU_ALIGNMENT sizeof(void*)

/**
 * Aligns a value up to the nearest multiple of the specified alignment.
 * @param val   The value to align.
 * @param align The alignment value (must be a power of two).
 * @return
 *     The aligned value.
 */
#define ALIGN_UP_n(val, align) (((val) + (align) - 1) & ~((align) - 1))

/**
 * Aligns a value up to the nearest multiple of the CPU preferred alignment.
 * @param val The value to align.
 * @return
 *     The aligned value.
 */
#define ALIGN_UP(val) ALIGN_UP_n(val, CPU_ALIGNMENT)



// Creates a new PeTextChunk structure with copies of the provided id, name, and content.
PeTextChunk* pe_create_textchunk(const char*   id,
                                 const PeUTF8* name,
                                 const PeUTF8* content,
                                 PeTextChunk*  chain
){
    return pe_attach_textchunk(pe_strdup(id), pe_strdup(name), pe_strdup(content), chain);
}


// Creates a new PeTextChunk that takes ownership of the provided id, name, and content.
PeTextChunk* pe_attach_textchunk(const char*   id_in_heap,
                                 const PeUTF8* name_in_heap,
                                 const PeUTF8* content_in_heap,
                                 PeTextChunk*  chain
){
    PeTextChunk* textchunk = malloc(sizeof(PeTextChunk));
    textchunk->id      = id_in_heap;
    textchunk->name    = name_in_heap;
    textchunk->content = content_in_heap;
    textchunk->_next   = chain;
    return textchunk;
}


// Compacts the fields of a PeTextChunk into a contiguous buffer.
size_t pe_compact_textchunk(const PeTextChunk* textchunk,
                            PeBYTE*            destbuf,
                            const char*        *out_id_ptr,
                            const PeUTF8*      *out_name_ptr,
                            const PeUTF8*      *out_content_ptr,
                            size_t             *out_content_size
){
    const size_t id_size      = strlen(textchunk->id     ) + 1;
    const size_t name_size    = strlen(textchunk->name   ) + 1;
    const size_t content_size = strlen(textchunk->content) + 1;

    // offset tracks the current position in the buffer
    // and accumulates the total bytes copied.
    size_t offset = 0;

    // copy `.id` and advance the offset
    if (destbuf) { memcpy(&destbuf[offset], textchunk->id, id_size); }
    if (out_id_ptr) { (*out_id_ptr) = destbuf ? (const char*)&destbuf[offset] : NULL; }
    offset = ALIGN_UP(offset + id_size);

    // copy `.name` and advance the offset
    if (destbuf) { memcpy(&destbuf[offset], textchunk->name, name_size); }
    if (out_name_ptr) { (*out_name_ptr) = destbuf ? (const PeUTF8*)&destbuf[offset] : NULL; }
    offset = ALIGN_UP(offset + name_size);

    // copy `.content` and advance the offset
    if (destbuf) { memcpy(&destbuf[offset], textchunk->content, content_size); }
    if (out_content_ptr)  { (*out_content_ptr)  = destbuf ? (const PeUTF8*)&destbuf[offset] : NULL; }
    if (out_content_size) { (*out_content_size) = content_size; }
    offset = ALIGN_UP(offset + content_size);
   
    return offset;
}


// Destroys a single PeTextChunk node and frees its associated resources.
const PeTextChunk* pe_destroy_namedstr(const PeTextChunk* textchunk
){
    if( !textchunk ) { return NULL; }
    PeTextChunk* next_in_chain = textchunk->_next;
    free( (void*)textchunk->id      );
    free( (void*)textchunk->name    );
    free( (void*)textchunk->content );
    free( (void*)textchunk );
    return next_in_chain;
}


// Destroys an entire chain of PeTextChunk nodes.
void pe_destroy_textchunk_chain(const PeTextChunk* textchunk
){
    while (textchunk!=NULL) {
        textchunk = pe_destroy_namedstr(textchunk);
    }
}
