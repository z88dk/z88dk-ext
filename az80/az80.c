/*
	Adapted for z88dk.  To build:
	zcc +cpm -O3 -create-app -DAMALLOC -oaz80 az80.c



/*
	HEADER:		CUG276;
	TITLE:		Z-80 Cross-Assembler (Portable);
	FILENAME:	AZ80EVAL.C;
	VERS ON:	0.1;
	DATE:		08/27/1988;
	SEE-ALSO:	AZ80.H;
	AUTHORS:	William C. Colley III;
*/

/*
		      Z-80 Cross-Assembler in Portable C

		Copyright (c) 1986-1988 William C. Colley, III

Revision History:

Ver	Date		Description

0.0	JUNE 1988	Derived from my S6 cross-assembler.  WCC3.

0.1	AUG 1988	Fixed a bug in the command line parser that puts it
			into a VERY long loop if the user types a command line
			like "AZ80 FILE.ASM -L".  WCC3 per Alex Cameron.

This file contains the assembler's expression evaluator and lexical analyzer.
The lexical analyzer chops the input character stream up into discrete tokens
that are processed by the expression analyzer and the line assembler.  The
expression analyzer processes the token stream into unsigned results of
arithmetic expressions.
*/

/*  Get global goodies:  */

#include "az80.h"

/*  Get access to global mailboxes defined in AZ80.C:			*/

extern char line[];
extern int filesp, forwd, pass;
extern unsigned pc;
extern FILE *filestk[], *source;
extern TOKEN arg, token;



/*  Get access to global mailboxes defined in AZ80.C:			*/

extern char errcode, line[];//, title[];
extern unsigned address, errors; //, pagelen;



/*  Error handler routine.  If the current error code is non-blank,	*/
/*  the error code is filled in and the	number of lines with errors	*/
/*  is adjusted.							*/

void error(code)
char code;
{
    if (errcode == ' ') { errcode = code;  ++errors; }
    return;
}



/*  Non-fatal error handler routine.  A message gets printed on the	*/
/*  stderr device, and the routine returns.				*/

void warning(msg)
char *msg;
{
    printf("Warning -- %s\n",msg);
    return;
}

int isalph(c)
char c;
{
    return (c >= '?' && c <= '~') || (c >= '#' && c <= '&') ||
	c == '!' || c == '.' || c == ':';
}

int isnum(c)
char c;
{
    return c >= '0' && c <= '9';
}

static int isalnum(c)
char c;
{
    return isalph(c) || isnum(c);
}



int ustrcmp(s,t)
char *s, *t ;
{
    SCRATCH int i;
    char ucs, uct, c ;

    c = *s++ ;
    ucs = ( ( (c) >= 'a' ) && ( (c) <= 'z' ) ) ? (c) - 'a' + 'A' : (c) ;
    c = *t ;
    uct = ( ( (c) >= 'a' ) && ( (c) <= 'z' ) ) ? (c) - 'a' + 'A' : (c) ;

    while (!( i = ucs - uct ) && *t++)
       {
       c = *s++ ; 
       ucs = ( ( (c) >= 'a' ) && ( (c) <= 'z' ) ) ? (c) - 'a' + 'A' : (c) ; 
       c = *t ;
       uct = ( ( (c) >= 'a' ) && ( (c) <= 'z' ) ) ? (c) - 'a' + 'A' : (c) ; 
       }
    return i;
}




/*  Buffer storage for line listing routine.  This allows the listing	*/
/*  output routines to do all operations without the main routine	*/
/*  having to fool with it.						*/

static FILE *list = NULL;

/*  Listing file open routine.  If a listing file is already open, a	*/
/*  warning occurs.  If the listing file doesn't open correctly, a	*/
/*  fatal error occurs.  If no listing file is open, all calls to	*/
/*  lputs() and lclose() have no effect.				*/

void lopen(nam)
char *nam;
{
    //FILE *fopen();
    void fatal_error(), warning();

    if (list) warning(TWOLST);
    else if (!(list = fopen(nam,"w"))) fatal_error(LSTOPEN);
    return;
}



/*  Buffer storage for hex output file.  This allows the hex file	*/
/*  output routines to do all of the required buffering and record	*/
/*  forming without the	main routine having to fool with it.		*/

static FILE *hex = NULL;
static unsigned cnt = 0;
static unsigned addr = 0;
static unsigned sum = 0;
static unsigned buf[HEXSIZE];

/*  Hex file open routine.  If a hex file is already open, a warning	*/
/*  occurs.  If the hex file doesn't open correctly, a fatal error	*/
/*  occurs.  If no hex file is open, all calls to hputc(), hseek(), and	*/
/*  hclose() have no effect.						*/

void hopen(nam)
char *nam;
{
    //FILE *fopen();
    void fatal_error(), warning();

    if (hex) warning(TWOHEX);
    else if (!(hex = fopen(nam,"w"))) fatal_error(HEXOPEN);
    return;
}

/*  Hex file write routine.  The data byte is appended to the current	*/
/*  record.  If the record fills up, it gets written to disk.  If the	*/
/*  disk fills up, a fatal error occurs.				*/

void hputc(c)
unsigned c;
{
    void record();

    if (hex) {
	buf[cnt++] = c;
	if (cnt == HEXSIZE) record(0);
    }
    return;
}

/*  Hex file address set routine.  The specified address becomes the	*/
/*  load address of the next record.  If a record is currently open,	*/
/*  it gets written to disk.  If the disk fills up, a fatal error	*/
/*  occurs.								*/

void hseek(a)
unsigned a;
{
    void record();

    if (hex) {
	if (cnt) record(0);
	addr = a;
    }
    return;
}

/*  Hex file close routine.  Any open record is written to disk, the	*/
/*  EOF record is added, and file is closed.  If the disk fills up, a	*/
/*  fatal error occurs.							*/

void hclose()
{
    void fatal_error(), record();

    if (hex) {
	if (cnt) record(0);
	record(1);
	if (fclose(hex) == EOF) fatal_error(DSKFULL);
    }
    return;
}

void record(typ)
unsigned typ;
{
    SCRATCH unsigned i;
    void fatal_error(), putb();

    putc(':',hex);  putb(cnt);  putb(high(addr));
    putb(low(addr));  putb(typ);
    for (i = 0; i < cnt; ++i) putb(buf[i]);
    putb(low(-sum));  putc('\n',hex);

    addr += cnt;  cnt = 0;

    if (ferror(hex)) fatal_error(DSKFULL);
    return;
}

void putb(b)
unsigned b;
{
    static char digit[] = "0123456789ABCDEF";

    putc(digit[b >> 4],hex);  putc(digit[b & 0x0f],hex);
    sum += b;  return;
}




