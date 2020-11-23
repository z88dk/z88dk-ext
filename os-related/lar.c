
/* NOTE: the UNLZH option is under the GPL license */


/* With z88dk, library editing may not work on some target      */
/* '-DNOEDIT' will limit the tool (and its size) in those cases */
/* MAXFILES is set to 64 to reduce the program size

/* FULL program */
/* zcc +cpm -create-app -O3 --opt-code-size -DUNCRUNCH -DUSQ -DUNLZH -DTOUPPER lar.c */
/* (work in progress) zcc +cpm -create-app -SO3 --max-allocs-per-node400000 -DUNCRUNCH -DUSQ -DUNLZH -DTOUPPER -compiler=sdcc lar.c */

/* MINIMAL program */
/* zcc +cpm -create-app -O3 --opt-code-size -DTOUPPER -DNOEDIT lar.c */

/* --Unix/Windows/MAC version--
   gcc -DUNCRUNCH -DUSQ -DTOUPPER lar.c */



/* Auto-expansion of compressed files, Stefano Bodrato, Oct - 2019 */
/* '-DUSQ' will add the automatic UNSQUEEZing of the file when appropriate */
/* '-DUNCRUNCH' will add the automatic UNCRUNCHing of the file when appropriate */
/* '-DAMALLOC -DHAVE_CALLOC': NOT YET WORKING, TO BE FIXED ! */
/* zcc +cpm -create-app -O3 --opt-code-size -DTOUPPER -DUSQ -DUNCRUNCH lar.c */


/*% /bin/env - /bin/ncc -O lar.c -o lar
From linus!sch Tue Jul 26 08:07:37 1983
Subject: CP/M Lu library maintainer

When transfering files to my personal computer, I often want to transfer
several files at once using the Umodem program.  To do this I wrote
the following small program to combine files for the CP/M LU program.

No special treatment necessary, just:
	cc -O lar.c -o lar
to make it.

-- 
Stephen Hemminger,  Mitre Corp. Bedford MA 
	{allegra,genrad,ihnp4, utzoo}!linus!sch	(UUCP)
	linus!sch@mitre-bedford			(ARPA)
----------------- lar.c ----------------------
*/

/*
 * Lar - LU format library file maintainer
 * by Stephen C. Hemminger
 *	linus!sch	or	sch@Mitre-Bedford
 *
 *  Usage: lar key library [files] ...
 *
 *  Key functions are:
 *	u - Update, add files to library
 *	t - Table of contents
 *	e - Extract files from library
 *	p - Print files in library
 *	d - Delete files in library
 *	r - Reorganize library
 *  Other keys:
 *	v - Verbose
 *
 *  This program is public domain software, no warranty intended or
 *  implied.
 *
 *  DESCRPTION
 *     Lar is a Unix program to manipulate CP/M LU format libraries.
 *     The original CP/M library program LU is the product
 *     of Gary P. Novosielski. The primary use of lar is to combine several
 *     files together for upload/download to a personal computer.
 *
 *  PORTABILITY
 *     The code is modeled after the Software tools archive program,
 *     and is setup for Version 7 Unix.  It does not make any assumptions
 *     about byte ordering, explict and's and shift's are used.
 *     If you have a dumber C compiler, you may have to recode new features
 *     like structure assignment, typedef's and enumerated types.
 *
 *  BUGS/MISFEATURES
 *     The biggest problem is text files, the programs tries to detect
 *     text files vs. binaries by checking for non-Ascii (8th bit set) chars.
 *     If the file is text then it will throw away Control-Z chars which
 *     CP/M puts on the end.  All files in library are padded with Control-Z
 *     at the end to the CP/M sector size if necessary.
 *
 *     No effort is made to handle the difference between CP/M and Unix
 *     end of line chars.  CP/M uses Cr/Lf and Unix just uses Lf.
 *     The solution is just to use the Unix command sed when necessary.
 *
 *  * Unix is a trademark of Bell Labs.
 *  ** CP/M is a trademark of Digital Research.
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>

#ifdef HAVE_CALLOC
#include <malloc.h>
#endif


unsigned char *ptr;
unsigned char outfile[80];	/*space to build output file name*/
FILE 	*infd;			/*currently open input file*/
FILE 	*outfd;			/*currently open output file*/


#if (defined(UNCRUNCH)||defined(UNLZH)||defined(USQ))
/*
 cisubstr(string, token) searches for lower case token in string s
 returns pointer to token within string if found, NULL otherwise
*/
#define cisubstr(s,t) strstr(s,t)

/*extract, build and print output file name*/
void getfilename() {
#ifdef TOUPPER
	for(ptr=outfile; (*ptr=getc(infd))!='\0'; ptr++) *ptr=toupper(*ptr);
#else
	for(ptr=outfile; (*ptr=getc(infd))!='\0'; ptr++) *ptr=tolower(*ptr);
#endif
	*(cisubstr(outfile,".")+4)='\0'; /*truncate non-name portion*/
	printf(" -> %s",outfile);
}

#endif


#if (defined(USQ)||defined(UNLZH))
#define MAGIC_UNCR	0xfe76

/* for bit/byte reader */
unsigned int getbuf;

/* get 16-bit word from file */
int getw16()
{
int temp;

temp = getc(infd);		/* get low order byte */
temp |= getc(infd) << 8;
if (temp & 0x8000) temp |= (~0) << 15;	/* propogate sign for big ints */
return temp;

}

/* get 16-bit (unsigned) word from file */
int getx16()
{
int temp;

temp = getc(infd);		/* get low order byte */
return temp | (getc(infd) << 8);

}
#endif




// ------------------------------------------------------------------------------------------

#ifdef UNLZH

/*
 * THIS PROGRAM SECTION IS UNDER THE GPL LICENSE
 *
 * lbrate 1.0 - fully extract CP/M `.lbr' archives.
 * Copyright (C) 2001 Russell Marks. See main.c for license details.
 *
 * readlzh.c - read LZH-compressed files.
 *
 * This is based on the well-known lzhuf.c. Since the original
 * licence was at best ambiguous, I asked all three authors if
 * I could use a modified version of lzhuf.c in a GPL'd program,
 * and the two who responded agreed.
 *
 * (The third, Yoshizaki, is thought by Okumura not likely to object,
 * perhaps since his code was based on Okumura's lzari.c - which has
 * always been under the licence mentioned below. :-))
 *
 * The following reflects what they consider to be the real licence
 * on lzhuf.c:
 *
 * lzhuf.c
 * Copyright (C) 1989 Haruhiko Okumura, Haruyasu Yoshizaki, and Kenji Rikitake.
 * Use, distribute, and modify this program freely.
 */


/********** LZSS compression **********/

#define MAGIC_LZH	0xfd76

/* these are the values required for the "Y" format */
#define LZ_N		2048
#define LZ_F		60
#define THRESHOLD	2

static unsigned int checksum;
static int oldver,lastchar;

unsigned char text_buf[LZ_N + LZ_F - 1];


/* Huffman coding */

