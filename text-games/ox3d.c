         /* OX3D - an aggressive 3D noughts and crosses player. */
		     /* Bugfixed and compacted - S.Bodrato 2013 */
                /* Tidiead up and improved MLA 1997 */
                  /* Translated to C by MLA 1990 */
           /* FORTRAN version developed on Prime by MLA 1986 */
      /* Based on an earlier algorithm developed by MLA 1974 - 1981 */

/* This version is dedicated to the proposition that a Real Programmer */
               /* can write FORTRAN in *any* language */

/*
 * zcc +zx -lndos -create-app -zorg=26500 -O3 -lm -DHELP -o ox3d ox3d.c
 * zcc +osca -lndos -oox3d.exe -O3 ox3d.c
 */

#pragma output osca_notimer

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <stdlib.h>
//#include <math.h>

#define COMSIZ (1817 * sizeof (int) + 128 * sizeof (long))
//#define COMSIZ (1817 * sizeof (char) + 128 * sizeof (long))

char board [64];               /* The playing board */
char cplane [384];             /* Cell plane lists */
int linsco [76];              /* Individual line scores */
char lcells [304];             /* Line cell lists */
char clines [448];             /* Cell line lists */
char clinh [448];              /* Cell line histograms */
int plnsco [18];              /* Individual plane scores */
int player;                   /* Shows whose move it is */
int sign;                     /* Stores the sign of a value */
int winner;                   /* Indicates the winner */
char win [2];                  /* Forced win flags */
char kkill [2];                 /* Stores forced moves */
int move;                     /* A move */
int movcnt;                   /* Move counter */
int strcnt;                   /* Strategic search depth */
int lincnt;                   /* Number of lines left */
char strmov [64];              /* Strategic moves */
long tacval [64];             /* Cell tactical values */

char bckup1 [COMSIZ];         /* Backup array for strategic lookahead */
char bckup2 [COMSIZ];         /* Ditto for tactical evaluations */
char bckup3 [COMSIZ];         /* Ditto for move suggestions */

int mute;                     /* Keep quiet flag */
int ttytyp;                   /* Terminal type */
int frcpnt;                   /* Forced move "pointer" */
int frccnt;                   /* Forced move counter */
int frcmov [64];              /* Forced moves */
int vismov;                   /* Move in visual representation */
int who [2];                  /* Shows who plays which side */
char rwho [2];                /* Shows who started playing which side */

char *plname [2] =            /* Player names */
{
   "Noughts",
   "Crosses"
};

char icplan [384] =
{
   0,  4,  8, 12, 14, 16,  0,  5,  8, 14, -1, -1,
   0,  6,  8, 14, -1, -1,  0,  7,  8, 13, 14, 17,
   0,  4,  9, 12, -1, -1,  0,  5,  9, 16, -1, -1,
   0,  6,  9, 17, -1, -1,  0,  7,  9, 13, -1, -1,
   0,  4, 10, 12, -1, -1,  0,  5, 10, 17, -1, -1,
   0,  6, 10, 16, -1, -1,  0,  7, 10, 13, -1, -1,
   0,  4, 11, 12, 15, 17,  0,  5, 11, 15, -1, -1,
   0,  6, 11, 15, -1, -1,  0,  7, 11, 13, 15, 16,
   1,  4,  8, 16, -1, -1,  1,  5,  8, 12, -1, -1,
   1,  6,  8, 13, -1, -1,  1,  7,  8, 17, -1, -1,
   1,  4,  9, 14, -1, -1,  1,  5,  9, 12, 14, 16,
   1,  6,  9, 13, 14, 17,  1,  7,  9, 14, -1, -1,
   1,  4, 10, 15, -1, -1,  1,  5, 10, 12, 15, 17,
   1,  6, 10, 13, 15, 16,  1,  7, 10, 15, -1, -1,
   1,  4, 11, 17, -1, -1,  1,  5, 11, 12, -1, -1,
   1,  6, 11, 13, -1, -1,  1,  7, 11, 16, -1, -1,
   2,  4,  8, 16, -1, -1,  2,  5,  8, 13, -1, -1,
   2,  6,  8, 12, -1, -1,  2,  7,  8, 17, -1, -1,
   2,  4,  9, 15, -1, -1,  2,  5,  9, 13, 15, 16,
   2,  6,  9, 12, 15, 17,  2,  7,  9, 15, -1, -1,
   2,  4, 10, 14, -1, -1,  2,  5, 10, 13, 14, 17,
   2,  6, 10, 12, 14, 16,  2,  7, 10, 14, -1, -1,
   2,  4, 11, 17, -1, -1,  2,  5, 11, 13, -1, -1,
   2,  6, 11, 12, -1, -1,  2,  7, 11, 16, -1, -1,
   3,  4,  8, 13, 15, 16,  3,  5,  8, 15, -1, -1,
   3,  6,  8, 15, -1, -1,  3,  7,  8, 12, 15, 17,
   3,  4,  9, 13, -1, -1,  3,  5,  9, 16, -1, -1,
   3,  6,  9, 17, -1, -1,  3,  7,  9, 12, -1, -1,
   3,  4, 10, 13, -1, -1,  3,  5, 10, 17, -1, -1,
   3,  6, 10, 16, -1, -1,  3,  7, 10, 12, -1, -1,
   3,  4, 11, 13, 14, 17,  3,  5, 11, 14, -1, -1,
   3,  6, 11, 14, -1, -1,  3,  7, 11, 12, 14, 16
};

