//
// feilipu, 2020 January
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//------------------------------------------------------------------------
// yash - yet another shell for Z80 machines with FAT file systems
//------------------------------------------------------------------------
//
// Derived from the example provided by Stephen Brennan.
// https://brennan.io/2015/01/16/write-a-shell-in-c/
// https://github.com/brenns10/lsh
//
// RC2014 Forum Post
// https://groups.google.com/d/msg/rc2014-z80/FIw9F8fNnIo/91isliSAAwAJ
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
#warning Do you have time?
#endif

// Note that since z88dk-lib only copies ONE header file automatically to the library header locations, the ffconf.h file needs to be ADDED MANUALLY to the 
// It should be located in the correct directory (along with ff.h) as per the examples provided below.

#if __YAZ180
// zcc +yaz180 -subtype=cpm -SO3 -v -m --list --max-allocs-per-node400000 -llib/yaz180/time -llib/yaz180/ff yash.c -o yash -create-app
// This is for the YAZ180, using the 82C55 IDE interface. There is only one drive supported. The program is loaded and run from the monitor.
// Drive 0:
#include <arch/yaz180.h>
#include <lib/yaz180/ffconf.h>      /* Declarations of FatFs configuration */
#include <lib/yaz180/ff.h>          /* Declarations of FatFs API */
#include <arch/yaz180/diskio.h>     /* Declarations of diskio & IDE functions */

#elif __RC2014
// zcc +rc2014 -subtype=cpm -SO3 -v -m --list --max-allocs-per-node400000 -llib/rc2014/ff yash.c -o yash -create-app
// zcc +rc2014 -subtype=cpm -clib=new -O2 -v -m --list -llib/rc2014/ff yash.c -o yash -create-app
// This is for the RC2014 when it has any CPM firmware and a 82C55 IDE Interface. The output will be written to the first FAT file system found on the drive.
// Most likely to be used with CP/M-IDE firmware, but any CPM that supports the standard 82C55 Hard Drive Module or CF Module interface will work.
// Drive 0:
#include <arch/rc2014.h>
#include <lib/rc2014/ffconf.h>      /* Declarations of FatFs configuration */
#include <lib/rc2014/ff.h>          /* Declarations of FatFs API */
#include <arch/rc2014/diskio.h>     /* Declarations of diskio functions */

#elif __SCZ180
// zcc +scz180 -subtype=hbios -SO3 -v -m --list --max-allocs-per-node400000 -llib/scz180/time -llib/scz180/diskio_sd -llib/scz180/ff yash.c -o yash -create-app
// Drive 0: (as we're not using hbios api, but calling SD directly)
//#include <arch/scz180.h>           /* Declarations of SD functions */
//#include <lib/scz180/ffconf.h>     /* Declarations of FatFs configuration */
//#include <lib/scz180/ff.h>         /* Declarations of FatFs API */
//#include <lib/scz180/diskio_sd.h>  /* Declarations of SD diskio functions */
//#pragma output CRT_ORG_BSS = 0xA800 /* move bss origin to address 0xA800 (check to confirm there is no overlap between data and bss sections, and set as needed)

// zcc +scz180 -subtype=hbios -SO3 -v -m --list --max-allocs-per-node400000 -llib/scz180/time -llib/hbios/diskio_hbios -llib/hbios/ff yash.c -o yash -create-app
// This is for the SCZ180 when it has RomWBW firmware and any type of drive. The drive number is the same as the logical drive number reported on boot.
// The HEX program is loaded in the dbgmon monitor M L, and started by R100.
// Drive 2: (or whichever is nominated by hbios).
// Relative directories are not working, use the full directory i.e "ls 2:/example" or "cp 2:/test.bin 2:/test/test2.bin"
#include <arch/scz180.h>
#include <arch/hbios.h>             /* Declarations of HBIOS functions */
#include <lib/hbios/ffconf.h>       /* Declarations of FatFs configuration */
#include <lib/hbios/ff.h>           /* Declarations of FatFs API */
#include <lib/hbios/diskio_hbios.h> /* Declarations of HBIOS diskio functions */
#pragma output CRT_ORG_BSS = 0xA800 /* move bss origin to address 0xA800 (check to confirm there is no overlap between data and bss sections, and set as needed) */

