#include <stdio.h>
#include <math.h>
#include <graphics.h>
#include <lib3d.h>


void turn (int degrees)
{
	if (degrees<0)
		turn_left(-degrees);
	else
		turn_right(degrees);
}

void main( void )
{
  int a = 0, n = 0, i, j, A = 2, N = 10;
  int deg = 72;

  clg();
  fwd(getmaxx()/2);
  turn(90);
  fwd(getmaxy()/2 - 10);
  pen_down();

 for ( i=0; i<=45; i++ ){
   if ( ++a > A )
     a = 1;
   for ( j=1; j<=5+a; j++ ){
     fwd( 20 );
     turn( deg );
   }
   fwd( 19 );
   deg = -deg;
   turn( deg );
   if ( ++n > N ){
     a = -a;
     n = 0;
   }
 }
 
 while (getk()!=' ') {};
}
