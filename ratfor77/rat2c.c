
/* This tool to changes the '#' FORTRAN style comments into 'C'.
   
   z88dk conversion

   to build:
      zcc +cpm -create-app rat2c.c
   
   Original header follows
   NOTE: THIS VERSION IS NOT VALID FOR BDS C ANYMORE !

*/

/* BDS C program to convert from RATFOR to C style comments
 * source:  rat2c.bds
 * version: May 21, 1981.
 */

/* define globals */

#include <stdio.h>
#include <fcntl.h>

#define YES 1
#define NO  0
#define EOS 0
#define CR  13
#define LF  10

#define VERSION "ver: May 21, 1981"
#define SIGNON	"Welcome to RAT4 to C comment converter.  "


//int  input;
//int  output;
FILE  *input;
FILE  *output;

char buffer[100];		/* general buffer */
//char inbuf[BUFSIZ];		/* input file buffer */
//char outbuf[BUFSIZ];		/* output file buffer */



/* output CR and LF to console */

ccrlf()
{
	putchar(CR);
	putchar(LF);
}


/* open the input file, which must exist.
 * keep trying until the file is open or the user gives up.
 */

openin()
{
	while (1) {
		ccrlf();
		puts("input file ?  ");
		gets(buffer);
		if (buffer[0] == EOS) {
			return(NO);
		}
		//input = fopen (buffer,inbuf);
		//if (input == -1) {
		if ((input = fopen(buffer,"rb")) == NULL) {
			ccrlf();
			puts("file not found");
		}
		else {
			return(YES);
		}
	}
}


/* open the output file, which must not exist.
 * keep trying until the file is open or the user gives up.
 */

openout()
{
	while (1) {
		ccrlf();
		puts("Output file ?  ");
		gets(buffer);
		if (buffer[0] == EOS) {
			return(NO);
		}
		/* make sure the file does not exist */
		//output = fopen(buffer,outbuf);
		//if (output != -1) {
		if ((output = fopen(buffer,"rb")) != NULL)
        {
			ccrlf();
			puts("File exists");
			fclose(output);
			continue;
        }

		//output = fcreat(buffer,output);
		//output = fopen(buffer,"wb");
		//if (output == -1) {
		if ((output = fopen(buffer,"wb")) == NULL)
		{
			ccrlf();
			puts("disk error");
		} else {
			return(YES);
		}
	}
}




/* open an input and output file.
 * the input file must exist.
 * the output file must not exist.
 * return NO if the user gives up by entering a null file name.
 */

openio()
{
	if (openin() == NO) {
		return(NO);
	}
	else {
		return(openout());
	}
}




/* close any open files */

closeio()
{
	if (input != NULL) {
		fclose(input);
	}
	if (output != NULL) {
		fflush(output);
		fclose(output);
	}
}

/* copy one line from input file to output file.
 * return YES if end of file has been seen.
 */

do1line()
{
int c;			/* the current character */
int comflag;		/* YES means in comment */

	/* start off each line not in a comment */
	comflag=NO;
	while (1) {
		/* get next character from input */
		c = fgetc(input);
		//if ( (c == -1) || (c == 0x1a) ) {
		if (c == EOF) {
			/* end of file */
			if (comflag == YES) {
				/* finish comment */
				fputc(' ',output);
				fputc('*',output);
				fputc('/',output);
			}
			/* output end of file mark */
			fputc(0x01a,output);
			return(YES);
		}
		else if ((comflag == NO) &
			 (c == '#') ) {
			/* start of comment */
			comflag = YES;
			fputc('/',output);
			fputc('*',output);
			fputc(' ',output);
		}
		else if (c == CR) {
			/* end line. assume LF will follow */
			c = fgetc(input);
			if (comflag == YES) {
				/* finish comment */
				fputc(' ',output);
				fputc('*',output);
				fputc('/',output);
			}
			/* finish line */
			fputc(CR,output);
			fputc(LF,output);
			/* no EOF seen yet */
			return(NO);
		}
		else {
			/* just copy next character */
			fputc(c,output);
		}
	}
}


/* copy input file to output file.
 * convert everything following a '#' on a line into
 * a C style comment.
 */

convert()
{
	/* copy one line at a time */
	while (do1line() == NO) {
		;
	}
}




/* convert one file after another interactively */

main()
{
	/* sign on */
	puts(SIGNON);
	puts(VERSION);
	ccrlf();
	/* mark files as closed */
	//input = -1;
	//output = -1;
	while (1) {
		if (openio() == NO) {
			closeio();
			break;
		}
		convert();
		closeio();
	}
	puts("End of comment converter");
	ccrlf();
}
