/* [UNIQUE.C of JUGPDS Vol.16]
*****************************************************************
*								*
*	Written by  Hakuo Katayose (JUG-CP/M No.179)		*
*		    49-114 Kawauchi-Sanjuunin-machi		*
*		    Sendai, Miyagi 980                          *
*		    Phone: 0222-61-3219				*
*								*
*	Edited & tested by Y. Monma (JUG-C/M Disk Editor)       * 
*								*
*****************************************************************
*/

/* unique - strip adjacent duplicate lines */

// usage:
//       unique  [-D]/[-F]/[-N] <filein >fileout

// converted to z88dk by Stefano Bodrato in 2022
// zcc +cpm -create-app  unique.c


#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define ON  1
#define OFF 0
#define MAXLINE 200


#define LINES 10000

char	opt_d;		/* dictionary order */
char	opt_f;		/* fold order	    */
char	opt_n;		/* counter option   */


int  strdfcmp(char *s, char *t)
{
	do {
		while( !(isalpha(*s) || isspace(*s) || isdigit(*s)) )
			s++;
		while( !(isalpha(*t) || isspace(*t) || isdigit(*t)) )
			t++;
		} while (tolower(*s) == tolower(*t) && *s != '\0');
	return( (int) (tolower(*s) - tolower(*t)) );
}


int  strdcmp(char *s, char *t)
{
	do {
		while( !(isalpha(*s) || isspace(*s) || isdigit(*s)) )
			s++;
		while( !(isalpha(*t) || isspace(*t) || isdigit(*t)) )
			t++;
		} while (*(s++) == *(t++) && *s != '\0');
	return( (int)(*s - *t) );
}


int  strfcmp(char *s, char *t)
{
	for ( ; tolower(*s) == tolower(*t); s++, t++)
		if (*s == '\0')
			return 0;
	return((int) (tolower(*s) - tolower(*t)) );
}


#define EOS      '\0'
//#define EOF      0x01a
#define NEWLINE  '\n'

int   getlin(char *s, int lim)
{
char *p;
int	c;

	p = s;
/*	while(--lim > 0 && (c = getchar()) != EOF && c != NEWLINE)  Original */
	while (1) {			/* fukusayou bousi */
		c = getchar();
		if(--lim<=0 || c == EOF || c == NEWLINE)
			break;
		*s++ = c;
	}
	if(c == NEWLINE)
		*s++ = c;
	*s = EOS;
	return( (int) (s-p) );
}


int main(int argc, char	**argv)
{
	char	buf1[MAXLINE], buf2[MAXLINE];
	int	i, len;
	unsigned wcount;
	char *ap;
	char sub[8][16];

	//dioinit(&argc,argv);
	opt_d = opt_f = opt_n = OFF;
	i = 0;
	while(--argc > 0)
		if ( (*++argv)[0] == '-')
			for (ap = argv[0]+1; *ap != '\0'; ap++)
				switch( toupper(*ap) ) {
				case 'D':
					opt_d = ON;
					break;
				case 'F':
					opt_f = ON;
					break;
				case 'N':
					opt_n = ON;
					break;
				}
		else
			strcpy(sub[i++], *argv);
	len = getlin(buf2, MAXLINE);
	while( len > 0 ) {
		strcpy(buf1, buf2);
		wcount = 1;
		while( (len = getlin(buf2, MAXLINE)) > 0 ) {
			if( opt_d == ON && opt_f == ON ) {
				if (strdfcmp( buf1, buf2) != 0)
					break;
				}
			else if( opt_d == ON ) {
				if (strdcmp( buf1, buf2) != 0)
					break;
				}
			else if( opt_f == ON ) {
				if (strfcmp( buf1, buf2) != 0)
					break;
				}
			else if (strcmp( buf1, buf2) != 0)
					break;
			wcount++;
			}
		if( opt_n == ON )
			printf( "%6d: %s", wcount, buf1);
		//printf( "%d: %s", wcount, buf1);
		else
			printf( "%s", buf1 );
		}
	//dioflush();
}


