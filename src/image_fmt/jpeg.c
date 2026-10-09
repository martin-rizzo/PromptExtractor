/*
| File    : image_fmt/jpeg.c
| Purpose : Read a JPEG file and extract text chunks.
| Author  : Martin Rizzo | <martinrizzo@gmail.com>
| Date    : Oct 5, 2026
| Repo    : https://github.com/martin-rizzo/PromptExtractor
| License : MIT
|- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
|                              PromptExtractor
| A lightweight CLI tool to extract embedded prompts from AI-generated images.
\_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "types.h"
#include "textchunk.h"
#include "jpeg.h"
#define JPEG_ERROR (-1)


#define GET_WORD(ptr,is_intel) (           \
    (is_intel) ? (ptr[0]) | (ptr[1]<<8)    \
               : (ptr[0]<<8) | (ptr[1])   )\

#define GET_DWORD(ptr,is_intel) (                                       \
    (is_intel) ? (ptr[0]) | (ptr[1]<<8) | (ptr[2]<<16) | (ptr[3]<<24)   \
               : (ptr[0]<<24) | (ptr[1]<<16) | (ptr[2]<<8) | (ptr[3])  )\

#define READ_WORD( out_var, file )  \
    out_var  = fgetc(file) << 8;    \
    out_var |= fgetc(file);         \
    if( out_var<0x0000 || 0xFFFF<out_var ) { out_var=-1; }



/*=========================== HELPER FUNCTIONS ============================*/

bool has_jpeg_signature(FILE *file) {
    if (file == NULL) { return false; }

    // Guardamos la posición inicial del puntero del archivo
    long initial_pos = ftell(file);
    if (initial_pos == -1L) { return false; } // El flujo no soporta posicionamiento (ej. stdin)

    PeBYTE header[4];
    size_t bytes_read = fread(header, 1, 4, file);

    // Restauramos el puntero del archivo a su estado original
    fseek(file, initial_pos, SEEK_SET);

    // Un JPEG válido necesita al menos 4 bytes para verificar la firma común
    if (bytes_read < 4) {
        return false;
    }

    // Validar el marcador de inicio obligatorio: FF D8
    if (header[0] != 0xFF || header[1] != 0xD8) { return false; }

    // Validar el tercer byte (marcador de segmento de aplicación, comúnmente FF)
    if (header[2] != 0xFF) { return false; }

    // Validar el cuarto byte (variantes comunes: E0 para JFIF, E1 para EXIF, E8 para SPIFF)
    if (header[3] == 0xE0 || header[3] == 0xE1 || header[3] == 0xE8) {
        return true;
    }

    return false;
}

/**
 * Reads the markers at the beginning of each data segment in a JPG file.
 *
 * This function reads the input file byte by byte until it finds a valid
 * segment marker. Segment markers in JPG format start with the byte 0xFF,
 * followed by a byte other than 0xFF.
 *
 * @param file  Pointer to the JPG file to be read.
 * @return
 *     The value of the read marker if found successfully,
 *     or JPEG_ERROR (-1) in case of error or end of file.
 */
static
int _read_jpg_marker( FILE *file ) {
    int hi_byte, lo_byte;

    bool is_valid_marker = false;
    while ( !is_valid_marker )
    {
        /* Buscar el byte de inicio de marcador (0xFF) */
        do { hi_byte = fgetc(file); } while( hi_byte != EOF && hi_byte != 0xFF );
        if( hi_byte == EOF ) { return JPEG_ERROR; }

        /* Consumir bytes 0xFF consecutivos de relleno */
        do { lo_byte = fgetc(file); } while( lo_byte != EOF && lo_byte == 0xFF );
        if( lo_byte == EOF ) { return JPEG_ERROR; }

        /* Un marcador es válido si lo_byte no es 0x00 ( byte stuffing ) */
        is_valid_marker = (lo_byte != 0x00);
    }
    return (hi_byte << 8) | lo_byte;
}

enum APP1_FORMAT {
    APP1_FORMAT_UNKNOWN,
    APP1_FORMAT_EXIF,
    APP1_FORMAT_XMP,
    APP1_FORMAT_EXTENDED_XMP,
};
typedef enum APP1_FORMAT APP1_FORMAT;