#elif __HBIOS
// zcc +hbios -SO3 -v -m --list --max-allocs-per-node400000 -llib/hbios/time -llib/hbios/diskio_hbios -llib/hbios/ff yash.c -o yash -create-app
// This is for any RetroBrew target when it has RomWBW firmware and any type of drive. The drive number is the same as the logical drive number reported on boot.
// The HEX program is loaded in the dbgmon monitor M L, and started by R100.
// Drive 2: (or whichever is nominated by hbios).
// Relative directories are not working, use the full directory i.e "ls 2:/example" or "cp 2:/test.bin 2:/test/test2.bin"
#include <arch/hbios.h>             /* Declarations of HBIOS functions */
#include <lib/hbios/ffconf.h>       /* Declarations of FatFs configuration */
#include <lib/hbios/ff.h>           /* Declarations of FatFs API */
#include <lib/hbios/diskio_hbios.h> /* Declarations of HBIOS diskio functions */
#pragma output CRT_ORG_BSS = 0xA800 /* move bss origin to address 0xA800 (check to confirm there is no overlap between data and bss sections, and set as needed) */

#elif __CPM
// a hacked solution for __RC2014 8085 CPU running CP/M from classic library
// uncomment below XXX include file
// zcc +cpm -clib=8085 -O2 --opt-code-speed=all -v -m --list -DAMALLOC -l../../libsrc/_DEVELOPMENT/lib/sccz80/lib/rc2014/ff_85 -l../../lib/clibs/rc2014-8085_clib yash.c -o yash85 -create-app
#include <../libsrc/_DEVELOPMENT/target/rc2014/config_rc2014-8085.h>

// a hacked solution for __RC2014 Z80 CPU running CP/M from classic library
// uncomment below XXX include file
// zcc +cpm -clib=default -O2 --opt-code-speed=all -v -m --list -DAMALLOC -l../../libsrc/_DEVELOPMENT/lib/sccz80/lib/rc2014/ff -l../../lib/clibs/rc2014-8085_clib yash.c -o yash -create-app
//#include <../libsrc/_DEVELOPMENT/target/rc2014/config_rc2014.h>

#include <_DEVELOPMENT/sccz80/arch/rc2014.h>            /* Declarations of RC2014 specifics */
#include <_DEVELOPMENT/sccz80/lib/rc2014/ffconf.h>      /* Declarations of FatFs configuration */
#include <_DEVELOPMENT/sccz80/lib/rc2014/ff.h>          /* Declarations of FatFs API */
#include <_DEVELOPMENT/sccz80/arch/rc2014/diskio.h>     /* Declarations of diskio functions */

#else
#warning - no FatFs library available
#endif

// PRAGMA DEFINES

#pragma printf = "%c %s %d %02u %lu %04X"       // enables %c, %s, %d, %u, %lu, %X only
#pragma output CLIB_DISABLE_FGETS_CURSOR=1      // disable classic lib stdio cursor

// DEFINES

#define MAX_FILES 2             // number of files open at any time

#define BUFFER_SIZE 1024        // size of working buffer (on heap)
#define LINE_SIZE 256           // size of a command line (on heap)
#define TOK_BUFSIZE 64          // size of token pointer buffer (on heap)

#define TOK_DELIM " \t\r\n\a"

#define DRIVE_SIZE 8388608      // size in bytes of default CP/M drive file
#define EXTENTS 2048            // number of directory extents in default CP/M drive

// GLOBALS

extern uint32_t cpm_dsk0_base[4];

static void * buffer;           /* create a scratch buffer on heap later */
                                /* for romwbw hbios buffer must be in bss (which is above 0x8000) */

static FATFS * fs;              /* Pointer to the filesystem object (on heap) */
                                /* FatFs work area needed for each volume */

static FIL File[MAX_FILES];     /* File object needed for each open file */

