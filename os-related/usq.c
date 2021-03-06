/* Program to unsqueeze files formed by sq.com
 *
 * Build hints (z88dk - OSCA):
 * zcc +osca -ousq -O3 -create-app --opt-code-size -pragma-define:CRT_INITIALIZE_BSS=0 -lflosxdos -DWILDCARD usq.c 
 * --or--
 * zcc +osca -ousq -create-app -SO3 --max-allocs-per-node400000 -pragma-define:CRT_INITIALIZE_BSS=0 -lflosxdos -DWILDCARD -compiler=sdcc usq.c
 *
 * Build hints (z88dk - CP/M):
 * zcc +cpm -ousq -create-app -O3 --opt-code-size -pragma-define:CRT_INITIALIZE_BSS=0 -DWILDCARD usq.c
 *   -- or --)
 * zcc +cpm -ousq -create-app -SO3 --max-allocs-per-node400000 -pragma-define:CRT_INITIALIZE_BSS=0 -DWILDCARD -compiler=sdcc usq.c
 * 
 * Build (gcc):
 * gcc -ousq usq.c
 *
 *
 * The original compiled program size was 12288, 
 * z88dk, with a bit of tweaking, can produce smaller programs (9032 during the tests)
 * or with comparable sizes when the support for wildcards is enabled (9963 during the tests).
 *
 * Output redirection of the program messages is available.
 * For printer spooling (>PRN:), add "-lcpmdevice" at compile time, it will increase the program size of about 1K.
 *
 * 
 * 
 * Useage:
 *
 *	usq [-count] [-fcount] [file1] [file2] ... [filen]
 *
 * where file1 through filen represent one or more files to be compressed,
 * and the following options may be specified:
 *
 *	-count		Previewing feature: redirects output
 * 			files to standard output with parity stripped
 *			and unprintables except CR, LF, TAB and  FF
 *			converted to periods. Limits each file
 *			to first count lines.
 *			Defaults to console, but see below how
 *			to capture all in one file for further
 *			processing, such as by PIP.
 *			Count defaults to a very high value.
 *			No CRC check is performed when previewing.
 *			Use drive: to cancel this.
 *
 *	-fcount		Same as -count except formfeed
 *			appended to preview of each file.
 *			Example: -f10.
 *
 * If no such items are given on the command line you will be
 * prompted for commands (one at a time). An empty command
 * terminates the program.
 *
 * The unsqueezed file name is recorded in the squeezed file.
 * 
 */
/* CHANGE HISTORY:
 * 1.3	Close inbuff to avoid exceeding maximum number of
 *	open files. Includes rearranging error exits.
 * 1.4	Add -count option to allow quick inspection of files.
 * 1.5  Break up long lines of introductory text
 * 1.5  -count no longer appends formfeed to preview of each file.
 *	-fcount (-f10, -F10) does append formfeed.
 * 1.6  Modified to work correctly under MP/M II (DIO.C change) and
 *      signon message shortened.
 * 2.0	Modified to work with CI-C86 compiler (CP/M-86 and MS-DOS)
 * 2.1  Modified for use in MLINK
 * 2.2  Modified for use with optimizing CI-C86 compiler (MS-DOS)
 * 3.0  Generalized for use under UNIX
 * 3.1  Found release date: 12/19/84
 * 3.2  More generalized for use under modern UNIX and z88dk
 */



#define SQMAIN

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include "sqcom.h"
#define VERSION "3.2   23/03/2012"



#ifdef __OSCA__
#include "flos.h"
#endif

#define ERROR -1
#define TRUE 1
#define FALSE 0

#define LARGE 30000

/* Decoding tree */
struct {
	int children[2];	/* left, right */
} dnode[NUMVALS - 1];

int bpos;	/* last bit position read */
int curin;	/* last byte value read */

/* Variables associated with repetition decoding */
int repct;	/*Number of times to retirn value*/
int value;	/*current byte value or EOF */

/* This must follow all include files */
unsigned int dispcnt;	/* How much of each file to preview */
char	ffflag;		/* should formfeed separate preview from different files */



/* get 16-bit word from file */

#ifdef Z80
int getw16(FILE *iob) __z88dk_fastcall
#else
int getw16(FILE *iob)
#endif
{
int temp;

temp = getc(iob);		/* get low order byte */
temp |= getc(iob) << 8;
if (temp & 0x8000) temp |= (~0) << 15;	/* propogate sign for big ints */
return temp;

}


/* get 16-bit (unsigned) word from file */

#ifdef Z80
int getx16(FILE *iob) __z88dk_fastcall
#else
int getx16(FILE *iob)
#endif
{
int temp;

temp = getc(iob);		/* get low order byte */
return temp | (getc(iob) << 8);

}


/* initialize decoding functions */

void init_cr()
{
	repct = 0;
}

void init_huff()
{
	bpos = 99;	/* force initial read */
}


/* Decode file stream into a byte level code with only
 * repetition encoding remaining.
 */

