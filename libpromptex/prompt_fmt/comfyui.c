/*
| File    : prompt_fmt/comfyui.c
| Purpose : Parse ComfyUI prompt and workflow JSONs to extract generation parameters
| Author  : Martin Rizzo | <martinrizzo@gmail.com>
| Date    : Oct 14, 2026
| Repo    : https://github.com/martin-rizzo/PromptExtractor
| License : MIT
|- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
|                              PromptExtractor
| A lightweight CLI tool to extract embedded prompts from AI-generated images.
\_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <promptex/types.h>
#include "yyjson.h"
#include "params_set.h"
#include "params_yyjson.h"
#include "comfyui_helpers.h"
#include "internal.h"
#include "comfyui.h"

static const PeUTF8* POSITIVE_PATHS[] = {
    "prompt", "text", "text_g", "text_positive", "populated_text", NULL
};
//static const PeUTF8* NEGATIVE_PATHS[] = {
//    "prompt", "text", "text_g", "text_negative", "populated_text", NULL
//};



struct PeTextBuffer {
    const PeUTF8* ptr;  ///< pointer to the text buffer
    size_t        size; ///< size of the buffer including null terminator
};
typedef struct PeTextBuffer PeTextBuffer;
typedef struct PeTextBuffer PeJSONBuffer;

struct ComfyJSONs {
    PeJSONBuffer workflow;
    PeJSONBuffer prompt;
};
typedef struct ComfyJSONs ComfyJSONs;


/**
 * Checks if a given text string appears to be a valid JSON.
 *
 * This function verifies that the provided text starts and ends with matching
 * JSON delimiters ('{' and '}' for objects, '[' and ']' for arrays).
 *
 * @param text The null-terminated UTF-8 string to evaluate.
 * @return
 *   `true` if the text starts and ends with valid JSON delimiters,
 *   `false` otherwise.
 */
static bool
_looks_like_json(const PeUTF8* text
){
    if (!text) return false;

    // find the first non-whitespace character
    while (*text && pe_isspace(*text)) {
        text++;
    }
    char first_char = *text;

    // find the last non-whitespace character
    size_t len = strlen(text);
    if (len == 0) return false;

    const char* end = text + len - 1;
    while (end > text && pe_isspace(*end)) {
        end--;
    }
    char last_char = *end;

    if (first_char == '{' && last_char == '}' ) { return true; }
    if (first_char == '[' && last_char == ']' ) { return true; }
    return false;
}


/**
 * Extracts workflow and prompt JSON strings from the provided text entries.
 *
 * Iterates through the list of text entries, looking for workflow and prompt
 * entries containing JSON strings, and assigns them to the corresponding
 * fields in the result structure.
 *
 * @param texts      An array of text entries to search through.
 * @param text_count The number of text entries in the array.
 * @return
 *     A ComfyJSONs structure populated with pointers to the extracted workflow
 *     and prompt JSON strings. Fields are NULL if no JSON strings are found.
 */
static ComfyJSONs
_extract_comfy_jsons(PeTextContent* texts,
                     int            text_count
){
    ComfyJSONs result;
    result.workflow.ptr  = NULL;
    result.workflow.size = 0;
    result.prompt.ptr  = NULL;
    result.prompt.size = 0;
    for (int i=0; i<text_count; ++i)
    {
        if (0==strcmp(texts[i].id, "WORKFLOW")) {
            if (_looks_like_json(texts[i].content)) {
                result.workflow.ptr  = texts[i].content;
                result.workflow.size = texts[i].content_size;
            }
        }
        else if (0==strcmp(texts[i].id, "PROMPT")) {
            if (_looks_like_json(texts[i].content)) {
                result.prompt.ptr  = texts[i].content;
                result.prompt.size = texts[i].content_size;
            }
        }
    }
    return result;
}



static bool
_set_params_from_initial_sampler(PeParams* params, const yyjson_val* root) {

    yyjson_obj_iter iter;
    yyjson_obj_iter_init(root, &iter);
    
    yyjson_val *key;
    while ((key = yyjson_obj_iter_next(&iter))) {
        //// the key is the node ID (e.g., "3", "4", "KSampler")
        //const char *node_id = yyjson_get_str(key);
        
        // the associated value is the node object
        yyjson_val *node = yyjson_obj_iter_get_val(key);
        if (!node) { continue; }

        const char* class_type = pecmf_get_classtype(node);
        if (!class_type) { continue; }


        if (PeUTF8_startswith(class_type,"ZSamplerTurbo"))
        {
            const yyjson_val* inputs = pecmf_get_inputs(node);
            if (!inputs) { continue; }

            PeImageSize size = pecmf_find_image_size(inputs, "latent_input", root);
            if (!size.width || !size.height) { continue; }

            const PeUTF8* positive      = pecmf_find_string(inputs, "positive"     , POSITIVE_PATHS, root);
            const PeUTF8* positive_stg2 = pecmf_find_string(inputs, "positive_stg2", POSITIVE_PATHS, root);
            const PeUTF8* positive_stg3 = pecmf_find_string(inputs, "positive_stg3", POSITIVE_PATHS, root);
            unused(positive_stg3);
            unused(positive_stg2);
            

            printf("\n");
            printf("###>> FOUND!!! class_type:%s\n", class_type);
            printf("\n");

            pe_set_string  (&params->prompt , positive        );
            pe_set_long    (&params->width  , size.width      );
            pe_set_long    (&params->height , size.height     );
            pe_set_yylong  (&params->seed   , inputs, "seed"  );
            pe_set_yylong  (&params->steps  , inputs, "steps" );
            pe_set_strfloat(&params->cfg    , "1.0"           );
            pe_set_string  (&params->sampler, "zsampler_turbo");
            return true;
        }
    }

    return false;
}

static bool
_set_params_from_prompt_json(PeParams* params,
                             PeUTF8*   prompt_json
){

    // in-situ parsing (modifies the original buffer to avoid string allocations)
    yyjson_doc *doc = yyjson_read(prompt_json, strlen(prompt_json), YYJSON_READ_INSITU);
    if (!doc) {
        fprintf(stderr, "Error parsing JSON\n");
        return false;
    }

    // get the root (must be an object containing node IDs as keys)
    yyjson_val *root = yyjson_doc_get_root(doc);
    if (!yyjson_is_obj(root)) {
        fprintf(stderr, "Prompt root is not a valid JSON object\n");
        yyjson_doc_free(doc);
        return false;
    }

    bool work = _set_params_from_initial_sampler(params, root);

    // free the document (does not free the original text buffer since it was in-situ)
    yyjson_doc_free(doc);

    return work;
}


/*===========================================================================
|  ////////////////////////// PUBLIC FUNCTIONS ///////////////////////////  | 
===========================================================================*/

bool pe_comfyui_parse(PeParams*      out_params,
                      PeTextContent* texts,
                      int            text_count
){
    if (!out_params) { return false; }

    // attempt to extract the 2 JSONs stored by ComfyUI in the image metadata
    ComfyJSONs comfy_jsons = _extract_comfy_jsons(texts, text_count);
    if (comfy_jsons.workflow.ptr==NULL || comfy_jsons.prompt.ptr==NULL) {
        return false;
    }

    // use `_storage` as the buffer for prompt_json;
    // `_storage` is guaranteed to be large enough to hold any of the
    // texts stored in the image metadata (`fileinfo->texts[n]`)
    PeUTF8* prompt_json = out_params->_storage.ptr;
    memcpy(prompt_json, comfy_jsons.prompt.ptr, comfy_jsons.prompt.size);
   
    return _set_params_from_prompt_json(out_params, prompt_json);
}
