
// Language options: -DLANG_IT, -DLANG_ES, -DLANG_FR, -DLANG_EN (default)
// In case of text misalignments, try adding -DYSHIFT

// ZX Spectrum
// zcc +zx -DUSE_UDGS -DGRAPHICS  -DVT_COLORS -DUSE_SOUND -lndos -create-app -lm dallas.c
// zcc +zx -DUSE_UDGS -DGRAPHICS -DUSE_SOUND -DVT_COLORS -clib=ansi -pragma-define:ansicolumns=32 -DUDG_FONT -pragma-redirect:CRT_FONT=_font_8x8_bbc_system -lndos -create-app -lm dallas.c

// MSXDOS
// zcc +msx -subtype=msxdos -DUSE_UDGS -DGRAPHICS -DUSE_SOUND -DVT_COLORS -clib=ansi -pragma-define:ansicolumns=32 -DUDG_FONT -pragma-redirect:CRT_FONT=_font_8x8_bbc_system -lndos -create-app -lm dallas.c

// Tatung Einstein
// zcc +cpm -subtype=einstein  -DUSE_UDGS -DGRAPHICS -DVT_COLORS -clib=ansi -pragma-define:ansicolumns=32 -DUDG_FONT -pragma-redirect:CRT_FONT=_font_8x8_bbc_system -lndos -create-app -lm dallas.c

// Spectravideo SVI
// zcc +svi -DUSE_UDGS -DGRAPHICS -DUSE_SOUND -DVT_COLORS -clib=ansi -pragma-define:ansicolumns=32 -DUDG_FONT -pragma-redirect:CRT_FONT=_font_8x8_bbc_system -lndos -create-app -lm dallas.c

// ZX81 (32K)  (  POKE 16389,166  :  NEW  :  LOAD ""  )
// zcc +zx81 -DALT_DELAY -DUSE_UDGS -DGRAPHICS -subtype=wrx -clib=wrxansi -pragma-define:ansicolumns=32 -create-app -lm -O3 dallas.c
#pragma output hrgpage = 42752
// zcc +zx81 -lgfx81 -DALT_DELAY -DGRAPHICS -DLOREZ -create-app -lm -O3 dallas.c

// LAMBDA 8300 (32K)
// zcc +lambda -DALT_DELAY -DGRAPHICS -DLOREZ -create-app -lm -O3 -D__ZX81__ dallas.c

// Jupiter ACE
// zcc +ace -DUSE_UDGS -DGRAPHICS -DLOREZ -DUSE_SOUND -lgfxace -lndos -create-app -lm dallas.c

// Commodore 128
// zcc +c128 -clib=gencon -DGRAPHICS -DLOREZ -lgfx128 -DUSE_UDGS  -DVT_COLORS -DUSE_SOUND -lndos -create-app -lm dallas.c

// Sharp MZ
// zcc +mz -clib=ansi -DGRAPHICS -DLOREZ -pragma-define:REGISTER_SP=0x6FFF -DVT_COLORS -DUSE_SOUND -lndos -create-app -lm dallas.c

// Mattel Aquarius
// zcc +aquarius -clib=ansi -DGRAPHICS -DLOREZ -DVT_COLORS -DUSE_SOUND -lndos -create-app -lm dallas.c

// EACA EG2000
// zcc +trs80 -subtype=eg2000disk -DVT_COLORS -DUSE_SOUND -lndos -create-app -lm -DUSE_UDGS dallas.c

// VTECH Laser 350, 500, 700
// zcc +laser500 -DUSE_SOUND -DVT_COLORS -lndos -lm -create-app -Cz--audio -DGRAPHICS -DLOREZ dallas.c

// Super-80r, Super-80v
// zcc +super80 -clib=vduem -lndos -create-app -lm -DUSE_UDGS -DUSE_SOUND dallas.c

// Microbee CP/M
// zcc +cpm -clib=ansi -DUSE_SOUND -create-app -lmicrobee  -DVT_COLORS -lndos -lm dallas.c
// zcc +cpm -clib=ansi -subtype=microbee -DUSE_SOUND -create-app -DVT_COLORS -lndos -lm dallas.c

// Sanyo MBC-200/MBC-1200 CP/M
// zcc +cpm -subtype=mbc200 -DUSE_SOUND  -DGRAPHICS -create-app -lndos -lm dallas.c

// Visual 1050 CP/M
// zcc +cpm -subtype=v1050 --generic-console -DUSE_UDGS -lndos -create-app -lm -DGRAPHICS dallas.c

// Xerox 820
// zcc +cpm -subtype=x820 --generic-console -lndos -create-app -lm dallas.c




// MinGW
// gcc -DVT_COLORS dallas.c






/*
                                INSTRUCTIONS

1.  INTRODUCTION
    This is a business simulation of the wheeler-dealer oil rich folk who live in Dallas.
    The program will appeal to all members of the family.  The program starts with
    setting up a map of Texas, splitting the region into hundreds of plots, each with a
    potential to discover oil.  You are required to make seismic surveys, bid for
    concessions, drill for oil, build production facilities and lay pipelines.  The game is
    menu driven and the player is able to direct his corporation along many diverse
    routes.  The object of the game is to accumultate $200M in net assets and $80M
    cash in order to take-over Euing Associates.  There are three levels of difficulty.

2.  THE BOARD
    A map of the oild fields is displayed.  On the left hills are shown in which drilling and
    production facilities are more expensive.  'D' at co-ordinate '(A,N)' is the city of Dallas.
    
	a)  Drilling rigs need to be moved from Dallas.
    b)  Pipelines need to be connected to the Dallas refineries to maximise production.
	
    A flashing cursor will indicate fields for which concessions are offered.
    After purchasing a field then progress through the following:
                         Concession purchased.
                         Drilling rig in place.
                         Oil strike.
                         Production facilities built.
                         Pipeline to Dallas refinery laid.

3.  MENU
    Drill              = Drilling well.
    Fin                = Request for finance.
    Lay                = Laying pipelines.
    Prod               = Build production facilities.
    Rig                = Move drilling rig.
    Seismic            = Seismic survey.
    To effect a command you are required to key underlined capitals. (First letter of menu commands D for Drill etc)

4.  PROBLEMS ENCOUNTERED
    Blow outs, tornado, drilling accidents, increase in government taxes, crude oil
    surplus and insufficient funds.

5.  HINTS ON PLAYING
    Start the game by making seismic surveys to establish plots which have good
    prospects.  You need to bid high for concessions until you have at least three.  (A bid
    of $8 - $10 million usually succeeds.)  Concessions east of hills and close to Dallas
    are worth more because development costs will be less.  You must develop a field
    in the correct order:  
		a) make seismic survey (this step is optional).  
		b) purchase concession.
		c) move Rig.
		d) Drill for oil.
		e) build Production facilities.
		f) Lay pipeline.
    You may make seismic surveys either before purchasing a concession where each
    survey costs $200,000 or make an appraisal survey with a test well for $1.2 million
    after purchasing the concession.  However, do not be put off by a 'Poor' report as it
    is still possible to strike a gusher even in a field with a 'Poor' report.  Fields are at
    greater risk from disasters, i.e. blow outs, accidents etc.  the more they are
    developed.  It is usually best not to risk the first field which is producing by laying a
    pipeline immediately.  Consequently, it is usually preferable to develop other fields,
    the cost of development can then be financed out of the revenue of the producing
    field.  Loans may be sought to finance the building of production facilites.  You will
    however be charged interest on the loans and risk factors also increase when loans
    exceed $20 million.  You will not become insolvent because before the company's 
    finances reach that state, you will have fallen prey to being taken-over by Euing
    Associates.
*/



#include <stdio.h>

#include <conio.h>

#ifdef __ZX81__
#ifndef USE_UDGS
	#undef gotoxy
	#define gotoxy(x,y) zx_setcursorpos_callee(y,x)
#endif
#endif

#ifdef __SANYO__
#include <arch/mbc200.h>
	#undef gotoxy
	#define gotoxy(x,y) mbc_setcursorpos_callee(x,y)
#endif


#include <math.h>
#include <ctype.h>

#pragma printf = "%0f%s%u"


#ifdef USE_SOUND
#include <sound.h>
#endif


#ifndef LANG_FR
#ifndef LANG_ES
#ifndef LANG_IT
#define LANG_EN 1
#endif
#endif
#endif


	#ifdef __X820__
	#define CUSTOM_CHR
	#endif

	#ifdef __SANYO__
	#define CUSTOM_CHR
	#endif

	#ifdef __SHARPMZ__
	#define CUSTOM_CHR
	#endif

	#ifdef __AQUARIUS__
	#define CUSTOM_CHR
	#endif

	#ifdef __ZX81__
	#ifndef USE_UDGS
	#define CUSTOM_CHR
	#endif
	#endif

	#ifdef __LASER500__
	#define CUSTOM_CHR
	#endif

#ifdef _WIN32

#include <windows.h>

#define cputs(x) printf("%s\n",x);

COORD GetConsoleCursorPosition(HANDLE hConsoleOutput)
{
    CONSOLE_SCREEN_BUFFER_INFO cbsi;
    if (GetConsoleScreenBufferInfo(hConsoleOutput, &cbsi))
    {
        return cbsi.dwCursorPosition;
    }
    else
    {
        // The function failed. Call GetLastError() for details.
        COORD invalid = { 0, 0 };
        return invalid;
    }
}

int wherey()
{
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD c = GetConsoleCursorPosition(h);
	return (c.Y);
}

void gotoxy(int x,int y)
{
	//    printf("%c[%d;%df",0x1B,y,x);

	COORD c = { x, y };
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(h, c); 
}

/*
 0 -> BLACK
 1 -> BLUE
 2 -> GREEN
 3 -> CYAN
 4 -> RED
 5 -> MAGENTA
 6 -> BROWN
 7 -> LIGHTGRAY
 8 -> DARKGRAY
 9 -> LIGHTBLUE
10 -> LIGHTGREEN
11 -> LIGHTCYAN
12 -> LIGHTRED
13 -> LIGHTMAGENTA
14 -> YELLOW
15 -> WHITE
*/

void textcolor(int x)
{
SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), x +7*16);
	//printf ("%c[%um%c",27,30+x);
}

void textbackground(int x)
{
	//printf ("%c[%um%c",27,40+x);
}

int getk () {
	if (_kbhit())
		return (_getch());
	return (0);
}

#define t_delay(x) usleep(x/10)

void clear_screen() {
	//system("color f0");
	system("cls");
	system("mode con: cols=32 lines=24");
};
//	textbackground(15); textcolor(0);
//	system("mode con: cols=32 lines=24");
//	printf ("%c[%um%c[%um",27,47,27,30);
//	printf ("%c[2J",27);
//	system("cls");
//	for (int x=0; x<=24; x++) {
//		gotoxy(x,0);
//		cputs ("                                ");
//	}
//
//}

#endif



#ifdef __SANYO__
	#define clear_screen() putch(26)
#else
#ifndef _WIN32
	#define clear_screen() putch(12)
#endif
#endif



char opponent[] = "EWING ASSOCIATES";
char opponent_short[] = "EWING";

char brd[] = "ABCDEFGHIJKLMN";

