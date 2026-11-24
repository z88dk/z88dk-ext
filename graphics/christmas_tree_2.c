/*
 * Christmas Tree Demo
 *
 * zcc +zx -lm -create-app christmas.c -o christmas
 * zcc +ts2068 -pragma-define:CLIB_ZX_CONIO32=1 -pragma-define:CLIB_DEFAULT_SCREEN_MODE=6 -DDETAILED -create-app  -Dhires -lm -o christmas christmas.c
 *
 */

#include <graphics.h>
#include <math.h>
#include <stdlib.h>

//#define WIDTH   256
//#define HEIGHT  192

//#define TREE_TOP_Y      25
//#define TREE_HEIGHT     150
//#define TREE_HALF_BASE  70

int cx;

int TREE_TOP_Y;
int TREE_HEIGHT;
int TREE_HALF_BASE;


/* --------------------------------------------------------- */
/* Draw a filled triangle tree                               */
/* --------------------------------------------------------- */

void draw_tree(void)
{
    int y;
    int w;

    for (y = 0; y < TREE_HEIGHT; y++) {

        w = (TREE_HALF_BASE * y) / TREE_HEIGHT;

        xordraw(cx - w, TREE_TOP_Y + y,
             cx + w, TREE_TOP_Y + y);
    }
}


/* --------------------------------------------------------- */
/* Trunk                                                     */
/* --------------------------------------------------------- */

void draw_trunk(void)
{
    int x;
    int y;
    int n;

	for(y=0;y<20;y++) {

		for(x=-8;x<=8;x++) {

			n=(rand()%8);

			if(n <5)
				xorplot(cx+x,
					 TREE_TOP_Y
					 +TREE_HEIGHT
					 +y);
		}
	}
}


/* --------------------------------------------------------- */
/* Star                                                      */
/* --------------------------------------------------------- */

void draw_star(void)
{
    int x = cx;
    int y = TREE_TOP_Y - 10;

    draw(x,     y - 6, x,     y + 6);
    draw(x - 6, y,     x + 6, y);

    draw(x - 4, y - 4, x + 4, y + 4);
    draw(x - 4, y + 4, x + 4, y - 4);
}


/* --------------------------------------------------------- */
/* Garland                                                   */
/* --------------------------------------------------------- */

void draw_garland(int yrel)
{
    int x;
    int y;
    int w;
    int amp;

    w = (TREE_HALF_BASE * yrel) / TREE_HEIGHT;

    amp = 3 + yrel / 20;

    for (x = -w; x <= w; x++) {

        y = TREE_TOP_Y
          + yrel
          + (int)(sin((double)x / 8.0) * amp);

        xorplot(cx + x, y);
    }
}

/* --------------------------------------------------------- */
/* Decorations                                               */
/* --------------------------------------------------------- */
void draw_ornaments(void)
{
    int i;
    int yrel;
    int w;
    int x;
    int y;

    for(i=0;i<20;i++) {

        yrel = 20 + rand() % (TREE_HEIGHT - 30);

        w = (TREE_HALF_BASE * yrel)
            / TREE_HEIGHT;

        x = cx - w + rand() % (2*w);

        y = TREE_TOP_Y + yrel;

        xorcircle(x,y,2,1);
    }
}


/* --------------------------------------------------------- */
/* Snow                                                      */
/* --------------------------------------------------------- */

void draw_snow(void)
{
    int i;

    for (i = 0; i < 200; i++) {

        plot(rand() % getmaxx(),
             rand() % getmaxy());
    }
}


/* --------------------------------------------------------- */
/* Add snow on the tree..                                                      */
/* --------------------------------------------------------- */

void letit_snow(void)
{
    int y;
    int x;
    int w;
    int n;

    for (y = 10; y < TREE_HEIGHT; y++) {

        w = (TREE_HALF_BASE * y) / TREE_HEIGHT;

        for (x = -w; x <= w; x++) {

            n = (rand() * (w * 2) + 1) / RAND_MAX;

            if (n < (w + x) / 16)
                continue;

            if ((rand() & (7-x)) == 0)
                unplot(cx + x, TREE_TOP_Y + y);
        }
    }
}

/* --------------------------------------------------------- */

int main(void)
{

	cx = getmaxx() / 2;
	TREE_TOP_Y      = 25;
	TREE_HEIGHT     = getmaxy()/19*15;
	TREE_HALF_BASE  = getmaxy()/34*14;

    clg();

    srand(1);

    draw_snow();

    draw_tree();
	letit_snow();

    draw_garland(20);
    draw_garland(40);
    draw_garland(60);
    draw_garland(80);
    draw_garland(105);
	draw_garland(133);
    
	draw_ornaments();

    draw_star();

    draw_trunk();

    while (1) {
        /* stay alive */
    }

    return 0;
}
