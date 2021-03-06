//
// feilipu, 2020 January
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//------------------------------------------------------------------------
// lut_calc - calculate the contents of two look-up tables
//------------------------------------------------------------------------
//
// This example application demonstrates how to open and write files
// using the ChaN FatFS library.
//
// The ff library, the diskio library underlying, and the time library
// all need to be installed from feilipu/z88dk-libraries and linked
// as shown.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <time.h>
#include <sys/time.h>

#if __YAZ180
#include <lib/yaz180/time.h>        /* Declaration of system time */
#elif __SCZ180
#include <lib/scz180/time.h>        /* Declaration of system time */
#elif __HBIOS
#include <lib/hbios/time.h>         /* Declaration of system time */
#elif __RC2014
#include <lib/rc2014/time.h>        /* Declaration of system time */
#warning No timer calculation possible.
#else
#error Do you have time?
#endif


#if __YAZ180
// zcc +yaz180 -subtype=app -clib=sdcc_iy -SO3 -v -m --list --max-allocs-per-node100000 -llib/yaz180/time -llib/yaz180/ff lut_calc.c -o lut_calc -create-app
// This is for the YAZ180, using the 82C55 IDE interface. There is only one drive supported. The program is loaded and run from the monitor.
// Drive 0:
#include <arch/yaz180.h>
#include <lib/yaz180/ff.h>          /* Declarations of FatFs API */
#include <arch/yaz180/diskio.h>     /* Declarations of diskio & IDE functions */

#elif __RC2014
// zcc +rc2014 -subtype=cpm -clib=sdcc_iy -SO3 -v -m --list --max-allocs-per-node100000 -llib/rc2014/time -llib/rc2014/ff lut_calc.c -o lut_calc -create-app
// This is for the RC2014 when it has any CPM firmware and a 82C55 IDE Interface. The output will be written to the first FAT file system found on the drive.
// Most likely to be used with CP/M-IDE firmware, but any CPM that supports the standard 82C55 IDE interface will work.
// Drive 0:
//#include <arch/rc2014.h>            /* Declarations of IDE functions */
//#include <lib/rc2014/ff.h>          /* Declarations of FatFs API */
//#include <arch/rc2014/diskio.h>     /* Declarations of diskio functions */

// zcc +rc2014 -subtype=hbios -clib=sdcc_iy -SO3 -v -m --list --max-allocs-per-node100000 -llib/hbios/time -llib/hbios/diskio_hbios -llib/hbios/ff lut_calc.c -o lut_calc -create-app
// This is for the RC2014 when it has RomWBW firmware and any type of drive. The drive number is the same as the logical drive number reported on boot.
// The program is loaded in the monitor, and started by g100.
// Drive 2: (or whichever is nominated by hbios).
#include <arch/rc2014.h>            /* Declarations of IDE functions */
#include <arch/hbios.h>             /* Declarations of HBIOS functions */
#include <lib/hbios/ff.h>           /* Declarations of FatFs API */
#include <lib/hbios/diskio_hbios.h> /* Declarations of diskio functions */
#pragma output CRT_ORG_BSS = 0x9000 // move bss origin to address 0x9000 (check to confirm there is no overlap between data and bss sections, and set as needed)

#elif __SCZ180
// zcc +scz180 -subtype=hbios -clib=sdcc_iy -SO3 -v -m --list --max-allocs-per-node100000 -llib/scz180/time -llib/scz180/diskio_sd -llib/scz180/ff lut_calc.c -o lut_calc -create-app
// Drive 0: (as we're not using hbios api, but calling SD directly)
//#include <arch/scz180.h>           /* Declarations of SD functions */
//#include <lib/scz180/ff.h>         /* Declarations of FatFs API */
//#include <lib/scz180/diskio_sd.h>  /* Declarations of diskio functions */
//#pragma output CRT_ORG_BSS = 0x8400 // move bss origin to address 0x8400 (check to confirm there is no overlap between data and bss sections, and set as needed)

// zcc +scz180 -subtype=hbios -clib=sdcc_iy -SO3 -v -m --list --max-allocs-per-node100000 -llib/scz180/time -llib/hbios/diskio_hbios -llib/hbios/ff lut_calc.c -o lut_calc -create-app
// This is for the SCZ180 when it has RomWBW firmware and any type of drive. The drive number is the same as the logical drive number reported on boot.
// The program is loaded in the monitor, and started by g100.
// Drive 2: (or whichever is nominated by hbios).
#include <arch/scz180.h>
#include <arch/hbios.h>             /* Declarations of HBIOS functions */
#include <lib/hbios/ff.h>           /* Declarations of FatFs API */
#include <lib/hbios/diskio_hbios.h> /* Declarations of diskio functions */
#pragma output CRT_ORG_BSS = 0x8400 // move bss origin to address 0x8400 (check to confirm there is no overlap between data and bss sections, and set as needed)

#elif __HBIOS
// zcc +hbios -clib=sdcc_iy -SO3 -v -m --list --max-allocs-per-node100000 -llib/hbios/time -llib/hbios/diskio_hbios -llib/hbios/ff lut_calc.c -o lut_calc -create-app
// This is for any RetroBrew target when it has RomWBW firmware and any type of drive. The drive number is the same as the logical drive number reported on boot.
// The program is loaded in the monitor, and started by g100.
// Drive 2: (or whichever is nominated by hbios).
#include <arch/hbios.h>             /* Declarations of HBIOS functions */
#include <lib/hbios/ff.h>           /* Declarations of FatFs API */
#include <lib/hbios/diskio_hbios.h> /* Declarations of diskio functions */
//#pragma output CRT_ORG_BSS = 0x8400 // move bss origin to address 0x8400 (check to confirm there is no overlap between data and bss sections, and set as needed)

