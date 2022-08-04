/* Z88DK conversion, somewhat good also for building with gcc
   Adapted by Stefano Bodrato Jul 2022
   
   zcc +cpm -create-app crypt.c
   zcc +cpm -create-app -compiler=sdcc -SO3 --max-allocs-per-node400000 crypt.c
   zcc +cpm -create-app -DBDSC_COMPAT crypt.c
   
   gcc crypt.c

*/

//#pragma scanf "%s"

/* last update 7 May 81

CRYPT program by kathy bacon and neal somos

	CRYPT can be used to "encrypt" a file for secrecy, using a 
special keyword which the user specifies; later, the original file
can be retrieved by re-encrypting the file with the same keyword.
If you forget your keyword...well, sorry.


CRYPT accepts command line arguments of three forms:

	 INFILE
	 INFILE,OUTFILE
	 .KEYWORD

If no output file is specified, the encrypted form of INFILE will be
written out as INFILE.CRP ( if the input file has an extension of 
".CRP", however, it will be written out as TEMP.$$$ ).
	In the second case, the "," means that the output file name
will follow.
	The key word itself can also be specified on the command line,
which is useful if you want to use a submit file. Only one keyword per
command line, however.
	CRYPT will ask for the keyword if none is given; it will not
be echoed to the console.x

	:::::::::::::::::::::::::::::::::::::::::::

NOTE!!!!!  There should be NO SPACES between a "-" and the filenames;
	   nor between a "!" and its keyword.

	:::::::::::::::::::::::::::::::::::::::::::

a sample call might therefore be:

A>crypt  tomb.c  help.crp,help.c   .yippeezoop


************************************************************/

//#include "bdscio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int debug;

FILE *fd_in;
FILE *fd_crypt;

#define	KEYCHAR	'.'
#define OUTCHAR ','
#define READ	0
//#define	ERROR_EXIT	{ fclose(fd_in);	exit(0);	}
#define NEWLINE	'\n'
#define DEBUG 	(debug != 0)
#define	BLANK	' '
//#define	BACKSPACE	'\b'
#define	BACKSPACE	8
#define CR	'\r'
#define NUL	'\0'

#define SECSIZ 128
#define FALSE 0
#define TRUE 1
#define	ERROR -1

#define setmem(a,b,c) memchr(a,c,b)

#define OK 0		/* General purpose "no error" return value */



#ifdef BDSC_COMPAT

int nrand (int n, int a, int b, int c)
{

#asm


	ld hl,9       ;get n (1st arg)
	add hl,sp
	ld a,(hl)
	cp 255        ;was it -1 (set seed) ?
	JP	NZ,nrand1


	pop de        ; ret addr
	pop hl
	LD	(bdsc_seed+4),HL
	pop hl
	LD	(bdsc_seed+2),HL
	pop hl
	LD	(bdsc_seed),HL
	push hl
	push hl
	push hl
	push de        ; ret addr
	ret            ;all done


nrand1:
	; (we avoid the interactive "randomize" option, not necessary)
	; and a                ;is it 0 (randomize)?
	; jr nz,nrand3
	;pop de        ; ret addr
	;pop bc
	;pop hl
	;push hl
	;push bc
	;push de
	;push hl
	;call puts_cons
    ;pop hl
	;	
;	LD	HL,5a97h	;start w/something odd
;nrand2:
;	PUSH	HL
;	LD 	c,cstat	;interrogate console status
;	call	bdos
;	POP	HL
;	INC	HL	;and keep it odd
;	INC	HL	;and growing
;	OR	a
;	JP	Z,nrand2	;until user types something.
;	LD	(seed),HL	;then plaster the value all over the
;	LD	(seed+2),HL	;seed.
;	LD	(seed+4),HL
;	ret
	

nrand3:
	LD	A,(bdsc_seed)    ;now compute next random number. from this
	OR	1                ; point on, the code is that of Prof. Paul Gans
	LD	(bdsc_seed),A    ;lsb of SEED must be 1
	
	LD 	B,6              ;clear 6 PROD bytes to 0
	LD	HL,bdsc_prod
randm1:
	LD 	(HL),0
	INC	HL
	DEC	B
	JP	NZ,randm1

	LD	BC,6             ;set byte counter
randm2:
	LD	HL,plier-1
	ADD	HL,BC            ;make addr of lsb of PLIER
	LD 	A,(HL)           ;PLIER byte
	PUSH	BC           ;save byte counter
	LD 	B,8              ;set bit counter

randm3:
	LD 	D,A              ;save PLIER byte
	LD	HL,bdsc_prod     ;shift whole PROD left one bit
	LD 	C,6
	XOR	A
randm4:
	LD 	A,(HL)           ;get byte	
	RLA                  ;shift left
	LD 	(HL),A           ;put byte
	INC	HL
	DEC	C
	JP	NZ,randm4

	LD 	A,D              ;recover PLIER byte
	RLA                  ;look at current high bit
	JP	NC,randm6        ;0 means no add cycle

	PUSH	AF           ;add SEED to PROD
	XOR	A
	LD 	C,6
	LD	HL,bdsc_prod
	LD	DE,bdsc_seed
randm5:
	LD	A,(DE)
	ADC	A,(HL)
	LD 	(HL),A
	INC	HL
	INC	DE
	DEC	C
	JP	NZ,randm5
	POP	AF

randm6:	DEC	B            ;test bit counter
	JP	NZ,randm3        ;go cycle more bits
	POP	BC               ;recover byte counter
	DEC	C                ;test it
	JP	NZ,randm2        ;go process more bytes

	LD 	B,6              ;complement PROD, add 1 to it,
	LD	HL,bdsc_seed     ;and transfer it to SEED.
	LD	DE,bdsc_prod
	XOR	A
	CCF
randm7:
	LD	A,(DE)
	CPL
	ADC	A,0
	LD 	(HL),A
	INC	HL
	INC	DE
	DEC	B
	JP	NZ,randm7

	DEC	HL               ;put the two high order bytes
	LD 	A,(HL)           ;into HL for return to C, not
	AND	7Fh              ;neglecting to zero the high
	LD 	H,A              ;order bit so a positive int
	LD	A,(bdsc_seed+4)  ;is returned
	LD 	L,A
	RET

plier:
	DEFM	0c5h,87h,1
	DEFM	0eh,9ah,0e0h	

bdsc_seed:	DEFM	1,0,0,0,0,0

bdsc_prod:	DEFM	0,0,0,0,0,0

#endasm
}

