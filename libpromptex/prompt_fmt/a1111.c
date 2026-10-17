/*
| File    : prompt_fmt/a1111.c
| Purpose :
| Author  : Martin Rizzo | <martinrizzo@gmail.com>
| Date    : Oct 10, 2026
| Repo    : https://github.com/martin-rizzo/PromptExtractor
| License : MIT
|- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
|                              PromptExtractor
| A lightweight CLI tool to extract embedded prompts from AI-generated images.
\_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <promptex/types.h>
#include <promptex/params.h>
#include "internal.h"
#include "a1111.h"

// Maximum number of lines supported for A1111 metadata (usually 3)
#define MAX_LINES 10


/*============================ A1111 DETECTION ============================*/

/**
 * Checks if a given text string matches the A1111 metadata format.
 *
 * This function verifies that the text contains essential A1111 markers
 * such as "Steps:", "Sampler:", or "CFG scale:", and ensures structural
 * consistency (e.g., proper placement of "Negative prompt:" and a reasonable
 * number of line breaks).
 *
 * @param text The text string to evaluate.
 * @return
 *     `true` if the text conforms to the A1111 format, `false` otherwise.
 */
static bool
_is_a1111_format(const PeUTF8* text) {
    if (text == NULL || text[0]=='\0')  { return false; }

    // count the number of lines and find the last line
    int num_of_lines = 1;
    const PeUTF8* last_line = text;
    const PeUTF8* ptr = text; while ( (ptr=strchr(ptr, '\n')) != NULL ) {
        last_line = ++ptr; // move past the '\n'
        ++num_of_lines;
    }
    // a valid A1111 text has at least 1 line (parameters only) 
    // and at most around 3 lines: Prompt, Negative, Parameters,
    // though newer versions may include a couple of extra lines;
    // too many lines usually means generic text
    if (num_of_lines > MAX_LINES) { 
        return false;
    }
    // check for mandatory steps, sampler, and CFG in the last line;
    // must have "Steps:" and at least one of the others
    const char* has_steps   = strstr(last_line, "Steps: ");
    const char* has_sampler = strstr(last_line, "Sampler: ");
    const char* has_cfg     = strstr(last_line, "CFG scale: ");
    if (!has_steps || (!has_sampler && !has_cfg)) {
        return false;
    }
    // validate negative prompt consistency;
    // if "Negative prompt:" appears, it must be preceded by a line break
    const char* has_negative = strstr(text, "Negative prompt: ");
    if (has_negative) {
        if (has_negative == text || *(has_negative - 1) != '\n') {
            return false; 
        }
    }
    // if all the above conditions are met, the string is valid
    return true;
}


/**
 * Finds the index of the first text chunk that contains A1111-encoded parameters.
 *
 * Iterates through the provided array of text chunks and returns the index
 * of the first one that matches the A1111 format criteria.
 *
 * @param texts      An array of text chunks to search through.
 * @param text_count The number of text chunks in the array.
 * @return
 *     The index of the first matching text chunk, or -1 if none are found.
 */
static int
_find_a1111_text_idx(PeTextContent* texts,
                     int            text_count
){
    int found_idx = -1;
    for (int i=0 ; i<text_count && found_idx==-1 ; ++i) {
        if (_is_a1111_format(texts[i].content)) {
            found_idx = i;
        }
    }
    return found_idx;
}


/*=========================== HELPER FUNCTIONS ============================*/

struct A1111Param {
    PeUTF8* name;
    PeUTF8* value;
    PeUTF8* next;
};
typedef struct A1111Param A1111Param;

/**
 * Extracts a single parameter from a string in A1111 metadata format.
 *
 * Parses a parameter starting at `ptr`, which typically follows the format
 * "name: value,". The function modifies the string in-place by inserting null
 * terminators to separate the name and value.
 *
 * @param ptr A pointer to the beginning of the parameter string.
 *            The string is modified in-place during parsing.
 * @return
 *     An `A1111Param` structure containing:
 *     - `name` : Pointer to the extracted parameter name.
 *     - `value`: Pointer to the extracted parameter value,
 *                or pointer to empty string if no value is present.
 *     - `next` : Pointer to the start of the next parameter in the string,
 *                or `NULL` if this is the last parameter.
 */
