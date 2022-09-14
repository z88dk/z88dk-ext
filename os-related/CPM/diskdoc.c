
// zcc +cpm -create-app -DAMALLOC -subtype=dmv diskdoc.c

/*  TAB s5,4
 *
 *  diskdoc
 *  a utility for diskette maintainance
 *
 *  by egil kvaleberg
 *     studpost 111
 *     n7034 trondheim-nth
 *     norway
 *
 *  diskdoc is intended for use and distribution
 *  among amateur computer users only. please respect this.
 *
 *  compile using the Ron Cain small C compiler as distributed
 *  by The Code Works. if you use the version of the compiler shown
 *  in the Dr. Dobbs May 1980 issue, you must ensure that the stack
 *  is properly initialized. if you use another C compiler, you'll
 *  have to revise (parts of) the file ddocsys.c. good luck.
 */

#define VERSION "ver c - 1 nov 81"
#define WHOMADEIT "by egil kvaleberg"

/*
 *  constants, change as required
 */

#define SECLEN 128	/* sector length, 128 or 256 is ok */

#define VOID 229	/* character used when zeroing diskette */

#define DHOME 26	/* default home character */
#define ESC 27		/* lead-in character */

#define CR 13		/* various ascii equates */
#define LF 10
#define BS 8
#define QOT 39




/*
 *  system dependant functions
 */

/*  TAB s5,4
 *
 *  system dependant functions for diskdoc
 *
 *  required are:
 *  botmem,topmem,sysok,conin,conout,const
 *  seldrv,lnext,read,write,rstdrv
 */


#include <stdlib.h>
#include <cpm.h>


/*
 *  globals
 */

int tracks,		/* number of tracks and sectors */
    sectors,firstsector;

int dfltrk,dflsec;	/* default values */
char dfldrv;

char secbuf[SECLEN];	/* buffer for patch */
char vfybuf[SECLEN];	/* buffer for write verify */

int home;		/* character to move cursor home */



/*
 *  this version is for small C and cp/m 8080 version 2.x.
 *  since bios must be used for sector read and write,
 *  character i/o functions use bios directly too.
 *  cp/m "compatible" operating systems will probably
 *  need to have these functions rewritten.
 */

char *xlt;	/* sector xlate table, used by cp/m version */

/*
 *  return pointer to bottom of free memory
 */

//int lastglobal;     /* dirty trick, will work in small-c */

char *botmem()
{   //return (&lastglobal);
   //return (&lastglobal+200);
   return (15000);
}

/*
 *  top of free memory
 *  remember that the stack needs some space too
 */

extern int bdos_entry @6;

char *topmem()
{
	
//   char *p,topofstack;
//    p=&topofstack;
//    return (p-300);

	return (bdos_entry-400);
}

/*
 *  return true if enviroment seems to be ok
 */

sysok()
{
   char *p;


//#asm
//    mvi     c,12	;check version number
//    call    5
//    ani     0f0h
//    cpi     20h 	;ver 2.x?
//    jnz     nogood
//    mov     a,h
//    ora     a		;cp/m?
//    jz	    ok
//nogood:
//    lxi     h,0 	;false if no good
//    pop     d
//    ret
//ok:
//#endasm
//    p=1;
//    return (*p==3);    /* check if xsub or despool is active */


// This tool is mostly based on BIOS calls,
// we take the risk of removing the CP/M version check

	if ((bdos(CPM_VERS,0) & 0xF0) != 0x20)
		return(0);

    p=(char *)1;
    return (*p==3);    /* check if xsub or despool is active */
}


/*
 *  bios call
 *  will only work with the original small C
 *  other C compilers will usually have the argument sequence reversed
 */

