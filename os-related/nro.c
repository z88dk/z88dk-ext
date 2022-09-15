
// zcc +cpm -create-app -O3 nro.c
// zcc +cpm -create-app -compiler=sdcc -SO3 --max-allocs-per-node400000 nro.c

// The z88dk adapted version can also be built with GCC.
// z88dk provides also the standard file redirection, choose between =file or >file, they should be similar
// the fcntl libraries in z88dk provide also an optional support for devices (e.g. >lst:)
 

/*
 *	Word Processor
 *	similar to Unix NROFF or RSX-11M RNO -
 *	adaptation of text processor given in
 *	"Software Tools", Kernighan and Plauger.
 *
 *	Stephen L. Browning
 *	5723 North Parker Avenue
 *	Indianapolis, Indiana 46220
 */

//#include "a:bdscio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "nro.h"


#define OK 0		/* General purpose "no error" return value */
#define EOS 0

//#define CPMEOF 0x1a	/* CP/M End-of-text-file marker (sometimes!)  */

#define FALSE 0
#define TRUE 1


struct docctl dc;
struct page pg;
FILE *oub;
FILE *pout;
struct cout co;
FILE *sofile[NFILES];	/* input file buffers	*/
struct macros mac;



/*
 *	underline a line
 */

void underl(char *p0,char *p1,int size)
{
	int i,j;

	j = 0;
	for (i=0; (p0[i] != '\n') && (j < size-1); ++i) {
		if (p0[i] >= ' ' && p0[i] <= '~') {
			if (isalpha(p0[i]) || isdigit(p0[i]) || dc.cuval > 0) {
				p1[j++] = '_';
				p1[j++] = '\b';
			}
		}
		p1[j++] = p0[i];
	}
	p1[j++] = '\n';
	p1[j] = EOS;
	while (*p1 != EOS) *p0++ = *p1++;
	*p0 = EOS;
}



/*
 *	insert bold face text
 */

void bold(char *p0,char *p1,int size)
{
	int i, j;

	j = 0;
	for (i=0; (p0[i] != '\n') && (j < size-1); ++i) {
		if (isalpha(p0[i]) || isdigit(p0[i])) {
			p1[j++] = p0[i];
			p1[j++] = '\b';
		}
		p1[j++] = p0[i];
	}
	p1[j++] = '\n';
	p1[j] = EOS;
	while (*p1 != EOS) *p0++ = *p1++;
	*p0 = EOS;
}



/*
 *	find minimum of two integer
 */

int min(int v1,int v2)
{
	return((v1 < v2) ? v1 : v2);
}



/*
 *	find maximum of two integers
 */

int max(int v1,int v2)
{
	return((v1 > v2) ? v1 : v2);
}



/*
 *	compute width of character string
 */

int width(char *s)
{
	int w;

	w = 0;
	while (*s != EOS) {
		if (*s == '\b') --w;
		else if (*s != '\n' && *s != '\r') ++w;
		++s;
	}
	return(w);
}



/*
 *	expand title buffer to include character string
 */

void expand(char *p0,char c,char *s)
{
	char tmp[MAXLINE];
	char *p, *q, *r;

	p = p0;
	q = tmp;
	while (*p != EOS) {
		if (*p == c) {
			r = s;
			while (*r != EOS) *q++ = *r++;
		}
		else *q++ = *p;
		++p;
	}
	*q = EOS;
	strcpy(p0,tmp);		/* copy it back */
}


/*
 *	get non-blank word from p0 into p1.
 *	return number of characters processed.
 */

int getwrd(char *p0,char *p1)
{
	int i;
	char *p, c;

	i = 0;
	while (*p0 == ' ' || *p0 == '\t') {
		++i;
		++p0;
	}
	p = p0;
	while (*p0 != ' ' && *p0 != EOS && *p0 != '\t') {
		if (*p0 == '\n' || *p0 == '\r') break;
		*p1 = *p0++;
		++p1;
		++i;
	}
	c = *(p1-1);
	if (c == '"') c = *(p1-2);
	if (c == '?' || c == '!') {
		*p1++ = ' ';
		++i;
	}
	if (c == '.' && (*p0 == '\n' || *p0 == '\r' || islower(*p))) {
		*p1++ = ' ';
		++i;
	}
	*p1 = EOS;
	return(i);
}



/*
 *	print character with test for printer
 */

#define prchar(c,f) fputc (c,f)

//void prchar(char c,FILE *fp)
//{
//	if (co.lpr == TRUE) {
//		bdos(5,c);
//	}
//	else {
//		fputc(c,fp);
//	}
//}



