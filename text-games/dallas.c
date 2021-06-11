
// ZX Spectrum
// zcc +zx -DUSE_UDGS -DGRAPHICS  -DVT_COLORS -DUSE_SOUND -lndos -create-app -lm dallas.c
// zcc +zx -DUSE_UDGS -DGRAPHICS -DUSE_SOUND -DVT_COLORS -clib=ansi -pragma-define:ansicolumns=32 -pragma-redirect:CRT_FONT=_font_8x8_bbc_system -lndos -create-app -lm dallas.c

// MSXDOS
// zcc +msx -subtype=msxdos  -DUSE_UDGS -DGRAPHICS -DUSE_SOUND -DVT_COLORS -clib=ansi -pragma-define:ansicolumns=32 -pragma-redirect:CRT_FONT=_font_8x8_bbc_system -lndos -create-app -lm dallas.c

// ZX81 (32K)  (  POKE 16389,166  :  NEW  :  LOAD ""  )
// zcc +zx81 -DUSE_PSG -DALT_DELAY -DUSE_UDGS -DGRAPHICS -subtype=wrx -clib=wrxansi -pragma-define:ansicolumns=32 -create-app -lm -O3 dallas.c
#pragma output hrgpage = 42752

// Microbee CP/M
// zcc +cpm -clib=ansi -DUSE_SOUND -create-app -lmicrobee  -DVT_COLORS -lndos -lm dallas.c
// zcc +cpm -clib=ansi -subtype=microbee -DUSE_SOUND -create-app -DVT_COLORS -lndos -lm dallas.c

// Commodore 128
// zcc +c128 -clib=gencon -DALT_DELAY -DUSE_UDGS  -DVT_COLORS -DUSE_SOUND -lndos -create-app -lm dallas.c


// MinGW
// gcc -DVT_COLORS dallas.c


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


#include <stdio.h>

#include <conio.h>
#include <math.h>
#include <ctype.h>

#pragma printf = "%0f%s%u"


#ifdef USE_SOUND
#include <sound.h>
#endif



#ifdef _WIN32

#include <windows.h>

void cputs(char *x) {
	_cputs(x);
	printf("\n");
}

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
	system("mode con: cols=32 lines=24");
	printf ("%c[%um%c[%um",27,47,27,30);
	printf ("%c[2J",27);
	system("cls");
}

#endif



#ifdef Z80
	#define clear_screen(); putch(12);
#endif


char opponent[] = "EWING ASSOCIATES";
char opponent_short[] = "EWING";

char brd[] = "ABCDEFGHIJKLMN";

#ifdef USE_UDGS
char trees[] = "\200  \200\200 \200 ";
#else
char trees[] = "@ @@ @ ";
#endif

int DF,WL;
int X,Y,Z;
int T,CS,BPD,BPDA,CON;

int AA[16][16];
int DD[16][16];
int TT[2];
int CC[2];

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
#ifdef __ZX81__
	gotoxy(32-strlen(total),wherey());
#else
	gotoxy(32-strlen(total),wherey()-1);
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
#else
	#ifndef LANG_IT
		gotoxy(19,0);
		cputs("BALANCE SHEET");
	#else
		gotoxy(20,0);
		cputs("BILANCIO");
	#endif
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
#else
	#ifndef LANG_IT
		gotoxy(18,4);
		cputs("CASH");
	#else
		gotoxy(18,4);
		cputs("CASSA");
	#endif
#endif

	dj(LA);

#ifdef LANG_ES
	gotoxy(18,6);
	cputs("DEUDAS");
#else
	#ifndef LANG_IT
		gotoxy(18,8);
		cputs("LOANS");
	#else
		gotoxy(18,6);
		cputs("DEBITI");
	#endif
#endif

	dj(MT);

#ifdef LANG_ES
	gotoxy(18,9);
	cputs("TOTAL");
	gotoxy(18,10);
	cputs("ACTIVO");
#else
	#ifndef LANG_IT
		gotoxy(18,9);
		cputs("TOTAL");
	#else
		gotoxy(18,9);
		cputs("TOTALE");
		gotoxy(18,10);
		cputs("ATTIVO");
	#endif
#endif

	NA=LA+PL+PIP+CON-MT;
	dj(NA);

#ifdef LANG_ES

	gotoxy(18,14);
	cputs("INGRESOS");
	gotoxy(18,15);
	cputs("AL MES");
	dj(PR);

