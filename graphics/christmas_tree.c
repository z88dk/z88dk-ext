
// Christmas tree, probably originally written
// for the TRS-80 High Resolution


// CP/M GSX build and run
//
// zcc +cpm -subtype=bbc -create-app -lm -lgfxgsx -DMAXX=320 -DMAXY=240 christmas.c
// zcc +cpm -subtype=televideo -create-app -lm -lgfxgsx -DMAXX=640 -DMAXY=200 christmas.c
//
// A>GENGRAF B:A.COM
// A>B:A


#include <graphics.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef C128
  #include <c128/vdc.h>
#endif

int x,y;
float xi;
long x1,n;
int xc,yc;

float i;

void main()
{

//ZX81 WRX
//invhrg();

#ifdef MAXX
  xc=MAXX/2;
#else
  xc=getmaxx()/2;
#endif

#ifdef MAXY
  xi=MAXY/2.8;
  yc=MAXY/6;
#else
  xi=getmaxy()/2.8;
  yc=getmaxy()/6;
#endif

  clg();
  
#ifdef C128
    outvdc(vdcFgBgColor,(vdcLightGreen << 4) | vdcBlack);
#endif

  for (i=0; i<xi; i+=0.15) {
#ifdef MAXY
	  x=xc+(xc*i*cos(i))/MAXY;
#else
	  x=xc+(xc*i*cos(i))/getmaxy();
#endif
	  y=i*2.1;
	  plot(x,yc+y);
  }
  plot(xc+5,yc-6);
  drawr(-10,0); drawr(8,6); drawr(-3,-9);
  drawr(-3,9); drawr(8,-6);
}