#ifdef USE_UDGS
char trees[] = "\200  \200\200 \200 ";
#else
	#ifdef CUSTOM_CHR

		#ifdef __X820__
			char trees[] = "\003  \003\003 \003 ";
		#endif
		#ifdef __SANYO__
			char trees[] = "\347  \347\347 \347 ";
		#endif
		#ifdef __SHARPMZ__
			char trees[] = "\226  \226\226 \226 ";
		#endif
		#ifdef __AQUARIUS__
				char trees[] = "\323  \323\323 \323 ";
		#endif
		#ifdef __ZX81__
				char trees[] = "*  ** * ";
		#endif
		#ifdef __LASER500__
				char trees[] = "\213\240\240\213\213\240\213\240";
		#endif
		
	#else
		char trees[] = "@ @@ @ ";
	#endif
#endif

int DF,WL;
int X,Y,Z;
int T,CS,BPD,BPDA,CON;

int AA[16][16];
int DD[16][16];
int TT;
int CC;

float CP,LA,NA,LN,RW;
float PR,PL,PP,PIP,PC;
float MT,JJ;
float CPL,TX,RVF,REV;
float V;

int RF,CB,LS;
char IS,JS,JJS;
int Z,U,UU;
int PO,WW,RX;



#ifdef GRAPHICS
#include <graphics.h>
char logo[] = {
//64, 24,
64, 22,
//  0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 ,
  0xFE , 0x03 , 0xE1 , 0xC0 , 0xE0 , 0x0F , 0x81 , 0xF8 ,
  0x83 , 0x02 , 0x21 , 0x40 , 0xA0 , 0x08 , 0x83 , 0x0C ,
  0xB9 , 0x02 , 0x21 , 0x40 , 0xA0 , 0x0A , 0x82 , 0xF4 ,
  0xA5 , 0x82 , 0xA1 , 0x40 , 0xA0 , 0x12 , 0x45 , 0x9A ,
  0xA4 , 0x84 , 0x91 , 0x40 , 0xA0 , 0x15 , 0x45 , 0x0A ,
  0xA2 , 0x85 , 0x51 , 0x40 , 0xA0 , 0x15 , 0x45 , 0x0E ,
  0xA2 , 0x85 , 0x51 , 0x40 , 0xA0 , 0x15 , 0x45 , 0x00 ,

  0xA2 , 0x85 , 0x51 , 0x40 , 0xA0 , 0x15 , 0x45 , 0x00 ,
  0xA2 , 0x85 , 0x51 , 0x40 , 0xA0 , 0x15 , 0x45 , 0x80 ,
  0xA2 , 0x85 , 0x51 , 0x40 , 0xA0 , 0x15 , 0x46 , 0xC0 ,
  0xA2 , 0x8A , 0x29 , 0x40 , 0xA0 , 0x28 , 0xA3 , 0x30 ,
  0xA2 , 0x8A , 0x29 , 0x40 , 0xA0 , 0x28 , 0xA0 , 0xD8 ,
  0xA2 , 0x8A , 0x29 , 0x40 , 0xA0 , 0x28 , 0xA0 , 0x6C ,
  0xA2 , 0x8A , 0x29 , 0x40 , 0xA0 , 0x28 , 0xA0 , 0x32 ,
  0xA2 , 0x8B , 0xE9 , 0x40 , 0xA0 , 0x2F , 0xA0 , 0x1A ,

  0xA2 , 0x90 , 0x05 , 0x40 , 0xA0 , 0x40 , 0x10 , 0x0A ,
  0xA2 , 0x97 , 0xF5 , 0x40 , 0xA0 , 0x5F , 0xD7 , 0x0A ,
  0xA4 , 0x94 , 0x15 , 0x40 , 0xA0 , 0x50 , 0x55 , 0x0A ,
  0xA5 , 0x94 , 0x15 , 0x40 , 0xA0 , 0x50 , 0x55 , 0x9A ,
  0xB9 , 0x14 , 0x15 , 0x7E , 0xBF , 0x50 , 0x52 , 0xF4 ,
  0x83 , 0x14 , 0x15 , 0x02 , 0x81 , 0x50 , 0x53 , 0x0C ,
  0xFE , 0x1C , 0x1D , 0xFE , 0xFF , 0x70 , 0x71 , 0xF8 ,
  0x40 , 0xE2 , 0xA7 , 0xED , 0xAB , 0xED , 0xBB , 0xFF 
  //0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
  };
#endif


#ifdef USE_UDGS
#include <sys/ioctl.h>

static unsigned char udgs[] = {
 28,60,62,60,28,8,8,24,
 255,255,195,195,195,195,255,255,
 24,24,28,24,24,24,24,60,
 28,60,60,28,28,28,28,8,
 1,65,65,65,65,255,255,255,
 0,0,255,0,0,255,0,0,
 36,36,36,36,36,36,36,36
};
#endif



void quick_pause() {
#ifdef ALT_DELAY
	t_delay (50000);
#else
	t_delay (62000);
	t_delay (62000);
	t_delay (62000);
#endif
}


void short_pause() {
#ifdef ALT_DELAY
int my_delay;
	for (my_delay=0; my_delay<20; my_delay++)
		quick_pause();
#else
	sleep(2);
#endif
}


void long_pause() {
	short_pause();
	short_pause();
}


char total[14];

void dj(float VV){
#ifdef VT_COLORS
		textcolor(0);
#endif
	sprintf(total,"  %1.1f",VV);
#if defined(__ZX81__) | defined(__SANYO__) | defined(__V1050__)
/*
	ld  hl,$1821       ; (33,24) = top left screen posn
	ld  de,(COLUMN)
	and a
	sbc hl,de
	ld  (COLUMN),hl
*/
	#ifdef CUSTOM_CHR
		extern int zx_curs_pos @16441;	//X,Y
		gotoxy(32-strlen(total),24-(zx_curs_pos>>8));
	#else
		gotoxy(32-strlen(total),wherey());
	#endif
#else
	#ifndef YSHIFT
		gotoxy(32-strlen(total),wherey());
	#else
		gotoxy(32-strlen(total),wherey()-1);
	#endif
#endif
	cputs(total);
#ifdef VT_COLORS
		textcolor(1);
#endif
};


void balance_sheet(){

#ifdef VT_COLORS
	textbackground(14);
	textcolor(1);
#endif

#ifdef LANG_ES
		gotoxy(18,0);
		cputs("DATOS FINANZAS");
#endif

#ifdef LANG_EN
		gotoxy(19,0);
		cputs("BALANCE SHEET");
#endif

#ifdef LANG_IT
		gotoxy(20,0);
		cputs("BILANCIO");
#endif

#ifdef LANG_FR
		gotoxy(19,0);
		cputs("FINANCE ETAT");
#endif



#ifdef VT_COLORS
	textbackground(15);
	textcolor(1);
#endif
	gotoxy(27,2);
	cputs("$M.");

#ifdef LANG_ES
		gotoxy(18,4);
		cputs("CAJA");
#endif

#ifdef LANG_EN
		gotoxy(18,4);
		cputs("CASH");
#endif

#ifdef LANG_IT
		gotoxy(18,4);
		cputs("CASSA");
#endif

#ifdef LANG_FR
		gotoxy(18,4);
		cputs("CASH");
#endif

	dj(LA);

#ifdef LANG_ES
	gotoxy(18,6);
	cputs("DEUDAS");
#endif

#ifdef LANG_EN
		gotoxy(18,8);
		cputs("LOANS");
#endif

#ifdef LANG_IT
		gotoxy(18,6);
		cputs("DEBITI");
#endif

#ifdef LANG_FR
		gotoxy(18,6);
		cputs("HYPOT");
#endif

	dj(MT);

#ifdef LANG_ES
	gotoxy(18,9);
	cputs("TOTAL");
	gotoxy(18,10);
	cputs("ACTIVO");
#endif

#ifdef LANG_EN
		gotoxy(18,9);
		cputs("TOTAL");
#endif

#ifdef LANG_IT
		gotoxy(18,9);
		cputs("TOTALE");
		gotoxy(18,10);
		cputs("ATTIVO");
#endif

#ifdef LANG_FR
		gotoxy(18,9);
		cputs("ACTIFS");
		gotoxy(18,10);
		cputs("TOTAL");
#endif


	NA=LA+PL+PIP+CON-MT;
	dj(NA);

#ifdef LANG_ES
	gotoxy(18,14);
	cputs("INGRESOS");
	gotoxy(18,15);
	cputs("AL MES");
	dj(PR);
#endif

#ifdef LANG_EN
		gotoxy(18,14);
		cputs("EARNINGS");
		//dj(NA-ST);
		dj(PR);
		/*
		gotoxy(18,11);
		cputs("EQUITY");
		gotoxy(18,12);
		cputs("STOCK");		//ACTIONS (Francais),  AZIONI (Italiano)
		dj(ST);
		*/
#endif

#ifdef LANG_IT
		gotoxy(18,14);
		cputs("RICAVO");
		gotoxy(18,15);
		cputs("MENSILE");
		dj(PR);
#endif

#ifdef LANG_FR
		gotoxy(18,14);
		cputs("P/LN");
		dj(PR);
#endif


#if defined(CUSTOM_CHR) && defined(__ZX81__)
	for (Z=0;Z<14;Z++){
		gotoxy(0,Z+2);
		putch(brd[Z]);
	 }
#endif

}

