/*
   GRAPHICS DEMO FOR Borland C++ 3.0
   Copyright (c) 1987,88,91 Borland International. All rights reserved.
   
   ..reworked for z88dk in 2022
   
    zcc +zx -lndos -llib3d -lm -create-app bgidemo.c
*/


//#include <dos.h>
#include <math.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <bgi.h>

//#define ESC	0x1b			/* Define the escape key	*/
#define TRUE	1			/* Define some handy constants	*/
#define FALSE	0			/* Define some handy constants	*/
//#define PI	3.14159 		/* Define a value for PI	*/
#define ON	1			/* Define some handy constants	*/
#define OFF	0			/* Define some handy constants	*/

#define NFONTS 11

char *Fonts[NFONTS] = {
  "DefaultFont",   "TriplexFont",   "SmallFont",
  "SansSerifFont", "GothicFont", "ScriptFont", "SimplexFont", "TriplexScriptFont",
  "ComplexFont", "EuropeanFont", "BoldFont"
};

char *LineStyles[] = {
  "SolidLn",  "DottedLn",  "CenterLn",  "DashedLn",  "UserBitLn"
};

char *FillStyles[] = {
  "EmptyFill",  "SolidFill",      "LineFill",      "LtSlashFill",
  "SlashFill",  "BkSlashFill",    "LtBkSlashFill", "HatchFill",
  "XHatchFill", "InterleaveFill", "WideDotFill",   "CloseDotFill"
};

char *TextDirect[] = {
  "HorizDir",  "VertDir"
};

char *HorizJust[] = {
  "LeftText",   "CenterText",   "RightText"
};

char *VertJust[] = {
  "BottomText",  "CenterText",  "TopText"
};

struct PTS {
  int x, y;
};	/* Structure to hold vertex points	*/

int    GraphDriver;		/* The Graphics device driver		*/
int    GraphMode;		/* The Graphics mode value		*/
double AspectRatio;		/* Aspect ratio of a pixel on the screen*/
int    MaxX, MaxY;		/* The maximum resolution of the screen */
int    MaxColors;		/* The maximum # of colors available	*/
int    ErrorCode;		/* Reports any graphics errors		*/
struct palettetype palette;		/* Used to read palette info	*/

/*									*/
/*	Function prototypes						*/
/*									*/

void Initialize(void);
//void ReportStatus(void);

void Bar3DDemo(void);


void PieDemo(void);
void PutPixelDemo(void);
void LineToDemo(void);
void BarDemo(void);
void LineRelDemo(void);
void ArcDemo(void);
void CircleDemo(void);

void FillStyleDemo(void);

void SayGoodbye(void);
void Pause(void);
void MainWindow(char *header);
void StatusLine(char *msg);
void DrawBorder(void);
void changetextstyle(int font, int direction, int charsize);
int  gprintf(int *xloc, int *yloc, char *fmt, ... );

/*									*/
/*	Begin main function						*/
/*									*/

int main()
{

  Initialize(); 		/* Set system into Graphics mode	*/
//  ReportStatus();		/* Report results of the initialization */

  PutPixelDemo();
  Bar3DDemo();
  BarDemo();
  ArcDemo();
  CircleDemo();
  PieDemo();
  LineRelDemo();
  LineToDemo();
  FillStyleDemo();

  SayGoodbye(); 		/* Give user the closing screen 	*/

  closegraph(); 		/* Return the system to text mode	*/
  return(0);
}


/*									*/
/*	CHANGETEXTSTYLE: similar to settextstyle, but checks for	*/
/*	errors that might occur whil loading the font file.		*/
/*									*/

void changetextstyle(int font, int direction, int charsize)
{
  int ErrorCode;

  graphresult();			/* clear error code		*/
  settextstyle(font, direction, charsize);
  ErrorCode = graphresult();		/* check result 		*/
  if( ErrorCode != grOk ){		/* if error occured		*/
    closegraph();
    printf(" Graphics System Error: %s\n", grapherrormsg( ErrorCode ) );
    exit( 1 );
  }
}


/*									*/
/*	INITIALIZE: Initializes the graphics system and reports 	*/
/*	any errors which occured.					*/
/*									*/

