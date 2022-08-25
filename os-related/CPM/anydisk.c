/*

'ANYDISK' July 27, 1982

z88dk conversion by Stefano Bodrato, 25/08/2022


	zcc +cpm -create-app -O3 anydisk.c
	zcc +cpm -create-app -compiler=sdcc anydisk.c

..add your target specific options for your own CP/M system, e.g.  -subtype=dmv

*/


/*

Original documentation and credentials:


	Disk Utility Program for CP/M version 2.x

	Original May, 1980	by Richard Damon
	Modified Sept 9, 1981	by Robert Ward
	Modified July, 1982	by Edward K. Ream

	Please send all reports of bugs to:

		Edward K. Ream
		1850 Summit Ave.
		Madison, WI 53705
		(608) 231 - 2952





The authors

The original version of this program was written by Richard Damon.
Robert Ward rewrote the program and called it DDNDISK.
Edward K. Ream further modified DDNDISK to produce ANYDISK.


This program allows the operator to examine and modifya CP/M disk.  

The commands available are:

	-	Bn  Set current track and sector to point at block n and enter block mode.
	-	Cn  Fill buffer with SIGNED value n.
	-	Dn  Set current disk number to n (0--# of drives-1) and print disk information.
	-	Ea n n ... n    Edit buffer starting from location a.
	-	Fn  Fill buffer with UNSIGNED value n.
	-	H   Print help message.
	-	M   Print the directory and the disk allocation map.  
	-	N   Go to next block or track/sector.  
	-	P   Print contents of buffer.
	-	R   Read sector or block into buffer (depending on mode).  
	-	Sn  Set current sector to n (0--# of sectors per track-1) and enter sector mode.  
	-	Tn  Set current track to n  (0--# of tracks  per disk-1) and enter sector mode.  
	-	W   Write sector or block from buffer (depending on mode).  
	-	X   Exit program.  

Notes:
	- Multiple commands may be specified on a line except for the X command
	  which must be the only command on the line.
	- Commands may be in upper or lower case letters.  
	- Spaces are ignored except in the E command where they are used as separaters for the numbers.


Typical commands
================

d0t0s1rp	read in the track 0 sector 1 of disk 0 (drive A) and print it.

e1A 4F		set buffer location 1A to 4F and set buffer location 1B to 20.

e0a 00w		set buffer location 0a to 0 and write buffer.  Note no space after last data byte.

b0rp		print the entire first block.

b0irp		print the first sector of the first block.



The M command prints the directory in two sections.  
Section 1 is a listing of each directory entry.  
For instance, entry 15 in the directory would be listed as:

	15 = DDNDISK C:0

The equal sign means the entry is valid.  
A star (*) would
be printed instead of the equal sign if the entry were deleted.  
The number following the colon is the extent number.  

The second section is a disk allocation map.  
There is one entry in this map for every block on the disk.  
If the block
is not allocated to any file, the characters "..." appear.
Otherwise, a number appears.  
This number refers to the
number of the directory entry which allocated the block.  
Refer back to section 1 to see which file allocated the block.  


The structure of a CP/M 2.x disk
================================

Here is a brief description of the CP/M 2.x disk format.
Several tracks are typically reserved for the bootstrap and a copy of CCP and BDOS.  
The number of reserved tracks is determined by BIOS for each type of disk.  
This number is called the track offset; it is printed by the 'D' command when a disk is selected.  

Most commands deal with the track offset automatically.  
The only exception is the 'T' command.  
When using the 'T' command, you must add the offset yourself.  
Thus, you can access ANY track with the 'T' command.  

Tracks following the reserved tracks store data.  
To speed up disk access, CP/M does not store data in consecutive sectors.  
The sector translate table converts from logically consecutive sectors to the order that those
sectors are actually stored on the disk.
(This table is used only on CP/M 2.x versions.)

All commands deal with LOGICAL sector numbers.  
This is what you almost always want.  
Whenever a sector is printed,
both the logical and physical sector numbers are shown.  
Thus, if you can find any physical sector if you must.  

A block is the smallest unit of a disk which can be allocated to a file.  
The size of a block is a multiple of 128 bytes.  
The block size is determined by BIOS for each type of disk.  
The block size is printed by the 'D' command.  

Several blocks are reserved at the beginning of the disk for the directory.  
Again, the number of reserved blocks is determined by BIOS for each type of disk.  
This number is also printed by the 'D' command.  

The format of each entry of the directory is as follows.  
Each entry describes one extent of a file.  

byte   0    : file code : 0 if file exists, 0xe5 if deleted
bytes  1- 8 : file name : file name (ascii)
bytes  9-11 : file type : file type (ascii)
byte   12   : file ext  : extent number (binary)
bytes  13,14: unused
bytes  15   : rec count : count of number of sectors in extent
bytes  16-31: map       : list of blocks used by this extent


Please note that byte 15 is NOT a reliable guide to the number of blocks allocated to a file.
The way to tell is simply to count the blocks in the map.
Block 0 is never allocated to any file and it signals the end of the map.
Remember that block numbers take either 1 or 2 bytes depending on whether there are less than 256 blocks or not.


Differences between DDNDISK and ANYDISK
=======================================

ANYDISK uses the CP/M 2.x BIOS select disk routine to get the characteristics of the disk dynamically.
Thus, different kinds of disks may be used without recompiling the program.

ANYDISK will work even with hard disks.  
Several arithmetic statements have been converted into loops in order to avoid overflow problems.
This also means that a single index variable can NOT be used.
Instead,  the current block, track and sector numbers are maintained separately.

The code that computes the disk map now needs lots less memory.
Only one disk block at a time is now loaded into memory.

The disk map now is more readable.
The format is useful even for hard disks.
Directory entries which have 0xe5 for a file name are completely ignored.

The 'D' command now prints a summary of the characteristics of the selected disk.

The 'H' command is new.  
It prints a help message.  


The 'I' command is now useful for printing parts of a block.
For example, to print just the 5'th record of block 20 use:

	b20i nnnn rp
	
The 'W' command now tells BIOS that all writes are to the directory.  
This forces BIOS to do the write at once.

You get a more informative prompt if you type a value which is out of range.

The ptmap routine contains two constants, M1COL and M2COL
which control how many entries will be put on each line
for the directory list and the disk allocation map.  
I use fairly small values because my screen only has 64 columns.  
Choose the constants you like best.

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//#include <fcntl.h>

#define setmem(a,b,c) memset(a,c,b)
#define BDOS_CKS  11	/* check keyboard status for "kbhit" */


