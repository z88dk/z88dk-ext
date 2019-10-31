
/* zcc +cpm -create-app -O3 --opt-code-size -ouncr uncrunch.c */
/* note: this program does not support CRUNCH 1.x format */


/*----------------------------------------------------------------------------*/
/*  UNCRunch/C - LZW uncruncher compatible with Z-80 CP/M program CRUNCH 2.3  */
/*			(C) Copyright 1986 - Frank Prindle		      */
/*		    May be reproduced for non-profit use only		      */
/*----------------------------------------------------------------------------*/
/* This program is largely based on the previous works of Steven Greenberg,   */
/* Joe Orost, James Woods, Ken Turkowski, Steve Davies, Jim McKie, Spencer    */
/* Thomas, Terry Welch, and (of course) Lempel and Zev (Ziv?), whoever they   */
/* are.									      */
/*----------------------------------------------------------------------------*/
/*  Portability related assumptions:					      */
/*    1. fopen called with "rb" or "wb" is sufficient to make getc/putc xfer  */
/*       bytes in binary, without newline or control-z translation (this is   */
/*       default for UNIX anyway, and the "b" has no effect).                 */
/*    2. size of type "long" is at least 4 bytes (for getbuf to work).        */
/*----------------------------------------------------------------------------*/
/* Version 2.32	12/15/86 - Mods for Microsoft 'C' version 3.0 under MS-DOS    */
/*	by Skip Hansen - Modem: (213) 541-2503				      */
/*									      */
/*		1. Changed several "char" declarations to "unsigned char".    */
/*		2. Added both top and bottom range checking on secondary      */
/*		   hash probe to prevent wrap around from fooling comparision.*/
/*		3. Added function cisubstr to replace index function which    */
/*		   is not included in Microsoft's library.		      */
/*----------------------------------------------------------------------------*/		
#define VERSION "2.32"
#define DATE "25 Dec 1986"

/*#define DUMBLINKER*/	/*define this if linker makes a huge executable image*/
			/*containing mostly zeros (big tables will instead be*/
			/*allocated at run time)*/

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

/*Macro definition - ensure letter is lower case*/
#define tolower(c) (((c)>='A' && (c)<='Z')?(c)-('A'-'a'):(c))

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

#ifdef	DUMBLINKER

  /*main lzw table and it's structure*/
  struct entry {
	short predecessor;	/*index to previous entry, if any*/
	unsigned char suffix;		/*character suffixed to previous entries*/
  } *table;

  /*auxilliary physical translation table*/
  /*translates hash to main table index*/
  short *xlatbl;

  /*byte string stack used by decode*/
  unsigned char *stack;

#else

  struct entry {
	short predecessor;	/*index to previous entry, if any*/
	unsigned char suffix;	/*character suffixed to previous entries*/
  } table[TABLE_SIZE];

  /*auxilliary physical translation table*/
  /*translates hash to main table index*/
  short xlatbl[XLATBL_SIZE];

  /*byte string stack used by decode*/
  unsigned char stack[TABLE_SIZE];

#endif

/*other global variables*/
unsigned char	codlen;		/*variable code length in bits (9-12)*/
short	trgmsk;			/*mask for codes of current length*/
unsigned char	fulflg;		/*full flag - set once main table is full*/
short	entry;			/*next available main table entry*/
long	getbuf;			/*buffer used by getcode*/
short	getbit;			/*residual bit counter used by getcode*/
unsigned char	entflg; 	/*inhibit main loop from entering this code*/
unsigned char	repeat_flag;	/*so send can remember if repeat required*/
int	finchar;		/*first character of last substring output*/
int	lastpr;			/*last predecessor (in main loop)*/
short	cksum;			/*checksum of all bytes written to output file*/

FILE 	*infd;			/*currently open input file*/
FILE 	*outfd;			/*currently open output file*/




/*find an empty entry in xlatbl which hashes from this predecessor/suffix*/
/*combo, and store the index of the next available lzw table entry in it*/
figure(pred,suff)
int pred;
int suff;
	{
	short *hash();
	auto int disp;
	register short *p;
	p=hash(pred,suff,&disp);

	/*follow secondary hash chain as necessary to find an empty slot*/
	while(((*p)&0xffff) != EMPTY)
		{
		p+=disp;
		if(p<xlatbl || p > xlatbl+XLATBL_SIZE)
			p+=XLATBL_SIZE;
		}

	/*stuff next available index into this slot*/
	*p=entry;
	}