void Initialize(void)
{
  int xasp, yasp;			/* Used to read the aspect ratio*/

  GraphDriver = DETECT; 		/* Request auto-detection	*/
  initgraph( &GraphDriver, &GraphMode, "" );
  ErrorCode = graphresult();		/* Read result of initialization*/
  if( ErrorCode != grOk ){		/* Error occured during init	*/
    printf(" Graphics System Error: %s\n", grapherrormsg( ErrorCode ) );
    exit( 1 );
  }

  //getpalette( &palette );		/* Read the palette from board	*/
  MaxColors = getmaxcolor() + 1;	/* Read maximum number of colors*/

  MaxX = getmaxx();
  MaxY = getmaxy();			/* Read size of screen		*/

  getaspectratio( &xasp, &yasp );	/* read the hardware aspect	*/
  AspectRatio = (double)xasp / (double)yasp; /* Get correction factor	*/

}

/*									*/
/*	MAINWINDOW: Establish the main window for the demo and set	*/
/*	a viewport for the demo code.					*/
/*									*/

void MainWindow( char *header )
{
  int height;

  cleardevice();			/* Clear graphics screen	*/
  //setcolor( MaxColors - 1 );		/* Set current color to white	*/
  //setviewport( 0, 0, MaxX, MaxY, 1 );	/* Open port to full screen	*/

  height = textheight( "H" );           /* Get basic text height        */

  changetextstyle( DEFAULT_FONT, HORIZ_DIR, 1 );
  settextjustify( CENTER_TEXT, TOP_TEXT );
  outtextxy( MaxX/3, 2, header );
  //setviewport( 0, height+4, MaxX, MaxY-(height+4), 1 );
  DrawBorder();
  //setviewport( 1, height+5, MaxX-1, MaxY-(height+5), 1 );

}

/*									*/
/*	PAUSE: Pause until the user enters a keystroke. If the		*/
/*	key is an ESC, then exit program, else simply return.		*/
/*									*/

void Pause(void)
{
  static char msg[] = "SPACE aborts or press a key...";
  int c;

  StatusLine( msg );			/* Put msg at bottom of screen	*/

  c = getch();				/* Read a character from kbd	*/

  if( ' ' == c ){			/* Does user wish to leave?	*/
    closegraph();			/* Change to text mode		*/
    exit( 1 );				/* Return to OS 		*/
  }

  if( 0 == c ){ 			/* Did use hit a non-ASCII key? */
    c = getch();			/* Read scan code for keyboard	*/
  }

  cleardevice();			/* Clear the screen		*/

}


/*									*/
/*	SAYGOODBYE: Give a closing screen to the user before leaving.	*/
/*									*/

void SayGoodbye(void)
{
  //struct viewporttype viewinfo; 	/* Structure to read viewport	*/
  int h, w;

  MainWindow( "== Finale ==" );

  //getviewsettings( &viewinfo ); 	/* Read viewport settings	*/
  changetextstyle( TRIPLEX_FONT, HORIZ_DIR, 4 );
  settextjustify( CENTER_TEXT, CENTER_TEXT );

  //h = viewinfo.bottom - viewinfo.top;
  h = getmaxy()/2;
  //w = viewinfo.right  - viewinfo.left;
  w = getmaxx()/2;
  outtextxy( w/2, h/2, "That's all, folks!" );

  StatusLine( "Press any key to EXIT" );
  getch();

  cleardevice();			/* Clear the graphics screen	*/

}

/*									*/
/*	PIEDEMO: Display a pie chart on the screen.			*/
/*									*/

#define adjasp( y )	((int)(AspectRatio * (double)(y)))
#define torad( d )	(( (double)(d) * PI ) / 180.0 )