/*
 *	output a null terminated string to the file
 *	specified by pbuf.
 */

void putlin(char *p,FILE *pbuf)
{
	while (*p != EOS) prchar(*p++,pbuf);
}



/*
 *	skips the number of lines specified by n.
 */

void skip(int n)
{
	int i;

	if (dc.prflg == TRUE && n > 0) {
		for(i=0; i<n; ++i) {
			prchar('\n',pout);
		}
		prchar('\r',pout);
	}
}



/*
 *	center title text into print buffer
 */



void justcntr(char *p,char *q,int limit[])
{
	int len;

	len = width(p);
	q = &q[(limit[RIGHT] + limit[LEFT] - len) >> 1];
	while (*p != EOS) *q++ = *p++;
}



/*
 *	left justify title text into print buffer
 */

void justleft(char *p,char *q,int limit)
{
	q = &q[limit];
	while (*p != EOS) *q++ = *p++;
}


/*
 *	right justify title text into print buffer
 */

void justrite(char *p,char *q,int limit)
{
	int len;

	len = width(p);
	q = &q[limit - len];
	while (*p != EOS) *q++ = *p++;
}



/*
 *	convert integer to decimal ascii string
 */

int itoda(int value,char *p,int size)
{
	char c[7];
	int i, j;
	int aval;

	aval = abs(value);
	c[0] = EOS;
	i = 1;
	do {
		c[i++] = (aval % 10) + '0';
		aval /= 10;
	} while (aval > 0 && i <= size);
	if (value < 0 && i <= size) c[i++] = '-';
	for (j=0; j<i; ++j) *p++ = c[i-j-1];
	return(i);
}



/*
 *	get field from title
 */

char *getfield(char *p,char *q,char delim)
{
	while (*p != delim && *p != '\r' && *p != '\n' && *p != EOS) {
		*q++ = *p++;
	}
	*q = EOS;
	if (*p == delim) ++p;
	return(p);
}



/*
 *	put out title or footer
 */

void puttl(char *p,int lim[],int pgno)
{
	int i;
	char pn[8];
	char t[MAXLINE];
	char h[MAXLINE];
	char delim;

	itoda(pgno,pn,6);
	for (i=0; i<MAXLINE; ++i) h[i] = ' ';
	delim = *p++;
	p = getfield(p,t,delim);
	expand(t,dc.pgchr,pn);
	justleft(t,h,lim[LEFT]);
	p = getfield(p,t,delim);
	expand(t,dc.pgchr,pn);
	justcntr(t,h,lim);
	p = getfield(p,t,delim);
	expand(t,dc.pgchr,pn);
	justrite(t,h,lim[RIGHT]);
	for (i=MAXLINE-4; h[i] == ' '; --i) h[i] = EOS;
	h[++i] = '\n';
	h[++i] = '\r';
	h[++i] = EOS;
	if (strlen(h) > 2) {
		for (i=0; i<pg.offset; ++i) prchar(' ',pout);
	}
	putlin(h,pout);
}



/*
 *	put out page header
 */

void phead()
{
	pg.curpag = pg.newpag;
	if (pg.curpag >= pg.frstpg && pg.curpag <= pg.lastpg) {
		dc.prflg = TRUE;
	}
	else {
		dc.prflg = FALSE;
	}
	++pg.newpag;
	if (dc.prflg == TRUE) {
		if (pg.m1val > 0) {
			skip(pg.m1val - 1);
			if ((pg.curpag % 2) == 0) {
				puttl(pg.ehead,pg.ehlim,pg.curpag);
			}
			else {
				puttl(pg.ohead,pg.ohlim,pg.curpag);
			}
		}
		skip(pg.m2val);
	}
	/*
	*	initialize lineno for the next page
	*/
	pg.lineno = pg.m1val + pg.m2val + 1;
}



/*
 *	split overstrikes (backspaces) into seperate buffer
 */

char strkovr(char *p,char *q)
{
	char *pp;
	int bsflg;

	bsflg = FALSE;
	pp = p;
	while (*p != EOS) {
		*q = ' ';
		*pp = *p;
		++p;
		if (*p == '\b') {
			if (*pp >= ' ' && *pp <= '~') {
				bsflg = TRUE;
				*q = *pp;
				++p;
				*pp = *p;
				++p;
			}
		}
		++q;
		++pp;
	}
	*q++ = '\r';
	*q = *pp = EOS;
	return(bsflg);
}



/*
 *	put out page footer
 */

