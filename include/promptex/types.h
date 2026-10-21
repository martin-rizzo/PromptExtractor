/*
| File    : types.h
| Purpose : Simple types used in the project.
| Author  : Martin Rizzo | <martinrizzo@gmail.com>
| Date    : Oct 2, 2026
| Repo    : https://github.com/martin-rizzo/PromptExtractor
| License : MIT
|- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
|                              PromptExtractor
| A lightweight CLI tool to extract embedded prompts from AI-generated images.
\_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/
#ifndef PE__TYPES_H__
#define PE__TYPES_H__
#include <stdbool.h>

/**
 * Maximum number of text chunks that can be analyzed from an image file.
 *
 * Defines the upper limit for the number of text contents or chunks
 * that the extractor can process from a single image file (e.g., JPEG, PNG).
 */
#define PE_MAX_TEXTCHUNK_COUNT 4

/**
 * Maximum size in bytes for a single text chunk.
 *
 * Acts as a safety limit to prevent memory allocation issues.
 * The value is 6 MB (6 * 1024 * 1024 bytes).
 */
#define PE_MAX_TEXTCHUNK_SIZE (6*1024*1024)


/*============================== BASIC TYPES ==============================*/

/**
 * Represents a basic UTF-8 character unit.
 */
typedef char PeUTF8;


/**
 * Represents a byte with a value range from 0 to 255.
 */
typedef unsigned char PeBYTE;


/**
 * Represents the size of an image.
 */
struct PeImageSize {
    unsigned width;
    unsigned height;
};
typedef struct PeImageSize PeImageSize;


/**
 * An optional string value.
 */
struct PeString {
    bool          has_value; ///< Flag indicating if the string value is present
    const PeUTF8* value;     ///< The string value
};
typedef struct PeString PeString;


/**
 * An optional long integer value.
 */
struct PeLong {
    bool  has_value; ///< Flag indicating if the integer value is present
    long  value;     ///< The long integer value
    bool  _converr;  ///< Flag indicating if hubo algun error durante la conversion (str -> int)
};
typedef struct PeLong PeLong;


/**
 * An optional double-precision floating-point value.
 */
struct PeDouble {
    bool   has_value; ///< Flag indicating if the double value is present
    double value;     ///< The double-precision floating-point value
    bool   _converr;  ///< Flag indicating if there was an error during conversion (str -> double)
};
typedef struct PeDouble PeDouble;


/**
 * An optional single-precision floating-point value.
 */
struct PeFloat {
    bool   has_value; ///< Flag indicating if the float value is present
    float  value;     ///< The single-precision floating-point value
    bool   _converr;  ///< Flag indicating if there was an error during conversion (str -> float)
};
typedef struct PeFloat PeFloat;



/*============================ STATE REPORTING ============================*/

enum PEREPORT_TYPE {
    PEREPORT_INFO,     ///< The reported message is an info message
    PEREPORT_WARNING,  ///< The reported message is a warning
    PEREPORT_ERROR     ///< The reported message is an execution error
};
typedef enum PEREPORT_TYPE PEREPORT_TYPE;
typedef void (PeReportFn)(PEREPORT_TYPE type, const PeUTF8* message);


#endif /*PE__TYPES_H__*/
