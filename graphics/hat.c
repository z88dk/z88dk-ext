/*

	Z88DK base graphics libraries examples
	Simple 3D math function drawing (not using the Z-buffer algorithm)
	
	The picture size is automatically adapted to the target display size
	
	to build:  zcc +<target> <stdio options> -lm -create-app hat.c
	or
	to build using math32:  zcc +<target> <stdio options> --math32 -create-app hat.c
	
	Examples:
	  zcc +zx -lm -lndos -create-app hat.c    (or -lmzx to use the maths code in ROM)
	  zcc +aquarius -lm -create-app hat.c
	  zcc +zx81 -create-app -lm hat.c
	
	
	
	stefano

*/

#include <graphics.h>
#include <stdio.h>
#include <math.h>



void main()
{
	int P,Q,ZI,XI,XL;
	
	float XR,YR,ZR,XT,ZT,ZS,YY;
	//int P,Q;
	float XP,YP;
	float ZP,XF,YF,X1,Y1;

	clg();

	XR=pi()*1.5;

	YR=1.0;
	XP=getmaxx()/2;
	XF=XR/XP;

	YP=getmaxy()/2;
	ZP=(XP+YP)/3;

	P=getmaxx()/1.96;
	Q=64;
	
	YF=getmaxy()/4;
	
	for (ZI=-Q; ZI<Q; ZI++) {
			ZT=ZI*2.25;
			ZS=ZT*ZT;
			XL=(sqrt(XP*XP-ZS)+0.5)*0.9;
			for (XI=-XL; XI<XL; XI++) {
				XT=sqrt(XI*XI+ZS)*XF;
				YY=(sin(XT)+0.4*sin(XT*3.0))*YF;
				X1=XI+ZI+P;
				Y1=YP-YY+ZI;
				plot(X1,Y1-1);
				undraw(X1,Y1,X1,getmaxy());
			}
	}
}