void pfoot()
{
	if (dc.prflg == TRUE) {
		skip(pg.m3val);
		if (pg.m4val > 0) {
			if ((pg.curpag % 2) == 0) {
				puttl(pg.efoot,pg.eflim,pg.curpag);
			}
			else {
				puttl(pg.ofoot,pg.oflim,pg.curpag);
			}
			skip(pg.m4val - 1);
		}
	}
}



/*
 *	put out line with proper spacing and indenting
 */

void put(char *p)
{
	char os[MAXLINE];
	int j;

	if (pg.lineno == 0 || pg.lineno > pg.bottom) {
		phead();
	}
	if (dc.prflg == TRUE) {
		if (!dc.bsflg) {
			if (strkovr(p,os) == TRUE) {
				for (j=0; j<pg.offset; ++j) prchar(' ',pout);
				for (j=0; j<dc.tival; ++j) prchar(' ',pout);
				putlin(os,pout);
			}
		}
		for (j=0; j<pg.offset; ++j) prchar(' ',pout);
		for (j=0; j<dc.tival; ++j) prchar(' ',pout);
		putlin(p,pout);
	}
	dc.tival = dc.inval;
	skip(min(dc.lsval-1,pg.bottom-pg.lineno));
	pg.lineno = pg.lineno + dc.lsval;
	if (pg.lineno > pg.bottom) pfoot();
}



/*
 *	end current filled line
 */

void brk()
{
	if(co.outp > 0) {
		co.outbuf[co.outp] = '\r';
		co.outbuf[co.outp + 1] = '\n';
		co.outbuf[co.outp + 2] = EOS;
		put(co.outbuf);
	}
	co.outp = 0;
	co.outw = 0;
	co.outwds = 0;
}



/*
 *	delete leading blanks, set tival
 */

void leadbl(char *p)
{
	int i,j;

	brk();
	for (i=0; p[i] == ' '; ++i) ;
	if (p[i] != '\n' && p[i] != '\r') dc.tival = i;
	for (j=0; p[i] != EOS; ++j) p[j] = p[i++];
	p[j] = EOS;
}



/*
 *	Expand escape sequences
 */

void expesc(char *p,char *q)
{
	char *s, *t;

	s = p;
	t = q;
	while (*s != EOS) {
		if (*s != '@') {
			*t++ = *s++;
		}
		else if (*(s+1) == '@') {
			*t++ = *s++;
			++s;
		}
		else if (tolower(*(s+1)) == 'n' && isalpha(*(s+2))) {
			s += 2;
			t += itoda(dc.nr[tolower(*s)-'a'],t,6) - 1;
			++s;
		}
		else {
			*t++ = *s++;
		}
	}
	*t = EOS;
	strcpy(p,q);
}



/*
 *	center a line by setting tival
 */

void center(char *p)
{
	dc.tival = max((dc.rmval + dc.tival - width(p)) >> 1,0);
}



/*
 *	spread words to justify right margin
 */

void spread(char p[],int outp,int nextra,int outwds)
{
	int i,j;
	int nb,ne,nholes;

	if((nextra <= 0) || (outwds <= 1)) return;
	dc.sprdir = ~dc.sprdir;
	ne = nextra;
	nholes = outwds - 1;	/* holes between words */
	i = outp - 1;	/* last non-blank character */
	j = min(MAXLINE-3,i+ne); /* leave room for CR, LF, EOS  */
	while(i < j) {
		p[j] = p[i];
		if(p[i] == ' ') {
			if(dc.sprdir == 0) nb = (ne - 1)/nholes + 1;
			else nb = ne/nholes;
			ne -= nb;
			--nholes;
			for(; nb>0; --nb) {
				--j;
				p[j] = ' ';
			}
		}
		--i;
		--j;
	}
}



/*
 *	put word in output buffer
 */

void putwrd(char *wrdbuf)
{
	int w;
	int last;
	int llval;
	char *p0, *p1;
	int nextra;

	w = width(wrdbuf);
	last = strlen(wrdbuf) + co.outp;
	llval = dc.rmval - dc.tival;
	if(((co.outp > 0) && ((co.outw + w) > llval)) || (last > MAXLINE)) {
		last -= co.outp;
		if(dc.juval == YES) {
			nextra = llval - co.outw + 1;
			/*
			*	Check whether last word was end of
			*	sentence and modify counts so that
			*	it is right justified.
			*/
			if (co.outbuf[co.outp-2] == ' ') {
				--co.outp;
				++nextra;
			}
			spread(co.outbuf,co.outp-1,nextra,co.outwds);
			if((nextra > 0) && (co.outwds > 1)) {
				co.outp += (nextra - 1);
			}
		}
		brk();
	}
	p0 = wrdbuf;
	p1 = co.outbuf + co.outp;
	while(*p0 != EOS) *p1++ = *p0++;
	co.outp = last;
	co.outbuf[co.outp++] = ' ';
	co.outw += w + 1;
	++co.outwds;
}



