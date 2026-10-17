/*
| File    : image_fmt/png.c
| Purpose : Read a PNG file and extract embedded text chunks.
| Author  : Martin Rizzo | <martinrizzo@gmail.com>
| Date    : Oct 4, 2026
| Repo    : https://github.com/martin-rizzo/PromptExtractor
| License : MIT
|- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
|                              PromptExtractor
| A lightweight CLI tool to extract embedded prompts from AI-generated images.
\_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "types.h"
#include "internal.h"
#include "textchunk.h"
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
static PeUTF8*
_load_text_chunk(size_t chunk_size, size_t min_allocation, size_t max_allocation, FILE* file) {

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

    PeUTF8* text_chunk = (PeUTF8*)calloc(1, bytes_to_alloc + SAFE_BYTES);
    size_t  bytes_read = fread(text_chunk, sizeof(PeBYTE), bytes_to_request, file);

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
static bool
_has_png_signature( FILE *file ) {
    static const PeBYTE SIGNATURE[] = {137, 80, 78, 71, 13, 10, 26, 10};
    PeBYTE buffer[sizeof(SIGNATURE)];

    return
        fread(buffer, sizeof(buffer), 1, file) &&
        memcmp(buffer, SIGNATURE, sizeof(SIGNATURE))==0;
}


/*===========================================================================
|  ////////////////////////// PUBLIC FUNCTIONS ///////////////////////////  | 
===========================================================================*/

/**
 * Extracts a chain of text chunks from a PNG file.
 *
 * Reads through the PNG file, identifies `tEXt` chunks, and builds a linked
 * list of `PeTextChunk` structures containing the extracted text data.
 * The caller is responsible for freeing the returned chain using
 * `pe_destroy_textchunk_chain()`.
 *
 * @param file   Pointer to the FILE object representing the PNG file.
 * @param report A callback function for reporting errors, warnings, and info messages.
 * @return
 *    A pointer to the first text chunk in the chain, or NULL if no PNG file
 *    was found or error occurred.
 */
PeTextChunk* pe_textchunk_chain_from_png(FILE *     file,
                                         PeReportFn report
){
    unused(report);
    #define CHUNK_HEADER_SIZE 8 /* CHUNK_LENGTH + CHUNK_TYPE */
    #define CHUNK_CRC_SIZE    4
    bool   text_found, end_of_file;
    PeBYTE chunk_header[CHUNK_HEADER_SIZE];
    PeBYTE *chunk_type; PeUTF8* candidate_chunk;
    unsigned chunk_size;
    size_t bytes_read;

    // verifica si el archivo es PNG mediante su firma
    // en caso de no serlo, retorna NULL con el puntero de lectura en la posicion inicial
    const long initial_pos = ftell(file);
    if( !_has_png_signature(file) ) {
        fseek(file, initial_pos, SEEK_SET);
        return NULL;
    }

    PeTextChunk* textchunk_chain = NULL;
    end_of_file = false;
    while( !end_of_file  )
    {
        // intenta leer el header del chunk
        bytes_read = fread( chunk_header, 1, sizeof(chunk_header), file );
        if( bytes_read!=sizeof(chunk_header) ) {
            end_of_file=true; continue;
        }

        // calcula el tama~nio y tipo del chunk
        chunk_size = (chunk_header[0] << 24) |
                     (chunk_header[1] << 16) |
                     (chunk_header[2] <<  8) |
                     (chunk_header[3]      ) ;
        chunk_type = &chunk_header[4];
        text_found = memcmp(chunk_type,"tEXt",4)==0;

        // si no es un "tEXt" chunk, saltamos al siguiente
        if( !text_found ) {
            fseek( file, chunk_size+CHUNK_CRC_SIZE, SEEK_CUR );
            continue;
        }

        // intenta cargar el chunk de texto
        // si no se pudo cargar ningun chuck, el archivo termino o hubo error
        candidate_chunk = _load_text_chunk( chunk_size, 0, PE_MAX_TEXTCHUNK_SIZE, file );
        fseek( file, CHUNK_CRC_SIZE, SEEK_CUR );
        if( !candidate_chunk ) {
            end_of_file=true; continue;
        }

        // TODO: deberia convertir el 'candidate_chunk' todo a mayusculas
        //       mas numeros y reemplazar cualquier signo raro con '_' (underscore)
        char* candidate_chunk_id = pe_strdup_UPPER_ASCII( candidate_chunk, 0, PE_ENCODING_ASCII );

        textchunk_chain = pe_create_textchunk(
            candidate_chunk_id,
            candidate_chunk,
            candidate_chunk + strlen(candidate_chunk) + 1,
            textchunk_chain);

        free( candidate_chunk_id );
        free( candidate_chunk );
    }

    return textchunk_chain;
}