#endif


// Customized puts, to avoid the final NEWLINE
int my_puts(char *s) {
	int i;
    for (i = 0; s[i]; ++i)
        putchar (s[i]);
}

void ERROR_EXIT()
{ fclose(fd_in);	exit(0);	}

/*************************************************************
	gets input from keyboard char by char, blanking out
as received. Assumes that "input" is big enough to handle input.
Returns length of input string.
******************************************************************/
int no_echo (char *input)
{
char *cp;

cp = input;

//while (!getk()) {}

while (1)
      {
		if (NEWLINE == (*cp = getchar()))
		  { *cp = NUL;
			break;
		  }
		cp++;
		putchar (BACKSPACE);
		putchar (BLANK);
      }
return (strlen(input));
}

#ifdef BDSC_COMPAT

/* This version reproduces the buggy results of the original function when build with BDS C.
   This permits to decode old files with a z88dk build. */

void get_seed (char *key, int *seed)
{
char *ptr;
int a, i, length;

length = strlen (key);
ptr = key;
for (i=0; i< length; i++) {
	if (i&1)
		seed [(i/2)] += ptr[i]*256;
	else
		seed [(i/2)] += ptr[i];
	}

if (i&1) 
{ i--; seed [(i/2)] -= ptr[i];}

}

#else
/*************************************************************
	get a seed for the random number generator
"seed" should be a 3-integer array

NOTE: this function is buggy, a single character key won't change the seed
      moreover it has a different behavior when compiled with BDS C
*************************************************************/
void get_seed (char *key, int *seed)
{
char *ptr;
int i, length;

length = strlen (key);
ptr = key;
for (i=0; i< length/2; i++)
	seed [i%3] +=	*ptr++;
}
#endif


/*********************************************************************
 returns index of t in s, ERROR if not found.  Thanx to Kernighan
 and Ritchie, p. 67
*********************************************************************/
int my_index (char *s, char *t)
{
int i, j, k;

for (i=0; s[i] != NUL; i++)
      { for (j=i, k=0; t[k] != NUL && s[j]==t[k]; j++, k++)
		;
	if (t[k] == NUL)
		return (i);
      }
return (ERROR);
}


int keylen, fine, nread;
int seed[3];
int ssplit;

