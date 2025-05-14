/* muzcat_simple.c -- decompression filter in simple, portable C
 * by pts@fazekas.hu at Tue Jun 16 16:50:24 CEST 2020
 *
 * This tool is a slow drop-in replacmeent of zcat (gzip -cd), without error
 * handling. It reads compressed data (can be gzip, flate or zip format) on
 * stdin, and it writes uncompressed data to stdout. There is no error
 * handling: if the input is invalid, the tool may do anything.
 *
 * Modifications Ho-Ro:
 * * Error handling: check magic and compression mode before processing,
 *   calculate CRC32 during expanding and check against CRC32 of archive.
 * * Compiles for Linux: gcc -Wall -Wextra -Wpedantic -std=c89 -o gunzip gunzip.c
 *
 * * CP/M needs a hack to get the real file size (w/o trailing ^Z)
 *   and convert ^Z at end of file into C EOF (-1).
 *
 * * Compiles for CP/M using HI-TECH C: 'cc -v -dMYTZ=1 -n gunzip.c'.
 *   Do not optimise '-o', OPTIM.COM stops due to 'Out of memory'.
 *   Program becomes too big when using time and date functions from library, use own functions.
 *
 * * Compile for CP/M with z88dk:
 *   zcc +cpm --opt-code-speed -DMYTZ=1 -o gunzip.com gunzip.c
 *   Beware, the z88dk version is slower than the HTC version.
 *   Times for uncompressing a big file (w/o write to disk) on real CP/M: HTC: 2min, z88dk: 2min 35s
 *
 * * Usage:
 *   gunzip <infile>              show <infile> info
 *   gunzip <infile> <outfile>    decompress <infile> into <outfile>
 *   gunzip <infile> -o           decompress <infile> into original file name
 *   gunzip <infile> -n           decompress <infile>, do not create output
 *
 * The implementation was inspired by https://www.ioccc.org/1996/rcm/index.html
 *
 * Portability notes:
 *
 * * Source code is compatible with C89, C99 and newer. GCC, Clang and TinyCC
 *   do work. 16-bit C compilers should also work.
 * * Source code is compatible with owccdos (http://github.com/pts/owccdos).
 * * Source code isn't compatible with MesCC in GNU Mes 0.22, because MesCC
 *   doesn't support global arrays (initialized or uninitialized). Apart from
 *   that, it would compile.
 * * Feel free to change 'int16_t' and 'char' to int.
 * * It doesn't matter whether 'char' is signed or unsigned.
 * * The code works with any sizeof(short) and sizeof(int).
 * * The code doesn't use multiplication or division.
 * * On Windows, setmode(0, O_BINARY); and setmode(1, O_BINARY) are needed,
 *   otherwise the CRT inserts '\r' (CR) characters, breaking the decompression.
 * * For Linux/UNIX use gcc: 'gcc -o gunzip gunzip.c'
 *
 * Similar code:
 *
 * * https://www.ioccc.org/1996/rcm/index.html
 * * https://github.com/ioccc-src/winner/blob/master/1996/rcm/rcm.c
 * * https://gist.github.com/bwoods/a6a467430ed1c5f3fa35d01212146fe7
 */

#define VERSION "20250514"

#include <stdio.h>  /* FILE functions, getc(), putc(), etc. */
#include <stdint.h> /* intN_t and uintN_t */
#include <stdlib.h> /* exit(), calloc */
#include <string.h> /* strcmp() */
#ifdef __unix__
#include <time.h>
#endif


int8_t constW[] = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14,
                   1, 15 };
int16_t constU[] = { 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
                   35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258 };
int8_t constP[] = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3,
                  3, 4, 4, 4, 4, 5, 5, 5, 5, 0 };
int16_t constQ[] = { 1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
                   257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145,
                   8193, 12289, 16385, 24577 };
int8_t constL[] = { 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7,
                   8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13 };
int16_t D, C, T, J, Y;

uint16_t B[17];
uint16_t G[17];

#ifdef __Z88DK
/* these big arrays will be "stack"ed in main to keep them out of .bss in the z88dk binary */
uint16_t *Z; /* int16_t Z[320];     640 */
uint16_t *N; /* int16_t N[1998];   3996 */
uint8_t *S; /* uint8_t S[32768]; 32768 Dictionary == lookback buffer. */
/*                               ----- */
/*             Total .bss size:  37404 */
/*                               ===== */
#else
uint16_t Z[320];
uint16_t N[1998];
uint8_t S[32768]; /* Dictionary == lookback buffer. */
#endif