//bios(int fun,int arg)
//{   char *ofs;
//    ofs=(fun-1)*3;
//#asm
//    pop     de			;ofs
//    pop     hl			;ret
//    pop     bc			;arg
//    push    bc
//    push    hl
//    push    de
//    ld      hl,(1)		;get pointer to bios
//    add     hl,de		;add offset
//    ld      de,retn1
//    push    de
//    ld      d,b			;arg in de too
//    ld      e,c
//    jp      (hl)		;go
//retn1:
//    ex      de,hl
//    ld      l,a
//    ld      h,0
//    pop     bc			;ofs
//    push    bc
//    ld      a,c
//    cp      (9-1)*3		;select disk function?
//    jr      z,retn2
//    cp      (16-1)*3	;sector translate?
//    jr		nz,retn3
//retn2:
//    ex      de,hl		;value came in hl
//retn3:
//#endasm
//}


/*
 *  return console status, true if ready
 */

int conolest()
{   return (bios(2,0,0));
}

/*
 *  get character from console, no echo
 */

int conin()
{   return (bios(3,0,0));
}

/*
 *  put character to console
 *  no character conversion should be performed
 */

void conout(char ch)
{   bios(4,ch,0);
}


/*
 *  select drive, drive name is 'a','b' etc.
 *  set values for track, sector counts and first sector
 *  return true if ok
 *  to find out what this version does, 
 *  refer to the cp/m 2.0 alteration guide
 */

char seldrv(char drv,int *pt,int *ps,int *pf)	    /* where to put track, sector and firstsector */
{   int *dph,*spt,*dsm,*off,halfsecs,trks;
    char *dpb,*bls;
    if ((dph=(int *)biosh(9,drv-'a',0))==0) return (0);
    xlt=(char *)dph[0]; 	    /* look at disk parameter header */
    dpb=(char *)dph[5];
    if (xlt) *pf=xlt[0];
    else *pf=255&biosh(16,0,0);   //sector translate
    spt=(int *)&dpb[0]; *ps=*spt;  /* and at disk parameter block too */
    bls=(char *)&dpb[2];
    dsm=(int *)&dpb[5];
    off=(int *)&dpb[13];
    /* this is tricky since unsigned divide isn't supported */
    halfsecs=(*dsm+1)<<(*bls-1);
    trks=((halfsecs/(*spt))*2)+(((halfsecs%(*spt))+(*spt-1))/(*spt));
    *pt=trks+*off;
    return (1);
}


/*
 *  increment track, 0 if no more
 */

nextt(int *t)
{   if ((++*t)>=tracks) return (*t=0);
    return (1);
}


/*
 *  increment sector, 0 if no more
 */

int nexts(int *s)
{   if ((++*s)>(sectors-1+firstsector)) {
	*s=firstsector;
	return (0);
    }
    return (1);
}


/*
 *  increment track and sector, 0 if no more
 */

next(int *t,int *s)
{   if (nexts(s)) return (1);
    return (nextt(t));
}


/*
 *  return next logical track/sector
 *  replace by a call to next() if not required
 *  cp/m version sets sector number according to translate table
 */

void lnext(int *t,int *s)
{   int l;
    if (xlt) {
	l=0;
	while (xlt[l++]!=*s);
	if (l>=sectors) {
	    nextt(t);
	    l=0;
	}
	*s=xlt[l];
    } else next(t,s);
}

/*
 *  reset the currently selected drive
 *  used prior to any other operation on a drive
 *  no error code is returned
 */

void rstdrv()
{   bios(8,0,0);
}

/*
 *  read sector, false if error
 */

char sread(int trk,int sec,char *adr)
{   bios(10,trk,0);
    bios(11,sec,0);
    bios(12,adr,0);
    if (bios(13,1,0)) return (0);
    return (1);
}

/*
 *  write sector, false if error
 */

char twrite(int trk,int sec,char *adr)
{   bios(10,trk,0);
    bios(11,sec,0);
    bios(12,adr,0);
    if (bios(14,1,0)) return (0);
    return (1);
}


/*
 *  some small C library functions need redefinition
 */

//#asm
//ccgo:
//    ret
//;
//qzexit:
//    jmp     0
//#endasm





/*
 *  make character printing
 */

char makep(char ch)
{   ch=ch&127;
    if ((ch>=' ')&(ch<127)) return (ch);
    return (' ');
}


/*
 *  put string
 */