OPCODE *my_bsearch(lo,hi,nam)
OPCODE *lo, *hi;
char *nam;
{
    SCRATCH int i;
    SCRATCH OPCODE *chk;

    for (;;) {
	chk = lo + (hi - lo) / 2;
	if (!(i = ustrcmp(chk -> oname,nam))) return chk;
	if (chk == lo) return NULL;
	if (i < 0) lo = chk;
	else hi = chk;
    }
}


/*  Operator table search routine.  This routine pats down the		*/
/*  operator table for a given operator and returns either a pointer	*/
/*  to it or NULL if the opcode doesn't exist.				*/

OPCODE *find_operator(nam)
char *nam;
{
    //OPCODE *my_bsearch();

    static OPCODE oprtbl[] = {
	{ REG,				A,		"A"	},
	{ REG,				AF,		"AF"	},
	{ BINARY + LOG1  + OPR,		AND,		"AND"	},
	{ REG,				B,		"B"	},
	{ REG,				BC,		"BC"	},
	{ REG,				C,		"C"	},
	{ REG,				D,		"D"	},
	{ REG,				DE,		"DE"	},
	{ REG,				E,		"E"	},
	{ BINARY + RELAT + OPR,		'=',		"EQ"	},
	{ BINARY + RELAT + OPR,		GE,		"GE"	},
	{ BINARY + RELAT + OPR,		'>',		"GT"	},
	{ REG,				H,		"H"	},
	{ UNARY  + UOP3  + OPR,		HIGH,		"HIGH"	},
	{ REG,				HL,		"HL"	},
	{ REG,				I,		"I"	},
	{ REG,				IX,		"IX"	},
	{ REG,				IY,		"IY"	},
	{ REG,				L,		"L"	},
	{ BINARY + RELAT + OPR,		LE,		"LE"	},
	{ UNARY  + UOP3  + OPR,		LOW,		"LOW"	},
	{ BINARY + RELAT + OPR,		'<',		"LT"	},
	{ REG,				M,		"M"	},
	{ BINARY + MULT  + OPR,		MOD,		"MOD"	},
	{ REG,				NC,		"NC"	},
	{ BINARY + RELAT + OPR,		NE,		"NE"	},
	{ UNARY  + UOP2  + OPR,		NOT,		"NOT"	},
	{ REG,				NZ,		"NZ"	},
	{ BINARY + LOG2  + OPR,		OR,		"OR"	},
	{ REG,				P,		"P"	},
	{ REG,				PE,		"PE"	},
	{ REG,				PO,		"PO"	},
	{ REG,				R,		"R"	},
	{ BINARY + MULT  + OPR,		SHL,		"SHL"	},
	{ BINARY + MULT  + OPR,		SHR,		"SHR"	},
	{ REG,				SP,		"SP"	},
	{ BINARY + LOG2  + OPR,		XOR,		"XOR"	},
	{ REG,				Z,		"Z"	}
    };

    return my_bsearch(oprtbl,oprtbl + (sizeof(oprtbl) / sizeof(OPCODE)),nam);
}


/*  Opcode table search routine.  This routine pats down the opcode	*/
/*  table for a given opcode and returns either a pointer to it or	*/
/*  NULL if the opcode doesn't exist.					*/

OPCODE *find_code(nam)
char *nam;
{
    //OPCODE *my_bsearch();

    static OPCODE opctbl[] = {
	{ ADC,			0x88,	"ADC"	},
	{ ADD,			0x80,	"ADD"	},
	{ CP,			0xa0,	"AND"	},
	{ BIT, 			0xcb40,	"BIT"	},
	{ CALL,			0xc4,	"CALL"	},
	{ NO_ARGS,		0x3f,	"CCF"	},
	{ PSEUDO + ISIF,	IF,	"COND"	},
	{ CP,			0xb8,	"CP"	},
	{ NO_ARGS,		0xeda9,	"CPD"	},
	{ NO_ARGS,		0xedb9,	"CPDR"	},
	{ NO_ARGS,		0xeda1,	"CPI"	},
	{ NO_ARGS,		0xedb1,	"CPIR"	},
	{ NO_ARGS,		0x2f,	"CPL"	},
	{ NO_ARGS,		0x27,	"DAA"	},
	{ PSEUDO,		DB,	"DB"	},
	{ PSEUDO,		DC,	"DC"	},
	{ DEC,			0x05,	"DEC"	},
	{ PSEUDO,		DB,	"DEFB"	},
	{ PSEUDO,		VAR,	"DEFL"	},
	{ PSEUDO,		DB,	"DEFM"	},
	{ PSEUDO,		DS,	"DEFS"	},
	{ PSEUDO,		DW,	"DEFW"	},
	{ NO_ARGS,		0xf3,	"DI"	},
	{ DJNZ,			0x10,	"DJNZ"	},
	{ PSEUDO,		DS,	"DS"	},
	{ PSEUDO,		DW,	"DW"	},
	{ NO_ARGS,		0xfb,	"EI"	},
	{ PSEUDO + ISIF,	ELSE,	"ELSE"	},
	{ PSEUDO,		END,	"END"	},
	{ PSEUDO + ISIF,	ENDIF,	"ENDC"	},
	{ PSEUDO + ISIF,	ENDIF,	"ENDIF"	},
	{ PSEUDO,		EQU,	"EQU"	},
	{ EX,			0xe3,	"EX"	},
	{ NO_ARGS,		0xd9,	"EXX"	},
	{ NO_ARGS,		0x76,	"HALT"	},
	{ PSEUDO + ISIF,	IF,	"IF"	},
	{ IM,			0xed46,	"IM"	},
	{ IN,			0xdb00,	"IN"	},
	{ DEC,			0x04,	"INC"	},
	{ PSEUDO,		INCL,	"INCL"	},
	{ NO_ARGS,		0xedaa,	"IND"	},
	{ NO_ARGS,		0xedba,	"INDR"	},
	{ NO_ARGS,		0xeda2,	"INI"	},
	{ NO_ARGS,		0xedb2,	"INIR"	},
	{ JP,			0xc2,	"JP"	},
	{ JR,			0x20,	"JR"	},
	{ LD,			0x40,	"LD"	},
	{ NO_ARGS,		0xeda8,	"LDD"	},
	{ NO_ARGS,		0xedb8,	"LDDR"	},
	{ NO_ARGS,		0xeda0,	"LDI"	},
	{ NO_ARGS,		0xedb0,	"LDIR"	},
	{ NO_ARGS,		0xed44,	"NEG"	},
	{ NO_ARGS,		0x00,	"NOP"	},
	{ CP,			0xb0,	"OR"	},
	{ PSEUDO,		ORG,	"ORG"	},
	{ NO_ARGS,		0xedbb,	"OTDR"	},
	{ NO_ARGS,		0xedb3,	"OTIR"	},
	{ OUT,			0xd300,	"OUT"	},
	{ NO_ARGS,		0xedab,	"OUTD"	},
	{ NO_ARGS,		0xeda3,	"OUTI"	},
	{ PSEUDO,		PAGE,	"PAGE"	},
	{ POP,			0xc1,	"POP"	},
	{ POP,			0xc5,	"PUSH"	},
	{ BIT,			0xcb80,	"RES"	},
	{ RET,			0xc0,	"RET"	},
	{ NO_ARGS,		0xed4d,	"RETI"	},
	{ NO_ARGS,		0xed45,	"RETN"	},
	{ RLC,			0xcb10,	"RL"	},
	{ NO_ARGS,		0x17,	"RLA"	},
	{ RLC,			0xcb00,	"RLC"	},
	{ NO_ARGS,		0x07,	"RLCA"	},
	{ NO_ARGS,		0xed6f,	"RLD"	},
	{ RLC,			0xcb18,	"RR"	},
	{ NO_ARGS,		0x1f,	"RRA"	},
	{ RLC,			0xcb08,	"RRC"	},
	{ NO_ARGS,		0x0f,	"RRCA"	},
	{ NO_ARGS,		0xed67,	"RRD"	},
	{ RST,			0xc7,	"RST"	},
	{ ADC,			0x98,	"SBC"	},
	{ NO_ARGS,		0x37,	"SCF"	},
	{ BIT,			0xcbc0,	"SET"	},
	{ RLC,			0xcb20,	"SLA"	},
	{ RLC,			0xcb28,	"SRA"	},
	{ RLC,			0xcb38,	"SRL"	},
	{ CP,			0x90,	"SUB"	},
	{ PSEUDO,		TITLE,	"TITLE"	},
	{ PSEUDO,		VAR,	"VAR"	},
	{ CP,			0xa8,	"XOR"	}
    };

    return my_bsearch(opctbl,opctbl + (sizeof(opctbl) / sizeof(OPCODE)),nam);
}


