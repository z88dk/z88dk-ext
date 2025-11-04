/* [CHANGE.C of JUGPDS Vol.18]
*****************************************************************
*								*
*	Written by  Hakuo Katayose (JUG-CP/M No.179)		*
*		    49-114 Kawauchi-Sanjuunin-machi		*
*		    Sendai, Miyagi 980                          *
*		    Phone: 0222-61-3219				*
*								*
*	Edited & tested by Y. Monma (JUG-C/M Disk Editor)       * 
*								*
*****************************************************************
*/


/* NOTE: at the moment on z88dk it works well only if both the file redirections are specified. */ 


/* change - change "from" into "to" 

	Usage: change [-l][-v] from [to] <infile [>outfile]

where
	-l: line # 
	-v: 

*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAXPAT   127
#define MAXLINE  253
#define MAXSET   253

#define EOS        '\0'
#define NEWLINE    '\n'
#define BACKSPACE  0x008
#define TAB        '\t'
#define CRETURN    '\r'
#define FORMFEED   0x00c
#define BLANK      ' '


#define YES    1
#define NO     0
#define ON     YES
#define OFF    NO

#define ERROR    -1
#define	OK       -2                /* Append 1986-11-25 */
#define DITTO    (char)-3                /* Append 1987-01-18 */


#define NOT      0x021             /* Append 1986-11-24 */
#define	EOL      0x024  /*'$'*/    /* Append 1986-11-25 */
#define	BOL      0x025  /*'%'*/    /* Append 1986-11-25 */
#define	PLUS     0x02b  /*'+'*/    /* Append 1986-11-25 */
#define	DASH     0x02d  /*'-'*/    /* Append 1986-11-25 */
#define	ANY      0x03f  /*'?'*/    /* Append 1986-11-25 */
#define	CHAR     0x061  /*'a'*/    /* Append 1986-11-25 */
#define	CCL      0x05b  /*'['*/    /* Append 1986-11-25 */
#define	CCLEND   0x05d  /*']'*/    /* Append 1986-11-25 */
#define NCCL     0x06f  /*'n'*/    /* Append 1986-11-25 */
#define BPAT     0x07b             /* Append 1986-11-25 */
#define	NPAT     0x07b             /* Append 1986-11-25 */
#define	NPATEND  0x07d             /* Append 1986-11-25 */
#define	EPAT     0x07d             /* Append 1986-11-25 */
#define	SUBSTRG  0x07e  /*'~'*/    /* Append 1987-01-18 */
#define AND      0x026  /*'&'*/    /* Append 1987-01-18 */
#define	ESCAPE   0x040  /*'@'*/    /* Append 1986-11-25 */
#define	CLOSIZE  4                 /* Append 1986-11-25 */
#define	CLOSURE  0x02a  /*'*'*/    /* Append 1986-11-25 */

#define COUNT    1                 /* Append 1987-01-18 */
#define PREVCL   2                 /* Append 1987-01-18 */
#define START    3                 /* Append 1987-01-18 */


#define remark(str)	(puts(str))

#define _copy(a,b,c) strncpy(a,b,c)

/*
void _copy(char *s, char *d, int l)
{
int	i;

	for (i=0; i < l; i++)
		*d++ = *s++;
	return;
}
*/


//#define cindex(a,b) index(a,b)

/* cindex - find character c in string str */

int  cindex(char *str, int c)
{
int	i;
	i = 0;
	while(*str && *str != c) *str++, i++;
	return (*str == EOS ? ERROR : i);
}



void error(char *str)
{
	fprintf(stderr,"%s\n", str);
	exit(1);
}



/* catsub - add replacement text to end of new */
void catsub(char lin[], int from, int to,char new[], int *k, int maxnew, char sub[10][MAXPAT])
//int	from, to, *k, maxnew;
//char	lin[], new[], sub[10][MAXPAT];
{
	int	i, j, ns;

	for (i = 0; sub[0][i] != EOS && *k < maxnew; i++) {
		if (sub[0][i] == DITTO) {
			for (j = from; j < to && *k < maxnew; j++)
				new[(*k)++] = lin[j];
			}
		else if (sub[0][i] == SUBSTRG) {
			ns = sub[0][++i] - '0';
			for (j = 0; sub[ns][j] != EOS && *k < maxnew; j++)
				new[(*k)++] = sub[ns][j];
			}
		else if (*k >= maxnew)
			break;
		else
			new[(*k)++] = sub[0][i];
		}
}



