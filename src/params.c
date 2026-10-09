#include "params.h"
#include "image_fmt/png.h"
#include "image_fmt/jpeg.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
    int i;
    size_t offset;
    PeBYTE* buffer;
    PeTextChunk* textchunk;
    size_t stream_name_size = strlen(stream_name) + 1;

    // FIRST PASS: calculate total size needed for the buffer
    offset = 0;
    buffer = NULL;

    offset += stream_name_size;
    i = 0;
    textchunk=chain; while (textchunk && i < PE_MAX_TEXT_CONTENTS)
    {
        offset += pe_compact_textchunk(
            textchunk,
            buffer,
            NULL, NULL, NULL, NULL
        );
        textchunk = textchunk->_next;
        ++i;
    }

    // Allocate PeParams with the calculated buffer size appended
    PeParams* params = calloc(1, sizeof(PeParams) + offset);
    if (!params) {
        if (report) report(PEREPORT_ERROR, "Memory allocation failed for `params`.");
        return NULL;
    }

    // SECOND PASS: copy data into the buffer
    offset = 0;
    buffer = (PeBYTE*)params + sizeof(PeParams);

    // copy `stream_name`
    memcpy(buffer, stream_name, stream_name_size);
    params->file.path = (const PeUTF8*)&buffer[offset];
    params->file.name = (const PeUTF8*)&buffer[offset];
    offset += stream_name_size;

    // copy `textchunks`
    i = 0;
    textchunk=chain; while (textchunk && i < PE_MAX_TEXT_CONTENTS)
    {
        offset += pe_compact_textchunk(
            textchunk,
            &buffer[offset],
            &params->file.texts[i].id,
            &params->file.texts[i].name,
            &params->file.texts[i].value,
            &params->file.texts[i]._scratchpad
        );
        textchunk = textchunk->_next;
        ++i;
    }
    params->file.texts_count = i;

    return params;
}


void pe_destroy_params(const PeParams* params
){
    if (params) { free((void*)params);  }
}