FILE *infile = NULL, *outfile = NULL;
char *inname = NULL, *outname = NULL;

#ifdef __Z88DK
/* buffered writing of single bytes to file */
#define OUTBUFSIZE 128
uint8_t outbuf[OUTBUFSIZE];
uint16_t outcnt = 0;
#endif

long bytecount;

/* Table of CRCs of all 8-bit messages. */
uint32_t crc_table[256];
uint32_t CRC32_gz, CRC32_calc = 0;


/* Make the table for a fast CRC. */
void make_crc_table(void) {
  uint32_t c;
  int16_t n, k;
  for (n = 0; n < 256; n++) {
    c = (uint32_t) n;
    for (k = 0; k < 8; k++) {
      if (c & 1) {
        c = 0xedb88320L ^ (c >> 1);
      } else {
        c = c >> 1;
      }
    }
    crc_table[n] = c;
  }
}


void init_crc( uint32_t c ) {
  CRC32_calc = c ^ 0xffffffffL;
}


void update_crc( uint8_t b ) {
  CRC32_calc = crc_table[(CRC32_calc ^ b) & 0xff] ^ (CRC32_calc >> 8);
}


uint32_t get_crc( void ) {
  return CRC32_calc ^ 0xffffffffL;
}


#ifdef __Z88DK
void chk_ctrl_c( void ) {
    if ( bdos( 6, 0xff ) == 3 ) { /* Ctrl C was typed */
        fprintf( stderr, "\n^C\n" );
        exit( -1 );
    }
}
#else
#define chk_ctrl_c()
#endif


/*
 * HACK for CP/M: ignore padding ^Z bytes at EOF
 */
int16_t getbyte() {
  static int heartbeat = 1024;
  if ( ++heartbeat >= 1024 ) {
    chk_ctrl_c();
    heartbeat = 0;
    fprintf( stderr, "<" );
    fflush( stderr );
  }
  if ( bytecount-- )
    return getc( infile );
  return EOF; /* C EOF */
}


int16_t putbyte( int16_t b ) {
  static int heartbeat = 1024;
  if ( ++heartbeat >= 1024 ) {
    chk_ctrl_c();
    heartbeat = 0;
    fprintf( stderr, ">" );
    fflush( stderr );
  }
  update_crc( b );
  if ( outfile ) {
#ifdef __Z88DK
    /*
     * HACK for z88dk: buffer the single byte writing
     */
    *(outbuf + outcnt) = b;
    if ( ++outcnt == OUTBUFSIZE ) {
      fwrite( outbuf, OUTBUFSIZE, 1, outfile );
      outcnt = 0;
    }
    return b;
#else
    return putc( b, outfile );
#endif
  } else
    return 0;
}


int16_t mc_bitread(int16_t arg) {
  int16_t oo, f;
  if (arg) {
    if (Y+7<arg) {
      J+=((((getbyte())&255))<<(Y));
      Y+=8;
    }
    if (Y<arg) {
      f=((getbyte())&255);
      if (arg==16) {
        oo=J+((((f)&((1<<(16-Y))-1)))<<(Y));
        J=((f)>>(16-Y));
      } else {
        oo=((J+((f)<<(Y)))&((1<<(arg))-1));
        J=((f)>>(arg-Y));
      }
      Y+=8-arg;
    } else {
      oo=((J)&((1<<(arg))-1));
      Y-=arg;
      J=((((J)>>(arg)))&((1<<(Y))-1));
    }
  } else {
    oo=0;
  }
  return oo;
}


int16_t mc_nalloc(void) {
  int16_t o;
  o=D;
  D=N[o];
  N[o]=0;
  return o;
}


void mc_free(int16_t arg) {
  if (arg!=0) {
    mc_free(N[arg]);
    mc_free(N[arg+1]);
    N[arg+1]=0;
    N[arg+2]=0;
    N[arg]=D;
    D=arg;
  }
}


int16_t mc_descend(int16_t arg) {
  while (N[arg]!=0) {
    if (mc_bitread(1)) {
      arg=N[arg+1];
    } else {
      arg=N[arg];
    }
  }
  return N[arg+2];
}


