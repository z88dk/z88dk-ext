/*
 * The  information  in  this  document  is  subject  to  change
 * without  notice  and  should not be construed as a commitment
 * by Digital Equipment Corporation or by DECUS.
 *
 * Neither Digital Equipment Corporation, DECUS, nor the authors
 * assume any responsibility for the use or reliability of  this
 * document or the described software.
 *
 *      Copyright (C) 1980, DECUS
 *
 * General permission to copy or modify, but not for profit,  is
 * hereby  granted,  provided that the above copyright notice is
 * included and reference made to  the  fact  that  reproduction
 * privileges were granted by DECUS.
 *
 * Compile command: cc grep -fop
 */

#include <stdio.h>

/*
 * grep.
 *
 * Runs on the Decus compiler or on vms.
 *
 * On vms, define as:
 *
 *      grep :== "$disk:[account]grep"      (native)
 *      grep :== "$disk:[account]grep grep" (Decus)
 *
 * See below for more information.
 */

char    *documentation[] = {
"grep searches a file for a given pattern",
"Execute by:",
"  grep [flags] regular_expr file_list",
"",
"Flags are single chars preceeded by '-':",
" -c  Print count of matching lines",
" -f  alt. 'file name' display action ",
" -n  Print line numbers",
" -v  Only print non-matching lines",
"",
0 };

char    *patdoc[] = {
"Valid regular_expr elements:",
"",
"Upper- and lower-case are always ignored",
"Blank lines never match.",
"",
"x, '\\', \"\\$\", '^', '$'",
"'.', ':a', ':d', ':n', \": \", ': ',",
"'*', \"fo*\", \"f\", \"foo\", etc.",
"'+', \"fo+\", '-', '[]', \"[xyz]\"",
"",
"See grep.txt for more details.",
0};

#define LMAX    512
#define PMAX    256

#define CHAR    1
#define BOL     2
#define EOL     3
#define ANY     4
#define CLASS   5
#define NCLASS  6
#define STAR    7
#define PLUS    8
#define MINUS   9
#define ALPHA   10
#define DIGIT   11
#define NALPHA  12
#define PUNCT   13
#define RANGE   14
#define ENDPAT  15

int cflag=0, fflag=0, nflag=0, vflag=0, nfile=0, debug=0;

char *pp, lbuf[LMAX], pbuf[PMAX];

extern char *cclass(), *pmatch();


/*** Main program - parse arguments & grep *************/
main(argc, argv)
int argc;
char *argv[];
{
   register char   *p;
   register int    c, i;
   int             gotpattern;

   FILE            *f;

   if (argc <= 1)
      usage("No arguments");
   if (argc == 2 && argv[1][0] == '?' && argv[1][1] == 0) {
      help(documentation);
      help(patdoc);
      return(0);
      }
   nfile = argc-1;
   gotpattern = 0;
   for (i=1; i < argc; ++i) {
      p = argv[i];
      if (*p == '-') {
         ++p;
         while (c = *p++) {
            switch(tolower(c)) {

            case '?':
               help(documentation);
               break;

            case 'C':
            case 'c':
               ++cflag;
               break;

            case 'D':
            case 'd':
               ++debug;
               break;

            case 'F':
            case 'f':
               ++fflag;
               break;

            case 'n':
            case 'N':
               ++nflag;
               break;

            case 'v':
            case 'V':
               ++vflag;
               break;

            default:
               usage("Unknown flag");
            }
         }
         argv[i] = 0;
         --nfile;
      } else if (!gotpattern) {
         compile(p);
         argv[i] = 0;
         ++gotpattern;
         --nfile;
      }
   }
   if (!gotpattern)
      usage("No pattern");
   if (nfile == 0) {
		usage("No arguments");
       //grep(stdin, 0);
		return(0);
	} else {
      fflag = fflag ^ (nfile > 0);
      for (i=1; i < argc; ++i) {
         if (p = argv[i]) {
            if ((f=fopen(p, "r")) == NULL)
               cant(p);
            else {
               grep(f, p);
               fclose(f);
            }
         }
      }
   }
}

/*** Display a file name *******************************/
file(s)
char *s;
{
   printf("File %s:\n", s);
}

/*** Report unopenable file ****************************/
cant(s)
char *s;
{
   fprintf(stderr, "%s: cannot open\n", s);
}

/*** Give good help ************************************/
help(hp)
char **hp;
{
   register char   **dp;

   for (dp = hp; *dp; ++dp)
      printf("%s\n", *dp);
}