void draw_board(){
	
	gotoxy(2,0);
#ifdef VT_COLORS
		textbackground(0); textcolor(15);
#endif
	cputs(brd);
#ifdef VT_COLORS
		textbackground(15); textcolor(0);
#endif
	
	for (Z=0;Z<14;Z++){
		//MM=rand()%5;
		gotoxy(0,Z+2);

#ifdef VT_COLORS
		textbackground(0); textcolor(15);
#endif
		putch(brd[Z]);
#ifdef VT_COLORS
		textbackground(15); textcolor(0);
#endif

#if defined(CUSTOM_CHR) && defined(__ZX81__)
	zx_asciimode(0);
		putch('\205');
	zx_asciimode(1);
#else
	#ifdef USE_UDGS
			putch('\206');
	#else
			#ifdef __AQUARIUS__
				putch('\326');
			#else
				putch('|');
			#endif
	#endif
#endif


//		putch(' ');
#ifdef __LASER500__
	#ifdef VT_COLORS
		textbackground(10); textcolor(15);
	#endif
#else
#ifdef VT_COLORS
		textcolor(10);
#endif
#endif
		cputs(trees+(rand()%7));
#ifdef VT_COLORS
		textbackground(15); textcolor(0);
#endif
		gotoxy(16,Z+2);

#if defined(CUSTOM_CHR) && defined(__ZX81__)
	zx_asciimode(0);
		putch('\005');
	zx_asciimode(1);
#else
	#ifdef USE_UDGS
			putch('\206');
	#else
			#ifdef __AQUARIUS__
				putch('\326');
			#else
				putch('|');
			#endif
	#endif
#endif
	 }


#ifdef USE_UDGS

	#ifdef __V1050__
		 // Vertical rows
		 draw(10,16,10,194);
		 draw(13,16,13,194);
		 draw(130,16,130,194);
		 draw(133,16,133,194);

		 draw(10,16,132,16);
		 draw(10,18,132,18);
		 draw(10,192,132,192);
		 draw(10,194,132,194);
	#else
		 gotoxy(1,1);
		 cputs(" \205\205\205\205\205\205\205\205\205\205\205\205\205\205");
		 gotoxy(1,16);
		 cputs(" \205\205\205\205\205\205\205\205\205\205\205\205\205\205");
	#endif
#else
	#ifdef CUSTOM_CHR

		#ifdef __X820__
			gotoxy(0,1);
			cputs(" \023\023\023\023\023\023\023\023\023\023\023\023\023\023\023\023");
			gotoxy(1,16);
			cputs("\023\023\023\023\023\023\023\023\023\023\023\023\023\023\023\023");
		#endif

		#ifdef __SANYO__
			gotoxy(0,1);
			cputs(" \234\350\350\350\350\350\350\350\350\350\350\350\350\350\350\237");
			gotoxy(1,16);
			cputs("\236\350\350\350\350\350\350\350\350\350\350\350\350\350\350\235");
			#ifdef GRAPHICS
				draw(12,18,12,160);
				draw(132,18,132,160);
			#endif
		#endif

		#ifdef __SHARPMZ__
			gotoxy(0,1);
			//cputs(" \254\204\204\204\204\204\204\204\204\204\204\204\204\255");
			cputs(" \246\204\204\204\204\204\204\204\204\204\204\204\204\204\204\234");
			gotoxy(1,16);
			//cputs(" \200\200\200\200\200\200\200\200\200\200\200\200\200\200");
			cputs("\277\204\204\204\204\204\204\204\204\204\204\204\204\204\204\276");
		#endif

		#ifdef __AQUARIUS__
			gotoxy(0,1);
			cputs(" \336\254\254\254\254\254\254\254\254\254\254\254\254\254\254\316");
			gotoxy(1,16);
			cputs("\317\254\254\254\254\254\254\254\254\254\254\254\254\254\254\337");
		#endif

		#ifdef __ZX81__
			zx_asciimode(0);
			gotoxy(2,1);
			printf("\203\203\203\203\203\203\203\203\203\203\203\203\203\203");
			gotoxy(2,16);
			printf("\003\003\003\003\003\003\003\003\003\003\003\003\003\003");
			zx_asciimode(1);
		#endif

		#ifdef __LASER500__
			gotoxy(1,1);
			#ifdef VT_COLORS
				textbackground(0); textcolor(15);
			#endif
			cputs("\200\201\201\201\201\201\201\201\201\201\201\201\201\201\201\202");
			gotoxy(1,16);
			cputs("\206\207\207\207\207\207\207\207\207\207\207\207\207\207\207\210");
			#ifdef VT_COLORS
				textbackground(15); textcolor(0);
			#endif
		#endif

	#else
		gotoxy(1,1);
		cputs("================");
		gotoxy(1,16);
		cputs("================");
	#endif
#endif

	 gotoxy(15,2);
	 putch('D');
	 
	 balance_sheet();
 }


char blnk[]="                                        ";
char input[20];


void clr_part() {
	gotoxy(0,18);
	cputs(blnk);
	gotoxy(0,19);
	cputs(blnk);
	gotoxy(0,20);
	cputs(blnk);
	gotoxy(0,21);
	cputs(blnk);
	gotoxy(0,22);
	printf("%s",blnk);
	gotoxy(0,18);
}

void clear() {
	gotoxy(0,17);
	cputs(blnk);
	clr_part();
}


void music() {
#ifdef USE_SOUND
	synth_phase(0x2212);
	bit_play("C4G-GCE");
	bit_play("DE");
	bit_play("C4G-");
	synth_play("C4AG");
	synth_play("EF");
	synth_play("G5");
	synth_play("DG7");
#else
	long_pause();
#endif
}


void sound_good(){
#ifdef USE_SOUND
	for (Z=1; Z<28; Z++) {
		bit_beep(6,1000-Z*Z);
		bit_beep(6,1200-Z*Z);
		bit_beep(6,900-Z*Z);
	}
#else
	short_pause();
#endif
}


void sound_oil(){
#ifdef USE_SOUND
	sound_good();
	for (Z=1; Z<500; Z++)
		bit_beep(3,10+rand()%400);
#else
	short_pause();
#endif
}


void sound_ok(){
#ifdef USE_SOUND
	bit_fx(2);
#else
	short_pause();
#endif
}

void sound_bad(){
#ifdef USE_SOUND
	for (Z=32; Z>8; Z--) {
		bit_beep(6,2000-Z*Z);
		bit_beep(6,2200-Z*Z);
	}
#else
	short_pause();
#endif
}


void sound_info(){
#ifdef USE_SOUND
	for (Z=1; Z<20; Z++) {
		bit_beep(8,4000-rand()%3000);
	}
#else
	short_pause();
#endif
}



void end_game() {
	short_pause();
	NA+=CP;
	clear_screen();


#ifdef VT_COLORS
	textcolor(0);
#endif

#ifdef LANG_ES
	cputs("ESTADO OPERACIONES\n\n");
	printf("NO. DE CONCESION = %u\n",CS);
#endif
#ifdef LANG_EN
		cputs("OPERATIONS STATEMENT\n\n");
		printf("NO. OF CONCESSION = %u\n",CS);
#endif
#ifdef LANG_IT
		cputs("RISULTATO\n\n");
		printf("NO. DI CONCESSIONI = %u\n",CS);
#endif
#ifdef LANG_FR
		cputs("ETAT OPERATIONS");
		printf("NO. DE CONCESSIONS  = %u\n", CS);
#endif

#ifdef LANG_ES
	printf("NO. DE POZOS PROD. = %u\n",WL);
#endif
#ifdef LANG_EN
		printf("NO. OF PROD. WELLS = %u\n",WL);
#endif
#ifdef LANG_IT
		printf("NO. DI POZZI ATTIVI = %u\n",WL);
#endif
#ifdef LANG_FR
	printf("NO. DE PUITS PROD.  = %u\n", WL);
#endif

#ifdef LANG_ES
	printf("BARRILES POR DIA = %u,000\n", BPDA);
#endif
#ifdef LANG_EN
		printf("BARRELS PER DAY = %u,000\n", BPDA);
#endif
#ifdef LANG_IT
		printf("BARILI PER GIORNO = %u,000\n", BPDA);
#endif
#ifdef LANG_FR
		printf("BARILS PAR JOUR     = %u.000\n", BPDA);
#endif

#ifdef LANG_ES
	printf("ACTIVO TOTAL ACTUAL = %1.1fM.$\n", NA);
#endif
#ifdef LANG_EN
		printf("TOTAL ASSETS NOW = $%1.1fM.\n", NA);
#endif
#ifdef LANG_IT
		printf("ASSETTO TOTALE = $%1.1fM.\n", NA);
#endif
#ifdef LANG_FR
		printf("ACTIFS = $%1.1fM.\n", NA);
#endif


#ifdef LANG_ES
		printf("ACTIVO DE LOS %s: %1.1fM.\n", opponent_short, CP);
#endif
#ifdef LANG_EN
		printf("%s ASSETS: %1.1fM.\n", opponent, CP);
#endif
#ifdef LANG_IT
		printf("ATTIVO DI %s: %1.1fM.\n", opponent_short, CP);
#endif
#ifdef LANG_FR
		printf("ACTIFS DE %s: %1.1fM.\n", opponent_short, CP);
#endif




#ifdef VT_COLORS
	textcolor(1);
#endif

#ifdef LANG_ES
	printf("\n\nHas tardado %u meses\n", T);
#endif
#ifdef LANG_EN
		printf("\n\nYou took %u months\n", T);
#endif
#ifdef LANG_IT
		printf("\n\nHai lavorato %u mesi\n", T);
#endif
#ifdef LANG_FR
		printf("\n\nIl vous a fallu %u mois\n", T);
#endif


#ifdef LANG_ES
	printf("Nivel de dificultad=%u\n", DF);
#endif
#ifdef LANG_EN
		printf("Your level of difficulty=%u\n", DF);
#endif
#ifdef LANG_IT
		printf("Livello di difficolta'=%u\n", DF);
#endif
#ifdef LANG_FR
		printf("Votre niveau de difficulte'=%u\n", DF);
#endif

	long_pause();
	short_pause();

	exit(0);
	
}



void player_wins() {
	clear_screen();
#ifdef LANG_ES
	cputs("NOTICIAS");
	sound_good();
	gotoxy(3,11);
	cputs("EWING ASSOCIATES ELIMINADA");
#endif

#ifdef LANG_EN
		cputs("NEWS FLASH");
		sound_good();
		
		gotoxy(3,11);
		cputs("Ewing Associates TAKEN OVER");

		short_pause();
		clear_screen();

		gotoxy(14,2);
		cputs("TELEX\n");
		cputs("TO PRESIDENT OF EWING OIL\n\n\n");
		cputs("I CONFIRM THAT YOUR COMPANY\n");
		cputs("HAVE STOCKS AND/OR VOTING\nRIGHTS IN EWING OIL TOTALING 51%\nI WILL MEET YOU AT 1500HRS TODAY\nAT THE EWING TOWER DALLAS TO\nDISCUSS FUTURE ORGANISATION OF\nEWING ASSOCIATES.\n\n");
		cputs("J.R.");
#endif
		
#ifdef LANG_IT
		cputs("NEWS FLASH");
		sound_good();		
		gotoxy(2,11);
		cputs("EWING ASSOCIATES SCONFITTA!!");
#endif

#ifdef LANG_FR
		cputs("BIEN JOUE'");
		sound_good();
		gotoxy(2,11);
		printf("%s FUSIONNE\n", opponent);
		printf("ET ACTIFS DE $%1.1fM.", CP);
#endif

	music();
	long_pause();
	end_game();
}