#else
#warning - no FatFs library available
#endif

// PRAGMAS

#pragma printf = "%s %c %u %li %lu" // enables %s, %c, %u, %li, %lu only

// DEFINES

#define BUFFER_SIZE 256             /* size of working buffers (on heap) */

// GLOBALS

static FIL FileHi, FileLo;          /* File object needed for each open file */
static FATFS * FatFs;               /* FatFs work area needed for each volume */
                                    /* Pointer to the filesystem object (on heap) */

static BYTE * bufferHi;             /* working buffer high byte*/
static BYTE * bufferLo;             /* working buffer low byte*/
                                    /* for romwbw hbios buffer must be in bss (which is above 0x8000) */
// helper function
static void put_rc (FRESULT rc);    /* print stderr codes to defined stderr IO */


int main (void)
{
    uint16_t theWord;
    uint16_t bw;

    FRESULT res;

#if !__RC2014
    struct timespec startTime, endTime, resTime;
#endif

    FatFs = (FATFS *)malloc(sizeof(FATFS));                     /* Get work area for the volume */
    bufferHi = (BYTE *)malloc(sizeof(BYTE)*BUFFER_SIZE);        /* Get working buffer space */
    bufferLo = (BYTE *)malloc(sizeof(BYTE)*BUFFER_SIZE);        /* Get working buffer space */

#if !__RC2014
    startTime.tv_sec = 1577836800 - UNIX_OFFSET;
    clock_settime(CLOCK_REALTIME, &startTime);                  /* Set the time of day, y2k epoch */
#endif

    if ((res = f_mount(FatFs, "0:", 1)) == FR_OK) {             /* Give a work area to the default drive */
    
        printf("\r\n\nFatFs->fs_type %u\nFatFs->fsize %lu\n", FatFs->fs_type, FatFs->fsize);

        printf("\r\nCreating 0:MultHi.bin");

        if ((res = f_open(&FileHi, "0:MultHi.bin", FA_CREATE_ALWAYS | FA_WRITE)) == FR_OK) {

            printf(" - Created\r\n\rCreating 0:MultLo.bin");

            if ((res = f_open(&FileLo, "0:MultLo.bin", FA_CREATE_ALWAYS | FA_WRITE)) == FR_OK) {

                printf(" - Created\r\n\nWorking...");
#if !__RC2014
                clock_gettime(CLOCK_REALTIME,&startTime);
#endif
                for (uint16_t x = 0; x < BUFFER_SIZE; ++x) {

                    for (uint16_t y = 0; y < BUFFER_SIZE; ++y) {

                        theWord = (uint8_t)x*(uint8_t)y;
                        bufferHi[y] = (uint8_t)(theWord>>8);
                        bufferLo[y] = (uint8_t)(theWord);
                    }
                    res = f_write(&FileHi, bufferHi, BUFFER_SIZE, &bw); // put_rc(res);
                    if (res != FR_OK || bw != BUFFER_SIZE) break;       // error or disk full
                    res = f_write(&FileLo, bufferLo, BUFFER_SIZE, &bw); // put_rc(res);
                    if (res != FR_OK || bw != BUFFER_SIZE) break;       // error or disk full
                }
#if !__RC2014
                clock_gettime(CLOCK_REALTIME,&endTime);
#endif
                f_close(&FileLo);

            } else {
                printf("\r\nCouldn't open 0:MultLo.bin - f_open error #%u\r\n", res);                
            }

            f_close(&FileHi);

        } else {
            printf("\r\nCouldn't open 0:MultHi.bin - f_open error #%u\r\n", res);
        }
        printf(" done\r\n");
#if !__RC2014
        timersub(&endTime, &startTime, &resTime);
        printf("The time taken was %li.%.4lu seconds\n", resTime.tv_sec, resTime.tv_nsec/100000 );
#endif
        f_mount(0, "0:", 0);                                    /* Free work area */
    } else {
        printf("\r\nCouldn't mount drive - f_mount error #%u\r\n", res);
    }
    // Perform any shutdown/cleanup.
    free(bufferLo);
    free(bufferHi);
    free(FatFs);
    return 0;
}


// helper function
// use put_rc to get a plain text interpretation of the disk return or error code.

static
void put_rc (FRESULT rc)
{
    const char *str =
        "OK\0" "DISK_ERR\0" "INT_ERR\0" "NOT_READY\0" "NO_FILE\0" "NO_PATH\0"
        "INVALID_NAME\0" "DENIED\0" "EXIST\0" "INVALID_OBJECT\0" "WRITE_PROTECTED\0"
        "INVALID_DRIVE\0" "NOT_ENABLED\0" "NO_FILE_SYSTEM\0" "MKFS_ABORTED\0" "TIMEOUT\0"
        "LOCKED\0" "NOT_ENOUGH_CORE\0" "TOO_MANY_OPEN_FILES\0" "INVALID_PARAMETER\0";

    FRESULT i;
    uint8_t res;

    res = (uint8_t)rc;

    for (i = 0; i != res && *str; ++i) {
        while (*str++) ;
    }
    fprintf(stderr,"rc=%u FR_%s\r\n", res, str);
}

