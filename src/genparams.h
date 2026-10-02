#ifndef __GENPARAMS_H__
#define __GENPARAMS_H__
#include <stdio.h>

typedef char utf8;


typedef struct GenParams GenParams;
struct         GenParams {

    const utf8* prompt;
    const utf8* negative_prompt;
    int         steps;
    int         seed;
    float       cfg;
    const utf8* _data[1];

};


GenParams* read_genparams(const char* filename);

GenParams* read_genparams_from_file(FILE* file);



#endif /*__GENPARAMS_H__*/