void puts(char *str)
{   while (*str) conout(*str++);
}


/*
 *  put digit
 */

void putdig(int d)
{   if ((d=d&15)>9) conout(d-10+'a');
    else conout(d+'0');
}


/*
 *  put decimal number,
 *  return number of digits printed
 */


int putnum(int n)
{   int d;
    if (n>9) d=putnum(n/10);
    else d=0;
    putdig(n%10);
    return (d+1);
}


/*
 *  put track and sector numbers
 */

void putsec(int t,int s)
{   puts("track "); putnum(t);
    puts(" sector "); putnum(s);
}


/*
 *  put hex byte
 */

void putbyte(int n)
{   putdig(n>>4);
    putdig(n);
}


/*
 *  put spaces
 */

void putsps(int n)
{   while (n--) conout(' ');
}


/*
 *  new line
 */

void nl()
{   conout(CR); conout(LF);
}


/*
 *  put newlines
 */

void putnls(int n)
{   while (n--) nl();
}


/*
 *  erase previous character
 */

void unput()
{   conout(BS); conout(' '); conout(BS);
}


/*
 *  move cursor to home position
 */

void gohome()
{   if (home>=128) conout(ESC);
    conout(home & 127);
}


/*
 *  convert decimal digit, -1 if error
 */

char makdec(signed char ch)
{   if ((ch=ch-'0')<0) return (-1);
    if (ch<=9) return (ch);
    return (-1);
}


/*
 *  convert hex digit, -1 if error
 */

char makhex(char ch)
{   if (ch<'0') return (-1);
    if (ch<='9') return (ch-'0');
    if ((ch=ch-'a'+10)<10) return (-1);
    if (ch<=15) return (ch);
    return (-1);
}


/*
 *  see if character can be found in a string
 */

int member(char ch,char *str)
{   while (*str) if (*str++==ch) return (1);
    return (0);
}


/*
 *  get character, no echo
 *  upper case is converted to lower
 */

char conlower()
{   char ch;
    ch=conin();
    if ((ch>='A')&(ch<='Z')) ch=ch+'a'-'A';
    return (ch);
}


/*
 *  get single character command,
 *  enter with possible commands and default command
 */

getcmd(char *cmds,char dfl)
{   char cmd,ch;
    while (1) {
	if ((cmd=conlower())==CR) cmd=dfl;
	if (member(cmd,cmds)==0) continue;
	conout(cmd);
	ch=0;
	while (ch!=BS) if ((ch=conlower())==CR) return (cmd);
	unput();
    }
}


/*
 *  ask for continue or quit, true if continue
 */

cont()
{   char cmd;
    nl(); puts("continue or quit ?");
    cmd=getcmd("cq",'c'); nl();
    return (cmd=='c');
}



/*
 *  test for break, true if yes
 */

tstbrk()
{   if (conolest()) {
	nl(); puts("break");
	if (cont()==0) return (1);
    }
    return (0);
}

/*
 *  adapt to terminal
 */

adapt()
{   nl(); nl(); puts("type character to move cursor home..");
    putbyte(home=conin());
    if (home==ESC) {
	puts(".."); putbyte(home=conin());
	home=home+128;
    }
    nl();
}


/*
 *  get drive name, enter with default drive
 */

getdrv(char dfl)
{   return (getcmd("abcdefgh",dfl));
}


/*
 *  select two drives
 *  parameters of drives selected must be equivalent
 */

seltwo(char *d1,char *d2,char *text1,char *text2)
{   int dtracks,dsectors,dfirstsector;
    nl(); nl(); puts(text1);
    *d1=dfldrv=getdrv(dfldrv);
    nl(); puts(text2);
    *d2=getdrv(dfldrv);
    seldrv(*d1,&tracks,&sectors,&firstsector);	   /* establish constants */
    rstdrv();
    seldrv(*d2,&dtracks,&dsectors,&dfirstsector);
    rstdrv();
    if ((dtracks!=tracks)|(dsectors!=sectors)|(dfirstsector!=firstsector)) {
	nl(); puts("drives are not compatible"); nl();
	return (0);
    }
    return (1);
}