#define N_CHAR		(256 + 1 - THRESHOLD + LZ_F)
/* kinds of characters (character code = 0..N_CHAR-1) */
#define LZ_T		(N_CHAR * 2 - 1)	/* size of table */
#define LZ_R		(LZ_T - 1)		/* position of root */
#define MAX_FREQ	0x8000		/* updates tree when the */
				   /* root frequency comes to this value. */


/* table for decoding the upper 6 bits of position */

/* for decoding */
unsigned char d_code[256] =
  {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
  0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
  0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
  0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
  0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
  0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
  0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
  0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
  0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
  0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
  0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
  0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
  0x0C, 0x0C, 0x0C, 0x0C, 0x0D, 0x0D, 0x0D, 0x0D,
  0x0E, 0x0E, 0x0E, 0x0E, 0x0F, 0x0F, 0x0F, 0x0F,
  0x10, 0x10, 0x10, 0x10, 0x11, 0x11, 0x11, 0x11,
  0x12, 0x12, 0x12, 0x12, 0x13, 0x13, 0x13, 0x13,
  0x14, 0x14, 0x14, 0x14, 0x15, 0x15, 0x15, 0x15,
  0x16, 0x16, 0x16, 0x16, 0x17, 0x17, 0x17, 0x17,
  0x18, 0x18, 0x19, 0x19, 0x1A, 0x1A, 0x1B, 0x1B,
  0x1C, 0x1C, 0x1D, 0x1D, 0x1E, 0x1E, 0x1F, 0x1F,
  0x20, 0x20, 0x21, 0x21, 0x22, 0x22, 0x23, 0x23,
  0x24, 0x24, 0x25, 0x25, 0x26, 0x26, 0x27, 0x27,
  0x28, 0x28, 0x29, 0x29, 0x2A, 0x2A, 0x2B, 0x2B,
  0x2C, 0x2C, 0x2D, 0x2D, 0x2E, 0x2E, 0x2F, 0x2F,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
  0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
  };

unsigned char d_len[256] =
  {
  0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
  0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
  0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
  0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
  0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
  0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
  0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
  0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
  0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
  0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
  0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
  0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
  0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
  0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
  0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
  0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
  0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
  0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
  0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
  0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
  0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
  0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
  0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
  0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
  0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
  0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
  0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
  0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
  0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
  0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
  0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
  0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
  };

static unsigned freq[LZ_T + 1];     /* frequency table */

/* pointers to parent nodes, except for the elements [LZ_T..LZ_T + N_CHAR - 1]
 * which are used to get the positions of leaves corresponding to the codes.
 */
static int prnt[LZ_T + N_CHAR];

/* pointers to child nodes (son[], son[] + 1) */
static int son[LZ_T];

/* for bit/byte reader */
//unsigned int getbuf=0;
unsigned char getlen=0;

#define ALLOC_BLOCK_SIZE	32768

static unsigned char *data_in_point,*data_in_max;
static unsigned char *data_out,*data_out_point;
static int data_out_len,data_out_allocated;


#define rawinput() getc(infd)
/*
static int rawinput(void)
{
if(data_in_point<data_in_max)
  return(*data_in_point++);
return(-1);
}*/


#ifdef Z80
void rawoutput(int byte) __z88dk_fastcall
#else
void rawoutput(int byte)
#endif
{
	putc(byte,outfd);
	checksum+=byte;
}

/*
static void rawoutput(int byte)
{
if(data_out_len>=data_out_allocated)
  {
  data_out_allocated+=ALLOC_BLOCK_SIZE;
  if((data_out=realloc(data_out,data_out_allocated))==NULL)
    fprintf(stderr,"lbrate: out of memory!\n"),exit(1);
  data_out_point=data_out+data_out_len;
  }

*data_out_point++=byte;
data_out_len++;
checksum+=byte;
}*/

static int lz_getbit(void)
{
int i;

while (getlen <= 8)
  {
  if ((lastchar = i = rawinput()) < 0) i = 0;
  getbuf |= i << (8 - getlen);
  getlen += 8;
  }
i = getbuf;
getbuf <<= 1;
getlen--;
return ((i&0x8000)?1:0);
}

static int getbyte(void)
{
int i;

while (getlen <= 8)
  {
  if ((lastchar = i = rawinput()) < 0) i = 0;
  getbuf |= i << (8 - getlen);
  getlen += 8;
  }
i = getbuf;
getbuf <<= 8;
getlen -= 8;
return ((i >> 8)&255);
}



/* initialization of tree */

void start_huff(void)
{
int i, j;

for (i = 0; i < N_CHAR; i++)
  {
  freq[i] = 1;
  son[i] = i + LZ_T;
  prnt[i + LZ_T] = i;
  }
i = 0; j = N_CHAR;
while (j <= LZ_R)
  {
  freq[j] = freq[i] + freq[i + 1];
  son[j] = i;
  prnt[i] = prnt[i + 1] = j;
  i += 2; j++;
  }
freq[LZ_T] = 0xffff;
prnt[LZ_R] = 0;
}


/* reconstruction of tree */

void reconst(void)
{
int i, j, k;
unsigned f;

/* collect leaf nodes in the first half of the table */
/* and replace the freq by (freq + 1) / 2. */
j = 0;
for (i = 0; i < LZ_T; i++)
  {
  if (son[i] >= LZ_T)
    {
    freq[j] = (freq[i] + 1) / 2;
    son[j] = son[i];
    j++;
    }
  }

/* begin constructing tree by connecting sons */
for (i = 0, j = N_CHAR; j < LZ_T; i += 2, j++)
  {
  k = i + 1;
  f = freq[j] = freq[i] + freq[k];
  for (k = j - 1; f < freq[k]; k--);
  k++;
  memmove(freq+k+1,freq+k,(j-k)*sizeof(freq[0]));
  freq[k]=f;
  memmove(son+k+1,son+k,(j-k)*sizeof(son[0]));
  son[k]=i;
  }

/* connect prnt */
for (i = 0; i < LZ_T; i++)
  {
  if ((k = son[i]) >= LZ_T)
    prnt[k] = i;
  else
    prnt[k] = prnt[k + 1] = i;
  }
}


/* increment frequency of given code by one, and update tree */

void lzupdate(int c)
{
int i, j, k, l;

if (freq[LZ_R] == MAX_FREQ)
  reconst();

c = prnt[c + LZ_T];
do
  {
  k = ++freq[c];

  /* if the order is disturbed, exchange nodes */
  if (k > freq[l = c + 1])
    {
    while (k > freq[++l]);
    l--;
    freq[c] = freq[l];
    freq[l] = k;

    i = son[c];
    prnt[i] = l;
    if (i < LZ_T) prnt[i + 1] = l;

    j = son[l];
    son[l] = i;

    prnt[j] = c;
    if (j < LZ_T) prnt[j + 1] = c;
    son[c] = j;

    c = l;
    }
  }
while ((c = prnt[c]) != 0);   /* repeat up to root */
}