/* esc - map array[i] into escaped charaters if appropriate */
int  esc(char array[], int *i)
{
char	c;

	c = array[*i];					/*Modify 1986-11-27*/
	if (c != ESCAPE)
		return( (int) c);
	if (array[(*i)+1] == EOS)
		return(ESCAPE);
	c = array[++(*i)];
	c = tolower(c);
	if (c == 'b')
		return(BACKSPACE);
	if (c == 'f')
		return(FORMFEED);
	if (c == 'n')
		return(NEWLINE);
	if (c == 'r')
		return(CRETURN);
	if (c == 's')
		return(BLANK);
	if (c == 't')
		return(TAB);
	return( (int) c);
}



/* maksub -make substitution string in sub */
int  maksub(char arg[], int from, char delim, char *sub)
{
	int	i;

	for (i = from; arg[i] != delim && arg[i] != EOS; i++)
		if (arg[i] == AND)
			*sub++ = DITTO;
		else if (arg[i] == ESCAPE && (isdigit(arg[i+1]))) {
			*sub++ = SUBSTRG;
			*sub++ = arg[++i];
			}
		else
			*sub++ = esc(arg, &i);
	*sub = EOS;
	if (arg[i] != delim)
		return ERROR;
	if (strlen(sub) > MAXLINE)
		return ERROR;
	return(i);
}



/* getsub - get substitution pattern into sub */
int  getsub(char	*arg, char *sub)
{
	return(maksub(arg, 0, EOS, sub));
}



/* copysub - copy line to sub from b to e */ 
void copysub(char lin[], int b, int e, char *sub)
{
	for( ; b <= e; b++)
		*sub++ = lin[b];
	*sub = EOS;
	return;
}



/* addset - put c in set[j] if it fits, increment j */
int  addset(char c, char set[], int *j, int maxsiz)
{
	if (*j > maxsiz)
		return(NO);
	set[(*j)++] = c;
	return(YES);
}



/* dodash - expand array[i-1] -array[i+1] into set[j]...from valid */
void dodash(char valid[], char array[], int *i, char set[], int *j, int maxsiz)
{
int	k, limit;

	(*i)++;
	(*j)--;
	limit = cindex(valid, esc(array, i));
	for (k = cindex(valid, set[*j]); k <= limit; k++)
		addset(valid[k], set, j, maxsiz);
}


char digits[] = "0123456789";
char lowalf[] = "abcdefghijklmnopqrstuvwxyz";
char upalf[]  = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char kana[]   = "¦§§¨©ª«¬­®¯±²³´µ¶·¸¹º»¼½¾¿ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜ¦Ý";


/* filset - expand set at array[i] into set[j], stop at delim */ 
void filset(char delim, char array[], int *i, char set[], int *j, int maxsiz)
{
//char	*digits, *lowalf, *upalf ,*kana, c;
//void dodash();

char c;

	for (; (c = array[*i]) != delim && c != EOS; (*i)++)
		if (c == ESCAPE)			/*Modify 1986-11-27*/
			addset(esc(array, i), set, j, maxsiz);
		else if (c != DASH)
			addset(c, set, j, maxsiz);	/*Modify 1986-11-27*/
		else if (*j == 0 || array[(*i)+1] == EOS)  /*Last Literal*/
			addset(DASH, set, j, maxsiz);
		else if (cindex(digits, set[(*j)-1]) != ERROR)
			dodash(digits, array, i, set, j, maxsiz);
		else if (cindex(lowalf, set[(*j)-1]) != ERROR)
			dodash(lowalf, array, i, set,j, maxsiz);
		else if (cindex(upalf, set[(*j)-1])  != ERROR)
			dodash(upalf, array, i, set, j, maxsiz);
		else if (cindex(kana , set[(*i)-1])  != ERROR)
			dodash(kana , array, i, set, j, maxsiz);
		else
			addset(DASH, set, j, maxsiz);
	return;
}



/* getccl - expand char class at arg[i] into pat[j] */
int  getccl(char arg[], int *i, char pat[], int *j)
{
int  jstart;
//void filset();

	(*i)++;
	if (arg[*i] == NOT) {
		pat[(*j)++] = NCCL;
		(*i)++;
		}
	else
		pat[(*j)++] = CCL;
	jstart = *j;
	addset(0, pat, j, MAXSET);
	filset(CCLEND, arg, i, pat, j, MAXSET);
	pat[jstart] = (*j) - jstart - 1;
	return (arg[*i] == CCLEND) ? OK : ERROR;
}