/*
 *  return skewed sector number, skew factor is two.
 *  this function is not equivalent to lnext().
 *  the only purpose of this function is to optimize
 *  scan and backup operations.
 *  hard disks will operate faster without a skew.
 *  hard disk is assumed if there are more than 100 tracks (!).
 */

int qskew(int sec)
{   if (tracks>100) return (sec);
    sec=sec+sec-firstsector;
    if (sec>(sectors-1+firstsector))
	return (sec-sectors+((sectors&1)==0));
    return (sec);
}


/*
 *  compare two sectors
 *  false if not equal
 */

eqsec(char *sec1,char *sec2)
{   int n;
    n=0;
    while (n<SECLEN) {
	if (sec1[n]!=sec2[n]) return (0);
	++n;
    }
    return (1);
}


/*
 *  read one track
 *  assume that drive is already selected
 */

readtrk(int trk,char *adr)
{   int sec;
    char cmd;
    sec=firstsector;
    while (1) {
	if (tstbrk()) return (0);
	if (sread(trk,qskew(sec),adr)==0) {
	    nl(); puts("error reading "); putsec(trk,qskew(sec));
	    nl(); puts("continue, retry or quit ?");
	    cmd=getcmd("qcr",'r');
	    if (cmd=='r') {
		sread(0,firstsector,vfybuf); /* reposition */
		continue;
	    }
	    dfltrk=trk; dflsec=qskew(sec);
	    if (cmd=='q') return (0);
	}
	adr=adr+SECLEN;
	if (nexts(&sec)==0) return (1);
    }
}

/*
 *  write one track
 */

writetrk(int trk,char *adr)
{   int sec;
    sec=firstsector;
    while (1) {
	if (tstbrk()) return (0);
	if (twrite(trk,qskew(sec),adr)==0) {
	    nl(); puts("error writing "); putsec(trk,qskew(sec)); nl();
	    return (0);
	}
	adr=adr+SECLEN;
	if (nexts(&sec)==0) return (1);
    }
}


/*
 *  get a unsigned decimal number
 *  enter with default, minimun and maximum values
 */

int getnum(int dfl,int min,int max)
{   int n,digits,newn; char ch;
    if (dfl>max) dfl=max;
    n=digits=0;
    while (1) {
	if ((ch=conlower())==CR) {
	    if (digits) {
		if (n>=min) break;
	    } else digits=putnum(n=dfl);
	} else if (ch==BS) {
	    if (digits) {
		--digits; n=n/10;
		unput();
	    }
	} else if (((makdec(ch))>=0) & ((newn=n*10+makdec(ch))<=max)) {
	    if ((newn>0)|(digits==0)) {
		++digits; n=newn;
		conout(ch);
	    }
	}
    }
    return (n);
}



/*
 *  get track and sector numbers
 */

getsec(int *t,int *s)
{   nl(); puts("track (0-"); putnum(tracks-1); puts(") ?");
    *t=getnum(*t,0,tracks-1);
    nl(); puts("sector ("); putnum(firstsector); puts("-"); 
    putnum(sectors-1+firstsector); puts(") ?");
    *s=getnum(*s,firstsector,sectors-1+firstsector);
}


/*
 *  get drive name, select and reset
 *  0 if cannot select
 */

getsel()
{   if (seldrv((dfldrv=getdrv(dfldrv)),&tracks,&sectors,&firstsector)==0) {
	nl(); puts("drive not ready"); nl();
	return (0);
    }
    rstdrv();
    return (1);
}


/*
 *  print byte in patch table
 *  returns -1 if end of line
 *	     0 if end of page
 *	     1 otherwise
 */

tbyte(int pos)
{   putbyte(secbuf[pos++]); /* print byte */
    puts(" "); 
    if (pos%16) return (1);
    puts(" '"); 	    /* print ascii equivalents */
    pos=pos-16;
    while (1) {
	conout(makep(secbuf[pos++]));
	if ((pos%16)==0) break;
    }
    puts("'"); nl();
    if (pos==SECLEN) return (0);
    putbyte(pos);	    /* print address too */
    puts("  ");
    return (-1);
}


