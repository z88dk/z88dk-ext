
/*
    This game is AI generated at the first shot, with the following prompt:

    "Scrivi il solito gioco di due cannoni che devono colpirsi da dietro 
	 a una montagna inserendo parametri di alzo e gittata.
	 Deve essere scritta in C con z88dk per ZX Spectrum e utilizzare la grafica.
	 Il prompt testuale potrebbe stare nella prima riga in alto
	 e la grafica occupare il resto dello schermo."

    Only few cosmetics changes were applied to clear the text prompt area,
	the bottom of the "mountain", and the wait-keypress routine capable of
	some randomness.
*/


#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <math.h>
#include <graphics.h>

#define SCREEN_W 256
#define SCREEN_H 192

int mountain_x1;
int mountain_x2;
int mountain_peak_x;
int mountain_peak_y;

int cannon1_x = 20;
int cannon2_x = 235;
int cannon_y  = 160;

void draw_mountain(void)
{
    int x;

    mountain_x1 = 80 + rand() % 30;
    mountain_x2 = 160 + rand() % 30;

    mountain_peak_x = 120 + rand() % 20;
    mountain_peak_y = 70 + rand() % 40;

    draw(mountain_x1, 191, mountain_peak_x, mountain_peak_y);
    draw(mountain_peak_x, mountain_peak_y, mountain_x2, 191);

    draw(mountain_x1,191,mountain_x2,191);
    draw(mountain_x1,190,mountain_x2,190);
}

int mountain_height(int x)
{
    long h;

    if(x < mountain_x1 || x > mountain_x2)
        return 191;

    if(x <= mountain_peak_x)
    {
        h = 191 -
            ((191 - mountain_peak_y) *
            (x - mountain_x1)) /
            (mountain_peak_x - mountain_x1);
    }
    else
    {
        h = mountain_peak_y +
            ((191 - mountain_peak_y) *
            (x - mountain_peak_x)) /
            (mountain_x2 - mountain_peak_x);
    }

    return (int)h;
}

void draw_cannons(void)
{
    circle(cannon1_x, cannon_y, 3, 1);
    circle(cannon2_x, cannon_y, 3, 1);

    draw(cannon1_x,cannon_y,
         cannon1_x+8,cannon_y-4);

    draw(cannon2_x,cannon_y,
         cannon2_x-8,cannon_y-4);
}

int shoot(int player)
{
    float angle;
    float power;
    float vx;
    float vy;

    float x;
    float y;
    float t;

    int px,py;

    printf("\nPlayer %d\n",player);

    printf("\nAngle ?");
    scanf("%f",&angle);
	angle += rand() % 2;

    printf("\nPower ?");
    scanf("%f",&power);
	power += rand() % 4;

    angle *= 3.14159265 / 180.0;

    if(player == 1)
    {
        x = cannon1_x;
        y = cannon_y;

        vx = cos(angle) * power;
    }
    else
    {
        x = cannon2_x;
        y = cannon_y;

        vx = -cos(angle) * power;
    }

    vy = sin(angle) * power;

    for(t=0;;t+=0.15)
    {
        px = (int)x;
        py = (int)y;

        plot(px,py);

        x = (player == 1)
            ? cannon1_x + vx*t
            : cannon2_x + vx*t;

        y = cannon_y
            - (vy*t - 0.5*9.8*t*t);

        px = (int)x;
        py = (int)y;

        if(px < 0 || px >= SCREEN_W)
            break;

        if(py < 0 || py >= SCREEN_H)
            break;

        if(py >= mountain_height(px))
        {
            printf("\nHit mountain!\n");
            return 0;
        }

        if(player == 1)
        {
            if(abs(px-cannon2_x) < 4 &&
               abs(py-cannon_y) < 4)
            {
                printf("\nDirect hit!\n");
                return 1;
            }
        }
        else
        {
            if(abs(px-cannon1_x) < 4 &&
               abs(py-cannon_y) < 4)
            {
                printf("\nDirect hit!\n");
                return 1;
            }
        }
    }

    printf("\nMissed!\n");
    return 0;
}

void wait_key () {
	int i = 0;
	while (getk()) {};
	while (!getk()) i++;
    srand(i);
}

int main(void)
{
    int player = 1;
	int i = 0;

    printf("\n               Cannon Duel - Press any key");
	wait_key();
    clg();

    draw_mountain();
    draw_cannons();

    while(1)
    {
		gotoxy(0,0);
        for (i=0; i<6; i++)
			printf("                      \n");
		gotoxy(0,0);

        if(shoot(player))
        {
            printf("\nPlayer %d wins!\n",player);
            break;
        }

        player = (player == 1) ? 2 : 1;
		
		if (player == 1) {
			wait_key();
			fputc_cons(12);
			draw_mountain();
			draw_cannons();
		}
    }

    return 0;
}
