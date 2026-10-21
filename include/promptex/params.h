/*
| File    : params.h
| Purpose : Handling generation parameters extracted from AI image files.
| Author  : Martin Rizzo | <martinrizzo@gmail.com>
| Date    : Oct 9, 2026
| Repo    : https://github.com/martin-rizzo/PromptExtractor
| License : MIT
|- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
|                              PromptExtractor
| A lightweight CLI tool to extract embedded prompts from AI-generated images.
\_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/
#ifndef PE__PARAMS_H__
#define PE__PARAMS_H__
#include <stdio.h>
#include <stdbool.h>
#include <promptex/types.h>  //< Basic types such as PeUTF8, PeBYTE, PeReportFn, etc. 

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
    PeTextContent texts[PE_MAX_TEXTCHUNK_COUNT]; ///< Array of text contents found in the file
                                                 ///  (max number of elements determined by `PE_MAX_TEXTCHUNK_COUNT`)
    int           text_count;                    ///< Number of valid elements in the `texts` array.
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
    PeString prompt;            ///< Text prompt used for generation
    PeString negative_prompt;   ///< Negative prompt used for generation
    PeString wildcard_prompt;   ///< Original prompt containing wildcards before dynamic expansion
    PeString sampler;           ///< Sampling method algorithm name
    PeLong   steps;             ///< Number of generation steps
    PeFloat  cfg;               ///< Classifier-free guidance scale
    PeLong   seed;              ///< Random seed for generation
    PeLong   width;             ///< Base output width of the image
    PeLong   height;            ///< Base output height of the image
    PeFloat  denoising;         ///< Strength of the overall denoising effect
    struct {// MODEL ------
        PeString name;          ///< Name of the checkpoint model used
        PeString hash;          ///< Unique hash identifier of the checkpoint model
    } model;
    struct {// HIRES.FIX ---
        PeString upscaler;      ///< Upscaling algorithm used for Hires. fix
        PeLong   steps;         ///< Generation steps specifically for Hires. fix
        PeFloat  denoising;     ///< Denoising strength applied during Hires. fix upscaling
        PeFloat  upscale;       ///< Requested target upscale multiplier factor
        PeLong   width;         ///< Requested specific target upscale width
        PeLong   height;        ///< Requested specific target upscale height
        PeFloat  calc_upscale;  ///< Automatically calculated upscale multiplier based on inputs
        PeLong   calc_width;    ///< Final calculated target pixel width for the output image
        PeLong   calc_height;   ///< Final calculated target pixel height for the output image
    } hires;
    struct {// INPAINT -----
        PeFloat denoising;     ///< Inpainting denoising strength applied only to the masked area
        PeFloat mask_blur;     ///< Blur radius in pixels applied to the edges of the inpaint mask
    } inpaint;
    struct {// OVERRIDE SETTINGS
        PeFloat     eta;       ///< Random noise scale modifier for ancestral samplers
        PeLong      ensd;      ///< Eta Noise Seed Delta added to seed for replication compatibility
        PeLong      clip_skip; ///< Number of final layers ignored in the CLIP text encoder
    } settings;

//-- FILE ----------------
    PeFileInfo  fileinfo;       ///< Information related to the original image file (JPEG, PNG, etc.)
//-- INTERNAL ------------
    PeStorage   _storage;       ///< Internal buffer where the strings referenced by each parameter are stored
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
 * Destroys and frees the memory allocated for a PeParams structure.
 * @param params A pointer to the `PeParams` to be destroyed.
 */
void pe_destroy_params(const PeParams* params
                       );



#endif /*PE__PARAMS_H__*/
