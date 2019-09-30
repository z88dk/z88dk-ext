/*
 *			A R C H X
 *
 * PDP-11/VAX  Archive extraction
 *
 */


/* zcc +cpm -create-app -O3 -Dunix -oarchx archx.c */


/*
 * Note: the )BUILD comment is extracted by a Decus C tool to construct
 * system-dependent compiler command lines.
 *
 * Text inside #ifdef DOCUMENTATION is converted to runoff by a
 * Decus C tool.
 */

/*)BUILD	$(TKBOPTIONS) = {
			TASK	= ...ARX
		}
*/


/* Temporary workaround for the z88dk bug on fputs */
#ifdef Z80
#define do_fputs(a,b)	fprintf(b,"%s",a)
#else
#define do_fputs(a,b) fputs(a,b)
#endif


#ifdef	DOCUMENTATION

title	archx	text file archiver extraction
index		text file archiver extraction

synopsis

	archx archive_files

description

	Archx manages archives (libraries) of source files, allowing
	a large number of small files to be stored without using
	excessive system resources.  Archx extracts all files from
	an archive.

	If no archive_name file is given, the standard input is read.
	Archive header records are echoed to the standard output.

archive file format

	Archive files are standard text files.  Each archive element is
	preceeded by a line of the format:
	.s.nf
	-h-	file.name	date	true_name
	.s.f
	Note that there is no line or byte count.  To prevent problems,
	a '-' at the beginning of a record within a user file or embedded
	archive will be "quoted" by doubling it.  The date and true filename
	fields are ignored.  On some operating systems, file.name is
	forced to lowercase.  The archive builder (archc) may prefix
	other characters by '-'.

	If the first non-blank line of an input file does not
	begin with "-h", the text will be appended to "archx.tmp"
	This is needed if archives are distributed by mail
	and arrive with initial routing and subject information.

diagnostics

	Diagnostic messages should be self-explanatory

author

	Martin Minow

bugs

#endif

#include	<stdio.h>
#include	<stdlib.h>
#include	<ctype.h>
#include	<string.h>
#ifdef vms
#include		<ssdef.h>
#include		<stsdef.h>
#define	IO_SUCCESS	(SS$_NORMAL | STS$M_INHIB_MSG)
#define	IO_ERROR	SS$_ABORT
#endif
/*
 * Note: IO_SUCCESS and IO_ERROR are defined in the Decus C stdio.h file
 */
#ifndef	IO_SUCCESS
#define	IO_SUCCESS	0
#endif
#ifndef	IO_ERROR
#define	IO_ERROR	1
#endif

#define EOS		0
#define	FALSE		0
#define	TRUE		1

/*
 * The following status codes are returned by gethdr()
 */
#define DONE	0
#define	GOTCHA	1
#define	NOGOOD	2

char		text[255];		/* Working text line		*/
char		name[81];		/* Current archive member name	*/
char		filename[81];		/* Working file name		*/
char		arfilename[81];		/* Archive file name		*/
char		fullname[81];		/* Output for argetname()	*/
int		verbose		= TRUE;	/* TRUE for verbosity		*/
int		first_archive;		/* For mail header skipping	*/




int
gethdr()
/*
 * If text is null, read a record, returning to signal input state:
 *	DONE	Eof read
 *	NOGOOD	-h- wasn't first non-blank line.  Line is in text[]
 *	GOTCHA	-h- found, parsed into name.
 */
{
	register char	*tp;
	register char	*np;

again:	if (text[0] == EOS
	 && fgets(text, sizeof text, stdin) == NULL)
	    return (DONE);
	if (text[0] == '\n' && text[1] == EOS) {
	    text[0] = EOS;
	    goto again;
	}
	if (text[0] != '-'
	 || text[1] != 'h'
	 || text[2] != '-')
	    return (NOGOOD);
	for (tp = &text[3]; isspace(*tp); tp++)
	    ;
	for (np = name; !isspace(*tp); *np++ = *tp++)
	    ;
	*np = EOS;
	return (GOTCHA);
}


arskip()
/*
 * Skip to next header
 */
{
	while (fgets(text, sizeof text, stdin) != NULL) {
	    if (text[0] == '-' && text[1] == 'h' && text[2] == '-')
		return;
	}
	text[0] = EOS;				/* EOF signal		*/
}


arexport(outfd)
register FILE	*outfd;
/*
 * Read secret archive format, writing archived data to outfd.
 * Clean out extraneous <cr>,<lf>'s
 */
{
	register char	*tp;
	unsigned int	nrecords;

	printf("Creating \"%s\", ", name);
	nrecords = 0;
	while (fgets(text, sizeof text, stdin) != NULL) {
	    tp = &text[strlen(text)];
	    if (tp > &text[1] && *--tp == '\n' && *--tp == '\r') {
		*tp++ = '\n';
		*tp = EOS;
	    }
	    if (text[0] == '-') {
		if (text[1] == 'h')
		    goto gotcha;
		do_fputs(text+1, outfd);
	    }
	    else {
			do_fputs(text, outfd);
	    }
	    nrecords++;
	}
	text[0] = EOS;
gotcha:	printf("%u records\n", nrecords);
	if (ferror(stdin) || ferror(outfd))
	    printf("Creation of \"%s\" completed with error\n", name);
}


