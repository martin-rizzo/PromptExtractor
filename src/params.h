#ifndef PE__PARAMS_H__
#define PE__PARAMS_H__
#include <stdio.h>
#include <stdbool.h>
#include "types.h"     //< Basic types such as PeUTF8, PeBYTE, PeReportFn, etc. 
#include "textchunk.h" //< PeTextChunk structure and related functions.


/**
 * Represents the text extracted from an image file (JPEG, PNG, etc.).
 */
struct PeTextContent {
    const char*   id;          ///< Constant uppercase C-style identifier (e.g., "USER_COMMENT")
    const PeUTF8* name;        ///< Name as it appears in the file (e.g., "User Comment")
    const PeUTF8* value;       ///< Text stored in the variable (e.g., "This is the embedded text")
    PeUTF8*       _scratchpad; ///< Modifiable copy of the text for internal use
};
typedef struct PeTextContent PeTextContent;


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
 * Information related to the original file from which parameters were extracted.
 */
struct PeFileInfo {
    const PeUTF8* name;                        ///< File name including extension
    const PeUTF8* path;                        ///< Full path of the file
    PeTextContent texts[PE_MAX_TEXTCHUNK_COUNT]; ///< Array of found text contents (max number of elements determined by `PE_MAX_TEXTCHUNK_COUNT`)
    int           texts_count;                 ///< Number of text contents found in the file
};
typedef struct PeFileInfo PeFileInfo;


/*============================ PeParams STRUCT ============================*/

/**
 * Generation parameters extracted from the AI image file.
 */
struct PeParams {
    PeString   prompt;        ///< Text prompt used for generation
    PeString   negative;      ///< Negative prompt used for generation
    PeInt      steps;         ///< Number of generation steps
    PeInt      seed;          ///< Random seed for generation
    PeFloat    cfg;           ///< Classifier-free guidance scale
    PeFileInfo file;          ///< Information related to the original image file (JPEG, PNG, etc.)
    const PeBYTE* _data[1];   ///< Internal variable-size data array
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