/*** Display usage summary *****************************/
usage(s)
char    *s;
{
   fprintf(stderr, "Error:  %s\n", s);
   fprintf(stderr,
      "Usage: grep [-cfnv] pattern [file ...].  grep ? for help\n");
   exit(1);
}

/*** Compile the pattern into global pbuf[] ************/
compile(source)
char       *source;   /* Pattern to compile */
{
   register char  *s;         /* Source string pointer     */
   register char  *lp;        /* Last pattern pointer      */
   register int   c;          /* Current character         */
   int            o;          /* Temp                      */
   char           *spp;       /* Save beginning of pattern */

   s = source;
   if (debug)
      printf("Pattern = \"%s\"\n", s);
   pp = pbuf;
   while (c = *s++) {
      /*
       * STAR, PLUS and MINUS are special.
       */
      if (c == '*' || c == '+' || c == '-') {
         if (pp == pbuf ||
              (o=pp[-1]) == BOL ||
              o == EOL ||
              o == STAR ||
              o == PLUS ||
              o == MINUS)
            badpat("Illegal occurrance op.", source, s);
         store(ENDPAT);
         store(ENDPAT);
         spp = pp;               /* Save pattern end     */
         while (--pp > lp)       /* Move pattern down    */
            *pp = pp[-1];        /* one byte             */
         *pp =   (c == '*') ? STAR :
            (c == '-') ? MINUS : PLUS;
         pp = spp;               /* Restore pattern end  */
         continue;
      }
      /*
       * All the rest.
       */
      lp = pp;         /* Remember start       */
      switch(c) {

      case '^':
         store(BOL);
         break;

      case '$':
         store(EOL);
         break;

      case '.':
         store(ANY);
         break;

      case '[':
         s = cclass(source, s);
         break;

      case ':':
         if (*s) {
            switch(tolower(c = *s++)) {

            case 'a':
            case 'A':
               store(ALPHA);
               break;

            case 'd':
            case 'D':
               store(DIGIT);
               break;

            case 'n':
            case 'N':
               store(NALPHA);
               break;

            case ' ':
               store(PUNCT);
               break;

            default:
               badpat("Unknown : type", source, s);

            }
            break;
         }
         else    badpat("No : type", source, s);

      case '\\':
         if (*s)
            c = *s++;

      default:
         store(CHAR);
         store(tolower(c));
      }
   }
   store(ENDPAT);
   store(0);                /* Terminate string     */
   if (debug) {
      for (lp = pbuf; lp < pp;) {
         if ((c = (*lp++ & 0377)) < ' ')
            printf("\\%o ", c);
         else    printf("%c ", c);
        }
        printf("\n");
   }
}

/*** Compile a class (within []) ***********************/
char *cclass(source, src)
char       *source;   /* Pattern start -- for error msg. */
char       *src;      /* Class start */
{
   register char   *s;        /* Source pointer    */
   register char   *cp;       /* Pattern start     */
   register int    c;         /* Current character */
   int             o;         /* Temp              */

   s = src;
   o = CLASS;
   if (*s == '^') {
      ++s;
      o = NCLASS;
   }
   store(o);
   cp = pp;
   store(0);                          /* Byte count      */
   while ((c = *s++) && c!=']') {
      if (c == '\\') {                /* Store quoted char    */
         if ((c = *s++) == '\0')      /* Gotta get something  */
            badpat("Class terminates badly", source, s);
         else    store(tolower(c));
      }
      else if (c == '-' &&
            (pp - cp) > 1 && *s != ']' && *s != '\0') {
         c = pp[-1];             /* Range start     */
         pp[-1] = RANGE;         /* Range signal    */
         store(c);               /* Re-store start  */
         c = *s++;               /* Get end char and*/
         store(tolower(c));      /* Store it        */
      }
      else {
         store(tolower(c));      /* Store normal char */
      }
   }
   if (c != ']')
      badpat("Unterminated class", source, s);
   if ((c = (pp - cp)) >= 256)
      badpat("Class too large", source, s);
   if (c == 0)
      badpat("Empty class", source, s);
   *cp = c;
   return(s);
}

/*** Store an entry in the pattern buffer **************/
store(op)
   int op;
{
   if (pp >= &pbuf[PMAX])
      error("Pattern too complex\n");
   *pp++ = op;
}

