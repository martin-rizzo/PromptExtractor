/*
| File    : prompt_fmt/comfyui.c
| Purpose :
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

/** Returns true if the character is considered whitespace. */
static inline bool _is_whitespace(char c) {
    return ('\x09' <= c && c <= ' ');
}

/** ?? */
static inline bool _starts_with(const char *str, const char *prefix) {
    while (*prefix) { if (*str++ != *prefix++) { return false; } }
    return true;
}

/** ?? */
static inline bool _str_equals(const char *str1, const char *str2) {
    return 0==strcmp(str1, str2);
}



struct ComfyUI {
    const PeUTF8* workflow;
    const PeUTF8* prompt;
};
typedef struct ComfyUI ComfyUI;



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
_looks_like_json(const PeUTF8* text) {
    if (!text) return false;

    // Buscar el primer carácter que no sea un espacio en blanco
    while (*text && _is_whitespace(*text)) {
        text++;
    }
    char first_char = *text;

    // Buscar el último carácter que no sea un espacio en blanco
    size_t len = strlen(text);
    if (len == 0) return false;

    const char* end = text + len - 1;
    while (end > text && _is_whitespace(*end)) {
        end--;
    }
    char last_char = *end;

    if (first_char == '{' && last_char == '}' ) { return true; }
    if (first_char == '[' && last_char == ']' ) { return true; }
    return false;
}


/**
 * Extracts workflow and prompt JSON strings from the provided file information.
 *
 * Iterates through the list of text entries in fileinfo, looking for
 * workflow and prompt entries containing JSON strings, and assigns them
 * to the corresponding fields in the result structure.
 * corresponding field in the result structure.
 *
 * @param fileinfo A pointer to the file information structure containing text entries.
 * @return
 *     A ComfyUI structure populated with pointers to the extracted workflow
 *     and prompt JSON strings. Fields are NULL if no JSON strings are found.
 */
static ComfyUI
_extract_comfyui_jsons(PeFileInfo* fileinfo) {
    ComfyUI result;
    result.workflow = NULL;
    result.prompt   = NULL;
    for (int i=0; i<fileinfo->texts_count; ++i)
    {
        if (0==strcmp(fileinfo->texts[i].id, "WORKFLOW")) {
            if (_looks_like_json(fileinfo->texts[i].content)) {
                result.workflow = fileinfo->texts[i].content;
            }
        }
        else if (0==strcmp(fileinfo->texts[i].id, "PROMPT")) {
            if (_looks_like_json(fileinfo->texts[i].content)) {
                result.prompt = fileinfo->texts[i].content;
            }
        }
    }
    return result;
}

static const char*
_get_class_type(const yyjson_val* node
){
    if (!yyjson_is_obj(node)) { return false; }

    yyjson_val *class_type = yyjson_obj_get(node, "class_type");
    if (!class_type) { return NULL; }

    const char *class_type_str = yyjson_get_str(class_type);
    if (!class_type_str) { return NULL; }

    return class_type_str;
}



/**
 * Returns the connected node for a given input connection identifier.
 * 
 * @param node       The base node (yyjson_val* object) from which to retrieve the input node.
 * @param input_name Name of the input connection donde buscar.
 * @param all_nodes  Un mapa id -> node (yyjson object) containing the entire comfyui 'prompt' structure.
 * @return
 *     Pointer to the node connected to de input or NULL if not found.
 */
static const yyjson_val*
_get_input_node(const yyjson_val* node,
                const char*       input_name,
                const yyjson_val* nodes_by_id)
{
    assert( node!=NULL );
    assert( input_name!=NULL );
    assert( nodes_by_id!=NULL );

    if (!yyjson_is_obj(node) || !yyjson_is_obj(nodes_by_id)) {
        return NULL;
    }

    const yyjson_val* inputs = yyjson_obj_get(node, "inputs");
    if (!yyjson_is_obj(inputs)) { return NULL; }

    const yyjson_val* wire_array = yyjson_obj_get(inputs, input_name);
    if (!yyjson_is_arr(wire_array) || yyjson_arr_size(wire_array)==0) { return NULL; }
        
    const yyjson_val* wire0 = yyjson_arr_get(wire_array, 0);
    if (!yyjson_is_str(wire0)) { return NULL; }

    const char* input_node_id = yyjson_get_str(wire0);

    const yyjson_val* input_node = yyjson_obj_get(nodes_by_id, input_node_id);
    if (!yyjson_is_obj(input_node)) { return NULL; }

    return input_node;
}