void PieDemo(void)
{
  //struct viewporttype vp;
  int xcenter, ycenter, radius, lradius;
  int x, y;
  double radians, piesize;

  MainWindow( "Pie Chart Demonstration" );


//  getviewsettings( &vp );		/* Get the current viewport	*/
//  xcenter = (vp.right - vp.left) / 2;	/* Center the Pie horizontally	*/
//  ycenter = (vp.bottom - vp.top) / 2+20;/* Center the Pie vertically	*/
//  radius  = (vp.bottom - vp.top) / 3;	/* It will cover 2/3rds screen	*/
//  piesize = (vp.bottom - vp.top) / 4.0; /* Optimum height ratio of pie	*/
xcenter = getmaxx()/2;
ycenter = getmaxy()/2;
radius = getmaxy()/3;


//  while( (AspectRatio*radius) < piesize ) ++radius;

  lradius = radius + ( radius / 5 );	/* Labels placed 20% farther	*/

  changetextstyle( TRIPLEX_FONT, HORIZ_DIR, 4 );
  settextjustify( CENTER_TEXT, TOP_TEXT );
  outtextxy( MaxX/3, 9, "This is a Pie Chart" );
  changetextstyle( TRIPLEX_FONT, HORIZ_DIR, 1 );
  settextjustify( CENTER_TEXT, TOP_TEXT );

  setfillstyle( SOLID_FILL, RED );
  pieslice( xcenter+10, ycenter-adjasp(10), 0, 90, radius );
  radians = torad( 45 );
  x = xcenter + (int)( cos( radians ) * (double)lradius );
  y = ycenter - (int)( sin( radians ) * (double)lradius * AspectRatio );
  settextjustify( LEFT_TEXT, BOTTOM_TEXT );
  outtextxy( x+10, y, "25 %" );

  setfillstyle( WIDE_DOT_FILL, GREEN );
  pieslice( xcenter, ycenter, 90, 135, radius );
  radians = torad( 113 );
  x = xcenter + (int)( cos( radians ) * (double)lradius );
  y = ycenter - (int)( sin( radians ) * (double)lradius * AspectRatio );
  settextjustify( RIGHT_TEXT, BOTTOM_TEXT );
  outtextxy( x-8, y, "12.5 %" );

  setfillstyle( INTERLEAVE_FILL, YELLOW );
  settextjustify( RIGHT_TEXT, CENTER_TEXT );
  pieslice( xcenter-10, ycenter, 135, 225, radius );
  radians = torad( 180 );
  x = xcenter + (int)( cos( radians ) * (double)lradius );
  y = ycenter - (int)( sin( radians ) * (double)lradius * AspectRatio );
  settextjustify( RIGHT_TEXT, CENTER_TEXT );
  outtextxy( x-20, y, "25 %" );

  setfillstyle( HATCH_FILL, BLUE );
  pieslice( xcenter, ycenter, 225, 360, radius );
  radians = torad( 293 );
  x = xcenter + (int)( cos( radians ) * (double)lradius );
  y = ycenter - (int)( sin( radians ) * (double)lradius * AspectRatio );
  settextjustify( LEFT_TEXT, TOP_TEXT );
  outtextxy( x-5, y, "37.5 %" );

  Pause();				/* Pause for user's response    */

}

/*									*/
/*	BARDEMO: Draw a 2-D bar chart using Bar and Rectangle.		*/
/*									*/

void BarDemo(void)
{
  int barheight[] = {
    1, 3, 5, 2, 4   };
  int styles[]	  = {
    1, 3, 10, 5, 9, 1	};
  int xstep, ystep;
  int sheight, swidth;
  int i, j, h;
  //struct viewporttype vp;
  char buffer[40];

  MainWindow( "Bar / Rectangle demonstration" );
  h = 3 * textheight( "H" );
 // getviewsettings( &vp );
  settextjustify( CENTER_TEXT, TOP_TEXT );
  changetextstyle( TRIPLEX_FONT, HORIZ_DIR, 4 );
  outtextxy( MaxX /3, 9, "These are 2-D Bars" );
  changetextstyle( DEFAULT_FONT, HORIZ_DIR, 1 );
//  setviewport( vp.left+50, vp.top+30, vp.right-50, vp.bottom-10, 1 );

//  getviewsettings( &vp );
//  sheight = vp.bottom - vp.top;
//  swidth  = vp.right  - vp.left;
  sheight = getmaxy()-10;
  swidth  = getmaxx()-10;

  line( h, h, h, sheight-h );
  line( h, sheight-h, sheight-h, sheight-h );
  ystep = (sheight - (2*h) ) / 5;
  xstep = (swidth  - (2*h) ) / 5;
  j = sheight - h;
  settextjustify( CENTER_TEXT, CENTER_TEXT );


  for( i=0 ; i<6 ; ++i ){
    line( h/2, j, h, j );
    itoa( i, buffer, 10 );
    outtextxy( 3, j-5, buffer );
    j -= ystep;
  }

  j = h;

  settextjustify( CENTER_TEXT, TOP_TEXT );
  for( i=0 ; i<6 ; ++i ){
    setfillstyle( styles[i], random(MaxColors) );
    line( j, sheight - h, j, sheight- 3 - (h/2) );
    itoa( i, buffer, 10 );
    outtextxy( j, sheight - (h/2), buffer );
    if( i != 5 ){
      bar( j, (sheight-h)-(barheight[i] * ystep), j+xstep, sheight-h-1 );
      rectangle( j, (sheight-h)-(barheight[i] * ystep), j+xstep, sheight-h);
    }
    j += xstep;
  }

  Pause();

}