/* stclos - insert closure entry at pat[j] */
int  stclos(char pat[], int *j, int lastj, int lastcl)
{
int	jt, jp;

	for (jp = (*j) - 1; jp >= lastj; jp--) {
		jt = jp + CLOSIZE;
		addset(pat[jp], pat, &jt, MAXSET);
		}
	(*j) += CLOSIZE;
	jp = lastj;
	addset(CLOSURE, pat, &lastj, MAXSET);
	addset(0, pat, &lastj, MAXSET);
	addset(lastcl, pat, &lastj, MAXSET);
	addset(0, pat, &lastj, MAXSET);
	return jp;
}



/* makpat - make pattern from arg[from], terminate at delim */
int  makpat(char arg[], int from, char delim, char pat[])
{
int	i, j, lastj, lastcl, lj, len, k, kk[10], dp;
char	c;
//int  addset();
//void _copy();

	j = 0; lastj = 0; lastcl = 0;
	k = '1'; dp = 0;
	for (i = from; arg[i] != delim && arg[i] != EOS; i++) {
		lj = j;
		c = arg[i];
		if (c == NPAT) {
			addset(BPAT, pat, &j, MAXSET);
			addset(k, pat, &j, MAXSET);
			kk[dp++] = k++;
			}
		else if (c == NPATEND) {
			addset(EPAT, pat, &j, MAXSET);
			addset(kk[--dp], pat, &j, MAXSET);
			}
		else if (c == ANY) {
			addset(ANY, pat, &j, MAXSET);
			}
		else if (c == BOL && i ==from) {
			addset(BOL, pat, &j, MAXSET);
			}
		else if (c == EOL && arg[i+1] == delim) {
			addset(EOL, pat, &j, MAXSET);
			}
		else if (c == CCL) {
			if (getccl(arg, &i, pat, &j) == ERROR) break;
			}
		else if ((c == CLOSURE || c == PLUS) && i > from) {
			lj = lastj;
			if (pat[lj] == BOL ||
			    pat[lj] == EOL || 
			    pat[lj] == CLOSURE)
				break;
			if (c == PLUS) {
				len = j - lj;
				_copy(&pat[lj], &pat[j], len);
				j += len;
				lj += len;
				lastj += len;
				}
			lastcl = stclos(pat, &j, lastj, lastcl);
			}
		else {
			addset(CHAR, pat, &j, MAXSET);
			addset(esc(arg, &i), pat, &j, MAXSET);
			}
		lastj = lj;
		}
	if (arg[i] != delim)
		return ERROR;
	if (j >= MAXSET)
		return ERROR;
	if (dp < 0)
		return ERROR;
	pat[j] = EOS;
	return( (int) (k - '1') );
}



/* getpat - convert argument into pattern */
int  getpat(char *arg, char *pat)
{
	return(makpat(arg, 0, EOS, pat));
}



/* locate - look for c in char class at pat(offset) */
int  locate(char c, char pat[], int	offset)
{
	int	i;

	for (i = offset + pat[offset]; i > offset; i--)
		if (c == pat[i])  return YES;
	return NO;
}



/* omatch - try to match a single pattern at pat(j) */
int omatch(char lin[], int *i, char pat[], int j)
{
	int	bump;
	char	c;

	c = lin[*i];
	if (c == EOS)
		return(NO);
	bump = -1;
	switch (pat[j]) {
		case BPAT :
		case EPAT : return(YES);
		case CHAR : if (c == pat[j+1])
				bump= 1;
			    break;
		case BOL :  if (*i == 0)
				bump = 0;
			    break;
		case ANY :  if (c != NEWLINE)
				bump = 1;
			    break;
		case EOL :  if (c == NEWLINE)
				bump = 0;
			    break;
		case CCL :  if (locate(c, pat, j+1) == YES)
				bump = 1;
			    break;
		case NCCL:  if (c != NEWLINE && locate(c, pat, j+1) == NO)
				bump = 1;
			    break;
		default  :  remark("Error in omatch: can't happen.\n");
	}
	if (bump >= 0) {
		*i += bump;
		return(YES);
		}
	return(NO);
}



/* patsiz - returns size of pattern entry at pat(n) */
int patsiz(char pat[], int n)
{
	switch (pat[n]) {
		case BPAT :
		case EPAT :
		case CHAR :
			  return(2);
		case BOL :
		case EOL :
		case ANY :
			  return(1);
		case CCL :
		case NCCL:
			  return (pat[n+1] + 2);
		case CLOSURE :
			  return CLOSIZE;
		default  :
			  error("Error in patsiz: can't happen.\n");
	}
}



