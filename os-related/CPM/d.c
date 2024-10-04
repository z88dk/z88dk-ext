
/* [@D.C of JUGPDS Vol.18]
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

/* @D - a directory program */

//  zcc +cpm -create-app -O3 -DAMALLOC d.c
//  (remember the -subtype=xxx flag for specific CP/M disk formats)


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <cpm.h>

#define max(a,b) (a>b?a:b)


struct tnode {
	char  *filename;
	int   fs;
	struct tnode *left;
	struct tnode *right;
};

int   bls, spt, sector, off, sectcnt, fcnt;
int   *sectran, *dph;
char  *dpb;



char *compafn(char *s, char *t)
{
	while ((*s == *t || *s == '?') && *s != '\0') {
		s++;
		t++;
		}
	return (*s == '\0');
}



struct tnode *talloc()
{
//	char *alloc();
	struct tnode *p;

	//return(p = alloc(sizeof(*p)));
	return(p = malloc(sizeof(*p)));
}


void error(char *str)
{
	fprintf(stderr,"%s\n", str);
	exit(1);
}


char *strsave(char *s)
{
char *p;
    if ((p = malloc(strlen(s)+1)) != NULL)
	strcpy(p, s);
    return p;
}


struct tnode *tree(struct tnode *p, char *fname, int fsize)
{
	struct tnode *talloc();
	//char *strsave();
	int  cond;

	if (p == NULL) {
		if ((p = talloc()) == NULL)
			return NULL;
		else if ((p->filename = strsave(fname)) == NULL)
			return NULL;
		else {
			p->fs = fsize;
			p->left = p->right = NULL;
			}
		}
	else if ((cond = strcmp(fname, p->filename)) == 0)
		p->fs = max(p->fs, fsize);
	else if (cond < 0)
		p->left = tree(p->left, fname, fsize);
	else
		p->right = tree(p->right, fname, fsize);
	return(p);
}


void treeprint(struct tnode *p)
{
	if (p != NULL) {
		treeprint(p->left);
		p->fs = (p->fs -1 ) / 8 + 1;
		p->fs = ((p->fs -1 ) / bls + 1) * bls;
		if (++fcnt%4)
			printf("%2d/%11s%4d |",
				 --(*p->filename), p->filename+1, p->fs);
		else
			printf("%2d/%11s%4d\n",
				 --(*p->filename), p->filename+1, p->fs);
		treeprint(p->right);
	}
}








user(char *s)
{
	int n;

	n = 0;
	if (isdigit(*s))
		while (isdigit(*s))
			n = n * 10 + *s++ - '0';
	else if (*s == '?')
		return ('?');
	return (n+1);
}


char *cindex(char *s, char *c)
{ 
	while (*s) {
		if (toupper(*s) == c)
			return s;
		s++;
		}
	return (-1);
}



int main(int argc, char *argv[])
{
	char name[14], *p, *pp, *adrive, *aucode, *dfname, *fn;
	char cdrive, cucode, buf[128]; //, *cindex();
	struct tnode *root; //, *tree();
	int    i, n, blm, fsize;

	//_allocp = NULL;

	fcnt = 0;
	adrive = &name[0];
	aucode = &name[1];
	dfname = &name[2];
	name[13] = '\0';
	p = dfname;
	for (i = 2; i < 13; i++) *p++ = '?';
	*adrive = cdrive = bdos(25,0);
	*aucode = cucode = bdos(32,255) + 1;

	if (argc > 1) {
		p = argv[1];
		if ((pp = cindex(p, '/')) != -1) {
			*aucode = user(p);
			p = pp + 1;
			}
		if ((pp = cindex(p, ':')) != -1) {
			if (*p >= 'A' && *p <= 'P')
				*adrive = *p - 'A';
			p = pp + 1;
			}
		pp = &name[2];
		if (*p != '\0') {
			for (i = 1; i <= 8; i++)
				if (*p == '\0' || *p == '.')
					*pp++ = ' ';
				else if (*p == '*')
					*pp++ = '?';
				else
					*pp++ = *p++;
			}
		pp = &name[10];
		if (*p == '*')
			p++;
		if (*p == '.')
			p++;
		if (*p != '\0') {
			for (i = 9; i <= 11; i++)
				if (*p == '\0')
					*pp++ = ' ';
				else if (*p == '*')
					*pp++ = '?';
				else
					*pp++ = *p++;
			}
		}
	//dph = biosh( 9,*adrive);
	dph = biosh( 9,*adrive,0);
	sectran = *dph;
	dpb = *(dph + 5);
	spt = *dpb;
	sectcnt = *(dpb + 7) + *(dpb + 8) * 256 + 1;
	sectcnt /= 4;
	blm = *(dpb + 3);
	bls = (blm + 1) / 8;
	off = *(dpb + 13) + *(dpb+14) * 256;
	root = NULL;
	for (sector = 0; sector < sectcnt; sector++) {
		bios(10, off+sector/spt, 0);
		//bios(11, biosh(16, sector%spt, sectran), 0);
		bios(11, biosh(16, sector%spt, sectran), 0);
		bios(12, buf, 0);
		if (bios(13,0,0) == 0)
			for (i = 0; i < 4; i++) {
				fn = &buf[32*i];
				fsize = *(fn +12) * 128 + *(fn + 15);
				*(fn + 12) = '\0';
				if (*fn < 0 || *fn > 15)
					continue;
				(*fn)++;
				if (!compafn(aucode, fn))
					continue;
				if ((root = tree(root, fn, fsize)) == NULL)
					error("alloc over flow.");
				}
		}
	*adrive += 'A';
	treeprint(root);
	bdos(14, cdrive);
	bdos(32, cucode-1);
}