void opponent_wins() {
	short_pause();
	clear_screen();

#ifdef LANG_ES
		short_pause();
		clear_screen();
		gotoxy(10,6);
		cputs("NOTICIAS");
		
		gotoxy(4,11);
		cputs("CONFIRMADA ABSORCION POR");
		gotoxy(8,13);
		cputs("EWING ASSOCIATES");

		music();
		clear_screen();

		gotoxy(14,2);
		cputs("TELEX\n");
		cputs("AL PRESIDENTE:\n");
		
		cputs("CONTROL DE ESA CORPORACION POR\nEWING ASSOCIATES TIENE EFECTO\nIMMEDIATO.\n\n");
		cputs(" PRESENTE SU DIMISION DE MODO INMEDIATO");
		gotoxy(18,16);
		cputs("J.R.");
		music();
 
#endif

#ifdef LANG_EN
		short_pause();
		clear_screen();
		gotoxy(10,6);

		cputs("NEWS FLASH");
		gotoxy(4,11);
		cputs("EWING ASS.TAKEOVER CONFIRMED");
		music();
		clear_screen();

		gotoxy(14,2);
		cputs("TELEX\n");
		cputs("TO THE PRESIDENT\n\n\n");
		cputs("CONTROL OF COMPANY BY EWING ASS.\nTAKES IMMEDIATE EFFECT.\n\n\n");
		cputs("I REQUIRE YOUR RESIGNATION IN\nWRITING TODAY");
		gotoxy(18,16);
		cputs("J.R.");
	
		music();
#endif

#ifdef LANG_IT
		short_pause();
		clear_screen();
		gotoxy(10,6);
		cputs("ATTENZIONE!");
		gotoxy(4,11);
		cputs("LA EWING ASS. ANNUNCIA:");
		music();
		clear_screen();

		gotoxy(14,2);
		cputs("TELEX\n");
		cputs("AL PRESIDENTE\n");
		cputs("IL CONTROLLO DELLA COMPAGNIA\nPASSA ALLA EWING ASSOCIATES\nCON EFFETTO IMMEDIATO.\n\n");
		cputs("IO ESIGO LE VOSTRE DIMISSIONI\nOGGI STESSO");
		gotoxy(18,16);
		cputs("J.R.E.");
		music();
#endif

#ifdef LANG_FR
		short_pause();
		clear_screen();
		gotoxy(10,6);

		cputs("FLASH INFO");
		gotoxy(4,11);
		cputs("PRISE DE CONTRLE CONFIRMEE");
		music();
		clear_screen();

		gotoxy(14,2);
		cputs("TELEX\n\n\n");
		cputs("EWING ASSOCIATES A GAGNE LA\n");
		cputs("BATAILLE DE L'ABSORTION.\n\n\n");
		cputs("ON DEMANDE VOTRE DEMISSION");
		gotoxy(18,16);
		cputs("J.R.");
	
		music();
#endif

	short_pause();
	end_game();
}


int is_expensive() {
	//  original logic to trigger a higher cost: "if (Y<=6)"
	if (Y<=10) return (0);
	if (X>7) return (0);
	return (1);
};


void insufficient_funds() {
		#ifdef LANG_ES
			printf("FONDOS INSUFICIENTES");
		#endif
		#ifdef LANG_EN
			printf("INSUFFICIENT FUNDS");
		#endif
		#ifdef LANG_IT
			printf("FONDI INSUFFICIENTI");
		#endif
		#ifdef LANG_FR
			printf("FONDS INSUFFISIANTS");
		#endif
		short_pause();
		clear();
}


// 3200
int setup_rig() {
	LN=(float)abs(X-2);
	// Yet another bug found in the original game :)
	//RW=(float)abs(Y-16);
	RW=(float)abs(16-Y);
	
	if (LN>RW) RW=LN;
		
	// Bug in the original game.. shouldn't it be the woods (X=COLUMNS) impacting on the cost ?
	//if (Y<=6) RW*=2.0;
	if (is_expensive()) RW*=2.0;
	PC=RW*0.2;

	// The ZX81 version had this extra check
	if ((LA-PC)<0) {
		insufficient_funds();
		return(0);
	}
	
	LA-=PC;
	return(1);
}


void auction() {
	clear();
	if (T>2) {
#ifdef LANG_ES
		gotoxy(0,17);
		printf("ESTADO OPERACIONES\n");
		printf("NO. DE CONCESION   = %u\n", CS);
		printf("NO. DE POZOS PROD. = %u\n", WL);
		printf("BARRILES POR DIA   = %u,000\n", BPDA);
		printf("ACTIVO DE LOS %s: %1.1fM.", opponent_short, CP);
#endif

#ifdef LANG_EN
		gotoxy(0,17);
		printf("OPERATIONS STATEMENT\n");
		printf("NO. OF CONCESSION  = %u\n", CS);
		printf("NO. OF PROD. WELLS = %u\n", WL);
		printf("BARRELS PER DAY    = %u,000\n", BPDA);
		printf("%s ASSETS: %1.1fM.", opponent, CP);
#endif

#ifdef LANG_IT
		gotoxy(0,17);
		printf("SITUAZIONE\n");
		printf("NO. DI CONCESSIONI  = %u\n", CS);
		printf("NO. DI POZZI ATTIVI = %u\n", WL);
		printf("BARILI PER GIORNO   = %u.000\n", BPDA);
		printf("%s ATTIVO: %1.1fM.", opponent, CP);
#endif

#ifdef LANG_FR
		gotoxy(0,17);
		printf("ETAT OPERATIONS\n");
		printf("NO. DE CONCESSIONS  = %u\n", CS);
		printf("NO. DE PUITS PROD.  = %u\n", WL);
		printf("BARILS PAR JOUR     = %u.000\n", BPDA);
		printf("ACTIFS DE %s: %1.1fM.", opponent, CP);
#endif
	
	long_pause();
	long_pause();
	clear();

	balance_sheet();
	}
	 
	gotoxy(0,22);

#ifdef VT_COLORS
	textcolor(4);
#endif

	
#ifdef LANG_ES
	cputs("VENTA CONCESION");
#endif
#ifdef LANG_EN
	cputs("CONCESSION SALES");
#endif
#ifdef LANG_IT
	cputs("CONCESSIONE IN VENDITA");
#endif
#ifdef LANG_FR
	cputs("VENTES CONCESSIONS");
#endif

#ifdef VT_COLORS
	textcolor(1);
#endif
	
	UU=1+rand()%(5-DF);

	for (U=1; U<=UU; U++) {
			X=2+rand()%13;
			Y=2+rand()%13;
			while (AA[X][Y]>=10) {
				X=2+rand()%13;
				Y=2+rand()%13;
			}


			for (Z=1; Z<=15; Z++) {
				gotoxy(X,Y);
#ifdef VT_COLORS
				textbackground(15);
#endif
				putch(' ');
				quick_pause();
				gotoxy(X,Y);
#ifdef VT_COLORS
				textbackground(4);
#endif
				putch('$');
				quick_pause();
			}

#ifdef VT_COLORS
			textbackground(15);
			textcolor(1);
#endif

			gotoxy(2,18);
			
			#ifdef LANG_ES
				printf("SU OFERTA? ");
			#endif
			#ifdef LANG_EN
					printf("YOUR BID? ");
			#endif
			#ifdef LANG_IT
					printf("OFFERTA? ");
			#endif
			#ifdef LANG_FR
					printf("VOTRE OFFRE? ");
			#endif

			gets(input);
			V=atof(input);
			
			CB=AA[X][Y]*(1+rand()%4);
			clear();
		
			if (V>LA) {
				insufficient_funds();
				gotoxy(X,Y);
				putch(' ');

			} else {

				gotoxy(0,21);

				if (V<CB) {
					#ifdef LANG_ES
						printf("OFERTA RECHAZADA");
					#endif
					#ifdef LANG_EN
							printf("BID FAILED");
					#endif
					#ifdef LANG_IT
							printf("RIFIUTATA!");
					#endif
					#ifdef LANG_FR
							printf("OFFRE REFUSEE");
					#endif
					sound_bad();
					gotoxy(X,Y);
					putch(' ');
					short_pause();
					
				} else {
					
					#ifdef LANG_ES
						printf("OFERTA SATISFACTORIA");
					#endif
					#ifdef LANG_EN
							printf("BID SUCCESSFUL");
					#endif
					#ifdef LANG_IT
							printf("ACCETTATA!");
					#endif
					#ifdef LANG_FR
							printf("OFFRE ACCEPTEE");
					#endif
					sound_good();
					CS++;
					AA[X][Y] = AA[X][Y]*10;
					gotoxy(X,Y);
		#ifdef VT_COLORS
					textcolor(5);
		#endif
				#ifdef USE_UDGS
					putch('\201');
				#else
					#ifdef CUSTOM_CHR
						#ifdef __X820__
							putch('#');
						#endif
						#ifdef __SANYO__
							putch('\225');
						#endif
						#ifdef __SHARPMZ__
							putch('\230');
							//putch('\227');
						#endif
						#ifdef __AQUARIUS__
							putch('\235');	//206, //321
						#endif
						#ifdef __ZX81__
							putch('#');
						#endif
						#ifdef __LASER500__
							putch('\214');
						#endif
					#else
						putch('#');
					#endif
				#endif
		#ifdef VT_COLORS
					textcolor(1);
		#endif
					LA-=V;
					CON+=V;
					balance_sheet();
					clr_part();
				}
		}
	}
}

char wait_keypress() {
	while (JJS=getk()!=0) {}
	while (JJS==0) JJS=getk();
	return (toupper(JJS));
}


void get_position() {
	clr_part();
	gotoxy(0,20);
	#ifdef LANG_ES
		printf("TECLEE LA LINEA: ");
	#endif
	#ifdef LANG_EN
			printf("KEY LINE NO. ");
	#endif
	#ifdef LANG_IT
			printf("LINEA :  ");
	#endif
	#ifdef LANG_FR
		printf("TAPEZ NO. DE LIGNE: ");
	#endif
		
	Y=wait_keypress();
#ifdef USE_SOUND
		bit_fx(0);
#endif
	putch(Y);
	Y=Y-'A'+2;


	#ifdef LANG_ES
		printf("\nTECLEE LA COLUMNA: ");
	#endif
	#ifdef LANG_EN
			printf("\n  KEY COLUMN NO. ");
	#endif
	#ifdef LANG_IT
			printf("\n COLONNA :  ");
	#endif
	#ifdef LANG_FR
		printf("\nTAPEZ NO. COLONNE: ");
	#endif

	X=wait_keypress();
#ifdef USE_SOUND
		bit_fx(0);
#endif
	putch(X);
	X=X-'A'+2;


	short_pause();
	clear();
	

}


