#ifndef __GENPARAMS_H__
#define __GENPARAMS_H__
#include <stdio.h>
#include <stdbool.h>
typedef char utf8;


struct GenParams {

    const utf8* prompt;
    const utf8* negative_prompt;
    int         steps;
    int         seed;
    float       cfg;
    const utf8* _data[1];

};
typedef struct GenParams GenParams;


enum REPORT_TYPE {

    REPORT_ERROR,
    REPORT_WARNING

};
typedef enum REPORT_TYPE REPORT_TYPE;


typedef void (ReportFn)(REPORT_TYPE type, const utf8* message);


const GenParams* read_genparams(const char* filename,
                                ReportFn    report
                                );

const GenParams* read_genparams_from_file(FILE*    file,
                                          ReportFn report
                                          );

#define free_genparams(x) free((void*)x)


#endif /*__GENPARAMS_H__*/
