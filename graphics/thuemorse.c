/*

	Z88DK base graphics libraries examples
	Thue-Morse sequence applied to turtle graphics
	
	
	to build:   zcc +<target> -lndos -create-app -llib3d -lm thuemorse.c


	
	$Id: thuemorse.c $

*/

#include <graphics.h>
#include <stdio.h>
#include <lib3d.h>

char th[256];
int c,d,i;
int b;
int mask;

void decimalToBinary(int num) {
    // 8 bit integer
    for (int i = 7; i >= 0; i--) {
        mask = (1 << i);
        if (num & mask)
			turn_right(60);
           //printf("1");
        else 
			fwd(5);
           //printf("0");
    }
}

void main()
{
  clg();
  unplot(0,0);
  pen_up();
  move (getmaxx()-10,getmaxy()-10);
  pen_down();

  i=1;
  th[0]=105;  // First 8 bits of the Thue-Morse sequence

  for (d=0; d<8; d++) {
    for (c=0; c<i; c++)
	    th[c+i] = ~th[c];   // one's complement
	i*=2;
  }
  
  
  for (c=0; c<256; c++) {
	decimalToBinary(th[c]);
  }
}

