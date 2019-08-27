
/*
		MazezaM

	Copyright (C) 2002
	Malcolm  Tyrrell
	tyrrelmr@cs.tcd.ie

	Amiga, GameBoy, Oric, Apple 2,
	Commodore 64 versions (C) 2003-2008 by
	Ventzislav Tzvetkov
	http://drhirudo.hit.bg

	CP/M version (C) 2004-2008
	Ventzislav  Tzvetkov
	http://drhirudo.hit.bg 

	z88dk version by Stefano Bodrato (www.z88dk.org)
	Build examples (NOT ONLY CP/M!!):

	zcc +cpm -DVT52 -DSCREENWIDTH=40 mazezam.c -o mazezam -create-app -O3
	zcc +zx -create-app -clib=ansi -pragma-define:ansicolumns=42 -lndos mazezam.c
	zcc +zx81 -create-app -subtype=wrx -clib=wrxansi -pragma-define:ansicolumns=42 mazezam.c
	
	

	This code may be used
	and distributed under
	the terms of the GNU
	General Public Licence.

*/

#pragma output hrgpage = 36096

#include <stdio.h> // We use printf(); putchar();
#include <string.h> // strlen for the Level name centering.

#ifdef VT52
//#define SCREENWIDTH 40 // size of screen horizontal. At least 28!
#else

#include <conio.h>
#define gotogxy(x,y) gotoxy(x,y)
extern int SCREENWIDTH;
__asm
EXTERN CONSOLE_COLUMNS
_SCREENWIDTH:  defw  CONSOLE_COLUMNS
__endasm;

#endif


#define LEVELS 12 // Number of Levels
#define TRUE 1
#define FALSE 0


unsigned int uLives,uWidth,uHeight, i, j, l, t, r, rx, lx, bCloseFlag=FALSE, Column;
		/* Global miscellaneous variables - i,j for loops and Player coordinates,
		t,r,rx,lx - for the Level build. */

char *MazeLine[12], // Text pointer to the Level (Max 12 lines)
	Mazezam[200], // Storage of the level.
	PressedKey;   // Which key is pressed.
static char *ARGH[]={	"ARGH!",
			"OUCH!",
			"GRRR!",
			"NOOU!",
			"NAAH!",
			"UUUF!",
			"AAAH!",
			"OUUF!",
			"PUFF!"
		};

/* We use escape control combinations for handling of the display */

ClearScreen()
{
#ifdef VT52
	printf("\033E");
#else
	printf("\014");
#endif
}

InverseOn()
{
#ifdef VT52
	printf("\033p");
#elase
	printf("\033[27m");
#endif
}

InverseOff()
{
#ifdef VT52
	printf("\033q");
#elase
	printf("\033[7m");
#endif
}

void PrintEqual()
{
	printf("=");
}

void PrintEnter()
{
	printf("\n");
}

void PrintSpace()
{
	printf(" ");
}

void FillRow()
{
	for (Column=0;Column<SCREENWIDTH;Column++)
		PrintEqual();
	PrintEnter();
}

void FillWall()
{
	for (l=0;l<((SCREENWIDTH-28)/2);l++) PrintEqual();
}

void FillSpace()
{
	for (l=0;l<28;l++) PrintSpace();
}

#ifdef VT52
void gotogxy(int x,int y)
{
	printf("\033Y%c%c",y+32,x+32); // Console sequence for setting cursor to X,Y position.

}
#endif