//4000
int drill() {
	clear();
#ifdef USE_SOUND
		bit_fx(6);
#endif

	gotoxy(0,17);
	#ifdef LANG_ES
		cputs("PERFORANDO UN POZO");
	#endif
	#ifdef LANG_EN
			cputs("DRILLING WELL");
	#endif
	#ifdef LANG_IT
			cputs("TRIVELLAZIONE RICHIESTA");
	#endif
	#ifdef LANG_FR
		cputs("FORAGE EN BONNE VOIE");
	#endif
	
	get_position(); 
	
	if ((AA[X][Y]<100)||(AA[X][Y]>999)) {
		gotoxy(0,21);
		#ifdef LANG_ES
			cputs("NO TIENES NINGUNA TORRE AHI");
		#endif
		#ifdef LANG_EN
				cputs("DRILLING RIG NOT ON SITE");
		#endif
		#ifdef LANG_IT
				cputs("ATTREZZATURA NON INSTALLATA");
		#endif
		#ifdef LANG_FR
				cputs("DERRICK PAS SUR LE SITE");
		#endif
		sound_info();
		return(0);
	}
	TT+=5;
	DD[X][Y] = DD[X][Y]+1+rand()%6;

	gotoxy(0,18);
	#ifdef LANG_ES
		printf("PROFUNDIDAD = %u,000 PIES", DD[X][Y]);
	#endif
	#ifdef LANG_EN
			printf("DEPTH = %u,000 FEET", DD[X][Y]);
	#endif
	#ifdef LANG_IT
			printf("PROF. = %u.000 METRI", DD[X][Y]);
	#endif
	#ifdef LANG_FR
		printf("PROFUNDEUR = %u,000 METRES", DD[X][Y]);
	#endif
	
	CC=(DD[X][Y]/3)+1;
	LA-=(float)CC;

	gotoxy(0,19);
	#ifdef LANG_ES
			printf("COSTO PERFORACION = %u.0M.$",CC);
	#endif
	#ifdef LANG_EN
			printf("COST = $%u.0M.",CC);
	#endif
	#ifdef LANG_IT
			printf("COSTO = $%u.0M.",CC);
	#endif
	#ifdef LANG_FR
			printf("COUT FORAGE = $%u.0M.",CC);
	#endif


	gotoxy(0,20);
	#ifdef LANG_ES
		printf("ANALISIS = ");
	#endif
	#ifdef LANG_EN
		printf("CORE SAMPLE = ");
	#endif
	#ifdef LANG_IT
		printf("ESTRAZIONE = ");
	#endif
	#ifdef LANG_FR
		printf("ECHANTILLON = ");
	#endif

	if (DD[X][Y]>16) {
		#ifdef LANG_ES
			cputs("POZO SECO");
		#endif
		#ifdef LANG_EN
			cputs("DRY HOLE");
		#endif
		#ifdef LANG_IT
			cputs("TERRA SECCA");
		#endif
		#ifdef LANG_FR
			cputs("PUITS SEC");
		#endif
		sound_bad();
		gotoxy(X,Y);
		putch(' ');
		CON-=8;
		short_pause();
		return(0);
	}

	PO=rand()%(AA[X][Y]*DD[X][Y]);
	WW=1+(TT/10)*10;   //ZX81 version: (1+TT/10)*10


	// The ZX81 version has a different probability check
	//if (PO>((20*(1+(TT/10)))*10)) {
	if (PO>(22*WW)) {
	#ifdef VT_COLORS
		textcolor(4);
	#endif
		#ifdef LANG_ES
			cputs("YACIMIENTO PETROLEO");
		#endif
		#ifdef LANG_EN
			cputs("OIL STRIKE");
		#endif
		#ifdef LANG_IT
			cputs("PETROLIO!!");
		#endif
		#ifdef LANG_FR
			cputs("DECOUV. PETROLE");
		#endif
		sound_oil();
		gotoxy(X,Y);
	#ifdef VT_COLORS
		textcolor(0);
	#endif
		#ifdef USE_UDGS
			putch('\203');
		#else
			#ifdef CUSTOM_CHR

				#ifdef __X820__
					putch('\037');
				#endif
				#ifdef __SANYO__
					putch('\226');
				#endif
				#ifdef __SHARPMZ__
					//putch('\221');
					putch('\227');
				#endif
				#ifdef __AQUARIUS__
					putch('\325');	//212
				#endif
				#ifdef __ZX81__
					putch('P');
				#endif
				#ifdef __LASER500__
					#ifdef VT_COLORS
						textbackground(0); textcolor(15);
					#endif
					putch('\211');
					#ifdef VT_COLORS
						textbackground(15); textcolor(1);
					#endif
				#endif

			#else
					putch('P');
			#endif
		#endif
	#ifdef VT_COLORS
		textcolor(1);
	#endif
		short_pause();
		AA[X][Y] = AA[X][Y]*10;
		
		/* ZX81 has an extra "unexpected events" here (program lines 4065..4142)*/
		
		return(0);
	}

	// The ZX81 version has a different probability check
	//if ((PO<((20*(1+(TT/10)))*1)) && (PO>((14*(1+(TT/20)))*10)))  {
	if ((PO<=(22*WW)) && (PO>(14*WW))) {
		#ifdef LANG_ES
			cputs("CAPA DE ROCAS");
		#endif
		#ifdef LANG_EN
			cputs("CAP ROCK");
		#endif
		#ifdef LANG_IT
			cputs("ROCCIA DURA");
		#endif
		#ifdef LANG_FR
			cputs("ROCHE");
		#endif
		sound_ok();
	}

	// The ZX81 version has a different probability check
	//if (PO<=((14*(1+(TT/10)))*10))  {
	if (PO<=(14*WW)) {
		#ifdef LANG_ES
			cputs("TIERRA CALIZA");
		#endif
		#ifdef LANG_EN
			cputs("LIMESTONE");
		#endif
		#ifdef LANG_IT
			cputs("PIETRA CALCAREA");
		#endif
		#ifdef LANG_FR
			cputs("CALCAIRE");
		#endif
		sound_info();
	}

	short_pause();
}

//3000
int rig() {
	clear();
#ifdef USE_SOUND
		bit_fx(6);
#endif
	gotoxy(0,17);
	#ifdef LANG_ES
		cputs("PONGO UNA TORRE");
	#endif
	#ifdef LANG_EN
			cputs("MOVING RIG");
	#endif
	#ifdef LANG_IT
			cputs("INSTALLAZIONE ATTREZZATURA");
	#endif
	#ifdef LANG_FR
			cputs("DEPLACEMENT DERRICK");
	#endif
	
	get_position();
	if (AA[X][Y]<10) {
		gotoxy(0,21);
		#ifdef LANG_ES
			cputs("LA CONCESION NO ESTA COMPRADA");
		#endif
		#ifdef LANG_EN
				cputs("NOT PURCHASED");
		#endif
		#ifdef LANG_IT
				cputs("CONCESSIONE NON ACQUISTATA");
		#endif
		#ifdef LANG_FR
				cputs("CONCESSION NON ACHETE'");
		#endif
		short_pause();
		return(0);
	}
	if (AA[X][Y]>100) return(0);
	
	if (!setup_rig()) return(0);

	AA[X][Y] = AA[X][Y]*10;
	gotoxy(X,Y);

#ifdef VT_COLORS
	textcolor(1);
#endif
	#ifdef USE_UDGS
		putch('\202');
	#else
		#ifdef CUSTOM_CHR
			#ifdef __X820__
				putch('\032');
			#endif
			#ifdef __SANYO__
				putch('\373');
			#endif
			#ifdef __SHARPMZ__
				putch('\250');
			#endif
			#ifdef __AQUARIUS__
				putch('\203');
			#endif
			#ifdef __ZX81__
				putch('/');
			#endif
			#ifdef __LASER500__
				#ifdef VT_COLORS
					textbackground(1); textcolor(15);
				#endif
				putch('\215');
				#ifdef VT_COLORS
					textbackground(15); textcolor(1);
				#endif
			#endif
		#else
			putch('|');
		#endif
	#endif
#ifdef VT_COLORS
	textcolor(0);
#endif
}


//2000
void loan() {
	JJ=40.0;
#ifdef USE_SOUND
		bit_fx(6);
#endif
	while (JJ>=30.0) {
		clear();
		gotoxy(0,17);
		#ifdef LANG_ES
			cputs("FINANCIACION");
		#endif
		#ifdef LANG_EN
				cputs("FINANCE?");
		#endif
		#ifdef LANG_IT
				cputs("FINANZIAMENTO");
		#endif
		#ifdef LANG_FR
				printf("FINANCES ?");
		#endif

		#ifdef LANG_ES
			printf("\nESCRIBA EL IMPORTE EN M.$ ");
		#endif
		#ifdef LANG_EN
				printf("\nKEY AMOUNT IN $M. ");
		#endif
		#ifdef LANG_IT
				printf("\nINSERIRE L'AMMONTARE IN $M. ");
		#endif
		#ifdef LANG_FR
				printf("\nTAPEZ MONTANT EN $M. ");
		#endif

		gets(input);
		JJ=atof(input);
	}
	
	clear();
	gotoxy(0,17);

	#ifdef LANG_ES
		printf("\nPEDIDOS %1.1fM.$\n",JJ);
	#endif
	#ifdef LANG_EN
			printf("\n$%1.1fM. REQUESTED\n",JJ);
	#endif
	#ifdef LANG_IT
			printf("\n%1.1fM. RICHIESTI\n",JJ);
	#endif
	#ifdef LANG_FR
			printf("\n%1.1fM. DEMANDES\n",JJ);
	#endif

	LS=1+(rand()%3)*abs((int)JJ);

	if (LS>30) {
	#ifdef LANG_ES
		printf("PRESTAMO DENEGADO");
	#endif
	#ifdef LANG_EN
			printf("NOT SANCTIONED");
	#endif
	#ifdef LANG_IT
			printf("NON CONCESSO");
	#endif
	#ifdef LANG_FR
			printf("EMPRUNT REFUSE'");
	#endif
	sound_bad();
	} else {
		#ifdef LANG_ES
			printf("PRESTAMO CONFIRMADO");
		#endif
		#ifdef LANG_EN
				printf("LOAN SANCTIONED");
		#endif
		#ifdef LANG_IT
				printf("PRESTITO CONCESSO");
		#endif
		#ifdef LANG_FR
				printf("EMPRUNT ACCORDE'");
		#endif
		sound_good();
		LA+=JJ;  MT+=JJ;
	}
	
	short_pause();
}


void setup_revenue() {
	REV=(float)(BPD*(30+rand()%10))/1000.0;
	PR+=REV;
	gotoxy(0,19);
	#ifdef LANG_ES
		printf("NIVEL DE PRODUCCION= %u.000 B.P.D.\n",BPD);
	#endif
	#ifdef LANG_EN
		printf("PRODUCTION LEVEL= %u.000 B.P.D.\n",BPD);
	#endif
	#ifdef LANG_IT
		printf("LIVELLO PRODUTT.= %u.000 B.P.G.\n",BPD);
	#endif
	#ifdef LANG_FR
		printf("ACCROIS. PROD.= %u.000 B.P.J.\n",BPD);
	#endif


	#ifdef LANG_ES
		printf("BENEFICIO MENSUAL= %1.1fM.$",REV);
	#endif
	#ifdef LANG_EN
			printf("MONTHLY REVENUE= $%1.1fM.",REV);
	#endif
	#ifdef LANG_IT
			printf("RENDITA MENSILE= $%1.1fM.",REV);
	#endif
	#ifdef LANG_FR
			printf("REVENUS EN HAUSSE= $%1.1fM.",REV);
	#endif
	
	BPDA+=BPD;
}


//5505
void facilities_lost() {

	sound_bad();
	#ifdef USE_SOUND
		bit_fx(3);
	#endif
	short_pause();

	WL-=1;
	PL-=PP;
	PR/=2;
	balance_sheet();
	gotoxy(X,Y);
#ifdef VT_COLORS
	textcolor(0);
#endif
	#ifdef USE_UDGS
		putch('\203');
	#else
		#ifdef CUSTOM_CHR
			#ifdef __X820__
				putch('\037');
			#endif
			#ifdef __SANYO__
				putch('\226');
			#endif
			#ifdef __SHARPMZ__
				//putch('\221');
				putch('\227');
			#endif
			#ifdef __AQUARIUS__
				putch('\325');	//212
			#endif
			#ifdef __ZX81__
				putch('P');
			#endif
			#ifdef __LASER500__
				#ifdef VT_COLORS
					textbackground(0); textcolor(15);
				#endif
				putch('\211');
				#ifdef VT_COLORS
					textbackground(15); textcolor(1);
				#endif
			#endif

		#else
			putch('P');
		#endif
	#endif
#ifdef VT_COLORS
	textcolor(1);
#endif
	AA[X][Y] = 1000;
}


//5500
void all_facilities_lost() {
	PIP-=PC;
	PL=PP;
	facilities_lost();
}



