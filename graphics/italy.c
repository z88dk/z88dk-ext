
// Italian flag
// This program relies on the colour clash
// 
// Based on an original program by Matthew Logue / Carlo Santagostino

// zcc +msx -create-app -lm italy.c
// zcc +zx -lndos -create-app -lm italy.c

#include <stdio.h>
#include <graphics.h>
#include <math.h>
#include <conio.h>

#ifdef __MSX__
#include <msx.h>
#endif


int cx,cy;
int x,y,z;


int colour_dot() {
	plot(cx+x-y,cy+x/2+y/2+z+1);
	plot(cx+x-y,cy+x/2+y/2+z);

	textcolor(RED);
	if (x<75)
		textcolor(WHITE);
	if (x<16)
		textcolor(GREEN);

	if ((x<82)&&(x>68)) return(0);
	if ((x<24)&&(x>8)) return(0);
	
	drawr(1,1);
	drawr(-1,1);
	drawr(1,-1);
	drawr(-1,-1);
	
	return(1);
}


int main() {

#ifdef __MSX__
	vdp_set_mode(2);
#endif
	bordercolor(BLACK);
	textbackground(BLACK);
	clrscr();
	clg();
	
	cx=getmaxx()/2; cy=getmaxy()/12;
	for (x=120; x>=-20; x-=4) {
		for (y=100; y>=0; y-=4) {
			z=30+((10*sin(x/11.0))*cos(y/52.0));
			colour_dot();
		}
	}
}