void text(char *p)
{
	int i;
	char wrdbuf[MAXLINE];

	if (*p == ' ' || *p == '\n' || *p == '\r') leadbl(p);
	expesc(p,wrdbuf);
	if (dc.ulval > 0) {
		/*
		*	Because of the way underlining is handled,
		*	MAXLINE should be declared to be three times
		*	larger than the longest expected input line
		*	for underlining.  Since many of the character
		*	buffers use this parameter, a lot of memory
		*	can be allocated when it may not really be
		*	needed.  A MAXLINE of 180 would allow about
		*	60 characters in the output line to be
		*	underlined (remember that only alphanumerics
		*	get underlined - no spaces or punctuation).
		*/
		underl(p,wrdbuf,MAXLINE);
		--dc.ulval;
	}
	if (dc.cuval > 0) {
		underl(p,wrdbuf,MAXLINE);
		--dc.cuval;
	}
	if (dc.boval > 0) {
		bold(p,wrdbuf,MAXLINE);
		--dc.boval;
	}
	if (dc.ceval > 0) {
		center(p);
		put(p);
		--dc.ceval;
	}
	else if (*p == '\r' || *p == '\n') put(p); /* all blank line */
	else if (dc.fill == NO) put(p);		/* unfilled */
	else {
		while ((i = getwrd(p,wrdbuf)) > 0) {
			putwrd(wrdbuf);
			p += i;
		}
	}
}



/*
 *	Collect macro definition from input stream
 */

int colmac(char *p,char d[],int i)
{
	while (*p != EOS) {
		if (i >= MXMLEN-1) {
			d[i-1] = EOS;
			return(ERR);
		}
		d[i++] = *p++;
	}
	d[i] = EOS;
	return(i);
}




/*
 *	Get macro definition from table
 */

char *getmac(char *name)
{
	int i;

	for (i = mac.lastp; i >= 0; --i) {
		if (!strcmp(name,mac.mnames[i])) {
			return(mac.mnames[i] + 3);
		}
	}
	return(NULL);
}




/*
 *	decodes nro command and returns its associated
 *	value.
 */

int comtyp(char *p,char *m)
{
	char c1, c2;
	char macnam[MNLEN];
	char *s;

	p++;
	/*
	*	First check to see if the command is a macro.
	*	If it is, truncate to two characters and return
	*	expansion in m.  Note that upper and lower case
	*	characters are handled differently for macro names,
	*	but not for normal command names.
	*/
	getwrd(p,macnam);
	macnam[2] = EOS;
	if ((s = getmac(macnam)) != NULL) {
		strcpy(m,s);
		return(MACRO);
	}
	c1 = tolower(*p++);
	c2 = tolower(*p);
	if (c1 == 'b' && c2 == 'o') return(BO);
	if (c1 == 'b' && c2 == 'p') return(BP);
	if (c1 == 'b' && c2 == 'r') return(BR);
	if (c1 == 'b' && c2 == 's') return(BS);
	if (c1 == 'c' && c2 == 'c') return(CC);
	if (c1 == 'c' && c2 == 'e') return(CE);
	if (c1 == 'c' && c2 == 'u') return(CU);
	if (c1 == 'd' && c2 == 'e') return(DE);
	if (c1 == 'e' && c2 == 'f') return(EF);
	if (c1 == 'e' && c2 == 'h') return(EH);
	if (c1 == 'e' && c2 == 'n') return(EN);
	if (c1 == 'f' && c2 == 'i') return(FI);
	if (c1 == 'f' && c2 == 'o') return(FO);
	if (c1 == 'h' && c2 == 'e') return(HE);
	if (c1 == 'i' && c2 == 'n') return(IN);
	if (c1 == 'j' && c2 == 'u') return(JU);
	if (c1 == 'l' && c2 == 's') return(LS);
	if (c1 == 'm' && c2 == '1') return(M1);
	if (c1 == 'm' && c2 == '2') return(M2);
	if (c1 == 'm' && c2 == '3') return(M3);
	if (c1 == 'm' && c2 == '4') return(M4);
	if (c1 == 'n' && c2 == 'e') return(NE);
	if (c1 == 'n' && c2 == 'f') return(NF);
	if (c1 == 'n' && c2 == 'j') return(NJ);
	if (c1 == 'n' && c2 == 'r') return(NR);
	if (c1 == 'o' && c2 == 'f') return(OF);
	if (c1 == 'o' && c2 == 'h') return(OH);
	if (c1 == 'p' && c2 == 'c') return(PC);
	if (c1 == 'p' && c2 == 'l') return(PL);
	if (c1 == 'p' && c2 == 'o') return(PO);
	if (c1 == 'r' && c2 == 'm') return(RM);
	if (c1 == 's' && c2 == 'o') return(SO);
	if (c1 == 's' && c2 == 'p') return(SP);
	if (c1 == 't' && c2 == 'i') return(TI);
	if (c1 == 'u' && c2 == 'l') return(UL);
	return(UNKNOWN);
}