/*  The symbol table is a binary tree of variable-length blocks drawn	*/
/*  from the heap with the calloc() function.  The root pointer lives	*/
/*  here:								*/

static SYMBOL *sroot = NULL;


/*  Listing file close routine.  The symbol table is appended to the	*/
/*  listing in alphabetic order by symbol name, and the listing file is	*/
/*  closed.  If the disk fills up, a fatal error occurs.		*/

static int col = 0;

extern int eject, listhex;
extern unsigned  bytes, listleft, obj[], pagelen;
extern char title[];

void check_page()
{
    if (pagelen && !--listleft) eject = TRUE;
    if (eject) {
	eject = FALSE;  listleft = pagelen;  fprintf(list,"\f");
	if (title[0]) { listleft -= 2;  fprintf(list,"%s\n\n",title); }
    }
    return;
}

/*  Listing file line output routine.  This routine processes the	*/
/*  source line saved by popc() and the output of the line assembler in	*/
/*  buffer obj into a line of the listing.  If the disk fills up, a	*/
/*  fatal error occurs.							*/

void lputs()
{
    SCRATCH int i, j;
    SCRATCH unsigned *o;
    void check_page(), fatal_error();
    extern char *strcpy (char *dest, const char *src);

    if (list) {
	i = bytes;  o = obj;
	do {
	    fprintf(list,"%c  ",errcode);
	    if (listhex) {
		fprintf(list,"%04x  ",address);
		for (j = 4; j; --j) {
		    if (i) { --i;  ++address;  fprintf(list," %02x",*o++); }
		    else fprintf(list,"   ");
		}
	    }
	    else fprintf(list,"%18s","");
	    fprintf(list,"   %s",line);  strcpy(line,"\n");
	    check_page();
	    if (ferror(list)) fatal_error(DSKFULL);
	} while (listhex && i);
    }
    return;
}



void list_sym(sp)
SYMBOL *sp;
{
    //void check_page();

    if (sp) {
	list_sym(sp -> left);
	fprintf(list,"%04x  %-10s",sp -> valu,sp -> sname);
	if (col = ++col % SYMCOLS) fprintf(list,"    ");
	else {
	    fprintf(list,"\n");
	    if (sp -> right) check_page();
	}
	list_sym(sp -> right);
    }
    return;
}


void lclose()
{
    void fatal_error(), list_sym();

    if (list) {
	if (sroot) {
	    list_sym(sroot);
	    if (col) fprintf(list,"\n");
	}
	fprintf(list,"\f");
	if (ferror(list) || fclose(list) == EOF) fatal_error(DSKFULL);
    }
    return;
}


/*  Add new symbol to symbol table.  Returns pointer to symbol even if	*/
/*  the symbol already exists.  If there's not enough memory to store	*/
/*  the new symbol, a fatal error occurs.				*/

SYMBOL *new_symbol(nam)
char *nam;
{
    SCRATCH int i;
    SCRATCH SYMBOL **p, *q;
    void fatal_error();
    //extern char *strcpy (char *dest, const char *src);
    //extern strlen ( char *str );

    for (p = &sroot; (q = *p) && (i = strcmp(nam,q -> sname)); )
	p = i < 0 ? &(q -> left) : &(q -> right);
    if (!q) {
	if (!(*p = q = (SYMBOL *)calloc(1,sizeof(SYMBOL) + strlen(nam))))
	    fatal_error(SYMBOLS);
	strcpy(q -> sname,nam);
    }
    return q;
}

/*  Look up symbol in symbol table.  Returns pointer to symbol or NULL	*/
/*  if symbol not found.						*/

SYMBOL *find_symbol(nam)
char *nam;
{
    SCRATCH int i;
    SCRATCH SYMBOL *p;

    for (p = sroot; p && (i = strcmp(nam,p -> sname));
	p = i < 0 ? p -> left : p -> right);
    return p;
}


int ishex(c)
char c ;
{
    char chup ;

    chup = ( ( (c) >= 'a' ) && ( (c) <= 'z' ) ) ? (c) - 'a' + 'A' : (c) ;
    return isnum(c) || ((c = chup) >= 'A' && c <= 'F');
}



/*************************************************************************************/


/*  Define global mailboxes for all modules:				*/

char errcode, line[MAXLINE + 1], title[MAXLINE], c ;
int pass = 0;
int eject, filesp, forwd, listhex;
unsigned address, bytes, errors, listleft, obj[MAXLINE], pagelen, pc;
FILE *filestk[FILES], *source;
TOKEN arg, token;

/*  Mainline routine.  This routine parses the command line, sets up	*/
/*  the assembler at the beginning of each pass, feeds the source text	*/
/*  to the line assembler, feeds the result to the listing and hex file	*/
/*  drivers, and cleans everything up at the end of the run.		*/

static int done, ifsp, off;

