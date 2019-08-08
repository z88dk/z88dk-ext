
/* Ported to gcc and z88dk by Stefano Bodrato  */
/* I had to provide a custom version of strpos */
/* to get it in working order, this shows      */
/* how far is z88dk from Small C, actually.    */

/* by Jim Woolley, FOG Disk Librarian, 12/82 */
/* an exercise in small-c  */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifndef SMALL_C
#define putch(a) fputc_cons(a)
#define CLS    26
#else
#define CLS    12
#define putch(a) fputc(a,stdout)
#endif

int strposn(char *haystack, char *needle)
{
   char *p = strstr(haystack, needle);
   if (p) return (-1);
   return 0;
}


#define TRUE   1
#define FALSE  0
#define END    "$"

int right, cclose, wrong;
char *skip;



main()         /* word puzzle game */

/* by Jim Woolley, FOG Disk Librarian, 12/82 */
/* an exercise in small-c  */

{
   int puzzle[12], answer[50];

   right=cclose=wrong=0;
   skip="\n\n\n\n\n\n";

   inpuzzle(puzzle);
   inanswer(answer);
   game(puzzle,answer);
   result();
}

inpuzzle(p)    /* define word puzzles */
int p[];       /* char pointer array did not work */
{
   p[0]="	 c c\n	garage\n	 r r\n";
   p[1]="	c\n	o\n	m\n	i\n	c\n";
   p[2]="	1234567\n	Dracula\n";
   p[3]="	drinks\n	house\n";
   p[4]="	 oo  y\n	g  db e\n";
   p[5]="	ieieceiie\n";
   p[6]="	CLAMS\n	SHe\n";
   p[7]="	t rn\n";
   p[8]="	HAir____\n";
   p[9]="	mirror\n	mirror\n	mirror\n	mirror\n";
   p[10]="	grnese\n	grenes\n	grseen\n	grness\n";
   p[11]=END;
}

inanswer(a)    /* define answers */
int a[];       /* char pointer array did not work */
{
   a[0]="TWO";a[1]="CAR";a[2]="GARAGE";a[3]=END;
   a[4]="STAND";a[5]="UP";a[6]="COMIC";a[7]=END;
   a[8]="COUNT";a[9]="DRACULA";a[10]=END;
   a[11]="DRINKS";a[12]="ON";a[13]="THE";a[14]="HOUSE";a[15]=END;
   a[16]="WAVING";a[17]="GOODBYE";a[18]=END;
   a[19]="I";a[20]="BEFORE";a[21]="E";a[22]="EXCEPT";a[23]="AFTER";a[24]="C";
   a[25]=END;
   a[26]="CLAMS";a[27]="ON";a[28]="THE";a[29]="HALF";a[30]="SHELL";a[31]=END;
   a[32]="NO";a[33]="U";a[34]="TURN";a[35]=END;
   a[36]="RECEDING";a[37]="HAIRLINE";a[38]=END;
   a[39]="FULL";a[40]="LENGTH";a[41]="MIRROR";a[42]=END;
   a[43]="MIXED";a[44]="GREENS";a[45]=END;
}

game(puzzle,answer)    /* play the game */
int puzzle[], answer[];
{
   int i, ans, correct, cnt, j;
   char guess[128];

   putch(CLS);
   puts(skip);
   puts("\nWord Puzzle, by Jim Woolley, 12/82\n\n");
   puts("\nI will display word puzzles.\n");
   puts("You guess what they are.");

   ans=0;

   for (i=0; test(puzzle[i]); i++)
   {
       header();
       puts(puzzle[i]);
       puts("\n\nWhat do you think this is?\n");
       gets(guess);
       convert(guess);

       cnt=0; correct=0;
       while (test(answer[ans]))
       {
           if (strposn(guess,answer[ans]))
               correct++;
           ans++; cnt++;
       }

       if (correct==cnt)
       {
           puts("\nVery good!");
           right++;
       }
       else if (correct>0)
       {
           puts("\nGood try.");
           cclose++;
       }
       else 
       {
           puts("\nWrong.");
           wrong++;
       }

       puts("  The answer is");
       for (j=(ans-cnt); j<ans; j++)
       {
           putch(' ');
           puts(answer[j]);
       }
       ans++;
   }
}

header()       /* let user read screen */
{
   puts("\n\nPress any key to continue ...");
#ifndef SMALL_C
   fgetc(stdin);
   putch(CLS); puts(skip);
#else
   while (getk()) {};
   while (!getk()) {};
   putch(CLS); puts(skip);
#endif
}

test(sp)       /* test for end character */
char *sp;
{
   if (*sp=='$')
       return(FALSE);
   else
       return(TRUE);
}

convert(sp)    /* convert string to upper case */
char *sp;
{
   while (*sp != '\0')
   {
       *sp=toupper(*sp);
       sp++;
   }
}

result()
{
   header();   
   puts("You had "); outdec(right); puts(" correct, ");
   outdec(cclose); puts(" close, and "); outdec(wrong);
   puts(" wrong.  Thank you."); puts(skip);
}

/********************************************************/
/*							*/
/*	outdec						*/
/*							*/
/*	function:	to output number in dec from	*/
/*			to console			*/
/*							*/
/*	date written:	Dec 16, 1980 By Mike Bernson	*/
/*							*/
/********************************************************/
outdec(number)
int number;
{
	char zero,num;
	int  place;

	place=10000;
	zero=0;

	while(place>0) {
		num=number/place+'0'; /* get current digit */
		if (num != '0' || place ==1 || zero) {
			zero=1; /* set zero suppress */
			putch(num); /* print character */
			}
		number=number % place;
		place=place/10;
		}
}