char ilcell [304] =
{
   0,  1,  2,  3,  0,  4,  8, 12,
   0,  5, 10, 15,  0, 16, 32, 48,
   0, 17, 34, 51,  0, 20, 40, 60,
   0, 21, 42, 63,  1,  5,  9, 13,
   1, 17, 33, 49,  1, 21, 41, 61,
   2,  6, 10, 14,  2, 18, 34, 50,
   2, 22, 42, 62,  3,  6,  9, 12,
   3,  7, 11, 15,  3, 18, 33, 48,
   3, 19, 35, 51,  3, 22, 41, 60,
   3, 23, 43, 63,  4,  5,  6,  7,
   4, 20, 36, 52,  4, 21, 38, 55,
   5, 21, 37, 53,  6, 22, 38, 54,
   7, 22, 37, 52,  7, 23, 39, 55,
   8,  9, 10, 11,  8, 24, 40, 56,
   8, 25, 42, 59,  9, 25, 41, 57,
  10, 26, 42, 58, 11, 26, 41, 56,
  11, 27, 43, 59, 12, 13, 14, 15,
  12, 24, 36, 48, 12, 25, 38, 51,
  12, 28, 44, 60, 12, 29, 46, 63,
  13, 25, 37, 49, 13, 29, 45, 61,
  14, 26, 38, 50, 14, 30, 46, 62,
  15, 26, 37, 48, 15, 27, 39, 51,
  15, 30, 45, 60, 15, 31, 47, 63,
  16, 17, 18, 19, 16, 20, 24, 28,
  16, 21, 26, 31, 17, 21, 25, 29,
  18, 22, 26, 30, 19, 22, 25, 28,
  19, 23, 27, 31, 20, 21, 22, 23,
  24, 25, 26, 27, 28, 29, 30, 31,
  32, 33, 34, 35, 32, 36, 40, 44,
  32, 37, 42, 47, 33, 37, 41, 45,
  34, 38, 42, 46, 35, 38, 41, 44,
  35, 39, 43, 47, 36, 37, 38, 39,
  40, 41, 42, 43, 44, 45, 46, 47,
  48, 49, 50, 51, 48, 52, 56, 60,
  48, 53, 58, 63, 49, 53, 57, 61,
  50, 54, 58, 62, 51, 54, 57, 60,
  51, 55, 59, 63, 52, 53, 54, 55,
  56, 57, 58, 59, 60, 61, 62, 63
};

char icline [448] =
{
   0,  1,  2,  3,  4,  5,  6,  0,  7,  8,  9, -1, -1, -1,
   0, 10, 11, 12, -1, -1, -1,  0, 13, 14, 15, 16, 17, 18,
   1, 19, 20, 21, -1, -1, -1,  2,  7, 19, 22, -1, -1, -1,
  10, 13, 19, 23, -1, -1, -1, 14, 19, 24, 25, -1, -1, -1,
   1, 26, 27, 28, -1, -1, -1,  7, 13, 26, 29, -1, -1, -1,
   2, 10, 26, 30, -1, -1, -1, 14, 26, 31, 32, -1, -1, -1,
   1, 13, 33, 34, 35, 36, 37,  7, 33, 38, 39, -1, -1, -1,
  10, 33, 40, 41, -1, -1, -1,  2, 14, 33, 42, 43, 44, 45,
   3, 46, 47, 48, -1, -1, -1,  4,  8, 46, 49, -1, -1, -1,
  11, 15, 46, 50, -1, -1, -1, 16, 46, 51, 52, -1, -1, -1,
   5, 20, 47, 53, -1, -1, -1,  6,  9, 21, 22, 48, 49, 53,
  12, 17, 23, 24, 50, 51, 53, 18, 25, 52, 53, -1, -1, -1,
  27, 34, 47, 54, -1, -1, -1, 28, 29, 35, 38, 49, 51, 54,
  30, 31, 40, 42, 48, 50, 54, 32, 43, 52, 54, -1, -1, -1,
  36, 47, 51, 55, -1, -1, -1, 37, 39, 49, 55, -1, -1, -1,
  41, 44, 50, 55, -1, -1, -1, 45, 48, 52, 55, -1, -1, -1,
   3, 56, 57, 58, -1, -1, -1,  8, 15, 56, 59, -1, -1, -1,
   4, 11, 56, 60, -1, -1, -1, 16, 56, 61, 62, -1, -1, -1,
  20, 34, 57, 63, -1, -1, -1, 22, 24, 38, 42, 58, 59, 63,
  21, 23, 35, 40, 60, 61, 63, 25, 43, 62, 63, -1, -1, -1,
   5, 27, 57, 64, -1, -1, -1,  9, 17, 29, 31, 59, 61, 64,
   6, 12, 28, 30, 58, 60, 64, 18, 32, 62, 64, -1, -1, -1,
  36, 57, 61, 65, -1, -1, -1, 39, 44, 59, 65, -1, -1, -1,
  37, 41, 60, 65, -1, -1, -1, 45, 58, 62, 65, -1, -1, -1,
   3, 15, 34, 42, 66, 67, 68,  8, 38, 66, 69, -1, -1, -1,
  11, 40, 66, 70, -1, -1, -1,  4, 16, 35, 43, 66, 71, 72,
  20, 24, 67, 73, -1, -1, -1, 22, 68, 69, 73, -1, -1, -1,
  23, 70, 71, 73, -1, -1, -1, 21, 25, 72, 73, -1, -1, -1,
  27, 31, 67, 74, -1, -1, -1, 29, 69, 71, 74, -1, -1, -1,
  30, 68, 70, 74, -1, -1, -1, 28, 32, 72, 74, -1, -1, -1,
   5, 17, 36, 44, 67, 71, 75,  9, 39, 69, 75, -1, -1, -1,
  12, 41, 70, 75, -1, -1, -1,  6, 18, 37, 45, 68, 72, 75
};

