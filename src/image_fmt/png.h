/*
| File    : image_fmt/png.h
| Purpose : Read a PNG file and extract text chunks.
| Author  : Martin Rizzo | <martinrizzo@gmail.com>
| Date    : Oct 4, 2026
| Repo    : https://github.com/martin-rizzo/ReadPrompt
| License : MIT
|- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
|                                ReadPrompt
| A lightweight CLI tool to extract embedded prompts from AI-generated images.
\_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/
#ifndef RP__PNG_H__
#define RP__PNG_H__
#include <stdio.h>
#include "embstrings.h"


RpEmbStrings* RP_embstrings_from_png( FILE *file );



#endif /*RP__PNG_H__*/
