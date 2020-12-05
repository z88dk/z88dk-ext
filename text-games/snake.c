/*
 * 
 * Native console mode ezample:
 * zcc +ts2068 -startup=2 -create-app -lndos -O3 snake.c
 * (uncomment the "gotoxy" redefinition and extend maxcol to 60)
 * 
 * ANSI mode (conio.h holds all the needed redefinitions):
 * zcc +ts2068ansi -startup=2 -create-app -lndos -O3 snake.c
 * 
 */


#include <stdlib.h>
#include <stdio.h>

#include <conio.h>

/* Display size constants - HW dependent */
#define maxrow 15

// 40 columns
//#define maxcol 38

// 64 columns
//#define maxcol 60

// 32 colums
#define maxcol 30



/* Uncomment this to force native ZX81 / ZX Spectrum / TS2068 console driver */
#define gotoxy(a,b)		zx_setcursorpos(b-1,a-1)

/* Uncomment this to force native ZX Spectrum / TS2068 console driver */
//#define gotoxy(a,b)     printf("\026%c%c",b,a)

/* Spectrum colors */
/*
#define BLACK			0
#define YELLOW			6
#define LIGHTRED		2
#define LIGHTBLUE		5
#define WHITE			3
#define DARKGRAY		2
#define LIGHTCYAN		5
#define LIGHTGREEN		4
#define LIGHTMAGENTA	3

#define textcolor(a)     printf("\020%c",a)
*/

/* Uncomment this not to use colors */
#define textcolor(x) {}
#define textbackground(x) {}

/* This happens in 'conio.h' already */
//#define cprintf printf


/* prototypes */

void draw_line(int col, int row);
void show_score();
void add_segment();
void setup_level();

/* For Linux compatibility uncomment this block and remove the reference to <conio.h> */
/*
#include <sys/time.h>
#include <unistd.h>

#define randomize() srand(time(NULL))
#define getch() getchar()
#define gotoxy(a,b)     printf("%c[%u;%uH",27,b,a)

int kbhit (void) {
	struct timeval tv;
	fd_set rdfs;
	tv.tv_sec = 0;   tv.tv_usec = 0;
	FD_ZERO(&rdfs);   FD_SET (STDIN_FILENO, &rdfs);
	select(STDIN_FILENO+1, &rdfs, NULL, NULL, &tv);
	return FD_ISSET(STDIN_FILENO, &rdfs);
}
*/

#ifdef SMALL_C
//#define kbhit() (getk() ? 1 : 0)
//#define getch() getk()
#endif

/* constants */

#define snake_start_col maxcol/2
#define snake_start_row 7

#define up_key 'q'
#define down_key 'a'
#define left_key 'o'
#define right_key 'p'

const int pause_length=60;

int high_score=0;


/* global variables */

int score, snake_length, speed, obstacles, level, firstpress;

char screen_grid[maxrow*maxcol];

char direction = right_key;

int i,j;

struct snake_segment {
  int row;
  int col;
} snake[100];


int main()
{

  /* Variable declarations within main() only */
  char keypress;

  do /* restart game loop */
  {
	
    obstacles=4; level=1; score=0; speed=14;
    randomize(); /* Ensure random seed initiated */
    setup_level();

    /* main loop */

    do
    {

      for (i=0;i<(speed*pause_length);i++) j=1+i; /*pause*/
      //sleep(speed*pause_length); /*pause*/

      /* If key has been hit, then check it is a direction key - if so,
         change direction */

#ifdef SMALL_C
      i=getk();
      if (i>0)
      {
        keypress=(char)i;
        if((keypress==right_key)||(keypress==left_key)||
           (keypress==up_key)||(keypress==down_key))
          direction = keypress;
      }
#else
      if (kbhit())
      {
        keypress=(char)getch();
        if((keypress==right_key)||(keypress==left_key)||
           (keypress==up_key)||(keypress==down_key))
          direction = keypress;
      }
#endif

      /* Add a segment to the end of the snake */
      add_segment();


      /* Blank last segment of snake */
      gotoxy(snake[0].col,snake[0].row);
      cprintf(" ");

      /* ... and remove it from the array */
        for (i=1;i<=snake_length;i++) {
          /*snake[i-1]=snake[i];  <- z88dk does not support it ! */
			snake[i-1].row=snake[i].row;
			snake[i-1].col=snake[i].col;
		}


      /* Display snake in yellow */

      textcolor(YELLOW);

      for (i=0;i<=snake_length;i++)
      {

        gotoxy(snake[i].col,snake[i].row);

        cprintf("O");

      }

      /* keeps cursor flashing in one place instead of following snake */

#ifndef SMALL_C
      gotoxy(1,1);
#endif


      /* If first press on each level, pause until a key is pressed */

      if (firstpress) { while(!kbhit()); firstpress = 0; }



      /* Collision detection - walls (bad!) */

      if ((snake[snake_length-1].row>maxrow+1)||(snake[snake_length-1].row<=1)||

          (snake[snake_length-1].col>maxcol+1)||(snake[snake_length-1].col<=1)||

      /* Collision detection - obstacles (bad!) */

          (screen_grid[snake[snake_length-1].row-2+maxrow*(snake[snake_length-1].col-2)]=='X'))

        keypress='x'; /* i.e. exit loop - game over */

      /* Collision detection - snake (bad!) */

      for (i=0;i<snake_length-1;i++)

        if ( (snake[snake_length-1].row)==(snake[i].row) &&
             (snake[snake_length-1].col)==(snake[i].col))
        {
          keypress='x'; /* i.e. exit loop - game over */
          break; /* no need to check any more segments */
        }

      /* Collision detection - food (good!) */

      if (screen_grid[snake[snake_length-1].row-2+maxrow*(snake[snake_length-1].col-2)]=='o')
      {

        /* increase score and length of snake */
        score+=snake_length*obstacles; show_score(); snake_length++; add_segment();

        /* if length of snake reaches certain size, onto next level */
        if (snake_length==(level+3)*2)
        {
          score+=level*1000; obstacles+=2; level++;  /* add to obstacles */

          if ((level%5==0)&&(speed>1)) speed--; /* increase speed every 5 levels */
          setup_level(); /* display next level */
        }
      }

    } while (keypress!='x');



    /* game over message */

    if (score > high_score) high_score = score;

    show_score();

    gotoxy((maxcol-10)/2,6); textcolor(LIGHTRED); cprintf("G A M E   O V E R");
    gotoxy((maxcol-13)/2,9); textcolor(YELLOW); cprintf(" Another Game (y/n)? ");

    do keypress=getch(); while((keypress!='y')&&(keypress!='n'));

  } while (keypress=='y');

}



