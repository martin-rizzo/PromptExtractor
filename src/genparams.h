#ifndef __GENPARAMS_H__
#define __GENPARAMS_H__
#include <stdio.h>
#include <stdbool.h>
#include "types.h"


struct GenParams {

    const RpUTF8* prompt;
    const RpUTF8* negative_prompt;
    int           steps;
    int           seed;
    float         cfg;
    const RpBYTE* _data[1];

};
typedef struct GenParams GenParams;
#define free_genparams(x) free((void*)x)



enum REPORT_TYPE {

    REPORT_ERROR,
    REPORT_WARNING

};
typedef enum REPORT_TYPE REPORT_TYPE;


typedef void (ReportFn)(REPORT_TYPE type, const RpUTF8* message);


const GenParams* read_genparams(const char* filename,
                                ReportFn    report
                                );

const GenParams* read_genparams_from_file(FILE*    file,
                                          ReportFn report
                                          );



#endif /*__GENPARAMS_H__*/