static
APP1_FORMAT _read_app1_format( FILE * file ) {
    static const char EXIF_ID[]           = "Exif\x00\x00";
    static size_t     EXIF_IDSIZE         = 6;
    static const char XMP_ID[]            = "http://ns.adobe.com/xap/1.0/\x00";
    static size_t     XMP_IDSIZE          = 29;
    static const char EXTENDED_XMP_ID[]   = "http://adobe.com\x00";
    static size_t     EXTENDED_XMP_IDSIZE = 17;

    PeBYTE app1_formatid[6];
    size_t bytes_to_skip = 0;
    size_t bytes_to_compare = sizeof(app1_formatid);
    APP1_FORMAT app1_format = APP1_FORMAT_UNKNOWN;

    fread( app1_formatid, bytes_to_compare, 1, file );

    if (0==memcmp(app1_formatid, EXIF_ID, bytes_to_compare)) {
        app1_format   = APP1_FORMAT_EXIF;
        bytes_to_skip = EXIF_IDSIZE - bytes_to_compare;
    }
    else if (0==memcmp(app1_formatid, XMP_ID, bytes_to_compare)) {
        app1_format   = APP1_FORMAT_XMP;
        bytes_to_skip =  XMP_IDSIZE - bytes_to_compare;
    }
    else if (0==memcmp(app1_formatid, EXTENDED_XMP_ID, bytes_to_compare)) {
        app1_format   = APP1_FORMAT_EXTENDED_XMP;
        bytes_to_skip = EXTENDED_XMP_IDSIZE - bytes_to_compare;
    }

    // saltear
    if (bytes_to_skip) {
        while (bytes_to_skip>0) { if (fgetc(file) == EOF) break; }
    }
    return app1_format;
}



//static
bool _search_tiff_dir( int  *out_offset,
                       int  *out_count,
                       int   tag_to_find,
                       FILE *file,
                       int   is_intel,
                       int   seek_offset )
{
    static const int ENTRY_SIZE = 12; PeBYTE dir_buffer[ 48 ];
    const int max_entry_count = sizeof(dir_buffer) / ENTRY_SIZE;
    PeBYTE* ptr; int tag, total_entries;
    size_t entry_count;
    
    if( seek_offset>=0 && fseek(file, seek_offset, SEEK_SET)!=0 ) {
        return false;
    }
    if( fread( dir_buffer, 2, 1, file ) != 1 ) {
        return false;
    }
    total_entries = GET_WORD(dir_buffer,is_intel);    
    while( total_entries>0 ) {
        entry_count    = (total_entries<max_entry_count) ? total_entries : max_entry_count;
        total_entries -= entry_count;
        ptr            = dir_buffer;
        if( fread( ptr, ENTRY_SIZE, entry_count, file ) != entry_count ) {
            return false;
        }
        while( entry_count-->0 ) {
            tag = GET_WORD(ptr,is_intel); ptr+=4;
            if( tag != tag_to_find ) { ptr+=8; }
            else {
                (*out_count)  = GET_DWORD(ptr,is_intel); ptr+=4;
                (*out_offset) = GET_DWORD(ptr,is_intel); ptr+=4;
                return true;
            }
        }
    }
    return false;
}