#define VERSION "\nJuly 27, 1982\n\n"



/*
	Define the data structures pointed to by
	the BIOS select disk function.
*/


/* DPH -- Disk Parameter Header.  One for each disk */

struct DPH {

	/* XLT: address of translation table.	*/
	/* one table for each TYPE of disk	*/
	char *	d_XLT;

	/* scratchpad area for BDOS use only	*/
	int	d_TEMP1;
	int	d_TEMP2;
	int	d_TEMP3;

	/* DIRBUF: address of 128-byte directory buffer	*/
	/* all disks use the same buffer		*/
	char *	d_DIRBUF;

	/* DPB: address of DPB. One for each type of drive */
	struct DPB *	d_DPB;

	/* CSV:	address of changed-disk check buffer	*/
	/* one for each drive				*/
	/* size of buffer is DPB -> CSK bytes		*/
	char *	d_CSV;

	/* ALV: address of allocation buffer		*/
	/* one for each drive				*/
	/* size of buffer is (DPB -> DSM / 8) + 1 bytes	*/
	char *	d_ALV;

};


/*
	DPB -- Disk Parameter Block
	One for each TYPE of disk
*/

struct DPB {

	/* sector per track */
	int	d_SPT;

	/* BSH: block shift factor		*/
	/* BLM = (2 ** BSH) - 1			*/

	/* BLS: block size. Note size VARIES	*/
	/* BLS = (BLM + 1) * 128		*/
	char	d_BSH;
	char	d_BLM;

	/* EXM: extent mask */
	/* if DSM > = 256 then block numbers take two bytes */
	/* Thus, each extent can hold only 8 block numbers */
	char	d_EXM;

	/* DSM maximum disk BLOCK number		*/
	/* BLOCKS numbered starting at zero		*/
	/* (DSM + 1) * BLS = # of bytes per drive	*/
	int	d_DSM;

	/* DRM:	directory max. directory entries 0 -- DRM */
	int	d_DRM;

