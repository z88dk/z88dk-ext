
//  This game was a 2008 Cymon's Games game
//  adapted to z88dk by Stefano Bodrato, 2024

//  zcc +zx -lndos -create-app -zorg=24576 battleship.c
//  zcc +zx81 -subtype=fast -create-app -DTHICK battleship.c
//  zcc +cpm -create-app battleship.c


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <math.h>

#define PL 0
#define CP 1

//#define FULL(x) ((x + (x < 2)) + 1)
//#define ADD(a,b) if (!bd[a][b].chit) {t[i].x = a; t[i].y = b; i++;}

typedef struct {
  unsigned int x : 4;
  unsigned int y : 4;
} COORD;

typedef struct {
  unsigned int pship : 3;
  unsigned int chit : 1;
  unsigned int cship : 3;
  unsigned int phit :3;
} GRID;

GRID bd[10][10];
COORD t[51];
char ship_life[2][5];

#ifdef THICK
char *ship_name[5] = {"pt ship", "submar.", "cruser", "b.ship", "carrier"};
#else
char *ship_name[5] = {"pt ship", "submarine", "cruser", "battleship", "carrier"};
#endif

int full(int x) {
	if (x < 2) return (x+2);
	return (x+1);
}

int i;

void add(int a, int b) {
	if (bd[a][b].chit) return (0);
	t[i].x = a;
	t[i].y = b;
	i++;
}

int in_range (int x) {
	return (isdigit(x+'0'));
}

char input[10];

void getloc (COORD* loc) {

  loc->x = 0;
  loc->y = 0;
  input[0] = 0;
  
  do {
    if (input[0]) printf ("Invalid location, letter first then number : ");
    scanf ("%s", input);
    if (isalpha (input[0]) && (loc->x = atoi (&input[1]))) {
      loc->y = tolower (input[0]) - 'a';
      if ((loc->y > 9) || (loc->x > 10) || (loc->x < 0)) loc->x = 0;
    }
  } while (!loc->x);
  
  loc->x --;
}

void show_board (void) {
  int x, y;

#ifdef THICK
  printf ("%8s     %8s\n_12345678910 _12345678910"
    ,"RADAR","FLEET");

  for (y = 0; y < 10; y++) {
    printf ("\n%c", y + 'a');
    for (x = 0; x < 10; x++)
      printf ("%c", (bd[x][y].phit) ? (bd[x][y].cship) ? 'X' : 'o' : '.');

    printf ("  %c", y + 'a');
    for (x = 0; x < 10; x++)
      printf ("%c", (bd[x][y].chit) ? (bd[x][y].pship) ? 'X'
      : 'o' : ".12345"[bd[x][y].pship]);
  }

  for (y = 4; y >= 0; y--) {
    printf ("\n%7s:", ship_name[y]);
	//printf ("\n %10s [%d]: ", ship_name[y], ship_life[CP][y]);   // debuging/cheating
    if (ship_life[CP][y]) for(x = 0; x < full(y); x++) putchar ('#');
    else {printf ("SUNK"); x=4;};
	for(;x<8;x++) putchar (' ');
    printf ("%7s:", ship_name[y]);
    for (x = 0; x < full(y); x++) putchar (".#"[ship_life[PL][y] > x]);
  }

#else

  printf ("%16s         %16s\n_ 1 2 3 4 5 6 7 8 9 10   _ 1 2 3 4 5 6 7 8 9 10"
    ,"R A D A R","F L E E T");

  for (y = 0; y < 10; y++) {
    printf ("\n%c ", y + 'a');
    for (x = 0; x < 10; x++)
      printf ("%c ", (bd[x][y].phit) ? (bd[x][y].cship) ? 'X' : 'o' : '.');

    printf ("   %c ", y + 'a');
    for (x = 0; x < 10; x++)
      printf ("%c ", (bd[x][y].chit) ? (bd[x][y].pship) ? 'X'
      : 'o' : ".12345"[bd[x][y].pship]);
  }

  for (y = 4; y >= 0; y--) {
    printf ("\n %10s : ", ship_name[y]);
	//printf ("\n %10s [%d]: ", ship_name[y], ship_life[CP][y]);   // debuging/cheating
    if (ship_life[CP][y]) for(x = 0; x < full(y); x++) putchar ('#');
    else {printf ("SUNK"); x=3;};
	for(;x<12;x++) putchar (' ');
    printf ("%10s : ", ship_name[y]);
    for (x = 0; x < full(y); x++) putchar (".#"[ship_life[PL][y] > x]);
  }
#endif
}

COORD start, end;
int xx,yy;

int valad_ship (int c) {
  int check, d, v;

  check = abs ((start.x + 10 * start.y) - (end.x + 10 * end.y));
  //printf ("- check: %d, full: %d - ",check, full(c));
  if (check % (full(c) - 1)) {
    printf ("\nInvalid location. The %s is only %d long\n"
            "and ships can only be placed vertical or horizontal.\n",
            ship_name[c], full(c));
    v = 0;
  } else {
    xx = 0;
	yy = 0;
    if ((check / (full(c) - 1)) - 1) yy = 1;
    else xx = 1;
    if (start.x > end.x) start.x = end.x;
    if (start.y > end.y) start.y = end.y;
    for (d = 0; d < full(c) && v; d++) {
      check = bd[start.x + d * xx][start.y + d * yy].pship;
      if (check && (check != 7)) {
        printf ("\nInvalid location. Ships can not overlap.\n");
        v = 0;
      }
    }
  }
  if (v) for (d = 0; d < full(c); d++)
    bd[start.x + d * xx][start.y + d * yy].pship = c + 1;
  return v;
}