/*
 *	convert ascii character to decimal.
 */

int atod(char c)
{
	return(((c < '0') || (c > '9')) ? -1 : c-'0');
}



/*
 *	convert string to decimal.
 *	processes only positive values.
 */

int ctod(char *p)
{
	int val, d;

	val = 0;
	while(*p != EOS) {
		d = atod(*p++);
		if(d == -1) return(val);
		val = 10 * val + d;
	}
	return(val);
}




/*
 *	skip blanks and tabs in character buffer.
 *	return number of characters skipped.
 */

char *skipbl(char *p)
{
	while (*p == ' ' || *p == '\t') ++p;
	return(p);
}


/*
 *	skip over word and punctuation
 */

char *skipwd(char *p)
{
	while (*p != ' ' && *p != '\t' && *p != '\r' && *p != '\n' && *p != EOS)
		++p;
	return(p);
}




/*
 *	Put macro definition into table
 */

int putmac(char *name,char *p)
{
	if (mac.lastp >= MXMDEF) return(ERR);
	if (mac.emb + strlen(name) + strlen(p) + 1 > &mac.mb[MACBUF]) {
		return(ERR);
	}
	++mac.lastp;
	mac.mnames[mac.lastp] = mac.emb;
	strcpy(mac.emb,name);
	strcpy(mac.emb + strlen(name) + 1,p);
	mac.emb += strlen(name) + strlen(p) + 2;
	return(OK);
}



/*
 *	get character from input file or push back buffer
 */

int ngetc(FILE *infp)
{
	int c;

	if (mac.ppb >= &mac.pbb[0]) {
		c = *mac.ppb--;
	}
	else {
		c = fgetc(infp);
	}
	return(c);
}



/*
 *	retrieve one line of input text
 */

int getlin(char *p,FILE *in_buf)
{
	int i;
	int c;
	char *q;

	q = p;
	for (i=0; i<MAXLINE-1; ++i) {
		c = ngetc(in_buf);
		if (c == CPMEOF || c == EOF) {
			*q = EOS;
			c = strlen(p);
			return(c == 0 ? EOF : c);
		}
		*q++ = c;
		if (c == '\n') break;
	}
	*q = EOS;
	return(strlen(p));
}



/*
 *	Define a macro
 */

void defmac(char *p,FILE *infp)
{
	int i;
	char name[MNLEN];
	char defn[MXMLEN];
	char *q;

	q = skipwd(p);
	q = skipbl(q);
	i = getwrd(q,name);
	if (!isalpha(*name)) {
		puts("***nro: missing or illegal macro definition name\n");
		exit(-1);
	}
	if (i > 2) name[2] = EOS;
	i = 0;
	while (getlin(p,infp) != EOF) {
		if (p[0] == dc.cmdchr && tolower(p[1]) == 'e' && tolower(p[2]) == 'n') {
			break;
		}
		if ((i = colmac(p,defn,i)) == ERR) {
			puts("***nro: macro definition too long\n");
			exit(-1);
		}
	}
	if (putmac(name,defn) == ERR) {
		puts("***nro: macro definition table full\n");
		exit(-1);
	}
}


/*
 *	get header or footer title
 */

void gettl(char *p,char *q,int limit[])
{
	p = skipwd(p);
	p = skipbl(p);
	strcpy(q,p);
	limit[LEFT] = dc.inval;
	limit[RIGHT] = dc.rmval;
}



/*
 *	retrieves optional argument following nro command.
 *	returns positive integer value with sign (if any)
 *	saved in character addressed by p_argt.
 */

int getval(char *p,char *p_argt)
{
	p = skipwd(p);
	p = skipbl(p);
	*p_argt = *p;
	if((*p == '+') || (*p == '-')) ++p;
	return(ctod(p));
}



/*
 *	Push character back into input stream
 */

