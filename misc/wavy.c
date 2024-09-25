/*
	Wavy Lines for the H19/Z19/H89/Z89
	Written by Leor Zolman, 11/81

	This program is configured for the H19 terminal, but may be used
	on ANY cursor-addressable terminal by:

		a) modifying the first four #define lines to suit your
		   particular terminal, and
		b) modifying the cursor addressing sequence (commented)
		   in the program to work for your terminal.

	  zcc +zx -lndos -create-app wavy.c

*/


#include <conio.h>   /* z88dk way to address the terminal oddities */


/* The following four #define statements need customizing for your terminal: */

#define INIT	"\14"	/* clear screen, enter graphics mode,	*/
				/* and turn off the cursor		*/
#define UNINIT	"\14"	/* clear screen, exit graphics mode,	*/
				/* and turn cursor back on		*/

#define SLASH '/'		/* these are special characters in H19	*/
#define BACKSLASH '\\'		/* graphics mode. If you don't have an  */
				/* H19, make these simply '/' and '\\'	*/


#define MAXL 200		/* maximum number of lines at one time */

int direc[MAXL], di;
char column[MAXL], co;
int i;
int sr,maxx,maxy;
int nlines;

main()
{
top:
	srand(sr);
	puts(UNINIT);
	screensize(&maxx, &maxy);
	puts("How many wavy lines (1-200, q to quit) ? ");
	if (!scanf("%d",&nlines) || nlines < 1 || nlines > MAXL) exit(0);
	puts(INIT);
	while (getk()==0) {};

	di = rand() % 2 * 2 - 1;

	for (i=0; i<nlines; i++)	/* initialize lines */
	{
		column[i] = rand() % maxx;	/* start out at random spot */
		direc[i] = di;			/* going in same direction */
	}

	while (1)
	{
	   putch('\n');
	   if (getk()!=0) goto top;
	   sr++;

	   for (i = 0; i < nlines; i++)
	   {	
		di = direc[i];
		co = column[i];

		gotoxy(co,maxy-1);

		putch ((di < 0) ? SLASH : BACKSLASH);

		if ((di > 0) ? (rand() % (maxx - co -1)) : (rand() % (1 + co)))
			column[i] += di;	/* either keep going, or */
		else
			direc[i] = -di;		/* change direction	*/
	   }
	}
}
