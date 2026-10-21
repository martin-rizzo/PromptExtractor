/*
| File    : prompt_fmt/comfyui.h
| Purpose : Parse ComfyUI prompt and workflow JSONs to extract generation parameters
| Author  : Martin Rizzo | <martinrizzo@gmail.com>
| Date    : Oct 14, 2026
| Repo    : https://github.com/martin-rizzo/PromptExtractor
| License : MIT
|- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
|                              PromptExtractor
| A lightweight CLI tool to extract embedded prompts from AI-generated images.
\_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/
#ifndef PE__COMFYUI_H__
#define PE__COMFYUI_H__
#include <stdio.h>
#include <stdbool.h>
#include <promptex/params.h>


bool pe_comfyui_parse(PeParams*      out_params,
                      PeTextContent* texts,
                      int            text_count);



#endif /*PE__COMFYUI_H__*/
