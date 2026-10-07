/*
| File    : image_fmt/png.h
| Purpose : Read a PNG file and extract text chunks.
| Author  : Martin Rizzo | <martinrizzo@gmail.com>
| Date    : Oct 4, 2026
| Repo    : https://github.com/martin-rizzo/PromptExtractor
| License : MIT
|- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
|                              PromptExtractor
| A lightweight CLI tool to extract embedded prompts from AI-generated images.
\_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/
#ifndef PE__PNG_H__
#define PE__PNG_H__
#include <stdio.h>
#include "embstrings.h"


PeEmbStrings* pe_embstrings_from_png( FILE *file );



#endif /*PE__PNG_H__*/
