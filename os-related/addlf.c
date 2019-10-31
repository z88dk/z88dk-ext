
/* zcc +cpm -create-app -oaddlf addlf.c */
/* example:   c> addlf <infile.txt >outfile.txt


/* addlf -- copy input to output; add line-feeds only if necessary.
 *	    WHRauser   10-4-83		   a better mouse trap.
 */
#include <stdio.h>	   /* Microsoft C  Ver 1.04 */

#define  CR  0x000D	   /* carriage return */
#define  LF  0x000A	   /* line feed */
#define  TRUE	  1
#define  FALSE	  0

main()	    /* copy input to output and add line-feeds only if needed. */
{
    int  c;
    int  addlf = FALSE;

    while ((c = getchar()) != EOF) {
	 if (addlf  &  c != LF) {
	      putchar(LF);
	      addlf = FALSE;
	 }
	 putchar(c);
	 if (c == CR)  addlf = TRUE;
    }
}