/*									*/
/*	DRAWBORDER: Draw a solid single line around the current 	*/
/*	viewport.							*/
/*									*/

void DrawBorder(void)
{
//  struct viewporttype vp;

//  setcolor( MaxColors - 1 );		/* Set current color to white	*/

//  setlinestyle( SOLID_LINE, 0, NORM_WIDTH );

  //getviewsettings( &vp );
  //rectangle( 0, 0, vp.right-vp.left, vp.bottom-vp.top );
  rectangle( 0, 0, getmaxx(), getmaxy() );

}


/*									*/
/*	STATUSLINE: Display a status line at the bottom of the screen.	*/
/*									*/

void StatusLine( char *msg )
{
  int height;

  //setviewport( 0, 0, MaxX, MaxY, 1 );	/* Open port to full screen	*/
  //setcolor( MaxColors - 1 );		/* Set current color to white	*/

  changetextstyle( DEFAULT_FONT, HORIZ_DIR, 1 );
  settextjustify( CENTER_TEXT, TOP_TEXT );
  setlinestyle( SOLID_LINE, 0, NORM_WIDTH );
  setfillstyle( EMPTY_FILL, 0 );

  height = textheight( "H" );           /* Detemine current height      */
  bar( 0, MaxY-(height+4), MaxX, MaxY );
  rectangle( 0, MaxY-(height+4), MaxX, MaxY );
  outtextxy( MaxX/2, MaxY-(height+2), msg );
  //setviewport( 1, height+5, MaxX-1, MaxY-(height+5), 1 );

}



/*									*/
/*	BAR3DDEMO: Display a 3-D bar chart on the screen.		*/
/*									*/

void Bar3DDemo(void)
{
  static int barheight[] = {
    1, 3, 5, 4, 3, 2, 1, 5, 4, 2, 3   };
  //struct viewporttype vp;
  int xstep, ystep;
  int i, j, h, color, bheight;
  char buffer[10];

  MainWindow( "Bar 3-D / Rectangle Demonstration" );

  h = 3 * textheight( "H" );
  //getviewsettings( &vp );
  settextjustify( CENTER_TEXT, TOP_TEXT );
  changetextstyle( TRIPLEX_FONT, HORIZ_DIR, 4 );
  outtextxy( MaxX/3, 9, "These are 3-D Bars" );
  changetextstyle( DEFAULT_FONT, HORIZ_DIR, 1 );
  //setviewport( vp.left+50, vp.top+40, vp.right-50, vp.bottom-10, 1 );
  //getviewsettings( &vp );

  //line( h, h, h, vp.bottom-vp.top-h );
  line( h, h, h, getmaxy );
  //line( h, (vp.bottom-vp.top)-h, (vp.right-vp.left)-h, (vp.bottom-vp.top)-h );
  line( h, getmaxy()-h, getmaxx()-h, getmaxy()-h );
  //xstep = ((vp.right-vp.left) - (2*h)) / 10;
  xstep = (getmaxx() - (2*h)) / 10;
  ystep = (getmaxy() - (2*h)) / 5;
  //j = (vp.bottom-vp.top) - h;
  j = getmaxy() - h;
  
  settextjustify( CENTER_TEXT, CENTER_TEXT );

  for( i=0 ; i<6 ; ++i ){
    line( h/2, j, h, j );
    itoa( i, buffer, 10 );
    outtextxy( 3, j-3, buffer );
    j -= ystep;
  }
  //line( h, getmaxy()-h, getmaxx()-h, getmaxy()-h );

  j = h;
  settextjustify( CENTER_TEXT, TOP_TEXT );

  for( i=0 ; i<11 ; ++i ){
    color = random( MaxColors );
    setfillstyle( i+1, color );
    //line( j, (vp.bottom-vp.top)-h, j, (vp.bottom-vp.top-3)-(h/2) );
	line( j, getmaxy()-h, j, (getmaxy()-3)-(h/2) );
	
    itoa( i, buffer, 10 );
    outtextxy( j, (getmaxy())-(h/2), buffer );
    if( i != 10 ){
      bheight = (getmaxy()) - h - 1;
      bar3d( j, (getmaxy()-h)-(barheight[i]*ystep), j+xstep, bheight, 2, 1 );
    }
    j += xstep;
  }

  Pause();				/* Pause for user's response    */

}



