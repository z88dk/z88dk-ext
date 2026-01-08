/*

	Z88DK base graphics libraries examples
	Simple 3D math function drawing using the Z-buffer algorithm
	
	exp function based alternative
	
	to build:  zcc +<target> <stdio options> -lm -create-app expwave.c
	or
	to build using math32:  zcc +<target> <stdio options> --math32 -create-app expwave.c
	
	Examples:
	  zcc +zx -lm -lndos -create-app expwave.c
	  zcc +aquarius -lm -create-app expwave.c

	
*/

#include <graphics.h>
#include <stdio.h>
#include <math.h>

void main()
{

float x,y,incr,yenlarge;
int z,buf;

	clg();
	incr=2.0/(float)getmaxx();
	yenlarge=(float)getmaxy() / 6;

	for (x=-3.0; x<0; x=x+incr)
	{
		buf=getmaxy();
		for (y=-2.0; y<2.0; y=y+0.3)
		{
			z = (float)getmaxy() - (yenlarge * (y + 2.0) + ( yenlarge * 3.5 * exp(-(x*x + y*y) ) ));

			if (buf>z)
			{
				buf = z;
				plot ( (int) ((float)getmaxx() / 6.0 * (x + 3.0)), z);
				plot ( (int) ((float)getmaxx() / 6.0 * (3.0 - x)), z);
			}
		}
	}
	
	while (getk() != '\n') {};
}
