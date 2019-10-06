/*
 *			A R C H I V E
 *
 * Create an archive in ancient PDP11 format
 * (archc.c is simpler, arch.c can both CREATE and UPDATE the archives)
 *
 * (wildcalrd handling on z88dk requires improvements)
 * zcc +cpm -create-app -O3 -Dunix archc.c
 *
 * This program requires the file redirection, be careful with simple CP/M emulators at command line,
 *   e.g. use double quotes:  C:\CPM\CPM.EXE "ARCH.COM FILE1 FILE2 >ARCHIVE"
 *        (stderr still must go to the console)
 */


/* Temporary workaround for the z88dk bug on fputs */
#ifdef Z88DK_BEFORE_OCT2019
#define do_fputs(a,b)	fprintf(b,"%s",a)
#else
#define do_fputs(a,b) fputs(a,b)
#endif


/*)BUILD	$(TKBOPTIONS) = {
			TASK	= ...ARC
		}
*/

#ifdef	DOCUMENTATION

title	archc	text file archive creation
index		text file archive creation

synopsis

	archc file[s] >archive

description

	Archc manages archives (libraries) of source files, allowing
	a large number of small files to be stored without using
	excessive system resources.  It copies the set of named
	files to standard output in archive format.

	The archx program will recreate the files from an archive.

	Note: there are no checks against the same file appearing
	twice in an archive.

archive file format

	Archive files are standard text files.  Each archive element is
	preceeded by a line of the format:
	.s.nf
	-h-	file.name	date	true_path_name
	.s.f
	Note that there is no line or byte count.  To prevent problems,
	a '-' at the beginning of a record within a user file or embedded
	archive will be "quoted" by doubling it.  The date and true filename
	fields are ignored.  On Dec operating systems, file.name is
	forced to lowercase.  Certain bytes at the beginning of a record are
	also prefixed by '-' to prevent mailers from treating them
	as commands.

diagnostics

	Diagnostic messages should be self-explanatory

author

	Martin Minow

#endif


#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<ctype.h>

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

char		text[513];		/* Working text			*/
char		name[81];		/* Current archive member name	*/
char		pathname[81];		/* Output for argetname()	*/
char		*timetext;		/* Time of day text		*/
int		verbose		= TRUE; /* TRUE for verbosity		*/
FILE		*infd;			/* Input file			*/


#ifdef	old_unix
	extern char	*strrchr();
#endif


fixname()
/*
 * Get file name (in pathname), stripping off device:[directory]
 * and ;version.  The archive name ("file.ext") is written to name[].
 * On a dec operating system, name is forced to lowercase.
 */
{
	char	*tp;
	char	*ip;
	char		bracket;

#ifdef	unix
	/*
	 * name is after all directory information
	 */
	if ((tp = strrchr(pathname, '/')) != NULL)
	    tp++;
	else
	    tp = pathname;
	strcpy(name, tp);
#else
	strcpy(name, pathname);
	if ((tp = strrchr(name, ';')) != NULL)
		*tp = EOS;
	while ((tp = strchr(name, ':')) != NULL)
		strcpy(name, tp + 1);
	switch (name[0]) {
	case '[':	bracket = ']';
			break;
	case '<':	bracket = '>';
			break;
	case '(':	bracket = ')';
			break;
	default:	bracket = EOS;
			break;
	}
	if (bracket != EOS) {
	    if ((tp = strchr(name, bracket)) == NULL) {
		fprintf(stderr, "? Illegal file name \"%s\"\n",
		    pathname);
	    }
	    else {
		strcpy(name, tp + 1);
	    }
	}
	for (tp = name; *tp != EOS; tp++) {
	    if (isupper(*tp))
		*tp = tolower(*tp);
	}
#endif
}

import()
/*
 * Add the file open on infd (with file name in pathname) to
 * the archive.
 */
{
	unsigned int	nrecords;

	fixname();
	nrecords = 0;
	printf("-h- %s\t%s\t%s\n", name, timetext, pathname);
	while (fgets(text, sizeof text, infd) != NULL) {
	    switch (text[0]) {
	    case '-':
	    case '.':
	    case '~':
		putchar('-');				/* Quote	*/
	    }
	    do_fputs(text, stdout);
	    nrecords++;
	}
	if (ferror(infd)) {
	    perror(name);
	    fprintf(stderr, "Error when importing a file\n");
	}
	if (verbose) {
	    fprintf(stderr, "%u records read from %s\n",
		nrecords, pathname);
	}
}





main(argc, argv)
int		argc;			/* Arg count			*/
char		*argv[];		/* Arg vector			*/
{
	register int		i;	/* Random counter		*/
	register char		*fn;	/* File name pointer		*/
	register char		*argp;	/* Arg pointer			*/
	int			nfiles;
	extern char		*ctime();
	extern long		time();
	long			timval;

	time(&timval);
	timetext = ctime(&timval);
	timetext[24] = EOS;
#ifdef vms
	argc = getredirection(argc, argv);
#endif
	if (argc <= 1)
	    fprintf(stderr, "No files to archive?\n");
#ifdef	unix
	for (i = 1; i < argc; i++) {
	    if ((infd = fopen(argv[i], "r")) == NULL)
		perror(argv[i]);
	    else {
		strcpy(pathname, argv[i]);
		import();
		fclose(infd);
	    }
	}
#else
	/*
	 * Decus C supports fwild/fnext for explicit processing
	 * of wild-card filenames.
	 */
	for (i = 1; i < argc; i++) {
	    if ((infd = fwild(argv[i], "r")) == NULL)
		perror(argv[i]);
	    else {
		for (nfiles = 0; fnext(infd) != NULL; nfiles++) {
		    fgetname(infd, pathname);
		    import();
		}
		fclose(infd);
		if (nfiles == 0)
		    fprintf(stderr, "No files match \"%s\"\n", argv[i]);
	    }
	}
#endif
}

#ifdef	old_unix
char *
strrchr(stng, chr)
register char	*stng;
register char	chr;
/*
 * Return rightmost instance of chr in stng.
 * This has the wrong name on some Unix systems.
 */
{
	register char	*result;

	result = NULL;

	do {
	    if (*stng == chr)
		result = stng;
	} while (*stng++ != EOS);
	return (result);
}
#endif

/*
 * getredirection() is intended to aid in porting C programs
 * to VMS (Vax-11 C) which does not support '>' and '<'
 * I/O redirection.  With suitable modification, it may
 * useful for other portability problems as well.
 */

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
#ifdef	vms
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
#else
	/*
	 * Note: argv[] is referenced to fool the Decus C
	 * syntax analyser, supressing an unneeded warning
	 * message.
	 */
	return (argv[0], argc);		/* Just return as seen	*/
#endif
}


