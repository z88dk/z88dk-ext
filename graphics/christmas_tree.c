
// Christmas tree, probably originally written
// for the TRS-80 High Resolution mod

#include <graphics.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int x,y;
float xi;
long x1,n;
int xc,yc;

float i;

void main()
{
  xc=getmaxx()/2;
  xi=getmaxy()/2.8;
  yc=getmaxy()/6;
  
  clg();
  
  for (i=0; i<xi; i+=0.15) {
	  x=xc+i*cos(i);
	  y=i*2.1;
	  plot(x,yc+y);
  }
  plot(xc+5,yc-6);
  drawr(-10,0); drawr(8,6); drawr(-3,-9);
  drawr(-3,9); drawr(8,-6);
}