int decode_char(void)
{
unsigned c;

c = son[LZ_R];

/* travel from root to leaf,
 * choosing the smaller child node (son[]) if the read bit is 0,
 * the bigger (son[]+1) if 1.
 */
while (c < LZ_T)
  {
  c += lz_getbit();
  c = son[c];
  }
c -= LZ_T;
lzupdate(c);
return c;
}

int decode_position(void)
{
unsigned i, j, c;

/* recover upper bits from table */
i = getbyte();
c = (unsigned)d_code[i] << (5+oldver);	/* 5, or 6 for 1.x */
j = d_len[i];

/* read lower bits verbatim */
j -= 3-oldver;				/* 3, or 2 for 1.x */
while (j--)
  {
  i = (i << 1) + lz_getbit();
  }
return c | (i & (oldver?0x3f:0x1f));	/* 0x1f, or 0x3f for 1.x */
}



// #define READ_WORD(x) (x)=rawinput(),(x)|=(rawinput()<<8)




#ifdef Z80
void unlzh(char *infile) __z88dk_fastcall
#else
void unlzh(char *infile)
#endif
{
int c,v,checktype;
unsigned int orig_checksum;
int i,j,k,r;

	if(!(infd=fopen(infile, "rb"))) {
		//printf("Can't open %s\n", infile);
		return;
	}
	
	//READ_WORD(magic);
	/* Process header */
	if(getx16() != MAGIC_LZH) {
		//printf(" not a compressed file\n");
		fclose(infd);
		return;
	}

	//filecrc = getw16();

	getfilename();
	printf(": lzh,");
	
	if(!(outfd=fopen(outfile, "wb"))) {
		printf("Can't create %s\n", outfile);
		fclose(infd);
		fclose(outfd);
		return;
	}

	
	/* four info bytes */
	rawinput();
	oldver=((v=rawinput())<0x20?1:0);
	checktype=rawinput();
	rawinput();

	getbuf=0;
	getlen=0;
	checksum=0;

	start_huff();

	r=LZ_N-LZ_F;
	memset(text_buf,32,r);

	while((c=decode_char())!=256)	/* 256 = EOF */
	  {
	  if(c<256)
		{
		rawoutput(c);
		text_buf[r++] = c;
		r&=(LZ_N-1);
		}
	  else
		{
		i=(r-decode_position()-1)&(LZ_N-1);
		j=c-256+THRESHOLD;
		for (k = 0; k < j; k++)
		  {
		  c=text_buf[(i+k)&(LZ_N-1)];
		  rawoutput(c);
		  text_buf[r++]=c;
		  r&=(LZ_N-1);
		  }
		}
	  }

	/* lastchar junk is needed because bit(/byte) reader reads a byte
	 * in advance.
	 */
	orig_checksum=lastchar;
	orig_checksum+=256*rawinput();

	/* see how the checksum turned out */
	#ifndef Z80
	checksum&=0xffff;
	#endif
	if(checktype==0 && checksum!=orig_checksum)
	  {
			printf("***** checksum error\n");
	  } else {
			fclose(infd);
			remove(infile);
			printf(" done.");
	  }

	fclose(outfd);
	fclose(infd);
}

#endif // UNLZH



// ------------------------------------------------------------------------------------------

/*  USQ function extracted from source version 3.2, 23/03/2012
	..in turn derived from the historical version 3.1 (12/19/84)  */

#ifdef USQ


#define SPEOF 256	/* special endfile token */
#define NUMVALS 257	/* 256 data values plus SPEOF*/

#define ERROR -1
#define DLE 0x90
#define RECOGNIZE 0xFF76	/* unlikely pattern */

#define LARGE 30000

unsigned int crc;	/* error check code */

#ifdef	HAVE_CALLOC
struct {
	int children[2];	/* left, right */
} *dnode;
#else
/* Decoding tree */
struct {
	int children[2];	/* left, right */
} dnode[NUMVALS - 1];
#endif

int bpos;	/* last bit position read */
int curin;	/* last byte value read */

/* Variables associated with repetition decoding */
int repct;	/*Number of times to retirn value*/
int value;	/*current byte value or EOF */

/* This must follow all include files */
unsigned int dispcnt;	/* How much of each file to preview */
char	ffflag;		/* should formfeed separate preview from different files */




/* Decode file stream into a byte level code with only
 * repetition encoding remaining.
 */ 
int getuhuff()
{
	int i;

	/* Follow bit stream in tree to a leaf*/
	i = 0;	/* Start at root of tree */
	do {
		if(++bpos > 7) {
			if((curin = getc(infd)) == ERROR)
				return ERROR;
			bpos = 0;
			/* move a level deeper in tree */
			i = dnode[i].children[1 & curin];
		} else
			i = dnode[i].children[1 & (curin >>= 1)];
	} while(i >= 0);

	/* Decode fake node index to original data value */
	i = -(i + 1);
	/* Decode special endfile token to normal EOF */
	i = (i == SPEOF) ? EOF : i;
	return i;
}

/* Get bytes with decoding - this decodes repetition,
 * calls getuhuff to decode file stream into byte
 * level code with only repetition encoding.
 *
 * The code is simple passing through of bytes except
 * that DLE is encoded as DLE-zero and other values
 * repeated more than twice are encoded as value-DLE-count.
 */
int getcr()
{
	int c;

	if(repct > 0) {
		/* Expanding a repeated char */
		--repct;
		return value;
	} else {
		/* Nothing unusual */
		if((c = getuhuff()) != DLE) {
			/* It's not the special delimiter */
			value = c;
			if(value == EOF)
				repct = LARGE;
			return value;
		} else {
			/* Special token */
			if((repct = getuhuff()) == 0)
				/* DLE, zero represents DLE */
				return DLE;
			else {
				/* Begin expanding repetition */
				repct -= 2;	/* 2nd time */
				return value;
			}
		}
	}
}


