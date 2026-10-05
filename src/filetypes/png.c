/*
| File    : filetypes/png.c
| Purpose : Read a PNG file and extract embedded text chunks.
| Author  : Martin Rizzo | <martinrizzo@gmail.com>
| Date    : Oct 4, 2026
| Repo    : https://github.com/martin-rizzo/ReadPrompt
| License : MIT
|- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
|                                ReadPrompt
| A lightweight CLI tool to extract embedded prompts from AI-generated images.
\_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "types.h"
#include "embstrings.h"
#include "png.h"
#define SAFE_BYTES 4  ///< bytes to pad to '\0' for security in strings

/*=========================== HELPER FUNCTIONS ============================*/


/**
 * Loads a text chunk from a file.
 * @param chunk_size      The number of bytes to read for the text chunk.
 * @param min_allocation  The minimum number of bytes to allocate.
 * @param max_allocation  The maximum number of bytes to allocate.
 * @param file            Pointer to a FILE object from which to read the chunk data.
 * @return
 *     A pointer to the loaded text chunk, or NULL on error.
 */
static RpUTF8*
load_text_chunk(size_t chunk_size, size_t min_allocation, size_t max_allocation, FILE* file) {

    // by default, the number of bytes to allocate and the number of
    // bytes to request are equal to the size of the chunk to be loaded
    size_t bytes_to_request = chunk_size;
    size_t bytes_to_alloc   = chunk_size;

    // `bytes_to_alloc` value is adjusted according to the user-supplied min/max
    // (unless those min/max limits are 0)
    if (min_allocation && bytes_to_alloc < min_allocation) { bytes_to_alloc = min_allocation; }
    if (max_allocation && bytes_to_alloc > max_allocation) { bytes_to_alloc = max_allocation; }
    
    // adjust the number of bytes to request if it exceeds the allocated amount
    if (bytes_to_request > bytes_to_alloc) {
        bytes_to_request = bytes_to_alloc;
    }

    RpUTF8* text_chunk = (RpUTF8*)calloc(1, bytes_to_alloc + SAFE_BYTES);
    size_t  bytes_read = fread(text_chunk, sizeof(RpBYTE), bytes_to_request, file);

    // if for some reason not all requested bytes could be read,
    // return NULL (error!)
    if (bytes_read != bytes_to_request) {
        free(text_chunk);
        return NULL;
    }

    // if fewer bytes than the chunk size were read,
    // advance the pointer to compensate for the unread bytes
    if (bytes_read < chunk_size) {
        fseek(file, chunk_size - bytes_read, SEEK_CUR);
    }

    return text_chunk;
}


/**
 * Checks if a given file has the PNG signature.
 * 
 * This function returns TRUE if the provided FILE object has a PNG signature.
 * After execution, the file pointer will be 8 bytes ahead in the file.
 * @param file Pointer to a FILE object representing the image file.
 * @return
 *     Returns TRUE if the file has the PNG signature;
 *     otherwise, returns FALSE.
 */
static
bool has_png_signature( FILE *file ) {
    static const RpBYTE SIGNATURE[] = {137, 80, 78, 71, 13, 10, 26, 10};
    RpBYTE buffer[sizeof(SIGNATURE)];

    // store the current position of the file
    const long initial_pos = ftell(file);

    bool has_signature =
        fread(buffer, sizeof(buffer), 1, file) &&
        memcmp(buffer, SIGNATURE, sizeof(SIGNATURE))==0;

    // restore the position if there was no PNG signature
    if (!has_signature) { fseek(file, initial_pos, SEEK_SET);  }
    return has_signature;
}


/*===========================================================================
|  ////////////////////////// PUBLIC FUNCTIONS ///////////////////////////  | 
===========================================================================*/

/**
 * Loads all text chunks from a PNG file.
 */
RpEmbStrings* RP_embstrings_from_png( FILE *file )
{
    #define CHUNK_HEADER_SIZE 8 /* CHUNK_LENGTH + CHUNK_TYPE */
    #define CHUNK_CRC_SIZE    4
    bool   text_found, end_of_file;
    RpBYTE chunk_header[CHUNK_HEADER_SIZE];
    RpBYTE *chunk_type; RpUTF8* candidate_chunk;
    unsigned chunk_size;
    //int idx, i;
    size_t bytes_read;
    //int key_length;
    //int max_key_length = 0;
    //bool found_any_key = false;

    if( !has_png_signature(file) ) { return NULL; }

    RpNamedStr* namedstr_chain = NULL;
    end_of_file = false;
    while( !end_of_file  )
    {
        /* intenta leer el header del chunk */
        bytes_read = fread( chunk_header, 1, sizeof(chunk_header), file );
        if( bytes_read!=sizeof(chunk_header) ) {
            end_of_file=true; continue;
        }

        /* calcula el tama~nio y tipo del chunk */
        chunk_size = (chunk_header[0] << 24) |
                     (chunk_header[1] << 16) |
                     (chunk_header[2] <<  8) |
                     (chunk_header[3]      ) ;
        chunk_type = &chunk_header[4];
        text_found = memcmp(chunk_type,"tEXt",4)==0;

        /* si no es un "tEXt" chunk, saltamos al siguiente */
        if( !text_found ) {
            fseek( file, chunk_size+CHUNK_CRC_SIZE, SEEK_CUR );
            continue;
        }

        /* intenta cargar el chunk de texto */
        /* si no se pudo cargar ningun chuck, el archivo termino o hubo error */
        candidate_chunk = load_text_chunk( chunk_size, 0, 0, file );
        fseek( file, CHUNK_CRC_SIZE, SEEK_CUR );
        if( !candidate_chunk ) {
            end_of_file=true; continue;
        }

        // mantener este codigo comentado, sera implementado en un futuro
        // idx = find_chunk_index( candidate_chunk, keys, num_keys );

        namedstr_chain = rp_create_namedstr(
            candidate_chunk,
            candidate_chunk + strlen(candidate_chunk) + 1,
            namedstr_chain);

        printf("tEXt chunk name: %s\n", namedstr_chain->name );


        free( candidate_chunk );
    }

    rp_destroy_namedstr_chain( namedstr_chain );
    return NULL;
}


