/*
| File    : image_fmt/jpeg.h
| Purpose : Read a JPEG file and extract text chunks.
| Author  : Martin Rizzo | <martinrizzo@gmail.com>
| Date    : Oct 5, 2026
| Repo    : https://github.com/martin-rizzo/ReadPrompt
| License : MIT
|- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
|                                ReadPrompt
| A lightweight CLI tool to extract embedded prompts from AI-generated images.
\_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/
#ifndef RP__JPEG_H__
#define RP__JPEG_H__
#include <stdio.h>
#include "embstrings.h"


RpEmbStrings* rp_embstrings_from_jpeg( FILE* file );


#endif /*RP__JPEG_H__*/
