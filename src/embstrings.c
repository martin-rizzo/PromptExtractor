/*
| File    : embstrings.c
| Purpose : Manage embedded strings and named strings.
| Author  : Martin Rizzo | <martinrizzo@gmail.com>
| Date    : Oct 4, 2026
| Repo    : https://github.com/martin-rizzo/ReadPrompt
| License : MIT
|- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
|                                ReadPrompt
| A lightweight CLI tool to extract embedded prompts from AI-generated images.
\_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/
#include <stdlib.h>
#include <string.h>
#include "embstrings.h"
#define NamedStrPointer RpNamedStr*


// Duplicates a string, allocating memory for the copy.
char *rp_strdup(const char *s) {
    if (!s) { return NULL; }
    size_t total_bytes = strlen(s) + 1;
    char   *newstring  = malloc(total_bytes);
    if (newstring) { memcpy(newstring, s, total_bytes); }
    return newstring;
}


/*============================ NamedStr STRUCT ============================*/

// Creates a new RpNamedStr structure with copies of the provided name and value.
RpNamedStr* rp_create_namedstr(const RpUTF8* name,
                               const RpUTF8* value,
                               RpNamedStr*   chain)
{
    return rp_attach_namedstr(strdup(name), strdup(value), chain);
}


// Creates a new RpNamedStr that takes ownership of the provided name and value.
RpNamedStr* rp_attach_namedstr(const RpUTF8* name_in_heap,
                               const RpUTF8* value_in_heap,
                               RpNamedStr*   chain)
{
    RpNamedStr* namedstr = malloc(sizeof(RpNamedStr));
    namedstr->name  = name_in_heap;
    namedstr->value = value_in_heap;
    namedstr->_next = chain;
    return namedstr;
}


// Destroys a single RpNamedStr node and frees its associated resources.
const RpNamedStr* rp_destroy_namedstr(const RpNamedStr* namedstr)
{
    if( !namedstr ) { return NULL; }
    RpNamedStr* next_in_chain = namedstr->_next;
    free( (void*)namedstr->name  );
    free( (void*)namedstr->value );
    free( (void*)namedstr );
    return next_in_chain;
}


// Destroys an entire chain of RpNamedStr nodes.
void rp_destroy_namedstr_chain(const RpNamedStr* namedstr)
{
    while (namedstr!=NULL) {
        namedstr = rp_destroy_namedstr(namedstr);
    }
}


/*========================== RpEmbStrings STRUCT ==========================*/

// Creates a new RpEmbStrings structure taking ownership of the string chain.
RpEmbStrings* rp_attach_embstrings(RpNamedStr* chain_in_heap)
{
    RpNamedStr* current;
    RpEmbStrings* embstrings = malloc(sizeof(RpEmbStrings));

    // count the number of elements in the chain
    current = chain_in_heap;
    int count = 0; while (current != NULL) {
        count++;
        current = current->_next;
    }

    // initialize RpEmbStrings and allocate space for 'items' to hold all chain elements
    embstrings->items  = malloc(sizeof(NamedStrPointer) * count);
    embstrings->count  = count;
    embstrings->_chain = chain_in_heap;

    // populate the 'items' array with each element from the chain
    current = chain_in_heap;
    int idx = count; while (current != NULL) {
        embstrings->items[--idx] = current;
        current = current->_next;
    }

    return embstrings;
}


// Destroys an RpEmbStrings structure and frees its associated resources.
void rp_destroy_embstrings(const RpEmbStrings* embstrings)
{
    if( !embstrings ) { return; }
    
    // free the array of pointers and the RpEmbStrings itself
    rp_destroy_namedstr_chain(embstrings->_chain);
    free((void*)embstrings->items);
    free((void*)embstrings);
}