/*
 *  position cursor during patch
 *  position is returned
 */

poscur(int pos)
{   int n;
    gohome(); putnls(6); puts("00  ");
    n=pos/16; while (n--) conout(LF);
    n=(pos/16)*16; while (n<pos) tbyte(n++);
    return (pos);
}


/*
 *  read and display one sector during patch
 */

 int pread()
 {  char *p;
    int pos;
    pos=0;
    while (pos<SECLEN) secbuf[pos++]=0;
    gohome(); putnls(2);
    if (sread(dfltrk,dflsec,secbuf)==0) puts("error reading ");
    putsec(dfltrk,dflsec);
    putsps(18); nl(); putsps(24);
    poscur(pos=0);
    while (tbyte(pos++));
    return (poscur(0));
 }


/*
 *  write one sector during patch
 *  verify by reading back
 */

int pwrite()
{   int c;
    gohome(); putnls(2); c=cont();
    gohome(); putnls(3);
    if (c) {
	if (twrite(dfltrk,dflsec,secbuf)==0) puts("error writing");
	else if (sread(dfltrk,dflsec,vfybuf)==0) puts("error reading back");
	else if (eqsec(secbuf,vfybuf)==0) puts("error on read verify");
    }
    putsps(22);
    return (poscur(0));
}


/*
 *  patch one byte, update screen
 *  and return new position
 */

patchpos(int pos,int new)
{   int tpos;
    secbuf[pos]=new;
    tpos=pos++; 
    if (pos>=SECLEN) pos=0;
    while (tbyte(tpos++)>0);
    return (pos);
}


/*
 *  shift one bit at current position
 */

int pshift(int pos)
{   int tpos,byte,carry;
    tpos=pos;
    carry=0;
    while (1) {
	byte=secbuf[tpos]&255;
	secbuf[tpos]=(byte>>1)|(carry<<7);
	carry=byte&1;
	if (tbyte(tpos++)==0) break;
    }
    return (poscur(pos));
}


/*
 *  next sector
 */

int pnsect()
{   next(&dfltrk,&dflsec);
    return (pread());
}

/*
 *  next logical sector
 *  useful only in systems with mapping between logical and physical
 *  sector numbers
 */

int plsect()
{   lnext(&dfltrk,&dflsec);
    return (pread());
}


/*
 *  set new track/sector address
 */

int psetts()
{   gohome(); putnls(2); putsps(38);
    gohome(); nl();
    getsec(&dfltrk,&dflsec);
    return (pread());
}


/*
 *  one patch command
 *  return new position, -1 if quit
 */

patchcmd(int pos)
{   char cmd,dig;
    cmd=conlower();
    if (cmd=='q') return (-1);
    else if (cmd==' ') pos=(pos+1)%SECLEN;
    else if (cmd==BS)  pos=(pos+SECLEN-1)%SECLEN;
    else if (cmd==CR)  pos=(pos+16)%SECLEN;
    else if (cmd=='s') pos=pshift(pos);
    else if (cmd=='n') pos=pnsect();
    else if (cmd=='l') pos=plsect();
    else if (cmd=='t') pos=psetts();
    else if (cmd=='r') pos=pread();
    else if (cmd=='w') pos=pwrite();
    else if (cmd==QOT) {
	conout(QOT);
	cmd=conin();
	conout(BS);
	if (cmd==BS) tbyte(pos);
	else pos=patchpos(pos,cmd);
    }
    else if ((dig=makhex(cmd))>=0) {
	putdig(dig);
	while ((cmd=conlower())!=BS) if (makhex(cmd)>=0) break;
	conout(BS);
	if (cmd==BS) tbyte(pos);
	else pos=patchpos(pos,(dig<<4)+makhex(cmd));
    }
    return (poscur(pos));
}



/*
 *  select drive during backup
 *  return 0 if something fishy
 */

