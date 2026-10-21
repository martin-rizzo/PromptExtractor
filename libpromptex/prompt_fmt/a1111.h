/*
| File    : prompt_fmt/a1111.h
| Purpose : Parses A1111 metadata from the text chunks extracted from the image file (.PNG, .JPG, etc.)
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


/**
 * Parses A1111 metadata from extracted text chunks and populates the parameters.
 *
 * This function identifies the first text chunk that conforms to the A1111
 * metadata format, extracts its content, and populates the provided
 * `PeParams` structure with the corresponding parameter values.
 *
 * @param out_params  Pointer to the `PeParams` structure where the parsed parameters will be stored.
 * @param texts       An array of `PeTextContent` structures containing the extracted text chunks from file.
 * @param text_count  The number of elements in the `texts` array.
 * @return
 *     `true` if the parameters were successfully parsed and set,
 *     `false` otherwise.
 */
bool pe_a1111_parse(PeParams*      out_params,
                    PeTextContent* texts,
                    int            text_count);



#endif /*PE__A1111_H__*/