#ifdef Z80
void unsqueeze(char *infile) __z88dk_fastcall
#else
void unsqueeze(char *infile)
#endif
{
	int i, c;
	char cc;

	unsigned int filecrc;	/* checksum */
	int numnodes;		/* size of decoding tree */
	unsigned int linect;	/* count of number of lines previewed */
	char obuf[128];		/* output buffer */
	int oblen;		/* length of output buffer */
	static char errmsg[] = "ERROR - write failure in %s\n";

#ifdef	HAVE_CALLOC
	dnode=calloc((size_t) NUMVALS, sizeof(dnode));
	if (!dnode) { 
		printf(" -->  out of memory");
		return;
		}
#endif

	if(!(infd=fopen(infile, "rb"))) {
		//printf("Can't open %s\n", infile);
		return;
	}
	/* Initialization */
	linect = 0;
	crc = 0;
	repct = 0;		/* initialize decoding functions */
	bpos = 99;		/* force initial read */

	/* Process header */
	if(getx16() != RECOGNIZE) {
		//printf(" is not a squeezed file\n");
		goto closein;
	}

	filecrc = getw16();

	/* Get original file name */
	
	//ptr = outfile;			/* send it to array */
	/*do {
		*ptr = getc(infd);
	} while(*ptr++ != '\0');
	
	printf(" -> %s: ", outfile);*/
	
	getfilename();

	numnodes = getw16();

	if(numnodes < 0 || numnodes >= NUMVALS) {
		printf("%s has invalid decode tree size\n", infile);
		goto closein;
	}

	/* Initialize for possible empty tree (SPEOF only) */
	dnode[0].children[0] = -(SPEOF + 1);
	dnode[0].children[1] = -(SPEOF + 1);

	/* Get decoding tree from file */
	for(i = 0; i < numnodes; ++i) {
		dnode[i].children[0] = getw16();
		dnode[i].children[1] = getw16();
	}

	if(dispcnt) {
		/* Use standard output for previewing */
		putchar('\n');
		while(((c = getcr(infd)) != EOF) && (linect < dispcnt)) {
			cc = 0x7f & c;	/* strip parity */
			if((cc < ' ') || (cc > '~'))
				/* Unprintable */
				switch(cc) {
				case '\r':	/* return */
					/* newline will generate CR-LF */
					goto next;
				case '\n':	/* newline */
					++linect;
				case '\f':	/* formfeed */
				case '\t':	/* tab */
					break;
				default:
					cc = '.';
				}
			putchar(cc);
		next: ;
		}
		if(ffflag)
			putchar('\f');	/* formfeed */
	} else {
		/* Create output file */
		if(!(outfd=fopen(outfile, "wb"))) {
			printf("Can't create %s\n", outfile);
			goto closeall;
		}
		printf(": sq,");
		/* Get translated output bytes and write file */
		oblen = 0;
		while((c = getcr(infd)) != EOF) {
			crc += c;
			obuf[oblen++] = c;
			if (oblen >= sizeof(obuf)) {
				if(!fwrite(obuf, sizeof(obuf), 1, outfd)) {
					printf(errmsg, outfile);
					goto closeall;
				}
				oblen = 0;
			}
		}
		if (oblen && !fwrite(obuf, oblen, 1, outfd)) {
			printf(errmsg, outfile);
			goto closeall;
		}

#ifdef Z80
		if(filecrc != crc )
#else
		if((filecrc && 0xFFFF) != (crc && 0xFFFF))
#endif
			printf("***** checksum error\n");
		else {
			fclose(infd);
			remove(infile);
			printf(" done.");
		}

	closeall:
		fclose(outfd);
	}

closein:
	fclose(infd);
#ifdef HAVE_CALLOC
	free(dnode);
#endif
}


#endif  // USQ




// ------------------------------------------------------------------------------------------


#ifdef UNCRUNCH

/*Macro definition - ensure letter is lower case*/
//#define tolower(c) (((c)>='A' && (c)<='Z')?(c)-('A'-'a'):(c))

#define TABLE_SIZE  4096	/*size of main lzw table for 12 bit codes*/
#define XLATBL_SIZE 5003	/*size of physical translation table*/

/*special values for predecessor in table*/
#define NOPRED 0x6fff		/*no predecessor in table*/
#define EMPTY  0x8000		/*empty table entry (xlatbl only)*/
#define REFERENCED 0x2000	/*table entry referenced if this bit set*/
#define IMPRED 0x7fff		/*impossible predecessor*/

#define EOFCOD 0x100		/*special code for end-of-file*/
#define RSTCOD 0x101		/*special code for adaptive reset*/
#define NULCOD 0x102		/*special filler code*/
#define SPRCOD 0x103		/*spare special code*/

#define REPEAT_CHARACTER 0x90	/*following byte is repeat count*/

#ifdef	HAVE_CALLOC

  /*main lzw table and it's structure*/
  struct entry {
	int predecessor;	/*index to previous entry, if any*/
	unsigned char suffix;		/*character suffixed to previous entries*/
  } *lzw_table;
  
  /*auxilliary physical translation table*/
  /*translates hash to main table index*/
  unsigned int *xlatbl;

  /*byte string stack used by decode*/
  unsigned char *stack;

#else

  struct entry {
	int predecessor;	/*index to previous entry, if any*/
	unsigned char suffix;	/*character suffixed to previous entries*/
  } lzw_table[TABLE_SIZE];

  /*auxilliary physical translation table*/
  /*translates hash to main table index*/
  unsigned int xlatbl[XLATBL_SIZE];

  /*byte string stack used by decode*/
  unsigned char stack[TABLE_SIZE];

#endif

/*other global variables*/
unsigned char	codlen;		/*variable code length in bits (9-12)*/
int	trgmsk;			/*mask for codes of current length*/
unsigned char	fulflg;		/*full flag - set once main table is full*/
int	entry;			/*next available main table entry*/
long	cr_getbuf;			/*buffer used by getcode*/
int	getbit;			/*residual bit counter used by getcode*/
unsigned char	entflg; 	/*inhibit main loop from entering this code*/
unsigned char	repeat_flag;	/*so send can remember if repeat required*/
int	finchar;		/*first character of last substring output*/
int	lastpr;			/*last predecessor (in main loop)*/
int	cksum;			/*checksum of all bytes written to output file*/



/*hash pred/suff into xlatbl pointer*/
/*duplicates the hash algorithm used by CRUNCH 2.3*/
unsigned int *hash(int pred,int suff,int *disploc)
	{
	unsigned int hashval;
	
	hashval=((((pred>>4) & 0xff) ^suff) | ((pred&0xf)<<8)) + 1;
	*disploc=hashval-XLATBL_SIZE;
	return (xlatbl + hashval);
	}


/*find an empty entry in xlatbl which hashes from this predecessor/suffix*/
/*combo, and store the index of the next available lzw table entry in it*/
void figure(int pred, int suff)
	{
	//unsigned int *hash(int pred,int suff,int *disploc);
	auto int disp;
	unsigned int *p;
	p=hash(pred,suff,&disp);

	/*follow secondary hash chain as necessary to find an empty slot*/
#ifdef Z80
	while((*p) != EMPTY)
#else
	while(((*p)&0xffff) != EMPTY)
#endif
		{
		p+=disp;
		if(p<xlatbl || p > xlatbl+XLATBL_SIZE)
			p+=XLATBL_SIZE;
		}

	/*stuff next available index into this slot*/
	*p=entry;
	}



/*enter the next code into the lzw table*/
void enterx(int pred, int suff)
/* int pred;		table index of predecessor*/
/* int suff;		suffix byte represented by this entry*/
	{
	register struct entry *ep;
	ep = &lzw_table[entry];


	/*update xlatbl to point to this entry*/
	figure(pred,suff);

	/*make the new entry*/
	ep->predecessor = (int)pred;
	ep->suffix = (unsigned char)suff;
	entry++;

	/*if only one entry of the current code length remains, update to*/
	/*next code length because main loop is reading one code ahead*/
	if(entry >= trgmsk)
		{
		if(codlen<12)
			{
			/*table not full, just make length one more bit*/
			codlen++;
			trgmsk=(trgmsk<<1)|1;
			}
		else
			{
			/*table almost full (fulflg==0) or full (fulflg==1)*/
			/*just increment fulflg - when it gets to 2 we will*/
			/*never be called again*/
			fulflg++;
			}
		}
	}