void putbak(char c)
{
	if (mac.ppb < &mac.pbb[0]) {
		mac.ppb = &mac.pbb[0];
		*mac.ppb = c;
	}
	else {
		if (mac.ppb >= &mac.pbb[MAXLINE-1]) {
			puts("***nro: push back buffer overflow\n");
			exit(-1);
		}
		*++mac.ppb = c;
	}
}



/*
 *	Push back string into input stream
 */

void pbstr(char p[])
{
	int i;

	for (i=strlen(p)-1; i>=0; --i) {
		putbak(p[i]);
	}
}



/*
 *	Evaluate macro expansion
 */

void maceval(char *p,char m[])
{
	int i;
	char *argp[10];
	char c;

	*p++ = EOS;		/* replace command char with EOS */
	/*
	*	initialize argp array to substitute command
	*	string for any undefined argument
	*/
	for (i=0; i<10; ++i) argp[i] = p;
	p = skipwd(p);
	*p++ = EOS;
	for (i=0; i<10; ++i) {
		p = skipbl(p);
		if (*p == '\r' || *p == '\n' || *p == EOS) break;
		if (*p == '\'' || *p == '"') {
			c = *p++;
			argp[i] = p;
			while (*p != c && *p != '\r' && *p != '\n' && *p != EOS) ++p;
			*p++ = EOS;
		}
		else {
			argp[i] = p;
			p = skipwd(p);
			*p++ = EOS;
		}
	}
	for (i=strlen(m)-1; i>=0; --i) {
		if (i > 0 && m[i-1] == '$') {
			if (!isdigit(m[i])) {
				putbak(m[i]);
			}
			else {
				pbstr(argp[m[i]-'0']);
				--i;
			}
		}
		else {
			putbak(m[i]);
		}
	}
}




/*
 *	set parameter and check range
 */

void set(int *param,int val,char type,int defval,int minval,int maxval)
{
	switch(type) {
	case '\r':
	case '\n':
		*param = defval;
		break;
	case '+':
		*param += val;
		break;
	case '-':
		*param -= val;
		break;
	default:
		*param = val;
		break;
	}
	*param = min(*param,maxval);
	*param = max(*param,minval);
}



/*
 *	space vertically n lines
 */

void space(int n)
{
	brk();
	if (pg.lineno > pg.bottom) return;
	if (pg.lineno == 0) phead();
	skip(min(n,pg.bottom+1-pg.lineno));
	pg.lineno += n;
	if (pg.lineno > pg.bottom) pfoot();
}




/*
 *	initialize parameters for nro word processor
 */

void init()
{
	int i;

	dc.fill = YES;
	dc.lsval = 1;
	dc.inval = 0;
	dc.rmval = PAGEWIDTH - 1;
	dc.tival = 0;
	dc.ceval = 0;
	dc.ulval = 0;
	dc.cuval = 0;
	dc.juval = YES;
	dc.boval = 0;
	dc.bsflg = FALSE;
	dc.pgchr = '#';
	dc.cmdchr = '.';
	dc.prflg = TRUE;
	dc.sprdir = 0;
	for (i=0; i<26; ++i) dc.nr[i] = 0;
	pg.curpag = 0;
	pg.newpag = 1;
	pg.lineno = 0;
	pg.plval = PAGELEN;
	pg.m1val = 2;
	pg.m2val = 2;
	pg.m3val = 2;
	pg.m4val = 2;
	pg.bottom = pg.plval - pg.m4val - pg.m3val;
	pg.offset = 0;
	pg.frstpg = 0;
	pg.lastpg = 30000;
	pg.ehead[0] = pg.ohead[0] = '\n';
	pg.efoot[0] = pg.ofoot[0] = '\n';
	for (i=1; i<MAXLINE; ++i) {
		pg.ehead[i] = pg.ohead[i] = EOS;
		pg.efoot[i] = pg.ofoot[i] = EOS;
	}
	pg.ehlim[LEFT] = pg.ohlim[LEFT] = dc.inval;
	pg.eflim[LEFT] = pg.oflim[LEFT] = dc.inval;
	pg.ehlim[RIGHT] = pg.ohlim[RIGHT] = dc.rmval;
	pg.eflim[RIGHT] = pg.oflim[RIGHT] = dc.rmval;
	co.outp = 0;
	co.outw = 0;
	co.outwds = 0;
	//co.lpr = FALSE;
	for (i=0; i<MAXLINE; ++i) co.outbuf[i] = EOS;
	for (i=0; i<MXMDEF; ++i) mac.mnames[i] = NULL;
	mac.lastp = 0;
	mac.emb = &mac.mb[0];
	mac.ppb = NULL;
}