int main(argc,argv)
int argc;
char **argv;
{
    SCRATCH unsigned *o;
    int newline();
    void asm_line();
    void lclose(), lopen(), lputs();
    void hclose(), hopen(), hputc();
    void error(char code), fatal_error(), warning();
    extern char *strcpy (char *dest, const char *src);
    extern void exit (int stat);

    printf("Z-80 Cross-Assembler (Portable) Ver 0.1\n");
    printf("Copyright (c) 1986-1988 William C. Colley, III\n\n");

    while (--argc > 0) {
	if (**++argv == '-') {
	    c = (*++*argv) ;
	    c = (( (c) >= 'a' ) && ( (c) <= 'z') ) ? (c) - 'a' + 'A' : (c) ;
	    switch (c) {
		case 'L':   if (!*++*argv) {
				if (!--argc) { warning(NOLST);  break; }
				else ++argv;
			    }
			    lopen(*argv);
			    break;

		case 'O':   if (!*++*argv) {
				if (!--argc) { warning(NOHEX);  break; }
				else ++argv;
			    }
			    hopen(*argv);
			    break;

		default:    warning(BADOPT);
	    }
	}
	else if (filestk[0]) warning(TWOASM);
	else if (!(filestk[0] = fopen(*argv,"r"))) fatal_error(ASMOPEN);
    }
    if (!filestk[0]) fatal_error(NOASM);

    while (++pass < 3) {
	fseek(source = filestk[0],0L,0);  done = off = FALSE;
	errors = filesp = ifsp = pagelen = pc = 0;  title[0] = '\0';
	while (!done) {
	    errcode = ' ';
	    if (newline()) {
		error('*');
		strcpy(line,"\tEND\n");
		done = eject = TRUE;  listhex = FALSE;
		bytes = 0;
	    }
	    else asm_line();
	    pc = word(pc + bytes);
	    if (pass == 2) {
		lputs();
		for (o = obj; bytes--; hputc(*o++));
	    }
	}
    }

    fclose(filestk[0]);  lclose();  hclose();

    if (errors) printf("%d Error(s)\n",errors);
    else printf("No Errors\n");

    exit(errors);
}

/*  Line assembly routine.  This routine gets the contents of the	*/
/*  argument field from the source file using the expression evaluator	*/
/*  and lexical analyzer.  It makes all validity checks on the		*/
/*  arguments validity, fills a buffer with the machine code bytes and	*/
/*  returns nothing.							*/

static char label[MAXLINE];
static int ifstack[IFDEPTH] = { ON };

static OPCODE *opcod;

static int oldc, eol;
static char *lptr;

static int oldt = FALSE;
static int quote = FALSE;


/*  Push character back onto input stream.  Only one level of push-back	*/
/*  supported.  \0 cannot be pushed back, but nobody would want to.	*/

void pushc(c)
char c;
{
    oldc = c;
    return;
}

/*  Begin new line of source input.  This routine returns non-zero if	*/
/*  EOF	has been reached on the main source file, zero otherwise.	*/

int newline()
{
    void fatal_error();

    oldc = '\0';  lptr = line;
    oldt = eol = FALSE;
    while (feof(source)) {
	if (ferror(source)) fatal_error(ASMREAD);
	if (filesp) {
	    fclose(source);
	    source = filestk[--filesp];
	}
	else return TRUE;
    }
    return FALSE;
}


/*  Get character from input stream.  This routine does a number of	*/
/*  other things while it's passing back characters.  All control	*/
/*  characters except \t and \n are ignored.  \t is mapped into ' '.	*/
/*  Semicolon is mapped to \n.  In addition, a copy of all input is set	*/
/*  up in a line buffer for the benefit of the listing.			*/


int popc()
{
    SCRATCH int c;

    if (oldc) { c = oldc;  oldc = '\0';  return c; }
    if (eol) return '\n';
    for (;;) {
	if ((c = getc(source)) != EOF && (c &= 0377) == ';' && !quote) {
	    do *lptr++ = c;
	    while ((c = getc(source)) != EOF && (c &= 0377) != '\n');
	}
	if (c == EOF) c = '\n';
	if ((*lptr++ = c) >= ' ' && c <= '~') return c;
	if (c == '\n') { eol = TRUE;  *lptr = '\0';  return '\n'; }
	if (c == '\t') return quote ? '\t' : ' ';
    }
}


/*  Get an alphanumeric string into the string value part of the	*/
/*  current token.  Leading blank space is trashed.			*/

void pops(s)
char *s;
{
    void pushc(), trash();

    trash();
    for (; isalnum(*s = popc()); ++s);
    pushc(*s);  *s = '\0';
    return;
}

/*  Trash blank space and push back the character following it.		*/

void trash()
{
    SCRATCH char c;
    void pushc();

    while ((c = popc()) == ' ');
    pushc(c);
    return;
}


void asm_line()
{
    SCRATCH char *p;
    SCRATCH int i;
    int isalph(), popc();
    OPCODE *find_code(), *find_operator();
    void do_label(), flush(), normal_op(), pseudo_op();
    void error(char code), pops(), pushc(), trash();

    address = pc;  bytes = 0;  eject = forwd = listhex = FALSE;
    for (i = 0; i < BIGINST; obj[i++] = NOP);

    label[0] = '\0';
    if ((i = popc()) != ' ' && i != '\n') {
	if (isalph(i)) {
	    pushc(i);  pops(label);
	    for (p = label;  *(p + 1);  ++p);
	    if (*p == ':') *p = '\0';
	    if (find_operator(label)) { label[0] = '\0';  error('L'); }
	}
	else {
	    error('L');
	    while ((i = popc()) != ' ' && i != '\n');
	}
    }

    trash();  opcod = NULL;
    if ((i = popc()) != '\n') {
	if (!isalph(i)) error('S');
	else {
	    pushc(i);  pops(token.sval);
	    if (!(opcod = find_code(token.sval))) error('O');
	}
	if (!opcod) { listhex = TRUE;  bytes = BIGINST; }
    }

    if (opcod && opcod -> attr & ISIF) { if (label[0]) error('L'); }
    else if (off) { listhex = FALSE;  flush();  return; }

    if (!opcod) { do_label();  flush(); }
    else {
	listhex = TRUE;
	if (opcod -> attr & PSEUDO) pseudo_op();
	else normal_op();
	while ((i = popc()) != '\n') if (i != ' ') error('T');
    }
    source = filestk[filesp];
    return;
}

void flush()
{
    while (popc() != '\n');
}

void do_label()
{
    SCRATCH SYMBOL *l;
    SYMBOL *find_symbol(), *new_symbol();
    void error(char code);

    if (label[0]) {
	listhex = TRUE;
	if (pass == 1) {
	    if (!((l = new_symbol(label)) -> attr)) {
		l -> attr = FORWD + VAL;
		l -> valu = pc;
	    }
	}
	else {
	    if (l = find_symbol(label)) {
		l -> attr = VAL;
		if (l -> valu != pc) error('M');
	    }
	    else error('P');
	}
    }
}