/*initialize the lzw and physical translation tables*/
void initb2()
	{
	register int i;
	register struct entry *p;
	p=lzw_table;

	/*first mark all entries of xlatbl as empty*/
	for(i=0;i<XLATBL_SIZE;i++) xlatbl[i]=EMPTY;

	/*enter atomic and reserved codes into lzw table*/
	for(i=0;i<0x100;i++) enterx(NOPRED,i);	/*first 256 atomic codes*/
	for(i=0;i<4;i++) enterx(IMPRED,0);	/*reserved codes*/
	}
	




/*initialize variables for each file to be uncrunched*/
void intram()
	{
	trgmsk=0x1ff;	/*nine bits*/
	codlen=9;	/*    "    */
	fulflg=0;	/*table empty*/
	entry=0;	/*    "      */
	getbit=0;	/*buffer emtpy*/
	entflg=1;	/*first code always atomic*/
	repeat_flag=0;	/*repeat not active*/
	cksum=0;	/*zero checsum*/
	cr_getbuf=0;
	}


/*return a code of length "codlen" bits from the input file bit-stream*/
unsigned int getcode()
	{
	register int hole;
	int code;

	/*always get at least a byte*/
	cr_getbuf=(cr_getbuf<<codlen)|(((long)getc(infd))<<(hole=codlen-getbit));
	getbit=8-hole;

	/*if is not enough to supply codlen bits, get another byte*/
	if(getbit<0)
		{
		cr_getbuf |= ((long)getc(infd))<<(hole-8);
		getbit+=8;
		}

	if(feof(infd))
		{
		printf("***** Unexpected EOF !\n");
		return EOFCOD;
		}

	/*skip spare or null codes*/
	if((code=((cr_getbuf>>8) & trgmsk)) == NULCOD || code == SPRCOD)
		{
		return getcode();	/*skip this code, get next*/
		}

	return code;
	}


/*write a byte to output file*/
/*repeat byte (0x90) expanded here*/
/*checksumming of output stream done here*/
#ifdef Z80
void send(unsigned char c) __z88dk_fastcall
#else
void send(unsigned char c)
#endif
	{
	static unsigned char savec;	/*previous byte put to output*/

	/*repeat flag may have been set by previous call*/
	if(repeat_flag)
		{

		/*repeat flag was set - emit (c-1) copies of savec*/
		/*or (if c is zero), emit the repeat byte itself*/
		repeat_flag=0;
		if(c)
			{
			cksum+=(savec&0xff)*(c-1);
			while(--c){
				putc(savec,outfd);
			}
			}
		else
			{
			putc(REPEAT_CHARACTER,outfd);
			cksum+=REPEAT_CHARACTER;
			}
		}
	else
		{
		/*normal case - emit c or set repeat flag*/
		if(c==REPEAT_CHARACTER)
			{
			repeat_flag++;
			}
		else
			{
			putc(savec=c,outfd);
			cksum+=(c&0xff);
			}
		}
	}
	

/*decode this code*/
#ifdef Z80
int decode(int code) __z88dk_fastcall
#else
int decode(int code)
#endif
	{
	register unsigned char *stackp;		/*byte string stack pointer*/
	register struct entry *ep;
	ep = &lzw_table[code];

	if(code>=entry)
		{
		/*the ugly exception, "WsWsW"*/
		entflg=1;
		enterx(lastpr,finchar);
		}

	/*mark corresponding table entry as referenced*/
	ep->predecessor |= REFERENCED;

	/*walk back the lzw table starting with this code*/
	stackp=stack;
	while(ep > &lzw_table[255]) /*i.e. code not atomic*/
		{
		*stackp++ = ep->suffix;
		ep = &lzw_table[(ep->predecessor)&0xfff];
		}

	/*then emit all bytes corresponding to this code in forward order*/
	send(finchar=(ep->suffix)&0xff); /*first byte*/

	while(stackp > stack)		 /*the rest*/
		{
		send(*--stackp);
		}

	return(entflg);
	}




/*attempt to reassign an existing code which has*/
/*been defined, but never referenced*/
void entfil(int pred, int suff)
/* int pred;		table index of predecessor*/
/* int suff;		suffix byte represented by this entry*/
	{
	auto int disp;
	register struct entry *ep;
	//unsigned int *hash(int pred,int suff,int *disploc);
	unsigned int *p;
	p=hash(pred,suff,&disp);

	/*search the candidate codes (all those which hash from this new*/
	/*predecessor and suffix) for an unreferenced one*/
	while(*p!=(int)EMPTY){

		/*candidate code*/
		ep = &lzw_table[*p];
		if(((ep->predecessor)&REFERENCED)==0){
			/*entry reassignable, so do it!*/
			ep->predecessor=pred;
			ep->suffix=suff;
			/*discontinue search*/
			break;
		}

		/*candidate unsuitable - follow secondary hash chain*/
		/*and keep searching*/
		p+=disp;
		if(p<xlatbl || p > xlatbl+XLATBL_SIZE)
			p+=XLATBL_SIZE;
	}
}




