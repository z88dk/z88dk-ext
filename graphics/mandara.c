/*
 * Sample Program for Standard and Enhanced Functions < mandara.c >
 *
 * Original program by Taiji Yamada, Tokyo Denki University
 * z88dk version by Stefano Bodrato, Apr. 2024
 *
 * Timex 2068
 * zcc +ts2068 -create-app -pragma-define:CLIB_DEFAULT_SCREEN_MODE=6 -O3 -DWIDE_REZ mandara.c
 * Commodore 128
 * zcc +c128 -lgfx128hr -create-app -O3 -DWIDE_REZ mandara.c
 *
 * zcc +zx -create-app -O3 mandara.c
 *
 * zx81 ARX (or WRX) modes (better seen with an inverse video mod)
 * zcc +zx81 -subtype=arx -clib=arx -create-app -O3 mandara.c
 *
 */
 
#include <stdio.h>
#include <graphics.h>

#ifdef SPECTRUM
  #include <arch/zx/spectrum.h>
#endif

#ifdef WIDE_REZ
  unsigned char stencil[256 * 4];
#else
  unsigned char stencil[256 * 2];
#endif


void ball (x,y,r,n)
{
	stencil_init(stencil);
#ifdef WIDE_REZ
	stencil_add_circle(x*2,y,r,1,stencil);
#else
	stencil_add_circle(x,y,r/2,1,stencil);
#endif
	stencil_render(stencil, n);
}


void mandara( int n, int x, int y, int l )
{
  int i, j;

  if ( n < 0 )
    return;

  ball( x+l/2, y+l/2, l/2-1, n+6);
  for ( i=0; i<3; i++ )
    for ( j=0; j<3; j++ )
      if ( i==1 || j==1 )
        mandara( n-1, x+l/3*i, y+l/3*j, l/3 );
}


int do_mandara( void )
{
  int n = 4, x = 20, y = 20, l = 160;
  mandara( n, x, y, l );
  return !0;
}


int main( void )
{
	
#ifdef SPECTRUM
  zx_colour(INK_BLUE|PAPER_WHITE);
  zx_border(0);
  zx_cls();
#endif

  clg();
  do_mandara();
  while (getk()!=' ') {};
}