process()
/*
 * Process archive open on stdin
 */
{
	register char		*fn;	/* File name pointer		*/
	register FILE		*outfd;
	register int		i;

	text[0] = EOS;
	while ((i = gethdr()) != DONE) {
	    switch (i) {
	    case GOTCHA:
		if ((outfd = fopen(name, "w")) == NULL) {
		    perror(name);
		    fprintf(stderr, "Can't create \"%s\"\n", name);
		    arskip();
		    continue;
		}
		break;

	    case NOGOOD:
		fprintf(stderr, "Missing -h-, writing to archx.tmp\n");
		fprintf(stderr, "Current text line: %s", text);
		strcpy(name, "archx.tmp");
		if ((outfd = fopen(name, "a")) == NULL) {
		    perror(name);
		    fprintf(stderr, "Cannot append to %s\n", name);
		    arskip();
		    continue;
		}
		break;
	    }
	    arexport(outfd);
	    fclose(outfd);
	}
}


main(argc, argv)
int		argc;			/* Arg count			*/
char		*argv[];		/* Arg vector			*/
{
	register int		i;	/* Random counter		*/
	int			status;	/* Exit status			*/

#ifdef	vms
	argc = getredirection(argc, argv);
#endif
	status = IO_SUCCESS;
	if (argc == 1)
	    process();
	else {
	    for (i = 1; i < argc; i++) {
		if (freopen(argv[i], "r", stdin) != NULL)
		    process();
		else {
		    perror(argv[i]);
		    status = IO_ERROR;
		}
	    }
	}
	exit(status);
}


/*
 * getredirection() is intended to aid in porting C programs
 * to VMS (Vax-11 C) which does not support '>' and '<'
 * I/O redirection.  With suitable modification, it may
 * useful for other portability problems as well.
 */

#ifdef	vms
static int
getredirection(argc, argv)
int		argc;
char		**argv;
/*
 * Process vms redirection arg's.  Exit if any error is seen.
 * If getredirection() processes an argument, it is erased
 * from the vector.  getredirection() returns a new argc value.
 *
 * Warning: do not try to simplify the code for vms.  The code
 * presupposes that getredirection() is called before any data is
 * read from stdin or written to stdout.
 *
 * Normal usage is as follows:
 *
 *	main(argc, argv)
 *	int		argc;
 *	char		*argv[];
 *	{
 *		argc = getredirection(argc, argv);
 *	}
 */
{
	register char		*ap;	/* Argument pointer	*/
	int			i;	/* argv[] index		*/
	int			j;	/* Output index		*/
	int			file;	/* File_descriptor 	*/

	for (j = i = 1; i < argc; i++) {   /* Do all arguments	*/
	    switch (*(ap = argv[i])) {
	    case '<':			/* <file		*/
		if (freopen(++ap, "r", stdin) == NULL) {
		    perror(ap);		/* Can't find file	*/
		    exit(IO_ERROR);	/* Is a fatal error	*/
		}

	    case '>':			/* >file or >>file	*/
		if (*++ap == '>') {	/* >>file		*/
		    /*
		     * If the file exists, and is writable by us,
		     * call freopen to append to the file (using the
		     * file's current attributes).  Otherwise, create
		     * a new file with "vanilla" attributes as if
		     * the argument was given as ">filename".
		     * access(name, 2) is TRUE if we can write on
		     * the specified file.
		     */
		    if (access(++ap, 2) == 0) {
			if (freopen(ap, "a", stdout) != NULL)
			    break;	/* Exit case statement	*/
			perror(ap);	/* Error, can't append	*/
			exit(IO_ERROR);	/* After access test	*/
		    }			/* If file accessable	*/
		}
		/*
		 * On vms, we want to create the file using "standard"
		 * record attributes.  create(...) creates the file
		 * using the caller's default protection mask and
		 * "variable length, implied carriage return"
		 * attributes. dup2() associates the file with stdout.
		 */
		if ((file = creat(ap, 0, "rat=cr", "rfm=var")) == -1
		 || dup2(file, fileno(stdout)) == -1) {
		    perror(ap);		/* Can't create file	*/
		    exit(IO_ERROR);	/* is a fatal error	*/
		}			/* If '>' creation	*/
		break;			/* Exit case test	*/

	    default:
		argv[j++] = ap;		/* Not a redirector	*/
		break;			/* Exit case test	*/
	    }
	}				/* For all arguments	*/
	return (j);
}
#endif

