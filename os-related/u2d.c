
#include	<stdio.h>
#include	<stdlib.h>

/*
 *	Unix To DOS text file conversion utility. 
 *
 *	Converts the Unix style end-of-line sequence 0AH
 *	to MS-DOS style end-of-line sequence 0DH 0AH
 *
 *  Requires z88dk (CP/M and OSCA targets)
 *  zcc +cpm -create-app u2d.c
 *  zcc +osca -lflosxdos -create-app u2d.c
 *
 */

#define	TMPNAME	"$U2D$TMP.$$$"

/*
 *	int	fixfile(char * filename)
 *
 *	"Fix" the text file specified by "filename".
 *	Returns 0 on error, non zero otherwise.
 */


	FILE *		infile;
	FILE *		outfile;
	char		ch;
	unsigned long	in, out;

int fixfile(char * filename)
{

	printf("u2d: %s: ", filename);
	fflush(stdout);
	printf(" %s - ",filename);
	infile = fopen(filename, "rb");	/* try to open input file */
	if (!infile) {
		printf("not found\n");
		return 0;
	}
	outfile = fopen(TMPNAME, "wb");	/* try to open scratch file */
	if (!outfile) {
		fclose(infile);
		printf("unable to open temp file %s\n", TMPNAME);
		return 1;
	}
	in = out = 0;
	while ((ch = fgetc(infile)) != EOF) {	/* while not end of file */
		if (fputc(ch, outfile) == EOF) {
			fclose(infile);
			fclose(outfile);
			printf("error writing to temp file %s\n", TMPNAME);
			return 1;
		}
		++in;
		++out;
		if (ch == '\n')
			++out;
	}
	fclose(infile);
	fclose(outfile);
	if (remove(filename) != 0) {
		printf("unable to delete %s\n", filename);
		return 1;
	}
	if (rename(TMPNAME, filename) != 0) {
		printf("unable to rename %s to %s\n", TMPNAME, filename);
		return 1;
	}
	printf("converted: %lu bytes read, %lu bytes written\n", in, out);
	return 0;

}

main(int argc, char ** argv)
{
	int	i,x;

	if (argc == 1) {
		fprintf(stderr, "u2d: usage: u2d file1 file2 ...\n");
		fprintf(stderr, "u2d: wildcards, e.g. *.c are expanded\n");
		exit(1);
	}
	
	    for (i = 1; i < argc; ++i) {
			if ((x=dir_move_first())!=0) return(0);

			while (x == 0) {
				if (wcmatch(argv[i],dir_get_entry_name())) {
					if (fixfile(argv[i])) {
						fprintf(stderr, "u2d: aborted!\n");
						exit(1);
					}
				}
				x = dir_move_next();
			}
	    }
}