char iclinh [64] =
{
   7, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 4, 7, 4, 4, 7, 
   4, 4, 4, 4, 4, 7, 7, 4, 4, 7, 7, 4, 4, 4, 4, 4, 
   4, 4, 4, 4, 4, 7, 7, 4, 4, 7, 7, 4, 4, 4, 4, 4, 
   7, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 4, 7, 4, 4, 7
};

/************************************************************************/
/* Display current board. */

void display ()
{
   int i, j, k;
   int kmax;
   char symbol;
   
   for (i = 0; i < 13; i+=4)
   {
      printf ("\n");
      for (j = i; j < 61; j += 16)
      {
         printf ("   ");
         kmax = j + 4;
         for (k = j; k < kmax; k++)
         {
            symbol = '.';
            if (k == win  [0] || k == win  [1] ||
                k == kkill [0] || k == kkill [1])
               symbol = '+';
            if (board [k] == -1)
               symbol = 'O';
            else if (board [k] == 1)
               symbol = 'X';
            else if (board [k] == 3)
               symbol = '*';
            //putchar (' ');
            putchar (symbol);
         }
      }
   }
   putchar ('\n');
   putchar ('\n');
}

/************************************************************************/
/* There is no simple sort in ANSI C library! */

void sort_it (numbers, index, size)
long *numbers;
int *index;
int size;
{
   int step;
   int temp;
   int do_more;
   int i;
   
   for (i = 0; i < size; i++)
      *(index + i) = i;
      
   step = size;
   while (step > 1)
   {
      step = step / 3 + 1;
      do_more = 1;
      while (do_more)
      {
         do_more = 0;
         for (i = 0; i + step < size; i++)
         {
            if (*(numbers + *(index + i)) < *(numbers + *(index + i + step)))
            {
               temp = *(index + i);
               *(index + i) = *(index + i + step);
               *(index + i + step) = temp;
               do_more = 1;
            }
         }
      }
   }
}

/************************************************************************/
/* A brief statement of the rules */

void intro ()
{
   printf ("\nThe game of 3D 0&X is played on a 4*4*4 board. The aim is to complete ");
   printf ("a straight line of own cells (in any  direction). Cells are referred to ");
   printf ("by 3-digit numbers indicating the tier (1 to 4), the row   within the tier  ");
   printf ("(1 to 4) and the cell within the row  (1 to 4). E.g. 213 indicates 2nd ");
   printf ("tier, 1st row, 3rd cell. The following commands are also available (all ");
   printf ("abbreviable to a single character):\n\n");
   printf ("Restart  - aborts the current game\n");
   printf ("Quit     - aborts the game\n");
   printf ("Display  - re-display the board\n");
   printf ("Suggest  - asks for a suggestion\n");
   printf ("Atomatic - takes over the current player\n");
#ifdef HELP
   printf ("Help     - lists available commands\n");
#endif
//   printf ("\nYou can play just one side, or both sides (in order to pre-set a position)\n");
//   printf ("or ask the program to play both sides and just sit and watch the show.\n");
}

/************************************************************************/
/* A function to get and parse answer to a question. */

int get_reply (prompt, options, dflt)
char *prompt;
char *options;
char dflt;
{
   char *rptr;
   char repc;
   char reply [10];
   
   while (1)
   {
      printf (prompt);
      fgets (reply, sizeof (reply), stdin);
      if (*reply == '\n' && dflt)
         return (tolower (dflt));
      rptr = options;
      repc = tolower (*reply);
      while (*rptr)
      {
         if (tolower (*rptr) == repc)
            return (repc);
         rptr++;
      }
      puts ("Please answer the question!");
   }
}

/************************************************************************/
/* A more detailed help. */
#ifdef HELP
void help ()
{
   puts ("You can either specify your move, or enter any of the commands listed");
   puts ("below (all can be abbreviated to a single character):");
   putchar ('\n');
   puts ("Suggest   - asks the program to suggest the best move available to you");
   puts ("            in the current position.");
   puts ("Automatic - will cause the program to start playing for the current player.");
   puts ("            It does not mean that you are given the control of the other");
   puts ("            player, however, since the program is perfectly capable of playing");
   puts ("            against itself. This command is useful for asking the program to ");
   puts ("            play one (or both) sides of a position you have just pre-set.");
   puts ("            Also, when your opponent announces a forced win and you cannot");
   puts ("            be bothered to play through to the inevitable defeat, you may");
   puts ("            use this command to see how the win is arrived at, without");
   puts ("            having to play the moves.");
   puts ("Restart   - will abandon the current game and give you an option of starting");
   puts ("            a new one. Useful in a tight corner, but your opponent may");
   puts ("            grumble if he has a forced win.");
   puts ("Quit      - quits out of the game altogether. This command is, in fact,");
   puts ("            available to you whenever you are asked for any input.");
   puts ("Display   - will completely redraw the board. Handy if your display has got");
   puts ("            messed up somehow.");
   puts ("Help      - Prints this text.");
   putchar ('\n');
   switch (get_reply ("More? (Y/N/Q) [Y]: ", "ynq", 'y'))
   {
      case 'n':
         display ();
         return;
         
      case 'q':
         exit (0);
         
      case 'y':
         break;
   }
   putchar ('\n');
   puts ("Moves are specified by three digit numbers, with the digits in the range ");
   puts ("from 1 to four inclusive. No spaces or other separators are allowed between");
   puts ("individual digits. The 1st digit describes the tier on which you want to ");
   puts ("place your marker. These are numbered from left to right or (in real 3d)");
   puts ("from top to bottom). The second digit specifies the row within the tier,");
   puts ("the count starting from the topmost row. Finally, the 3rd digit shows the");
   puts ("cell within the row. Cells are numbered from left to right.");
   putchar ('\n');
   puts ("For example the following diasplay shows a nought in the position 324 and");
   puts ("a cross in the position 144.");
   putchar ('\n');
   puts ("            . . . .    . . . .     . . . .     . . . .");
   puts ("            . . . .    . . . .     . . . O     . . . .");
   puts ("            . . . .    . . . .     . . . .     . . . .");
   puts ("            . . . X    . . . .     . . . .     . . . .");
   puts ("");
   puts ("A plus sign on the display indicates a cell in an almost complete line.");
   puts ("I.e. by occupying it, one of the players would win the game. When the");
   puts ("game is won, all plus signs are removed from the board and the winning");
   puts ("line is picked out with asterisks.");
   putchar ('\n');
   while (1)
   {
      if (get_reply ("Finished reading? [Y]: ", "yn", 'y') == 'y')
         break;
   }
   display ();
   return;
}
#endif
/************************************************************************/
/* Opening moves are special! */

