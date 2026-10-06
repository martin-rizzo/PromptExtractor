/*
| File    : embstrings.c
| Purpose : Manage embedded strings and named strings.
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
#include "embstrings.h"
#define NamedStrPointer RpNamedStr*

/**
 * Extracts a 16-bit UTF-16 code unit from a byte buffer at a specified index.
 *
 * Reads two consecutive bytes from the buffer starting at `idx` and combines
 * them into a single 16-bit unsigned integer. The byte order is determined by
 * the `is_little_endian` parameter.
 *
 * @param buf   Pointer to the source byte buffer. Must be an unsigned integer
 *              type (e.g., `unsigned char`, `uint8_t`, or `RpBYTE`).
 * @param idx   The byte offset within the buffer where the code unit begins.
 * @param is_little_endian Boolean flag specifying the byte order:
 *                          - `0` (false): Big-endian format.
 *                          - `1` (true ): Little-endian format.
 * @return
 *     A 16-bit unsigned integer representing the decoded UTF-16 code unit.
 */
#define GET_UTF16(buf,idx,is_little_endian) (                   \
    (is_little_endian) ? (buf[(idx)+0])    | (buf[(idx)+1]<<8)  \
                       : (buf[(idx)+0]<<8) | (buf[(idx)+1]   ) )\


/*===================== SIMPLE UTF16/UTF8 CONVERSION ======================*/

/**
 * Converts a Unicode codepoint to its UTF-8 byte representation.
 *
 * Writes the encoded bytes into the provided output buffer. The buffer
 * must be capable of holding at least 4 bytes.
 *
 * @param codept The Unicode codepoint to convert. Valid values are in the range
 *               [0x0000, 0x10FFFF], excluding Unicode surrogate halves
 *               (0xD800–0xDFFF).
 * @param out    Pointer to the output buffer where the UTF-8 bytes will be written.
 * @return The number of bytes written (1 to 4), or 0 if the codepoint is
 *         invalid (e.g., in the surrogate range or greater than 0x10FFFF).
 */
static
size_t _codepoint_to_utf8(unsigned int codept, RpUTF8 *out) {
    if (codept <= 0x7F) {
        out[0] = (RpUTF8)codept;
        return 1;
    } else if (codept <= 0x7FF) {
        out[0] = (RpUTF8)(((codept >>  6)       ) | 0xC0);
        out[1] = (RpUTF8)(((codept      ) & 0x3F) | 0x80);
        return 2;
    } else if (codept <= 0xD7FF || (codept >= 0xE000 && codept <= 0xFFFF)) {
        out[0] = (RpUTF8)(((codept >> 12)       ) | 0xE0);
        out[1] = (RpUTF8)(((codept >>  6) & 0x3F) | 0x80);
        out[2] = (RpUTF8)(((codept      ) & 0x3F) | 0x80);
        return 3;
    } else if (codept >= 0x10000 && codept <= 0x10FFFF) {
        out[0] = (RpUTF8)(((codept >> 18)       ) | 0xF0);
        out[1] = (RpUTF8)(((codept >> 12) & 0x3F) | 0x80);
        out[2] = (RpUTF8)(((codept >>  6) & 0x3F) | 0x80);
        out[3] = (RpUTF8)(((codept      ) & 0x3F) | 0x80);
        return 4;
    }
    // outside the valid Unicode range / isolated surrogates
    return 0; 
}


// Converts a source character buffer into a newly allocated UTF-8 string.
RpUTF8 *rp_strdup_utf8(const void* buffer, size_t bufsize, RP_ENCODING bufencoding) {
    if (!buffer) { return NULL; }
    const RpBYTE* src = (const RpBYTE *)buffer;
    RpUTF8* result = NULL;

    switch (bufencoding) {

        //-----------------------------------------------------------
        // if the buffer is already in UTF-8 (or ASCII), simply copy it
        case RP_ENCODING_ASCII:
        case RP_ENCODING_UTF8:
            // if bufsize is not specified,
            // use the length of the string
            if (bufsize==0) { bufsize = strlen((const char *)buffer); }
            result = malloc(bufsize+1);
            if (result) {
                memcpy(result, buffer, bufsize);
                result[bufsize] = '\0';
            }
            return result;

            
        //-----------------------------------------------------------
        // UTF-16 buffers require character-by-character conversion
        case RP_ENCODING_UTF16LE:
        case RP_ENCODING_UTF16BE:
            // if bufsize is not specified,
            // search for the null terminator (0x00 0x00)
            if (bufsize == 0) {
                while (!( src[bufsize] == 0x00 && src[bufsize + 1] == 0x00 ))
                { bufsize += 2; }
            }
            // worst case:
            // each uint16 generates up to 3 bytes in UTF-8 (+1 byte for '\0')
            const size_t max_utf8size = (bufsize / 2) * 3 + 1;
            result = malloc(max_utf8size);
            if (!result) return NULL;

            size_t i = 0;      // Read index in bytes for the source buffer
            size_t out_idx = 0; // Write index for the result buffer
            while (i < bufsize) {
                unsigned codept = GET_UTF16(src,i,bufencoding==RP_ENCODING_UTF16LE);
                i += 2;
                // surrogate pair codepoint correction
                if (codept >= 0xD800 && codept <= 0xDBFF && i < bufsize ) {
                    unsigned low = GET_UTF16(src,i,bufencoding==RP_ENCODING_UTF16LE);
                    if (low >= 0xDC00 && low <= 0xDFFF) {
                        codept = 0x10000 + (((codept - 0xD800) << 10) | (low - 0xDC00));
                        i += 2; // Consume the second 16-bit word
                    }
                }
                // convert, replacing with '?' if the sequence is invalid
                size_t written = _codepoint_to_utf8(codept, &result[out_idx]);
                if (written == 0) { result[out_idx++] = '?'; }
                else              { out_idx += written; }
            }

            // ensure null terminator
            result[out_idx++] = '\0'; 

            // adjust memory block to actual used size and return
            RpUTF8 *shrunk = realloc(result, out_idx);
            return shrunk ? shrunk : result;


        //-----------------------------------------------------------
        // any unknown encoding returns an empty string
        default:
            result = malloc(4);
            if (result) { memset(result, 0, 4); }
            return result;

    }
}