#else
	#ifndef LANG_IT

		gotoxy(18,14);
		cputs("EARNINGS");
		//dj(NA-ST);
		dj(PR);
		/*
		gotoxy(18,11);
		cputs("EQUITY");
		gotoxy(18,12);
		cputs("STOCK");
		dj(ST);
		*/

	#else

		gotoxy(18,14);
		cputs("RICAVO");
		gotoxy(18,15);
		cputs("MENSILE");
		dj(PR);

	#endif
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

#ifdef USE_UDGS
		putch('\206');
#else
		putch('|');
#endif

//		putch(' ');
#ifdef VT_COLORS
		textcolor(10);
#endif
		cputs(trees+(rand()%7));
#ifdef VT_COLORS
		textcolor(0);
#endif
		gotoxy(16,Z+2);
#ifdef USE_UDGS
		putch('\206');
#else
		putch('|');
#endif
	 }


#ifdef USE_UDGS
	 gotoxy(1,1);
	 cputs(" \205\205\205\205\205\205\205\205\205\205\205\205\205\205");
	 gotoxy(1,16);
	 cputs(" \205\205\205\205\205\205\205\205\205\205\205\205\205\205");
#else
	 gotoxy(1,1);
	 cputs("================");
	 gotoxy(1,16);
	 cputs("================");
#endif

	 gotoxy(15,2);
	 putch('D');
	 
	 balance_sheet();
 
 }


char blnk[]="                                ";
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
	NA+=CP;
	clear_screen();


#ifdef VT_COLORS
	textcolor(0);
#endif

#ifdef LANG_ES
	cputs("ESTADO OPERACIONES\n\n");
	printf("NO. DE CONCESION = %u\n",CS);
#else
	#ifndef LANG_IT
		cputs("OPERATIONS STATEMENT\n\n");
		printf("NO. OF CONCESSION = %u\n",CS);
	#else
		cputs("RISULTATO\n\n");
		printf("NO. DI CONCESSIONI = %u\n",CS);
	#endif
#endif

#ifdef LANG_ES
	printf("NO. DE POZOS PROD. = %u\n",WL);
#else
	#ifndef LANG_IT
		printf("NO. OF PROD. WELLS = %u\n",WL);
	#else
		printf("NO. DI POZZI ATTIVI = %u\n",WL);
	#endif
#endif

#ifdef LANG_ES
	printf("BARRILES POR DIA = %u,000\n", BPDA);
#else
	#ifndef LANG_IT
		printf("BARRELS PER DAY = %u,000\n", BPDA);
	#else
		printf("BARILI PER GIORNO = %u,000\n", BPDA);
	#endif
#endif

#ifdef LANG_ES
	printf("ACTIVO TOTAL ACTUAL = %1.1fM.$\n", NA);
#else
	#ifndef LANG_IT
		printf("TOTAL ASSETS NOW = $%1.1fM.\n", NA);
	#else
		printf("ASSETTO TOTALE = $%1.1fM.\n", NA);
	#endif
#endif

#ifdef VT_COLORS
	textcolor(1);
#endif

#ifdef LANG_ES
	printf("\n\nHAS TARDADO %u MESES\n", T);
#else
	#ifndef LANG_IT
		printf("\n\nYou took %u months\n", T);
	#else
		printf("\n\nHai lavorato %u mesi\n", T);
	#endif
#endif

#ifdef LANG_ES
	printf("NIVEL DE DIFICULTAD=%u\n", DF);
#else
	#ifndef LANG_IT
		printf("Your level of difficulty=%u\n", DF);
	#else
		printf("Livello di difficolta'=%u\n", DF);
	#endif
#endif

	exit(0);
	
}


