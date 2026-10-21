/*
| File    : prompt_fmt/comfyui_helpers.h
| Purpose : Helper functions for handling ComfyUI API format JSONs ("prompt")
| Author  : Martin Rizzo | <martinrizzo@gmail.com>
| Date    : Oct 17, 2026
| Repo    : https://github.com/martin-rizzo/PromptExtractor
| License : MIT
|- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
|                              PromptExtractor
| A lightweight CLI tool to extract embedded prompts from AI-generated images.
\_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/
#ifndef PE__COMFYUI_HELPERS_H__
#define PE__COMFYUI_HELPERS_H__
#include "yyjson.h"
#include <promptex/types.h>


/**
 * Returns the unique class type identifier of a node.
 * @param node  The node object to inspect (must be a valid JSON object).
 * @return
 *     The class type string, or NULL if the node is invalid.
 */
const PeUTF8* pecmf_get_classtype(const yyjson_val* node);


/**
 * Returns the inputs object of a node.
 *
 * Each input within the returned object can either hold a direct value
 * or a reference (connection) to another node.
 *
 * @param node  The node object to inspect (must be a valid JSON object).
 * @return
 *     The inputs object, or NULL if the node is invalid.
 */
const yyjson_val* pecmf_get_inputs(const yyjson_val* node);


/**
 * Returns the node connected to a specific input.
 *
 * @param inputs       The inputs object of the current node (must be a valid JSON object).
 * @param input_name   The name of the input to check.
 * @param nodes_by_id  A JSON object mapping node IDs to their corresponding node objects.
 * @return
 *     The connected node value, or NULL if the connection is not found.
 */
yyjson_val* pecmf_get_connected_node(const yyjson_val* inputs,
                                     const PeUTF8*     input_name,
                                     const yyjson_val* nodes_by_id);


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
                                  const yyjson_val* nodes_by_id);


/** Internal helper function to recursively find a string value. */
const PeUTF8* _pecmf_find_string_r_(const yyjson_val* inputs,
                                    const PeUTF8*     input_name,
                                    const PeUTF8*     route_input_names[],
                                    const yyjson_val* nodes_by_id,
                                    int               recursive_depth);


/**
 * Finds a valid string by traversing the input connections of nodes starting from a specified input.
 *
 * This function recursively traverses the input connections of ComfyUI nodes
 * to locate a valid string value. It starts from the given `inputs` object
 * and follows the connection specified by `input_name`. If it's a connection
 * to another node, it continues the search exploring inputs with the names
 * specified in `search_routes`.
 *
 * @param inputs          The JSON object representing the inputs of the current node.
 * @param input_name      The name of the input to start the search from. If NULL,
 *                        the function proceeds directly to the search routes.
 * @param search_routes   An array of input names to try when searching for the string.
 *                        The last element of the array must be NULL to mark the end.
 *                        The `search_routes` pointer can be NULL to skip routing.
 * @param nodes_by_id     A JSON object mapping node IDs to their corresponding node objects.
 * @param recursive_depth The current depth of recursion to prevent infinite loops.
 * @return
 *     A pointer to the found string,
 *     or NULL if no valid string is found or if an error occurs.
 */
static inline
const PeUTF8* pecmf_find_string(const yyjson_val* inputs,
                                const PeUTF8*     input_name,
                                const PeUTF8*     route_input_names[],
                                const yyjson_val* nodes_by_id
){ return _pecmf_find_string_r_(inputs, input_name, route_input_names, nodes_by_id, 0); }



#endif /*PE__COMFYUI_HELPERS_H__*/