static bool
_is_latent_image_creator(const yyjson_val* node
){
    if (!yyjson_is_obj(node)) { return false; }

    const char *class_type = _get_class_type(node);
    if (!class_type) { return false; }

    if (_starts_with(class_type, "EmptyZImageLatentImage")) { return true; }
    if (_str_equals (class_type, "EmptyLatentImage"      )) { return true; }
    return false;
}

static const yyjson_val*
_find_initial_sampler(const yyjson_val* root, PeParams* params) {

    yyjson_obj_iter iter;
    yyjson_obj_iter_init(root, &iter);
    
    yyjson_val *key;
    while ((key = yyjson_obj_iter_next(&iter))) {
        // La clave es el ID del nodo (ej: "3", "4", "KSampler")
        //const char *node_id = yyjson_get_str(key);
        
        // El valor asociado es el objeto del nodo
        yyjson_val *node = yyjson_obj_iter_get_val(key);
        if (!node) { continue; }

        const char* class_type = _get_class_type(node);
        if (!class_type) { continue; }

        if (_starts_with(class_type,"ZSamplerTurbo")) {
            const yyjson_val* latent_node = _get_input_node(node,"latent_input", root);
            if (_is_latent_image_creator(latent_node)) {
                printf("\n");
                printf("###>> FOUND!!! class_type:%s\n", class_type);
                printf("\n");

                const yyjson_val* inputs = pecmf_get_inputs(node);
                if (inputs) {
                    pe_set_yylong  (&params->seed   , inputs, "seed"  );
                    pe_set_yylong  (&params->steps  , inputs, "steps" );
                    pe_set_strfloat(&params->cfg    , "1.0"           );
                    pe_set_string  (&params->sampler, "zsampler_turbo");
                }


                return node;
            }
        }
    }

    return NULL;
}


void parse_comfy_prompt(PeParams* params, PeUTF8 *json_buf, size_t buf_len) {
    // 1. Parseo in situ (modifica el buffer original para evitar allocaciones de strings)
    yyjson_doc *doc = yyjson_read(json_buf, buf_len, YYJSON_READ_INSITU);
    if (!doc) {
        fprintf(stderr, "Error al parsear el JSON\n");
        return;
    }

    // 2. Obtener la raíz (debe ser un objeto que contiene los IDs de nodos como claves)
    yyjson_val *root = yyjson_doc_get_root(doc);
    if (!yyjson_is_obj(root)) {
        fprintf(stderr, "La raíz del prompt no es un objeto JSON válido\n");
        yyjson_doc_free(doc);
        return;
    }


    const yyjson_val* initial_sampler = _find_initial_sampler(root, params);
    (void)initial_sampler;


    // 4. Liberar el documento (no libera el buffer de texto original ya que fue in-situ)
    yyjson_doc_free(doc);
}


/*===========================================================================
|  ////////////////////////// PUBLIC FUNCTIONS ///////////////////////////  | 
===========================================================================*/

bool pe_comfyui_parse(PeParams* params
){
    if (!params) { return false; }

    ComfyUI comfyui = _extract_comfyui_jsons(&params->fileinfo);

    if (!comfyui.workflow || !comfyui.prompt) {
        return false;
    }

    printf("\n");
    printf("##>> workflow: %s\n", comfyui.workflow ? "true" : "false");
    printf("##>> prompt: %s\n", comfyui.prompt ? "true" : "false");
    printf("\n");

    strcpy(params->_storage.ptr, comfyui.prompt);
    parse_comfy_prompt(params, params->_storage.ptr, strlen(params->_storage.ptr));



    return true;
}