void comand(char *p)
{
	int ct, val;
	int spval;
	int index;
	char argtyp;
	char name[MAXLINE];
	char macexp[MXMLEN];

	ct = comtyp(p,macexp);
	if (ct == UNKNOWN) {
		printf("*** nro: unrecognized command %s\n",p);
		return;
	}
	expesc(p,name);
	val = getval(p,&argtyp);
	switch (ct) {
	case BO: /* bold face */
		set(&dc.boval,val,argtyp,1,0,HUGE);
		dc.cuval = dc.ulval = 0;
		break;
	case BP: /* begin page */
		if(pg.lineno > 0) space(HUGE);
		set(&pg.curpag,val,argtyp,pg.curpag+1,-HUGE,HUGE);
		pg.newpag = pg.curpag;
		break;
	case BR: /* break */
		brk();
		break;
	case BS: /* backspaces in output */
		set(&dc.bsflg,val,argtyp,1,0,1);
		break;
	case CC: /* command character */
		if (argtyp == '\r' || argtyp == '\n') dc.cmdchr = '.';
		else dc.cmdchr = argtyp;
		break;
	case CE: /* center */
		brk();
		set(&dc.ceval,val,argtyp,1,0,HUGE);
		break;
	case CU: /* continuous underline */
		set(&dc.cuval,val,argtyp,1,0,HUGE);
		dc.ulval = dc.boval = 0;
		break;
	case DE: /* define macro */
		defmac(p,sofile[dc.flevel]);
		break;
	case EF: /* even footer */
		gettl(p,pg.efoot,&pg.eflim[0]);
		break;
	case EH: /* even header */
		gettl(p,pg.ehead,&pg.ehlim[0]);
		break;
	case EN: /* end macro definition */
		puts("***nro: missing .de command\n");
		break;
	case FI: /* fill */
		brk();
		dc.fill = YES;
		break;
	case FO: /* footer */
		gettl(p,pg.efoot,&pg.eflim[0]);
		gettl(p,pg.ofoot,&pg.oflim[0]);
		break;
	case HE: /* header */
		gettl(p,pg.ehead,&pg.ehlim[0]);
		gettl(p,pg.ohead,&pg.ohlim[0]);
		break;
	case IN: /* indenting */
		set(&dc.inval,val,argtyp,0,0,dc.rmval-1);
		dc.tival = dc.inval;
		break;
	case JU: /* justify */
		dc.juval = YES;
		break;
	case LS: /* line spacing */
		set(&dc.lsval,val,argtyp,1,1,HUGE);
		break;
	case M1: /* set topmost margin */
		set(&pg.m1val,val,argtyp,2,0,HUGE);
		break;
	case M2: /* set second top margin */
		set(&pg.m2val,val,argtyp,2,0,HUGE);
		break;
	case M3: /* set first bottom margin */
		set(&pg.m3val,val,argtyp,2,0,HUGE);
		pg.bottom = pg.plval - pg.m4val - pg.m3val;
		break;
	case M4: /* set bottom-most margin */
		set(&pg.m4val,val,argtyp,2,0,HUGE);
		pg.bottom = pg.plval - pg.m4val - pg.m3val;
		break;
	case MACRO: /* macro expansion */
		maceval(p,macexp);
		break;
	case NE: /* need n lines */
		brk();
		if ((pg.bottom-pg.lineno+1) < (val*dc.lsval)) {
			space(HUGE);
		}
		break;
	case NF: /* no fill */
		brk();
		dc.fill = NO;
		break;
	case NJ: /* no justify */
		dc.juval = NO;
		break;
	case NR: /* set number register */
		p = skipwd(p);
		p = skipbl(p);
		if (!isalpha(*p)) {
			puts("***nro: invalid or missing number register name\n");
		}
		else {
			index = tolower(*p) - 'a';
			p = skipwd(p);
			val = getval(p,&argtyp);
			set(&dc.nr[index],val,argtyp,0,-HUGE,HUGE);
		}
		break;
	case OF: /* odd footer */
		gettl(p,pg.ofoot,&pg.oflim[0]);
		break;
	case OH: /* odd header */
		gettl(p,pg.ohead,&pg.ohlim[0]);
		break;
	case PC: /* page number character */
		if (argtyp == '\r' || argtyp == '\n') dc.pgchr = EOS;
		else dc.pgchr = argtyp;
		break;
	case PL: /* page length */
		set(&pg.plval,val,argtyp,PAGELEN,
			pg.m1val+pg.m2val+pg.m3val+pg.m4val+1,HUGE);
		pg.bottom = pg.plval - pg.m3val - pg.m4val;
		break;
	case PO: /* page offset */
		set(&pg.offset,val,argtyp,0,0,HUGE);
		break;
	case RM: /* right margin */
		set(&dc.rmval,val,argtyp,PAGEWIDTH,dc.tival+1,HUGE);
		break;
	case SO: /* source file */
		p = skipwd(p);
		p = skipbl(p);
		if (getwrd(p,name) == 0) break;
		if (dc.flevel+1 >= NFILES) {
			puts("***nro: .so commands nested too deeply\n");
			exit(-1);
		}
//		if (fopen(name,&sofile[dc.flevel+1]) == ERR) {
		if ((sofile[dc.flevel+1] = fopen(name,"rb")) == 0) {
			printf("***nro: unable to open %s\n",name);
			exit(-1);
		}
		++dc.flevel;
		break;
	case SP: /* space */
		set(&spval,val,argtyp,1,0,HUGE);
		space(spval);
		break;
	case TI: /* temporary indent */
		brk();
		set(&dc.tival,val,argtyp,0,0,dc.rmval);
		break;
	case UL: /* underline */
		set(&dc.ulval,val,argtyp,0,1,HUGE);
		dc.cuval = dc.boval = 0;
		break;
	}
}