static uint8_t directoryBlock[32] = {0xE5,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20, \
                                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

/*
  Function Declarations for built-in shell commands:
 */

// CP/M related functions
int8_t ya_dmount(char ** args); // mount a CP/M drive
int8_t ya_mkdrv(char ** args);  // create a FATFS CP/M drive file

// system related functions
int8_t ya_md(char ** args);     // memory dump
int8_t ya_help(char ** args);   // help
int8_t ya_exit(char ** args);   // exit to CP/M

// fat related functions
int8_t ya_frag(char ** args);   // check file for fragmentation
int8_t ya_ls(char ** args);     // directory listing
int8_t ya_rm(char ** args);     // delete a file
int8_t ya_cp(char ** args);     // copy a file
int8_t ya_mv(char ** args);     // move (rename) a file
int8_t ya_cd(char ** args);     // change the current working directory
int8_t ya_pwd(char ** args);    // show the current working directory
int8_t ya_mkdir(char ** args);  // create a new directory
int8_t ya_chmod(char ** args);  // change file or directory attributes
int8_t ya_mount(char ** args);  // mount a FAT file system
int8_t ya_umount(char ** args); // unmount a FAT file system

// disk related functions
int8_t ya_ds(char ** args);     // disk status
int8_t ya_dd(char ** args);     // disk dump sector

// time related functions
#if !__RC2014 && !__CPM
int8_t ya_clock(char ** args);  // set the time (UNIX epoch)
int8_t ya_tz(char ** args);     // set timezone (no daylight savings, so adjust manually)
int8_t ya_diso(char ** args);   // print the local time in ISO: 2013-03-23 01:03:52
int8_t ya_date(char ** args);   // print the local time in US: Sun Mar 23 01:03:52 2013
#endif

// helper functions
uint8_t ya_num_builtins(void);
static void put_rc (FRESULT rc);    // print error codes to defined error IO
static void put_dump (const uint8_t * buff, uint16_t ofs, uint8_t cnt);

// main loop functions
int8_t ya_execute(char ** args);
void ya_split_line(char ** tokens, char * line);
void ya_loop(void);

// RC2014 CP/M-IDE & YAZ180 CP/M stores four LBA bases from cpm_dsk0_base.
// XXX Adjust this base value to suit the current build.
static void dsk0_helper (void);
#if __RC2014 || __CPM
static void dsk0_helper(void) __naked
{
    __asm
        defc _cpm_dsk0_base = 0xF800    ; For all RC2014 Builds
;       INCLUDE "../libsrc/_DEVELOPMENT/target/rc2014/config_rc2014-8085_public.inc" ; XXX uncommment only for cpm/sccz80/classic/8085
;       INCLUDE "../libsrc/_DEVELOPMENT/target/rc2014/config_rc2014_public.inc" ; XXX uncomment only for cpm/sccz80/classic/z80
    __endasm;
}
#elif __YAZ180
static void dsk0_helper(void) __naked
{
    __asm
        defc _cpm_dsk0_base = 0x0040    ; For YAZ180 Build
    __endasm;
}
#else
#warning - no disk 0 LBA base - dmount inoperative
#endif

/*
  List of builtin commands.
 */

struct Builtin {
    const char * name;
    int8_t (*func) (char ** args);
    const char * help;
};

struct Builtin builtins[] = {
  // CP/M related functions
    { "dmount", &ya_dmount, "drive: [path]file - mount a CP/M drive"},
    { "mkdrv", &ya_mkdrv, "[file][extents][bytes] - create a FATFS CP/M drive file, dir directory extents, of bytes size"},

// fat related functions
    { "frag", &ya_frag, "[file] - check for file fragmentation"},
    { "ls", &ya_ls, "[path] - directory listing"},
    { "rm", &ya_rm, "[file] - delete a file"},
    { "cp", &ya_cp, "[src][dest] - copy a file"},
    { "mv", &ya_mv, "[src][dest] - move (rename) a file"},
    { "cd", &ya_cd, "[path] - change the current working directory"},
    { "pwd", &ya_pwd, "- show the current working directory"},
    { "mkdir", &ya_mkdir, "[path] - create a new directory"},
    { "chmod", &ya_chmod, "[path][attr][mask] - change file or directory attributes"},
    { "mount", &ya_mount, "[drive] - mount a FAT file system"},
    { "umount", &ya_umount, "[drive] - unmount a FAT file system"},

// disk related functions
    { "ds", &ya_ds, "[drive] - disk status"},
    { "dd", &ya_dd, "[drive][sector] - disk dump, drive in decimal, sector in decimal"},

// time related functions
#if !__RC2014 && !__CPM
    { "clock", &ya_clock, "[timestamp] - set the time (UNIX epoch) 'date +%s'"},
    { "tz", &ya_tz, "[tz] - set timezone (no daylight saving)"},
    { "diso", &ya_diso, "- local time ISO format: 2013-03-23 01:03:52"},
    { "date", &ya_date, "- local time: Sun Mar 23 01:03:52 2013" },
#endif

// system related functions
    { "md", &ya_md, "[origin] - memory dump, origin in hexadecimal"},
    { "help", &ya_help, "- this is it"},
    { "exit", &ya_exit, "- exit and return to CCP"}
};

uint8_t ya_num_builtins(void) {
    return sizeof(builtins) / sizeof(struct Builtin);
}


/*
  Builtin function implementations.
*/


/**
   @brief Builtin command:
   @param args List of args.  args[0] is "dmount". args[1] drive letter. [2] drive file.
   @return Always returns 1, to continue executing.
 */
int8_t ya_dmount(char ** args)  /* mount a drive on CP/M */
{
    FRESULT res;
    uint8_t i = 0;

    if (args[1] == NULL || args[2] == NULL) {
        fprintf(stdout, "yash: expected 2 arguments to \"dmount\"\n");
#if __RC2014 || __YAZ180
    } else {
        res = f_mount(fs, (const TCHAR*)"0:", 0);
        if (res != FR_OK) { put_rc(res); return 1; }

        // set up CPM drive LBA location
        i = ((uint8_t)*args[1]&0b01011111) - 'A';
        if (i > 3) { fprintf(stdout,"Drive %c: doesn't exit\n", (uint8_t)*args[1]&0b01011111); return 1; }    // maximum 4 drives in CP/M-IDE
        fprintf(stdout,"Mounting \"%s\" on drive %c:", args[2], (uint8_t)*args[1]&0b01011111);

        res = f_open(&File[0], (const TCHAR *)args[2], FA_OPEN_EXISTING | FA_READ);
        if (res != FR_OK) { put_rc(res); return 1; }
        cpm_dsk0_base[i] = (&File[0])->obj.fs->database + ((&File[0])->obj.fs->csize * ((&File[0])->obj.sclust - 2));
        fprintf(stdout," from LBA %lu\n", cpm_dsk0_base[i]);

        f_close(&File[0]);
#endif
    }
    return 1;
}


/**
   @brief Builtin command:
   @param args List of args.  args[0] is "mkdrv". args[1] is the nominated drive name.
                              args[2] is the number of directory entries, args[3] is file size in bytes.
   @return Always returns 1, to continue executing.
 */
int8_t ya_mkdrv(char ** args)  /* create a file for CP/M drive */
{
    FRESULT res;
    int16_t dirEntries;
    int16_t dirBytesWritten;
    uint32_t fileSize;
    uint32_t lbaBase;

    if (args[1] == NULL ) {
        fprintf(stdout, "yash: expected 1 argument to \"mkdrv\"\n");
    } else {
        fprintf(stdout,"Creating \"%s\"", args[1]);
        res = f_open(&File[0], (const TCHAR*)args[1], FA_CREATE_ALWAYS | FA_WRITE);
        if (res != FR_OK) { put_rc(res); return 1; }

        if (fileSize = atol(args[3]) == 0) fileSize = DRIVE_SIZE;
        res = f_expand(&File[0], fileSize, 1);
        if (res != FR_OK) {
            fprintf(stdout, "\nInsufficient space");
            put_rc(res);
            f_close(&File[0]);
            f_unlink((const TCHAR*)args[1]);
            return 1;
        }

        if (dirEntries = atoi(args[2]) == 0) dirEntries = EXTENTS;
        for (uint16_t i = 0; i < dirEntries; i++) {
            // There are 4 Directory Entries (Extents) per CPM sector
            res = f_write ( &File[0], (const uint8_t *)directoryBlock, 32, &dirBytesWritten );
            if (res != FR_OK || dirBytesWritten != 32) {
                fprintf(stdout, "\nCP/M Directory Extents incomplete");
                put_rc(res);
                f_close(&File[0]);
                return 1;
            }
        }

        lbaBase = (&File[0])->obj.fs->database + ((&File[0])->obj.fs->csize * ((&File[0])->obj.sclust - 2));
        f_close(&File[0]);

        fprintf(stdout," at base sector LBA %lu\n", lbaBase);
    }
    return 1;
}


/*
  system related functions
 */


/**
   @brief Builtin command:
   @param args List of args.  args[0] is "md". args[1] is the origin address.
   @return Always returns 1, to continue executing.
 */
int8_t ya_md(char ** args)      /* dump RAM contents from nominated origin. */
{
    static uint8_t * origin = 0;
    uint16_t ofs;
    uint8_t * ptr;

    if (args[1] == NULL) {
        fprintf(stdout, "yash: expected 1 argument to \"md\"\n");
    } else {
        origin = (uint8_t *)strtoul(args[1], NULL, 16);
    }

    fprintf(stdout, "\nOrigin: %04X\n", (uint16_t)origin);

    for (ptr=origin, ofs = 0; ofs < 0x100; ptr += 16, ofs += 16) {
        put_dump(ptr, ofs, 16);
    }

    origin += 0x100;            /* go to next page (next time) */
    return 1;
}


/**
   @brief Builtin command:
   @param args List of args.  args[0] is "help".
   @return Always returns 1, to continue executing.
 */
int8_t ya_help(char ** args)    /* print some help. */
{
    uint8_t i;
    (void *)args;

    fprintf(stdout,"yash v1.2 2025\n");
    fprintf(stdout,"The following functions are built in:\n");

    for (i = 0; i < ya_num_builtins(); ++i) {
        fprintf(stdout,"  %s %s\n", builtins[i].name, builtins[i].help);
    }
    return 1;
}


/**
   @brief Builtin command:
   @param args List of args.  args[0] is "exit".
   @return Always returns 0, to terminate execution.
 */
int8_t ya_exit(char ** args)    /* exit to CP/M */
{
    (void *)args;

    f_mount(0, (const TCHAR*)"", 0);    /* Unmount the default drive */
    return 0;
}


/*
  fat related functions
 */


/**
   @brief Builtin command:
   @param args List of args.  args[0] is "frag".  args[1] is the name of the file.
   @return Always returns 1, to continue executing.
 */
int8_t ya_frag(char ** args)    /* check file for fragmentation */
{
    FRESULT res;
    DWORD clst, clsz, step;
    FSIZE_t fsz;

    if (args[1] == NULL) {
        fprintf(stdout, "yash: expected 1 argument to \"frag\"\n");
    } else {

        fprintf(stdout,"Checking \"%s\"", args[1]);
        res = f_open(&File[0], (const TCHAR *)args[1], FA_OPEN_EXISTING | FA_READ);
        if (res != FR_OK) { put_rc(res); return 1; }

        fsz = f_size(&File[0]);                                     /* File size */
        clsz = (DWORD)(&File[0])->obj.fs->csize * FF_MAX_SS;        /* Cluster size */
        if (fsz > 0) {                                              /* Check file size non-zero */
            clst = (&File[0])->obj.sclust - 1;                      /* An initial cluster leading the first cluster for first test */
            while (fsz) {                                           /* Check clusters are contiguous */
                step = (fsz >= clsz) ? clsz : (DWORD)fsz;
                res = f_lseek(&File[0], f_tell(&File[0]) + step);   /* Advances file pointer a cluster */
                if (res != FR_OK) { put_rc(res); return 1; }
                if (clst + 1 != (&File[0])->clust) break;           /* Is not the cluster next to previous one? */
                clst = (&File[0])->clust;
                fsz -= step;                                        /* Get current cluster for next test */
            }
            fprintf(stdout," at LBA %lu", (&File[0])->obj.fs->database + ((&File[0])->obj.fs->csize * ((&File[0])->obj.sclust - 2)));
            if (fsz == 0) {                                         /* All checked contiguous without fail? */
                fprintf(stdout," is OK\n");
            } else {
                fprintf(stdout," is fragmented\n");
            }
        }

        f_close(&File[0]);
    }
    return 1;
}

/**
   @brief Builtin command:
   @param args List of args.  args[1] is the drive number.
   @return Always returns 1, to continue executing.
 */
int8_t ya_mount(char ** args)   /* mount a FAT file system */
{
    if (args[1] == NULL) {
        put_rc(f_mount(fs, (const TCHAR*)"0:", 0));
    } else {
        put_rc(f_mount(fs, (const TCHAR*)args[1], 0));
    }
    return 1;
}


/**
   @brief Builtin command:
   @param args List of args.  args[1] is the drive number.
   @return Always returns 1, to continue executing.
 */
int8_t ya_umount(char ** args)  /* unmount a FAT file system */
{
    if (args[1] == NULL) {
        put_rc(f_mount(0, (const TCHAR*)"0:", 0));
    } else {
        put_rc(f_mount(0, (const TCHAR*)args[1], 0));
    }
    return 1;
}


/**
   @brief Builtin command:
   @param args List of args.  args[0] is "ls".  args[1] is the path.
   @return Always returns 1, to continue executing.
 */
int8_t ya_ls(char ** args)      /* print directory contents */
{
    DIR dir;                    /* Stack Directory Object */
    FRESULT res;
    uint32_t p1;
    uint16_t s1, s2;

    static FILINFO Finfo;       /* Static File Information */

    res = f_mount(fs, (const TCHAR*)"0:", 0);
    if (res != FR_OK) { put_rc(res); return 1; }

    if(args[1] == NULL) {
        res = f_opendir(&dir, (const TCHAR*)".");
    } else {
        res = f_opendir(&dir, (const TCHAR*)args[1]);
    }
    if (res != FR_OK) { put_rc(res); return 1; }

    p1 = s1 = s2 = 0;
    while(1) {
        res = f_readdir(&dir, &Finfo);
        if ((res != FR_OK) || !Finfo.fname[0]) break;
        if (Finfo.fattrib & AM_DIR) {
            s2++;
        } else {
            s1++; p1 += Finfo.fsize;
        }
        fprintf(stdout, "%c%c%c%c%c %u/%02u/%02u %02u:%02u %9lu  %s\n",
                (Finfo.fattrib & AM_DIR) ? 'D' : '-',
                (Finfo.fattrib & AM_RDO) ? 'R' : '-',
                (Finfo.fattrib & AM_HID) ? 'H' : '-',
                (Finfo.fattrib & AM_SYS) ? 'S' : '-',
                (Finfo.fattrib & AM_ARC) ? 'A' : '-',
                (Finfo.fdate >> 9) + 1980, (Finfo.fdate >> 5) & 15, Finfo.fdate & 31,
                (Finfo.ftime >> 11), (Finfo.ftime >> 5) & 63,
                (DWORD)Finfo.fsize, Finfo.fname);
    }
    fprintf(stdout, "%4u File(s),%10lu bytes total\n%4u Dir(s)", s1, p1, s2);

    if(args[1] == NULL) {
        res = f_getfree( (const TCHAR*)".", (DWORD*)&p1, &fs);
    } else {
        res = f_getfree( (const TCHAR*)args[1], (DWORD*)&p1, &fs);
    }
    if (res == FR_OK) {
        fprintf(stdout, ", %10lu bytes free\n", p1 * (DWORD)(fs->csize * 512));
    } else {
        put_rc(res);
    }
    return 1;
}


/**
   @brief Builtin command:
   @param args List of args.  args[0] is "rm".  args[1] is the directory or file.
   @return Always returns 1, to continue executing.
 */
int8_t ya_rm(char ** args)      /* delete a directory or file */
{
    if (args[1] == NULL) {
        fprintf(stdout, "yash: expected 1 argument to \"rm\"\n");
    } else {
        put_rc(f_unlink((const TCHAR*)args[1]));
    }
    return 1;
}


/**
   @brief Builtin command:
   @param args List of args.  args[0] is "cp".  args[1] is the src, args[2] is the dst
   @return Always returns 1, to continue executing.
 */
int8_t ya_cp(char ** args)      /* copy a file */
{
    FRESULT res;
    uint32_t p1;
    uint16_t s1, s2;

#if !__RC2014 && !__CPM
    struct timespec startTime, endTime, resTime;
#endif

    if (args[1] == NULL || args[2] == NULL) {
        fprintf(stdout, "yash: expected 2 arguments to \"cp\"\n");
    } else {
        fprintf(stdout,"Opening \"%s\"\n", args[1]);
        res = f_open(&File[0], (const TCHAR*)args[1], FA_OPEN_EXISTING | FA_READ);
        if (res != FR_OK) { put_rc(res); return 1; }
        fprintf(stdout,"Creating \"%s\"\n", args[2]);
        res = f_open(&File[1], (const TCHAR*)args[2], FA_CREATE_ALWAYS | FA_WRITE);
        if (res != FR_OK) {
            put_rc(res);
            f_close(&File[0]);
            return 1;
        }
        fprintf(stdout,"Copying file...");
#if !__RC2014 && !__CPM
        clock_gettime( CLOCK_MONOTONIC, &startTime );
#endif
        p1 = 0;
        while (1) {
            res = f_read(&File[0], buffer, sizeof(char)*BUFFER_SIZE, &s1);
            if (res != FR_OK || s1 == 0) break;   /* error or eof */
            res = f_write(&File[1], buffer, s1, &s2);
            p1 += s2;
            if (res != FR_OK || s2 < s1) break;   /* error or disk full */
        }
#if !__RC2014 && !__CPM
        clock_gettime( CLOCK_MONOTONIC, &endTime );
#endif
        f_close(&File[1]);
        f_close(&File[0]);

        fprintf(stdout, "\nCopied %lu bytes", p1);

#if !__RC2014 && !__CPM
        timersub(&endTime, &startTime, &resTime);
        fprintf(stdout, ", the time taken was %li.%.4lu seconds", resTime.tv_sec, resTime.tv_nsec/100000);
#endif
        fprintf(stdout, "\n");
    }
    return 1;
}


/**
   @brief Builtin command:
   @param args List of args.  args[0] is "mv".  args[1] is the src, args[2] is the dst
   @return Always returns 1, to continue executing.
 */
int8_t ya_mv(char ** args)      /* move (rename) a file */
{
    if (args[1] == NULL || args[2] == NULL) {
        fprintf(stdout, "yash: expected 2 arguments to \"mv\"\n");
    } else {
        put_rc(f_rename((const TCHAR*)args[1],(const TCHAR*)args[2]));
    }
    return 1;
}


/**
   @brief Builtin command:
   @param args List of args.  args[0] is "cd".  args[1] is the directory.
   @return Always returns 1, to continue executing.
 */
int8_t ya_cd(char ** args)
{
    if (args[1] == NULL) {
        fprintf(stdout, "yash: expected 1 argument to \"cd\"\n");
    } else {
        put_rc(f_chdir((const TCHAR*)args[1]));
    }
    return 1;
}


/**
   @brief Builtin command:
   @param args List of args.  args[0] is "pwd".
   @return Always returns 1, to continue executing.
 */
int8_t ya_pwd(char ** args)     /* show the current working directory */
{
    FRESULT res;
    (void *)args;

    uint8_t * directory = (uint8_t *)malloc(sizeof(uint8_t)*LINE_SIZE);     /* Get area for directory name buffer */

    if (directory != NULL) {
        res = f_getcwd((char *)directory, sizeof(uint8_t)*LINE_SIZE);
        if (res != FR_OK) {
            put_rc(res);
        } else {
            fprintf(stdout, "%s", directory);
        }
        free(directory);
    }

    return 1;
}


/**
   @brief Builtin command:
   @param args List of args.  args[0] is "mkdir". args[1] is the directory.
   @return Always returns 1, to continue executing.
 */
int8_t ya_mkdir(char ** args)   /* create a new directory */
{
    if (args[1] == NULL) {
        fprintf(stdout, "yash: expected 1 argument to \"mkdir\"\n");
    } else {
        put_rc(f_mkdir((const TCHAR*)args[1]));
    }
    return 1;
}


/**
   @brief Builtin command:
   @param args List of args.  args[0] is "chmod".  args[1] is the directory.
   @return Always returns 1, to continue executing.
 */
int8_t ya_chmod(char ** args)   /* change file or directory attributes */
{
#if !FF_USE_CHMOD
    (void *)args;
#else
    if (args[1] == NULL && args[2] == NULL && args[3] == NULL) {
        fprintf(stdout, "yash: expected 3 arguments to \"chmod\"\n");
    } else {
        put_rc(f_chmod((const TCHAR*)args[1], atoi(args[2]), atoi(args[3])));
    }
#endif
    return 1;
}


/*
  disk related functions
 */


/**
   @brief Builtin command:
   @param args List of args.  args[0] is "ds". args[1] is the drive number.
   @return Always returns 1, to continue executing.
 */
int8_t ya_ds(char ** args)      /* disk status */
{
    FRESULT res;
    int32_t p1;
    const uint8_t ft[] = {0, 12, 16, 32};   // FAT type

    if (args[1] == NULL) {
        fprintf(stdout, "yash: expected 1 argument to \"ds\"\n");
        return 1;
    } else {
        res = f_getfree( (const TCHAR*)args[1], (DWORD*)&p1, &fs);
        if (res != FR_OK) { put_rc(res); return 1; }
    }

    fprintf(stdout, "FAT type = FAT%u\nBytes/Cluster = %lu\nNumber of FATs = %u\n"
        "Root DIR entries = %u\nSectors/FAT = %lu\nNumber of clusters = %lu\n"
        "Volume start (lba) = %lu\nFAT start (lba) = %lu\nDIR start (lba,cluster) = %lu\nData start (lba) = %lu\n",
        ft[fs->fs_type & 3], (DWORD)(fs->csize * 512), fs->n_fats,
        fs->n_rootdir, fs->fsize, (DWORD)fs->n_fatent - 2,
        fs->volbase, fs->fatbase, fs->dirbase, fs->database);
    return 1;
}


/**
   @brief Builtin command:
   @param args List of args.  args[0] is "dd". args[1] is the drive number. args[2] is the sector in decimal.
   @return Always returns 1, to continue executing.
 */
int8_t ya_dd(char ** args)      /* disk dump */
{
    FRESULT res;
    static BYTE drive;
    static uint32_t sect;
    uint16_t ofs;
    uint8_t * ptr;

    if (args[1] != NULL && args[2] != NULL) {
        drive = (BYTE)strtoul(args[1], NULL, 10);
        sect = strtoul(args[2], NULL, 10);
    }

    res = disk_read( drive, buffer, sect, 1);
    if (res != FR_OK) { fprintf(stdout, "rc=%d\n", (WORD)res); return 1; }
    fprintf(stdout, "Drive:%u LBA:%lu\n", drive, sect++);
    for (ptr=(uint8_t *)buffer, ofs = 0; ofs < 0x200; ptr += 16, ofs += 16)
        put_dump(ptr, ofs, 16);
    return 1;
}


/*
  time related functions
 */

#if !__RC2014 && !__CPM

/**
   @brief Builtin command:
   @param args List of args.  args[0] is "clock".  args[1] is the UNIX time.
   @return Always returns 1, to continue executing.
 */
int8_t ya_clock(char ** args)   /* set the time (using UNIX epoch) */
{
    if (args[1] != NULL) {
        set_system_time(atol(args[1]) - UNIX_OFFSET);
    }
    return 1;
}


/**
   @brief Builtin command:
   @param args List of args.  args[0] is "tz".  args[1] is TZ offset in hours.
   @return Always returns 1, to continue executing.
 */
int8_t ya_tz(char ** args)      /* set timezone (no daylight savings, so adjust manually) */
{
    if (args[1] != NULL) {
        set_zone(atol(args[1]) * ONE_HOUR);
    }
    return 1;
}


/**
   @brief Builtin command:
   @param args List of args.  args[0] is "diso".
   @return Always returns 1, to continue executing.
 */
int8_t ya_diso(char ** args)    /* print the local time in ISO std: 2013-03-23 01:03:52 */
{
    time_t theTime;
    struct tm CurrTimeDate;     // set up an array for the RTC info.
    char timeStore[26];

    (void *)args;

    time(&theTime);
    localtime_r(&theTime, &CurrTimeDate);
    isotime_r(&CurrTimeDate, timeStore);
    fprintf(stdout, "%s\n", timeStore);

    return 1;
}


/**
   @brief Builtin command:
   @param args List of args.  args[0] is "date".
   @return Always returns 1, to continue executing.
 */
int8_t ya_date(char ** args)    /* print the local time: Sun Mar 23 01:03:52 2013 */
{
    time_t theTime;
    struct tm CurrTimeDate;     // set up an array for the RTC info.
    char timeStore[26];

    (void *)args;

    time(&theTime);
    localtime_r(&theTime, &CurrTimeDate);
    asctime_r(&CurrTimeDate, timeStore);
    fprintf(stdout, "%s\n", timeStore);

    return 1;
}

#endif

/*
  helper functions
 */

/*  use put_rc to get a plain text interpretation of the disk return or error code. */
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
    fprintf(stderr,"\nrc=%u FR_%s\n", res, str);
}