short diags [16] =              /* The four body diagonals */
{
      0, 21, 42, 63,
      3 ,22, 41, 60,
     12, 25, 38, 51,
     15, 26, 37, 48
};

void opening ()
{
   short i, j;          /* Loop counters */
   short cell;                  /* A cell */
   short myone;                 /* Position of my cell in diagonal */
   short hisone;                /* Ditto for his cell */
   short spoilt;                /* The spoilt diagonal id */

   if (movcnt <= 0) 

/* None of the important cells occupied. Choose any one. */

   {
      move = diags [rand () % 16];
      return;
   }

/* Check through the diagonals and ignore free ones. If there is a */
/* spoiled one, note it. If an owned diagonal is found, we want to */
/* move into it!                                                   */

   spoilt = -1;                        /* No spoiled so far */
   for (i = 0; i <= 3; i++)
   {
      myone = 0;
      hisone = 0;

/* Work out the ownership of this body diagonal. */

      for (j = 0; j <= 3; j++)
      {
         cell = diags [4 * i + j];
         if (board [cell] == 0) continue;
         if (board [cell] == sign)
         {
            if (myone == 0) 
               myone = j + 1;
            else
               myone = -myone;
         }
         else
         {
            if (hisone == 0)
               hisone = j + 1;
            else
               hisone = -hisone;
         }
      }

      if (myone == 0 && hisone == 0) continue; /* Free line */
      if (myone != 0 && hisone != 0)           /* Overcrowded line */
      {
         spoilt = i;                       /* Remember spoiled line */
         continue;
      }

      if (hisone == 0)
      {
         move = diags [4 - myone + 4 * i];
         return;
      } 
      else
      {
         if (hisone != 1 && hisone != 4)
         {
            strmov [0] = diags [0 + 4 * i];
            strmov [1] = diags [3 + 4 * i];
         }
         else
         {
            strmov [0] = diags [1 + 4 * i];
            strmov [1] = diags [2 + 4 * i];
         }
         move = strmov [rand () % 2];
         return;
      }
   }
   if (spoilt == -1)
   {
      move = diags [rand () % 16];
      return;
   }

/*    The choice consists of all body diagonal cells, except for the
 *    spoiled diagonal.
 */
   strcnt = 0;
   for (i = 0; i < 4; i++)
   {
      if (i != spoilt)
      for (j = 0; j < 4; j++)
      {
         cell = diags [j + 4 * i];
         if (board [cell] == 0)
            strmov [strcnt++] = cell;
      }
   }
   move = strmov [rand () % strcnt];
}

/************************************************************************/
/* Initialise everything for a new game. */

void init ()
{
   int i, j;
   char prompt [80];

   who [0] = who [1] = 0;
   for (player = 0; player < 2; player++)
   {
      sprintf (prompt, "Who plays %s, me or you? ", plname [player]);
      if (rwho [player] == 'y')
         strcat(prompt, "[me again] ");
      else if (rwho [player] == 'm')
         strcat (prompt, "[you again] ");
      else
         rwho [player] = 0;
      switch (get_reply (prompt, "myq", rwho [player]))
      {
         case 'm':                       /* He says Me, so... */
            rwho [player] = 'm';
            who [player] = 'h';          /* Him or Human */
            printf ("OK... You play ");
            if (who [0] == who [1])
               printf ("%s *and* %s!\n", plname [0], plname [1]);
            else
               printf ("%s.\n", plname [player]);
            break;
            
         case 'y':                       /* He says You, so... */
            rwho [player] = 'y';
            who [player] = 'm';          /* Me or Machine */
            printf ("OK... I'll play ");
            if (who [0] == who [1])
               printf ("%s *and* %s!\n", plname [0], plname [1]);
            else
               printf ("%s.\n", plname [player]);
            break;
            
         case 'q':
            puts ("If you wish...\n");
            exit (0);
      }     
   }

   srand (time (NULL));
   
   memset (clinh,  '\0', sizeof (clinh));
   memset (linsco, '\0', sizeof (linsco));
   memset (plnsco, '\0', sizeof (plnsco));
   memcpy (clines, icline, sizeof (clines));
   memcpy (lcells, ilcell, sizeof (lcells));
   memcpy (cplane, icplan, sizeof (cplane));

   for (i = 0, j = 3; i < 64; i++, j +=7)
   {
      board [i] = 0;
      clinh [j] = iclinh [i];
   }
      
   move = 0;
   lincnt = 76;
   frccnt = 0;
   movcnt = 0;
   mute = 0;
   winner = -1;
   win [0] = -1;
   win [1] = -1;
   kkill [0] = -1;
   kkill [1] = -1;
   player = 0;
   sign = -1;
   vismov = 0;

   if (who [0] != 'm' || who [1] != 'm')
      display ();
   else 
      putchar ('\n');
}