void player_wins() {
	clear_screen();
#ifdef LANG_ES
	cputs("NOTICIAS");
	sound_good();		
	gotoxy(3,11);
	cputs("EWING ASSOCIATES ELIMINADA");

#else
	#ifndef LANG_IT
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
		cputs("HAVE STOCKS AND/OR VOTINGRIGHTS IN EWING OIL TOTALING 51% . I WILL MEET YOU AT 1500HRS TODAYAT THE EWING TOWER DALLAS TO DISCUSS FUTURE ORGANISATION OF EWING ASSOCIATES.");
		cputs("J.R.");
		
	#else
		cputs("NEWS FLASH");
		sound_good();		
		gotoxy(2,11);
		cputs("EWING ASSOCIATES SCONFITTA!!");

	#endif
#endif

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
		cputs("EWING ASSOCIATED");

		music();
		clear_screen();

		gotoxy(14,2);
		cputs("TELEX\n");
		cputs("AL PRESIDENTE:\n");
		
		cputs("CONTROL DE ESA CORPORACION POR EWING ASSOCIATED TIENE EFECTO IMMEDIATO.\n\n");
		cputs(" PRESENTE SU DIMISION DE MODO INMEDIATO");
		gotoxy(18,16);
		cputs("J.R.");
		music();
 

#else
	#ifndef LANG_IT
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
		cputs("CONTROL OF COMPANY BY EWING ASS.TAKES IMMEDIATE EFFECT.\n\n\n");
		cputs("I REQUIRE YOUR RESIGNATION IN     WRITING TODAY");
		gotoxy(18,16);
		cputs("J.R.");
	
		music();
	#else
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
		cputs("IL CONTROLLO DELLA COMPAGNIA PASSA ALLA EWING ASS CON EFFETTO IMMEDIATO.\n\n");
		cputs("IO ESIGO LE VOSTRE DIMISSIONI OGGI STESSO");
		gotoxy(18,16);
		cputs("J.R.E.");
		music();
 

	#endif
#endif
	short_pause();
	end_game();
}



// 3200
int setup_rig() {
	LN=(float)abs(X-2);
	RW=(float)abs(Y-16);
	
	if (LN>RW) {
		LA-=(LN*0.2);
		PC=LN*0.2;
		return(0);
	}
	
	if (Y<=6) RW*=2.0;
	
	PC=RW*0.2;
	LA-=PC;
}