void	Level(int MazeNumber)
{
	int k;
	char *LevelName; // Pointer to the Level name string.

	ClearScreen();
	switch (MazeNumber)
		{
		case 1:
			LevelName="Humble Origins";uWidth=7;uHeight=2;lx=rx=1;
			MazeLine[1]=" #  #  ";
			MazeLine[2]=" #  ## ";
			break;

		case 2:
			LevelName="Easy Does It";uWidth=8;uHeight=lx=3;rx=2;
			MazeLine[1]="  #  ###";
			MazeLine[2]="  # # # ";
			MazeLine[3]=" # # #  ";
			break;

		case 3:
			LevelName="Up,  Up and Away";uWidth=5;uHeight=lx=11;rx=1;
			MazeLine[1]= "  #  ";
			MazeLine[2]= " # ##";
			MazeLine[3]= " ##  ";
			MazeLine[4]= "# #  ";
			MazeLine[5]= " # # ";
			MazeLine[6]= " ### ";
			MazeLine[7]= "# #  ";
			MazeLine[8]= " # # ";
			MazeLine[9]= " # # ";
			MazeLine[10]="# ## ";
			MazeLine[11]=" #   ";
			break;

		case 4:
			LevelName="Little  Harder";uWidth=5;uHeight=4;lx=2;rx=1;
			MazeLine[1]=" # # ";
			MazeLine[2]="  # #";
			MazeLine[3]=" #   ";
			MazeLine[4]=" ## #";
			break;

		case 5:
			LevelName="To and Fro";uWidth=13;uHeight=6;lx=rx=1;
			MazeLine[1]="   #####     ";
			MazeLine[2]="# #####  ### ";
			MazeLine[3]=" # ### ####  ";
			MazeLine[4]="# #  ####### ";
			MazeLine[5]=" ### #   ##  ";
			MazeLine[6]="# # # ##  #  ";
			break;

		case 6:
			LevelName="Loop-de-Loop";uWidth=14;uHeight=4;lx=2;rx=4;
			MazeLine[1]=" ##### ## ##  ";
			MazeLine[2]="   ##  ##  ## ";
			MazeLine[3]="##  # ## ###  ";
			MazeLine[4]="   ########  #";
			break;

		case 7:
			LevelName="Somehow Easy";uWidth=5;uHeight=rx=3;lx=1;
			MazeLine[1]="  ## ";
			MazeLine[2]=" #   ";
			MazeLine[3]=" # ##";
			break;

		case 8:
			LevelName="Be  Prepared";uWidth=7;uHeight=6;lx=5;rx=3;
			MazeLine[1]="   #   ";
			MazeLine[2]=" ####  ";
			MazeLine[3]=" ### ##";
			MazeLine[4]=" # # # ";
			MazeLine[5]=" # ##  ";
			MazeLine[6]="# ##   ";
			break;

		case 9:
			LevelName="Two  Front Doors";uWidth=16;uHeight=rx=7;lx=1;
			MazeLine[1]="       #######  ";
			MazeLine[2]="  #### #### # # ";
			MazeLine[3]="## ## ###### # #";
			MazeLine[4]="##     # #      ";
			MazeLine[5]="  ############# ";
			MazeLine[6]=" # ##    # ###  ";
			MazeLine[7]="  #   ###     ##";
			break;

		case 10:
			LevelName="Through, through";uWidth=15;uHeight=4;lx=3;rx=1;
			MazeLine[1]=" ####  ####  ##";
			MazeLine[2]=" # ## ## # ##  ";
			MazeLine[3]=" # ## #### ##  ";
			MazeLine[4]=" # ##  ####  # ";
			break;

		case 11:
			LevelName="Double Cross";uWidth=9;uHeight=lx=7;rx=3;
			MazeLine[1]=" #  #### ";
			MazeLine[2]=" #  # ## ";
			MazeLine[3]=" # #### #";
			MazeLine[4]="# ##  #  ";
			MazeLine[5]="  #   ###";
			MazeLine[6]=" ######  ";
			MazeLine[7]="  #      ";
			break;

		case 12:
			LevelName="Inside Out";uWidth=14;uHeight=10;lx=8;rx=1;
			MazeLine[1]= "            # ";
			MazeLine[2]= " ##########  #";
			MazeLine[3]= " ###       ## ";
			MazeLine[4]= " # ######## # ";
			MazeLine[5]= " ### ###  ### ";
			MazeLine[6]= " ###   #  ### ";
			MazeLine[7]= " # ####### ## ";
			MazeLine[8]= " # # #    ### ";
			MazeLine[9]= " ############ ";
			MazeLine[10]="              ";
			break;


		default:
			ClearScreen();
			gotogxy(9,7);
			printf("Congratulations!");
			gotogxy(2,8);
			printf("You've completed the MazezaM");
			gotogxy(12,10);
			InverseOn();
			printf("Well Done!");
			InverseOff();
			bCloseFlag=TRUE;
			getkey();
			break;
		}

	if (bCloseFlag) return;

	for (i=1;i<uHeight+1;i++) strcpy(&Mazezam[i*uWidth],MazeLine[i]); // Copy the Level Data.

	l=((SCREENWIDTH-uWidth)/2);
	t=((22-uHeight)/2);
	r=SCREENWIDTH-l-uWidth;
	printf("==");
	printf("  Level ");
	if (MazeNumber<10) printf("0%d  ",MazeNumber);
	else printf("%d  ",MazeNumber);
	for (i=0;i<(SCREENWIDTH-28);i++) PrintEqual();
	printf("  Lives ");
	if (uLives<10) printf("0%d",uLives);
	else printf("%d",uLives);
	printf("  ==\n");


	for (i=1;i!=t+1;i++) FillRow();

	for (i=1;i!=uHeight+1;i++)
	{
		if (i==lx) for (k=0;k!=l;k++) printf("_");
		if (i!=lx) for (k=0;k!=l;k++) PrintEqual();
		printf(MazeLine[i]); // Print the Level line.
		if (i==rx) for (k=0;k!=r;k++) printf("_");
		if (i!=rx) for (k=0;k!=r;k++) PrintEqual();
		PrintEnter();
	}

	for (i=t+uHeight+1;i!=22;i++) FillRow();
	j=strlen(LevelName);
	for (k=0;k!=((SCREENWIDTH-2-j)/2);k++) PrintEqual();
	PrintSpace();
	printf(LevelName);
	PrintSpace();
	for (k=0;k!=((SCREENWIDTH-2-j)/2);k++) PrintEqual();
	PrintEnter();

	for (i=0;i!=l;i++)
	{
		gotogxy(i+1,t+lx);
		printf("P"); //Start animation
		gotogxy(i,t+lx);
		printf("_"); // Fill after
	}

	gotogxy(l-1,t+lx);
	printf("OP");
	i=lx;
	j=1;
}