	/* AL0, AL1:  allocation vector for directory	*/
	/* each bit reserves one BLOCK			*/
	char	d_AL0;	/* high bit = block 0 */
	char	d_AL1;	/* high bit = block 8 */

	/* CKS: size of directory check vector (or 0)	*/
	/* fixed media:		CKS = 0			*/
	/* removeable media:	CKS = (DRM + 1)/4	*/
	int	d_CKS;

	/* OFF: track offset.  number of skipped tracks */
	int	d_OFF;
};


/* Define BIOS entry points. */

#define SELDSK	9
#define SETTRK	10
#define SETSEC	11
#define SETDMA	12
#define RDSEC	13
#define WTSEC	14
#define	SECTRAN	16	/* BE = logical sec # */
			/* DE = address of translation table */


/* Set this constant to the number of drives you have. */

#define NDRVS 5

/*
	Set BBUFSIZ to the size of the largest block
	that any of your disks use.

	Set MBUFSIZ to the largest number of directory
	entries that are placed on any of your disks.

	There is code (in the select routine) which
	makes sure that these constants are, in fact,
	large enough.  If not,  a message is issued and
	the program aborts, so you shouldn't be able to
	bomb the program (or your disks).
*/

#define BBUFSIZ 4096
#define MBUFSIZ 4096

/* Do not change ANY of the following constants. */

#define PHYS 0
#define LOG 1
#define YES 1
#define NO  0
#define SECSIZE	128
#define table TXTABLE


/* Define global constants set by the select() routine. */

struct DPH * d_header;	/* pointer to DPH		*/
struct DPB * d_disk;	/* pointer to DPB		*/

int	TPD;		/* tracks  per disk		*/
int	SPT;		/* sectors per track		*/
int	BPD;		/* blocks  per disk		*/
int	SPB;		/* sectors per block		*/
char *	TXTABLE;	/* pointer to translate table	*/
int	DIRBLKS;	/* blocks in the directory	*/
int	DIRSIZE;	/* entries in the directory	*/
int	OFFSET;		/* offset of first track	*/





/* Print the header message of a dump. */

void prthdr(int mode, int disk, int block, int track, int sector, char *table)
{
	printf("\ndisk %c, ", disk + 'A');
	if (mode == LOG) {
		printf("block %d, ", block);
	}
	printf("track %d, ",
	track + (mode == LOG ? OFFSET : 0));
	printf("physical sector %d, ",
	biosh(SECTRAN, sector, (unsigned int)table));
	printf("logical sector %d", sector);
	printf("\n");
}




/* Dump out the buffer in hex and in ascii. */

void dmpbuff(char *pntr, int nsects, int mode, int disk, int block, int track, int sector, char *table)
{
	int i,j,lchar;

	//prthdr(mode,nsects,disk,block,track,sector,table);
	prthdr(mode,disk,block,track,sector,table);

	for (i = 0; i < nsects * SECSIZE; i += 16) {

		if(i % SECSIZE == 0 && mode == LOG) {
			printf("\n\nRecord %d\n", i/SECSIZE);
		}
		printf("\n%04x ", i);

		for (j = 0; j < 16; j++){
			if(j%4 == 0) {
				printf(" ");
			}
			if (j%8 == 8) {
				printf(" ");
			}
			printf("%02x", pntr [i+j]);
		}

		printf("  ");
		for (j = 0; j < 16; j++) {
			lchar = pntr [i+j];
			if (lchar < 0x1f || lchar > 0x7d) {
				printf(".");
			}
			else {
				printf("%c",lchar);
			}
		}
		//if (kbhit()) break;
		if (getk() != 0) break;
	}
	printf("\n");
}


/* Get a number from the keyboard. */

int getnum(char **pntr, int low, int high, int base)
{
	int number;
	char c, buffer[50], *bp;

	number = 0;
	while(**pntr == ' ') (*pntr)++;
	while((c = toupper(*(*pntr)++))>='0' && c<= '9' ||
	base ==16 && (c-=7) > '9' && c<= ('F'-7))
		number = base*number+c-'0';
	(*pntr)--;

	if (number < low || number > high) {
		if (base == 16) {
			printf("%x is out of range.\n",number);
			printf("Enter a hex number between ");
			printf("%x and %x:  ", low, high);
		}
		else {
			printf("%d is out of range.\n",number);
			printf("Enter a decimal number ");
			printf("between %d and %d:  ",
			low, high);
		}
		bp = gets(buffer);
		number = getnum(&bp, low, high, base);
	}
	return number;
}


