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
#include "types.h"       //< Basic types such as PeUTF8, PeBYTE, PeReportFn, etc. 
#include "textchunk.h"   //< PeTextChunk structure and related functions.
#include "params_defs.h" //< Definitions for parameter types, fileinfo and storage.


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