/*enter the next code into the lzw table*/
enterx(pred,suff)
int pred;		/*table index of predecessor*/
int suff;		/*suffix byte represented by this entry*/
	{
	register struct entry *ep;
	ep = &table[entry];


	/*update xlatbl to point to this entry*/
	figure(pred,suff);

	/*make the new entry*/
	ep->predecessor = (short)pred;
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
initb2()
	{
	register int i;
	register struct entry *p;
	p=table;

	/*first mark all entries of xlatbl as empty*/
	for(i=0;i<XLATBL_SIZE;i++) xlatbl[i]=EMPTY;

	/*enter atomic and reserved codes into lzw table*/
	for(i=0;i<0x100;i++) enterx(NOPRED,i);	/*first 256 atomic codes*/
	for(i=0;i<4;i++) enterx(IMPRED,0);	/*reserved codes*/
	}
	




/*hash pred/suff into xlatbl pointer*/
/*duplicates the hash algorithm used by CRUNCH 2.3*/
short *hash(pred,suff,disploc)
int pred;
int suff;
int *disploc;
	{
	register int hashval;
	
	hashval=((((pred>>4) & 0xff) ^suff) | ((pred&0xf)<<8)) + 1;
	*disploc=hashval-XLATBL_SIZE;
	return (xlatbl + hashval);
	}
	

/*initialize variables for each file to be uncrunched*/
intram()
	{
	trgmsk=0x1ff;	/*nine bits*/
	codlen=9;	/*    "    */
	fulflg=0;	/*table empty*/
	entry=0;	/*    "      */
	getbit=0;	/*buffer emtpy*/
	entflg=1;	/*first code always atomic*/
	repeat_flag=0;	/*repeat not active*/
	cksum=0;	/*zero checsum*/
	}


/*return a code of length "codlen" bits from the input file bit-stream*/
getcode()
	{
	register int hole;
	int code;

	/*always get at least a byte*/
	getbuf=(getbuf<<codlen)|(((long)getc(infd))<<(hole=codlen-getbit));
	getbit=8-hole;

	/*if is not enough to supply codlen bits, get another byte*/
	if(getbit<0)
		{
		getbuf |= ((long)getc(infd))<<(hole-8);
		getbit+=8;
		}

	if(feof(infd))
		{
		printf("***** Unexpected EOF on input file!\n");
		return EOFCOD;
		}

	/*skip spare or null codes*/
	if((code=((getbuf>>8) & trgmsk)) == NULCOD || code == SPRCOD)
		{
		return getcode();	/*skip this code, get next*/
		}

	return code;
	}


/*write a byte to output file*/
/*repeat byte (0x90) expanded here*/
/*checksumming of output stream done here*/
send(c)
register unsigned char c;
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
decode(code)
short code;
	{
	register unsigned char *stackp;		/*byte string stack pointer*/
	register struct entry *ep;
	ep = &table[code];

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
	while(ep > &table[255]) /*i.e. code not atomic*/
		{
		*stackp++ = ep->suffix;
		ep = &table[(ep->predecessor)&0xfff];
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
entfil(pred,suff)
int pred;		/*table index of predecessor*/
int suff;		/*suffix byte represented by this entry*/
	{
	auto int disp;
	register struct entry *ep;
	short *hash();
	short *p;
	p=hash(pred,suff,&disp);

	/*search the candidate codes (all those which hash from this new*/
	/*predecessor and suffix) for an unreferenced one*/
	while(*p!=(short)EMPTY){

		/*candidate code*/
		ep = &table[*p];
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

/*
 cisubstr(string, token) searches for lower case token in string s
 returns pointer to token within string if found, NULL otherwise
*/

char *cisubstr(s, t)
char *s,*t;
{
char *ss,*tt;
/* search for first char of token */
for(ss=s; *s; s++)
	if (*s==*t)
		/* compare token with substring */
		for(ss=s,tt=t; ;){
			if(*tt==0)
				return s;
			if(*ss++ != *tt++)
				break;
		}
return NULL;
}




/*uncrunch a single file*/
uncrunch(filename)
char *filename;
	{
	int c;			
	unsigned char *p;
	//char *cisubstr();
	unsigned char outfn[80];	/*space to build output file name*/
	int pred;			/*current predecessor (in main loop)*/
	unsigned char reflevel;		/*ref rev level from input file*/
	unsigned char siglevel;		/*sig rev level from input file*/
	unsigned char errdetect;	/*error detection flag from input file*/
	short file_cksum;		/*checksum read from input file*/

	/*initialize variables for uncrunching a file*/
	intram();

	/*open input file*/
	if ( 0 == (infd = fopen(filename,"rb")) )
		{
		printf("***** can't open %s\n", filename);
		return;
		}

	/*verify this is a crunched file*/
	if (getc(infd) != 0x76 || getc(infd) != 0xfe)
		{
		printf("***** %s is not a crunched file\n",filename);
		return;
		}

	/*extract and build output file name*/
	printf("%s --> ",filename);
	for(p=outfn; (*p=getc(infd))!='\0'; p++) *p=tolower(*p);
	*(cisubstr(outfn,".")+4)='\0'; /*truncate non-name portion*/
	printf("%s\n",outfn);

	/*open output file*/
	if ( 0 == (outfd =fopen( outfn,"wb")) )
		{
		printf("***** can't create %s\n",outfn);
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
		if(file_cksum!=cksum)
			{
			printf("***** checksum error detected in ");
			printf("%s!\n",filename);
			}
		}

	/*close files*/
	fclose(infd);
	fclose(outfd);

	/*all done this file*/
	return;
	}


main(argc,argv)
int argc;
char *argv[];
	{
	/*usage check*/
	if (argc-- < 2)
		{
		printf("Usage : uncr <crunched_file_name> ...\n");
		exit(0);
		}

	/*who am i*/
	printf("UNCRunch/C Version %s (%s)\n\n",VERSION,DATE);

#ifdef	DUMBLINKER
	/*allocate storage now for the big tables (keeps load short)*/
	table=(struct entry *)malloc(TABLE_SIZE*sizeof(struct entry));
	xlatbl=(short *)malloc(XLATBL_SIZE*sizeof(short));
	stack=(char *)malloc(TABLE_SIZE*sizeof(char));
	if(table==NULL || xlatbl==NULL || stack==NULL)
		{
		printf("***** not enough memory to run this program!\n");
		exit(0);
		}
#endif

	/*do all the files specified*/
	while(argc--) uncrunch(*++argv);

	/*all done all files*/
	exit(0);
	}
	
	
/*end of source file*/