/*									*/
/*	PUTPIXELDEMO: Display a pattern of random dots on the screen	*/
/*	and pick them back up again.					*/
/*									*/

void PutPixelDemo(void)
{
  int seed = 1958;
  int i, x, y, h, w, color;
  //struct viewporttype vp;

  MainWindow( "PutPixel / GetPixel Demonstration" );

  //getviewsettings( &vp );
  h = getmaxy()-20;
  w = getmaxx();

  srand( seed );			/* Restart random # function	*/

  for( i=0 ; i<5000 ; ++i ){		/* Put 5000 pixels on screen	*/
    x = 1 + random( w - 1 );		/* Generate a random location	*/
    y = 1 + random( h - 1 );
    color = random( MaxColors );
    putpixel( x, y+17, color );
  }

  srand( seed );			/* Restart Random # at same #	*/

  for( i=0 ; i<5000 ; ++i ){		/* Take the 5000 pixels off	*/
    x = 1 + random( w - 1 );		/* Generate a random location	*/
    y = 1 + random( h - 1 );
    color = getpixel( x, y );		/* Read the color pixel 	*/
    if( color == random( MaxColors ) )	/* Used to keep RANDOM in sync	*/
      putpixel( x, y+17, 0 );		/* Write pixel to BLACK 	*/
  }

  Pause();				/* Wait for user's response     */

}


/*									*/
/*	LINETODEMO: Display a pattern using moveto and lineto commands. */
/*									*/

#define MAXPTS	15

void LineToDemo(void)
{
  //struct viewporttype vp;
  struct PTS points[MAXPTS];
  int i, j, h, w, xcenter, ycenter;
  int radius, angle, step;
  double  rads;

  MainWindow( "MoveTo / LineTo Demonstration" );

  //getviewsettings( &vp );
  h = getmaxy();
  w = getmaxx();

  xcenter = w / 2;			/* Determine the center of circle */
  ycenter = h / 2;
  radius  = (h - 30) / (AspectRatio * 2);
  step	  = 360 / MAXPTS;		/* Determine # of increments	*/

  angle = 0;				/* Begin at zero degrees	*/
  for( i=0 ; i<MAXPTS ; ++i ){		/* Determine circle intercepts	*/
    rads = (double)angle * PI / 180.0;	/* Convert angle to radians	*/
    points[i].x = xcenter + (int)( cos(rads) * radius );
    points[i].y = ycenter - (int)( sin(rads) * radius * AspectRatio );
    angle += step;			/* Move to next increment	*/
  }

  circle( xcenter, ycenter, radius );	/* Draw bounding circle 	*/

  for( i=0 ; i<MAXPTS ; ++i ){		/* Draw the cords to the circle */
    for( j=i ; j<MAXPTS ; ++j ){	/* For each remaining intersect */
      moveto(points[i].x, points[i].y); /* Move to beginning of cord	*/
      lineto(points[j].x, points[j].y); /* Draw the cord		*/
    }
  }

  Pause();				/* Wait for user's response     */

}

/*									*/
/*	FILLSTYLEDEMO: Display the standard fill patterns available.	*/
/*									*/

void FillStyleDemo(void)
{
  int h, w, style;
  int i, j, x, y;
  //struct viewporttype vp;
  char buffer[40];

  MainWindow( "Pre-defined Fill Styles" );

  //getviewsettings( &vp );
  w = 2 * ((getmaxx()  +  1) / 13);
  h = 2 * ((getmaxy() - 10) / 10);

  x = w / 2;
  y = h / 2;		/* Leave 1/2 blk margin 	*/
  style = 0;

  for( j=0 ; j<3 ; ++j ){		/* Three rows of boxes		*/
    for( i=0 ; i<4 ; ++i ){		/* Four column of boxes 	*/
      setfillstyle(style, MaxColors-1); /* Set the fill style and WHITE */
      bar( x, y, x+w, y+h );		/* Draw the actual box		*/
      rectangle( x, y, x+w, y+h );	/* Outline the box		*/
      itoa( style, buffer, 10 );	/* Convert style 3 to ASCII	*/
      outtextxy( x+(w / 2), y+h+4, buffer );
      ++style;				/* Go on to next style #	*/
      x += (w / 2) * 3; 		/* Go to next column		*/
    }				/* End of coulmn loop		*/
    x = w / 2;				/* Put base back to 1st column	*/
    y += (h / 2) * 3;			/* Advance to next row		*/
  }					/* End of Row loop		*/

  settextjustify( LEFT_TEXT, TOP_TEXT );

  Pause();				/* Wait for user's response     */

}


