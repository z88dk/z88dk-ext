
/*  zcc +zx -lndos -create-app -DINTERACTIVE -clib=ansi -pragma-define:ansicolumns=80 hanoi.c  */
/*  zcc +zx81 -lndos -create-app -DINTERACTIVE -clib=wrxansi -subtype=wrx -pragma-define:ansicolumns=80 hanoi.c  */


/*
 *		C u r s o r   c o n t r o l   f u n c t i o n s
 *
 * On VMS, cursor must be linked with kbgetc and kbputb.
 *
 */

#include <stdio.h>
#include <conio.h>


int	top[3]	= { 22, 22, 22 };

#ifdef	Z80

ttyput(buffer, buflen)
char		*buffer;	/* What to output			*/
int		buflen;		/* Number of bytes to output		*/
/*
 * Output to the console
 */
{
	int	i;
	char	*bp;

	bp = buffer;
	for (i = buflen; i-- > 0; fputc_cons(*bp++));
}

#else

ttyput(buffer, buflen)
char		*buffer;	/* What to output			*/
int		buflen;		/* Number of bytes to output		*/
/*
 * Output to the terminal.  RSX mode.
 */
{
	snprintf(buffer, buflen, vt_fd);
	fflush(vt_fd);
}

#endif

#define ESC	0233

static char dcastring[] = { ESC, 'Y', '?', '?' };


#define DCAROW	2
#define DCACOL	3



#ifdef	Z80

dca(vtcol, vtrow)
int	vtcol;		/* X -- Row on the screen			*/
int	vtrow;		/* Y -- Column on the screen			*/
{
	gotoxy(vtcol,vtrow);
}


#else

dca(x, y)
int	x;		/* X -- Column on the screen			*/
int	y;		/* Y -- Row on the screen			*/
/*
 * Move the cursor to the indicated position.  [0,0] is the top left-hand
 * corner of the screen
 */
{
	dcastring[DCACOL] = 32 + x;
	dcastring[DCAROW] = 32 + y;
	ttyput(dcastring, sizeof dcastring);
}

#endif

static char clrscr[] = { ESC, 'H', ESC, 'J', 0 };

erase()
/*
 * Clear the screen
 */
{
	ttyput(clrscr, sizeof clrscr);
}


#ifdef	Z80

cput(x, y, c)
{
	dca(x, y);
	fputc_cons(c);
}

#else
	
cput(x, y, c)
{
	dca(x, y);
	putc(c, vt_fd);
}

#endif

draw(ring, centre, y, ch)
{
	register int i;

	dca(centre-ring, y);
	for (i=0; i<ring; ++i)
#ifdef	Z80
		fputc_cons(ch);
#else
		putc(ch, vt_fd);
#endif
	dca(centre+1, y);
	for (i=0; i<ring; ++i)
#ifdef	Z80
		fputc_cons(ch);
#else
		putc(ch, vt_fd);
#endif
}

movering(ring, from, to)
{
	int fromc, toc;
	int fromy, toy;

	fromc = 15 + from*25;
	toc = 15 + to*25;
	fromy = ++top[from];
	toy = top[to]--;
	while (fromy != 10) {
		draw(ring, fromc, fromy, ' ');
		draw(ring, fromc, --fromy, 'x');
	}
	if (fromc < toc) 
		while (fromc != toc) {
			cput(fromc-ring, fromy, ' ');
			cput(fromc, fromy, 'x');
			cput(fromc+1, fromy, ' ');
			cput(fromc+ring+1, fromy, 'x');
			++fromc;
		}
	else if (fromc > toc)
		while (fromc != toc) {
			cput(fromc+ring, fromy, ' ');
			cput(fromc, fromy, 'x');
			cput(fromc-1, fromy, ' ');
			cput(fromc-ring-1, fromy, 'x');
			--fromc;
		}
	while (fromy != toy) {
		draw(ring, fromc, fromy, ' ');
		draw(ring, fromc, ++fromy, 'x');
	}
}


/*
 * Hanoi.
 */

hanoi(n, a, b, c)
{
	if (n == 0)
		return;
	hanoi(n-1, a, c, b);
	movering(n, a, b);
	hanoi(n-1, c, b, a);
}

setup(n)
{
	register int i;

	erase();
	for (i=11; i<23; ++i) {
		cput(15, i, '|');
		cput(40, i, '|');
		cput(65, i, '|');
	}
	dca(5, 23);
	for (i=5; i<76; ++i)
#ifdef	Z80
		fputc_cons('-');
#else
		putc('-', vt_fd);
#endif
	for (i=n; i>0; --i)
		draw(i, 15, top[0]--, 'x');
}


char inbuf[10];

main(argc, argv)
char *argv[];
{
	register int n;

#ifdef INTERACTIVE
		printf ("%c Enter number of rings: ",12);
		n = atoi(gets(inbuf));
#else
	if (argc == 0) {
		argc = 2;
		argv[0] = "hanoi";
		argv[1] = "5";
	}
		else {
		if (argc < 2) {
			fprintf(stderr, "Arg. count!\n");
			exit(1);
		}
		n = atoi(argv[1]);
	}
		if (n<0 || n>13) {
			fprintf(stderr, "Bad number of rings!\n");
			exit(1);
		}
#ifndef Z80
	if ((vt_fd = fopen("ti:", "wn")) == NULL)
		error("Can't open terminal");
#endif
#endif
	setup(n);
	hanoi(n, 0, 2, 1);
}

