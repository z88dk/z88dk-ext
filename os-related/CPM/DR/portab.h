/*****************************************************************************
*
*           C P / M   C   R U N   T I M E   L I B   H E A D E R   F I L E
*           -------------------------------------------------------------
*       Copyright 1982 by Digital Research Inc.  All rights reserved.
*
*       This is an include file for assisting the user to write portable
*       programs for C.
*
*****************************************************************************/

// z88dk specific definitions

// The z88dk command line parser breaks it, so we disable the feature
#pragma output CRT_ENABLE_COMMANDLINE=0
// Same for fcntl, the CP/M commands use BDOS
#pragma output CRT_OPEN_MAX=0
// Skip BSS data initialization.. risky optimization.
//#pragma output CRT_INITIALIZE_BSS=0
// Removing the MS DOS protection would save about 50 bytes, but it is nice to have
#pragma output noprotectmsdos


/* #un!define ALCYON 1 */                       /* using Alcyon compiler   */
/*      ALCYON undefined 9/3/82 by FZ   */
/*
 *      Standard type definitions
 */
                                                /***************************/
#define BYTE    char                            /* Signed byte             */
#define BOOLEAN unsigned char                   /* 2 valued (true/false)   */
#define WORD    short                           /* Signed word (16 bits)   */
#define UWORD   unsigned int                    /* unsigned word           */
#define LONG    long                            /* signed long (32 bits)   */
#define ULONG   unsigned long                   /* Unsigned long           */
#define REG     /**/                            /* register variable       */
#define LOCAL   auto                            /* Local var on 68000      */
#define EXTERN  extern                          /* External variable       */
#define MLOCAL  static                          /* Local to module         */
#define GLOBAL  /**/                            /* Global variable         */
#define VOID    void                            /* Void function return    */
                                                /***************************/
#ifdef ALCYON
#define UBYTE   char
#else
#define UBYTE   unsigned char                   /* Unsigned byte           */
#endif



/****************************************************************************/
/*      Miscellaneous Definitions:                                          */
/****************************************************************************/
#define FAILURE (-1)                    /*      Function failure return val */
#define SUCCESS (0)                     /*      Function success return val */
#define YES     1                       /*      "TRUE"                      */
#define NO      0                       /*      "FALSE"                     */
#define FOREVER for(;;)                 /*      Infinite loop declaration   */
#define NULL    0                       /*      Null pointer value          */
#define EOF     (-1)                    /*      EOF Value                   */
#define TRUE    (1)                     /*      Function TRUE  value        */
#define FALSE   (0)                     /*      Function FALSE value        */

/*************************** end of portab.h ********************************/