/*uncrunch a single file*/
#ifdef Z80
void uncrunch(char *filename) __z88dk_fastcall
#else
void uncrunch(char *filename)
#endif
	{
	//int c;			
	unsigned char *p;
	//char *cisubstr();
	int pred;			/*current predecessor (in main loop)*/
	unsigned char reflevel;		/*ref rev level from input file*/
	unsigned char siglevel;		/*sig rev level from input file*/
	unsigned char errdetect;	/*error detection flag from input file*/
	int file_cksum;		/*checksum read from input file*/

	/*initialize variables for uncrunching a file*/
	intram();
	
#ifdef	HAVE_CALLOC
	lzw_table=calloc((size_t) TABLE_SIZE, sizeof(lzw_table));
	if (!lzw_table) {
		printf(" -->  out of memory");
		return;
		}
	xlatbl=calloc((size_t)XLATBL_SIZE, (size_t)sizeof(int));
	if (!xlatbl) {
		printf(" -->  out of memory");
#ifdef HAVE_CALLOC
		free(lzw_table);
#endif
		return;
		}
	stack = calloc((size_t)TABLE_SIZE, (size_t)sizeof(char));
	if (!stack) {
		printf(" -->  out of memory");
#ifdef HAVE_CALLOC
		free(lzw_table);
		free(xlatbl);
#endif
		return;
		}
#endif

	/*open input file*/
	if ( 0 == (infd = fopen(filename,"rb")) )
		{
		printf("***** can't open %s\n", filename);
#ifdef HAVE_CALLOC
		free(lzw_table);
		free(xlatbl);
		free(stack);
#endif
		return;
		}

	/*verify this is a crunched file*/
	
	if(getx16() != MAGIC_UNCR) {
		//printf(" not a compressed file\n");
		fclose(infd);
#ifdef HAVE_CALLOC
		free(lzw_table);
		free(xlatbl);
		free(stack);
#endif
		return;
	}


	/*extract, build and print output file name*/
	getfilename();
	printf(": lzw,");

	/*open output file*/
	if ( 0 == (outfd =fopen( outfile,"wb")) )
		{
		printf("***** can't create %s\n",outfile);
#ifdef HAVE_CALLOC
		free(lzw_table);
		free(xlatbl);
		free(stack);
#endif
		return;
		}

	/*read the four info bytes*/
	reflevel=getc(infd);
	siglevel=getc(infd);
	errdetect=getc(infd);
	getc(infd); /*skip spare*/

	/*make sure we can uncrunch this format file*/
	/*note: this program does not support CRUNCH 1.x format*/
	if(siglevel < 0x20 || siglevel > 0x2f)
		{
		printf("***** this version of UNCR cannot process %s!\n",
			filename);
#ifdef HAVE_CALLOC
		free(lzw_table);
		free(xlatbl);
		free(stack);
#endif
		return;
		}

	/*set up atomic code definitions*/
	initb2();

	/*main decoding loop*/
	pred=NOPRED;
	for(;;)
		{
		/*remember last predecessor*/
		lastpr=pred;

		/*read and process one code*/
		if((pred=getcode())==EOFCOD) /*end-of-file code*/
			{
			break; /*all lzw codes read*/
			}

		else if(pred==RSTCOD) /*reset code*/
			{
			entry=0;
			fulflg=0;
			codlen=9;
			trgmsk=0x1ff;
			pred=NOPRED;
			entflg=1;
			initb2();
			}

		else /*a normal code (nulls already deleted)*/
			{
			/*check for table full*/
			if(fulflg!=2)
				{
				/*strategy if table not full*/
				if(decode(pred)==0)enterx(lastpr,finchar);
				else entflg=0;
				}
			else
				{
				/*strategy if table is full*/
				decode(pred);
				entfil(lastpr,finchar); /*attempt to reassign*/
				}
			}
		}

	/*verify checksum if required*/
	if(errdetect==0)
		{
		file_cksum=getc(infd);
		file_cksum|=getc(infd)<<8;
#ifdef Z80
		if(file_cksum!=cksum)
#else
		if(file_cksum!=(cksum & (0xffff)))
#endif
			{
			printf("***** checksum error\n");
			} else {
				fclose(infd);
				remove(filename);
				printf(" done.");
			}
		}

	/*close files*/
	fclose(infd);
	fclose(outfd);

	/*all done this file*/
#ifdef HAVE_CALLOC
	free(lzw_table);
	free(xlatbl);
	free(stack);
#endif
	return;
	}
	
#endif  // UNCRUNCH




// ------------------------------------------------------------------------------------------


#define ACTIVE	00
#define UNUSED	0xff
#define DELETED 0xfe
#define CTRLZ	0x1a

#define MAXFILES 64
#define SECTOR	 128
#define DSIZE	( sizeof(struct ludir) )

#define SLOTS_SEC (SECTOR/DSIZE)
#define equal(s1, s2) ( strcmp(s1,s2) == 0 )
/* if you don't have void type just define as blank */
#define VOID

/* if no enum's then define false as 0 and true as 1 and bool as int */
typedef enum {false=0, true=1} bool;

/* Globals */
char   fname[MAXFILES];
bool ftouched[MAXFILES];


#ifdef Z80
#define word unsigned int
#define wtoi (unsigned int)
#define itow (unsigned int)
#else
typedef struct {
    unsigned char   lobyte;
    unsigned char   hibyte;
} word;
#endif

#ifndef Z80
/* convert word to int */
#ifdef OPT_SIZE
int wtoi(word w) {
	return ((w.hibyte<<8) + w.lobyte);
};
#else
	#define wtoi(w) ((w.hibyte<<8) + w.lobyte)
#endif

#ifndef NOEDIT
#ifdef OPT_SIZE
void itow(word dst,int src) {
	dst.hibyte = (src & 0xff00) >> 8;
	dst.lobyte = src & 0xff;
};
#else
	#define itow(dst,src)	dst.hibyte = (src & 0xff00) >> 8; dst.lobyte = src & 0xff;
#endif
#endif
#endif



struct ludir {			/* Internal library ldir structure */
    unsigned char   l_stat;	/*  status of file */
    char    l_name[8];		/*  name */
    char    l_ext[3];		/*  extension */
    word    l_off;		/*  offset in library */
    word    l_len;		/*  lengty of file */
    char    l_fill[16];		/*  pad to 32 bytes */
} ldir[MAXFILES];


int     errcnt, nfiles, nslots;
bool	verbose = false;
char	*cmdname;


//char   *getfname(), *sprintf();
//int	table(), extract(), print();

/* print error message and exit */
void help () {
    fprintf (stderr, "Usage: %s {utepdr}[v] library [files] ...\n", cmdname);
    fprintf (stderr, "Functions are:\n");
#ifndef NOEDIT
	fprintf (stderr, "\tu - Update, add files to library\n");
#endif
    fprintf (stderr, "\tt - Table of contents\n");
    fprintf (stderr, "\te - Extract files from library\n");
    fprintf (stderr, "\tp - Print files in library\n");
#ifndef NOEDIT
    fprintf (stderr, "\td - Delete files in library\n");
    fprintf (stderr, "\tr - Reorginize library\n");
#endif
    fprintf (stderr, "Flags are:\n\tv - Verbose\n");
    exit (1);
}


void conflict() {
   fprintf(stderr,"Conficting keys\n");
   help();
}


#ifdef Z80
void error (char *str) __z88dk_fastcall
#else
void error (char *str)
#endif
{
    fprintf (stderr, "%s: %s\n", cmdname, str);
    exit(1);
}


#ifdef Z80
void cant (char *name) __z88dk_fastcall
#else
void cant (char *name)
#endif
{
    //extern int  errno;
    //extern char *sys_errlist[];

    fprintf (stderr, "Cannot open file :%s\n", name);
    exit (1);
}


/* Get file names, check for dups, and initialize */
void filenames (int ac, char **av)
{
    register int    i, j;

    errcnt = 0;
    for (i = 0; i < ac - 3; i++) {
	fname[i] = av[i + 3];
	ftouched[i] = false;
	if (i == MAXFILES)
	    error ("Too many file names.");
    }
    fname[i] = NULL;
    nfiles = i;
    for (i = 0; i < nfiles; i++)
	for (j = i + 1; j < nfiles; j++)
	    if (equal (fname[i], fname[j])) {
		fprintf (stderr, "%s", fname[i]);
		error (": duplicate file name");
	    }
}


#ifdef Z80
void getdir (FILE *f) __z88dk_fastcall
#else
void getdir (FILE *f)
#endif
{
int entry;

    rewind(f);
	
    if (fread ((char *) & ldir[0], DSIZE, 1, f) != 1)
	error ("No directory\n");

    nslots = wtoi (ldir[0].l_len) * SLOTS_SEC;
	nslots--;

    if (fread ((char *) & ldir[1], DSIZE, nslots-1, f) != nslots-1)
	error ("Can't read directory - is it a library?");
}


