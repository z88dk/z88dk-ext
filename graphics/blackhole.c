// **Black Hole** by Dusty W. Murray
// BASIC conversions by Steven Reid 2023
// z88dk port by Stefano Bodrato, 2025

// zcc +<target> -lndos -create-app -lm blackhole.c

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <lib3d.h>
#include <graphics.h>
 
#define ITERATIONS 10

float pi2=M_PI*2.0;

float w,h;
int  mw,mh,mr;

float x,y;
int i,t;
int ch;

float ads;
float ad;
float rds;

float r;
float rd;
float rs;
float a;

float ar;



int main() {
	
  //srand (clock());
  
  w = getmaxx();
  h = getmaxy();
  mw = w/2;
  mh = h/2;

  mr = mw*2;
  clg();

  while (getk()!='q') {
	ads = 0.004;
	rds = 1.0;
	r = 5.0;
	rd = 0.0;
	rs = mh/4.3;
	a = 0.0;
	ad = ads;

	  for (i=1; i<=ITERATIONS; i++) {
		  for (t=0; t<=99; t++) {
			  ch=(rand() % 5);
				if (ch == 0) rd = rds;
				if (ch == 1) rd = -rds * 0.991;
				if (ch == 2) ad = ads;
				if (ch == 3) ad = -ads;
			  r += rd;
			  if (r<3.0) r = 3.0;
			  if (r>w) r = mr;
			  a += ad;
			  if (a<0.0) a += pi2;
			  if (a>pi2) a -= pi2;
			  ar = a+r/rs;
			  x = r*sin(ar)+mw;
			  y = r*cos(ar)+mh;
			  if ((x >= 0) && (x <= w) && (y<=h)) plot (x,y);
		  }
	  }
  }

}
