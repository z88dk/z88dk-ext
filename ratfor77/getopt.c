/*
 * getopt - get option letter from argv
 */

#include <stdio.h>
#include <string.h>

extern char	*optarg;	/* Global argument pointer. */
char	*optarg;

//extern int	optind77 = 0;	/* Global argv index. */
extern int	optind77;	/* Global argv index. */
int	optind77;

static char	*scan = NULL;	/* Private scan pointer. */

int our_getopt(int argc, char *argv[], char *optstring)
{
	register char c;
	register char *place;

	optarg = NULL;

	if (scan == NULL || *scan == '\0') {
		if (optind77 == 0)
			optind77++;

		if (optind77 >= argc || argv[optind77][0] != '-' || argv[optind77][1] == '\0')
			return(EOF);
		if (strcmp(argv[optind77], "--")==0) {
			optind77++;
			return(EOF);
		}

		scan = argv[optind77]+1;
		optind77++;
	}

	c = *scan++;
	place = strchr(optstring, (int) c);

	if (place == NULL || c == ':') {
		fprintf(stderr, "%s: unknown option -%c\n", argv[0], c);
		return('?');
	}

	place++;
	if (*place == ':') {
		if (*scan != '\0') {
			optarg = scan;
			scan = NULL;
		} else {
			optarg = argv[optind77];
			optind77++;
		}
	}

	return(c);
}