/* Print a helpful message. */

void help()
{
	printf("Commands are...\n\n");
	printf("Bn  enter block mode and set block n.\n");
	printf("Cn  fill buffer with short n.\n");
	printf("Dn  set disk to n and print disk info.\n");
	printf("Ea n1 n2 ...  edit buffer[a] with n1 n2 ...\n");
	printf("Fn  fill buffer with unsigned n.\n");
	printf("H   print this message.\n");
	printf("I   enter track/sector mode.\n");
	printf("M   print disk allocation map.\n");
	printf("N   go to next block or track/sector.\n");
	printf("P   print buffer.\n");
	printf("R   read sector or block into buffer.\n");
	printf("Sn  set current sector to n (first sector is 0).\n");
	printf("Tn  set current track  to n (first track  is 0).\n");
	printf("W   write sector or block from buffer.\n");
	printf("X   exit program (must be alone on line).\n");
}


/* Print information about the selected disk. */

void info()
{
	printf("Disk Parameter Header  = %x (hex)\n", d_header);
	printf("Disk Parameter Block   = %x (hex)\n", d_disk);
	printf("Sector Translate Table = %x (hex)\n", TXTABLE);
	printf("Track offset:      %d\n", OFFSET);
	printf("Tracks  per disk:  %d\n", TPD);
	printf("Blocks  per disk:  %d, %x (hex)\n", BPD, BPD);
	printf("Sectors per track: %d\n", SPT);
	printf("Sectors per block: %d\n", SPB);
	printf("Chars   per block: %d\n", SPB * SECSIZE);
	printf("Directory blocks:  %d\n", DIRBLKS);
	printf("Directory entries: %d\n", DIRSIZE);
	printf("\n");
}




/* Read a block or sector into the buffer. */

//void rdbuff(char *pntr, int nsects, int mode, int disk, int block, int track, int sector, char *table)
void rdbuff(char *pntr, int nsects, int mode, int disk, int track, int sector, char *table)
{
	int i;
	int s;

	bios(SELDSK, disk,0);
	for (i = 0; i < nsects; i++, sector++){
		if (sector >= SPT) {
			track++;
			sector = 0;
		}
		if (track >= TPD) {
			printf("Read truncated!\n");
			return;
		}
		bios(SETTRK, track + (mode==LOG ? OFFSET : 0),0);
		s = biosh(SECTRAN, sector, (unsigned int)table);
		bios(SETSEC, s, 0);
		bios(SETDMA, (unsigned int)pntr + (i * SECSIZE),0);
		bios(RDSEC,0,0);
	}	
}


/* Print the directory and a disk allocation map. */

#define M1COL 3		/* Entries/line:  directory	*/
#define M2COL 16	/* Entries/line:  alloc map	*/

void ptmap(int disk, char *table)
{
	int count, i, j, k;
	int track, sector, d, ex;
	char dir[BBUFSIZ];	/* Current directory bloc. */
	int  map[MBUFSIZ];	/* The disk allocation map. */
	char *intdir;
	int id;

	intdir = dir;

	/* Clear the map. */
	setmem(&map, 2*MBUFSIZ, 0);


	printf("The directory is...\n");

	track = sector = 0;
	for (count = i = 0; i < DIRSIZE; i++) {

		if (i%(4*SPB) == 0) {
			/* Read the next next block. */
			rdbuff	(dir, SPB, LOG, disk,
			track, sector, table);
			sector += SPB;
			if (sector >= SPT) {
				track++;
				sector -= SPT;
			}
			j = 0;
		}
		else {
			j += 32;
		}

		/* Skip never allocated entries completely. */
		if (dir [j+1] == 0xe5) {
			continue;
		}

		/* Print M1COL entries per line. */
		if (count%M1COL == 0) {
			putchar('\n');
		}
		count++;

		/* Print directory number. */
		printf("%3x", i);

		/* Print '=' for nondeleted extents */
		printf("%s",  dir[j]==0 ? " = " : " * ");

		/* Save extent number. */
		ex = dir [j + 12];

		/* Print the file name and extent. */
		dir [j + 12] = 0;
		printf("%s:%x  ", &dir [j+1], ex);

		/* Skip deleted blocks. */
		if (dir [j] == 0xe5) {
			continue;
		}

		/* See whether block # fits in 8 bits. */
		if (BPD <= 256){
			for(k = 16; k < 32 && dir [j+k]; k++) {
				d = dir [j + k];
				if (d) {
					map [d] = i;
				}
				else {
					break;
				}
			}
		}
		else {
			for(k = 8; k<16 && intdir[j/2 + k]; k++){
				id = intdir [j/2 + k];
				if (id) {
					map [id] = i;
				}
				else {
					break;
				}
			}
		}
	}

	printf("\n\nThe disk map is...\n\n");

	for(i = 0; i < BPD; i++) {

		if (i%M2COL == 0) {
			printf("%3x: ", i);
		}

		if (map [i]) {
			printf("%3x ", map [i]);
		}
		else {
			printf("... ");
		}

		if(i%M2COL == M2COL-1) {
			putchar('\n');
		}
	}
	putchar('\n');
}