/* filarg - check if name matches argument list */
#ifdef Z80
int filarg (char *name) __z88dk_fastcall
#else
int filarg (char *name)
#endif
{
    register int    i;

    if (nfiles <= 0)
	return 1;

    for (i = 0; i < nfiles; i++)
	if (equal (name, fname[i])) {
	    ftouched[i] = true;
	    return 1;
	}

    return 0;
}


void not_found () {
    register int    i;

    for (i = 0; i < nfiles; i++)
	if (!ftouched[i]) {
	    fprintf (stderr, "%s: not in library.\n", fname[i]);
	    errcnt++;
	}
}


/* convert nm.ex to a Unix style string */
char *getfname (char *nm, char *ex)
{
    static char namebuf[14];
    register char  *cp, *dp;

    for (cp = namebuf, dp = nm; *dp != ' ' && dp != &nm[8];) {
#ifdef TOUPPER
	*cp++ = islower (*dp) ? toupper (*dp) : *dp;
#else
	*cp++ = isupper (*dp) ? tolower (*dp) : *dp;
#endif
	++dp;
    }
    *cp++ = '.';

    for (dp = ex; *dp != ' ' && dp != &ex[3];) {
#ifdef TOUPPER
	*cp++ = islower (*dp) ? toupper (*dp) : *dp;
#else
	*cp++ = isupper (*dp) ? tolower (*dp) : *dp;
#endif
	++dp;
    }

    *cp = '\0';
    return namebuf;
}


// used by setfunc(
int table(char *lib)
{
    FILE   *lfd;
    register int    i, total;
    int active = 0, unused = 0, deleted = 0;
    char *uname;

    if ((lfd = fopen (lib, "rb")) == NULL)
	cant (lib);

    getdir (lfd);
    total = wtoi(ldir[0].l_len);
    if(verbose) {
 	printf("Name          Index Length\n");
	printf("Directory           %4d\n", total);
    }

    for (i = 1; i < nslots; i++)
	switch(ldir[i].l_stat) {
	case ACTIVE:
		active++;
		uname = getfname(ldir[i].l_name, ldir[i].l_ext);
		if (filarg (uname))
		    if(verbose)
			printf ("%-12s   %4d %4d\n", uname,
			    wtoi (ldir[i].l_off), wtoi (ldir[i].l_len));
		    else
			printf ("%s\n", uname);
		total += wtoi(ldir[i].l_len);
		break;
	case UNUSED:
		unused++;
		break;
	default:
		deleted++;
	}
    if(verbose) {
	printf("--------------------------\n");
	printf("Total sectors       %4d\n", total);
	printf("\nLibrary %s has %d slots, %d deleted %d active, %d unused\n",
		lib, nslots, deleted, active, unused);
    }

    VOID fclose (lfd);
    not_found ();
}


#ifndef NOEDIT
void putdir (FILE *f)
{

#ifdef __GNUC__
	if (fseek(f, 0L, SEEK_SET) == -1)   //<< workaround for gcc
#else
    if (rewind(f) == -1)
#endif
        error("Can't rewind the library file\n");

    if (fwrite ((char *) ldir, DSIZE, nslots, f) != nslots)
	error ("Can't write directory - library may be botched");
}


void initdir (FILE *f)
{
    register int    i;
    int     numsecs;
    char    line[80];
	
    static struct ludir blankentry;

/*
    static struct ludir blankentry = {
	UNUSED,
	{ ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' },
	{ ' ', ' ', ' ' },
    };
*/
	blankentry.l_stat = UNUSED;
    strcpy (blankentry.l_name, "        ");
	strcpy (blankentry.l_ext, "   ");

    for (;;) {
	printf ("Number of slots to allocate: ");
	if (fgets (line, 80, stdin) == NULL)
	    error ("Eof when reading input");
	nslots = atoi (line);
	if (nslots < 1)
	    printf ("Must have at least one!\n");
	else if (nslots > MAXFILES)
	    printf ("Too many slots\n");
	else
	    break;
    }

    numsecs = nslots / SLOTS_SEC;
    nslots = numsecs * SLOTS_SEC;

    for (i = 0; i < nslots; i++)
	ldir[i] = blankentry;
    ldir[0].l_stat = ACTIVE;
    itow (ldir[0].l_len, numsecs);

    putdir (f);
}
#endif


void putname (char *cpmname, char *unixname)
{
    register char  *p1, *p2;

    for (p1 = unixname, p2 = cpmname; *p1; p1++, p2++) {
	while (*p1 == '.') {
	    p2 = cpmname + 8;
	    p1++;
	}
	if (p2 - cpmname < 11)
	    *p2 = islower(*p1) ? toupper(*p1) : *p1;
	else {
	    fprintf (stderr, "%s: name truncated\n", unixname);
	    break;
	}
    }
    while (p2 - cpmname < 11)
	*p2++ = ' ';
}


void acopy (FILE *fdi, FILE *fdo, unsigned int nsecs)
{
    register int    i, c;
    int	    textfile = 1;

    while( nsecs-- != 0) 
	for(i=0; i<SECTOR; i++) {
		c = getc(fdi);
		if( feof(fdi) ) 
			error("Premature EOF\n");
		if( ferror(fdi) )
		    error ("Can't read");
		if( !isascii(c) )
		    textfile = 0;
		if( nsecs != 0 || !textfile || c != CTRLZ) {
			putc(c, fdo);
			if ( ferror(fdo) )
			    error ("write error");
		}
	 }
}


void getfiles (char *name, bool pflag)
{
    FILE *lfd, *ofd;
    register int    i;
    char   *unixname;

    if ((lfd = fopen (name, "rb"))  == NULL)
	cant (name);

    ofd = pflag ? stdout : NULL;
    getdir (lfd);

    for (i = 1; i < nslots; i++) {
	if(ldir[i].l_stat != ACTIVE)
		continue;
	unixname = getfname (ldir[i].l_name, ldir[i].l_ext);
	if (!filarg (unixname))
	    continue;
	fprintf(stderr,"%s", unixname);
	if (ofd != stdout)
	    ofd = fopen (unixname, "wb");
	if (ofd == NULL) {
	    fprintf (stderr, "  - can't create");
	    errcnt++;
	} else {
	    VOID fseek (lfd, (long) wtoi (ldir[i].l_off) * SECTOR, SEEK_SET);
	    acopy (lfd, ofd, wtoi (ldir[i].l_len));
	    if (ofd != stdout) {
			VOID fclose (ofd);
#ifdef UNCRUNCH
			uncrunch(unixname);
#endif
#ifdef USQ
			unsqueeze(unixname);
#endif
#ifdef UNLZH
			unlzh(unixname);
#endif

		}
		
	}
	// extra close() call, to fix z88dk compiled programs behavior
	if (ofd != stdout)
		VOID fclose (ofd);
	putc('\n', stderr);
    }
    VOID fclose (lfd);
    not_found ();
}


// used by setfunc(
int extract(char *name)
{
	getfiles(name, false);
}


/// used by setfunc(
int print(char *name)
{
	getfiles(name, true);
}