int16_t mc_mktree(int16_t arg) {
  int16_t oo, q, o, f;
  B[0]=0; B[1]=0; B[2]=0; B[3]=0; B[4]=0; B[5]=0; B[6]=0; B[7]=0; B[8]=0;
  B[9]=0; B[10]=0; B[11]=0; B[12]=0; B[13]=0; B[14]=0; B[15]=0; B[16]=0;
  oo=0;
  while (oo<arg) { ((B[Z[oo]])++); oo++; }
  B[0]=0;
  G[0]=0;
  G[1]=((G[0]+B[0])<<1); G[2]=((G[1]+B[1])<<1);
  G[3]=((G[2]+B[2])<<1); G[4]=((G[3]+B[3])<<1);
  G[5]=((G[4]+B[4])<<1); G[6]=((G[5]+B[5])<<1);
  G[7]=((G[6]+B[6])<<1); G[8]=((G[7]+B[7])<<1);
  G[9]=((G[8]+B[8])<<1); G[10]=((G[9]+B[9])<<1);
  G[11]=((G[10]+B[10])<<1); G[12]=((G[11]+B[11])<<1);
  G[13]=((G[12]+B[12])<<1); G[14]=((G[13]+B[13])<<1);
  G[15]=((G[14]+B[14])<<1); G[16]=((G[15]+B[15])<<1);
  N[3]=0;
  oo=0;
  while (oo<arg) {
    if (Z[oo]) {
      q=G[Z[oo]];
      ((G[Z[oo]])++);
      f=3;
      o=Z[oo];
      while (o) {
        o--;
        if (N[f]==0) {
          N[f]=mc_nalloc();
        }
        if ((0!=((q)&(((1)<<(o)))))) {
          f=N[f]+1;
        } else {
          f=N[f]+0;
        }
      }
      N[f]=mc_nalloc();
      N[N[f]+2]=oo;
    }
    oo++;
  }
  return N[3];
}


void mc_write(int16_t arg) {
  S[T]=arg;
  T++; T&=32767;
  if (T==C) {
    putbyte(S[C]);
    C++; C&=32767;
  }
}



#if defined HI_TECH_C | defined __Z88DK

typedef long time_t;

/* gmtime() taken from z88dk lib src */
#define SECS_PER_MINUTE ((time_t)60L)
#define SECS_PER_HOUR	((time_t)(60L * SECS_PER_MINUTE))
#define SECS_PER_DAY	((time_t)(24L * SECS_PER_HOUR))
#define SECS_PER_YEAR	((time_t)(365L * SECS_PER_DAY))
#define SECS_PER_LEAP	((time_t)(SECS_PER_YEAR+SECS_PER_DAY))

static int is_leap( int year ) {
  year += 1900; /* Get year, as ordinary humans know it */
  /*
   * The rules for leap years are not
   * as simple as "every fourth year
   * is leap year":
   */
  if( (unsigned int)year % 100 == 0 ) {
    return (unsigned int)year % 400 == 0;
  }
  return (unsigned int)year % 4 == 0;
}

int8_t __days_per_month[] = {31,28,31,30,31,30,31,31,30,31,30,31};

/* hard coded: timezone =  (GMT) */
#ifndef MYTZ
#define MYTZ 0 /* GMT */
#endif

char *isotime( time_t *tp ) {
  /* Creates time string in ISO format: */
  /* e.g. "2025-04-30 12:34:56\0"          */
  static char timestr[ 20 ];

  time_t t, secs_this_year;

  int8_t sec  = 0;
  int8_t min  = 0;
  int8_t hour = 0;
  int8_t mday = 1;
  int8_t mon  = 0;
  int year = 70;

  memset( timestr, 0, 20 );

  t = *tp + MYTZ * SECS_PER_HOUR;

  /*
   *	This loop handles dates in 1970 and later
   */
  while ( secs_this_year = is_leap(year) ? SECS_PER_LEAP : SECS_PER_YEAR,
          t >= secs_this_year ) {
      t -= secs_this_year;
      year++;
  }
  /*
   *	This loop handles dates before 1970
   */
  while ( t < 0 )
    t += is_leap(--year) ? SECS_PER_LEAP : SECS_PER_YEAR;

  if ( is_leap(year) )					/* leap year ? */
    __days_per_month[1]++;

  while ( t >= __days_per_month[mon] * SECS_PER_DAY ) {
    t -= __days_per_month[mon++] * SECS_PER_DAY;
  }

  if ( is_leap(year) )					/* leap year ? restore Feb */
      __days_per_month[1]--;

  while ( t >= SECS_PER_DAY ) {
    t -= SECS_PER_DAY;
    mday++;
  }
  while ( t >= SECS_PER_HOUR ) {
    t -= SECS_PER_HOUR;
    hour++;
  }
  while ( t >= SECS_PER_MINUTE ) {
    t -= SECS_PER_MINUTE;
    min++;
  }
  sec = t;

  sprintf( timestr, "%04d-%02d-%02d %02d:%02d:%02d",
            1900 + year, mon + 1, mday,
            hour, min, sec );

  return timestr;
}
#endif


