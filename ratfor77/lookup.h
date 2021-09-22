
/*
 * from K&R "The C Programming language"
 * Table lookup routines
 * structure and definitions
 *
 */

					/* basic table entry */
struct hashlist {
	S_CHAR	*name;
	S_CHAR	*def;
	struct	hashlist *next;		/* next in chain     */
};

#define HASHMAX	100			/* size of hashtable */

					/* hash table itself */


extern struct hashlist *lookup(S_CHAR *s);
