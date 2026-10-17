/*
| File    : prompt_fmt/a1111.h
| Purpose :
| Author  : Martin Rizzo | <martinrizzo@gmail.com>
| Date    : Oct 10, 2026
| Repo    : https://github.com/martin-rizzo/PromptExtractor
| License : MIT
|- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
|                              PromptExtractor
| A lightweight CLI tool to extract embedded prompts from AI-generated images.
\_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/
#ifndef PE__A1111_H__
#define PE__A1111_H__
#include <stdio.h>
#include <stdbool.h>
#include <promptex/params.h>


bool pe_a1111_parse(PeParams* params);



#endif /*PE__A1111_H__*/