#ifndef NOEDIT
int fcopy(FILE *ifd, FILE *ofd)
{
    int total = 0;
    int i, n;
    char sectorbuf[SECTOR];


    while ( (n = fread( sectorbuf, 1, SECTOR, ifd)) != 0) {
	if (n != SECTOR)
	    for (i = n; i < SECTOR; i++)
		sectorbuf[i] = CTRLZ;
	if (fwrite( sectorbuf, 1, SECTOR, ofd ) != SECTOR)
		error("write error");
	++total;
    }
    return total;
}


void addfil (char *name, FILE *lfd)
{
    FILE	*ifd;
    register int secoffs, numsecs;
    register int i;
	
    if ((ifd = fopen (name, "rb")) == NULL) {
	fprintf (stderr, "%s: can't find to add\n",name);
	errcnt++;
	return;
    }
    if(verbose)
        fprintf(stderr, "%s\n", name);
    for (i = 0; i < nslots; i++) {
	if (equal( getfname (ldir[i].l_name, ldir[i].l_ext), name) ) /* update */
	    break;
	if (ldir[i].l_stat != ACTIVE)
		break;
    }
    if (i >= nslots) {
	fprintf (stderr, "%s: can't add library is full\n",name);
	errcnt++;
	return;
    }

    ldir[i].l_stat = ACTIVE;
    putname (ldir[i].l_name, name);
    VOID fseek(lfd, 0L, SEEK_END);		/* append to end */
    secoffs = ftell(lfd) / 128L;

    itow (ldir[i].l_off, secoffs);
    numsecs = fcopy (ifd, lfd);
    itow (ldir[i].l_len, numsecs);
    VOID fclose (ifd);
}

// used by setfunc(
int update(char *name)
{
    FILE *lfd;
    register int    i;

    if ((lfd = fopen (name, "r+b")) == NULL) {
	    cant (name);
	initdir (lfd);
    }
    else
	getdir (lfd);		/* read old directory */

    if(verbose)
	    fprintf (stderr,"Updating files:\n");
    for (i = 0; i < nfiles; i++)
	addfil (fname[i], lfd);
    if (errcnt == 0)
	putdir (lfd);
    else
	fprintf (stderr, "fatal errors - library not changed\n");
    VOID fclose (lfd);
}

// used by setfunc(
int del_entry(char *lname)
{
    FILE *f;
    char *unixnm;
    register int    i;

    if ((f = fopen (lname, "r+b")) == NULL)
	cant (lname);

    if (nfiles <= 0)
	error("Filename to delete from Library was not specified");

    getdir (f);
	
    for (i = 0; i < nslots; i++) {
        unixnm = getfname (ldir[i].l_name, ldir[i].l_ext);
	if (!filarg (unixnm))
	    continue;
	ldir[i].l_stat = DELETED;
	if (verbose)
	    printf("Deleted File %s\n",unixnm);
    }

    not_found();
    if (errcnt > 0)
	fprintf (stderr, "errors - library not updated\n");
    else
	putdir (f);
    if (fclose(f) == EOF)
	printf("Updated library file not closed--Is it a Stream File?\n");
}


#define copymem(dst,src,n) memcpy(dst,src,n)
/*
copymem(dst, src, n)
register char *dst, *src;
register unsigned int n;
{
	while(n-- != 0)
		*dst++ = *src++;
}
*/


void copyentry(struct ludir *old, FILE *of, struct ludir *new, FILE *nf )
{
    register int secoffs, numsecs;
    char buf[SECTOR];

    new->l_stat = ACTIVE;
    copymem(new->l_name, old->l_name, 8);
    copymem(new->l_ext, old->l_ext, 3);
    VOID fseek(of, (long) wtoi(old->l_off)*SECTOR, SEEK_SET);
    VOID fseek(nf, 0L, SEEK_END);
    secoffs = ftell(nf) / SECTOR;

    itow (new->l_off, secoffs);
    numsecs = wtoi(old->l_len);
    itow (new->l_len, numsecs);

    while(numsecs-- != 0) {
	if( fread( buf, 1, SECTOR, of) != SECTOR)
	    error("read error");
	if( fwrite( buf, 1, SECTOR, nf) != SECTOR)
	    error("write error");
    }
}


// used by setfunc(
int reorg(char *name)
{
    FILE *olib, *nlib;
    int oldsize;
    register int i, j;
    struct ludir odir[MAXFILES];
    //char tmpname[SECTOR];
	char tmpname[]="lutemp.tmp";

    //strcpy(tmpname,name);

    if( (olib = fopen(name,"rb")) == NULL)
	cant(name);

    if( (nlib = fopen(tmpname, "wb")) == NULL)
	cant(tmpname);

    getdir(olib);
    printf("Old library has %d slots\n", oldsize = nslots);
    for(i = 0; i < nslots ; i++)
	    copymem( (char *) &odir[i], (char *) &ldir[i],
			sizeof(struct ludir));
    initdir(nlib);
    errcnt = 0;

    for (i = j = 1; i < oldsize; i++)
	if( odir[i].l_stat == ACTIVE ) {
	    if(verbose)
		fprintf(stderr, "Copying: %-8.8s.%3.3s\n",
			odir[i].l_name, odir[i].l_ext);
	    copyentry( &odir[i], olib,  &ldir[j], nlib);
	    if (++j >= nslots) {
		errcnt++;
		fprintf(stderr, "Not enough room in new library\n");
		break;
	    }
	}

    VOID fclose(olib);
    putdir(nlib);
    VOID fclose (nlib);

    if(errcnt == 0) {
/*
**	if ( unlink(name) < 0 || link(tmpname, name) < 0) {
**	    VOID unlink(tmpname);
**	    cant(name);
**       }
*/
		VOID remove(name);
		VOID rename(tmpname, name);
    } else {
	fprintf(stderr,"Errors, library not updated\n");
    //VOID delete(tmpname);
	VOID remove(tmpname);
	}

}
#endif


int main (int argc, char **argv)
{
    register char *flagp;
    char   *aname;			/* name of library file */
    int	   (*function)() = NULL;	/* function to do on library */
/* set the function to be performed, but detect conflicts */
#define setfunc(val)	if(function != NULL) conflict(); else function = val

    //cmdname = argv[0];
	cmdname = "LAR";
    if (argc < 3)
	help ();

    aname = argv[2];
    filenames (argc, argv);

    for(flagp = argv[1]; *flagp; flagp++)
	switch (*flagp) {
	case '-':
		break;
	case 't': 
	case 'T': 
	    setfunc(table);
	    break;
	case 'e': 
	case 'E': 
	    setfunc(extract);
	    break;
	case 'p': 
	case 'P': 
	    setfunc(print);
	    break;
#ifndef NOEDIT
	case 'u': 
	case 'U': 
	    setfunc(update);
	    break;
	case 'd': 
	case 'D': 
	    setfunc(del_entry);
	    break;
	case 'r': 
	case 'R': 
	    setfunc(reorg);
	    break;
#endif
	case 'v':
	case 'V':
	    verbose = true;
	    break;
	default: 
	    help ();
    }
	
    if(function == NULL) {
	fprintf(stderr,"No function key letter specified\n");
	help();
    }

    (*function)(aname);
}
