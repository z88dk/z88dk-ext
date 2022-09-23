/*
HEADER:		;
TITLE:		Count;
VERSION:	1.1;

DESCRIPTION:	"Counts the lines, words, or characters in a file.";

KEYWORDS:	File, utility, count;
SYSTEM:		CP/M-80;
FILENAME:	COUNT.C;
AUTHORS:	Unknown;
COMPILERS:	BDS C;
*/
/************************************************************************

	Ver. 1.0:
		Author and date unknown.
	Ver. 1.1:
		Updated to BDS C, v1.44
		Rick Hollinbeck, Norwood, CO 81423

*************************************************************************
	: : : : : : : : : : : : : : : : : : : : : : : : : : : :

		Macros for constant definitions

	: : : : : : : : : : : : : : : : : : : : : : : : : : : :
*/
//#include <bdscio.h>	/* For BDS C, V1.4 */
#include <stdio.h>	/* For BDS C, V1.4 */
#include <ctype.h>

//#define EOFF -1		/* end of file marker returned by getc() */
//#define NOFILE -1	/* no such file indication given by fopen() */

#define YES 1		/* true */
#define NO 0		/* false */

#define CPMEOF 0x1A	/* CP/M's end file char for ascii files */
#define CR 0x0D		/* Carriage return */

/*	-------------------------------------------------------

	Name:		badcmd()
	Result:		---
	Errors:		---
	Globals:	---
	Macros:		---
	Procedures:	puts()

	Action:		Print the invocation error message
			on the console

	------------------------------------------------------- */


void badcmd()
	{
	puts("Correct invocation form is: COUNT <filename> <item>");
	puts("Where <item> is C[har] or W[ords] or L[ines]");
	//return;
	}

/*	: : : : : : : : : : : : : : : : : : : : : : : : : : : :
	function chrcount --count the characters in the specified input file
	: : : : : : : : : : : : : : : : : : : : : : : : : : : : */
/*	-------------------------------------------------------

	Name:		chrcount(file)
	Result:		# of chars in file
	Errors:		---
	Globals:	---
	Macros:		EOFF,EOF
	Procedures:	getc()

	Action:		---

	------------------------------------------------------- */


int chrcount(FILE *file)
	{
	unsigned cc;	/* character count */
	int c;		/* 1 char buffer */

	cc =0;
	//while( (c = getc(file)) != EOFF && c != CPMEOF)
	while( (c = getc(file)) != EOF && c != CPMEOF)
	  cc++;
	return (cc);
	}		/* end chrcount */


/*	-------------------------------------------------------

	Name:		wrdcount(file)
	Result:		# of words (strings enclosed in space) in file
	Errors:		---
	Globals:	---
	Macros:		EOFF,EOF,CR,YES,NO
	Procedures:	getc(),isspace()

	Action:		---

	------------------------------------------------------- */

int wrdcount(FILE *file)
	{
	int inword;	/* switch to tell if the present char is in a word */
	unsigned wc;	/* word count */
	int c;		/* 1 char buffer */

	wc =0;
	inword = NO;
	while( (c = getc(file)) != EOF && c != CPMEOF)
	  if(isspace(c) || c == CR)
	    inword = NO;
	  else if(inword == NO) {
	    inword = YES;
	    wc++;
	    }
	return (wc);
	}		/* end wrdcount */


/*	-------------------------------------------------------

	Name:		lincount(file)
	Result:		# of lines ('\n's) in file
	Errors:		---
	Globals:	---
	Macros:		EOFF,EOF
	Procedures:	getc()

	Action:		---

	------------------------------------------------------- */


int lincount(FILE *file)
	{
	unsigned lc;	/* line count */
	int c;		/* 1 char buffer */

	lc =0;
	while( (c = getc(file)) != EOF && c != CPMEOF)
	  if(c == '\n')
	    lc++;
	return (lc);
	}		/* end lincount */



/*	-------------------------------------------------------

	Name:		main(argc,argv)
	Result:		---
	Errors:		invocation syntax or no such file
	Globals:	---
	Macros:		NOFILE
	Procedures:	badcmd(),printf(),chrcount(),wrdcount()
			lincount(),exit(),fopen(),tolower()

	Action:		Interpert the command line
			handle invocation errors
			open file for buffered input
			handle no file error
			call the counting routine specified
			print results of counting routine on console

	------------------------------------------------------- */


void main(int argc,char *argv[])
	{
	//int fd;
	//char inbuf[BUFSIZ];
	FILE *inbuf;

	if(argc != 3)
	  badcmd();
	//else if( (fd = fopen(argv[1],inbuf)) == NOFILE )
	else if( (inbuf = fopen(argv[1],"rb")) == 0 )
	  printf("No such file %s\n",argv[1]);
	else if( tolower( *argv[2] ) == 'c' )
	  printf("There are %u characters in file %s\n",
			chrcount(inbuf),argv[1]);
	else if( tolower( *argv[2] ) == 'w' )
	  printf("There are %u words in file %s\n",wrdcount(inbuf),argv[1]);
	else if( tolower( *argv[2] ) == 'l' )
	  printf("There are %u lines in file %s\n",lincount(inbuf),argv[1]);
	else
	  badcmd();

	//exit();
	}