static
PeTextChunk* _read_exif_user_comment( FILE *file ) {
    static const PeBYTE IntelHeader[4]    = { 0x49, 0x49, 0x2A, 0x00 };
    static const PeBYTE MotorolaHeader[4] = { 0x4D, 0x4D, 0x00, 0x2A };
    static const int TAG_EXIF_OFFSET  = 0x8769;
    static const int TAG_USER_COMMENT = 0x9286;
    static const int CHAR_CODE_SIZE   = 8; // char code designation = 8 bytes

    const long tiff_start = ftell( file );

    PeBYTE  header_and_offset[8];
    PeBYTE* header_data = &header_and_offset[0];
    PeBYTE* offset_data = &header_and_offset[4];

    bool is_intel;
    // int ifd0_offset, exif_offset, usercomment_offset, count, read_size;
    // const long tiff_start = ftell( file );

    // identify tiff header 
    fread( header_and_offset, sizeof(header_and_offset), 1, file );
    if (0==memcmp(header_data, IntelHeader, sizeof(IntelHeader))) {
         is_intel = true; /* intel format (little-endian) */
    }
    else if (0==memcmp(header_data, MotorolaHeader, sizeof(MotorolaHeader))) {
         is_intel = false; /* motorola format (big-endian) */
    }
    else { return NULL; } // unknown format

    size_t ifd0_offset = GET_DWORD( offset_data, is_intel );

    /* find UserComment */
    int exif_offset, usercomment_offset, count, usercomment_size;
    if( !_search_tiff_dir( &exif_offset, &count, TAG_EXIF_OFFSET,
                           file, is_intel, tiff_start+ifd0_offset ) ) {
        return NULL;
    }
    if( !_search_tiff_dir( &usercomment_offset, &usercomment_size, TAG_USER_COMMENT,
                           file, is_intel, tiff_start+exif_offset ) ) {
        return NULL;
    }

    if (usercomment_size>CHAR_CODE_SIZE)
    {
        usercomment_size -= CHAR_CODE_SIZE;
        char* usercomment = calloc( 1, usercomment_size+4 );
        fseek( file, tiff_start + usercomment_offset + CHAR_CODE_SIZE, SEEK_SET );
        fread( usercomment, usercomment_size, 1, file );
        PE_ENCODING encoding = pe_detect_encoding(usercomment, usercomment_size);

        PeTextChunk* textchunk = pe_attach_textchunk(
                                    pe_strdup_utf8("USER_COMMENT", 0, PE_ENCODING_ASCII),
                                    pe_strdup_utf8("User Comment", 0, PE_ENCODING_ASCII),
                                    pe_strdup_utf8(usercomment, usercomment_size, encoding),
                                    NULL);
        free( usercomment );
        return textchunk;
    }


    return NULL;
}
    



/*===========================================================================
|  ////////////////////////// PUBLIC FUNCTIONS ///////////////////////////  | 
===========================================================================*/

PeTextChunk* pe_textchunk_chain_from_jpeg(FILE*      file,
                                          PeReportFn report
){
    PE_UNUSED(report);
    enum  {
        MARKER_SOI          = 0xFFD8,
        MARKER_EOI          = 0xFFD9,
        MARKER_DRI          = 0xFFDD,
        MARKER_APP0         = 0xFFE0,
        MARKER_APP1,MARKER_APP2,MARKER_APP3,MARKER_APP4,
        MARKER_RST0         = 0xFFD0,
        MARKER_RST1,MARKER_RST2,MARKER_RST3,MARKER_RST4,
        MARKER_RST5,MARKER_RST6,MARKER_RST7
    };
    size_t start;
    int marker, size16bits;
    
    // recorrer uno por uno los data-segments del archivo jpeg
    // identificandolos mediante su 'marker'
    marker = 0; while( marker!=JPEG_ERROR && marker!=MARKER_EOI ) {
        marker = _read_jpg_marker( file );
        switch( marker ) {
            
            // el segmento `APP1` es donde la mayoria de las apps 
            // almacena los prompts y parametros de generacion de la imagen
            case MARKER_APP1:
                start = ftell( file );
                READ_WORD( size16bits, file );
                switch (_read_app1_format( file ))
                {
                    case APP1_FORMAT_EXIF:
                        return _read_exif_user_comment( file );

                    case APP1_FORMAT_XMP:
                        break;

                    case APP1_FORMAT_EXTENDED_XMP:
                        break;

                    default:
                        break;
                }
                if( size16bits>=2 ) { fseek(file, start+size16bits, SEEK_SET); }
                else                { marker=JPEG_ERROR; }
                break;
                
            // este segmento siempre tiene 4 bytes (2 del marker y 2 de ??)
            case MARKER_DRI:
                fseek(file, 2, SEEK_CUR);
                break;
                
            // segmentos sin parametro de tamaño
            case MARKER_SOI:
            case MARKER_EOI:
            case MARKER_RST0: case MARKER_RST1: case MARKER_RST2:
            case MARKER_RST3: case MARKER_RST4: case MARKER_RST5:
            case MARKER_RST6: case MARKER_RST7:
            case JPEG_ERROR:
                break;
    
            default:
                READ_WORD( size16bits, file );
                if( size16bits>=2 ) { fseek(file, size16bits-2, SEEK_CUR); }
                else                { marker=JPEG_ERROR; }
                break;
        }
    }
    return NULL;

}