//5000
int facilities() {

#ifdef USE_SOUND
		bit_fx(6);
#endif
		clear();
		gotoxy(0,17);
		#ifdef LANG_ES
			cputs("INSTALACIONES");
		#endif
		#ifdef LANG_EN
			cputs("PRODUCTION FACILITIES");
		#endif
		#ifdef LANG_IT
			cputs("COSTRUZIONE RAFFINERIA");
		#endif
		#ifdef LANG_FR
			cputs("USINES DE PRODUCTION");
		#endif
		
		get_position();

		
		if ((AA[X][Y]<1000)||(AA[X][Y]>10000)) {
			gotoxy(0,20);
			#ifdef LANG_ES
				cputs("AHI NO ESTA BROTANDO PETROLEO");
			#endif
			#ifdef LANG_EN
				cputs("OIL NOT STRUCK");
			#endif
			#ifdef LANG_IT
				cputs("IL PETROLIO LI NON SGORGA!!");
			#endif
			#ifdef LANG_FR
				cputs("FORAGE DECEVANT");
			#endif
			sound_info();
			return(0);
		}
		
		PP=20.0;
		// // Bug in the original game ?  shouldn't it be the woods (X=COLUMNS) impacting on the cost ?
		//    Perhaps it considers a higher cost because it may be on water
		//if (Y<=6) PP=30.0;
		if (is_expensive()) PP=30.0;

		if ((LA-PP)<0) {
			insufficient_funds();
			return(0);
		}
		
		LA-=PP;
		// ZX81 adds extra unexpected costs, PGM lines 5034..5042:
		BPD=20+rand()%160;
		gotoxy(0,17);
		#ifdef LANG_ES
			printf("COSTO= %1.1fM.$",PP);
		#endif
		#ifdef LANG_EN
			printf("COST= $%1.1fM.",PP);
		#endif
		#ifdef LANG_IT
			printf("COSTO= $%1.1fM.",PP);
		#endif
		#ifdef LANG_FR
			printf("COUT= $%1.1fM.",PP);
		#endif
		
		setup_revenue();
		short_pause();
		WL+=1;
		AA[X][Y] = AA[X][Y]*10;
		gotoxy(X,Y);
//#ifdef __ZX81__
//	*zx_cyx2aaddr(X,Y) = 0x72;
//#endif
	#ifdef VT_COLORS
		textcolor(4);
	#endif
		#ifdef USE_UDGS
			putch('\204');
		#else
			#ifdef CUSTOM_CHR

				#ifdef __X820__
					putch('\002');
				#endif
				#ifdef __SANYO__
					putch('\206');
				#endif
				#ifdef __SHARPMZ__
					putch('\232');
				#endif
				#ifdef __AQUARIUS__
					putch('\031');
				#endif
				#ifdef __ZX81__
					putch('$');
				#endif
				#ifdef __LASER500__
					putch('\207');
				#endif
				
			#else
				putch('$');
			#endif
		#endif
	#ifdef VT_COLORS
		textcolor(1);
	#endif
	
	PL+=PP;
	balance_sheet();
	clear();
	
	if (RF==4) {
		clr_part();
		gotoxy(0,19);
		#ifdef LANG_ES
			cputs("DERRUMBAMIENTO");
		#endif
		#ifdef LANG_EN
			cputs("BLOW OUT");
		#endif
		#ifdef LANG_IT
			cputs("TERREMOTO!!");
		#endif
		#ifdef LANG_FR
			cputs("EXPLOSION");
		#endif
		
		facilities_lost();
	}
}

//1000
void survey() {
#ifdef USE_SOUND
		bit_fx(6);
#endif
	JS='Y';
	while ((JS != 'N') && (JS != 'n')) {
#ifdef USE_SOUND
		bit_fx(1);
#endif
		gotoxy(0,17);
		#ifdef LANG_ES
			printf("ESTUDIO SISMICO");
		#endif
		#ifdef LANG_EN
			printf("SEISMIC SURVEY ?");
		#endif
		#ifdef LANG_IT
			printf("PERIZIA SISMICA ?");
		#endif
		#ifdef LANG_FR
			printf("ETUDE SISMIQUE");
		#endif
		get_position();
	
		if (AA[X][Y]<=30) {
			gotoxy(0,17);
			
			V=1.2;
			if (AA[X][Y]<10) V=0.2;

			#ifdef LANG_ES
					printf("COSTE ESTUDIO = %1.1f.$",V);
			#endif
			#ifdef LANG_EN
					printf("APPRAISAL SURVEY @ $%1.1fM.",V);
			#endif
			#ifdef LANG_IT
					printf("COSTO PERIZIA = $%1.1fM.",V);
			#endif
			#ifdef LANG_FR
					printf("COUT ETUDE EVALUATION $%1.1fM.",V);
			#endif

			LA-=V;

			gotoxy(0,18);
			#ifdef LANG_ES
				printf("ESTUDIO SISMICO MUESTRA ");
			#endif
			#ifdef LANG_EN
				printf("SEISMIC SURVEY FACTOR IS ");
			#endif
			#ifdef LANG_IT
				printf(" IL FATTORE E' ");
			#endif
			#ifdef LANG_FR
				printf("FACTEUR ETUDE SISMIQUE: ");
			#endif

			switch (AA[X][Y]) {
				case 1:
				case 10:
					#ifdef LANG_ES
						printf("POBRE");
					#endif
					#ifdef LANG_EN
							printf("POOR");
					#endif
					#ifdef LANG_IT
							printf("BASSO");
					#endif
					#ifdef LANG_FR
							printf("MEDIOCRE");
					#endif
					#ifdef USE_SOUND
						sound_bad();
					#endif
					break;

				case 2:
				case 20:
					#ifdef LANG_ES
						printf("POSITIVA");
					#endif
					#ifdef LANG_EN
							printf("FAIR");
					#endif
					#ifdef LANG_IT
							printf("SUFF.");
					#endif
					#ifdef LANG_FR
							printf("MOYEN");
					#endif
					#ifdef USE_SOUND
						sound_ok();
					#endif
					break;

				case 3:
				case 30:
					#ifdef LANG_ES
						printf("BUENA");
					#endif
					#ifdef LANG_EN
							printf("GOOD");
					#endif
					#ifdef LANG_IT
							printf("BUONO");
					#endif
					#ifdef LANG_FR
							printf("BON");
					#endif
					#ifdef USE_SOUND
						sound_good();
					#endif
					break;
				
				default:
					#ifdef LANG_ES
						printf("NO DISPONIBLE");
					#endif
					#ifdef LANG_EN
							printf("\nNOT AVAILABLE");
					#endif
					#ifdef LANG_IT
							printf("NON CALCOLABILE");
					#endif
					#ifdef LANG_FR
							printf("NON CALCULABLE");
					#endif
					
					
					#ifdef USE_SOUND
						bit_fx(7);
					#endif
					break;
				}
		} else {
				gotoxy(0,19);
				#ifdef LANG_ES
					printf("NO DISPONIBLE");
				#endif
				#ifdef LANG_EN
						printf("\nNOT AVAILABLE");
				#endif
				#ifdef LANG_IT
						printf("NON CALCOLABILE");
				#endif
				#ifdef LANG_FR
						printf("NON CALCULABLE");
				#endif
		}
		
		short_pause();
		
		clear();
		gotoxy(0,18);
		#ifdef LANG_ES
			printf("OTRO ESTUDIO SISMICO ?");
		#endif
		#ifdef LANG_EN
				printf("ANOTHER SEISMIC SURVEY ?");
		#endif
		#ifdef LANG_IT
				printf("UN'ALTRA PERIZIA SISMICA ?");
		#endif
		#ifdef LANG_FR
				printf("AUTRE ETUDE SISMIQUE ?");
		#endif
		JS=wait_keypress();
	}
}


//6000
int pipeline() {

#ifdef USE_SOUND
		bit_fx(6);
#endif
		clear();

		RX=1+rand()%DF;

		gotoxy(0,17);
		#ifdef LANG_ES
			cputs("CONSTRUCCION GASEODUCTO");
		#endif
		#ifdef LANG_EN
			cputs("LAYING PIPELINE");
		#endif
		#ifdef LANG_IT
			cputs("COSTRUZIONE OLEODOTTO");
		#endif
		#ifdef LANG_FR
			cputs("POSE DE PIPELINE");
		#endif
		
		get_position();


		if (AA[X][Y]==32700) {
			gotoxy(0,20);
			#ifdef LANG_ES
				cputs("NO DISPONIBLE");
			#endif
			#ifdef LANG_EN
				cputs("\nNOT AVAILABLE");
			#endif
			#ifdef LANG_IT
				cputs("NON DISPONIBILE");
			#endif
			#ifdef LANG_FR
				cputs("INDISPONIBLE");
			#endif
			
			#ifdef USE_SOUND
				bit_fx(7);
			#endif
			return(0);
		}


		if (AA[X][Y]<10000) {
			gotoxy(0,20);
			#ifdef LANG_ES
				cputs("NO HAY INSTALACIONES CONSTRUIDAS");
			#endif
			#ifdef LANG_EN
				cputs("PRODUCTION FACILITIES NOT BUILT");
			#endif
			#ifdef LANG_IT
				cputs("RAFFINERIA NON COSTRUITA");
			#endif
			#ifdef LANG_FR
				cputs("PUITS PAS CONSTRUIT");
			#endif
			sound_info();
			return(0);
		}
			
		if (!setup_rig()) return(0);
		BPD=25+rand()%50;
		PIP+=PC;
		
		gotoxy(0,17);
		#ifdef LANG_ES
			printf("COSTE DEL GASEODUCTO= %1.1fM.$",PC);
		#endif
		#ifdef LANG_EN
			printf("COST OF PIPELINE= $%1.1fM.",PC);
		#endif
		#ifdef LANG_IT
			printf("COSTO OLEODOTTO= $%1.1fM.",PC);
		#endif
		#ifdef LANG_FR
			printf("COUT PIPELINE= $%1.1fM.",PC);
		#endif
		
		setup_revenue();
		short_pause();
		
		if (RX==5) {
			clr_part();
			gotoxy(0,19);
			#ifdef LANG_ES
				cputs("SABOTAJE");
			#endif
			#ifdef LANG_EN
					cputs("SABOTAGE");
			#endif
			#ifdef LANG_IT
					cputs("SABOTAGGIO!!");
			#endif
			#ifdef LANG_FR
					cputs("SABOTAGE");
					//cputs("USINES ET PIPELINES DESTRUITS");
			#endif

			all_facilities_lost();
			balance_sheet();
			clear();
			return(0);
		}

		if (RX==4) {
			clr_part();
			gotoxy(0,19);
			// Same in all languages :)
			#ifdef LANG_FR
				cputs("TORNADE");
			#else
				cputs("TORNADO");
			#endif

			all_facilities_lost();
			balance_sheet();
			clear();
			return(0);
		}

		#if defined(GRAPHICS) && !defined(LOREZ)
			#ifdef __V1050__
				draw(120,30,8*X+6,12*Y+6);
			#else
			#ifdef __SANYO__
				draw(120,24,8*X+6,10*Y+6);
			#else
				draw(120,24,8*X+6,8*Y+6);
			#endif
			#endif
		#else

 		if ((17-X)>Y) {
			Z=Y;
			for (U=X; U<=14; U++) {
				if (Z>=3) Z--;
				if (AA[U][Z]<10) {
					gotoxy(U,Z);
					#ifdef __AQUARIUS__
						putch('\306');
					#else
						putch('.');
					#endif
				}
			}
		} else {
			U=X;
			for (Z=Y; Z>=3; Z--) {
				if (U<=14) U++;
				if (AA[U][Z]<10) {
					gotoxy(U,Z);
					#ifdef __AQUARIUS__
						putch('\306');
					#else
						putch('.');
					#endif
				}
			}
		}

			//gotoxy(X,Y);
			//putch('O');
		#endif

		// Let's seal it
		AA[X][Y] = 32700;
		balance_sheet();
		clear();

}


