
/* Disassemble a Z80 binary file. Based on cpm disz80.c.            azz '98 */



#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const unsigned char *disz80(const unsigned char *PC, char *s, unsigned pc);

void main(int argc, char **argv) {
   //unsigned char mem[65536], this; /* Increase if required. */
   unsigned char mem[30000], this; /* Increase if required. */
   char line[256];
   unsigned pc,len,oldpc;
   FILE *f;
   
   if ((f = fopen(argv[1], "r")) == NULL) {
      printf("Could not open file. Usage: disasm <file>.\n");
      exit(20);
   };
   
   // Quick hack to keep the CP/M .COM programs in the right position
   //for(pc=0;!feof(f);mem[256 + pc++]=fgetc(f)); /* I like that line */
   
   for(pc=0;!feof(f);mem[pc++]=fgetc(f)); /* I like that line */
   fclose(f);
   
   len=pc-1;
   printf("; %d bytes read from %s\n", len, argv[1]);
   
   oldpc=65535;
   for(pc=0;pc < len;) {
      printf("A%04xx:    ",pc);
      this = mem[pc];
      pc = (unsigned)disz80(mem + pc, line, pc);
      pc = (unsigned)((int)pc - (int)mem); /* That is horrible! */
      if (oldpc == pc) {
	 sprintf(line,"DB 0%02x",mem[pc]);
	 pc++;
      };
      oldpc = pc;
      printf("%-20s", line);
      if ((this>31)&&(this<128)) printf("; %c", this);
      printf("\n");
   };
};

/*	written by Michael Bischoff (mbi@mo.math.nat.tu-bs.de)		     */
/*	June-1994							     */
/*									     */
/*	This file is distributed under the GNU COPYRIGHT		     */
/*	see COPYRIGHT.GNU for Copyright details				     */

/* 8-Bit registers */
static const char *dtab1[] = { "B", "C", "D", "E", "H", "L", NULL, "A" };
/* condition codes */
static const char *dtab4[] = { "NZ", "Z", "NC", "C", "PO", "PE", "P", "M" };
/* opcodes */
static const char *dtab5[] = { "ADD A,", "ADC A,", "SUB ", "SBC A,", "AND ", "XOR ", "OR ", "CP " };
static const char *dtabix[] = { "POP %s", "EX (SP),%s", "PUSH %s", "JP(%s)",
				    "EX DE,%s", "LD SP,%s" };
static const char *dtab2[] = { "BC", "DE", NULL, "SP" };
/*static const char *dtab3[] = { "BC", "DE", NULL, "AF" }; */

static const char *op1tab[] = {	/* NULLs are handled explicitly */
    "NOP",       NULL,        "LD (BC),A", NULL, NULL, NULL, NULL, "RLCA",
    "EX AF,AF'", NULL,        "LD A,(BC)", NULL, NULL, NULL, NULL, "RRCA",
    "DJNZ A%04xx", NULL,        "LD (DE),A", NULL, NULL, NULL, NULL, "RLA",
    "JR A%04xx",   NULL,        "LD A,(DE)", NULL, NULL, NULL, NULL, "RRA",
    "JR NZ,A%04xx",NULL,	      NULL,        NULL, NULL, NULL, NULL, "DAA",
    "JR Z,A%04xx", NULL,        NULL, 	   NULL, NULL, NULL, NULL, "CPL",
    "JR NC,A%04xx",NULL,        NULL,	   NULL, NULL, NULL, NULL, "SCF",
    "JR C,A%04xx", NULL,	      NULL,	   NULL, NULL, NULL, NULL, "CCF" };

static const char *op2tab[] = {
    NULL, "POP BC", NULL, "JP A%04xx",      NULL, "PUSH BC", NULL, NULL,   NULL, "RET",     NULL, "???",         NULL, "CALL %04x", NULL, NULL,
    NULL, "POP DE", NULL, "OUT (%03x),A", NULL, "PUSH DE", NULL, NULL,   NULL, "EXX",     NULL, "IN A,(%03x)", NULL, "???",       NULL, NULL,
    NULL, "POP HL", NULL, "EX (SP),HL",   NULL, "PUSH HL", NULL, NULL,   NULL, "JP (HL)", NULL, "EX DE,HL",    NULL, "???",       NULL, NULL,
    NULL, "POP AF", NULL, "DI",           NULL, "PUSH AF", NULL, NULL,   NULL, "LD SP,HL",NULL, "EI",          NULL, "???",       NULL, NULL };
