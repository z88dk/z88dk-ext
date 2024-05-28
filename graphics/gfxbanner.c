
#include <stdio.h>
#include <math.h>
#include <graphics.h>
#include <string.h>

#include <lib3d.h>


// ROM font
// zcc +zx -lndos -create-app -lm gfxbanner.c
// zcc +zx81 -clib=wrx -subtype=wrx -create-app -lm gfxbanner.c

// Generic (provided font)
//  zcc +zx -lndos -create-app -lm -DUSE_FONTFN -pragma-redirect:CRT_FONT=_font_6x8_einstein_system  gfxbanner.c


// 6x8 fonts (good for TMS99x8 text modes)
// _font_6x8_clairsys;
// _font_6x8_coleco_adam_system;
// _font_6x8_einstein_system;
// _font_6x8_msx_system;
// _font_6x8_pmd85_system;

// 8x8 fonts
// _font_8x8_bbc_system;
// _font_8x8_c64_system;
// _font_8x8_clairsys;
// _font_8x8_clairsys_bold;
// _font_8x8_coleco_adam_system;
// _font_8x8_cpc_system;
// _font_8x8_einstein_system;
// _font_8x8_msx_system;
// _font_8x8_pmd85_system;
// _font_8x8_sam_system;
// _font_8x8_vga_rom;
// _font_8x8_zx_system;



#ifndef USE_FONTFN

// Set the ROM font location here.

#ifdef __SPECTRUM__
#define FONT 15360
#endif

#ifdef __ZX81__
#define FONT 7680
#endif


#else


unsigned int FONTFN() {
#asm
extern CRT_FONT
ld hl,CRT_FONT
dec h
#endasm
}

#endif



float s,g,e;
int a,b,c,d;
unsigned char p,q;

void rotate_text(int width, int height, int x, int y, int angle, char *text)
{
 x+=width*7; y+=height*8;
 g=angle/180.0*pi();
 s=sin(g); g=cos(g);
 a=0;
 
#ifdef __ZX81__
 text=strlwr(text);
#endif
 
 while (text[a]!=0) {
#ifdef USE_FONTFN
   b=FONTFN()+text[a]*8+7;
#else
#ifdef __ZX81__
   b=FONT+ascii_zx(text[a])*8+7;
#else
   b=FONT+text[a]*8+7;
#endif
#endif
   for (c=6;c>0;c--) {
	 p=*(unsigned char *)(b-c);
     for (d=0;d<7;d++) {
       q=p/2;
       if (p&1)
         for (e=0.0;e<(0.0+height);e+=0.4) {
           plot(x+(c*height+e)*s-d*width*g,y-(c*height+e)*g-d*width*s);
           drawr((1.2-width)*g,(1.2-width)*s);
         }
       p=q;
     }
   }
   x=x+g*width*8.0;
   y=y+s*width*8.0-g*width;
   
   a++;
 }
}

int x;
void main()
{
	clg();
	rotate_text(2, 3, 0, 0, 26, "Hello World!");
}
