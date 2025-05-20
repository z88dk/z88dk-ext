/* This version is a simplified port for z88dk, subfolders are not supported */

/* zcc +cpm -compiler=sdcc -SO3 -v -m --list --max-allocs-per-node100000 -create-app -ocp cp.c */
/* zcc +cpm -SO2 -v -m --list -create-app -ocp cp.c */

/*
 * This file is In the public domain.  Use it as you see fit.
 */

/*
 * "cp" is an extremely simple file copy program:
 *  * A single C source file, so it should be easy to compile
 *    and run on any system with a C compiler.
 *  * Extremely portable standard C.  The only non-ANSI function
 *    used is mkdir().
 *  * Does not require libarchive or any other special library.
 *
 * Usage:  cp [destination][source]
 *
 * Copy 1MB file on RC2014 from CF  62.5 seconds
 *                              DOM 60.2 seconds
 *
 * 2025 repeated test SDCC 4.2  DOM 57.3 seconds
 *
 * 2025 new text post SDCC 4.5  DOM 59.7 seconds 63.1 seconds
 *                    SCCZ80    DOM 60.0 seconds 60.0 seconds
 *
 *
 * Reference CP/M PIP           DOM 46.9 Seconds
 *
 * Released into the public domain.
 */

/* These are all highly standard and portable headers. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma printf = "%s"               /* enables %s only */

#define BUFFER_SIZE 32768           /* size of working buffer */

static char buffer[BUFFER_SIZE];    /* working buffer */

static FILE *In, *Out;

int main(int argc, char **argv)
{
    int br, bw;

    if (argc != 3)
    {
        printf("Usage: cp outfile infile\n");
        exit(1);
    }

    In = fopen(argv[2], "rb");
    if (In == NULL) {
        fprintf(stderr, "Unable to open %s\n", argv[2]);
        exit(1);
    } else {
        Out = fopen(argv[1], "wb");
        if (Out == NULL) {
            fprintf(stderr, "Unable to open %s\n", argv[1]);
            fclose(In);
            exit(1);
        } else {

            while (1)
            {
                br = fread(buffer, sizeof(char), BUFFER_SIZE, In);
                if (br == 0) break;     // error or end of file
                bw = fwrite(buffer, sizeof(char), br, Out);
                if (bw != br) break;     // error or disk full
            }

            fclose(Out);
            fclose(In);
        }
    }
    return (0);
}