#undef M1COL
#undef M2COL



/*
	Select a drive for all future disk operations.
	drive is 0 for drive A:, 1 for drive B:, etc.
*/

void select (char drive)
{
	int alloc, i, nsectors;

	printf("select drive = %c\n", drive + 'A');

	/* Point at the DPH and DPB. */
	d_header = (struct DPH *)biosh(SELDSK, drive, 0);
	if (d_header == 0) {
		printf("Select failed!\n");
		exit(0);
	}
	d_disk = d_header -> d_DPB;

	/* calculate global constants for this drive */
	TXTABLE = d_header -> d_XLT;
	SPB = d_disk   -> d_BLM + 1;
	BPD = d_disk   -> d_DSM + 1;
	SPT	 = d_disk   -> d_SPT;
	OFFSET  = d_disk   -> d_OFF;

	/*
		Compute number of logical tracks on the disk.
		For hard disks nsectors can overflow.
		thus, we must use a for loop instead of:
	 */

	/* comment out -----
		nsectors = SPB * BPD;
		ntracks  = nsectors / SPT;
		if (nsectors % SPT != 0) {
			TPD++;
		}
	----- end comment out */

	nsectors = 0;
	TPD  = 1;
	for (i = 0; i < BPD; i++) {
		nsectors += SPB;
		if (nsectors > SPT) {
			nsectors -= SPT;
			TPD++;
		}
	}

	/* Add the number of hidden tracks. */
	TPD += OFFSET;

	/* Compute the number of blocks in the directory. */
	alloc = ((d_disk -> d_AL0) << 8) | (d_disk -> d_AL1);
	DIRBLKS = 0;
	for (i = 0; i < 16; i++) {
		if ((alloc & 1) != 0) {
			DIRBLKS++;
		}
		alloc = alloc >> 1;
	}

	/*
		Compute the number of entries in the directory.
		There are 4 entries in each 128-byte sector.
	*/

	DIRSIZE = DIRBLKS * SPB * 4;

	/* Make sure the directory buffer is big enough. */
	if (SPB * SECSIZE > BBUFSIZ) {
		printf("Block buffer is too small.\n");
		exit(0);
	}

	/* Make sure the map buffer is big enough. */
	if (DIRSIZE > MBUFSIZ) {
		printf("Map buffer is too small.\n");
		exit(0);
	}
}


/* Write the buffer to the disk. */

//void wrbuff(char *pntr, int nsects, int mode, int disk, int block, int track, int sector, char *table)
void wrbuff(char *pntr, int nsects, int mode, int disk, int track, int sector, char *table)
{
	int i;
	int s;

	bios (SELDSK, disk,0);
	for (i = 0; i < nsects; i++, sector++) {
		if (sector >= SPT) {
			sector = 0;
			track++;
		}
		if (track >= TPD) {
			printf("Write truncated!\n");
			return;
		}
		bios(SETTRK, track + (mode==LOG ? OFFSET : 0),0);
		s = biosh(SECTRAN, sector, (unsigned int)table);
		bios(SETSEC, s,0);
		bios(SETDMA, (unsigned int)pntr + (i * SECSIZE),0);

		/* Force immediate write. */
		bios(WTSEC, 1,0);
	}
}


