

// zcc +zx -create-app -O3 -lm -DGFXSCALEX=2/5 -DGFXSCALEY=2/5 -llib3d orbits.c


/* Developed by Puskar Jasu */

/* https://www.puskarcoding.com/project/how-to-draw-moving-of-moon-and-earth-around-sun-by-c-program/ */


#include <graphics.h>
#include <conio.h>
#include <bgi.h>
#include <math.h>


int main(void)
{
    double x, y, i, a = 0, b = 0, c = 0, d = 0, f = 0, h = 0, j;
    int graphic_driver = DETECT, graphic_mode;
    initgraph(&graphic_driver, &graphic_mode, "//turboc3/bgi");
    x = 256 ;
    y = 256 ;
    //while (!kbhit())
	while (getk()!=' ')
    {
        //setcolor(15);
		bgi_fillstyle=7;
        settextstyle(1, 0, 4);
        outtextxy(0, 0, "moon");
        outtextxy(0, 30, "earth");
        outtextxy(0, 60, "sun");
        outtextxy(530, 0, "puskar");
        outtextxy(530, 30, "jasu");
        setbkcolor(0);
        //setcolor(4);
		bgi_fillstyle=4;
        //setfillstyle(1, 4);
        fillellipse(x, y, 50, 50);
        i = 210 * cos((0 + f) * 3.14159 / 180);
        j = 170 * sin((0 + f) * 3.14159 / 180);
        f++;
        if (f == 360)
            f = 0;
        //setcolor(1);
		bgi_fillstyle=1;
        //setfillstyle(1, 1);
        fillellipse(x + i, y - j, 30, 30);
        a = 90 * cos((0 + c) * 3.14159 / 180);
        b = 70 * sin((0 + c) * 3.14159 / 180);
        ellipse(x, y, 0, 360, 210, 170);
        c += 5;
        if (c == 360)
            c = 0;
        //setcolor(6);
		bgi_fillstyle=6;
        //setfillstyle(1, 6);
        fillellipse(x + i + a, y - j - b, 15, 15);
        a = 300 * cos((0 + d) * 3.14159 / 180);
        b = 239 * sin((0 + d) * 3.14159 / 180);
        //setcolor(0);
		bgi_fillstyle=0;
        //setfillstyle(1, 0);
        fillellipse(x + a, y - b, 27, 27);
        //setcolor(6);
		bgi_fillstyle=6;
        ellipse(x + i, y - j, 0, 360, 90, 70);
        d++;
        if (d == 360)
            d = 0;
        if ((f > 38 && f < 52) || (f > 128 && f < 142) || (f > 218 && f < 232) || (f > 308 && f < 322))
        {
            a = 35 * cos((0 + h) * 3.14159 / 180);
            b = 30 * sin((0 + h) * 3.14159 / 180);
            //setcolor(0);
			bgi_fillstyle=0;
            //setfillstyle(1, 0);
            fillellipse(x + i + a, y - j - b, 20, 20);
        }
        h += 5;
        if (h == 360)
            h = 0;
        //delay(200);
		msleep(500);
        cleardevice();
    }
    getch();
    closegraph();
    return 0;
}
