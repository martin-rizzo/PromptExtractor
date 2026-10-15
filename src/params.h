#ifndef PE__PARAMS_H__
#define PE__PARAMS_H__
#include <stdio.h>
#include <stdbool.h>
#include "types.h"     //< Basic types such as PeUTF8, PeBYTE, PeReportFn, etc. 
#include "textchunk.h" //< PeTextChunk structure and related functions.


/**
 * An optional integer value.
 */
struct PeInt {
    bool has_value; ///< Flag indicating if the integer value is present
    int  value;     ///< The integer value
};
typedef struct PeInt PeInt;


/**
 * An optional floating-point value.
 */
struct PeFloat {
    bool  has_value; ///< Flag indicating if the float value is present
    float value;     ///< The floating-point value
};
typedef struct PeFloat PeFloat;


/**
 * An optional string value.
 */
struct PeString {
    bool          has_value; ///< Flag indicating if the string value is present
    const PeUTF8* value;     ///< The string value
};
typedef struct PeString PeString;


/**
 * Represents a text variable extracted directly from an image file (JPEG, PNG, etc.).
 */
struct PeTextContent {
    const char*   id;           ///< Identifier of the variable in uppercase format (e.g., "USER_COMMENT")
    const PeUTF8* name;         ///< Name of the variable as it appears in the file (e.g., "User Comment")
    const PeUTF8* content;      ///< The text content of the variable (e.g., "This is the embedded text")
    const size_t  content_size; ///< Size in bytes of the `content` buffer
};
typedef struct PeTextContent PeTextContent;


/**
 * Information related to the original file from which parameters were extracted.
 */
struct PeFileInfo {
    const PeUTF8* name;                          ///< File name including extension
    const PeUTF8* path;                          ///< Full path of the file
    PeTextContent texts[PE_MAX_TEXTCHUNK_COUNT]; ///< Array of found text contents (max number of elements determined by `PE_MAX_TEXTCHUNK_COUNT`)
    int           texts_count;                   ///< Number of text contents found in the file
};
typedef struct PeFileInfo PeFileInfo;


/**
 * Internal storage buffer for parameter strings and user data.
 *
 * This structure manages the underlying memory buffer for all string data
 * referenced by the `PeParams` structure (e.g., `prompt`, `negative`, etc).
 * The `ptr` member points to a contiguous memory region where these strings
 * are stored; the size of this buffer is always sufficient to store any text
 * contents from `peparams.fileinfo.texts[n].content`. Likewise, the `size`
 * member indicates the total size in bytes.
 *
 * It also provides two user data pointers (`user1`, `user2`) that can be
 * allocated by external code (e.g., `prompt_fmt` implementations) to store
 * custom data. These pointers are automatically freed when the associated
 * `PeParams` structure is destroyed.
 */
struct PeStorage {
    void*  ptr;    ///< Pointer to the internal data buffer.
    size_t size;   ///< Total size of the internal data buffer in bytes.
    void*  user1;  ///< Optional user data pointer 1, automatically freed upon `PeParams` destruction.
    void*  user2;  ///< Optional user data pointer 2, automatically freed upon `PeParams` destruction.
};
typedef struct PeStorage PeStorage;


/*============================ PeParams STRUCT ============================*/

/**
 * Generation parameters extracted from the AI image file.
 */
struct PeParams {
    PeString    prompt;    ///< Text prompt used for generation
    PeString    negative;  ///< Negative prompt used for generation
    PeInt       steps;     ///< Number of generation steps
    PeInt       seed;      ///< Random seed for generation
    PeFloat     cfg;       ///< Classifier-free guidance scale
    PeFileInfo  fileinfo;  ///< Information related to the original image file (JPEG, PNG, etc.)
    PeStorage   _storage;  ///< Internal buffer where the strings referenced by each parameter are actually stored.
};
typedef struct PeParams PeParams;


/**
 * Creates a `PeParams` by extracting generation parameters from a image file (e.g., JPEG, PNG).
 * @param fullpath  The full path to the image file.
 * @param report    A callback function for reporting errors, warnings, and info messages.
 * @return
 *     A pointer to the newly created PeParams structure, or NULL if an error occurs.
 */
const PeParams* pe_params_from_path(const PeUTF8* fullpath,
                                    PeReportFn    report
                                    );

/**
 * Creates a `PeParams` by extracting generation parameters from the provided stream.
 * @param stream      A pointer to the file stream to read from.
 * @param stream_name Specifies the source of the stream. It can be:
 *                     - The full path of the associated file.
 *                     - An identifying name if the stream is not file-backed.
 *                     - An empty string as a fallback.
 * @param report     A callback function for reporting errors, warnings, and info messages.
 * @return
 *     A pointer to the newly created PeParams structure, or NULL if an error occurs.
 */
const PeParams* pe_params_from_stream(FILE*         stream,
                                      const PeUTF8* stream_name,
                                      PeReportFn    report
                                      );


/**
 * Creates a `PeParams` by extracting generation parameters from a chain of text chunks.
 * @param chain       A pointer to the first node of the text chunk chain.
 * @param stream_name Specifies the source of the chain. It can be:
 *                     - The full path of the associated file.
 *                     - An identifying name if the chain is not file-backed.
 *                     - An empty string as a fallback.
 * @param report      A callback function for reporting errors, warnings, and info messages.
 * @return
 *     A pointer to the newly created PeParams structure, or NULL if an error occurs.
 */
const PeParams* pe_params_from_chain(PeTextChunk*  chain,
                                     const PeUTF8* stream_name,
                                     PeReportFn    report
                                     );

/**
 * Destroys and frees the memory allocated for a PeParams structure.
 * @param params A pointer to the `PeParams` to be destroyed.
 */
void pe_destroy_params(const PeParams* params
                       );



#endif /*PE__PARAMS_H__*/