static int bad;


/*  Expression analysis routine.  The token stream from the lexical	*/
/*  analyzer is processed as an arithmetic expression and reduced to an	*/
/*  unsigned value.  If an error occurs during the evaluation, the	*/
/*  global flag	forwd is set to indicate to the line assembler that it	*/
/*  should not base certain decisions on the result of the evaluation.	*/

unsigned expr()
{
    SCRATCH unsigned u;
    unsigned eval();

    bad = FALSE;
    u = eval(START);
    return bad ? 0 : u;
}


void exp_error(c)
char c;
{
	void error(char code);
    forwd = bad = TRUE;  error(c);
}




void make_number(base)
unsigned base;
{
    SCRATCH char *p, chup ;
    SCRATCH unsigned d;
    void exp_error();

    token.attr = VAL;
    token.valu = 0;
    for (p = token.sval; *p; ++p) {
	chup = ( ( (*p) >= 'a' ) && ( (*p) <= 'z' ) ) ? (*p) - 'a' + 'A' : (*p) ;
	d = chup - (isnum(*p) ? '0' : 'A' - 10);
	token.valu = token.valu * base + d;
	if (!ishex(*p) || d >= base) { exp_error('D');  break; }
    }
    clamp(token.valu);
    return;
}





/*  Lexical analyzer.  The source input character stream is chopped up	*/
/*  into its component parts and the pieces are evaluated.  Symbols are	*/
/*  looked up, operators are looked up, etc.  Everything gets reduced	*/
/*  to an attribute word, a numeric value, and (possibly) a string	*/
/*  value.								*/


TOKEN *lex()
{
    SCRATCH char c, *p, chup ;
    SCRATCH unsigned b;
    SCRATCH OPCODE *o;
    SCRATCH SYMBOL *s;
    //VOID *find_operator();
    //SYMBOL *find_symbol();
    void exp_error(), make_number(), pops(), pushc(), trash();

    if (oldt) { oldt = FALSE;  return &token; }
    trash();
    if (isalph(c = popc())) {
	pushc(c);  pops(token.sval);
	if (o = find_operator(token.sval)) {
	    token.attr = o -> attr;  token.valu = o -> valu;
	}
	else {
	    token.attr = VAL;
	    if (!strcmp(token.sval,"$")) token.valu = pc;
	    else if (s = find_symbol(token.sval)) {
		token.valu = s -> valu;
		if (pass == 2 && s -> attr & FORWD) forwd = TRUE;
	    }
	    else { token.valu = 0;  exp_error('U'); }
	}
    }
    else if (isnum(c)) {
	pushc(c);  pops(token.sval);
	for (p = token.sval; p[1]; ++p);
	chup = ( ( (*p) >= 'a' ) && ( (*p) <= 'z' ) ) ? (*p) - 'a' + 'A' : (*p) ;
	switch (chup) {
	    case 'B':	b = 2;  break;

	    case 'O':
	    case 'Q':	b = 8;  break;

	    default:	++p;
	    case 'D':	b = 10;  break;

	    case 'H':	b = 16;  break;
	}
	*p = '\0';  make_number(b);
    }
    else switch (c) {
	case '(':   token.attr = UNARY + LPREN + OPR;
		    goto opr1;

	case ')':   token.attr = BINARY + RPREN + OPR;
		    goto opr1;

	case '+':   token.attr = BINARY + UNARY + ADDIT + OPR;
		    goto opr1;

	case '-':   token.attr = BINARY + UNARY + ADDIT + OPR;
		    goto opr1;

	case '*':   token.attr = BINARY + UNARY + MULT + OPR;
		    goto opr1;

	case '/':   token.attr = BINARY + MULT + OPR;
opr1:		    token.valu = c;
		    break;

	case '<':   token.valu = c;
		    if ((c = popc()) == '=') token.valu = LE;
		    else if (c == '>') token.valu = NE;
		    else pushc(c);
		    goto opr2;

	case '=':   token.valu = c;
		    if ((c = popc()) == '<') token.valu = LE;
		    else if (c == '>') token.valu = GE;
		    else pushc(c);
		    goto opr2;

	case '>':   token.valu = c;
		    if ((c = popc()) == '<') token.valu = NE;
		    else if (c == '=') token.valu = GE;
		    else pushc(c);
opr2:		    token.attr = BINARY + RELAT + OPR;
		    break;

	case '\'':
 case '"':   quote = TRUE;  token.attr = STR;
		    for (p = token.sval; (*p = popc()) != c; ++p)
			if (*p == '\n') { exp_error('"');  break; }
		    *p = '\0';  quote = FALSE;
		    if ((token.valu = token.sval[0]) && token.sval[1])
			token.valu = (token.valu << 8) + token.sval[1];
		    break;

	case ',':   token.attr = SEP;
		    break;

        case '\n':  token.attr = EOL;
		    break;
    }
    return &token;
}

/*  Push back the current token into the input stream.  One level of	*/
/*  pushback is supported.						*/

void unlex()
{
    oldt = TRUE;
    return;
}


unsigned eval(pre)
unsigned pre;
{
   register unsigned op, u, v;
   TOKEN *lex();
   void exp_error(), unlex();

   for (;;) {
      u = op = lex() -> valu;
      switch (token.attr & TYPE) {
	 case REG:   exp_error('S');  break;

	 case SEP:   if (pre != START) unlex();
	 case EOL:   exp_error('E');  return 0;

	 case OPR:   if (!(token.attr & UNARY)) { exp_error('E');  break; }
		     u = eval((op == '+' || op == '-') ?
			   (unsigned) UOP1 : token.attr & PREC);
		     switch (op) {
			case '-':   u = word(-u);  break;

			case NOT:   u ^= 0xffff;  break;

			case HIGH:  u = high(u);  break;

			case LOW:   u = low(u);  break;
		     }

	 case VAL:	
	 case STR:   for (;;) {
			op = lex() -> valu;
			switch (token.attr & TYPE) {
			   case REG:   exp_error('S');  break;

			   case SEP:   if (pre != START) unlex();
			   case EOL:   if (pre == LPREN) exp_error('(');
				       return u;

			   case STR:
			   case VAL:   exp_error('E');  break;

			   case OPR:   if (!(token.attr & BINARY)) {
					  exp_error('E');  break;
				       }
				       if ((token.attr & PREC) >= pre) {
					  unlex();  return u;
				       }
				       if (op != ')')
					  v = eval(token.attr & PREC);
				       switch (op) {
					  case '+':   u += v;  break;

					  case '-':   u -= v;  break;

					  case '*':   u *= v;  break;

					  case '/':   u /= v;  break;

					  case MOD:   u %= v;  break;

					  case AND:   u &= v;  break;

					  case OR:    u |= v;  break;

					  case XOR:   u ^= v;  break;

					  case '<':   u = u < v;  break;

					  case LE:    u = u <= v;  break;

					  case '=':   u = u == v;  break;

					  case GE:    u = u >= v;  break;

					  case '>':   u = u > v;  break;

					  case NE:    u = u != v;  break;

					  case SHL:   if (v > 15)
							 exp_error('E');
						      else u <<= v;
						      break;

					  case SHR:   if (v > 15)
							 exp_error('E');
						      else u >>= v;
						      break;

					  case ')':   if (pre == LPREN)
							 return u;
						      exp_error('(');
						      break;
				       }
				       clamp(u);
				       break;
			}
		     }
		     break;
      }
   }
}