/*
 *	process input files from command line
 */

void profile()
{
	char ibuf[MAXLINE];

	for (dc.flevel=0; dc.flevel>=0; --dc.flevel) {
		while (getlin(ibuf,sofile[dc.flevel]) != EOF) {
			if (ibuf[0] == dc.cmdchr) comand(ibuf);
			else text(ibuf);
		}
		if (dc.flevel > 0) fclose(sofile[dc.flevel]);
	}
	if (pg.lineno > 0) space(HUGE);
}



/*
 *	process switch values from command line
 */

int pswitch(char *p,int *q)
{
	int swgood;

	swgood = TRUE;
	if (*p == '-') {
		switch (tolower(*++p)) {
		case 'b':
			dc.bsflg = TRUE;
			break;
		case 'm':
//			if (fopen(++p,&sofile[0]) == ERR) {
			if ((sofile[0] = fopen(++p,"rb")) == 0) {
				printf("***nro: unable to open file %s\n",p);
				exit(-1);
			}
			profile();
			fclose(sofile[0]);
			break;
		case 'p':
			set(&pg.offset,ctod(++p),'1',0,0,HUGE);
			break;
		case 'v':
			printf("NRO version 1.0\n");
			*q = TRUE;
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			pg.lastpg = ctod(p);
			break;
		default:
			swgood = FALSE;
			break;
		}
	}
	else if (*p == '+') {
		pg.frstpg = ctod(++p);
	}
	else {
		swgood = FALSE;
	}
	if (swgood == FALSE) {
		printf("nro: illegal switch %s\n",p);
		return(ERR);
	}
	return(OK);
}

void main(int argc,char *argv[])
{
	FILE *ifp;
	FILE *ofp;
	int i;
	int swflg;

	swflg = FALSE;
	pout = stdout;
	ifp = ofp = 0;
	init();
	for (i=1; i<argc; ++i) {
		if (*argv[i] == '-' || *argv[i] == '+') {
			if (pswitch(argv[i],&swflg) == ERR) exit(-1);
		}
		else if (*argv[i] == '=') {
			if (ofp == 0) {
//				if (!strcmp(argv[i]+1,"$P")) {
//					ofp = 1;
//					co.lpr = TRUE;
//				}
//				else if ((ofp = fcreat(argv[i]+1,&oub)) == ERR) {
				if ((oub = ofp = fopen(argv[i]+1,"wb")) == 0) {
					printf("nro: cannot create %s\n",argv[i]+1);
					exit(-1);
				}
				else {
					pout = oub;
				}
			}
			else {
				puts("nro: too many output files\n");
				exit(-1);
			}
		}
	}
	for (i=1; i<argc; ++i) {
		if (*argv[i] != '-' && *argv[i] != '+' && *argv[i] != '=') {
			//if ((ifp = fopen(argv[i],&sofile[0])) == ERR) {
			if ((ifp = sofile[0] = fopen(argv[i],"rb")) == 0) {
				printf("nro: unable to open file %s\n",argv[i]);
				exit(-1);
			}
			else {
				profile();
				fclose(sofile[0]);
			}
		}
	}
	if ((ifp == 0 && swflg == FALSE) || argc <= 1) {
		puts("Usage: nro [-n] [+n] [-pxx] [-v] [-b] [-mmacfile] infile ... [=outfile]\n");
		exit(-1);
	}
	if (pout != stdout) {
		//fputc(CPMEOF,pout);
		//fflush(pout);
		fclose(pout);
	}
}