/* amatch (non-recursive) - look for match starting at lin(from) */
int amatch(char lin[], int from, char pat[], char sub[10][MAXPAT], int maxsub)
//char	lin[], pat[], sub[10][MAXPAT];
//int	from, maxsub;
{
	int	i, j, offset, stack, bgn[10], lst[10], l;

	stack = 0;
	offset = from;
	for (j = 0; pat[j] != EOS; j += patsiz(pat, j))
		if (pat[j] == BPAT )
			bgn[pat[j+1]-'0'] = offset;
		else if (pat[j] == EPAT)
			lst[pat[j+1]-'0'] = offset-1;
		else if (pat[j] == CLOSURE) {
			stack = j;
			j += CLOSIZE;
			for (i = offset; lin[i] != EOS; )
				if (omatch(lin, &i, pat, j) == NO)
					break;
			pat[stack + COUNT] = i - offset;
			pat[stack + START] = offset;
			offset = i;
			}
		else if (omatch(lin, &offset, pat, j) == NO) {
			for ( ; stack > 0; stack = pat[stack + PREVCL] ) {
				if (pat[stack + COUNT] > 0)
					break;
				}
			if (stack <= 0)
				return(ERROR);
			pat[stack + COUNT]--;
			j = stack + CLOSIZE;
			offset = pat[stack + START] + pat[stack + COUNT];
			}
	for (l = 1; l <= maxsub; l++)
		copysub(lin, bgn[l], lst[l], sub[l]);
	return(offset);
}



int   getlin(char *s, int lim)
{
char *p;
int	c;

	p = s;
/*	while(--lim > 0 && (c = getchar()) != EOF && c != NEWLINE)  Original */
	while (1) {			/* fukusayou bousi */
		c = getchar();
		if(--lim<=0 || c == EOF || c == NEWLINE)
			break;
		*s++ = c;
	}
	if(c == NEWLINE)
		*s++ = c;
	*s = EOS;
	return( (int) (s-p) );
}



int main(int argc, char **argv)
{
	char	lin[MAXLINE+1], pat[MAXPAT], new[MAXLINE+1], sub[10][MAXPAT];
	char	cmd[2][MAXLINE+1];
	char	opt_l, opt_v, *ap;
	int	i, k, lastm, m, lno, maxsub, nl, nx;

	//dioinit(&argc, argv);
	opt_l = opt_v = OFF;
	nl = 1; i = 0;
#ifdef __Z88DK
//	argv+=2;
#endif
	while (--argc > 0) {
		if ((*++argv)[0] == '-') /* check option */
			for (ap = argv[0]+1; *ap != '\0'; ap++) {
				if (tolower(*ap) == 'l') {
					opt_l = ON;
					nl = 0;
					for (; isdigit(nx = *(ap+1)); ap++)
						nl = nl*10 + nx - '0';
					nl = (nl == 0 ? 1 : nl);
					}
				else if (tolower(*ap) == 'v')
					opt_v = ON;
				else
					fprintf(stderr,
					"[-%c]:Illegal option\n",tolower(*ap));
				}
		else if (i < 2)
			strcpy(cmd[i++], *argv);
		else
			remark("Too many arguments!\n");
		}
	if (i == 0)
		error("Usage: change [-l][-v] from [to] <infile [>outfile]\n");
	if ((maxsub = getpat(cmd[0], pat)) == ERROR)
		error("Illegal from pattern.\n");
	if (i > 1) {
		if (getsub(cmd[1], sub[0]) == ERROR)
			error("Illegal to pattern.\n");
		}
	else
		sub[0][0] = EOS;

	//printf("Changed to "); puts(&sub[0][0]); printf("\n");
	fprintf(stderr, "Changing %s to %s\n", cmd[0], &sub[0][0]);
	
	lno = 0;
	while (getlin(lin, MAXLINE) > 0) {
		lno += nl;	k = 0;	lastm =	-1;
		for (i = 0; lin[i] != EOS; ) {
			m = amatch(lin, i, pat, sub, maxsub);
			if (m >= 0 && lastm != m) { /* replace matched text */
				catsub(lin, i, m, new, &k, MAXLINE, sub);
				lastm = m;
				}
			if (m == ERROR || m == i) /* no match or null match */
				addset(lin[i++], new, &k, MAXLINE);
			else			/* skip matched text */
				i = m;
			}
		if (k >= MAXLINE) {
			new[MAXLINE] = EOS;
			fprintf(stderr, "line truncated.\n:%s", new);
			printf("        %s\n", new);
			}
		else
			new[k] = EOS;
		if (lastm >= 0)
			if (opt_l == ON)
				fprintf(stderr, "%6d: %s", lno, new);
			else
				fprintf(stderr, "%s", new);
		if (opt_v == ON)
			if (opt_l == ON)
				printf("%6d: %s", lno, new);
			else
				printf("%s", new );
		}
	//dioflush();
}

