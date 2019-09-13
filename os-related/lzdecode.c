
/*
	NOTE: When built with z88dk THE DECOMPRESSION FAILS WITH DEFAULT OPTIMIZATION LEVELS  !

	To get a correct build :
	zcc +cpm -create-app -lm -O0 lzdecode.c
*/

/* +++Date last modified: 05-Jul-1997 */

/**************************************************************
    lzhuf.c
    written by Haruyasu Yoshizaki 1988/11/20
    some minor changes 1989/04/06
    comments translated by Haruhiko Okumura 1989/04/07
    getbit and getbyte modified 1990/03/23 by Paul Edwards
      so that they would work on machines where integers are
      not necessarily 16 bits (although ANSI guarantees a
      minimum of 16).  This program has compiled and run with
      no errors under Turbo C 2.0, Power C, and SAS/C 4.5
      (running on an IBM mainframe under MVS/XA 2.2).  Could
      people please use YYYY/MM/DD date format so that everyone
      in the world can know what format the date is in?
    external storage of filesize changed 1990/04/18 by Paul Edwards to
      Intel's "little endian" rather than a machine-dependant style so
      that files produced on one machine with lzhuf can be decoded on
      any other.  "little endian" style was chosen since lzhuf
      originated on PC's, and therefore they should dictate the
      standard.
    initialization of something predicting spaces changed 1990/04/22 by
      Paul Edwards so that when the compressed file is taken somewhere
      else, it will decode properly, without changing ascii spaces to
      ebcdic spaces.  This was done by changing the ' ' (space literal)
      to 0x20 (which is the far most likely character to occur, if you
      don't know what environment it will be running on.
**************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

FILE  *infile, *outfile;
static unsigned long int  textsize = 0, codesize = 0, printcount = 0;

char wterr[] = "Can't write.";

static void Error(char *message)
{
    printf("\n%s\n", message);
    exit(EXIT_FAILURE);
}

/********** LZSS compression **********/

#define N       4096    /* buffer size */
#define F       60  /* lookahead buffer size */
#define THRESHOLD   2
#define NIL     N   /* leaf of tree */

unsigned char
        text_buf[N + F - 1];
static int     match_position, match_length,
        lson[N + 1], rson[N + 257], dad[N + 1];

static void InitTree(void)  /* initialize trees */
{
    int  i;

    for (i = N + 1; i <= N + 256; i++)
        rson[i] = NIL;        /* root */
    for (i = 0; i < N; i++)
        dad[i] = NIL;         /* node */
}

static void InsertNode(int r)  /* insert to tree */
{
    int  i, p, cmp;
    unsigned char  *key;
    unsigned c;

    cmp = 1;
    key = &text_buf[r];
    p = N + 1 + key[0];
    rson[r] = lson[r] = NIL;
    match_length = 0;
    for ( ; ; ) {
        if (cmp >= 0) {
            if (rson[p] != NIL)
                p = rson[p];
            else {
                rson[p] = r;
                dad[r] = p;
                return;
            }
        } else {
            if (lson[p] != NIL)
                p = lson[p];
            else {
                lson[p] = r;
                dad[r] = p;
                return;
            }
        }
        for (i = 1; i < F; i++)
            if ((cmp = key[i] - text_buf[p + i]) != 0)
                break;
        if (i > THRESHOLD) {
            if (i > match_length) {
                match_position = ((r - p) & (N - 1)) - 1;
                if ((match_length = i) >= F)
                    break;
            }
            if (i == match_length) {
                if ((c = ((r - p) & (N-1)) - 1) < (unsigned)match_position) {
                    match_position = c;
                }
            }
        }
    }
    dad[r] = dad[p];
    lson[r] = lson[p];
    rson[r] = rson[p];
    dad[lson[p]] = r;
    dad[rson[p]] = r;
    if (rson[dad[p]] == p)
        rson[dad[p]] = r;
    else
        lson[dad[p]] = r;
    dad[p] = NIL; /* remove p */
}

