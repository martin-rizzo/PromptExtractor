/*
| File    : types.h
| Purpose : Simple types used in the project.
| Author  : Martin Rizzo | <martinrizzo@gmail.com>
| Date    : Oct 2, 2026
| Repo    : https://github.com/martin-rizzo/PromptExtractor
| License : MIT
|- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
|                              PromptExtractor
| A lightweight CLI tool to extract embedded prompts from AI-generated images.
\_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/
#ifndef PE__TYPES_H__
#define PE__TYPES_H__
#ifndef _POSIX_C_SOURCE
#define strdup(str) pe_strdup_utf8(str,0,PE_ENCODING_UTF8)
#endif
#define PE_UNUSED(x) ((void)&x);


// caracter codificado en UTF-8
typedef char PeUTF8;

// un byte con rango de valores de 0 a 255
typedef unsigned char PeBYTE;



/*============================ STATE REPORTING ============================*/

enum PEREPORT_TYPE {
    PEREPORT_INFO,     ///< el mensaje reportado es un mensaje informativo
    PEREPORT_WARNING,  ///< el mensaje reportado es simplemente un warning
    PEREPORT_ERROR     ///< el mensaje reportado es un error en la ejecución del programa
};
typedef enum PEREPORT_TYPE PEREPORT_TYPE;
typedef void (PeReportFn)(PEREPORT_TYPE type, const PeUTF8* message);


/*============================= TEXT ENCODING =============================*/

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



#endif /*PE__TYPES_H__*/