/*********************************************************************/
/* Check a proposed move */

int check_move (text)
char *text;
{
   int i;
   char *cptr = text;
   
   for (i = 0; i < 3; i++, cptr++)
   {
      if (*cptr < '1' || *cptr > '4')
         break;
   }
   if (i == 0) 
      return (1);
   if (i < 3 || (*cptr && *cptr != '\n'))
   {
      printf ("%s is not a valid move description.\n", text);
#ifdef HELP
      printf ("If in doubt, type H for help.\n");
#endif
      return (1);
   }
   return (0);      
}

/************************************************************************/
/* Get move from player. */

void get_move ()
{
   char reply [10];

   while (1)
   {
      if (who [0] == who [1])
         printf ("%s' move ", plname [player]);
      else
         printf ("Your move ");
      if (vismov > 0) 
         printf ("[%d]: ", vismov);
      else
         printf ("[s]: ");
      
      fgets (reply, sizeof (reply), stdin);
      *(reply + strlen (reply) - 1) = '\0';
      if (*reply == '\0')
      {
         if (vismov > 0)
         {
            move = 16 * (vismov / 100) + 4 * ((vismov / 10 ) % 10) + 
               vismov % 10 - 21;
            vismov = 0;
            return;
         }
         else
         {
            move = 0;
            reply [0] = 's';
            reply [1] = '\0';
         }
      }
      else
         move = atoi (reply);
      if (move == 0)
      {
         switch (move = tolower (*reply))
         {
            case 'a':
               who [player] = 'm';
               move = 'a';
               return;
            
            case 'd':
               display ();
               move = 'd';
               break;
               
#ifdef HELP
            case 'h':
               help ();
               move = 'h';
               break;
#endif
               
            case 'r':
               move = 'r';
               return;
               
            case 'q':
               move = 'q';
               return;
               
            case 's':
               move = 's';
               return;
               
            default:
               (void) check_move (reply);
               break;
         }
      }
      else if (check_move (reply) == 0)
      {
         move =
            16 * (*reply - '1') + 4 * (*(reply + 1) - '1') + *(reply + 2) - '1';
         if (board [move] == 0)
            return;
         printf ("Cell %s already occupied. Please try again.\n", reply);
      }
   }
}

/************************************************************************/
/* Do the housekeeping... */

void update ()
{
   int i, j, k;
   int ic, jc, kc;   
   int score;
   int newsco;
   int ownsco;
   int abssco;
   char cell, line, plane;
   int temp;

   winner = -1;
   vismov = 0;
   if (win [0] == move)
      win [0] = -1;
   if (kkill [0] == move)
      kkill [0] = -1;
   if (win [0] < 0)
      win [0] = kkill [0];
   if (win [1] == move)
      win [1] = -1;
   if (kkill [1] == move)
      kkill [1] = -1;
   if (win [1] < 0)
      win [1] = kkill [1];

/* Update the "balance of power" scores for each plane passing
 * therough the newly occupied cell.
 */
   for (i = 6 * move, ic = 0; ic< 6; i++, ic++)
   {
      if ((plane = cplane [i]) < 0)
         break;
      plnsco [plane] += sign;
   }
   
/* Now consider all lines passing through that cell. */

   for (i = 7 * move, ic = 0; ic < 7; i++, ic++)
   {
      if ((line = clines [i]) < 0) /* End of line list */
         continue;
      score = linsco [line];

/* If the owner of the cell is not the owner of the line,
 * the line is now spoilt.
 */
      if ((ownsco = score * sign) < 0)
      {
         linsco [line] = 99;   /* Spoilt line */
         lincnt--;
         newsco = 99;
      }
      else

/* If line not spoilt, update the degree of its ownership. */

      {
         newsco = score + sign;
         linsco [line] = newsco;

/* If the ownership is 4, somebody has just won! */

         abssco = newsco * sign;
         if (abssco == 4)
            winner = player;
      }
      
/* Now loop through all cells in this line. */

      for (j = 4 * line, jc = 0; jc < 4; j++, jc++)
      {
         cell = lcells [j];

/* Do we have a winner? If so, we overwrite the existing cell owner value
 * with a special value wgich will display as '*'.
 */
         if (winner >= 0)
         {
            if (cell < 0) 
               cell = -(cell + 1);
            board [cell] = 3;
            continue;
         }

/* No winner. Nothing to do for occupied cells. */

         if (cell < 0)
            continue;

/* Is it the cell we have just moved into? If so, just note that it
 * is now occupied.
 */
         if (move == cell)
         {
            lcells [j] = -(cell + 1);
            continue;
         }
            
/* Cell still free: update histogram of line scores for this cell. */

         temp = score + 7 * cell + 3;
         clinh [temp] -= 1;

/* Ignore spoilt lines */

         if (newsco != 99)
         {
            clinh [temp - score + newsco] += 1;

/* Is it a forced line? */

            if (abssco == 3)
            {
               if (win [player] >= 0) 
                  kkill [player] = cell;
               else
                  win [player] = cell;
               continue;
            }
         }
         else
         {
            
/* Line spoilt. Mark it as such in list of lines passing through
 * this cell.
 */
            for (k = 7 * cell, kc = 0; kc < 7; k++, kc++)
            {
               if (line == clines [k])
               {
                  clines [k] = -1;
                  break;
               }
            }
         }
      }
      if (winner >= 0)
         return;
   }
   
/* End of complicated houskeeping. Now for the trivial stuff... */

   board [move] = sign;
   movcnt++;
   player = 1 - player;
   sign = -sign;
   return;
}