static
void put_dump (const uint8_t * buff, uint16_t ofs, uint8_t cnt)
{
    uint8_t i;

    fprintf(stdout,"%04X:", ofs);

    for(i = 0; i < cnt; ++i) {
        fprintf(stdout," %02X", buff[i]);
    }
    fputc(' ', stdout);
    for(i = 0; i < cnt; ++i) {
        fputc((buff[i] >= ' ' && buff[i] <= '~') ? buff[i] : '.', stdout);
    }
    fputc('\n', stdout);
}


/*
  main loop functions
 */


/**
   @brief Execute shell built-in function.
   @param args Null terminated list of arguments.
   @return 1 if the shell should continue running, 0 if it should terminate
 */
int8_t ya_execute(char ** args)
{
    uint8_t i;

    if (args[0] == NULL) {
        // An empty command was entered.
        return 1;
    }

    for (i = 0; i < ya_num_builtins(); ++i) {
        if (strcmp(args[0], builtins[i].name) == 0) {
            return (*builtins[i].func)(args);
        }
    }
    return 1;
}


/**
   @brief Split a line into tokens (very naively).
   @param tokens, null terminated array of token pointers.
   @param line, the line.
 */
void ya_split_line(char ** tokens, char * line)
{
    uint16_t position = 0;
    char * token;

    if (tokens && line) {
        token = strtok(line, TOK_DELIM);

        while ((token != NULL) && (position < TOK_BUFSIZE-1)) {
            tokens[position++] = token;
            token = strtok(NULL, TOK_DELIM);
        }

        tokens[position] = NULL;
    }
}


