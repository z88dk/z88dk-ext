/*

	Z88DK base graphics libraries examples
	Simple 3D math function drawing using the Z-buffer algorithm
	
	The picture size is automatically adapted to the target display size
	

*/

#include <graphics.h>
#include <stdio.h>
#include <math.h>
#include <lib3d.h>

void main()
{

float x,y,incr,yenlarge;
int z,buf;

	clg();
	incr=2.0/(float)getmaxx();
	yenlarge=(float)getmaxy() / 12.0;

	for (x=-6.0; x<0; x=x+incr)
	{
		buf=255;
		for (y=-6.0; y<6.0; y=y+0.3)
		{
			z = (unsigned char) (float)getmaxy() - (yenlarge * (y + 6.0) + yenlarge * cos(x) * cos(y) * exp(sqrt (x*x + y*y)/4) );

			if (buf>z)
			{
				buf = z;
				plot ( (int) ((float)getmaxx() / 12.0 * (x + 6.0)),  z);
				plot ( (int) ((float)getmaxx() / 12.0 * (6.0 - x)),  z);
			}
		}
	}
	
	while (getk() != '\n') {};
}