/*** Report a bad pattern specification ****************/
badpat(message, source, stop)
char  *message;       /* Error message */
char  *source;        /* Pattern start */
char  *stop;          /* Pattern end   */
{
   fprintf(stderr, "-GREP-E-%s, pattern is\"%s\"\n", message, source);
   fprintf(stderr, "-GREP-E-Stopped at byte %d, '%c'\n",
         stop-source, stop[-1]);
   error("?GREP-E-Bad pattern\n");
}

/*** Scan the file for the pattern in pbuf[] ***********/
grep(fp, fn)
FILE       *fp;       /* File to process            */
char       *fn;       /* File name (for -f option)  */
{
   register int lno, count, m;

   lno = 0;
   count = 0;
   while (fgets(lbuf, LMAX, fp)) {
      ++lno;
      m = match();
      if ((m && !vflag) || (!m && vflag)) {
         ++count;
         if (!cflag) {
            if (fflag && fn) {
               file(fn);
               fn = 0;
            }
            if (nflag)
               printf("%d\t", lno);
            printf("%s\n", lbuf);
         }
      }
   }
   if (cflag) {
      if (fflag && fn)
         file(fn);
      printf("%d\n", count);
   }
}

/*** Match line (lbuf) with pattern (pbuf) return 1 if match ***/
match()
{
   register char   *l;        /* Line pointer       */

   for (l = lbuf; *l; ++l) {
      if (pmatch(l, pbuf))
         return(1);
   }
   return(0);
}

/*** Match partial line with pattern *******************/
char *pmatch(line, pattern)
char               *line;     /* (partial) line to match      */
char               *pattern;  /* (partial) pattern to match   */
{
   register char   *l;        /* Current line pointer         */
   register char   *p;        /* Current pattern pointer      */
   register char   c;         /* Current character            */
   char            *e;        /* End for STAR and PLUS match  */
   int             op;        /* Pattern operation            */
   int             n;         /* Class counter                */
   char            *are;      /* Start of STAR match          */

   l = line;
   if (debug > 1)
      printf("pmatch(\"%s\")\n", line);
   p = pattern;
   while ((op = *p++) != ENDPAT) {
      if (debug > 1)
         printf("byte[%d] = 0%o, '%c', op = 0%o\n",
               l-line, *l, *l, op);
      switch(op) {

      case CHAR:
         if (tolower(*l++) != *p++)
            return(0);
         break;

      case BOL:
         if (l != lbuf)
            return(0);
         break;

      case EOL:
         if (*l != '\0')
            return(0);
         break;

      case ANY:
         if (*l++ == '\0')
            return(0);
         break;

      case DIGIT:
         if ((c = *l++) < '0' || (c > '9'))
            return(0);
         break;

      case ALPHA:
         c = tolower(*l++);
         if (c < 'a' || c > 'z')
            return(0);
         break;

      case NALPHA:
         c = tolower(*l++);
         if (c >= 'a' && c <= 'z')
            break;
         else if (c < '0' || c > '9')
            return(0);
         break;

      case PUNCT:
         c = *l++;
         if (c == 0 || c > ' ')
            return(0);
         break;

      case CLASS:
      case NCLASS:
         c = tolower(*l++);
         n = *p++ & 0377;
         do {
            if (*p == RANGE) {
               p += 3;
               n -= 2;
               if (c >= p[-2] && c <= p[-1])
                  break;
            }
            else if (c == *p++)
               break;
         } while (--n > 1);
         if ((op == CLASS) == (n <= 1))
            return(0);
         if (op == CLASS)
            p += n - 2;
         break;

      case MINUS:
         e = pmatch(l, p);       /* Look for a match    */
         while (*p++ != ENDPAT); /* Skip over pattern   */
         if (e)                  /* Got a match?        */
            l = e;               /* Yes, update string  */
         break;                  /* Always succeeds     */

      case PLUS:                 /* One or more ...     */
         if ((l = pmatch(l, p)) == 0)
            return(0);           /* Gotta have a match  */
      case STAR:                 /* Zero or more ...    */
         are = l;                /* Remember line start */
         while (*l && (e = pmatch(l, p)))
            l = e;               /* Get longest match   */
         while (*p++ != ENDPAT); /* Skip over pattern   */
         while (l >= are) {      /* Try to match rest   */
            if (e = pmatch(l, p))
               return(e);
            --l;                 /* Nope, try earlier   */
         }
         return(0);              /* Nothing else worked */

      default:
         printf("Bad op code %d\n", op);
         error("Cannot happen -- match\n");
      }
   }
   return(l);
}

/*** Report an error ***********************************/
error(s)
char *s;
{
   fprintf(stderr, "%s", s);
   exit(1);
}
