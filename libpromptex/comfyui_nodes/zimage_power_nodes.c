/*
| File    : comfy_nodes/zimage_power_nodes.c
| Purpose : Simulates the behavior of some "ComfyUI-ZImagePowerNodes" nodes
| Author  : Martin Rizzo | <martinrizzo@gmail.com>
| Date    : Oct 20, 2026
| Repo    : https://github.com/martin-rizzo/PromptExtractor
| License : MIT
|- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
|                              PromptExtractor
| A lightweight CLI tool to extract embedded prompts from AI-generated images.
\_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/
#include <assert.h>
#include <promptex/types.h>
#include "internal.h"
#include "comfyui_nodes/zimage_power_nodes.h"
typedef struct { double width, height; } RectSize;


/*------------------------- LANSCAPE SIZES TABLE --------------------------*/

typedef struct {
    const PeUTF8*  name;
    const RectSize rectsize;
} LandscapeSize;

static const LandscapeSize LANDSCAPE_SIZES_BY_ASPECT_RATIO[] = { 
    { "1:1  (square)"      , {1024.0, 1024.0} }, // Social media posts and profile pictures
    { "4:3  (retro tv)"    , {1182.4,  886.8} }, // Legacy television and older computer monitors
    { "3:2  (photo)"       , {1252.8,  837.0} }, // DSLR cameras and standard 35mm film # (1254.1, 836.1)
    { "16:10  (monitor)"   , {1295.3,  809.5} }, // Common in MacBooks and productivity laptops
    { "16:9  (widescreen)" , {1365.3,  768.0} }, // Current universal standard for video and TV
    { "2:1  (univisium)"   , {1448.2,  724.0} }, // Modern streaming series and smartphone screens
    { "21:9  (ultrawide)"  , {1564.2,  670.4} }, // Wide cinema format and ultrawide monitors
    { "12:5  (anamorphic)" , {1586.4,  661.0} }, // Standard theatrical widescreen cinema release
    { "70:27  (cinerama)"  , {1648.8,  636.0} }, // Extreme panoramic cinema format
    { "32:9  (super wide)" , {1930.9,  543.0} }, // Dual-monitor width for ultra-wide displays
  //{ "48:35  (35 mm)"     , {1199.2,  874.4} }, // ??
  //{ "71:50  (~imax)"     , {1220.2,  859.3} }, // ??
    { NULL                 , {   0.0,    0.0} }  // End of list
};

RectSize
_get_landscape_size(const PeUTF8* name, RectSize default_) {
    for (const LandscapeSize* item = LANDSCAPE_SIZES_BY_ASPECT_RATIO; item->name; ++item) {
        if (PeUTF8_equals(name, item->name)) { return item->rectsize; }
    }
    return default_;
}

/*----------------------------- SCALES TABLE ------------------------------*/

typedef struct {
    const char* name;
    double      scale;
} Scale;

static const Scale SCALES_BY_NAME[] = {
    { "small"                , 1.0},
    { "medium (recommended)" , 1.3},
    { "large"                , 1.6},
    { NULL                   , 0.0}  // End of list
};

double
_get_scale(const PeUTF8* name, float default_) {
    for (const Scale* item = SCALES_BY_NAME; item->name; ++item) {
        if (PeUTF8_equals(name, item->name)) { return item->scale; }
    }
    return default_;
}


/*===========================================================================
|  ////////////////////////// PUBLIC FUNCTIONS ///////////////////////////  | 
===========================================================================*/

// Simulates the calculation performed in the "Empty Z-Image Latent Image" node.
PeImageSize pecmf_zipn_calc_image_size(bool          orientation,
                                       const PeUTF8* ratio,
                                       const PeUTF8* size
){
    static const RectSize DEFAULT_BASE_SIZE = { 1024.0, 1024.0 };
    if (ratio==NULL || size==NULL) { return EMPTY_IMAGE_SIZE; }

    bool     is_vertical    = (orientation == false);
    double   scale          = _get_scale(size, 1.0f);
    RectSize landscape_size = _get_landscape_size(ratio, DEFAULT_BASE_SIZE);

    // discard any null or negative scale to prevent errors,
    // as the returned size is always unsigned
    if (scale<=0.0) { return EMPTY_IMAGE_SIZE; }

    // calculate the desired dimensions in landscape mode
    unsigned desired_width  = (unsigned)(landscape_size.width  * scale);
    unsigned desired_height = (unsigned)(landscape_size.height * scale);

    // swap width and height if portrait/vertical orientation is requested
    if (is_vertical) {
        unsigned temp=desired_width; desired_width=desired_height; desired_height=temp;
    }

    // fix image size to be divisible by the grid
    const int         GRID_SIZE  = 32;
    const PeImageSize image_size = {
        (desired_width  / GRID_SIZE) * GRID_SIZE,
        (desired_height / GRID_SIZE) * GRID_SIZE
    };
    return image_size;
}