void main(int argc, char **argv)
{
char filename[30], filecrypt[30], after[10];
char key[100], t[100], argument[100];
char *comarg;
char cryptbuf [8*SECSIZ];
int i, l;

debug = FALSE;
if (argc == 1)	/* we'll change this later, so no snide remarks, neal */
      { printf("\nthis is a cryptic program!! heeheeheehee");
	exit(0);
      }
setmem (key, 100, NUL);
for (i=1; i<argc; i++)		/* check for key */
      { comarg = argv[i];
	if (*comarg == KEYCHAR)	/* key found */
	      { strcpy (key, &comarg[1]);
		break;
	      }
	if (OK == strcmp (comarg, "-D"))	/* debug */
	      debug = TRUE;
      }

if (i == argc)			/* no key found on command line */
      {
	while (1)	/* get 'key' for encrypting file */
	      { my_puts ("\nkey ?     (no echo)\n");
		if (0 != (keylen =no_echo (key)))
		      { if DEBUG printf("\nthanks that was <%s>",key);
			my_puts("\nplease retype for verification:\n");
			if (0 != (l = no_echo (t)))
			      { if (OK != strcmp (key,t))
				      { my_puts("\nthey aren't the same,");
					my_puts(" so we'd better start over.\n");
					continue;	/* while (1) */
				      }
				else my_puts("\nkey verified.");
				break;	/* from while (1) */
			      }
		      }
	
		my_puts ("\nOK, be that way!");
		
		exit(0);
	
	      }	/* end while (1) */
      }
argv++;
while (--argc)
      { strcpy (argument,*argv++);

	if DEBUG printf("\nnext argument is    |%s|", argument);
	if (OK == strcmp (argument, "-D"))	/* debug */
		continue;		/* while (--argc) */
	if (argument[0] == KEYCHAR)		/* key */
		continue;

	//len = strlen (argument);
	if (argument[0]==OUTCHAR || argument[strlen (argument)-1]== OUTCHAR)	/* ERROR!!*/
	      { my_puts("\n\nPROFOUND error:");
	        my_puts ("\nNO spaces allowed around output specifier ',' ");
		printf("\n%s is illegal.", argument);
		continue;
	      }


	setmem (filename, 30, NUL); setmem(filecrypt, 30, NUL);
	
	//if (ERROR != my_index (argument, ","))
	if (ERROR != (ssplit = my_index (argument, ","))) {
	      //sscanf (argument, "%s,%s", filename, filecrypt);
		*(argument+ssplit)='\0';
		strcpy(filename,argument);
		strcpy(filecrypt,argument+ssplit+1);
		//*(argument+ssplit)=',';	 // re-join strings
	}

	else strcpy (filename, argument);

	printf("\nNow processing file <%s>", filename);
	//if (ERROR == (fd_in = open(filename, READ)))
	if((fd_in = fopen(filename,"rb"))==0)
	      { printf("\ncan not open <%s>", filename);
		continue;		/* while (--argc) */
	      }

	if (filecrypt[0] == NUL)	/* outfile not specified */
	      {	//sscanf (filename, "%s.%s", filecrypt, after);
		  	if (ERROR != (ssplit = my_index (filename, "."))) {
				*(filename+ssplit)='\0';
				strcpy(filecrypt,filename);
				strcpy(after,filename+ssplit+1);
			} else strcpy(filecrypt,filename);
		if (OK == strcmp (after, "CRP"))
			strcpy (filecrypt, "TEMP.$$$");
		else strcat (filecrypt, ".CRP");
	      }
	printf("\nencrypted file will be <%s>", filecrypt);
	//if (ERROR == (fd_crypt = creat ("TEMP.$$$")))
	if((fd_crypt = fopen("TEMP.$$$","wb"))==0)
	      { printf("\ncan not create temporary file");
		continue;	/* while (--argc) */
	      }


/* set up seed for the number generator */
	seed[0] = seed[1] = seed[2] = 0;
	get_seed (key, seed);

#ifdef BDSC_COMPAT
	nrand (-1, seed[0], seed[1], seed[2] );
#else
	srand(seed[0]+256*seed[1]+512*seed[2]);
#endif
	
	if DEBUG printf("\nfd_in=%d, fd_crypt=%d",fd_in,fd_crypt);
	fine = FALSE;	/* musical end */
	while (!fine)
	      { 
			nread=fread(cryptbuf, 1, 8*SECSIZ, fd_in);
		  /*
			if (OK > (nread = read (fd_in, cryptbuf, 8)))
		      { printf("\nread returned %d for <%s>",
					      nread,  filename );
			if DEBUG printf("\nfd_in=%d",fd_in);
			ERROR_EXIT();
		      }
			  */
		for (i=0; i<nread; i++)
#ifdef BDSC_COMPAT
			cryptbuf[i] ^= nrand(1,0,0,0);
#else
			cryptbuf[i] ^= rand()%0xff;
#endif
		//if (nread != write (fd_crypt, cryptbuf, nread))
		if (fwrite(cryptbuf, 1, nread, fd_crypt) < nread)
		      { printf("\nerror writing to temporary file");
			if DEBUG printf("\nfd_crypt=%d",fd_crypt);
			ERROR_EXIT();
		      }
		if (nread == 0 || nread != (8*SECSIZ))	fine = TRUE;
	      }
	if (OK != fclose (fd_in))
		printf("\nerror closing file <%s>", filename);
	if (OK != fclose (fd_crypt))
	      { my_puts ("\nerror closing temporary file");
		exit(0);
	      }
	remove (filecrypt);
	rename ("TEMP.$$$", filecrypt);
	printf("\n\nEncrypted form of <%s> is in <%s>",
                                 filename,   filecrypt);

      }	/* end while (--argc) */

}	/* end main() */

