
// zcc +zx -lndos -llib3d -create-app bgitest2.c


#include<bgi.h>
//#include<conio.h>
 
int main() {
   int gd = DETECT, gm, x, y;
   initgraph(&gd, &gm, "C:\\TC\\BGI");
 
   settextstyle(BOLD_FONT,HORIZ_DIR,2);
   outtextxy(90,8,"PIE CHART");
   /* Setting cordinate of center of circle */
   x = getmaxx()/2+10;
   y = getmaxy()/2+10;
 
   settextstyle(SANS_SERIF_FONT,HORIZ_DIR,1);
   setfillstyle(BKSLASH_FILL, RED);
   pieslice(x, y, 0, 60, 70);
   outtextxy(x + 70, y - 50, "FOOD");
 
   setfillstyle(EMPTY_FILL, YELLOW);
   pieslice(x, y, 60, 160, 70);
   outtextxy(x - 60, y - 70, "RENT");
 
   setfillstyle(LTSLASH_FILL, GREEN);
   pieslice(x, y, 160, 220, 70);
   outtextxy(x - 127, y, "ELECTRICITY");
 
   setfillstyle(XHATCH_FILL, BROWN);
   pieslice(x, y, 220, 360, 70);
   outtextxy(x, y + 80, "SAVINGS");
 
   getch();
   closegraph();
   return 0;
}