/* open gzip and show archive info */
/* do not use time functions from lib due to too big CP/M program size */
FILE *gzip_open() {
#define RECSIZE 128
  FILE *fp;
  uint8_t *cp;
  uint8_t n;
  long lrpos;
  uint32_t ISIZE;
  time_t mtime;
#ifdef __unix__
  char timestr[20];
#endif

/* use part of global array uint8_t S[] as record buffer */

  if ( ( fp = fopen( inname, "rb" ) ) == NULL ) {
    perror( inname );
    exit( 2 );
  }

  fseek( fp, 0, SEEK_END ); /* go to EOF */
  bytecount = ftell( fp ); /* get position */
  if ( bytecount < 10 ) {
    fprintf( stderr, "%s: No gzip format\n", inname );
    exit( 2 );
  }
  lrpos = (bytecount - 1) & (long)(-RECSIZE); /* pos of last record */

  fseek( fp, lrpos, SEEK_SET ); /* go to last record */
  n = fread( S, 1, RECSIZE, fp ); /* read this record */
  if ( n == RECSIZE ) { /* search backwards for char != ^Z */
      cp = S + RECSIZE;
      n = RECSIZE+1;
      while( --n )
          if ( *--cp != 26 ) /* *real* last file position found */
              break;
  }
  bytecount = lrpos + n;

  fseek( fp, 0, SEEK_SET ); /* rewind infile */
  fread( S, 1, RECSIZE, fp ); /* read header */

  if ( S[ 0 ] != 0x1f || S[ 1 ] != 0x8b || S[ 2 ] != 0x08 ) {
    fprintf( stderr, "%s: Bad header\n", inname );
    exit( 2 );
  }
  /* fprintf( stderr, "XFL: %d, OS: %d - ", S[8], S[9] ); */
  fprintf( stderr, "compr. %ld -> ", bytecount );
  if ( S[ 3 ] == 0x08 ) {
    outname = (char *)(S+10);
    fprintf( stderr, "%s ", outname);
  }

  fseek( fp, bytecount - 8, SEEK_SET ); /* read 2 x uint32_t at end of file */
  fread( &CRC32_gz, 4, 1, fp ); /* pos: -8 */
  fread( &ISIZE, 4, 1, fp ); /* pos: -4 */

  /* get modification time of archive content and display in ISO 8601 format */
  mtime = *(uint32_t*)(S+4);
#ifdef __unix__
  strftime( timestr, sizeof timestr, "%Y-%m-%d %H:%M:%S", localtime( &mtime ) );
  fprintf( stderr, "%u %s\n", ISIZE, timestr );
#else
  /* use local isotime function */
  fprintf( stderr, "%lu %s\n", ISIZE, isotime( &mtime ) );
#endif

  fseek( fp, 0, SEEK_SET ); /* rewind infile */
  return fp;
}


void errexit( char *msg ) {
  fprintf( stderr, "%s\n", msg );
  exit( -1 );
}