void Title()
{
	ClearScreen();
        FillWall();
	printf("        =============       ");
	FillWall();
	PrintEnter();
        for (l=0;l<SCREENWIDTH;l++) PrintEqual();
	PrintEnter();
	for (l=0;l<((SCREENWIDTH-22)/2);l++) PrintEqual();
	InverseOn();
	printf("http://drhirudo.hit.bg");
	InverseOff();
	for (l=0;l<((SCREENWIDTH-22)/2);l++) PrintEqual();
	PrintEnter();
	FillRow();
	FillWall();
        FillSpace();
	FillWall();
	PrintEnter();
	FillWall();
	printf("        M A Z E Z A M       ");
	FillWall();
	PrintEnter();
	FillWall();
	FillSpace();
	FillWall();
	PrintEnter();
	FillWall();
	printf(" CPM version (C) 2004-08 by ");
	FillWall();
	PrintEnter();
	FillWall();
	printf("     Ventzislav Tzvetkov    ");
	FillWall();
	PrintEnter();
	FillWall();
	FillSpace();
	FillWall();
	PrintEnter();
	FillRow();
	PrintEnter();
	PrintEnter();	
	printf("               Keys:\n\n           MOVE - I,J,K,L\n\n");
	printf("          RETRY LEVEL - r\n          QUIT  GAME  - q\n\n");
	printf("       Press SPACE to start\n");
	uLives=3;
	Column=SCREENWIDTH/2;
	for (;;)
	{

		gotogxy(Column,9);
		printf("P");
		PressedKey=getkey();

		if (PressedKey==' ') break;
		if (PressedKey=='Q' || PressedKey=='q')
			{
			bCloseFlag=TRUE;
			break;
			}
		if (PressedKey=='L' || PressedKey=='l')
			{
			gotogxy(Column,9);
			Column+=(Column<(SCREENWIDTH-((SCREENWIDTH-26)/2)));
			PrintSpace();
			}
		if (PressedKey=='J' || PressedKey=='j')
			{
			gotogxy(Column,9);
			Column-=(Column>((SCREENWIDTH-28)/2));
			PrintSpace();
			}
 	}
}