baksel(char drv,char otherdrv,char *name)
{   int dummy;
    if (drv==otherdrv) {
	nl(); puts("insert "); puts(name); puts(" diskette");
	if (cont()==0) return (0);
    }
    if (seldrv(drv,&dummy,&dummy,&dummy)==0) {
	nl(); puts(name); puts(" drive is not ready"); nl();
	return (0);
    }
    return (1);
}


/*
 *  backup, copy entire diskette
 */

void backup()
{   int trk,ftrk,ltrk;
    char sdrive,ddrive; char *adr;
    if (seltwo(&sdrive,&ddrive,
	       "source diskette in drive (a-h) ?",
	       "destination diskette in drive (a-h) ?")==0) return;
    if (cont()==0) return;
    trk=-1; 
    while (nextt(&trk)) {
	adr=botmem();
	ftrk=trk;
	if (baksel(sdrive,ddrive,"source")==0) return;
	while (1) {
	    if (readtrk(trk,adr)==0) return;
	    ltrk=trk;
	    adr=adr+(SECLEN*sectors);
	    if ((adr+(SECLEN*sectors))>=topmem()) break;
	    if (nextt(&trk)==0) break;
	}
	adr=botmem(); trk=ftrk;
	if (baksel(ddrive,sdrive,"destination")==0) return;
	while (1) {
	    if (writetrk(trk,adr)==0) {
		puts("backup aborted"); nl();
		return;
	    }
	    if (trk>=ltrk) break;
	    nextt(&trk);
	    adr=adr+(SECLEN*sectors);
	}
    }
    nl(); puts("backup finished"); nl();
}


/*
 *  compare contents of two diskettes
 */

void compare()
{   int trk,sec;
    char drive1,drive2;
    char *adr1,*adr2;
    if (seltwo(&drive1,&drive2,
	       "compare diskette in drive (a-h) ?",
	       "to diskette in drive (a-h) ?")==0) return;
    if (cont()==0) return;
    trk=-1; 
    while (nextt(&trk)) {
	adr1=botmem();
	adr2=adr1+(SECLEN*sectors);
	if (baksel(drive1,drive2,"first")==0) return;
	if (readtrk(trk,adr1)==0) return;
	if (baksel(drive2,drive1,"second")==0) return;
	if (readtrk(trk,adr2)==0) return;
	sec=firstsector-1;
	while (nexts(&sec)) {
	    if (eqsec(adr1,adr2)==0) {
		dfltrk=trk; dflsec=qskew(sec);
		nl(); puts("compare error in "); putsec(dfltrk,dflsec);
	    }
	    adr1=adr1+SECLEN; adr2=adr2+SECLEN;
	}
    }
    nl(); puts("compare finished"); nl();
}


/*
 *  patch
 */

void patch()
{   int pos;
    nl(); nl(); puts("patch diskette in drive (a-h) ?");
    if (getsel()==0) return;
    if (dflsec<firstsector) dflsec=firstsector;
    getsec(&dfltrk,&dflsec);
    putnls(60); gohome();
    puts("<sp>=next-byte, <bs>=previous-byte, <cr>=next-line, <hex>, ',"); 
    nl();
    puts("next, logical-next, trk-sec, re-read, shift-bit, write or quit ?");
    putnls(4);
    puts("    +0 +1 +2 +3 +4 +5 +6 +7 +8 +9 +a +b +c +d +e +f");
    puts("   0123456789abcdef");
    pos=pread();
    while ((pos=patchcmd(pos))>=0);
    putnls(60);
}


/*
 *  scan: check all sectors for crc errors etc.
 */

void scan()
{   int trk,sec;
    nl(); nl(); puts("scan diskette in drive (a-h) ?");
    if (getsel()==0) return;
    if (cont()) {
	nl();
	trk=0; sec=firstsector-1;
	while (next(&trk,&sec)) {
	    if (tstbrk()) return;
	    if (sec==firstsector) {
		puts("track "); putnum(trk); conout(CR);
	    }
	    if (sread(trk,qskew(sec),vfybuf)==0) {
		puts("the dubious quality of ");
		putsec(trk,qskew(sec)); puts(" has been detected"); nl();
		dfltrk=trk; dflsec=qskew(sec);
	    }
	}
	puts("scan finished"); nl();
    }
}


