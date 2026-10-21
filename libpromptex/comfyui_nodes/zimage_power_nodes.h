/*
| File    : comfy_nodes/zimage_power_nodes.h
| Purpose : Simulates the behavior of some "ComfyUI-ZImagePowerNodes" nodes
| Author  : Martin Rizzo | <martinrizzo@gmail.com>
| Date    : Oct 20, 2026
| Repo    : https://github.com/martin-rizzo/PromptExtractor
| License : MIT
|- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
|                              PromptExtractor
| A lightweight CLI tool to extract embedded prompts from AI-generated images.
\_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/
#include <promptex/types.h>
#include "yyjson.h"
#ifndef PE__ZIMAGE_POWER_NODES_H__
#define PE__ZIMAGE_POWER_NODES_H__


/**
 * Calculates the image size based on orientation, aspect ratio, and scale size.
 *
 * Simulates the calculation performed in the "Empty Z-Image Latent Image" node.
 * It determines the base dimensions from the given aspect ratio, applies the
 * specified scale, and adjusts the dimensions for portrait/vertical orientation
 * if needed. Finally, it snaps the dimensions to the nearest multiple of the
 * grid size (32 pixels).
 *
 * @param orientation If `true`, the image is in landscape mode.
 *                    If `false`, it is in portrait (vertical) mode.
 * @param ratio       The aspect ratio string (e.g., "1:1 (square)", "16:9 (widescreen)").
 * @param size        The scale size string (e.g., "small", "medium (recommended)", "large").
 * @return
 *    A `PeImageSize` structure containing the calculated width and height,
 *    or EMPTY_IMAGE_SIZE ({0, 0}) in case of an error.
 *
 * @see Original Python implementation:
 *      https://github.com/martin-rizzo/ComfyUI-ZImagePowerNodes/blob/v2.1.2/nodes/empty_zimage_latent_image.py#L125
 */
PeImageSize pecmf_zipn_calc_image_size(bool          orientation,
                                       const PeUTF8* ratio,
                                       const PeUTF8* size);


                                       
#endif /*PE__ZIMAGE_POWER_NODES_H__*/
