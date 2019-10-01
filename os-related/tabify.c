
/* CPM only ! */
/* zcc +cpm -create-app -O3 tabify.c */

/*
	Tabify.c	written by Leor Zolman

	This filter takes sequences of spaces in a file and turns
	them, whenever possible, into tabs. Usage:

		A>tabify oldfile newfile <cr>

	Quoted strings are not processed, but there should NOT be
	any `lone' double quotes within the file being tabified.
*/

#include <bdscio.h>

int scount, column, i;
FILE *ifd;
FILE *ofd;
int c;
//char ibuf[BUFSIZ], obuf[BUFSIZ];

main(argc,argv)
char **argv;
{
	if (argc < 2 || argc > 3) {
		printf("usage: tabify oldfile [newfile]\n");
		exit(0);
	}

	ifd = fopen(argv[1],"r");

	if (argc == 2) argv[2] = "tabify.tmp";
	//ofd = fcreat(argv[2],obuf);
	ofd = fopen(argv[2],"w");

	if (ifd == NULL  || ofd == NULL ) {
		printf("Can't open file(s)\n");
		exit(1);
	}

	scount = column = 0;

	do {
		c = fgetc(ifd);
		if (c == EOF) {
			fputc(CPMEOF,ofd);
			break;
		 }
		switch(c) {
		   case '\r':	fputc(c,ofd);
				scount = column = 0;
				break;
		   case '\n':	fputc(c,ofd);
				scount = 0;
				putchar('*');
				break;
		   case ' ':	column++;
				scount++;
				if (!(column%8)) {
				   if (scount > 1)
					fputc('\t',ofd);
				   else
					fputc(' ',ofd);
					scount = 0;
				 }
				break;
		   case '\t':	scount = 0;
				column += (8-column%8);
				fputc('\t',ofd);
				break;
		   case '"':	for (i = 0; i < scount; i++)
					fputc(' ',ofd);
				fputc('"',ofd);
				do {
				   c = fgetc(ifd);
				   if (c == EOF) {
				    printf("\nQuote error.\n");
				    exit(1);
				   }
				   fputc(c,ofd);
				} while (c != '"');
				do {
					c = fgetc(ifd);
					fputc(c,ofd);
				} while (c != '\n');
				column = scount = 0;
				break;
		   case CPMEOF:	fputc(CPMEOF,ofd);
				break;
		   default:	for (i=0; i<scount; i++)
					fputc(' ',ofd);
				scount = 0;
				column++;
				fputc(c,ofd);
		 }
	 } while (c != CPMEOF);

	fclose(ifd);
	fclose(ofd);

	if (argc == 2) {
		unlink(argv[1]);
		rename(argv[2],argv[1]);
	}
	
	putchar('\n');
	return(0);
}

putc1(c,buf)
char c;
{
	if (putc(c,buf) < 0) {
		printf("\nWrite error (out of disk space?)\n");
		exit(1);
	}
}