/*  Fatal error handler routine.  A message gets printed on the stderr	*/
/*  device, and the program bombs.					*/

void fatal_error(msg)
char *msg;
{
    extern void exit (int stat);

    printf("Fatal Error -- %s\n",msg);
    exit(-1);
}

/*  Machine opcode argument field parsing routine.  The token stream	*/
/*  from the lexical analyzer is processed to extract addresses and	*/
/*  addressing mode information.  The resulting value is passed back	*/
/*  through the same type of token buffer that the lexical analyzer	*/
/*  uses.  In addition, the addressing mode information is returned as	*/
/*  the	function return value.						*/


unsigned get_arg()
{
    SCRATCH int c;
    SCRATCH unsigned a, u;
    TOKEN *lex();
    int popc();
    unsigned eval(), expr();
    void exp_error(), pushc(), trash(), unlex();

    a = NUM;  u = 0;  bad = FALSE;
    switch (lex() -> attr & TYPE) {
	case SEP:   exp_error('S');  a = NONE;  break;

	case EOL:   unlex();  a = NONE;  break;

	case REG:   a = token.valu;  break;

	case OPR:   if (token.valu == '(') {
			if ((lex() -> attr & TYPE) == REG) {
			    a = token.valu;  lex();
			    switch (a) {
				case C:	    a = C_IND;  break;

				case BC:
				case DE:
				case SP:    a += SP_IND - SP;  break;

				case HL:    a = HL_IND;  break;

				case IX:    a = IX_IND;  goto do_ix_ind;

				case IY:    a = IY_IND;
do_ix_ind:				    if ((token.attr & TYPE) == OPR &&
						(token.valu == '+' ||
						token.valu == '-')) {
						unlex();
						if ((u = eval(LPREN)) > 0x7f
						    && u < 0xff80)
						    exp_error('V');
					    }
					    break;

				default:    exp_error('R');  break;
			    }
			    if ((token.attr & TYPE) != OPR ||
				token.valu != ')') exp_error(')');
			    break;
			}
			else {
			    unlex();  u = eval(LPREN);
			    trash();  pushc(c = popc());
			    if (c == ',' || c == '\n') {
				a = NUM_IND;  break;
			    }
			    token.attr = VAL;  token.valu = u;
			}
		    }

	case VAL:
	case STR:   unlex();  u = eval(START);  unlex();  break;
    }
    arg.valu = bad ? 0 : u;  return arg.attr = a;
}