int main(int argc, char **argv) {
  int16_t o, q, ty, oo, ooo, oooo, f, p, x, v, h, g;

#if defined __Z88DK
  /* these big arrays will be "stack"ed here to keep them out of .bss in the z88dk binary */
  uint16_t Z_on_stack[320];
  uint16_t N_on_stack[1998];
  uint8_t S_on_stack[32768];
  /* init to zero */
  memset( Z_on_stack, 0, sizeof Z_on_stack );
  memset( N_on_stack, 0, sizeof N_on_stack );
  memset( S_on_stack, 0, sizeof S_on_stack );
  /* provide global links */
  Z = Z_on_stack;
  N = N_on_stack;
  S = S_on_stack;
#endif

  if ( argc < 2 || argc > 3 ) {
    fprintf( stderr, "gunzip version %s\n", VERSION );
#ifdef CPM
/* CPM cannot access argv[0], args are converted to upper case */
#define OPTION_N "-N"
#define OPTION_O "-O"
    fprintf( stderr, "usage: gunzip <infile> [-n | -o | <outfile>]\n" );
#else
/* handle normal UNIX cmd line args */
#define OPTION_N "-n"
#define OPTION_O "-o"
    fprintf( stderr, "usage: %s <infile> [-n | -o | <outfile>]\n", argv[ 0 ] );
#endif
    return 1 ;
  }

  inname = argv[ 1 ];

  infile = gzip_open(); /* open file and show archive info */

  if ( argc == 2 ) { /* show only archive info, ready */
    fclose( infile );
    return 0;
  }

  /* no uncompressed name in gzip or no cmd line arg "-o" given  */
  if ( !outname || strcmp( argv[2], OPTION_O ) )
    outname = argv[ 2 ];

  if ( strcmp( argv[2], OPTION_N ) ) { /* no cmd line arg "-n" */
    if ( ( outfile = fopen( outname, "wb" ) ) == NULL ) {
      perror( outname );
      return 3;
    }
  }
  else
    outfile = NULL;

  make_crc_table();
  init_crc( 0 );

  /**********************************************/
  /* HIC SUNT DRACONES - do not touch below ... */
  /**********************************************/
  ty=3;
  while (ty!=4) {
    oo=0; ooo=0;
    J=0; Y=0; C=0; T=0;
    v=0; h=0;
    N[0]=0; N[1]=0; N[2]=0;
    N[3]=0; N[4]=0; N[5]=0;
    D=6;
    o=D;
    while (o<1998) {
      N[o]=o+3; o++;
      N[o]=0; o++;
      N[o]=0; o++;
    }
    ty=getbyte();
    if ((0!=((512+ty)&(256)))) {
      ty=4;
    } else if (ty==120) {
      mc_bitread(8);
    } else if (ty==80) {
      mc_bitread(8);
      o=mc_bitread(8);
      ty=3;
      if (o==3) {
        mc_bitread(8);
        mc_bitread(16);
        mc_bitread(16);
        ty=mc_bitread(8);
        mc_bitread(8);
        mc_bitread(16); mc_bitread(16);
        mc_bitread(16); mc_bitread(16);
        oo=mc_bitread(8); oo+=((mc_bitread(8))<<(8));
        ooo=mc_bitread(8); ooo+=((mc_bitread(8))<<(8));
        mc_bitread(16); mc_bitread(16);
        f=mc_bitread(8); f+=((mc_bitread(8))<<(8));
        q=mc_bitread(8); q+=((mc_bitread(8))<<(8));
        while (f) { mc_bitread(8); f--; }
        while (q) { mc_bitread(8); q--; }
      } else if (o==7) {
        o=0; while (o<13) { mc_bitread(8); o++; }
      } else if (o==5) {
        o=0; while (o<17) { mc_bitread(8); o++; }
        o=mc_bitread(8); o+=((mc_bitread(8))<<(8));
        while (o) { mc_bitread(8); o--; }
      } else if (o==1) {
        oo=0; while (oo<25) { mc_bitread(8); oo++; }
        f=mc_bitread(8); f+=((mc_bitread(8))<<(8));
        o=mc_bitread(8); o+=((mc_bitread(8))<<(8));
        q=mc_bitread(8); q+=((mc_bitread(8))<<(8));
        oo=0; while (oo<12) { mc_bitread(8); oo++; }
        while (f) { mc_bitread(8); f--; }
        while (o) { mc_bitread(8); o--; }
        while (q) { mc_bitread(8); q--; }
      }
    } else if (ty==31) {
      mc_bitread(16);
      o=mc_bitread(8);
      mc_bitread(16); mc_bitread(16); mc_bitread(16);
      if ((0!=((o)&(2)))) {
        mc_bitread(16);
      }
      if ((0!=((o)&(4)))) {
        q=mc_bitread(16);
        while (q) { mc_bitread(8); q--; }
      }
      if ((0!=((o)&(8)))) {
        while (mc_bitread(8)) {}
      }
      if ((0!=((o)&(16)))) {
        while (mc_bitread(8)) {}
      }
      if ((0!=((o)&(32)))) {
        f=0; while (f<12) { mc_bitread(8); f++; }
      }
    }
    if (ty==0) {
      while (oo) { g=getbyte(); putbyte(g); oo--; }
      while (ooo) {
        g=getbyte(); putbyte(g);
        g=getbyte(); putbyte(g);
        oo=32767;
        while (oo) {
          g=getbyte(); putbyte(g);
          g=getbyte(); putbyte(g);
          oo--;
        }
        ooo--;
      }
    } else if (ty==4) {
    } else if (ty!=3) {
      o=0;
      while (o==0) {
        o=mc_bitread(1);
        q=mc_bitread(2);
        if (q) {
          if (q==1) {
            oo=288;
            while (oo) {
              oo--;
                if (oo<144) {
                  Z[oo]=8;
                } else if (oo<256) {
                  Z[oo]=9;
                } else if (oo<280) {
                  Z[oo]=7;
                } else {
                  Z[oo]=8;
                }
            }
            v=mc_mktree(288);
            Z[0]=5; Z[1]=5; Z[2]=5; Z[3]=5; Z[4]=5; Z[5]=5; Z[6]=5; Z[7]=5;
            Z[8]=5; Z[9]=5; Z[10]=5; Z[11]=5; Z[12]=5; Z[13]=5; Z[14]=5; Z[15]=5;
            Z[16]=5; Z[17]=5; Z[18]=5; Z[19]=5; Z[20]=5; Z[21]=5; Z[22]=5; Z[23]=5;
            Z[24]=5; Z[25]=5; Z[26]=5; Z[27]=5; Z[28]=5; Z[29]=5; Z[30]=5; Z[31]=5;
            h=mc_mktree(32);
          } else {
            p=mc_bitread(5)+257;
            x=mc_bitread(5)+1;
            v=mc_bitread(4)+4;
            oo=0;
            while (oo<v) { Z[constW[oo]]=mc_bitread(3); oo++; }
            while (oo<19) { Z[constW[oo]]=0; oo++; }
            v=mc_mktree(19);
            ooo=0;
            oo=0;
            while (oo<p+x) {
              oooo=mc_descend(v);
              if (oooo==16) {
                oooo=ooo; f=3+mc_bitread(2);
              } else if (oooo==17) {
                oooo=0; f=3+mc_bitread(3);
              } else if (oooo==18) {
                oooo=0; f=11+mc_bitread(7);
              } else {
                ooo=oooo; f=1;
              }
              q=f;
              while (q) { Z[oo]=oooo; oo++; q--; }
            }
            mc_free(v);
            v=mc_mktree(p);
            oo=x;
            while (oo) { oo--; Z[oo]=Z[oo+p]; }
            h=mc_mktree(x);
          }
          oo=mc_descend(v);
          while (oo!=256) {
            if (oo<257) {
              mc_write(oo);
            } else {
              oo-=257;
              f=constU[oo]+mc_bitread(constP[oo]);
              oo=mc_descend(h);
              oo=constQ[oo]+mc_bitread(constL[oo]);
              if (T<oo) {
                oo=32768-oo+T;
              } else {
                oo=T-oo;
              }
              while (f) {
                mc_write(S[oo]);
                oo++; oo&=32767;
                f--;
              }
            }
            oo=mc_descend(v);
          }
          mc_free(v);
          mc_free(h);
        } else {
          mc_bitread((Y&7));
          oo=mc_bitread(16);
          mc_bitread(16);
          while (oo) { mc_write(mc_bitread(8)); oo--; }
        }
      }
      while (C!=T) {
        putbyte(S[C]);
        C++; C&=32767;
      }
    }
    mc_bitread(((Y)&7));
    if (ty==31) {
      mc_bitread(16); mc_bitread(16); mc_bitread(16); mc_bitread(16);
    } else if (ty==120) {
      mc_bitread(16); mc_bitread(16);
    }
  }

  fclose( infile );
  if ( outfile ) {
#ifdef __Z88DK
    if ( outcnt )  /* write remaining bytes in outbuf */
      fwrite( outbuf, 1, outcnt, outfile );
#endif
    fclose( outfile );
  }
  fprintf( stderr, "\n" );

  if ( get_crc() != CRC32_gz )
    fprintf( stderr, "CRC error\n" );
  return 0;
}