// Detects the encoding of a string.
RP_ENCODING rp_detect_encoding(const void *buffer, size_t buffer_size) {
    if (!buffer || buffer_size <= 0) { return RP_ENCODING_UNKNOWN; }
    const unsigned char * const buf = (const unsigned char *)buffer;
    
    // check for byte order mark (BOM)
    if (buffer_size >= 2) {
        if (buf[0] == 0xFE && buf[1] == 0xFF) { return RP_ENCODING_UTF16BE; }
        if (buf[0] == 0xFF && buf[1] == 0xFE) { return RP_ENCODING_UTF16LE; }
    }
    if (buffer_size >= 3 && buf[0] == 0xEF && buf[1] == 0xBB && buf[2] == 0xBF) {
        return RP_ENCODING_UTF8;
    }

    // check for UTF-16 heuristic (interleaved null bytes) 
    size_t null_even=0, null_odd=0, consecutive_null=0;
    bool   has_non_ascii = false;
    for (size_t i = 0; i<buffer_size && consecutive_null<2 ; i++) {
        if (buf[i] >= 128 ) { has_non_ascii = true; }
        if (buf[i] == 0 ) {
            if (i % 2 == 0) { null_even++; } else { null_odd++; }
            consecutive_null++;
        } else { 
            consecutive_null = 0;
        }
    }

    // even for ASCII/UTF-8, a 'null' byte might occasionally slip in (even
    // or odd position) since the loop above can stop during two consecutive
    // nulls. (hence the following check uses <=1)
    if (null_even<=1 && null_odd<=1 ) {
        return has_non_ascii ? RP_ENCODING_UTF8 : RP_ENCODING_ASCII;
    }

    // at this point, we are in UTF-16 territory (with multiple nulls)
    return null_even > null_odd ? RP_ENCODING_UTF16BE : RP_ENCODING_UTF16LE;
}


/*============================ NamedStr STRUCT ============================*/

// Creates a new RpNamedStr structure with copies of the provided name and value.
RpNamedStr* rp_create_namedstr(const RpUTF8* name,
                               const RpUTF8* value,
                               RpNamedStr*   chain)
{
    return rp_attach_namedstr(strdup(name), strdup(value), chain);
}


// Creates a new RpNamedStr that takes ownership of the provided name and value.
RpNamedStr* rp_attach_namedstr(const RpUTF8* name_in_heap,
                               const RpUTF8* value_in_heap,
                               RpNamedStr*   chain)
{
    RpNamedStr* namedstr = malloc(sizeof(RpNamedStr));
    namedstr->name  = name_in_heap;
    namedstr->value = value_in_heap;
    namedstr->_next = chain;
    return namedstr;
}


// Destroys a single RpNamedStr node and frees its associated resources.
const RpNamedStr* rp_destroy_namedstr(const RpNamedStr* namedstr)
{
    if( !namedstr ) { return NULL; }
    RpNamedStr* next_in_chain = namedstr->_next;
    free( (void*)namedstr->name  );
    free( (void*)namedstr->value );
    free( (void*)namedstr );
    return next_in_chain;
}


// Destroys an entire chain of RpNamedStr nodes.
void rp_destroy_namedstr_chain(const RpNamedStr* namedstr)
{
    while (namedstr!=NULL) {
        namedstr = rp_destroy_namedstr(namedstr);
    }
}


/*========================== RpEmbStrings STRUCT ==========================*/

// Creates a new RpEmbStrings structure taking ownership of the string chain.
RpEmbStrings* rp_attach_embstrings(RpNamedStr* chain_in_heap)
{
    RpNamedStr* current;
    RpEmbStrings* embstrings = malloc(sizeof(RpEmbStrings));

    // count the number of elements in the chain
    current = chain_in_heap;
    int count = 0; while (current != NULL) {
        count++;
        current = current->_next;
    }

    // initialize RpEmbStrings and allocate space for 'items' to hold all chain elements
    embstrings->items  = malloc(sizeof(NamedStrPointer) * count);
    embstrings->count  = count;
    embstrings->_chain = chain_in_heap;

    // populate the 'items' array with each element from the chain
    current = chain_in_heap;
    int idx = count; while (current != NULL) {
        embstrings->items[--idx] = current;
        current = current->_next;
    }

    return embstrings;
}


// Destroys an RpEmbStrings structure and frees its associated resources.
void rp_destroy_embstrings(const RpEmbStrings* embstrings)
{
    if( !embstrings ) { return; }
    
    // free the array of pointers and the RpEmbStrings itself
    rp_destroy_namedstr_chain(embstrings->_chain);
    free((void*)embstrings->items);
    free((void*)embstrings);
}
