/* nc2code.c - given a netcdf file, generate code to read or write it.
 * Copyright (C) 2012 Matthew Bishop
 */
#include "sds.h"

#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void generate_f90_code(FILE *fout, SDSInfo *info, int generate_att);
extern void generate_simple_f90_code(FILE *fout, SDSInfo *info, int gen_att);

enum Language {
    LANG_NONE,
    LANG_C,
    LANG_F90,
    LANG_F90_SIMPLE,
    LANG_F77
};

static const char USAGE[] = "Usage: nc2code [OPTION]... FILE.nc\n";
static const char OPTIONS[] =
"Inspect a NetCDF file's metadata and emit code in various languages\n"
"to read or write that file.\n\n"
"Options:\n"
"  -a       generate code to read attributes; default = no\n"
"  -c       generate C code\n"
"  -f       generate Fortran 90 code; default\n"
"  -s       generate Fortran 90 code for the SimpleSDS module\n"
"  -F       generate Fortran 77 code\n"
"  -h       print this help message\n"
"  -o FILE  write the code to FILE instead of stdout\n"
"  -q       goes about its work more quietly\n"
;

/* Options from command line. */
static enum Language gen_lang = LANG_NONE;
static char *input_file = NULL, *output_file = NULL;
static FILE *fout = NULL;
static int be_verbose = 1, generate_att = 0;

static void arg_parse_error(const char *argv0, const char *fmt, ...)
{
    va_list ap;

    fputs(USAGE, stderr);
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, "\nTry `%s -h` for more information\n", argv0);
    exit(-1);
}

static void parse_args(int argc, char **argv)
{
    int i, j, nexti;

    /* parse 'em */
    for (i = 1; i < argc; i = nexti) {
        nexti = i + 1;
        if (argv[i][0] == '-') {
            for (j = 1; argv[i][j] != '\0'; j++) {
                switch (argv[i][j]) {
                case 'a':
                    generate_att = 1;
                    break;
                case 'c':
                    gen_lang = LANG_C;
                    break;
                case 'f':
                    gen_lang = LANG_F90;
                    break;
                case 'F':
                    gen_lang = LANG_F77;
                    break;
                case 'h':
                    puts(USAGE);
                    puts(OPTIONS);
                    exit(0);
                    break;
                case 'o':
                    if (i + 1 < argc) {
                        output_file = argv[i + 1];
                        nexti = i + 2;
                    } else {
                        arg_parse_error(argv[0], "Missing output file after '%s'", argv[i]);
                    }
                    break;
                case 'q':
                    be_verbose = 0;
                    break;
                case 's':
                    gen_lang = LANG_F90_SIMPLE;
                    break;
                default:
                    arg_parse_error(argv[0], "Invalid option '%c'", argv[i][j]);
                    break;
                }
            }
        } else if (input_file) {
            arg_parse_error(argv[0], "Input file already given");
        } else {
            input_file = argv[i];
        }
    }

    /* verify 'em */
    if (!input_file) {
        arg_parse_error(argv[0], "No input file given");
    }

    if (gen_lang == LANG_NONE) {
        if (be_verbose)
            puts("Defaulting to Fortran 90 output");
        gen_lang = LANG_F90;
    }

    if (output_file && be_verbose) {
        printf("Writing output to %s instead of stdout\n", output_file);
    }
}

int main(int argc, char **argv)
{
    SDSInfo *nc_info;

    parse_args(argc, argv);

    /* read netcdf metadata */
    nc_info = open_nc_sds(input_file);
    assert(nc_info != NULL);

    /* open output file */
    if (output_file) {
        fout = fopen(output_file, "w");
        if (!fout) {
            fprintf(stderr, "Opening output file '%s': %s\n", output_file,
                    strerror(errno));
            exit(-1);
        }
    } else {
        fout = stdout;
    }

    /* generate code */
    switch (gen_lang) {
    case LANG_C:
        puts("C code generation not implemented yet");
        break;
    case LANG_F90:
        generate_f90_code(fout, nc_info, generate_att);
        break;
    case LANG_F90_SIMPLE:
        generate_simple_f90_code(fout, nc_info, generate_att);
        break;
    case LANG_F77:
        puts("Fortran 77 code generation not implemented yet");
        break;
    default:
        abort();
        break;
    }

    if (fout != stdout && fclose(fout) != 0) {
        fprintf(stderr, "Closing output file '%s': %s\n", output_file,
                strerror(errno));
        exit(-1);
    }
    return 0;
}