void player_setup (void) {
  int ship;
  for (ship = 4; ship >= 0; ship--)
    do {
      putchar('\n');
      show_board ();
      printf ("\nEnter start location for your %s : ", ship_name[ship]);
      getloc(&start);
      printf ("Enter end location (length %d) : ", full(ship));
      getloc(&end);
    } while (!valad_ship (ship));
  show_board ();
}

int cc,dd;
int sx,sy;

void auto_setup (int pl) {

  for (cc = 0; cc < 5; cc++) {
    do {
      sx = rand() % 10;
	  sy = rand() % 10;
      xx = 0;
	  yy = 0;
      if (rand() < RAND_MAX / 2) {
        xx = 1;
        if (sx + full(cc) > 10) sx -= full(cc);
      } else {
        yy = 1;
        if (sy + full(cc) > 10) sy -= full(cc);
      }
      for (dd = 0; (dd < full(cc)) &&
      ((pl) ? !bd[sx + dd * xx][sy + dd * yy].cship
      : !bd[sx + dd * xx][sy + dd * yy].pship)  ; dd++);
    } while (dd < full(cc));
    for (dd = 0; dd < full(cc); dd++)
      if (pl)
        bd[sx + dd * xx][sy + dd * yy].cship = cc + 1;
      else bd[sx + dd * xx][sy + dd * yy].pship = cc + 1;
  }
}

char cinput;

void init (void) {

#ifdef THICK
#ifdef SPECTRUM
  putchar (1);    // 32 columns
  putchar (32);
#endif
#endif
  putchar (12);    // CLS
  

  
  for (cc = 0; cc < 10; cc++)
    for (dd = 0; dd < 10; dd++) {
      bd[cc][dd].pship = 0;
	  bd[cc][dd].chit = 0;
	  bd[cc][dd].cship = 0;
	  bd[cc][dd].phit = 0;
	}
  for (cc = 0; cc < 5; cc++) {
    ship_life[PL][cc] = full(cc);
	ship_life[CP][cc] = full(cc);
  }
  printf ("Battleship (R)\n\nDo you want (A)uto or (M)anual setup ? (a/m) ");
  while (!isalpha (cinput = getchar()));
  srand (clock());
  if (tolower (cinput) == 'm')
    player_setup ();
  else auto_setup (PL);
  auto_setup (CP);
}

int check_for_lose (int player) {
  int c;

  for (c = 0; (c < 5) && !ship_life[player][c]; c++);
  return (c == 5);
}

void player_turn (void) {
  COORD shot;
  int ship;

  show_board ();
  printf ("\n\nYour shot coordinates : ");
  getloc (&shot);
  if (bd[shot.x][shot.y].phit)
    printf ("A wasted shot! You already fired there!\n");
  else {
    bd[shot.x][shot.y].phit = 1;
    ship = bd[shot.x][shot.y].cship;
    if (ship) {
      printf ("HIT!\n");
      if (!(--ship_life[CP][--ship]))
        printf ("You sunk my %s.\n",ship_name[ship]);
    } else printf ("Miss.\n");
  }
}

int hit_no_sink (int x, int y) {
  if (bd[x][y].chit) {
    if (bd[x][y].pship == 7) {
      return 1;
    } else if ((bd[x][y].pship) && (ship_life[PL][bd[x][y].pship - 1]))
      return 1;
  }
  return 0;
}

int m[5] = {0, 1, 0, -1, 0};
int cx,cy,dx,dy;

int fill_t (void) {
  int x = 0;
  i = 0;

  for (cx = 0; cx < 10; cx++)
    for (cy = 0; cy < 10; cy++)
      if (hit_no_sink (cx,cy)) {
        for (x = 0; x < 4; x++)
          if ( in_range (cx + m[x]) && in_range (cy + m[x + 1]) ) {
            if (hit_no_sink (cx + m[x], cy + m[x + 1])) {
              dx = cx; dy = cy;
              while (in_range (dx) && in_range (dy)
                && hit_no_sink (dx, dy)) {dx -= m[x]; dy -= m[x + 1];}
              if (in_range (dx) && in_range (dy))  add (dx, dy);
            }
          }
        if (!i)
          for (x = 0; x < 4; x++)
            if ( in_range (cx) && in_range (cy + m[x + 1]) )
              add (cx + m[x], cy + m[x + 1]);
      }
  if (!i)
    for (cx = 0; cx < 10; cx++)
      for (cy = 0; cy < 10; cy++)
        if ((cx + cy) % 2) add (cx, cy);
  return i;
}

void compy_turn (void) {
  int z, c;

  c = fill_t ();
  z = rand () % c;
  printf ("\nMy shot : %c%d\n", t[z].y + 'a', t[z].x + 1);
  bd[t[z].x][t[z].y].chit = 1;
  c = bd[t[z].x][t[z].y].pship;
  if (c) {
    printf ("HIT!\n");
    if (!(--ship_life[PL][c - 1]))
      printf ("I sunk your %s.\n", ship_name[c - 1]);
  } else printf ("Miss.\n");
}

void play (void) {
  int winner = 0;

  if (rand () < RAND_MAX / 2) {
    printf ("\nYou go first.\n");
    player_turn ();
  } else printf ("\nI'll go first.\n");
  do {
    compy_turn ();
    if (check_for_lose (PL)) {
      winner = 1;
      printf ("\nI win!\n");
    } else {
      player_turn ();
      if (check_for_lose (CP)) {
        winner = 1;
        printf ("\nYou win!\n");
      }
    }
  } while (!winner);
  show_board ();
}


int main (void) {
  
  init ();
  play ();
  
}