static void DeleteNode(int p)  /* remove from tree */
{
    int  q;

    if (dad[p] == NIL)
        return;         /* not registered */
    if (rson[p] == NIL)
        q = lson[p];
    else
    if (lson[p] == NIL)
        q = rson[p];
    else {
        q = lson[p];
        if (rson[q] != NIL) {
            do {
                q = rson[q];
            } while (rson[q] != NIL);
            rson[dad[q]] = lson[q];
            dad[lson[q]] = dad[q];
            lson[q] = lson[p];
            dad[lson[p]] = q;
        }
        rson[q] = rson[p];
        dad[rson[p]] = q;
    }
    dad[q] = dad[p];
    if (rson[dad[p]] == p)
        rson[dad[p]] = q;
    else
        lson[dad[p]] = q;
    dad[p] = NIL;
}

/* Huffman coding */

#define N_CHAR      (256 - THRESHOLD + F)
                /* kinds of characters (character code = 0..N_CHAR-1) */
#define T       (N_CHAR * 2 - 1)    /* size of table */
#define R       (T - 1)         /* position of root */
#define MAX_FREQ    0x8000      /* updates tree when the */
typedef unsigned char uchar;


/* table for encoding and decoding the upper 6 bits of position */



/* for decoding */
uchar d_code[256] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
    0x0C, 0x0C, 0x0C, 0x0C, 0x0D, 0x0D, 0x0D, 0x0D,
    0x0E, 0x0E, 0x0E, 0x0E, 0x0F, 0x0F, 0x0F, 0x0F,
    0x10, 0x10, 0x10, 0x10, 0x11, 0x11, 0x11, 0x11,
    0x12, 0x12, 0x12, 0x12, 0x13, 0x13, 0x13, 0x13,
    0x14, 0x14, 0x14, 0x14, 0x15, 0x15, 0x15, 0x15,
    0x16, 0x16, 0x16, 0x16, 0x17, 0x17, 0x17, 0x17,
    0x18, 0x18, 0x19, 0x19, 0x1A, 0x1A, 0x1B, 0x1B,
    0x1C, 0x1C, 0x1D, 0x1D, 0x1E, 0x1E, 0x1F, 0x1F,
    0x20, 0x20, 0x21, 0x21, 0x22, 0x22, 0x23, 0x23,
    0x24, 0x24, 0x25, 0x25, 0x26, 0x26, 0x27, 0x27,
    0x28, 0x28, 0x29, 0x29, 0x2A, 0x2A, 0x2B, 0x2B,
    0x2C, 0x2C, 0x2D, 0x2D, 0x2E, 0x2E, 0x2F, 0x2F,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
};

uchar d_len[256] = {
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
};

unsigned freq[T + 1]; /* frequency table */

int prnt[T + N_CHAR]; /* pointers to parent nodes, except for the */
            /* elements [T..T + N_CHAR - 1] which are used to get */
            /* the positions of leaves corresponding to the codes. */

int son[T];   /* pointers to child nodes (son[], son[] + 1) */

unsigned getbuf = 0;
uchar getlen = 0;

static int GetBit(void)    /* get one bit */
{
    unsigned i;
	
    while (getlen <= 8) {
        if ((int)(i = getc(infile)) < 0) i = 0;
        getbuf |= i << (8 - getlen);
		getlen += 8;
    }
    i = getbuf;
    getbuf <<= 1;
    getlen--;
	
    return (int)((i & 0x8000) >> 15);
}

static int GetByte(void)   /* get one byte */
{
    unsigned i;

    while (getlen <= 8) {
        if ((int)(i = getc(infile)) < 0) i = 0;
        getbuf |= i << (8 - getlen);
        getlen += 8;
    }
    i = getbuf;
    getbuf <<= 8;
    getlen -= 8;

    return (int)((i & 0xff00) >> 8);
}




/* initialization of tree */

static void StartHuff(void)
{
    int i, j;

    for (i = 0; i < N_CHAR; i++) {
        freq[i] = 1;
        son[i] = i + T;
        prnt[i + T] = i;
    }
    i = 0; j = N_CHAR;
    while (j <= R) {
        freq[j] = freq[i] + freq[i + 1];
        son[j] = i;
        prnt[i] = prnt[i + 1] = j;
        i += 2; j++;
    }
    freq[T] = 0xffff;
    prnt[R] = 0;
}


/* reconstruction of tree */