/*
 *  test a diskette by writing a pattern first,
 *  and then reading it back
 */

void test()
{   int trk,sec,n;
    char *adr,*p;
    nl(); nl(); puts("diskette to test in drive (a-h) ?");
    if (getsel()==0) return;
    nl(); puts("the test will destroy the contents of the diskette");
    if (cont()==0) return;
    p="**** diskdoc **** test pattern *"; /* length is 32 */
    n=0;	    /* fill pattern to compare to */
    while (n<SECLEN) {
	secbuf[n]=p[n&31];
	++n;
    }
    puts("does diskette already contain test pattern (y-n) ?");
    if (getcmd("yn",'n')=='n') {
	nl(); puts("writing test pattern...");
	trk=0; sec=firstsector-1;
	while (next(&trk,&sec)) {
	    secbuf[0]=trk; secbuf[1]=qskew(sec);
	    if (tstbrk()) return (0);
	    if (twrite(trk,qskew(sec),secbuf)==0) {
		nl(); puts("error writing "); putsec(trk,qskew(sec)); nl();
		return (0);
	    }
	}
    }
    trk=-1;	    /* read test */
    nl(); puts("reading test pattern...");
    while (nextt(&trk)) {
	if (readtrk(trk,botmem())==0) return;
	sec=firstsector-1; adr=botmem();
	while (nexts(&sec)) {
	    secbuf[0]=trk; secbuf[1]=qskew(sec);
	    if (eqsec(adr,secbuf)==0) {
		dfltrk=trk; dflsec=qskew(sec);
		nl(); puts("bad test pattern in "); putsec(dfltrk,dflsec);
	    }
	    adr=adr+SECLEN;
	}
    }
    nl(); puts("test completed"); nl();
}


/*
 *  zero diskette contents
 */

void zero()
{   int trk,trkmax,n,m;
    char *adr;
    nl(); nl(); puts("zero diskette in drive (a-h) ?");
    if (getsel()==0) return;
    nl(); puts("zero how many tracks (0-"); putnum(tracks-1); puts(") ?");
    trkmax=getnum(tracks-1,0,tracks-1);
    if (cont()==0) return;
    n=0; adr=botmem(); m=SECLEN*sectors;
    while (n<m) adr[n++]=VOID;
    trk=0;
    while (trk<trkmax) {
	if (writetrk(trk,botmem())==0) return;
	nextt(&trk);
    }
    nl();
}





/*
 *  begin
 */

void main()
{   char cmd;
    dfldrv='a'; dfltrk=0; dflsec=0;
    home=DHOME;
    if (sysok()==0) {
		nl(); puts("Unsupported CP/M configuration");
		exit(0);
	}
    nl(); puts("welcome to the world of diskdoc "); 
    nl(); puts(VERSION); nl(); puts(WHOMADEIT);
    nl(); nl(); puts("kindly enter your request");
    cmd=0;
    while (cmd!='e') {
	nl();
	puts("adapt, backup, compare, exit, patch, scan, test or zero ?");
	cmd=getcmd("abcepstz",'e');
	if (cmd=='a') adapt();
	else if (cmd=='b') backup();
	else if (cmd=='c') compare();
	else if (cmd=='p') patch();
	else if (cmd=='s') scan();
	else if (cmd=='t') test();
	else if (cmd=='z') zero();
	else {
	    nl(); puts("re-insert the system diskette and type <cr> ");
	    if (conlower()!=CR) cmd=0;
	    nl();
	}
    }
    nl(); puts("thanks for having consulted diskdoc");
    exit(0);
}



/*
 *  the small C runtime library (as shown in the Dr. Dobbs
 *  September 1980 issue and as distributed by The Code
 *  Works) must be named ddoclib and edited
 *  so that it contains the primitive functions only:
 *  ccgchar,ccgint,ccpchar,...,ccmult,ccdiv
 */

//#include ddoclib
