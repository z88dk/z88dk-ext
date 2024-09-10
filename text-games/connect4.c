/* 4inarow 2007-Oct-25
 * by Joseph Larson (c) 2008
 * based on a BASIC game by James L. Murphy
 * as found in 'More BASIC Computer Games' by David H. Ahl (c) 1979
 */


// zcc +zx -lndos -DUSE_UDGS -create-app row4.c


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

//#define SHOWBD for (c=9; c >= 0; c--) puts (bd[c])


#ifdef USE_UDGS

#include <sys/ioctl.h>

#define _X 128
#define _O 129

static unsigned char udgs[] = {
    0b00111100, // X
    0b01000010,
    0b10000001,
    0b10000001,
    0b10000001,
    0b01000010,
    0b00111100,
    0b00000000,

//    0b10000001,	// X
//    0b01000010,
//    0b00100100,
//    0b00011000,
//    0b00100100,
//    0b01000010,
//    0b10000001,
//    0b00000000,

    0b00111100, // O
    0b01111110,
    0b11111111,
    0b11111111,
    0b11111111,
    0b01111110,
    0b00111100,
    0b00000000
};

#else

#define _X 'X'
#define _O 'O'

#endif

unsigned long v[16] = {1, 75, 500, 1e9, 1, 800, 4000, 1e9
                      ,1,100, 900, 1e7, 1, 450, 3000, 1e7};
int inrow[4];
int open[4];
int h[8];
char bd[9][20];


void showbd() {
	int c;
	for (c=9; c >= 0; c--) puts (bd[c]);
}

void intro (void) {
  putchar(12);
  puts ("Four in a Row\n---- -- - ---\n"
  "Stack X's and O's in order to\n"
  "make 4 in a row either\n"
  "vertically, horizontally or\n"
  "diagonally before your\n"
  "opponent does.\n");
}

void init (void) {
  int c;

  for (c = 0; c < 8;) {
    h[c] = 0;
    strcpy (bd[++c], "- - - - - - - - ");
  }
  strcpy (bd[0], "1 2 3 4 5 6 7 8 ");
}

int count (int x, int y, char token) {
  int w, k, dx, dy, cx, cy, c, t;
  char op;

  x *= 2; op = (token != _X) ? _X : _O;
  for (c = 0; c < 4; c++) {
    inrow[c] = 1; open[c] = 0;
    dx = 2 * (c - 1 - (c > 2));
    dy = (c != 3);
    for (w = 0; w < 2; w++) {
      t = 1;
      for (k = 1; k < 4 && bd [cy = y + dy * k][cx = x + dx * k] != op; k++)
        if (cx <= 15 && cx >= 0 && cy <= 8 && cy > 0) {
          if (t && bd[cy][cx] == token) inrow[c]++;
          else {open[c]++; t = 0;}
        }
      dx = -dx; dy = -dy;
    }
    if (inrow[c] > 3) return 1;
  }
  k = 0;
  for (c = 0; c < 8; c++) if (h[c] < 8) k++;
  if (!k) return 2;
  return 0;
}

int domove (int m, char token) {
  bd [++h[m]][2 * m] = token;
  return count (m, h[m], token);
}

int getmove (int pl) {
  int input = 0;

  do {
    if (input) puts ("Illegal move, try again.");
    printf ("Player %d, pick a column (1 - 8) ? ", pl); scanf ("%d", &input);
    if (input < 1 || input > 8 || h[input - 1] > 7) input = -1;
  } while (input < 0);
  return --input;
}

unsigned long rank, bestrank;
int bestmove, numsame;

int compmove (void) {
  int w, x, y, c, n[4];
  char token;

  bestmove = 0;
  bestrank = 0;
  numsame = 1;

  for (x = 0; x < 8; x++) {
    y = h[x] + 1;
    if (y < 9) {
      rank = 1; token = _O;
      for (w = 0; w < 2; w++) {
        if (count (x, y, token)) {
          printf ("Computer picks column %d\n", x + 1); return x;
        }
        for (c = 0; c < 4; c++) n[c] = 0;
        for (c = 0; c < 4; c++) {
          open[c] += inrow[c];
          if (open[c] > 3) {rank += 4; n[inrow[c] - 1]++;}
        }
        for (c = 0; c < 4; c++) if (n[c]--)
          rank += v[8 * w + 4 * (n[c] ? 1 : 0) + c] + n[c] * v[8 * w + c];
        token = _X;
      }
      if (y < 8) if (count(x, y + 1, token)) rank = 2;
      if (rank == bestrank) if (rand() < RAND_MAX / ++numsame) {
        bestrank = rank; bestmove = x;
      }
    }
    if (rank > bestrank) {bestrank = rank; bestmove = x; numsame = 1;}
  }
  printf ("Computer picks column %d\n", bestmove + 1);
  return bestmove;
}

int main (void) {
  int numpl, w = 0;


#ifdef USE_UDGS
  void *param = &udgs;
  console_ioctl(IOCTL_GENCON_SET_UDGS, &param);

#ifdef __SPECTRUM
  putchar(1);
  putchar(32);
#endif

#endif

  intro ();
//  srand (time (NULL));
  init ();
  printf ("One or two human players? (1/2) "); scanf ("%d", &numpl);
  while (numpl > 2 || numpl < 1) {
    printf ("Please type the number 1 or 2 ? "); scanf ("%d", &numpl);
  }
  srand (clock());
  if (!--numpl) puts ("The Computer will be Player 2.");
  if (rand () % 2) {
    puts ("Player 1 goes first.");
    showbd();
    domove (getmove (1), _X);
  } else puts ("Player 2 goes first.");
  while (!w) {
    showbd();
    if (!(w = domove ((numpl) ? getmove (2) : compmove (), _O))) {
      showbd();
      w = domove (getmove (1), _X);
    } else if (w == 1) w = 3;
  }
  showbd();
  switch (w) {
    case 1 : puts ("Player 1 wins!\n\n"); break;
    case 2 : puts ("Tie game.\n\n"); break;
    case 3 : puts ("Player 2 wins!\n\n");
  }
  exit (0);
}
