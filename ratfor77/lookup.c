#include <stdio.h>
#include <string.h>
#include "lookup.h"

static
struct	hashlist *hashtab[HASHMAX];

/*
 * from K&R "The C Programming language"
 * Table lookup routines
 *
 * hash - for a hash value for string s
 *
 */
#ifdef Z80
	int hash(S_CHAR *s) __z88dk_fastcall
#else
	int hash(S_CHAR *s)
#endif
{
	int	hashval;

	for (hashval = 0; *s != '\0';)
		hashval += *s++;
	return (hashval % HASHMAX);
}

/*
 * lookup - lookup for a string s in the hash table
 *
 */
struct hashlist *lookup(S_CHAR *s)
{
	struct hashlist *np;

	for (np = hashtab[hash(s)]; np != NULL; np = np->next)
		if (strcmp(s, np->name) == 0)
			return(np);	/* found     */
	return(NULL);		/* not found */
}


/*
 * strsave - save string s somewhere
 *
 */
S_CHAR *strsave(S_CHAR *s)
{
	//S_CHAR *p, *malloc();
	S_CHAR *p;

	if ((p = malloc(strlen(s)+1)) != NULL)
		strcpy(p, s);
	return(p);
}


/*
 * install - install a string name in hashtable and its value def
 *
 */
struct hashlist *install(S_CHAR *name, S_CHAR *def)
{
	int hashval;
	struct hashlist *np, *lookup();
	//S_CHAR *strsave(), *malloc();

	if ((np = lookup(name)) == NULL) {	/* not found.. */
		np = (struct hashlist *) malloc(sizeof(*np));
		if (np == NULL)
			return(NULL);
		if ((np->name = strsave(name)) == NULL)
			return(NULL);
		hashval = hash(np->name);
		np->next = hashtab[hashval];
		hashtab[hashval] = np;
	} else					/* found..     */
		free(np->def);			/* free prev.  */
	if ((np->def = strsave(def)) == NULL)
		return(NULL);
	return(np);
}

