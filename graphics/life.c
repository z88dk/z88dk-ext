/* life.c  -*- C -*-
 * 
 * Simple cellular automaton, as described at
 * http://mathworld.wolfram.com/ElementaryCellularAutomaton.html
 * 
 * This version can be compiled with z88dk
 * (set SIZE to the max. Y screen size or smaller)
 * If the target system provides a shell, you can pass the
 * percentage (density) as a command argument.
 * At runtime, keep "R" pressed to restart from a randomly built pattern
 * keep SPACE pressed to exit.
 * 
 * By Guido Gonzato, May 2022. - Stefano Bodrato, Apr 2024
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <time.h>

#include <graphics.h>

#define random(a)  rand()%a

#define SIZE  40

#define ALIVE 1
#define DEAD  0

char a[SIZE][SIZE], b[SIZE][SIZE];

void print_cells (char[SIZE][SIZE]);
void evolve_cells (char[SIZE][SIZE], char[SIZE][SIZE]);
void initialise (int);

  int x, y, n;

// -----

void initialise (int percentage)
{
  for (x = 0; x < SIZE; x++)
    for (y = 0; y < SIZE; y++)
      a[x][y] = (random (100) < percentage) ? ALIVE: DEAD;
}

// -----

void main_loop (void)
{
  unsigned long iterations = 0;
  int stop = 0;
  int key=0;
  
  while (! stop) {
    evolve_cells (a, b);
    print_cells (b);
    //refresh ();
    evolve_cells (b, a);
    print_cells (a);
    //refresh ();
    if (++iterations % 1000)
		
	key=getk();

	if (key == " ")
	  stop = 1;

	if ((key == "r") || (key == "R"))
	  initialise (random (100));

  }

} // --- main_loop ()

// -----

int main (int argc, char *argv[])
{
  unsigned int percentage;
  
  srand (clock());
  clg();

  if (2 == argc)
    percentage = atoi (argv [1]); // no checks!
  else
    percentage = 10;

//  printf ("Press SPACE to exit, R to reset");

//  showinfobox ("Press any key to restart,\n"
//               "or ESC to exit.");
  
  initialise (percentage);
  main_loop ();
  
  //closegraph ();
  
} // main() ()

// -----

#define NORTH(n) (n == 0) ? (SIZE - 1) : (n - 1)
#define SOUTH(n) (n == SIZE - 1) ? (0) : (n + 1)
#define WEST(n)  (n == 0) ? (SIZE - 1) : (n - 1)
#define EAST(n)  (n == SIZE - 1) ? (0) : (n + 1)

void print_cells (char cells[SIZE][SIZE])
{
  int x, y, color;
  
  for (x = 0; x < SIZE; x++)
    for (y = 0; y < SIZE; y++) {
      if (DEAD == cells[x][y])
		  unplot (x, y);
	  else
		  plot (x, y);
    }
} // print_cells ()

// -----

void evolve_cells (char old[SIZE][SIZE], char new[SIZE][SIZE])
{
  // evolve cells in array new[][] based on the state of array old[][]
  
  for (x = 0; x < SIZE; x++)
    for (y = 0; y < SIZE; y++) {
      // count alive neighbours of (x, y) cell, 
      // starting from N, clockwise
      n = 0;
      if (ALIVE == old[x][NORTH(y)])         // N
	n++;
      if (ALIVE == old[EAST(x)][NORTH(y)])   // NE
	n++;
      if (ALIVE == old[EAST(x)][y])          // E
	n++;
      if (ALIVE == old[EAST(x)][SOUTH(y)])   // SE
	n++;
      if (ALIVE == old[x][SOUTH(y)])         // S
	n++;
      if (ALIVE == old[WEST(x)][SOUTH(y)])   // SW
	n++;
      if (ALIVE == old[WEST(x)][y])          // W
	n++;
      if (ALIVE == old[WEST(x)][NORTH(y)])   // NW
	n++;
      if (n < 2 || n > 3)
        new[x][y] = DEAD;
      else
        if (3 == n)
          new[x][y] = ALIVE;
        else
          new[x][y] = old[x][y];
    }
  
} // evolve_cells ()

// ----- end of file life.c
