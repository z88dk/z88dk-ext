/*
HEADER:		;
TITLE:		Chop;
VERSION:	1.1;
DATE:		10/27/1985;

DESCRIPTION:	"Removes sectors from the beginning of a file,
		and creates a new file containing the remaining data.";

KEYWORDS:	File, utility;
SYSTEM:		CP/M-80;
FILENAME:	CHOP.C;
AUTHORS:	Unknown;
COMPILERS:	BDS C;
*/
/************************************************************************

	Ver. 1.0: Author and date unknown.
	Version 1.1, October 27, 1985:
		Sign-on message modified to provide instructions
		if the user formats the command line incorrectly.
		John M. Smith, CUG librarian, Utilities IV diskette.

	Usage: A>CHOP file1 file2 n

		file1 is input, file2 is created, and
		"n" is the number of records to "chop".

*************************************************************************/


#include <stdio.h>
#include <stdlib.h>

#define SECSIZ 128

 int nskip, recnum, nread;
 int infd, outfd;


main(int argc, char *argv[])
{

 char buf[SECSIZ];

 puts("\nFile Chopper, V1.1");

 if (argc != 4)
  { puts("Removes 128 byte sectors from the front of a file.\n");
    puts("Usage: CHOP file1 file2 n\n");
    puts("file1 is input, file2 is created, and");
    puts("n is the number of sectors to remove.");
    exit(0); }

 if ((nskip = atoi(argv[3])) == 0)
  { puts("No records to skip\n"); exit(0); }

 /* open files */

// if ((infd = open(argv[1],0)) == -1)
 if((infd = fopen(argv[1],"rb")) == 0)
  { printf("%s%s","File not found: ",argv[1]); exit(0); }

// if ((outfd = creat(argv[2])) == -1)
 if((outfd = fopen(argv[2],"wb"))==0)
	 { printf("%s%s","Cannot create: ",argv[2]); exit(0); }

 printf("Skipping %u records\n", nskip);
 for (recnum = 0; recnum < nskip; recnum++)
//  { if (read(infd,buf,1) <= 0)
  { if (fread(buf, 1, SECSIZ, infd) < SECSIZ)
     { printf("%s %u","File read error at record",recnum); exit(0); }
  }

// while (read(infd,buf,1) > 0)
 while ((nread = fread(buf, 1, SECSIZ, infd)) > 0)
  //{ if (write(outfd,buf,1) == -1)
  { if (fwrite(buf, 1, SECSIZ, outfd) != nread)
     { printf("Output file error at record %u",recnum); exit(0); }
  }

 puts("Done");

 fclose(outfd);

 }
