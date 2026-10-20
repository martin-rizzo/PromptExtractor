/*
| File    : prompt_fmt/comfyui_helpers.h
| Purpose : Funciones helpers para manejar JSONs con formato de API de ComfyUI ("prompt")
| Author  : Martin Rizzo | <martinrizzo@gmail.com>
| Date    : Oct 17, 2026
| Repo    : https://github.com/martin-rizzo/PromptExtractor
| License : MIT
|- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
|                              PromptExtractor
| A lightweight CLI tool to extract embedded prompts from AI-generated images.
\_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/
#include "yyjson.h"
#include <promptex/types.h>


yyjson_val* pecmf_get_inputs(yyjson_val* node);