#ifdef USE_UDGS
#ifdef __CONIO_VT100
#ifndef UDG_FONT

int font_position() {
__asm
	extern ansifont
	ld hl,ansifont
__endasm
}

#endif
#endif
#endif



int main() {

#ifdef VT_COLORS
 textbackground(1); textcolor(14);
#endif

#ifdef __SANYO__
 putch(27); putch('A');  // 40 lines text mode
 //set_crtc_reg(6,0x40);   // Vert. displayed rows, alter the display resolution
   //set_crtc_reg(1,0x28);   // Horiz. displayed characters per line
#endif

#ifdef SPECTRUM 
putch(1); putch(32);
#endif

clear_screen();

#ifdef GRAPHICS
clg();
#endif


#ifdef __C128__
// Set PAPER and BORDER color
outp(0xd020,14);
outp(0xd021,14);
// Enable the ROM character set
outp(0xd018,0x84);
#endif


#ifndef NOINTRO

gotoxy(10,2);

#ifndef __C128__
#ifdef LANG_ES
cputs("BIENVENIDO A");
#else
	cputs(" WELCOME TO");
#endif
#endif

#ifdef GRAPHICS
#ifdef LOREZ
	putsprite (spr_or,0,15,logo);
#else	
	putsprite (spr_or,96,70,logo);
#endif
#else
	gotoxy(7,10);
	cputs("** D A L L A S **");
#endif

music();


#ifdef USE_UDGS
  void *param = &udgs;

	#ifdef __CONIO_VT100

		#ifdef UDG_FONT

			  extern unsigned char font_8x8_bbc_system[];
			  memcpy(font_8x8_bbc_system + 1536, param, 56);

		#else
		  
			#ifdef __ZX81__
			  memcpy(font_position() + 768, param, 56);
			#else
			  memcpy(font_position() + 1536, param, 56);
			#endif

		#endif

	#else
	  console_ioctl(IOCTL_GENCON_SET_UDGS, &param);
	#endif

#endif


#ifdef VT_COLORS
	textbackground(15);
#endif
clear_screen();

#ifdef __C128__
// Set PAPER and BORDER color
outp(0xd020,7);
outp(0xd021,7);
// Enable the redefined character set
outp(0xd018,0x8c);
#endif


#ifdef VT_COLORS
 textbackground(1); textcolor(15);
#endif

#ifdef LANG_ES
	gotoxy(11,2);
#ifdef ANSI_ATTR
	vtrendition(4);
#endif
	cputs("OBJETIVO");
#ifdef VT_COLORS
 textbackground(15); textcolor(1);
#endif
	gotoxy(0,4);
	printf("Vd. es presidente de una\ncorporacion rival de %s.\n Su meta es arruinar a los %s.", opponent, opponent_short);
#ifdef VT_COLORS
		textcolor(4);
#endif
	cputs("\nLOS FALLOS RECAERAN EN USTED\n!! RESIGNACION !!");
#endif

#ifdef LANG_EN
		gotoxy(11,2);
#ifdef ANSI_ATTR
		vtrendition(4);
#endif
		cputs("OBJECTIVE");
#ifdef VT_COLORS
		textbackground(15); textcolor(1);
#endif
		gotoxy(0,4);
		printf("You are president of a rival\ncorporation to %s.\nYour aim is to takeover %s!!", opponent, opponent_short);
#ifdef VT_COLORS
		textcolor(4);
#endif
		cputs("\nFAILURE WILL RESULT IN YOUR OWN\nRESIGNATION !!");
#endif

#ifdef LANG_IT
		gotoxy(11,2);
#ifdef ANSI_ATTR
		vtrendition(4);
#endif
		cputs("OBIETTIVO");
#ifdef VT_COLORS
		textbackground(15); textcolor(1);
#endif
		gotoxy(0,4);
		printf("Sei pressidente di una societa'\nrivale della %s.\nIl tuo fine e'battere gli %s.", opponent, opponent_short);
#ifdef VT_COLORS
		textcolor(4);
#endif
		cputs("\nSE FALLIRAI SARAI ESONERATO\nDALL'INCARICO !!");
#endif

#ifdef LANG_FR
		gotoxy(11,2);
#ifdef ANSI_ATTR
		vtrendition(4);
#endif
		cputs("OBJECTIF");
#ifdef VT_COLORS
		textbackground(15); textcolor(1);
#endif
		gotoxy(0,4);
		printf("Vous etes le president d'une\ncompanie rivale de %s.\nVotre objectif est de renverser\nles %s.", opponent, opponent_short);
#ifdef VT_COLORS
		textcolor(4);
#endif
		cputs("\nSI VOUS ECHOUEZ, VOUS SEREZ OBLIGE DE DEMISSIONER");
#endif


#endif   // NOINTRO


#ifdef VT_COLORS
	textcolor(0);
#endif
	gotoxy(5,13);

#ifdef ANSI_ATTR
	vtrendition(1);
#endif

#ifdef LANG_ES
	cputs("NIVEL DE DIFICULTAD");
#ifdef ANSI_ATTR
	vtrendition(2);
#endif
	cputs("\n\n1. Moderado\n2. Dificil\n3. Muy dificil");
#endif

#ifdef LANG_EN
		cputs("LEVEL OF DIFFICULTY");
#ifdef ANSI_ATTR
		vtrendition(2);
#endif
		cputs("\n\n1. Moderate\n2. Difficult\n3. Very difficult");
#endif

#ifdef LANG_IT
		cputs("LIVELLO DI DIFFICOLTA'");
#ifdef ANSI_ATTR
		vtrendition(2);
#endif
		cputs("\n\n1. Semplice\n2. Difficile\n3. Molto difficile");
#endif

#ifdef LANG_FR
		cputs("NIVEAU DE DIFFICULTE'");
#ifdef ANSI_ATTR
		vtrendition(2);
#endif
		cputs("\n\n1. Moyen\n2. Difficile\n3. Tres difficile");
#endif


	gotoxy(0,21);
#ifdef ANSI_ATTR
	vtrendition(5);
#endif

#ifdef LANG_ES
	cputs("ELIJA NIVEL");
#endif

#ifdef LANG_EN
		cputs("Key 1 to 3");
#endif

#ifdef LANG_IT
		cputs("Scegli 1-3");
#endif

#ifdef LANG_FR
		cputs("Votre choix");
#endif

	DF=0;
	while ((DF<1) || (DF>3)) {
		DF=getk()-'0';
		rand();	// << randomize while waiting for the keypress
	}

	T=0; CS=0; WL=0;
	TT=4; PP=20.0;
	BPD=0; BPDA=0; CON=0; 
	LA=100.0;
	MT=0.0; PIP=MT; PL=MT; PR=MT;

	clear_screen();
#if defined(__V1050__)
	gotoxy(1,2);
#else
#ifndef YSHIFT
	gotoxy(1,1);
#else
	gotoxy(1,0);
#endif
#endif

#ifdef VT_COLORS
	textbackground(1); textcolor(3);
#endif

#ifdef LANG_ES
	cputs("SIMBOLOS USADOS EN EL MAPA\n\n\n");
#ifdef VT_COLORS
	textbackground(15); textcolor(0);
#endif
	cputs("     = ARBOLES\n");
	cputs("     = CONCESION COMPRADA\n");
	cputs("     = TORRE PERFORACION\n");
	cputs("     = YACIMIENTO\n");
	cputs("     = INSTALACIONES\n");
#endif

#ifdef LANG_EN
		cputs("KEY TO SYMBOLS USED IN MAP\n\n\n");
#ifdef VT_COLORS
	textbackground(15); textcolor(0);
#endif
		cputs("     = Woods\n");
		cputs("     = Concession purchased\n");
		cputs("     = Drilling rig\n");
		cputs("     = Oil strike\n");
		cputs("     = Production facilities\n");
#endif

#ifdef LANG_IT
		cputs("SIMBOLOGIA USATA PER LA MAPPA\n\n\n");
#ifdef VT_COLORS
	textbackground(15); textcolor(0);
#endif
		cputs("     = Bosco\n");
		cputs("     = Concessione acquistata\n");
		cputs("     = Trivelle\n");
		cputs("     = Petrolio\n");
		cputs("     = Raffineria\n");
#endif

#ifdef LANG_FR
		cputs("SYMBOLES UTILISES DANS LA CARTE\n\n\n");
#ifdef VT_COLORS
	textbackground(15); textcolor(0);
#endif
		cputs("     = Foret\n");
		cputs("     = Concession achetee\n");
		cputs("     = Derrick\n");
		cputs("     = Jet d'huile\n");
		cputs("     = Installations de prod.\n");
#endif





#ifdef __ZX81__

	#ifdef USE_UDGS

			gotoxy(2,4);
		#ifdef VT_COLORS
			textcolor(10);
		#endif
			printf("\200\n");
		#ifdef VT_COLORS
			textcolor(5);
		#endif
			printf("  \201\n");
		#ifdef VT_COLORS
			textcolor(1);
		#endif
			printf("  \202\n");
		#ifdef VT_COLORS
			textcolor(0);
		#endif
			printf("  \203\n");
		#ifdef VT_COLORS
			textcolor(4);
		#endif
			printf("  \204\n");

	#else

			gotoxy(2,4);
		#ifdef VT_COLORS
			textcolor(10);
		#endif
			printf("*\n");
		#ifdef VT_COLORS
			textcolor(5);
		#endif
			printf("  #\n");
		#ifdef VT_COLORS
			textcolor(1);
		#endif
			printf("  /\n");
		#ifdef VT_COLORS
			textcolor(0);
		#endif
			printf("  P\n");
		#ifdef VT_COLORS
			textcolor(4);
		#endif
			printf("  $\n");

	#endif

#else

	#ifdef USE_UDGS

		#ifdef __V1050__
			gotoxy(2,5);
		#else
			gotoxy(2,4);
		#endif

		#ifdef VT_COLORS
			textcolor(10);
		#endif
			cputs("\200\n");
		#ifdef VT_COLORS
			textcolor(5);
		#endif
			cputs("  \201\n");
		#ifdef VT_COLORS
			textcolor(1);
		#endif
			cputs("  \202\n");
		#ifdef VT_COLORS
			textcolor(0);
		#endif
			cputs("  \203\n");
		#ifdef VT_COLORS
			textcolor(4);
		#endif
			cputs("  \204\n");

	#else

			#ifdef CUSTOM_CHR
				#ifdef __X820__
						gotoxy(2,4);
						cputs("\003\n");
						cputs("  #\n");
						cputs("  \032\n");
						cputs("  \037\n");
						cputs("  \002\n");
				#endif
				#ifdef __SANYO__
						gotoxy(2,4);
						cputs("\347\n");
						cputs("  \225\n");
						cputs("  \373\n");
						cputs("  \226\n");
						cputs("  \206\n");
				#endif
				#ifdef __SHARPMZ__
						gotoxy(2,4);
					#ifdef VT_COLORS
						textcolor(10);
					#endif
						cputs("\226\n");
					#ifdef VT_COLORS
						textcolor(5);
					#endif
						cputs("  \230\n");
						//cputs("  \227\n");
					#ifdef VT_COLORS
						textcolor(1);
					#endif
						cputs("  \250\n");
					#ifdef VT_COLORS
						textcolor(0);
					#endif
						//cputs("  \221\n");
						cputs("  \227\n");
					#ifdef VT_COLORS
						textcolor(4);
					#endif
						cputs("  \232\n");
				#endif
				
				#ifdef __AQUARIUS__
						gotoxy(2,4);
					#ifdef VT_COLORS
						textcolor(10);
					#endif
						cputs("\323\n");
					#ifdef VT_COLORS
						textcolor(5);
					#endif
						cputs("  \235\n");	//206, //321
					#ifdef VT_COLORS
						textcolor(1);
					#endif
						cputs("  \203\n");
					#ifdef VT_COLORS
						textcolor(0);
					#endif
						cputs("  \325\n");	//212
					#ifdef VT_COLORS
						textcolor(4);
					#endif
						cputs("  \031\n");
				#endif

				#ifdef __LASER500__
						gotoxy(2,4);
					#ifdef VT_COLORS
						textbackground(10); textcolor(15);
					#endif
						cputs("\213\n");
					#ifdef VT_COLORS
						textbackground(15); //textcolor(0);
						textcolor(5);
					#endif
						printf("  \214\n\n  ");
					#ifdef VT_COLORS
						textbackground(1); textcolor(15);
					#endif
						printf("\215\n\n");
					#ifdef VT_COLORS
						textbackground(15); //textcolor(0);
					#endif
						printf("  ");
					#ifdef VT_COLORS
						textbackground(0); textcolor(15);
					#endif
						cputs("\211\n");
					#ifdef VT_COLORS
						textbackground(15); //textcolor(0);
					#endif
						printf("  ");
					#ifdef VT_COLORS
						textcolor(4);
					#endif
						cputs("\207\n");
				#endif

			#else
					gotoxy(2,4);
				#ifdef VT_COLORS
					textcolor(10);
				#endif
					cputs("@\n");
				#ifdef VT_COLORS
					textcolor(5);
				#endif
					cputs("  #\n");
				#ifdef VT_COLORS
					textcolor(1);
				#endif
					cputs("  |\n");
				#ifdef VT_COLORS
					textcolor(0);
				#endif
					cputs("  P\n");
				#ifdef VT_COLORS
					textcolor(4);
				#endif
					cputs("  $\n");
			#endif

	#endif

#endif


	long_pause();
/*	
#ifdef VT_COLORS
	textbackground(15);  textcolor(0);
#endif
	clear_screen();

#ifdef LANG_ES
	gotoxy(6,10);
	cputs("ESTOY CONFIRMANDO LA");
	gotoxy(6,13);
	cputs("ESTRUCTURA GEOLOGICA");
	gotoxy(7,15);
	cputs("ESPERE UN MOMENTO.");
#endif
#ifdef LANG_EN
		gotoxy(6,10);
		cputs("GEOLOGICAL STRUCTURE");
		gotoxy(8,13);
		cputs("IS BEING SET UP");
#endif
#ifdef LANG_IT
		gotoxy(7,10);
		cputs("STO PREPARANDO LA");
		gotoxy(7,13);
		cputs("STRUTTURA GEOLOGICA");
#endif
#ifdef LANG_FR
		gotoxy(14,10);
		cputs("RELEVE");
		gotoxy(7,13);
		cputs("STRUCTURE GEOLOGIQUE");
#endif
*/
	// Set up initial opponent's capital basing on the difficulty level
	CP=15.0*DF;


	for (X=2; X<=15; X++)
		for (Y=2; Y<=15; Y++) {
			AA[X][Y] = 1+rand()%2;
			if (AA[X][Y] == 2)
				AA[X][Y] = AA[X][Y]+rand()%2;
		}
/*
#ifndef ALT_DELAY
	short_pause();
#endif
*/

#ifdef VT_COLORS
	textbackground(15); textcolor(0);
#endif

	clear_screen();

#ifdef __C128__
// Set PAPER and BORDER color
outp(0xd020,7);
outp(0xd021,7);
#endif
	
	draw_board();


	while (1) {
KY:
		clear();
		balance_sheet();
		
		RVF=1.0;
		RF=3+rand()%(11/DF);
		CPL=10.0+rand()%20;
		
		if (T>10) {
			if ((DF<3)&&(RF==4)) {
				gotoxy(0,18);
				#ifdef LANG_ES
					printf("%s\nPIERDE $%1.1f M.", opponent, CPL);
				#endif
				#ifdef LANG_EN
					printf("%s\nLOSSES $%1.1f M.", opponent, CPL);
				#endif
				#ifdef LANG_IT
					printf("LA %s\nHA PERSO $%1.1f M.", opponent, CPL);
				#endif
				#ifdef LANG_FR
					printf("%s\nA PERDU $%1.1f M.", opponent, CPL);
				#endif
				CP-=CPL;
				sound_good();
				short_pause();
				clear();
			}
			if (RF==2) {
				gotoxy(0,18);
				#ifdef LANG_ES
						cputs("EXCESO DE CRUDO");
						gotoxy(0,19);
						cputs("RENTA MES NULA");
				#endif
				#ifdef LANG_EN
						cputs("CRUDE OIL SURPLUS");
						gotoxy(0,19);
						cputs("REVENUE NIL");
				#endif
				#ifdef LANG_IT
						cputs("SURPLUS DI PETROLIO");
						gotoxy(0,19);
						cputs("RICAVI NULLI!");
				#endif
				#ifdef LANG_FR
						cputs("EXCEDENT DE PETROLE BRUT");
						gotoxy(0,19);
						cputs("PAS DE REVENUS CE MOIS");
				#endif
				RVF=0.0;
				sound_bad();
				short_pause();
				clear();
			}
			if (RF==3) {
				gotoxy(0,18);
				#ifdef LANG_ES
						cputs("COTIZACION EN BOLSA DESFAVORABLE");
						gotoxy(0,19);
						cputs("RENTA REDUCIDA A LA MITAD");
				#endif
				#ifdef LANG_EN
						cputs("UNFAVOURABLE CRUDE OIL EXCHANGE DEAL");
						gotoxy(0,19);
						cputs("REVENUE REDUCED BY HALF");
				#endif
				#ifdef LANG_IT
						cputs("MERCATO QUASI SATURO");
						gotoxy(0,19);
						cputs("RICAVI DIMEZZATI!");
				#endif
				#ifdef LANG_FR
						//cputs("EXTENDENT DE BRUT.");
						cputs("MARCHE PETROLE BRUT DEFAVORABLE");
						gotoxy(0,19);
						//cputs("REVENU DIMINUES DE MOITIE");
						cputs("REVENUS REDUIT DE MOITIE");
				#endif
				RVF=0.5;
				sound_info();
				short_pause();
				clear();
			}
			TX=1.0+(rand()%(15))/10.0;
			if (RF=7) {
				gotoxy(0,18);
				#ifdef LANG_ES
						printf("IMPUESTOS ESTATALES=$%1.1f M.$",TX);
				#endif
				#ifdef LANG_EN
						printf("GOVERNMENT TAXES=$%1.1f M.",TX);
				#endif
				#ifdef LANG_IT
						printf("TASSA GOVERNATIVA=$%1.1f M.",TX);
				#endif
				#ifdef LANG_FR
						printf("L'ETAT AUGMENTE' LES IMPOTS SUR LE PETROLE. - (COUT $%1.1f M.)",TX);
				#endif
				LA-=TX;
				sound_info();
				short_pause();
				clear();
			}
		}


		if ((NA>200.0) && (LA>80.0))  player_wins();
		
		CP+=(rand()%((int)CP/3));
		if (CP>(NA*4.0)) opponent_wins();
		LA-=(trunc(MT/2.0)/10.0);
		T++;
		if ((T==2)||(T==8)||(T==15)||(T==20)||(T==30)||(T==40)||(T==60)||(T==80)||(T==100)||(T==225)) auction();
		
		LA+=PR*RVF;
		TX=2+rand()%6;
		

		gotoxy(0,21);
#ifdef VT_COLORS
		textcolor(4);
#endif
		#ifdef LANG_ES
				printf("ESTU FINAN GASE INST PERF TORRE");
		#endif
		#ifdef LANG_EN
				printf("DRILL FIN LAY PROD. RIG SEISMIC");
		#endif
		#ifdef LANG_IT
				printf("TRIV FIN OLEO RAFF ATTR SISM");
		#endif
		#ifdef LANG_FR
				printf("F=FORAGE A=FINANCE P=PIPELINE   Q=PROD. D=DERRICK S=SISMIQUE");
		#endif
#ifdef VT_COLORS
		textcolor(1);
#endif
		

		IS=0;
		
		while (IS==0) IS=getk();
		

		#ifdef LANG_ES
			switch(IS) {
				case 'p':	//4000
				case 'P':
					drill();
					break;
				case 't':	//3000
				case 'T':
					rig();
					break;
				case 'f':	//2000
				case 'F':
					loan();
					break;
				case 'i':	//5000
				case 'I':
					facilities();
					break;
				case 'e':	//1000
				case 'E':
					survey();
					break;
				case 'g':	//6000
				case 'G':
					pipeline();
					break;
			}
		#endif

		#ifdef LANG_EN
				switch(IS) {
					case 'd':
					case 'D':
						drill();
						break;
					case 'r':
					case 'R':
						rig();
						break;
					case 'f':
					case 'F':
						loan();
						break;
					case 'p':
					case 'P':
						facilities();
						break;
					case 's':
					case 'S':
						survey();
						break;
					case 'l':
					case 'L':
						pipeline();
						break;
				}
		#endif

		#ifdef LANG_IT
				switch(IS) {
					case 't':
					case 'T':
						drill();
						break;
					case 'a':
					case 'A':
						rig();
						break;
					case 'f':
					case 'F':
						loan();
						break;
					case 'r':
					case 'R':
						facilities();
						break;
					case 's':
					case 'S':
						survey();
						break;
					case 'o':
					case 'O':
						pipeline();
						break;
				}
		#endif

		#ifdef LANG_FR
				switch(IS) {
					case 'f':
					case 'F':
						drill();
						break;
					case 'd':
					case 'D':
						rig();
						break;
					case 'a':
					case 'A':
						loan();
						break;
					case 'q':
					case 'Q':
						facilities();
						break;
					case 's':
					case 'S':
						survey();
						break;
					case 'p':
					case 'P':
						pipeline();
						break;
				}
		#endif
	}
}
