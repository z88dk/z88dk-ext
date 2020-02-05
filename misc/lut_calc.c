
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <time.h>
#include <sys/time.h>

#if __YAZ180
#include <arch/yaz180.h>
#include <lib/yaz180/time.h>        /* Declaration of system time */
#elif __SCZ180
#include <arch/scz180.h>
#include <lib/scz180/time.h>        /* Declaration of system time */
#elif __HBIOS
#include <lib/scz180/time.h>        /* Declaration of system time */
#elif __RC2014
#include <lib/rc2014/time.h>        /* Declaration of system time */
#warning No timer calculation possible.
#else
#error Do you have time?
#endif


#if __YAZ180
// zcc +yaz180 -subtype=app -clib=sdcc_iy -SO3 -v -m --list --max-allocs-per-node100000 -llib/yaz180/time -llib/yaz180/ff lut_calc.c -o lut_calc -create-app
// Drive 0:
#include <lib/yaz180/ff.h>          /* Declarations of FatFs API */
#include <arch/yaz180/diskio.h>     /* Declarations of diskio & IDE functions */
//#elif __RC2014
//#include <lib/rc2014/ff.h>        /* Declarations of FatFs API */
//#include <arch/rc2014/diskio.h>   /* Declarations of diskio & IDE functions */

#elif __RC2014
// zcc +rc2014 -subtype=hbios -clib=sdcc_iy -SO3 -v -m --list --max-allocs-per-node100000 -llib/hbios/time -llib/hbios/diskio_hbios -llib/hbios/ff lut_calc.c -o lut_calc -create-app
// Drive 2: (or which ever is nominated by hbios).
#include <lib/hbios/ff.h>           /* Declarations of FatFs API */
#include <lib/hbios/diskio_hbios.h> /* Declarations of diskio functions */
#include <arch/hbios.h>             /* Declarations of HBIOS functions */
#pragma output CRT_ORG_BSS = 0x8400 // move bss origin to address 0x8400 (check to confirm there is no overlap between data and bss sections)

//#elif __SCZ180
// zcc +scz180 -subtype=hbios -clib=sdcc_iy -SO3 -v -m --list --max-allocs-per-node100000 -llib/scz180/time -llib/scz180/diskio_sd -llib/scz180/ff lut_calc.c -o lut_calc -create-app
// Drive 0: (as we're not using hbios api, but calling SD directly)
//#include <lib/scz180/ff.h>         /* Declarations of FatFs API */
//#include <lib/scz180/diskio_sd.h>  /* Declarations of diskio functions */
//#include <arch/scz180.h>           /* Declarations of SD functions */

#elif __SCZ180
// zcc +scz180 -subtype=hbios -clib=sdcc_iy -SO3 -v -m --list --max-allocs-per-node100000 -llib/scz180/time -llib/hbios/diskio_hbios -llib/hbios/ff lut_calc.c -o lut_calc -create-app
// Drive 2: (or which ever is nominated by hbios).
#include <lib/hbios/ff.h>           /* Declarations of FatFs API */
#include <lib/hbios/diskio_hbios.h> /* Declarations of diskio functions */
#include <arch/hbios.h>             /* Declarations of HBIOS functions */
#pragma output CRT_ORG_BSS = 0x8400 // move bss origin to address 0x8400 (check to confirm there is no overlap between data and bss sections)

#elif __HBIOS
// zcc +hbios -clib=sdcc_iy -SO3 -v -m --list --max-allocs-per-node100000 -llib/hbios/time -llib/hbios/diskio_hbios -llib/hbios/ff lut_calc.c -o lut_calc -create-app
// Drive 2: (or which ever is nominated by hbios).
#include <lib/hbios/ff.h>           /* Declarations of FatFs API */
#include <lib/hbios/diskio_hbios.h> /* Declarations of diskio functions */
#include <arch/hbios.h>             /* Declarations of HBIOS functions */
#pragma output CRT_ORG_BSS = 0x8400 // move bss origin to address 0x8400 (check to confirm there is no overlap between data and bss sections)

#else
#warning - no FatFs library available
#endif

#pragma printf = "%s %c %u %li %lu"     // enables %s, %c, %u, %li, %lu only

#define BUFFER_SIZE 256             /* size of working buffers (on heap) */


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

    struct timespec startTime, endTime, resTime;

    FatFs = (FATFS *)malloc(sizeof(FATFS));                     /* Get work area for the volume */
    bufferHi = (BYTE *)malloc(sizeof(BYTE)*BUFFER_SIZE);        /* Get working buffer space */
    bufferLo = (BYTE *)malloc(sizeof(BYTE)*BUFFER_SIZE);        /* Get working buffer space */

    startTime.tv_sec = 1577836800 - UNIX_OFFSET;

    clock_settime(CLOCK_REALTIME, &startTime);                  /* Set the time of day, y2k epoch */

    if ((res = f_mount(FatFs, "2:", 1)) == FR_OK) {             /* Give a work area to the default drive */
    
        printf("\r\n\nFatFs->fs_type %u\nFatFs->fsize %lu\n", FatFs->fs_type, FatFs->fsize);

        printf("\r\nCreating 2:MultHi.bin");

        if ((res = f_open(&FileHi, "2:MultHi.bin", FA_CREATE_ALWAYS | FA_WRITE)) == FR_OK) {

            printf(" - Created\r\n\rCreating 2:MultLo.bin");

            if ((res = f_open(&FileLo, "2:MultLo.bin", FA_CREATE_ALWAYS | FA_WRITE)) == FR_OK) {

                printf(" - Created\r\n\nWorking...");

                clock_gettime(CLOCK_REALTIME,&startTime);

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

                clock_gettime(CLOCK_REALTIME,&endTime);
                f_close(&FileLo);

            } else {
                printf("\r\nCouldn't open 2:MultLo.bin - f_open error #%u\r\n", res);                
            }

            f_close(&FileHi);

        } else {
            printf("\r\nCouldn't open 2:MultHi.bin - f_open error #%u\r\n", res);
        }

        timersub(&endTime, &startTime, &resTime);
        printf("The time taken was %li.%.4lu seconds\n", resTime.tv_sec, resTime.tv_nsec/100000 );

        f_mount(0, "2:", 0);                                    /* Free work area */
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