void setup_level()
{


 /* variables local to setup_level() */

 int row,col;


  /* Set up global variables for new level */

  snake_length=level+4; direction = right_key;

  firstpress = 1;

 /* Fill grid with blanks */

  for(row=0;row<maxrow;row++)
    for(col=0;col<maxcol;col++)
     screen_grid[row+maxrow*col]= ' ';



  /* Fill grid with Xs and food */

  for(i=0;i<obstacles*2;i++)
  {

   row= rand()%maxrow;
   col= rand()%maxcol;

   if(i<obstacles)
     screen_grid[row+maxrow*col]='X';
   else
     screen_grid[row+maxrow*col]='o';

  }



  /* Create snake array of length snake_length */

  for(i=0;i<snake_length;i++)
  {

    snake[i].row=snake_start_row;

    snake[i].col=snake_start_col+i;

  }



  /* Draw playing board */

  draw_line(1,1);

  for(row=0;row<maxrow;row++)
  {

    gotoxy(1,row+2);
    textcolor(LIGHTBLUE); cprintf("|");

    textcolor(WHITE);
    for(col=0;col<maxcol;col++)
      cprintf("%c",screen_grid[row+maxrow*col]);

    textcolor(LIGHTBLUE);
    cprintf("|");

  }

  draw_line(1,maxrow+2);

  show_score();

  gotoxy((maxcol-14)/2,maxrow+5);
  textcolor(LIGHTRED);
  cprintf("~~ SNAKE GAME ~~");

  textcolor(DARKGRAY);  
  gotoxy(2,maxrow+7);
  cprintf("Left:%c, Right:%c, Up:%c, Down:%c,  Exit:x.",

          left_key,right_key,up_key,down_key);

}



void draw_line(int col, int row)
{

  gotoxy(col,row); textcolor(LIGHTBLUE);
  for (col=0;col<maxcol+2;col++) cprintf("=");

}



void show_score()
{

  textcolor(LIGHTCYAN);
  gotoxy(2,maxrow+3);
  cprintf("Lev:%05d",level);

  gotoxy((maxcol-6)/2,maxrow+3);
  textcolor(LIGHTGREEN);
  cprintf("Sc:%05d",score);

  gotoxy((maxcol-9),maxrow+3);
  textcolor(LIGHTMAGENTA);
  cprintf("High:%05d",high_score);

}


void add_segment()
{

  switch(direction)
  {
    case(right_key): snake[snake_length].row=snake[snake_length-1].row;
                     snake[snake_length].col=snake[snake_length-1].col+1;
                     break;

    case(left_key) : snake[snake_length].row=snake[snake_length-1].row;
                     snake[snake_length].col=snake[snake_length-1].col-1;
                     break;

    case(up_key)   : snake[snake_length].row=snake[snake_length-1].row-1;
                     snake[snake_length].col=snake[snake_length-1].col;
                     break;

    case(down_key) : snake[snake_length].row=snake[snake_length-1].row+1;
                     snake[snake_length].col=snake[snake_length-1].col;
  }

}
