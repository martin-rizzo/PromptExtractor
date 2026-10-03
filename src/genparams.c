#include "genparams.h"
#include <stdlib.h>



static
const GenParams* genparams_empty() {
    GenParams* params = malloc(sizeof(GenParams));
    params->prompt          = "";
    params->negative_prompt = "";
    params->steps           = 0;
    params->seed            = 0;
    params->cfg             = 0.0f;
    params->_data[0]        = 0;
    return params;
}


/**
 * Reads generation parameters from a given file handle.
 *
 * @param file   The file handle to read from.
 * @param report A callback function for reporting errors or warnings.
 * @return A pointer to the populated GenParams structure.
 */
const GenParams* read_genparams_from_file(FILE* file, ReportFn report) {
    if (!file) {
        report(REPORT_ERROR, "File handle is NULL");
        return genparams_empty();
    }
    // NO IMPLEMENTAR TODAVIA
    return genparams_empty();
}


/**
 * Reads generation parameters from a file specified by its path.
 *
 * @param filename The path to the file to read.
 * @param report   A callback function for reporting errors or warnings.
 * @return A pointer to the populated GenParams structure.
 */
const GenParams* read_genparams(const char* filename, ReportFn report) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        report(REPORT_ERROR, "Failed to open file");
        return genparams_empty();
    }
    const GenParams* params = read_genparams_from_file(file, report);
    fclose(file);
    return params;
}