void auction() {
	clear();
	if (T>2) {
#ifdef LANG_ES

		gotoxy(0,17);
		cputs("ESTADO OPERACIONES");
		printf("NO. DE CONCESION   = %u\n", CS);
		printf("NO. DE POZOS PROD. = %u\n", WL);
		printf("BARRILES POR DIA   = %u,000\n", BPDA);
		printf("ACTIVO DE LOS %s: %1.1fM.", opponent_short, CP);

#else
		#ifndef LANG_IT

			gotoxy(0,17);
			cputs("OPERATIONS STATEMENT");
			printf("NO. OF CONCESSION  = %u\n", CS);
			printf("NO. OF PROD. WELLS = %u\n", WL);
			printf("BARRELS PER DAY    = %u,000\n", BPDA);
			printf("%s ASSETS: %1.1fM.", opponent, CP);

		#else

			gotoxy(0,17);
			cputs("SITUAZIONE");
			printf("NO. DI CONCESSIONI  = %u\n", CS);
			printf("NO. DI POZZI ATTIVI = %u\n", WL);
			printf("BARILI PER GIORNO   = %u.000\n", BPDA);
			printf("%s ATTIVO: %1.1fM.", opponent, CP);

		#endif
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
#else
	#ifndef LANG_IT
		cputs("CONCESSION SALES");
	#else
		cputs("CONCESSIONE IN VENDITA");
	#endif
#endif

#ifdef VT_COLORS
	textcolor(1);
#endif
	
	UU=1+rand()%(4-DF);

	for (U=1; U<=UU; U++) {
			X=2+rand()%13;
			Y=2+rand()%13;

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
		#else
			#ifndef LANG_IT
				printf("YOUR BID? ");
			#else
				printf("OFFERTA? ");
			#endif
		#endif

			gets(input);
			V=atof(input);
			
			CB=AA[X][Y]*(1+rand()%4);
			clear();
		
		if (V>LA) {
		#ifdef LANG_ES
			printf("FONDOS INSUFICIENTES");
		#else
			#ifndef LANG_IT
				printf("INSUFFICIENT FUNDS");
			#else
				printf("FONDI INSUFFICIENTI");
			#endif
		#endif
			gotoxy(X,Y);
			putch(' ');
			short_pause();

		} else {

			gotoxy(0,21);

			if (V<CB) {
				#ifdef LANG_ES
					printf("OFERTA RECHAZADA");
				#else
					#ifndef LANG_IT
						printf("BID FAILED");
					#else
						printf("RIFIUTATA!");
					#endif
				#endif
				sound_bad();
				gotoxy(X,Y);
				putch(' ');
				short_pause();
				
			} else {
				
				#ifdef LANG_ES
					printf("OFERTA SATISFACTORIA");
				#else
					#ifndef LANG_IT
						printf("BID SUCCESSFUL");
					#else
						printf("ACCETTATA!");
					#endif
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
				putch('#');
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
	#else
		#ifndef LANG_IT
			printf("KEY LINE NO. ");
		#else
			printf("LINEA :  ");
		#endif
	#endif
		
	Y=wait_keypress();
#ifdef USE_SOUND
		bit_fx(0);
#endif
	putch(Y);
	Y=Y-'A'+2;


	#ifdef LANG_ES
		printf("\nTECLEE LA COLUMNA: ");
	#else
		#ifndef LANG_IT
			printf("\n  KEY COLUMN NO. ");
		#else
			printf("\n COLONNA :  ");
		#endif
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
	#else
		#ifndef LANG_IT
			cputs("DRILLING WELL");
		#else
			cputs("TRIVELLAZIONE RICHIESTA");
		#endif
	#endif
	
	get_position(); 
	
	if ((AA[X][Y]<100)||(AA[X][Y]>999)) {
		gotoxy(0,21);
		#ifdef LANG_ES
			cputs("NO TIENES NINGUNA TORRE AHI");
		#else
			#ifndef LANG_IT
				cputs("DRILLING RIG NOT ON SITE");
			#else
				cputs("ATTREZZATURA NON INSTALLATA");
			#endif
		#endif
		sound_info();
		return(0);
	}
	TT[0]+=5;
	DD[X][Y] = DD[X][Y]+1+rand()%6;

	gotoxy(0,18);
	#ifdef LANG_ES
		printf("PROFUNDIDAD = %u,000 PIES", DD[X][Y]);
	#else
		#ifndef LANG_IT
			printf("DEPTH = %u,000 FEET", DD[X][Y]);
		#else
			printf("PROF. = %u.000 METRI", DD[X][Y]);
		#endif
	#endif
	
	CC[0]=(DD[X][Y]/3)+1;
	LA-=CC[0];

	gotoxy(0,18);
	#ifdef LANG_ES
		printf("PROFUNDIDAD = %u,000 PIES", DD[X][Y]);
	#else
		#ifndef LANG_IT
			printf("DEPTH = %u,000 FEET", DD[X][Y]);
		#else
			printf("PROF. = %u.000 METRI", DD[X][Y]);
		#endif
	#endif

	gotoxy(0,20);
	#ifdef LANG_ES
		printf("ANALISIS = ");
	#else
		#ifndef LANG_IT
			printf("CORE SAMPLE = ");
		#else
			printf("ESTRAZIONE = ");
		#endif
	#endif

	if (DD[X][Y]>20) {
		#ifdef LANG_ES
			cputs("POZO SECO");
		#else
			#ifndef LANG_IT
				cputs("DRY HOLE");
			#else
				cputs("TERRA SECCA");
			#endif
		#endif
		sound_bad();
		gotoxy(X,Y);
		putch(' ');
		CON-=8;
		short_pause();
		return(0);
	}

	PO=rand()%(AA[X][Y]*DD[X][Y]);
	WW=1+(TT[0]/10)*10;

	if (PO>(22*WW)) {
	#ifdef VT_COLORS
		textcolor(4);
	#endif
		#ifdef LANG_ES
			cputs("YACIMIENTO PETROLEO");
		#else
			#ifndef LANG_IT
				cputs("OIL STRIKE");
			#else
				cputs("PETROLIO!!");
			#endif
		#endif
		sound_oil();
		gotoxy(X,Y);
	#ifdef VT_COLORS
		textcolor(0);
	#endif
		#ifdef USE_UDGS
			putch('\203');
		#else
			putch('P');
		#endif
	#ifdef VT_COLORS
		textcolor(1);
	#endif
		short_pause();
		AA[X][Y] = AA[X][Y]*10;
		return(0);
	}

	if ((PO>=(22*WW)) && (PO>(14*WW))) {
		#ifdef LANG_ES
			cputs("CAPA DE ROCAS");
		#else
			#ifndef LANG_IT
				cputs("CAP ROCK");
			#else
				cputs("ROCCIA DURA");
			#endif
		#endif
		sound_ok();
	}

	if (PO<=(14*WW)) {
		#ifdef LANG_ES
			cputs("TIERRA CALIZA");
		#else
			#ifndef LANG_IT
				cputs("LIMESTONE");
			#else
				cputs("PIETRA CALCAREA");
			#endif
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
	#else
		#ifndef LANG_IT
			cputs("MOVING RIG");
		#else
			cputs("INSTALLAZIONE ATTREZZATURA");
		#endif
	#endif
	
	get_position();
	if (AA[X][Y]<10) {
		gotoxy(0,21);
		#ifdef LANG_ES
			cputs("LA CONCESION NO ESTA COMPRADA");
		#else
			#ifndef LANG_IT
				cputs("NOT PURCHASED");
			#else
				cputs("CONCESSIONE NON ACQUISTATA");
			#endif
		#endif
		short_pause();
		return(0);
	}
	if (AA[X][Y]>100) return(0);
	
	setup_rig();

	AA[X][Y] = AA[X][Y]*10;
	gotoxy(X,Y);

#ifdef VT_COLORS
	textcolor(1);
#endif
	#ifdef USE_UDGS
		putch('\202');
	#else
		putch('|');
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
		#else
			#ifndef LANG_IT
				cputs("FINANCE?");
			#else
				cputs("FINANZIAMENTO");
			#endif
		#endif

		#ifdef LANG_ES
			printf("\nESCRIBA EL IMPORTE EN M.$ ");
		#else
			#ifndef LANG_IT
				printf("\nKEY AMOUNT IN $M. ");
			#else
				printf("\nINSERIRE L'AMMONTARE IN $M. ");
			#endif
		#endif
		gets(input);
		JJ=atof(input);
	}
	
	clear();
	gotoxy(0,17);

	#ifdef LANG_ES
		printf("\nPEDIDOS %1.1fM.$\n",JJ);
	#else
		#ifndef LANG_IT
			printf("\n$%1.1fM. REQUESTED\n",JJ);
		#else
			printf("\n%1.1fM. RICHIESTI\n",JJ);
		#endif
	#endif
	LS=1+(rand()%3)*abs((int)JJ);
	if (LS>30) {
	#ifdef LANG_ES
		printf("PRESTAMO DENEGADO");
	#else
		#ifndef LANG_IT
			printf("NOT SANCTIONED");
		#else
			printf("NON CONCESSO");
		#endif
	#endif
	sound_bad();
	} else {
		#ifdef LANG_ES
			printf("PRESTAMO CONFIRMADO");
		#else
			#ifndef LANG_IT
				printf("LOAN SANCTIONED");
			#else
				printf("PRESTITO CONCESSO");
			#endif
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
	#else
		#ifndef LANG_IT
			printf("PRODUCTION LEVEL= %u.000 B.P.D.\n",BPD);
		#else
			printf("LIVELLO PRODUTT.= %u.000 B.P.G.\n",BPD);
		#endif
	#endif


	#ifdef LANG_ES
		printf("BENEFICIO MENSUAL= %1.1fM.$",REV);
	#else
		#ifndef LANG_IT
			printf("MONTHLY REVENUE= $%1.1fM.",REV);
		#else
			printf("RENDITA MENSILE= $%1.1fM.",REV);
		#endif
	#endif
	
	BPDA+=BPD;
}


//5505
void facilities_lost() {

	sound_bad();
	#ifdef USE_SOUND
				bit_fx(3);
	#endif

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
		putch('P');
	#endif
#ifdef VT_COLORS
	textcolor(1);
#endif
	AA[X][Y]=1000;
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
		#else
			#ifndef LANG_IT
				cputs("PRODUCTION FACILITIES");
			#else
				cputs("COSTRUZIONE RAFFINERIA");
			#endif
		#endif
		
		get_position();

		
		if ((AA[X][Y]<1000)||(AA[X][Y]>10000)) {
			gotoxy(0,20);
			#ifdef LANG_ES
				cputs("AHI NO ESTA BROTANDO PETROLEO");
			#else
				#ifndef LANG_IT
					cputs("OIL NOT STRUCK");
				#else
					cputs("IL PETROLIO LI NON SGORGA!!");
				#endif
			#endif
			sound_info();
			return(0);
		}
		
		PP=20.0;
		if (Y<=6) PP=30.0;
		if ((LA-PP)<0) {
			#ifdef LANG_ES
				cputs("FONDOS INSUFICIENTES");
			#else
				#ifndef LANG_IT
					cputs("INSUFFICIENT FUNDS");
				#else
					cputs("FONDI INSUFFICIENTI");
				#endif
			#endif
			short_pause();
			return(0);
		}
		
		LA-=PP;
		BPD=20+rand()%160;
		gotoxy(0,17);
		#ifdef LANG_ES
			printf("COSTO= %1.1fM.$",PP);
		#else
			#ifndef LANG_IT
				printf("COST= $%1.1fM.",PP);
			#else
				printf("COSTO= $%1.1fM.",PP);
			#endif
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
			putch('$');
		#endif
	#ifdef VT_COLORS
		textcolor(1);
	#endif
	
	PL+=PP;
	balance_sheet();
	clear();
	
	if (RF==4) {
		gotoxy(0,17);
		#ifdef LANG_ES
			cputs("DERRUMBAMIENTO");
		#else
			#ifndef LANG_IT
				cputs("BLOW OUT");
			#else
				cputs("TERREMOTO!!");
			#endif
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
		#else
			#ifndef LANG_IT
				printf("SEISMIC SURVEY ?");
			#else
				printf("PERIZIA SISMICA ?");
			#endif
		#endif
		get_position();

		
			if (AA[X][Y]<=30) {
				gotoxy(0,17);
				#ifdef LANG_ES
					printf("COSTE ESTUDIO @ $1.2M.");
				#else
					#ifndef LANG_IT
						printf("APPRAISAL SURVEY @ $1.2M.");
					#else
						printf("COSTO PERIZIA @ $1.2M.");
					#endif
				#endif
				LA-=1.2;

			gotoxy(0,18);
			#ifdef LANG_ES
				printf("ESTUDIO SISMICO MUESTRA ");
			#else
				#ifndef LANG_IT
					printf("SEISMIC SURVEY FACTOR IS ");
				#else
					printf(" IL FATTORE E' ");
				#endif
			#endif

			switch (AA[X][Y]) {
				case 1:
				case 10:
					#ifdef LANG_ES
						printf("POBRE");
					#else
						#ifndef LANG_IT
							printf("POOR");
						#else
							printf("BASSO");
						#endif
					#endif
					#ifdef USE_SOUND
						sound_bad();
					#endif
					break;

				case 2:
				case 20:
					#ifdef LANG_ES
						printf("POSITIVA");
					#else
						#ifndef LANG_IT
							printf("FAIR");
						#else
							printf("SUFF.");
						#endif
					#endif
					#ifdef USE_SOUND
						sound_ok();
					#endif
					break;

				case 3:
				case 30:
					#ifdef LANG_ES
						printf("BUENA");
					#else
						#ifndef LANG_IT
							printf("GOOD");
						#else
							printf("BUONO");
						#endif
					#endif
					#ifdef USE_SOUND
						sound_good();
					#endif
					break;
				
				default:
					#ifdef LANG_ES
						printf("NO DISPONIBLE");
					#else
						#ifndef LANG_IT
							printf("\nNOT AVAILABLE");
						#else
							printf("NON CALCOLABILE");
						#endif
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
				#else
					#ifndef LANG_IT
						printf("NOT AVAILABLE");
					#else
						printf("NON CALCOLABILE");
					#endif
				#endif
		}
		
		short_pause();
		
		clear();
		gotoxy(0,18);
		#ifdef LANG_ES
			printf("OTRO ESTUDIO SISMICO ?");
		#else
			#ifndef LANG_IT
				printf("ANOTHER SEISMIC SURVEY ?");
			#else
				printf("UN'ALTRA PERIZIA SISMICA ?");
			#endif
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

		RX=1+rand()%(6/DF);

		gotoxy(0,17);
		#ifdef LANG_ES
			cputs("CONSTRUCCION GASEODUCTO");
		#else
			#ifndef LANG_IT
				cputs("LAYING PIPELINE");
			#else
				cputs("COSTRUZIONE OLEODOTTO");
			#endif
		#endif
		
		get_position();

		
		if (AA[X][Y]<10000) {
			gotoxy(0,20);
			#ifdef LANG_ES
				cputs("NO HAY INSTALACIONES CONSTRUIDAS");
			#else
				#ifndef LANG_IT
					cputs("PRODUCTION FACILITIES NOT BUILT");
				#else
					cputs("RAFFINERIA NON COSTRUITA");
				#endif
			#endif
			sound_info();
			return(0);
		}
			
		setup_rig();
		BPD=25+rand()%50;
		PIP+=PC;
		
		gotoxy(0,17);
		#ifdef LANG_ES
			printf("COSTE DEL GASEODUCTO= %1.1fM.$",PC);
		#else
			#ifndef LANG_IT
				printf("COST OF PIPELINE= $%1.1fM.",PC);
			#else
				printf("COSTO OLEODOTTO= $%1.1fM.",PC);
			#endif
		#endif
		
		setup_revenue();
		short_pause();
		
		if (RX==5) {
			gotoxy(0,17);
			#ifdef LANG_ES
				cputs("SABOTAJE");
			#else
				#ifndef LANG_IT
					cputs("SABOTAGE");
				#else
					cputs("SABOTAGGIO!!");
				#endif
			#endif
			sound_bad();
			all_facilities_lost();
			balance_sheet();
			clear();
			return(0);
		}

		if (RX==4) {
			gotoxy(0,17);
			// Same in all languages :)
				cputs("TORNADO");
			sound_bad();
			all_facilities_lost();
			balance_sheet();
			clear();
			return(0);
		}

		#ifdef GRAPHICS
				draw(120,24,8*X+6,8*Y+6);
		#else
			gotoxy(X,Y);
			putch('O');
		#endif

		balance_sheet();
		clear();

}


#ifdef USE_UDGS
#ifdef __CONIO_VT100
int font_position() {
#asm
	extern ansifont
	ld hl,ansifont
#endasm
}
#endif
#endif



int main() {

		
#ifdef USE_UDGS
  void *param = &udgs;

#ifdef __CONIO_VT100
  //extern unsigned char font_8x8_bbc_system[];
  //memcpy(font_8x8_bbc_system + 1536, param, 56);
  
#ifdef __ZX81__
  memcpy(font_position() + 768, param, 56);
#else
  memcpy(font_position() + 1536, param, 56);
#endif

#else
  console_ioctl(IOCTL_GENCON_SET_UDGS, &param);
#endif

#endif


#ifdef _WIN32
	system(" ");
#endif

#ifdef VT_COLORS
 textbackground(1); textcolor(14);
#endif

putch(1);putch(32);
clear_screen();

#ifdef __C128__
outp(0xd020,7);
outp(0xd021,7);
#endif


#ifdef GRAPHICS
clg();
#endif
gotoxy(10,2);

#ifdef LANG_ES
cputs("BIENVENIDO A");
#else
	cputs(" WELCOME TO");
#endif

#ifdef GRAPHICS
	putsprite (spr_or,96,70,logo);
#else
	gotoxy(7,10);
	cputs("** D A L L A S **");
#endif

music();

#ifdef VT_COLORS
	textbackground(15);
#endif
clear_screen();

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
	printf("VD. ES PRESIDENTE DE UNA CORPORACION RIVAL DE %s. SU META ES ARRUINAR A LOS %s.", opponent, opponent_short);
	cputs("\nLOS FALLOS RECAERAN EN USTED    !! RESIGNACION !!");
#else
	#ifndef LANG_IT
		gotoxy(11,2);
#ifdef ANSI_ATTR
		vtrendition(4);
#endif
		cputs("OBJECTIVE");
#ifdef VT_COLORS
		textbackground(15); textcolor(1);
#endif
		gotoxy(0,4);
		printf("You are president of a rival    corporation to %s. Your aim is to takeover %s!!", opponent, opponent_short);
		cputs("\nFAILURE WILL RESULT IN YOUR OWN RESIGNATION !!");
	#else
		gotoxy(11,2);
#ifdef ANSI_ATTR
		vtrendition(4);
#endif
		cputs("OBIETTIVO");
#ifdef VT_COLORS
		textbackground(15); textcolor(1);
#endif
		gotoxy(0,4);
		printf("Sei pressidente di una societa'  rivale della %s.  Il tuo fine e'battere gli %s.", opponent, opponent_short);
		cputs("\nSE FALLIRAI SARAI ESONERATO     DALL'INCARICO !!");
	#endif
#endif

#ifdef VT_COLORS
	textcolor(0);
#endif
	gotoxy(5,13);
#ifdef LANG_ES
#ifdef ANSI_ATTR
	vtrendition(1);
#endif
	cputs("NIVEL DE DIFICULTAD");
#ifdef ANSI_ATTR
	vtrendition(2);
#endif
	cputs("\n\n1. Moderado\n2. Dificil\n3. Muy dificil");
#else
	#ifndef LANG_IT
#ifdef ANSI_ATTR
		vtrendition(1);
#endif
		cputs("LEVEL OF DIFFICULTY");
#ifdef ANSI_ATTR
		vtrendition(2);
#endif
		cputs("\n\n1. Moderate\n2. Difficult\n3. Very difficult");
	#else
#ifdef ANSI_ATTR
		vtrendition(1);
#endif
		cputs("LIVELLO DI DIFFICOLTA'");
#ifdef ANSI_ATTR
		vtrendition(2);
#endif
		cputs("\n\n1. Semplice\n2. Difficile\n3. Molto difficile");
	#endif
#endif


	gotoxy(0,21);
#ifdef ANSI_ATTR
	vtrendition(5);
#endif
#ifdef LANG_ES
	cputs("ELIJA NIVEL");
#else
	#ifndef LANG_IT
		cputs("Key 1 to 3");
	#else
		cputs("Scegli 1-3");
	#endif
#endif

	DF=0;
	while ((DF<1) || (DF>3)) {
		DF=getk()-'0';
		rand();	// << randomize while waiting for the keypress
	}

	T=0; CS=0; WL=0;
	TT[0]=4; PP=20.0;
	BPD=0; BPDA=0; CON=0; 
	LA=100.0;
	MT=0.0; PIP=MT; PL=MT; PR=MT;

	clear_screen();
#ifdef __ZX81__
	gotoxy(2,1);
#else
	gotoxy(2,0);
#endif

#ifdef LANG_ES
#ifdef VT_COLORS
	textbackground(1); textcolor(3);
#endif
	cputs("SIMBOLOS USADOS EN EL MAPA\n\n\n");
#ifdef VT_COLORS
	textbackground(15); textcolor(0);
#endif
	cputs("     = ARBOLES\n");
	cputs("     = CONCESION COMPRADA\n");
	cputs("     = TORRE PERFORACION\n");
	cputs("     = YACIMIENTO\n");
	cputs("     = INSTALACIONES\n");
#else
	#ifndef LANG_IT
#ifdef VT_COLORS
		textbackground(1); textcolor(3);
#endif
		cputs("KEY TO SYMBOLS USED IN MAP\n\n\n");
#ifdef VT_COLORS
	textbackground(15); textcolor(0);
#endif
		cputs("     = Woods\n");
		cputs("     = Concession purchased\n");
		cputs("     = Drilling rig\n");
		cputs("     = Oil strike\n");
		cputs("     = Production facilities\n");
	#else
#ifdef VT_COLORS
		textbackground(1); textcolor(3);
#endif
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
		printf("@\n");
	#ifdef VT_COLORS
		textcolor(5);
	#endif
		printf("  #\n");
	#ifdef VT_COLORS
		textcolor(1);
	#endif
		printf("  |\n");
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

		gotoxy(2,4);
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


	long_pause();
	
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
#else
	#ifndef LANG_IT
		gotoxy(6,10);
		cputs("GEOLOGICAL STRUCTURE");
		gotoxy(8,13);
		cputs("IS BEING SET UP");
	#else
		gotoxy(7,10);
		cputs("STO PREPARANDO LA");
		gotoxy(7,13);
		cputs("STRUTTURA GEOLOGICA");
	#endif
#endif

	// Set up initial capital basing on the difficulty level
	CP=10.0*DF;

#ifdef VT_COLORS
	textbackground(15); textcolor(0);
#endif

	for (X=2; X<=15; X++)
		for (Y=2; Y<=15; Y++) {
			AA[X][Y] = 1+rand()%2;
			if (AA[X][Y] == 2)
				AA[X][Y] = AA[X][Y]+rand()%2;
		}

	short_pause();
	clear_screen();
	
	draw_board();


	while (1) {
KY:
		clear();
		balance_sheet();
		
		RVF=1.0;
		RF=3+rand()%(11/DF);
		CPL=10.0+rand()%20;
		
		if (T>10) {
			if ((DF>1)&&(RF==4)) {
				gotoxy(0,18);
				#ifdef LANG_ES
					printf("%s\nPIERDE $%1.1f M.", opponent, CPL);
				#else
					#ifndef LANG_IT
						printf("%s\nLOSSES $%1.1f M.", opponent, CPL);
					#else
						printf("LA %s\nHA PERSO $%1.1f M.", opponent, CPL);
					#endif
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
				#else
					#ifndef LANG_IT
						cputs("CRUDE OIL SURPLUS");
						gotoxy(0,19);
						cputs("REVENUE NIL");
					#else
						cputs("SURPLUS DI PETROLIO");
						gotoxy(0,19);
						cputs("RICAVI NULLI!");
					#endif
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
				#else
					#ifndef LANG_IT
						cputs("UNFAVOURABLE CRUDE OIL EXCHANGE DEAL");
						gotoxy(0,19);
						cputs("REVENUE REDUCED BY HALF");
					#else
						cputs("MERCATO QUASI SATURO");
						gotoxy(0,19);
						cputs("RICAVI DIMEZZATI!");
					#endif
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
				#else
					#ifndef LANG_IT
						printf("GOVERNMENT TAXES=$%1.1f M.",TX);
					#else
						printf("TASSA GOVERNATIVA=$%1.1f M.",TX);
					#endif
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
		#else
			#ifndef LANG_IT
				printf("DRILL FIN LAY PROD. RIG SEISMIC");
			#else
				printf("TRIV FIN OLEO RAFF ATTR SISM");
			#endif
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
		#else
			#ifndef LANG_IT

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
			#else
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
		#endif
	}
}