void main()
{
	char buffer [BBUFSIZ];
	char buff[80], *bufp, c;
	char *temp;
	int mode, nsects, disk, b, i, j;
	int block, track, sector;

	/* Sign on. */
	printf("Welcome to ddndisk version 3:  ");
	printf(VERSION);

	/* Initialize */
	track	= 0;
	sector	= 0;
	block	= 0;
	mode	= PHYS;
	nsects	= 1;

	/* The default drive is drive A: */
	disk  = 0;
	select(disk);

	for (;;) {
		bufp = gets(buff);

		/* The 'X' request must be the only thing on a line. */
		//if (tolower(*bufp) == 'x' && *(bufp + 1) == '\0') {
		// z88dk fix, August 2022
		if (toupper(*bufp) == 'X') {
			break;
		}

		while (c = *bufp++) switch (toupper(c)) {

	case 'B':
		mode   = LOG;
		nsects = SPB;

		/* Get block number from the user. */
		block = getnum(&bufp, 0, BPD-1 ,16);

		/*
			Convert block number to track/sector.
			A loop is used to avoid overflow.
		*/
		track = sector = 0;
		for(b = block; b; b--) {
			sector += SPB;
			if (sector >= SPT) {
				track++;
				sector -= SPT;
			}
		}
		break;

	case 'C':
		/* Fill the buffer with a signed constant. */
		temp = getnum(&bufp, -32765, 32765, 16);
		for(i = 0; i < nsects * SECSIZE; i++) {
			buffer[i] = temp[i];
		}
		break;

	case 'D':
		/* Select the disk and print disk info. */
		disk = getnum(&bufp, 0, NDRVS-1, 10);
		select(disk);
		info();
		break;

	case 'E':
		/* Patch the buffer with a list of chars. */
		i = getnum(&bufp, 0, nsects*SECSIZE-1, 16);
		while(*bufp ==' ') {	
			buffer [i++] = getnum(&bufp,0,255,16);
			if(i >= nsects*SECSIZE) {
				break;
			}
		}
		break;

	case 'F':
		/* Fill the buffer with an unsigned constant */
		i = getnum(&bufp, 0, 255, 16);
		for(j = 0; j < nsects*SECSIZE; j++) {
			buffer [j] = i;
		}
		break;

	case 'H':
		/* Print a help message. */
		help();
		break;

	case 'I':
		/* Enter track/sector mode. */
		if (mode != PHYS) {
			mode = PHYS;
			nsects = 1;

			/* Convert block to track/sector. */
			track = sector = 0;
			for (b = block; b; b--) {
				sector += SPB;
				if (sector >= SPT) {
					track++;
					sector -= SPT;
				}
			}
			track += OFFSET;
		}
		break;

	case 'M':
		/* Print the directory and allocation map. */
		ptmap(disk, table);
		break;

	case 'N':
		/* Go to next sector or block. */
		if (mode == LOG) {
			block++;
			sector += SPB;
		}
		else {
			sector++;
		}

		if (sector >= SPT) {
			track++;
			sector -= SPT;
			if (track >= TPD) {
				/* Stop the scan. */
				*bufp = '\0';
				printf("No next sector.\n");
			}
		}
		break;

	case 'P':
		/* Print the buffer. */
		dmpbuff	(buffer, nsects, mode, disk,
		block, track, sector, table);
		break;

	case 'R':
		/* Read a block or sector into the buffer. */
		rdbuff	(buffer, nsects, mode, disk,
		track, sector, table);
		break;

	case 'S':
		/*
			Set LOGICAL sector #.
			Enter track/sector mode.
		*/
		if (mode == LOG) {
			mode = PHYS;
			track = OFFSET;
		}
		nsects = 1;
		sector = getnum(&bufp, 0, SPT - 1, 10);
		break;

	case 'T':
		if (mode == LOG) {
			mode = PHYS;
			sector = 0;
		}
		nsects = 1;
		track = getnum(&bufp, 0, TPD - 1, 10);
		break;

	case 'W':
		/* Write the buffer to the disk. */
		wrbuff	(buffer, nsects, mode, disk,
		track, sector, table);
		break;

	case ' ':
	case 13:
	case 10:
		/* Ignore spaces. */
		break;

	default:
		/* Unknown request. */
		printf("%c ?????\n",c);
		*bufp = '\0';
		break;

	} /* end switch */

	//if(kbhit()) getchar();
	getk();
	}
}