static void reconst(void)
{
    int i, j, k;
    unsigned f, l;

    /* collect leaf nodes in the first half of the table */
    /* and replace the freq by (freq + 1) / 2. */
    j = 0;
    for (i = 0; i < T; i++) {
        if (son[i] >= T) {
            freq[j] = (freq[i] + 1) / 2;
            son[j] = son[i];
            j++;
        }
    }
    /* begin constructing tree by connecting sons */
    for (i = 0, j = N_CHAR; j < T; i += 2, j++) {
        k = i + 1;
        f = freq[j] = freq[i] + freq[k];
        for (k = j - 1; f < freq[k]; k--);
        k++;
        l = (j - k) * 2;
        memmove(&freq[k + 1], &freq[k], l);
        freq[k] = f;
        memmove(&son[k + 1], &son[k], l);
        son[k] = i;
    }
    /* connect prnt */
    for (i = 0; i < T; i++) {
        if ((k = son[i]) >= T) {
            prnt[k] = i;
        } else {
            prnt[k] = prnt[k + 1] = i;
        }
    }
}


/* increment frequency of given code by one, and update tree */

static void update(int c)
{
    int i, j, k, l;

    if (freq[R] == MAX_FREQ) {
        reconst();
    }
    c = prnt[c + T];
    do {
        k = ++freq[c];

        /* if the order is disturbed, exchange nodes */
        if ((unsigned)k > freq[l = c + 1]) {
            while ((unsigned)k > freq[++l]);
            l--;
            freq[c] = freq[l];
            freq[l] = k;

            i = son[c];
            prnt[i] = l;
            if (i < T) prnt[i + 1] = l;

            j = son[l];
            son[l] = i;

            prnt[j] = c;
            if (j < T) prnt[j + 1] = c;
            son[c] = j;

            c = l;
        }
    } while ((c = prnt[c]) != 0); /* repeat up to root */
}

unsigned code, len;




static int DecodeChar(void)
{
    unsigned c;

    c = son[R];

    /* travel from root to leaf, */
    /* choosing the smaller child node (son[]) if the read bit is 0, */
    /* the bigger (son[]+1} if 1 */
    while (c < T) {
        c += GetBit();
        c = son[c];
    }
    c -= T;
    update(c);
    return (int)c;
}

static int DecodePosition(void)
{
    unsigned i, j, c;

    /* recover upper 6 bits from table */
    i = GetByte();
    c = (unsigned)d_code[i] << 6;
    j = d_len[i];

    /* read lower 6 bits verbatim */
    j -= 2;
    while (j--) {
        i = (i << 1) + GetBit();
    }
    return (int)(c | (i & 0x3f));
}



static void Decode(void)  /* recover */
{
    int  i, j, k, r, c;
    unsigned long int  count;

    textsize = fgetc(infile);
    textsize |= ((unsigned long)fgetc(infile) << 8);
    textsize |= ((unsigned long)fgetc(infile) << 16);
    textsize |= ((unsigned long)fgetc(infile) << 24);
	
    if (ferror(infile))
        Error("Can't read");  /* read size of text */
    if (textsize == 0)
        return;
    StartHuff();
    for (i = 0; i < N - F; i++)
        text_buf[i] = 0x20;
    r = N - F;
    for (count = 0; count < textsize; ) {
        c = DecodeChar();
        if (c < 256) {
            if (putc(c, outfile) == EOF) {
                Error(wterr);
            }
            text_buf[r++] = (unsigned char)c;
            r &= (N - 1);
            count++;
        } else {
            i = (r - DecodePosition() - 1) & (N - 1);
            j = c - 255 + THRESHOLD;
            for (k = 0; k < j; k++) {
                c = text_buf[(i + k) & (N - 1)];
                if (putc(c, outfile) == EOF) {
                    Error(wterr);
                }
                text_buf[r++] = (unsigned char)c;
                r &= (N - 1);
                count++;
            }
        }
        if (count > printcount) {
            printf("%12ld\r", count);
            printcount += 1024;
        }
    }
    printf("%12ld\n", count);
}

int main(int argc, char *argv[])
{
    char  *s;

    if (argc != 3) {
        printf("'lzdecode d file2 file1' decodes file2 into file1.\n");
        return EXIT_FAILURE;
    }
    if ((s = argv[1], (infile = fopen(s, "rb")) == NULL)
     || (s = argv[2], (outfile = fopen(s, "wb")) == NULL)) {
        printf("??? %s\n", s);
        return EXIT_FAILURE;
    }

    Decode();
	
    fclose(infile);
    fclose(outfile);
    return EXIT_SUCCESS;
}