/************************************************************************/
/* Evaluate a move. */

void eval ()
{
   char plane;
   char cell;
   char line;
   char cplanh [7];
   int minus2;
   int minus1;
   int zero;
   int plus1;
   int plus2;
   int adjust;
   int temp;
   int i, j, k;
   int ic, jc, kc;
      
   long value;
   long svalue;
   long tvalue;
   long sbest = -1;

   strcnt = 0;
   
/* Loop through all empty cells. */

   for (i = 0; i < 64; i++)
   {
      tacval [i] = -100;
      if (board [i] != 0)
         continue;

/* Go through all planes passing through this cell and compile the
 * 'histogram' of plane ownerships.
 */
   
      memset (cplanh, '\0', sizeof (cplanh));
      for (j = 6 * i, jc = 0; jc < 6; j++, jc++)
      {
         if ((plane = cplane [j]) < 0)
            break;
         temp = sign * plnsco [plane] + 3; /* Empty plane is element 3 */
         if (temp < 0)                     /* Treat ownerships in excess ... */
            temp = 0;                      /* ... of 3, as 3 (plus or minus) */
         if (temp > 6)
            temp = 6;
         (cplanh [temp])++;
      }
   
/* For ease of manipulation, pull out into appropriately named
 * variables the various line counts for this cell.
 */
      temp = 7 * i + 1;
      if (sign < 0)
         temp += 4;
      minus2 = clinh [temp];
      temp += sign;
      minus1 = clinh [temp];
      temp += sign;
      zero   = clinh [temp];
      temp += sign;
      plus1  = clinh [temp];
      temp += sign;
      plus2  = clinh [temp];

/* Calculate preliminary base value... */

      value = 2 * (minus1 + 8 * (zero + 8 * (minus2 + 8 * plus1)));

/* If we have one forcing line passing through here, must be careful.
 * Work out if we need to beware of setting up a win for the
 * opponent. Loop through all lines passing through this cell. If
 * the line would be forced by the currently considered move, find
 * the other blank cell on that line. If that is on the opponent's 2
 * line, then for strategic purposes this cell is useless and we
 * might as well forget that there is a forcing line for this cell.
 */
      if (plus2 == 1)
      {
         for (j = 7 * i, jc = 0; jc < 7; j++, jc++)
         {
            if ((line = clines [j]) < 0)
               continue;
            if (sign * linsco [line] != 2)
               continue;
            for (k = 4 * line, kc = 0; kc < 4; k++, kc++)
            {
               if ((cell = lcells [k]) < 0)
                  continue;
               if (cell == i)
                  continue;
               if ((temp = clinh [ 7 * cell + 3 - 2 * sign]) == 0)
                  continue;
               if (temp > 0)
                  goto no_good;
               plus2 = 0;
               break;
            }
            break;
         }
      }

/* A few ad hoc heuristics follow... */

      adjust = 0;
      if (player == 1 && mute <= 0 && zero + 2 * cplanh [5] + cplanh [0] > 4)
         cplanh [0] += minus2;
      else
      {
         temp = minus1 - plus1;
         if (temp > 0 && lincnt != 74)
         {
            adjust = temp;
            (cplanh [4])--;
         }
      }
   
      if (cplanh [0] == 1 && minus1 + 2 * minus2 < 3)
         cplanh [0] = 0;
      
/* Finish off calculating the base value. */

      value += 8198L * (cplanh [6] + 8 * cplanh [0]);

/* Unless exploring strategies, calculate the tactical value. */

      if (mute <= 0)
      {
         tvalue = value + 1020L * adjust;
         if (minus2 > 2)
            tvalue += 262144L;
         tacval [i] = tvalue;
      }
   
/* Calculate the strategic value of the cell. */

      if (plus2 == 0)
         continue;

      svalue = value + 128L * (cplanh [5] + 512L * (plus1 + 8 * plus2));

      if (sbest < svalue)  /* Is this a better strategic move? */
      {
         strcnt = 1;
         strmov [0] = i;
         sbest = svalue;
      }
      else if (sbest == svalue) /* No, but it is just as good! */
         strmov [strcnt++] = i;
no_good:
      continue;
   }
}

/************************************************************************/
/* Back up the game info */

