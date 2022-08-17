
// zcc +<target> -lndos -lm -create-app web.c



// ZX Spectrum, compact code using ROM math routines
// zcc +zx -lndos -lmzx -create-app -O3 web.c


// ZX 81, WRX mod
// zcc +zx81 -lm81 -subtype=wrx -clib=ansi -create-app -O3 web.c


// C128 High Resolution graphics (tiny text in graphics mode)
// zcc +c128 -Dhires -clib=gfxtext -lgfx128hr -lm -create-app web.c



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <graphics.h>


char sides[10];
int za;
double x,zz;
double deg;
int radius;
int a,b,maxx,maxy;
int x_center,max_radius;

void main() {
  printf ("%cHow many sides ",12);
  gets(sides);
  za=atoi(sides);  zz=360.0/za;
  clg();
  maxx=getmaxx();  maxy=getmaxy();
  deg=pi()/180.0;
  max_radius=maxy/2;
  x_center=maxx/2;
  radius=max_radius;
  a=x_center; b=max_radius;
  while (radius>0) {
    x+=zz;
    plot(a,b);
    radius--;
#ifdef hires
    a=(radius*cos(x*deg))*2+x_center;
#else
    a=radius*cos(x*deg)+x_center;
#endif
    b=radius*sin(x*deg)+max_radius;
    drawto(a,b);
    draw(x_center,max_radius,a,b);
  }

}