/**
   @brief Loop getting input and executing it.
 */
void ya_loop(void)
{
    int8_t status;
    uint16_t len = LINE_SIZE-1;

    char * line = (char *)malloc(LINE_SIZE * sizeof(char));    /* Get work area for the line buffer */
    if (line == NULL) return;

    char ** args = (char **)malloc(TOK_BUFSIZE * sizeof(char*));    /* Get tokens buffer ready */
    if (args == NULL) return;

    do {
        fflush(stdin);
        fprintf(stdout,"\n> ");

#if __RC2014 && __CPM
        fgets_cons(line, (size_t)len);
#else
        getline(&line, &len, stdin);
#endif
        ya_split_line(args, line);

        status = ya_execute(args);

    } while (status);

    free(args);
    free(line);
}


/**
   @brief Main entry point.
   @param argc Argument count.
   @param argv Argument vector.
   @return status code
 */
int main(int argc, char ** argv)
{
    (void)argc;
    (void *)argv;

#if !__RC2014 && !__CPM
    set_zone((int32_t)10 * ONE_HOUR);               /* Australian Eastern Standard Time */
    set_system_time(1661990400 - UNIX_OFFSET);      /* Initial time: 00.00 September 1, 2022 UTC */
#endif

    fs = (FATFS *)malloc(sizeof(FATFS));                    /* Get work area for the volume */
    buffer = (char *)malloc(BUFFER_SIZE * sizeof(char));    /* Get working buffer space */

    // Run command loop if we got all the memory allocations we need.
    if (fs && buffer) {
        ya_loop();
    }

    // Perform any shutdown/cleanup.
    free(buffer);
    free(fs);

    return 0;
}