void backup (bckup)
char *bckup;
{
   char *bck;
   
   bck = bckup;

   memcpy (bck, (char *) board, sizeof (board));
   bck += sizeof (board);
   memcpy (bck, (char *) cplane, sizeof (cplane));
   bck += sizeof (cplane);
   memcpy (bck, (char *) linsco, sizeof (linsco));
   bck += sizeof (linsco);
   memcpy (bck, (char *) lcells, sizeof (lcells));
   bck += sizeof (lcells);
   memcpy (bck, (char *) clines, sizeof (clines));
   bck += sizeof (clines);
   memcpy (bck, (char *) clinh, sizeof (clinh));
   bck += sizeof (clinh);
   memcpy (bck, (char *) plnsco, sizeof (plnsco));
   bck += sizeof (plnsco);
   memcpy (bck, &player, sizeof (player));
   bck += sizeof (player);
   memcpy (bck, &sign, sizeof (sign));
   bck += sizeof (sign);
   memcpy (bck, &winner, sizeof (winner));
   bck += sizeof (winner);
   memcpy (bck, (char *) win, sizeof (win));
   bck += sizeof (win);
   memcpy (bck, (char *) kkill, sizeof (kkill));
   bck += sizeof (kkill);
   memcpy (bck, &move, sizeof (move));
   bck += sizeof (move);
   memcpy (bck, &movcnt, sizeof (movcnt));
   bck += sizeof (movcnt);
   memcpy (bck, &strcnt, sizeof (strcnt));
   bck += sizeof (strcnt);
   memcpy (bck, &lincnt, sizeof (lincnt));
   bck += sizeof (lincnt);
   memcpy (bck, (char *) strmov, sizeof (strmov));
   bck += sizeof (strmov);
   memcpy (bck, (char *) tacval, sizeof (tacval));
   bck += sizeof (tacval);
}

/************************************************************************/
/* Restore game info */

void restore (bckup)
char *bckup;
{
   char *bck = bckup;
   
   memcpy ((char *) board, bck, sizeof (board));
   bck += sizeof (board);
   memcpy ((char *) cplane, bck, sizeof (cplane));
   bck += sizeof (cplane);
   memcpy ((char *) linsco, bck, sizeof (linsco));
   bck += sizeof (linsco);
   memcpy ((char *) lcells, bck, sizeof (lcells));
   bck += sizeof (lcells);
   memcpy ((char *) clines, bck, sizeof (clines));
   bck += sizeof (clines);
   memcpy ((char *) clinh, bck, sizeof (clinh));
   bck += sizeof (clinh);
   memcpy ((char *) plnsco, bck, sizeof (plnsco));
   bck += sizeof (plnsco);
   memcpy (&player, bck, sizeof (player));
   bck += sizeof (player);
   memcpy (&sign, bck, sizeof (sign));
   bck += sizeof (sign);
   memcpy (&winner, bck, sizeof (winner));
   bck += sizeof (winner);
   memcpy ((char *) win, bck, sizeof (win));
   bck += sizeof (win);
   memcpy ((char *) kkill, bck, sizeof (kkill));
   bck += sizeof (kkill);
   memcpy (&move, bck, sizeof (move));
   bck += sizeof (move);
   memcpy (&movcnt, bck, sizeof (movcnt));
   bck += sizeof (movcnt);
   memcpy (&strcnt, bck, sizeof (strcnt));
   bck += sizeof (strcnt);
   memcpy (&lincnt, bck, sizeof (lincnt));
   bck += sizeof (lincnt);
   memcpy ((char *) strmov, bck, sizeof (strmov));
   bck += sizeof (strmov);
   memcpy ((char *) tacval, bck, sizeof (tacval));
   bck += sizeof (tacval);
}

/************************************************************************/
/* Prune the forcing sequence */

void prune ()
{
   int tmppnt;

   for (frcpnt = 0; frcpnt < frccnt; frcpnt++)
   {
      restore (bckup1);
      frcmov [frcpnt] = -(frcmov [frcpnt] + 1);
      for (tmppnt = 0; tmppnt < frccnt; tmppnt++)
      {
         if ((move = frcmov [tmppnt]) < 0)
            continue;
         update ();
         if ((move = win [1 - player]) < 0)
            break;
         update ();
         if (win [player] >= 0) 
            goto skip;
         if (win [1 - player] >= 0)
            break;
      }
      frcmov [frcpnt] = -(frcmov [frcpnt] + 1);
skip:
      continue;
   }
   frcpnt = 0;
   for (tmppnt = 0; tmppnt < frccnt; tmppnt++)
   {
      if (frcmov [tmppnt] >= 0)
         frcmov [frcpnt++] = frcmov [tmppnt];
   }
   frccnt = frcpnt + 1;
   restore (bckup1);
}

/************************************************************************/
/* Make a strategic move selection. */

void strsel ()
{
   int temp;
   
   frccnt = 0;
   move = -1;
   if (strcnt == 0)
      return;
   backup (bckup1);
   while (strcnt)
   {
      move = strmov [rand () % strcnt];
      frcmov [frccnt++] = move;
      update ();
      if (win [player] >= 0)
         goto done;
      move = win [1 - player];
      update ();
      if (winner >= 0)
         break;
      if ((move = win [player]) >= 0)
         goto force;
      eval ();
   }
   restore (bckup1);
   move = -1;
   frccnt = 0;
   return;
   
force:
   frcmov [frccnt++] = move;
   
done:
   restore (bckup1);
   if (mute != 2)
      prune ();
   frcpnt = 1;
   move = frcmov [0];
   return;
}

/************************************************************************/
/* Make a tactical move selection. */

