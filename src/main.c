/*
| File    : main.h
| Purpose : Parsing command line arguments and running the `promptex` command.
| Author  : Martin Rizzo | <martinrizzo@gmail.com>
| Date    : Oct 2, 2026
| Repo    : https://github.com/martin-rizzo/PromptExtractor
| License : MIT
|- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
|                              PromptExtractor
| A lightweight CLI tool to extract embedded prompts from AI-generated images.
\_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "params.h"
#include "image_fmt/png.h"
#include "image_fmt/jpeg.h"
typedef PeUTF8*       StringPtr;
typedef const PeUTF8* ConstStringPtr;
#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define OPT_EQ(arg,opt,long_opt) (strcmp(arg,(opt)) == 0 || strcmp(arg,(long_opt)) == 0)

static const char HELP[] = 
"Usage: %s [OPTIONS] image-file(s)\n"
"\n"
"  Display information about the prompt embedded in AI-generated images.\n"
"\n"
"  OPTIONS:\n"
"    -n, --no-color    Disable color output.\n"
"    -h, --help        Show this help message and exit.\n"
"    -v, --version     Show version information and exit.\n"
"\n"
"  Examples:\n"
"    %s image1.png image2.jpg\n"
"\n";

/*============================== ANSI COLORS ==============================*/

/**
 * Represents a set of ANSI color codes for terminal output.
 * 
 * This struct holds pointers to ANSI escape codes that define the color theme.
 * Each member corresponds to a specific color category.
 */
struct TerminalColors {
    const char *primary,   /**< Default text color.                  */
               *highlight, /**< Highlight color for emphasis.        */
               *data,      /**< Color for data values.               */
               *success,   /**< Color for success messages.          */
               *error,     /**< Color for error messages.            */
               *info,      /**< Color for informational messages.    */
               *reset;     /**< ANSI code to reset color to default. */
};

/** Instance of TerminalColors used to store the current color theme. */
static struct TerminalColors C;

/**
 * Sets the current terminal color theme based on the provided index.
 *
 * This function configures the color theme by assigning appropriate ANSI
 * escape codes to the members of the global `TerminalColors` struct.
 * If the index is 0, it disables all colors.
 *
 * @param index The theme index.
 */
void set_color_theme(int index) {
    switch( index ) {
        case 0:
            C.primary = C.highlight = C.data = \
            C.success = C.error = C.info = C.reset = "";
            break;
        case 1:
        default:
            C.primary   = "\x1b[;37m";
            C.highlight = "\x1b[1;37m";
            C.data      = "\x1b[;32m";
            C.success   = "\x1b[;1;32m";
            C.error     = "\x1b[;1;31m";
            C.info      = "\x1b[;1;34m";
            C.reset     = "\x1b[0m";
            break;
    }
}

/*================================ HELPERS ================================*/

/**
 * Prints the help message for the program.
 * 
 * @param argv An array of strings containing command-line arguments, the same
 *             `argv` parameter found in C's main function. The first element
 *             (argv[0]) should contain the name of the executable.
 * @return The exit code that should be returned by the command (0 == success)
 */
int print_help(int argc, char* argv[]) {
    PE_UNUSED(argc);
    printf(HELP, argv[0], argv[0]);
    return 0;
}

/**
 * Print the version information of the program.
 * 
 * @return The exit code that should be returned by the command (0 == success)
 */
int print_version(void) {
    printf( "%s%s: %s%d.%d\n",
            C.highlight, "version",
            C.data     , VERSION_MAJOR, VERSION_MINOR
          );
    return 0;
}

void print_report(PEREPORT_TYPE type, const PeUTF8* message) {
    switch( type ) {
        case PEREPORT_WARNING:
            fprintf(stderr, "WARNING: %s\n", message);
            break;
        case PEREPORT_ERROR:
            fprintf(stderr, "ERROR: %s\n", message);
            break;
        default:
            fprintf(stderr, "%s\n", message);
            break;
    }
}



/*===================== PRINTING PROMPT & GEN PARAMS ======================*/

static
int print_prompt(FILE* stream, const PeUTF8* stream_name) {
    const PeParams* params = pe_params_from_stream(stream, stream_name, print_report);
    printf("##>> PATH: %s\n", params->fileinfo.path);
    printf("##>> NAME: %s\n", params->fileinfo.name);
    for (int i=0; i<params->fileinfo.texts_count; ++i) {
        printf("##>> %s (\"%s\"): %.60s\n",
            params->fileinfo.texts[i].id,
            params->fileinfo.texts[i].name,
            params->fileinfo.texts[i].content
        );
    }
    pe_destroy_params(params);
    return 0;
}


/**
 * Prints the prompts embedded in multiple image files.
 *
 * @param filepaths  An array of strings where each element is a filepath of an image.
 * @param count      The number of files in the filepaths array.
 * @return The exit code that should be returned by the command (0 == success)
 */
static
int print_all_prompts(ConstStringPtr filepaths[], int count) {

    // loop through each file and print it
    for( int fidx=0 ; fidx < count ; ++fidx ) {
        ConstStringPtr path = filepaths[fidx];
        FILE*          file = fopen(path, "r");
        if (file) {
            print_prompt(file, path);
            printf("\n");
            fclose(file);
        }
    }
    return 0;
}


/*===========================================================================
|  //////////////////////////////// MAIN /////////////////////////////////  | 
===========================================================================*/

/**
 * Main entry point of the program.
 * 
 * This function handles command-line arguments, processes options such as
 * --help, --version, and manages the list of filenames provided. It then
 * executes the appropriate action based on the parsed arguments.
 * 
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 * @return The exit status of the program (0 for success, >=1 for errors).
 */
int main(int argc, char* argv[])
{
    bool disable_options   = false; // flag to disable further option parsing.     
    bool disable_colors    = false; // flag to disable color output.               
    bool show_help         = false; // flag to indicate if help should be shown.   
    bool show_version      = false; // flag to indicate if version should be shown.
    int  color_theme       = 1;     // color theme to use (default = colorful).    
    ConstStringPtr *filenames = malloc(sizeof(ConstStringPtr) * argc);
    int error = 0;                // Error code returned by the program.
    int i, file_count;            // Loop index and count of filenames.  
    set_color_theme(0);

    // iterates over all the arguments provided by the user
    file_count = 0;
    for( i=1 ; i < argc && !error ; ++i ) {
        const char* arg = argv[i];

        if( arg[0] == '-'  && !disable_options ) {
            if     (OPT_EQ(arg, "-h", "--help"    )) { show_help       = true; }
            else if(OPT_EQ(arg, "-v", "--version" )) { show_version    = true; }
            else if(OPT_EQ(arg, "-n", "--no-color")) { disable_colors  = true; }
            else if( strcmp(arg,"--")==0           ) { disable_options = true; }
            else { fprintf(stderr, "Unknown option: %s\n", arg); error=1; }
        }
        else {
            filenames[file_count++] = argv[i];
        }
    }

    // set color theme based on user preference
    if( disable_colors ) { color_theme = 0; }
    set_color_theme(color_theme);

    // if no filenames were provided, show help
    if( file_count==0 ) { show_help = true; }

    // execute the command based on parsed options
    if( !error ) { 
        if     ( show_version ) { error = print_version();  }
        else if( show_help    ) { error = print_help(argc,argv); }
        else                    { error = print_all_prompts(filenames,file_count); }
    }

    // free the allocated memory for filenames
    free(filenames);
    return error;
}
