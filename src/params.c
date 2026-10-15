#include "params.h"
#include "image_fmt/png.h"
#include "image_fmt/jpeg.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * Extracts the basename from a given file path.
 *
 * This function analyzes the provided path string and returns a pointer
 * to the last component of the path (i.e., the filename), regardless of
 * whether the path uses forward slashes ('/') or backslashes ('\').
 * It works correctly for both Windows and Linux paths.
 *
 * @param path The input file path string.
 * @return
 *     A pointer to the start of the basename within the path string.
 *     If the path is NULL, returns NULL.
 */
static const PeUTF8*
_get_basename(const PeUTF8 *path) {
    if (!path) { return NULL; }
    const char *last_backslash = strrchr(path, '\\');
    const char *last_slash     = strrchr(path, '/');
    const char *basename_start = NULL;

    // use the slash that appears later in the string
    if (last_backslash > last_slash) { basename_start = last_backslash; }
    else                             { basename_start = last_slash;     }
    return (basename_start != NULL) ? (basename_start + 1) : path;
}


/*===========================================================================
|  ////////////////////////// PUBLIC FUNCTIONS ///////////////////////////  | 
===========================================================================*/


// Creates a `PeParams` by extracting generation parameters from a image file (e.g., JPEG, PNG).
const PeParams* pe_params_from_path(const PeUTF8* fullpath,
                                    PeReportFn    report
){
    FILE* stream = fopen(fullpath, "rb");
    if (!stream) {
        if (report) report(PEREPORT_ERROR, "Failed to open file");
        return NULL;
    }
    const PeParams* params = pe_params_from_stream(stream, fullpath, report);
    fclose(stream);
    return params;
}


// Creates a `PeParams` by extracting generation parameters from the provided stream.
const PeParams* pe_params_from_stream(FILE*         stream,
                                      const PeUTF8* stream_name,
                                      PeReportFn    report
){
    const PeParams* params = NULL;

    PeTextChunk* chain  = NULL;
    if (chain == NULL) { chain = pe_textchunk_chain_from_png (stream,report); }
    if (chain == NULL) { chain = pe_textchunk_chain_from_jpeg(stream,report); }
    params = pe_params_from_chain(chain, stream_name, report);
    pe_destroy_textchunk_chain(chain);

    return params;
}


// Creates a `PeParams` by extracting generation parameters from a chain of text chunks.
const PeParams* pe_params_from_chain(PeTextChunk*  chain,
                                     const PeUTF8* stream_name,
                                     PeReportFn    report
){
    size_t content_size;
    PeTextChunk* textchunk;
    size_t stream_name_size = strlen(stream_name) + 1;

    //-- FIRST PASS: calculate total size needed for the buffer -------------
    int i = 0;
    size_t textchunks_size  = 0;
    size_t max_content_size = 0;
    textchunk=chain; while (textchunk && i < PE_MAX_TEXTCHUNK_COUNT)
    {
        textchunks_size += pe_compact_textchunk(
            textchunk,
            NULL,
            NULL, NULL, NULL, &content_size
        );
        if (max_content_size<content_size) { max_content_size = content_size; }
        textchunk = textchunk->_next;
        ++i;
    }

    const size_t storage_size     = max_content_size;
    const size_t total_parms_size = sizeof(PeParams) + storage_size + stream_name_size + textchunks_size;

    // allocate PeParams with the calculated buffer size appended
    PeParams* params = calloc(1, total_parms_size);
    if (!params) {
        if (report) report(PEREPORT_ERROR, "Memory allocation failed for `params`.");
        return NULL;
    }

    //-- SECOND PASS: copy data into the buffer -----------------------------
    size_t  offset = 0;
    PeBYTE* buffer = (PeBYTE*)params + sizeof(PeParams);

    // dejar un espacio para `_storage`
    params->_storage.ptr  = (void*)&buffer[offset];
    params->_storage.size = storage_size;
    offset += storage_size;

    // copy `stream_name`
    memcpy(buffer, stream_name, stream_name_size);
    params->fileinfo.path = (const PeUTF8*)&buffer[offset];
    params->fileinfo.name = _get_basename( params->fileinfo.path );
    offset += stream_name_size;

    // copy `textchunks`
    i = 0;
    textchunk=chain; while (textchunk && i < PE_MAX_TEXTCHUNK_COUNT)
    {
        offset += pe_compact_textchunk(
            textchunk,
            &buffer[offset],
            &params->fileinfo.texts[i].id,
            &params->fileinfo.texts[i].name,
            &params->fileinfo.texts[i].content,
            (size_t*)(&params->fileinfo.texts[i].content_size)
        );
        if (params->fileinfo.texts[i].content_size > max_content_size) {
            max_content_size = params->fileinfo.texts[i].content_size;
        }
        textchunk = textchunk->_next;
        ++i;
    }
    params->fileinfo.texts_count = i;

    return params;
}


// Destroys and frees the memory allocated for a PeParams structure.
void pe_destroy_params(const PeParams* params
){
    if (params) { free((void*)params);  }
}