void tacsel ()
{
   int index [64];
   int tacmov [64];
   int temp;
   int taccnt = 0;
   int i, j;
   
   long adjval;
   long tbest = -100;
   long crtval;
   
/* Note that it is dangerous to resort to tactical forcing if you
 * are in a bad position. Hence if our top choices got rejected by
 * the tactical look-ahead, we artificially devalue forcing moves.
 */
   backup (bckup2);
   if (mute >= 0)
      mute = 2;
   sort_it (tacval, index, 64);
   //crtval = 0.95 * tacval [index [0]];
   crtval = (95 * tacval [index [0]])/100;

   for (i = 0; i < 64; i++)
   {
      move = index [i];
      adjval = tacval [move];
      if (adjval < tbest)
         goto done;
      temp = -1;
      update ();
      if (win [1 - player] >= 0)
      {
         if (adjval < 200 || adjval < crtval) 
            adjval /= 128;
      }
      else
      {
         eval ();
         strsel ();
         temp = move; /* Move is about to be overwritten! */
      }
      restore (bckup2);
      move = index [i];
      if (temp > 0)
         adjval = frccnt;
      frccnt = 0;
      
      if (adjval < tbest)
         continue;
      if (adjval > tbest)
      {
         taccnt = 1;
         tacmov [0] = move;
         tbest = adjval;
      }
      else 
         tacmov [taccnt++] = move;
   }
   
done:
   if (mute >= 0)
      mute = 1;
   move = tacmov [rand () % taccnt];
}

/************************************************************************/
/* Select a move. */

void select_move ()
{
   int plane;
   int line;
   int cell;
   int forced = 0;
   
   move = -1;              /* No move selected so far */
   if (movcnt <= 4)
      opening ();          /* Special move selection in the opening */
   else if (win [0] >= 0 || win [1] >= 0)
   {
      if (win [player] >= 0)
         move = win [player];
      else
         move = win [1 - player];
      forced = 1;
   }
   else if (frccnt == 0)
   {
      eval ();             /* Evaluate free cells */
      if (mute == 0)
         mute = 1;         /* Suppress display */
      strsel ();           /* Make strategic selection */
      if (move == -1)      /* Got something strategic? */
         tacsel ();        /* No... Select tactically */
   }
   else
      move = frcmov [frcpnt++];  /* Forced move */
      
/* Decompose the selected move and (just in case!) check its legality. */

   cell = move % 4 + 1;
   line = (move / 4) % 4 + 1;
   plane = move / 16 + 1;
   if (mute > 0)
      mute = 0;
   vismov = 100 * plane + 10 * line + cell;
   if (move < 0 || move >= 64 || board [move] != 0)
   {
      printf ("OX3D logic error!\n");
      if (move < 0 || move >= 64)
         printf ("%d is not a legal move!\n", vismov);
      else
         printf ("Selected cell %d is already occupied!\n", vismov);
      exit (1);
   }

/* If a forced win found, announce it! */

   if (frccnt > 0 && frcpnt == 1 && forced == 0)
   {
      if (who [0] == 'm' && who [1] == 'm' && player == 0)
         putchar ('\n');
      if (mute >= 0)
         printf ("\n**** Forced win for %s in %d moves ****\n\n", 
            plname [player], frccnt);
      else
         printf ("\n=== A forced win available to %s ===\n\n", plname [player]);
   }

/* State the chosen move. */

   if (mute < 0)
      printf ("Suggested");
   else if (who [0] == who [1])
      printf ("%s'", plname [player]);
   else
      printf ("My");
   printf (" move is %d", vismov);
   if (kkill [player] >= 0)
      puts ("  -   Mate!");
   else if (frccnt > 0)
         puts (" ... Check!");
   else
      putchar ('\n');
}

/************************************************************************/
/* Main module */

main ()
{
   int reply;
   
   printf ("OX3D - MLA version 3.1, 01 Feb 87\n\n");
   reply = get_reply ("Do you need instructions? [Y]: ", "ynq", 'y');
   if (reply == 'q')
      exit (0);
   if (reply == 'y')
      intro ();
   putchar ('\n');
      
   rwho [0] = 0;
   rwho [1] = 0;
   
   while (1)
   {
      init ();
      while (1)
      {
         if (who [player] == 'h')
         {
            get_move ();
            if (move == 's')
            {
               backup (bckup3);
               mute = -1;
               select_move  ();
               mute = 0;
               restore (bckup3);
               if (frccnt > 0)
               {
                  frccnt = 0;
                  memset (frcmov, '\0', sizeof (frcmov));
               }
               winner = -1;
               continue;
            }
            if (move == 'q' || move == 'r') 
               break;
            if (move == 'a' || move == 'h' || move == 'd')
               continue;
         }
         else
         {
            if (player == 0 && who [0] == 'm' && who [1] == 'm' && movcnt > 0)
            {
               char buf [6];
               printf ("More? Y/N/R/Q [Y]: ");
               fgets (buf, sizeof (buf), stdin);
               if ((move = tolower (*buf)) == 'q' || move == 'r')
                  break;
            }      
            select_move ();
            vismov = -1;
         }
         update ();
         if (winner >= 0)
            break;
         display ();
         if (lincnt == 0)  /* No lines left -- a draw */
            break;
         if (move < 0)
            break;
      }
      if (winner >= 0)
      {
         display ();
         if (who [0] == who [1])
            printf ("%s wins.\n", plname [winner]);
         else if (who [winner] == 'm')
            printf ("I win. Hard luck...\n");
         else
            printf ("You win. Good game!\n");
      }
      else if (lincnt == 0)
         printf ("No lines left - a draw.\n");
      else
      {
         if (who [player] != 'm')
         {
            if (frccnt == 0)
               printf ("Well, all right...\n");
            else 
               printf ("Quitter! I've got you mated, so I win anyway.\n");
         }
      }
      if (move == 'q')
         exit (0);
      if (move == 'r')
         continue;
      if (get_reply ("\nAnother game? [Y]: ", "ynq", 'y') != 'y')
         break;
      puts ("Good! Let's have another game...\n");
   }
   printf ("Well, next time perhaps...\n");
}
