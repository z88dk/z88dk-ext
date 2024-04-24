
// Circle drawing in C with FP maths compared with a direct library call

#include <math.h>
#include <graphics.h>
#include <stdio.h>

#define RES     320
#define DIVIDER 127

char xtable[RES/8];
char ytable[RES/8];

void mycircle (int x0, int y0, int r)
{
    int i;
    int x,y;
    for(i=0;i<RES/8;i++)
    {
      x=xtable[i]*r/DIVIDER;
      y=ytable[i]*r/DIVIDER;
      plot(x0+x,y0+y);
      plot(x0-x,y0+y);
      plot(x0+x,y0-y);
      plot(x0-x,y0-y);
      plot(x0+y,y0+x);
      plot(x0-y,y0+x);
      plot(x0+y,y0-x);
      plot(x0-y,y0-x);
    }
}

main()
{
    int i;
    for(i=1; i<RES/8; i++)
    {
        int x=DIVIDER*cos((float)i*(2*M_PI/RES));
        xtable[i]=x;
        x=DIVIDER*sin((float)i*(2*M_PI/RES));
        ytable[i]=x;
    }
    xtable[0]=DIVIDER;
    ytable[0]=0;
    mycircle(getmaxx()/3,getmaxy()/2,getmaxy()/3);
	circle((getmaxx()/3)*2,getmaxy()/2,getmaxy()/3,1);
    while (!getk()) {};
}
