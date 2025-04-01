/* LTYPE -  */

#define	VERSION		3
#define REVISION	0
#define MOD_DATE	"84-03-01"

/* This program will type a member of a LBR file... any member,
   BUT anything other than an ASCII file will produce a screenful
   of garbage.

   USE: LTYPE <library> <member>
   COMPILE/LINK: cc1 ltype
		 nl2 ltype libacc
   By S. Kluger 01-13-83

   vers 3.0   3-1-84
   Upgrade to lattice c ms dos P. H. Mack.
*/

#define	ERROR  0xFEFF

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char curdsk, fcb[36];
char fnam[12], libnam[16], dirbuf[128], *dirp;
int  fd, dirsiz, filsiz;

#define OK	0


/************************************************
 error exit
*************************************************/

erxit(strg)
char *strg;
{
	printf(strg);
	exit(-1);
}


/************************************************
 reload
*************************************************/

reload()
{
	read(fd,dirbuf,128);
	dirp = dirbuf;
	dirsiz--;
}


/************************************************
 Typing function 
*************************************************/

doit()

{
	int	j;
	int	c;

	dirsiz = filsiz;
	do
	{
		reload();

		for (j=0; j<128; j++){

			if (*dirp == 0x1a)
				exit(0);
			putchar(*dirp);
			if(*dirp == 0x0a)
			putchar(0x0d);
			dirp++;
		}
	}while(dirsiz != 0);
}


/************************************************
 match char
*************************************************/

char matchr(st,ch)
char *st,ch;
{
	int i;
	for(i=0; st[i]; i++){
		if(st[i] == ch) return(i);
	}
	return(0);
}


/************************************************
 open library file
*************************************************/

opnlib(file)
char *file;
{
	char l, *npnt;

	strcpy(libnam,file);
	l = matchr(libnam,'.');
	if (l == 0) strcat(libnam, ".LBR");
//	setfcb(fcb,libnam);	/* build name in fcb */
//	movmem(fcb,fnam,12);	/* get from dfcb2 for log */
//
//	if(strcmp(fcb+9, "LBR")){
//		printf("got %s\n",libnam);
//		erxit("Invalid file spec, MUST be type .LBR\n");
//	}

	fd = open(libnam,O_RDONLY,0);
	if(fd == -1) erxit("Library file not found.\n");
}

/************************************************
 find library member
*************************************************/

fndmem(file)
char *file;
{
	char dnam[16], fname[36];
	long int	floc;

	setfcb(fname, file);
	read(fd,dirbuf,128);
	dirp = dirbuf;
	dirsiz = *(dirp+14);
	dirp += 32;

	do{
		if (*dirp == 255) return(ERROR);
		if (*dirp == 0){
			strcpy(dnam, dirp+1);
			dnam[11]=0;
			if(strcmp(dnam, fname+1) == 0){
				filsiz = (*(dirp+14)) + ((*(dirp+15)) * 256);
				floc=(*(dirp+12)) + ((*(dirp+13)) * 256);
				lseek(fd,floc *128,0);
				return(OK);
			}
		}
		dirp += 32;
		if(dirp > dirbuf+128) reload();
	}
	while(dirsiz);
	return(ERROR);
}


/************************************************
 main
*************************************************/

main(argc,argv)
int argc;
char **argv;
{
	printf("\n\rLTYPE vers:%d.%02d  %s\n\r\n",
	VERSION,REVISION,MOD_DATE);

	opnlib(argv[1]);
	if (fndmem(argv[2]) == ERROR) erxit("\n\rMember not in LBR file!\n\r");
	printf("\n\rFile present - %d sectors.\n\r",filsiz);
	doit();
}
