/*
| File    : prompt_fmt/comfyui_helpers.c
| Purpose : Helper functions for handling ComfyUI API format JSONs ("prompt")
| Author  : Martin Rizzo | <martinrizzo@gmail.com>
| Date    : Oct 17, 2026
| Repo    : https://github.com/martin-rizzo/PromptExtractor
| License : MIT
|- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
|                              PromptExtractor
| A lightweight CLI tool to extract embedded prompts from AI-generated images.
\_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/
#include <assert.h>
#include <promptex/types.h>
#include "internal.h"
#include "yyjson.h"
#include "comfyui_nodes/zimage_power_nodes.h"
#include "comfyui_helpers.h"


// Returns the unique class type identifier of a node.
const PeUTF8* pecmf_get_classtype(const yyjson_val* node
){
    return yyjson_get_str( yyjson_obj_get(node, "class_type") );
}


// Returns the inputs object of a node.
const yyjson_val* pecmf_get_inputs(const yyjson_val* node
){
    yyjson_val* inputs = yyjson_obj_get(node, "inputs");
    return yyjson_is_obj(inputs) ? inputs : NULL;
}


// Returns the node connected to a specific input.
yyjson_val* pecmf_get_connected_node(const yyjson_val* inputs,
                                     const PeUTF8*     input_name,
                                     const yyjson_val* nodes_by_id
){
    // like "yyjson", this function handles NULL inputs gracefully
    if (inputs==NULL || input_name==NULL || nodes_by_id==NULL) {
        return NULL;
    }
    yyjson_val*   input_node_id_val = yyjson_arr_get(yyjson_obj_get(inputs, input_name), 0);
    const PeUTF8* input_node_id     = yyjson_get_str(input_node_id_val);
    yyjson_val*   input_node        = yyjson_obj_get(nodes_by_id, input_node_id);
    return input_node;
}


/**
 * Finds the image size of an empty latent node connected to a specified input.
 *
 * This function examines the specified input to locate a connected node that
 * generates empty latents. Upon finding such a node, it extracts and returns
 * the image dimensions associated with that empty latent.
 *
 * This function is useful for determining the overall image size generated
 * by a ComfyUI workflow.
 *
 * @param inputs      The inputs object of the current node.
 * @param input_name  The name of the input to check for a connected empty latent node.
 * @param nodes_by_id A JSON object mapping node IDs to their corresponding node object.
 * @return The image size of the connected empty latent node.
 *         Returns `EMPTY_IMAGE_SIZE` ({0, 0}) if no suitable connected node is
 *         found or if the node data is invalid.
 */
PeImageSize pecmf_find_image_size(const yyjson_val* inputs,
                                  const PeUTF8*     input_name,
                                  const yyjson_val* nodes_by_id
){
    PeImageSize result = EMPTY_IMAGE_SIZE;

    yyjson_val* node = pecmf_get_connected_node(inputs, input_name, nodes_by_id);
    if (!node) { return EMPTY_IMAGE_SIZE; }

    const PeUTF8* classtype = pecmf_get_classtype(node);
    if (!classtype) { return EMPTY_IMAGE_SIZE; }

    const yyjson_val* node_inputs = pecmf_get_inputs(node);
    if (!node_inputs) { return EMPTY_IMAGE_SIZE; }

    // handle custom "Empty Z-Image Latent Image" node
    // - https://github.com/martin-rizzo/ComfyUI-ZImagePowerNodes
    if (PeUTF8_startswith(classtype,"EmptyZImageLatentImage"))
    {
        const bool    orientation = yyjson_get_bool( yyjson_obj_get(node_inputs, "orientation") );
        const PeUTF8* ratio       = yyjson_get_str ( yyjson_obj_get(node_inputs, "ratio"      ) );
        const PeUTF8* size        = yyjson_get_str ( yyjson_obj_get(node_inputs, "size"       ) );
        if (ratio==NULL || size==NULL) { return EMPTY_IMAGE_SIZE; }
        result = pecmf_zipn_calc_image_size(orientation, ratio, size);
    }

    // handle native comfyui "Empty Latent Image" node
    else if (PeUTF8_equals(classtype,"EmptyLatentImage"))
    {
        const double width  = yyjson_get_num( yyjson_obj_get(node_inputs, "width" ) );
        const double height = yyjson_get_num( yyjson_obj_get(node_inputs, "height") );
        if (width<=0.0 || height<=0.0) { return EMPTY_IMAGE_SIZE; }
        result.width  = (unsigned)width;
        result.height = (unsigned)height;
    }

    return result;
}


// Finds a valid string by traversing the input connections of nodes starting from a specified input.
const PeUTF8* _pecmf_find_string_r_(const yyjson_val* inputs,
                                    const PeUTF8*     input_name,
                                    const PeUTF8*     search_routes[],
                                    const yyjson_val* nodes_by_id,
                                    int               recursive_depth
){
    // like "yyjson", this function handles NULL inputs gracefully
    if (inputs==NULL || nodes_by_id==NULL) {
        return NULL;
    }
    // prevents infinite recursion
    if (recursive_depth>10) {
        return NULL;
    }
    
    const yyjson_val* node_inputs  = inputs;
    const PeUTF8*     found_string = NULL;

    // if `input_name` is specified, first try routing the search through that input
    if (input_name) {
        yyjson_val* input_val = yyjson_obj_get(inputs, input_name);

        // if the input is a direct string, return it immediately
        if (yyjson_is_str(input_val)) {
            return yyjson_get_str(input_val);
        }
        // if the input is a connection to another node,
        // traverse to that node and update `node_inputs`
        else if (yyjson_is_arr(input_val)) {
            yyjson_val* node = pecmf_get_connected_node(inputs, input_name, nodes_by_id);
            node_inputs = pecmf_get_inputs(node);
            if (!node_inputs) { return NULL; }
        }
        // if the input is of any other type,
        // assume an error and return NULL
        else {
            return NULL;
        }
    }

    // if the string hasn't been resolved yet and `search_routes`
    // are specified, then iterate through each route name and attempt
    // to continue the search from there
    if (!found_string && search_routes) {
        for (const PeUTF8** name=search_routes ; name!=NULL && !found_string ; ++name) {
            found_string = _pecmf_find_string_r_(node_inputs,
                                                 *name,
                                                 search_routes,
                                                 nodes_by_id,
                                                 recursive_depth+1);
        }
    }
    return found_string;
}