int main()
{
	int Mazeno,Loop,rand;

#ifdef VT52
	printf("\033ø5"); // Hide Cursor escape sequence
#endif

	for (;;)
	{
		Title();
		Mazeno=1;
		if (bCloseFlag) break;
		Level(Mazeno); 

		for(;;)
		{

			PressedKey=getkey();
			if (PressedKey=='R' || PressedKey=='r')
			{
				uLives--;
			if (uLives<1) PressedKey='q';
		 		else
				{
				gotogxy(l+j-3,t+i-1);
				InverseOn();
				printf(ARGH[rand%9]);
				InverseOff();
				getkey();
				Level(Mazeno);
				}
			}



			if (PressedKey=='Q' || PressedKey=='q')
			{
				gotogxy(l+j-3,t+i-1);
				InverseOn();
				printf(ARGH[rand%9]);
				InverseOff();
				getkey();
				gotogxy(((SCREENWIDTH-10)/2),10);
				InverseOn();
				printf("GAME OVER!");
				InverseOff();
				getkey();
				bCloseFlag=TRUE;
				break;
   			}

			if ((PressedKey=='L' || PressedKey=='l') && (i==rx && j==uWidth))
			{
				gotogxy(l+j-1,t+i);
				PrintSpace();
				for (i=l+uWidth;i!=SCREENWIDTH;i++)
				{
					gotogxy(i,t+rx);
					printf("P");
					gotogxy(i,t+rx);
					printf("_");
				}
				InverseOn();
				i=t+rx;
				switch (rand%6)
				{
				case 0:
					gotogxy(13,i);
					printf("Hurray!");
					break;

				case 1:
					gotogxy(13,i);
					printf("Hurrah!");
					break;

				case 2:
					gotogxy(16,i);
					printf("Yes!");
					break;

				case 3:
					gotogxy(14,i);
					printf("Great!");
					break;

				case 4:
					gotogxy(12,i);
					printf("Yee-hah!");
					break;

				default:
					gotogxy(16,i);
					printf("Yay!");
				}
				InverseOff();
				getkey();
				uLives++;
				Level(++Mazeno);
				PressedKey=0;
			}
			if ((PressedKey=='L' || PressedKey=='l') && j<uWidth)
				if ((Mazezam[i*uWidth+j])==' ')
				{
					gotogxy(l+j-1,t+i);
					PrintSpace();
					j++;
					gotogxy(l+j-1,t+i);
					printf("P");
				} 

				else

					if (Mazezam[(i*uWidth)+uWidth-1]==' ')
					{
					j++;
					for (Loop=uWidth-1;Loop>0;Loop--)
					Mazezam[(i*uWidth)+Loop]=Mazezam[(i*uWidth)+Loop-1];
					Mazezam[i*uWidth]=' ';
					gotogxy(l,t+i);
					for (Loop=0;Loop<uWidth;Loop++) putchar(Mazezam[i*uWidth+Loop]);
					gotogxy(l+j-1,t+i);
					printf("P");
					}

			if ((PressedKey=='J' || PressedKey=='j') && j>1)
				if (Mazezam[(i*uWidth)+j-2]==' ')
				{
				gotogxy(l+j-1,t+i);
				PrintSpace();
				j--;
				gotogxy(l+j-1,t+i);
				printf("P");
				}
				else

					if (Mazezam[i*uWidth]==' ')
					{
					j--;
					for (Loop=1;Loop!=uWidth;Loop++)
					Mazezam[(i*uWidth)+Loop-1]=Mazezam[(i*uWidth)+Loop];
					Mazezam[(i*uWidth)+uWidth-1]=' ';
					gotogxy(l,t+i);
					for (Loop=0;Loop<uWidth;Loop++) putchar(Mazezam[i*uWidth+Loop]);
					gotogxy(l+j-1,t+i);
					printf("P");
					}

					if ((PressedKey=='K' || PressedKey=='k') && i<uHeight)
						if (Mazezam[((i+1)*uWidth)+j-1]==' ')
						{
						gotogxy(l+j-1,t+i);
						PrintSpace();
						i++;
						gotogxy(l+j-1,t+i);printf("P");
						}

			if ((PressedKey=='I' || PressedKey=='i') && i>1)
				if (Mazezam[((i-1)*uWidth)+j-1]==' ')
				{
				gotogxy(l+j-1,t+i);
				PrintSpace();
				i--;
				gotogxy(l+j-1,t+i);
				printf("P");
				}

			PressedKey=0;
			if (bCloseFlag) break;
			rand++;
		}
		if (bCloseFlag) break;
	}
	ClearScreen();

#ifdef VT52
	printf("\033ù5"); // Show Cursor escape sequence
#endif
	return 0;
}