/*									*/
/*	CIRCLEDEMO: Display a random pattern of circles on the screen	*/
/*	until the user says enough.					*/
/*									*/

void CircleDemo(void)
{
  int mradius;				/* Maximum radius allowed	*/

  MainWindow( "Circle Demonstration" );
  StatusLine( "ESC Aborts - Press a Key to stop" );

  mradius = MaxY / 10;			/* Determine the maximum radius */

  while( getk() ){}
  while( !getk() ){			/* Repeat until a key is hit	*/
    setcolor( random( MaxColors - 1 ) + 1 );	/* Randomly select a color	*/
    circle( random(MaxX), 10+random(MaxY-30), random(mradius) );
  }					/* End of WHILE not KBHIT	*/

  Pause();				/* Wait for user's response     */

}

/*									*/
/*	ARCDEMO: Display a random pattern of arcs on the screen */
/*	until the user says enough.					*/
/*									*/

void ArcDemo(void)
{
  int mradius;				/* Maximum radius allowed	*/
  int eangle;				/* Random end angle of Arc	*/
  //struct arccoordstype ai;		/* Used to read Arc Cord info	*/

  MainWindow( "Arc Demonstration" );
  StatusLine( "ESC Aborts - Press a Key to stop" );

  mradius = MaxY / 10;			/* Determine the maximum radius */

  setfillstyle( EMPTY_FILL, MaxColors-1);
  

  while( getk() ){}
  while( !getk() ){			/* Repeat until a key is hit	*/
    setcolor( random( MaxColors - 1 ) + 1 );	/* Randomly select a color	*/
    eangle = random( 358 ) + 1; 	/* Select an end angle		*/
	
    arc( random(MaxX-mradius*2)+mradius, random(MaxY-mradius*2)+mradius, random(eangle), eangle, mradius );
	//getarccoords( &ai );		/* Read Cord data		*/
    //line( ai.x, ai.y, ai.xstart, ai.ystart ); /* line from start to center */
    //line( ai.x, ai.y,	ai.xend,   ai.yend ); /* line from end to center   */
	
	
	
  }					/* End of WHILE not KBHIT	*/

  Pause();				/* Wait for user's response     */

}

/*									*/
/*	LINERELDEMO: Display pattern using moverel and linerel cmds.	*/
/*									*/

void LineRelDemo(void)
{
  //struct viewporttype vp;
  int h, w, dx, dy, cx, cy;
  struct PTS outs[7];


  MainWindow( "MoveRel / LineRel Demonstration" );
  //StatusLine( "Any key to continue, SPACE to Abort" );

  //getviewsettings( &vp );
  cx = getmaxx() / 2;	/* Center of the screen coords	*/
  cy = getmaxy( ) / 2;

  h  = cx / 8;
  w  = cy / 9;

  dx = 2 * w;
  dy = 2 * h;

  setcolor( BLACK );

  setfillstyle( WIDE_DOT_FILL, BLUE );
  bar( 0, 10, getmaxx(), getmaxy()-10 );	/* Draw backgnd */



  /*	Draw a Tesseract object on the screen using the LineRel and	*/
  /*	MoveRel drawing commands.					*/

  moveto( cx-dx, cy-dy );
  linerel(  w, -h );
  linerel(  3*w,	0 );
  linerel(   0,  5*h );
  linerel( -w,	h );
  linerel( -3*w,	0 );
  linerel(   0, -5*h );

  moverel( w, -h );
  linerel(   0,  5*h );
  linerel( w+(w/2), 0 );
  linerel(   0, -3*h );
  linerel( w/2,   -h );
  linerel( 0, 5*h );

  moverel(  0, -5*h );
  linerel( -(w+(w/2)), 0 );
  linerel( 0, 3*h );
  linerel( -w/2, h );

  moverel( w/2, -h );
  linerel( w, 0 );

  moverel( 0, -2*h );
  linerel( -w, 0 );

  Pause();				/* Wait for user's response     */

}