static const char *cbtab1[] = {
    "RLC", "RRC", "RL", "RR", "SLA", "SRA", "? SLIA", "SRL" };
static const char *cbtab2[] = {
    NULL, "BIT", "RES", "SET" };
static unsigned char ixy_possible8[32] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00,
    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0xbf, 0x40,
    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static char edcmds[] = {
    0x44, 0xa0, 0xa8, 0xb0, 0xb8, 0x47, 0x4f, 0x57,
    0x5f, 0xa1, 0xb1, 0xa9, 0xb9, 0x46, 0x56, 0x5e,
    0x67, 0x6f, 0xa3, 0xab, 0xb3, 0xbb, 0x45, 0x4d,
0 };
static const char *edtxt[] = {
    "NEG", "LDI", "LDD", "LDIR", "LDDR", "LD I,A", "LD R,A", "LD A,I",
    "LD A,R", "CPI", "CPIR", "CPD", "CPDR", "IM 0", "IM 1", "IM 2", "RRD",
    "RLD", "OUTI", "OUTD", "OTIR", "OTDR", "RETN", "RETI"
    };

static const unsigned char *edcommand(const unsigned char *PC, char *s) {
    unsigned o, w;
    const char *dr;
    o = *PC;
    
    dtab2[2] = "HL";
    dr = dtab2[(o & 0x30) >> 4];

    switch (o & 0xcf) {
    case 0x42:
	sprintf(s, "SBC HL,%s", dr);
	break;
    case 0x43:
	w = *++PC;
	w += *++PC << 8;
	sprintf(s, "LD (%04x),%s", w, dr);
	break;
    case 0x4a:
	sprintf(s, "ADC HL,%s", dr);
	break;
    case 0x4b:
	w = *++PC;
	w += *++PC << 8;
	sprintf(s, "LD %s,(%04x)", dr, w);
	break;
    default:
	if (o && (dr = strchr(edcmds, o))) {
	    strcpy(s, edtxt[dr-edcmds]);
	    break;
	}
	return PC-1;
    }
    return PC+1;
}


