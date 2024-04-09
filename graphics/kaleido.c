
/* kaleido.c  -*- C -*-
 * 
 * To compile:
 * zcc +zx -create-app -O3 -lm -DGFXSCALEX=2/5 -DGFXSCALEY=2/5 -llib3d kaleido.c
 * zcc +ts2068 -create-app -O3 -lm -DGFXSCALEX=4/5 -DGFXSCALEY=2/5 -llib3d -pragma-define:CLIB_DEFAULT_SCREEN_MODE=6 kaleido.c
 * 
 * By Guido Gonzato, December 2018
 * z88dk conversion by Stefano Bodrato, 9/4/2024
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

#include <bgi.h>
#include <stdlib.h>
#include <time.h>

int xc, yc;

void rnd_circles (int x, int y, int r)
{
  fillellipse (xc + x, yc + y, r, r);
  fillellipse (xc - x, yc + y, r, r);
  fillellipse (xc - x, yc - y, r, r);
  fillellipse (xc + x, yc - y, r, r);
  fillellipse (xc + y, yc + x, r, r);
  fillellipse (xc - y, yc + x, r, r);
  fillellipse (xc - y, yc - x, r, r);
  fillellipse (xc + y, yc - x, r, r);
}

void rnd_bars (int x, int y, int r)
{
  bar (xc + x - r, yc + y - r, xc + x + r, yc + y + r);
  bar (xc - x - r, yc + y - r, xc - x + r, yc + y + r);
  bar (xc - x - r, yc - y - r, xc - x + r, yc - y + r);
  bar (xc + x - r, yc - y - r, xc + x + r, yc - y + r);
  bar (xc + y - r, yc + x - r, xc + y + r, yc + x + r);
  bar (xc - y - r, yc + x - r, xc - y + r, yc + x + r);
  bar (xc - y - r, yc - x - r, xc - y + r, yc - x + r);
  bar (xc + y - r, yc - x - r, xc + y + r, yc - x + r);
}

// -----

int main (int argc, char *argv[])
{
  int
    x, y, r,
    n,
    stop = 0;
  
  srand (clock());

  clg();

  xc=320;
  yc=240;
  
  n = 0;
  
  while (!stop) {
    
    // define random position and radius of a circle
	
    r = 5 + random (25);
    x = random (xc-(r*3))+3;
    y = random (yc-(r*2))+3;

	bgi_fillstyle=random (7);

    // draw 8 filled circles
    rnd_circles (x, y, r);
    
    // define a random box
    r = 5 + random (10);
    x = random (xc-(r*3))+3;
    y = random (yc-(r*2))+3;

	bgi_fillstyle=random (7);

    // draw 8 boxes
    rnd_bars (x, y, r);
    
    
    // every 20 frames, fade the graphics
    if (20 == n++) {
		clg();
		if (getk()==' ')
			stop = 1;
		n = 0;
    }
  }
  
  closegraph ();
  
}

// ----- end of file kaleido.c
