/*
| File    : image_fmt/jpeg.h
| Purpose : Read a JPEG file and extract text chunks.
| Author  : Martin Rizzo | <martinrizzo@gmail.com>
| Date    : Oct 5, 2026
| Repo    : https://github.com/martin-rizzo/PromptExtractor
| License : MIT
|- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
|                              PromptExtractor
| A lightweight CLI tool to extract embedded prompts from AI-generated images.
\_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/
#ifndef PE__JPEG_H__
#define PE__JPEG_H__
#include <stdio.h>
#include "embstrings.h"


PeEmbStrings* pe_embstrings_from_jpeg( FILE* file );


#endif /*PE__JPEG_H__*/