static A1111Param
_extract_a1111_param(PeUTF8* ptr) {

    bool have_more_params = false;
    bool value_is_empty   = false;
    
    A1111Param a1111_param;
    a1111_param.name = ptr;

    // find the first colon (:), comma (,), or end of string (\0)
    while (*ptr != ':' && *ptr != ',' && *ptr != '\0') { ++ptr; }

    // if ':' is NOT found, it ends abruptly with an empty value,
    // containing only the parameter name.
    if (*ptr!=':') {
        assert( *ptr==',' || *ptr=='\0' );
        have_more_params = (*ptr==',');
        value_is_empty   = true;
    }
    // if ':' is found, it marks it as the start of the value
    // and searches for the first comma (,) or end of string (\0)
    else {
        assert( *ptr==':' );
        *ptr++='\0';
        a1111_param.value = ptr;
        while (*ptr != ',' && *ptr != '\0') { ++ptr; }
        have_more_params = (*ptr==',');
        value_is_empty   = false;
    }
  
    *ptr = '\0';
    if (value_is_empty) { a1111_param.value = ptr; }
    a1111_param.next = (have_more_params ? ptr+1 : NULL);
    return a1111_param;
}

/**
 * Parses a string in "WxH" format and assigns the width and height values.
 *
 * This function takes a string containing dimensions separated by 'x'
 * (e.g., "1024x768"), splits it in-place, and sets the provided
 * `width` and `height` parameters using `pe_set_long`.
 * The function is generic enough to support other separators as well
 * (e.g., "1024,768" is also valid).
 *
 * @param width    A PeLong struct to store the parsed width value.
 * @param height   A PeLong struct to store the parsed height value.
 * @param strvalue String containing the dimensions in "WxH" format.
 */
static void
_set_wxh(PeLong* width, PeLong* height, PeUTF8* strvalue) {
    assert( width!=NULL );
    assert( height!=NULL );
    if (strvalue==NULL) { return; }
    PeUTF8* str_width  = strvalue;
    PeUTF8* str_height = strvalue;
    
    // iterates through the first sequence of digits and takes the
    // first non-digit character as the separator
    PeUTF8* ptr  = strvalue;
    while( '0'<=*ptr && *ptr<='9' ) { ++ptr; }
    if (*ptr!='\0') {
        *ptr = '\0';
        str_height = ++ptr;
    }

    pe_set_long(width , str_width);
    pe_set_long(height, str_height);
}


/*========================== SETTING PARAMETERS ===========================*/

// Parameter type in A1111 metadata
enum PTYPE {
    PTYPE_TEXTLINE, ///< Comes from a complete, independent text line (e.g., Prompt, Negative Prompt)
    PTYPE_CSV       ///< Comes from a comma-separated argument within the last line (e.g., Seed, CFG scale, Sampler)
};
typedef enum PTYPE PTYPE;


/**
 * Sets a parameter in the `PeParams` structure based on its name and value.
 *
 * This function maps a given parameter name (e.g., "Steps", "CFG scale", "Prompt")
 * to the corresponding field within the provided `PeParams` structure. It handles
 * various data types, including strings, integers, floats, and width/height pairs.
 *
 * @param p      Pointer to the `PeParams` structure where the parameter will be set.
 * @param name   The name of the parameter to set (e.g., "Steps", "Seed").
 * @param value  The string value associated with the parameter.
 * @param ptype  The type of the parameter (e.g., text line or CSV argument).
 * @return
 *     `true` if the parameter was successfully identified and set,
 *     `false` otherwise.
 */
static bool
_set_param(PeParams* p, PeUTF8* name, PeUTF8* value, PTYPE ptype
){
#define IF_NAME(param_name)   if( 0==strcmp(param_name,name) )
#define ELIF_NAME(param_name) else IF_NAME(param_name)

    assert(p!=NULL);
    assert(name!=NULL);
    assert(value!=NULL);
    unused(ptype);
    
      IF_NAME("Prompt"            ) pe_set_string(&p->prompt            , value);
    ELIF_NAME("Negative prompt"   ) pe_set_string(&p->negative_prompt   , value);
    ELIF_NAME("Wildcard prompt"   ) pe_set_string(&p->wildcard_prompt   , value);
    ELIF_NAME("Model"             ) pe_set_string(&p->model.name        , value);
    ELIF_NAME("Model hash"        ) pe_set_string(&p->model.hash        , value);
    ELIF_NAME("Sampler"           ) pe_set_string(&p->sampler           , value);
    ELIF_NAME("Steps"             ) pe_set_long  (&p->steps             , value);
    ELIF_NAME("CFG scale"         ) pe_set_float (&p->cfg               , value);
    ELIF_NAME("Seed"              ) pe_set_long  (&p->seed              , value);
    ELIF_NAME("Denoising strength") pe_set_float (&p->denoising         , value);
    ELIF_NAME("Hires upscaler"    ) pe_set_string(&p->hires.upscaler    , value);
    ELIF_NAME("Hires steps"       ) pe_set_long  (&p->hires.steps       , value);
    ELIF_NAME("Hires upscale"     ) pe_set_float (&p->hires.upscale     , value);
    ELIF_NAME("Mask blur"         ) pe_set_float (&p->inpaint.mask_blur , value);
    ELIF_NAME("Eta"               ) pe_set_float (&p->settings.eta      , value);
    ELIF_NAME("ENSD"              ) pe_set_long  (&p->settings.ensd     , value);
    ELIF_NAME("Clip skip"         ) pe_set_long  (&p->settings.clip_skip, value);
    ELIF_NAME("Size"              ) _set_wxh(&p->width      , &p->height      , value);
    ELIF_NAME("Hires resize"      ) _set_wxh(&p->hires.width, &p->hires.height, value);
    /*
    else {
        pe_add_unknown_param(&p->unknowns, name, value);
    }
    */
    else { return false; }
    return true;

#undef IF_NAME
#undef ELIF_NAME
}