void normal_op()
{
    SCRATCH unsigned opcode, tmp;
    unsigned *o, *p;
    static unsigned IM_tbl[] = { 0x00, 0x10, 0x18 };
    int popc();
    unsigned expr(), get_arg();
    void do_label(), error(char code), grab_comma(), insert_prebyte(), pushc(), trash();
    TOKEN *lex();

    do_label();  opcode = opcod -> valu;  o = obj;
    if (opcode > 0xff) *o++ = high(opcode);
    *o = low(opcode);
    switch (opcod -> attr & OPTYPE) {
	case LD:
	    p = o;
	    switch (get_arg()) {
		case IX:
		case IY:	insert_prebyte(&o,arg.attr);

		case BC:
		case DE:
		case HL:
		case SP:	*o = 0x01 + ((arg.attr & 007) << 3);
				grab_comma();
				switch (get_arg()) {
				    case NUM_IND:   if (*o == 0x21) *o += 0x09;
						    else {
							*o += 0x4a;
							insert_prebyte(&o,
							    arg.attr);
						    }

				    case NUM:	    *++o = low(arg.valu);
						    *++o = high(arg.valu);
						    break;

				    case IX:
				    case IY:	    insert_prebyte(&o,
							arg.attr);

				    case HL:	    if (*o == 0x31) {
							*o = 0xf9;  break;
						    }

				    default:	    error(arg.attr < NUM ?
							'R' : 'S');
						    break;
				}
				break;

		case I:		*o++ = 0xed;  *o = 0x47;  goto do_ld_i;

		case R:		*o++ = 0xed;  *o = 0x4f;  goto do_ld_i;

		case BC_IND:	*o = 0x02;  goto do_ld_i;

		case DE_IND:	*o = 0x12;
do_ld_i:			grab_comma();
				if (get_arg() != A)
				    error(arg.attr < NUM ? 'R' : 'S');
				break;

		case NUM_IND:	*++o = low(arg.valu);
				*++o = high(arg.valu);
				grab_comma();  *p = 0x02;
				switch (get_arg()) {
				    case A:	    *p = 0x32;  break;

				    case BC:
				    case DE:
				    case SP:	    *p = 0x43;

				    case IX:
				    case IY:	    *p += (arg.attr & 007)
							<< 3;
						    insert_prebyte(&o,
							arg.attr);
						    break;

				    case HL:	    *p = 0x22;  break;

				    default:	    error(arg.attr < NUM ?
							'R' : 'S');
						    break;
				}
				break;

		case A:		grab_comma();
				switch (get_arg()) {
				    case BC_IND:    *o = 0x0a;  break;

				    case DE_IND:    *o = 0x1a;  break;

				    case I:	    *o++ = 0xed;  *o = 0x57;
						    break;

				    case R:	    *o++ = 0xed;  *o = 0x5f;
						    break;

				    case NUM_IND:   *o++ = 0x3a;
						    *o++ = low(arg.valu);
						    *o = high(arg.valu);
						    break;

				    default:	    *o += 0x38;  goto do_ld_a;
				}
				break;				

		case IX_IND:
		case IY_IND:	insert_prebyte(&o,arg.attr);  ++p;
				*++o = low(arg.valu);  arg.attr = HL_IND;

		default:	if (arg.attr <= A) {
				    *p += (arg.attr & 007) << 3;
				    grab_comma();  get_arg();
do_ld_a:			    if (arg.attr == NUM) {
					*p -= 0x3a;
					if (arg.valu > 0xff &&
					    arg.valu < 0xff80) {
					    *++o = 0;  error('V');
					}
					else *++o = low(arg.valu);
					break;
				    }
				    if (arg.attr <= A || arg.attr ==
					IX_IND || arg.attr == IY_IND) {
					if ((*p += arg.attr & 007) == 0x76) {
					    *p = 0x06;  error('R');  break;
					}
					if (arg.attr > A) {
					    insert_prebyte(&o,arg.attr);
					    *++o = low(arg.valu);
					}
					break;
				    }
				}
				error(arg.attr < NUM ? 'R' : 'S');  break;
	    }
	    break;

	case POP:
	    switch (get_arg()) {
		case IX:
		case IY:	insert_prebyte(&o,arg.attr);
				arg.attr = HL;

		default:	if (arg.attr < BC || arg.attr > AF)
				    error(arg.attr < NUM ? 'R' : 'S');
				else *o += (arg.attr & 007) << 3;
				break;
	    }
	    break;

	case EX:
	    tmp = get_arg();  grab_comma();
	    switch (tmp) {
		case SP_IND:	if (get_arg() == IX || arg.attr == IY)
				    insert_prebyte(&o,arg.attr);
				else if (arg.attr != HL)
				    error(arg.attr < NUM ? 'R' : 'S');
				break;

		case DE:	if (get_arg() == HL) *o = 0xeb;
				else error(arg.attr < NUM ? 'R' : 'S');
				break;

		case AF:	if (get_arg() == AF) {
				    *o = 0x08;  trash();
				    if ((tmp = popc()) == '\'') break;
				    pushc(tmp);  error('S');  break;
				}
				error(arg.attr < NUM ? 'R' : 'S');
				break;
	    }
	    break;	    

	case ADD:
	    if (get_arg() == IX || arg.attr == IY) {
		insert_prebyte(&o,arg.attr);
		tmp = arg.attr;  arg.attr = HL;
	    }
	    else tmp = HL;
	    goto do_adc;	

	case ADC:
	    get_arg();  tmp = HL;
do_adc:	    if (arg.attr == HL) {
		switch (*o) {
		    case 0x80:	*o = 0x09;  break;

		    case 0x88:	*o++ = 0xed;  *o = 0x4a;  break;

		    case 0x98:	*o++ = 0xed;  *o = 0x42;  break;
		}
		grab_comma();
		if (get_arg() == BC || arg.attr == DE || arg.attr == SP ||
		    arg.attr == tmp) *o += (arg.attr & 007) << 3;
		else error(arg.attr < NUM ? 'R' : 'S');
		break;
	    }
	    if (arg.attr != A) { error('S');  break; }
	    grab_comma();

	case CP:
	    switch (get_arg()) {
		case NUM_IND:
		case NONE:	error('S');  break;

		case NUM:	*o++ += 0x46;
				if (arg.valu > 0xff && arg.valu < 0xff80) {
				    *o = 0;  error('V');
				}
				else *o = low(arg.valu);
				break;

		case IX_IND:
		case IY_IND:	insert_prebyte(&o,arg.attr);  *o++ += 0x06;
				*o = low(arg.valu);  break;

		default:	if (arg.attr > A) error('R');
				else *o += arg.attr & 007;
				break;
	    }
	    break;

	case DEC:
	    p = o;
	    switch (get_arg()) {
		case NUM:
		case NUM_IND:
		case NONE:	error('S');  break;

		case IX:
		case IY:	insert_prebyte(&o,arg.attr);

		case SP:
		case HL:
		case DE:
		case BC:	*o = (arg.attr & 007 ^ *o) << 3 ^ 0x23;
				break;

		case IX_IND:
		case IY_IND:	insert_prebyte(&o,arg.attr);  ++p;
				*++o = arg.valu;  arg.attr = HL_IND;

		default:	if (arg.attr > A) error('R');
				else *p += (arg.attr & 007) << 3;
				break;
	    }
	    break;

	case BIT:
	    if (get_arg() != NUM) error('S');
	    else if (arg.valu > 7) error('V');
	    else *o += arg.valu << 3;
	    grab_comma();

	case RLC:
	    switch (get_arg()) {
		case NUM:
		case NUM_IND:
		case NONE:	error('S');  break;

		case IX_IND:
		case IY_IND:	insert_prebyte(&o,arg.attr); *(o + 1) = *o;
				*o++ = low(arg.valu);  arg.attr = HL_IND;

		default:	if (arg.attr > A) error('R');
				else *o += arg.attr & 007;
				break;
	    }
	    break;
		

	case JR:
	    if (get_arg() == NUM) { *o -= 0x08;  goto do_djnz; }
	    if (arg.attr == C) arg.attr = CY;
	    if (arg.attr < NZ || arg.attr > CY) {
		*++o = 0xfe;  error('S');  break;
	    }
	    *o += (arg.attr & 007) << 3;  grab_comma();

	case DJNZ:
	    if (get_arg() != NUM) { *++o = 0xfe;  error('S');  break; }
do_djnz:    if ((tmp = arg.valu - (pc + 2)) > 0x7f && tmp < 0xff80) {
		*++o = 0xfe;  error('B');
	    }
	    else *++o = low(tmp);
	    break;

	case JP:
	    switch (get_arg()) {
		case IX_IND:
		case IY_IND:	insert_prebyte(&o,arg.attr);
				if (arg.valu) error('V');

		case HL_IND:	*o = 0xe9;  break;

		default:	goto do_call;
	    }
	    break;

	case CALL:
	    get_arg();
do_call:    if (arg.attr == NUM) *o += *o == 0xc4 ? 0x09 : 0x01;
	    else {
		if (arg.attr == C) arg.attr = CY;
		if (arg.attr < NZ || arg.attr > M) error('S');
		else *o += (arg.attr & 007) << 3;
		grab_comma();
		if (get_arg() != NUM) error('S');
	    }
	    *++o = low(arg.valu);  *++o = high(arg.valu);  break;

	case RET:
	    if (get_arg() == NONE) { *o += 0x09;  break; }
	    if (arg.attr == C) arg.attr = CY;
	    if (arg.attr < NZ || arg.attr > M) error('S');
	    else *o += (arg.attr & 007) << 3;
	    break;

	case IN:
	    tmp = get_arg();  grab_comma();
	    switch (get_arg()) {
		case C_IND:	*(o - 1) = 0xed;  *o = 0x40;
				if (tmp <= L || tmp == A)
				    *o += (tmp & 007) << 3;
				else error(tmp < NUM ? 'R' : 'S');
				break;

		case NUM_IND:	if (arg.valu <= 0xff) {
				    *o = arg.valu;
				    if (tmp != A) error(tmp < NUM ? 'R' : 'S');
				}
				else error('V');
				break;

		case NUM:	error('S');  break;

		default:	error('R');  break;
	    }
	    break;

	case OUT:
	    switch (get_arg()) {
		case C_IND:	*(o - 1) = 0xed;  *o = 0x41;  grab_comma();
				if (get_arg() <= L || arg.attr == A)
				    *o += (arg.attr & 007) << 3;
				else error(arg.attr < NUM ? 'R' : 'S');
				break;

		case NUM_IND:	if (arg.valu <= 0xff) {
				    *o = arg.valu;  grab_comma();
				    if (get_arg() != A)
					error(arg.attr < NUM ? 'R' : 'S');
				}
				else error('V');
				break;

		case NUM:	error('S');  break;

		default:	error('R');  break;
	    }
	    break;

	case RST:
	    if (get_arg() != NUM) error('S');
	    else if (arg.valu & 0xffc7) error('V');
	    else *o |= arg.valu;
	    break;

	case IM:
	    if (get_arg() != NUM) error('S');
	    else if (arg.valu > 2) error('V');
	    else *o |= IM_tbl[arg.valu];

	case NO_ARGS:
	    break;
    }
    if ((lex() -> attr & TYPE) != EOL) error('T');
    bytes = (o - obj) + 1;  return;
}