#ifdef Z80
int getuhuff(FILE *ib) __z88dk_fastcall
#else
int getuhuff(FILE *ib)
#endif
{
	int i;

	/* Follow bit stream in tree to a leaf*/
	i = 0;	/* Start at root of tree */
	do {
		if(++bpos > 7) {
			if((curin = getc(ib)) == ERROR)
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

#ifdef Z80
int getcr(FILE *ib) __z88dk_fastcall
#else
int getcr(FILE *ib)
#endif
{
	int c;

	if(repct > 0) {
		/* Expanding a repeated char */
		--repct;
		return value;
	} else {
		/* Nothing unusual */
		if((c = getuhuff(ib)) != DLE) {
			/* It's not the special delimiter */
			value = c;
			if(value == EOF)
				repct = LARGE;
			return value;
		} else {
			/* Special token */
			if((repct = getuhuff(ib)) == 0)
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
	FILE *inbuff, *outbuff;	/* file buffers */
	int i, c;
	char cc;

	char *p;
	unsigned int filecrc;	/* checksum */
	int numnodes;		/* size of decoding tree */
	char outfile[128];	/* output file name */
	unsigned int linect;	/* count of number of lines previewed */
	char obuf[128];		/* output buffer */
	int oblen;		/* length of output buffer */
	static char errmsg[] = "ERROR - write failure in %s\n";

	if(!(inbuff=fopen(infile, "rb"))) {
		printf("Can't open %s\n", infile);
		return;
	}
	/* Initialization */
	linect = 0;
	crc = 0;
	init_cr();
	init_huff();

	/* Process header */
	if(getx16(inbuff) != RECOGNIZE) {
		printf("%s is not a squeezed file\n", infile);
		goto closein;
	}

	filecrc = getw16(inbuff);

	/* Get original file name */
	p = outfile;			/* send it to array */
	do {
		*p = getc(inbuff);
	} while(*p++ != '\0');

	printf("%s -> %s: ", infile, outfile);


	numnodes = getw16(inbuff);

	if(numnodes < 0 || numnodes >= NUMVALS) {
		printf("%s has invalid decode tree size\n", infile);
		goto closein;
	}

	/* Initialize for possible empty tree (SPEOF only) */
	dnode[0].children[0] = -(SPEOF + 1);
	dnode[0].children[1] = -(SPEOF + 1);

	/* Get decoding tree from file */
	for(i = 0; i < numnodes; ++i) {
		dnode[i].children[0] = getw16(inbuff);
		dnode[i].children[1] = getw16(inbuff);
	}

	if(dispcnt) {
		/* Use standard output for previewing */
		putchar('\n');
		while(((c = getcr(inbuff)) != EOF) && (linect < dispcnt)) {
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
		if(!(outbuff=fopen(outfile, "wb"))) {
			printf("Can't create %s\n", outfile);
			goto closeall;
		}
		printf("unsqueezing,");
		/* Get translated output bytes and write file */
		oblen = 0;
		while((c = getcr(inbuff)) != EOF) {
			crc += c;
			obuf[oblen++] = c;
			if (oblen >= sizeof(obuf)) {
				if(!fwrite(obuf, sizeof(obuf), 1, outbuff)) {
					printf(errmsg, outfile);
					goto closeall;
				}
				oblen = 0;
			}
		}
		if (oblen && !fwrite(obuf, oblen, 1, outbuff)) {
			printf(errmsg, outfile);
			goto closeall;
		}

		if((filecrc && 0xFFFF) != (crc && 0xFFFF))
			printf("ERROR - checksum error in %s\n", outfile);
		else	printf(" done.\n");

	closeall:
		fclose(outbuff);
	}

closein:
	fclose(inbuff);
}


#ifdef WILDCARD
/* 
 * Wildcard comparison tool
 * Found in the BDS C sources, (wildexp..),written by Leor Zolman.
 * contributed by: W. Earnest, Dave Hardy, Gary P. Novosielski, Bob Mathias and others
 * 
*/

int match(char *wildnam, char *filnam)
{
   char c;
   while (c = *wildnam++)
	if (c == '?')
		if ((c = *filnam++) && c != '.')
			continue;
		else
			return FALSE;
	else if (c == '*')
	{
		while (c = *wildnam)
		{ 	wildnam++;
			if (c == '.') break;
		}
		while (c = *filnam)
		{	filnam++;
			if (c == '.') break;
		}
	}
	else if (c == *filnam++)
	 	continue;
	else return FALSE;

   if (!*filnam)
	return TRUE;
   else
	return FALSE;
}
#endif


void obey(char *p)
{
	char *q;
	#ifdef WILDCARD
	int x;
	#endif

	if(*p == '-') {
		if(ffflag = ((*(p+1) == 'F') || (*(p+1) == 'f')))
			++p;
		/* Set number of lines of each file to view */
		dispcnt = 65535;	/* default */
		if(*(p+1))
			if((dispcnt = atoi(p + 1)) == 0)
				printf("\nBAD COUNT %s", p + 1);
		return;
	}	

	/* Check for ambiguous (wild-card) name */
	for(q = p; *q != '\0'; ++q)
		if(*q == '*' || *q == '?') {
		#ifdef WILDCARD
			if ((x=dir_move_first())!=0) return;
			while (x == 0) {
				if (match(p,dir_get_entry_name()))
					obey(dir_get_entry_name());
				x = dir_move_next();
			}
		#else
			printf("\nAmbiguous name %s ignored", p);
		#endif
			return;
		}

	unsqueeze(p);
}

int main(int argc, char *argv[])
{
	int i,c;
	char inparg[16];	/* parameter from input */

	dispcnt = 0;	/* Not in preview mode */

	printf("File unsqueezer version %s (original author: R. Greenlaw)\n\n", VERSION);

	/* Process the parameters in order */
	for(i = 1; i < argc; ++i)
		obey(argv[i]);

	if(argc < 2) {
		printf("Enter file names, one line at a time, or type <RETURN> to quit.");
		do {
			printf("\n*");
			for(i = 0; i < 16; ++i) {
				if((c = getchar()) == EOF)
					c = '\n';	/* force empty (exit) command */
					#ifdef __OSCA__
					fputc_cons(c);
					#endif
				if((inparg[i] = c) == '\n') {
					inparg[i] = '\0';
					break;
				}
			}
			if(inparg[0] != '\0')
				obey(inparg);
		} while(inparg[0] != '\0');
	}
}