/* PC is only needed for JR instructions */
const unsigned char *disz80(const unsigned char *PC, char *s, unsigned pc) {
    unsigned o, m, r;
    const char *regh, *regl, *ir;
    char i_offset[8];
    unsigned w;
    
    dtab1[6] = "(HL)";
    ir = "HL";
    o = *PC;
 redo:
    regh = dtab1[m = (o >> 3) & 7];
    regl = dtab1[r = o & 7];
    
    switch (o >> 6) {
    case 0:		/* the lower block */
	switch(o & 7) {
	case 4:
	    sprintf(s, "INC %s", regh);
	    break;
	case 5:
	    sprintf(s, "DEC %s", regh);
	    break;
	case 6:
	    sprintf(s, "LD %s,0%02x", regh, *++PC);
	    break;
	default:
	    dtab2[2] = ir;
	    switch (o) {
	    case 0x01: case 0x11: case 0x21: case 0x31:
		w = *++PC;
		w += *++PC << 8;
		sprintf(s, "LD %s,%04x", dtab2[o>>4], w);
		break;
	    case 0x03: case 0x13: case 0x23: case 0x33:
		sprintf(s, "INC %s", dtab2[o>>4]);
		break;
	    case 0x0b: case 0x1b: case 0x2b: case 0x3b:
		sprintf(s, "DEC %s", dtab2[o>>4]);
		break;
	    case 0x09: case 0x19: case 0x29: case 0x39:
		sprintf(s, "ADD %s,%s", ir, dtab2[o>>4]);
		break;
	    case 0x22: case 0x2a: case 0x32: case 0x3a:
		/* commands which use a 16 bit argument */
		w = *++PC;
		w += *++PC << 8;
		switch (o) {
		case 0x22:
		    sprintf(s, "LD (%04x),%s", w, ir);
		    break;
		case 0x2a:
		    sprintf(s, "LD %s,(%04x)", ir, w);
		    break;
		case 0x32:
		    sprintf(s, "LD (%04x),A", w);
		    break;
		case 0x3a:
		    sprintf(s, "LD A,(%04x)", w);
		    break;
		}
		break;
	    case 0x10: case 0x18: case 0x20:
	    case 0x28: case 0x30: case 0x38:
		/* jump relative */
#if 1
		if ((w = *++PC) & 0x80)
		    w -= 256;
		w += pc + 2;
#else
		w = *++PC;	/* no service for destination address */
#endif
		sprintf(s, op1tab[o], w);
		break;
	    default:
		strcpy(s, op1tab[o]);
		break;
	    }
	}
	break;	/* lower block done */
    case 1:		/* second block is easy */
	if (o != 0x76)
	    sprintf(s, "LD %s,%s", regh, regl);
	else
	    strcpy(s, "HALT");
	break;
    case 2:		/* third block is easy */
	strcpy(s, dtab5[m]);
	strcat(s, regl);
	break;
    case 3:		/* fourth block */
	switch(o & 7) {
	case 0:
	    sprintf(s, "RET %s", dtab4[m]);
	    break;
	case 2:
	    w = *++PC;
	    w += *++PC << 8;
	    sprintf(s, "JP %s,A%04xx", dtab4[m], w);
	    break;
	case 4:
	    w = *++PC;
	    w += *++PC << 8;
	    sprintf(s, "CALL %s,A%04xx", dtab4[m], w);
	    break;
	case 6:
	    sprintf(s, "%s0%02x", dtab5[m], *++PC);
	    break;
	case 7:
	    sprintf(s, "RST %03xH", m << 3);
	    break;
	default:
	    switch (o) {
	    case 0xc3: case 0xcd:
		w = *++PC;
		w += *++PC << 8;
		break;
	    case 0xd3: case 0xdb:
		w = *++PC;
		break;

	    /* SPECIAL Z80 COMMANDS START HERE: */
	    case 0xcb:	/* rotate-commands */
	    ddcb:
		o = *++PC;
		regl = dtab1[r = o & 7];
		m = (o >> 3) & 7;
		if (o & 0xc0)
		    sprintf(s, "%s %d,%s", cbtab2[o >> 6], m, dtab1[r]);
		else
		    sprintf(s, "%s %s", cbtab1[m], dtab1[r]);
		return PC+1;
	    case 0xed:
		return edcommand(PC+1, s);
	    case 0xdd:
		ir = "IX";
		goto ixiy;
	    case 0xfd:
		ir = "IY";
	    ixiy:
		dtab1[6] = i_offset;
		m = PC[2];	/* offset */
		sprintf(i_offset, "(%s%c%02x)", ir, m&0x80 ? '-' : '+',
			m&0x80 ? 256-m : m);
		o = *++PC;
		if (o == 0xcb) {
		    /* rotate with index reg */
		    o = PC[2];
		    if ((o & 7) != 6)
			return PC-1;	/* invalid opcode! */
		    ++PC;	/* skip offset */
		    goto ddcb;
		}
		if (ixy_possible8[o >> 3] & (1<<(o&7))) {
		    /* is a command which addresses 8 bit index+offset */
		    ++PC;	/* skip offset */
		    goto redo;
		}
		{   unsigned char *p, xtra1[] = {
		    0x21, 0x22, 0x2a, 0x09, 0x19, 0x29, 0x39, 0x23, 0x2b, 0
		    }, xtra2[] = {
		    0xe1, 0xe3, 0xe5, 0xe9, 0xeb, 0xf9, 0 };
		    if (o) {
			if ((p = strchr(xtra1, o)))
			    goto redo;
			else if ((p = strchr(xtra2, o))) {
			    sprintf(s, dtabix[p-xtra2], ir);
			    return PC+1;
			}
		    }
		}
		return PC-1;	/* unknown opcode */
	    }
	    sprintf(s, op2tab[o & 63], w);
	    break;
	}
	break;
    }
    return PC+1;
}