void grab_comma()
{
    void error(char code), unlex();
    TOKEN *lex();

    if ((lex() -> attr & TYPE) != SEP) { error('S');  unlex(); }
}

void insert_prebyte(optr,pb)
unsigned **optr;
unsigned pb;
{
    SCRATCH unsigned *q;

    for (q = ++*optr; q > obj ; --q) *q = *(q - 1);
    if (pb == IX || pb == IX_IND) *q = 0xdd;
    else if (pb == IY || pb == IY_IND) *q = 0xfd;
    else *q = 0xed;
    return;
}

void pseudo_op()
{
    SCRATCH char *s;
    SCRATCH unsigned *o, u;
    SCRATCH SYMBOL *l;
    int popc();
    unsigned expr();
    SYMBOL *find_symbol(), *new_symbol();
    TOKEN *lex();
    void do_label(), error(char code), fatal_error(), hseek();
    void pushc(), trash(), unlex();
    extern char *strcpy (char *dest, const char *src);

    o = obj;
    switch (opcod -> valu) {
	case DB:
	case DC:    do_label();
		    do {
			if ((lex() -> attr & TYPE) == SEP) {
			    *o++ = 0;  ++bytes;
			}
			else if (token.attr == STR) {
			    trash();  pushc(u = popc());
			    if (u != ',' && u != '\n') goto do_byte;
			    for (s = token.sval; *s; *o++ = *s++) ++bytes;
			    lex();
			}
			else {
do_byte:		    unlex();
			    if ((u = expr()) > 0xff && u < 0xff80) {
				u = 0;  error('V');
			    }
			    *o++ = low(u);  ++bytes;
			}
		    } while ((token.attr & TYPE) == SEP);
		    if (bytes && opcod -> valu == DC) *(o - 1) |= 0x80;
		    break;

	case DS:    do_label();
		    u = word(pc + expr());
		    if (forwd) error('P');
		    else {
			pc = u;
			if (pass == 2) hseek(pc);
		    }
		    break;

	case DW:    do_label();
		    do {
			if ((lex() -> attr & TYPE) == SEP) u = 0;
			else { unlex();  u = expr(); }
			*o++ = low(u);  *o++ = high(u);
			bytes += 2;
		    } while ((token.attr & TYPE) == SEP);
		    break;

	case ELSE:  listhex = FALSE;
		    if (ifsp) off = (ifstack[ifsp] = -ifstack[ifsp]) != ON;
		    else error('I');
		    break;

	case END:   do_label();
		    if (filesp) { listhex = FALSE;  error('*'); }
		    else {
			done = eject = TRUE;
			if (pass == 2 && (lex() -> attr & TYPE) != EOL) {
			    unlex();  hseek(address = expr());
			}
			if (ifsp) error('I');
		    }
		    break;

	case ENDIF: listhex = FALSE;
		    if (ifsp) off = ifstack[--ifsp] != ON;
		    else error('I');
		    break;

	case EQU:   if (label[0]) {
			if (pass == 1) {
			    if (!((l = new_symbol(label)) -> attr)) {
				l -> attr = FORWD + VAL;
				address = expr();
				if (!forwd) l -> valu = address;
			    }
			}
			else {
			    if (l = find_symbol(label)) {
				l -> attr = VAL;
				address = expr();
				if (forwd) error('P');
				if (l -> valu != address) error('M');
			    }
			    else error('P');
			}
		    }
		    else error('L');
		    break;

	case IF:    if (++ifsp == IFDEPTH) fatal_error(IFOFLOW);
		    address = expr();
		    if (forwd) { error('P');  address = TRUE; }
		    if (off) { listhex = FALSE;  ifstack[ifsp] = (int) NULL; }
		    else {
			ifstack[ifsp] = address ? ON : OFF;
			if (!address) off = TRUE;
		    }
		    break;

	case INCL:  listhex = FALSE;  do_label();
		    if ((lex() -> attr & TYPE) == STR) {
			if (++filesp == FILES) fatal_error(FLOFLOW);
			if (!(filestk[filesp] = fopen(token.sval,"r"))) {
			    --filesp;  error('V');
			}
		    }
		    else error('S');
		    break;

	case ORG:   u = expr();
		    if (forwd) error('P');
		    else {
			pc = address = u;
			if (pass == 2) hseek(pc);
		    }
		    do_label();
		    break;

	case PAGE:  listhex = FALSE;  do_label();
		    if ((lex() -> attr & TYPE) != EOL) {
			unlex();  pagelen = expr();
			if (pagelen > 0 && pagelen < 3) {
			    pagelen = 0;  error('V');
			}
		    }
		    eject = TRUE;
		    break;

	case TITLE: listhex = FALSE;  do_label();
		    if ((lex() -> attr & TYPE) == EOL) title[0] = '\0';
		    else if ((token.attr & TYPE) != STR) error('S');
		    else strcpy(title,token.sval);
		    break;

	case VAR:   if (label[0]) {
			if (pass == 1) {
			    if (!((l = new_symbol(label)) -> attr)
				|| (l -> attr & SOFT)) {
				l -> attr = FORWD + SOFT + VAL;
				address = expr();
				if (!forwd) l -> valu = address;
			    }
			}
			else {
			    if (l = find_symbol(label)) {
				address = expr();
				if (forwd) error('P');
				else if (l -> attr & SOFT) {
				    l -> attr = SOFT + VAL;
				    l -> valu = address;
				}
				else error('M');
			    }
			    else error('P');
			}
		    }
		    else error('L');
		    break;
    }
    return;
}