/**
 * Parses an array of text lines and populates the corresponding parameters.
 *
 * This function processes each line in the provided array, extracting parameter
 * names and values, and updates the `params` structure accordingly. It handles
 * various line formats, including standalone text lines (e.g., prompts) and
 * comma-separated key-value pairs.
 *
 * @param params      Pointer to the `PeParams` structure to be populated.
 * @param lines       An array of text line strings to parse.
 * @param lines_count The number of lines in the `lines` array.
 * @return
 *     The total number of parameters successfully extracted and
 *     set in the `params` structure.
 */
static int
_set_params_from_lines(PeParams* params,
                       PeUTF8*   lines[],
                       int       lines_count
){
    assert( params!=NULL );
    if (lines==NULL || lines_count<=0) { return false; }

    bool worked;
    int num_of_params = 0;

    // determine which is the prompt line and which is the parameters line;
    // typically the prompt is the first line and the parameters are
    // in the last line, but if there is only a single line of text, the
    // situation becomes a bit ambiguous.
    PeUTF8* prompt_line     = lines[0];
    PeUTF8* parameters_line = lines[lines_count-1];
    if (lines_count==1) {
        prompt_line     = NULL;
        parameters_line = lines[0];
    }

    // if the prompt line is found, the entire line
    // is assigned directly to the "Prompt" parameter.
    if (prompt_line) {
        worked = _set_param(params,
                            "Prompt",
                            pe_strip_inplace(prompt_line,0),
                            PTYPE_TEXTLINE);
        if (worked) { ++num_of_params; }
    }

    // if the line with comma-separated parameters is found,
    // parse each of those parameters one by one in the "Name : Value," format.
    if (parameters_line) {
        PeUTF8* ptr = parameters_line;
        while (ptr!=NULL) {
            A1111Param a1111_param = _extract_a1111_param(ptr);
            if (a1111_param.name[0]!='\0' && a1111_param.value!=NULL) {
                worked = _set_param(params,
                                    pe_strip_inplace(a1111_param.name , 0),
                                    pe_strip_inplace(a1111_param.value, 0),
                                    PTYPE_CSV);
                if (worked) { ++num_of_params; }
            }
            ptr = a1111_param.next;
        }
    }

    // for lines from the second (index=1) to the second-to-last (lines_count-1),
    // they should all be in the "Name : Value" format.
    for (int i = 1; i < lines_count - 1; i++) {
        PeUTF8* line_param = lines[i];
        PeUTF8* separator  = strchr(line_param, ':');
        if (separator) {
            *separator = '\0';
            worked = _set_param(params,
                                pe_strip_inplace(line_param ,separator-line_param),
                                pe_strip_inplace(separator+1, 0),
                                PTYPE_TEXTLINE);
            if (worked) { ++num_of_params; }
        }
    }
    return num_of_params;
}


/*===========================================================================
|  ////////////////////////// PUBLIC FUNCTIONS ///////////////////////////  | 
===========================================================================*/


bool pe_a1111_parse(PeParams* params
){
    PeFileInfo* fileinfo = &params->fileinfo;

    // check each extracted text from the file (fileinfo->texts[n])
    // to see if any matches the A1111 format.
    int text_idx = _find_a1111_text_idx(fileinfo->texts, fileinfo->texts_count);
    if (text_idx<0) { return false; }

    // use `_storage` as the buffer for raw_text;
    // `_storage` is guaranteed to be large enough to
    // hold any of the texts stored in `fileinfo->texts[n]`
    PeUTF8* raw_text = params->_storage.ptr;
    memcpy(raw_text,
           fileinfo->texts[text_idx].content,
           fileinfo->texts[text_idx].content_size);

    // separate raw_text into text lines
    PeUTF8* lines[MAX_LINES];
    int line_count = pe_split_lines(raw_text, lines, MAX_LINES);
    if (line_count<=0 || line_count>MAX_LINES) {
        return false;
    }

    // set each of the parameters found in the text lines
    return _set_params_from_lines(params, lines, line_count);
}
