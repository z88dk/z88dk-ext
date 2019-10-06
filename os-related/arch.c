/*
 *
 *			A R C H I V E
 *
 * PDP-11/VAX Archiver, roughly from software tools.
 *
 *
 *
 *
 * Create an archive in ancient PDP11 format
 *
 * TODO: wildcalrd handling on z88dk
 *
 * Build example:
 * zcc +cpm -create-app -O3 -Dunix -DAMALLOC -oarch arch.c
 *
 * CP/M emulators note:
 *    only text files created by CP/M programs will be processed correctly!
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

title	arch	text file archiver
index		text file archiver

synopsis
	arch [-options] [-z logfile] archive_name file[s]

description

	Arch manages archives (libraries) of source files, allowing
	a large number of small files to be stored without using
	excessive system resources.  The following options may
	be specified:
	.lm +8
	.s.i -8;c	Force creation of new archive
	.s.i -8;d	Delete file from archive.
	.s.i -8;i	Insert, same as update
	.s.i -8;p	Print files on standard output
	.s.i -8;r	Replace, same as update
	.s.i -8;l	List archive contents (directory)
	.s.i -8;u	Update, same as replace
	.s.i -8;x	Extract named files
	.s.i -8;v	Verbose
	.s.i -8;z	Write verbose log to indicated file
	.s.lm -8
	The file name arguments may contain '*' and '?' wildcards, where
	'*' matches any string of characters, and '?' matches one character.
	('%' may be used as a synonym for '?'.)  There is a slight, but
	suble difference in the way wild cards are processed for the
	various commands:
	.lm +8
	.s.i -8;directory, delete, and extract
	.s
	Match ('*' and '?') against the files in the
	archive, performing the operation on all files that
	match.  Except for delete, "no argument" matches
	all files in the archive.
	.s.i -8;insert, replace, and update
	.s
	Expand the wild-card arguments against the files
	stored on the operating system -- eliminating all
	wild cards.  Then, match against the archived
	files.  Those that match are replaced by the
	file on the operating system.  After replacement,
	any additional files are appended to the archive.
	Files in the archive that are not in the directory
	are unchanged.
	.s
	Currently, insert, replace, and update work the same.
	If it seems reasonable, the program may be extended
	as follows:
	.lm +8
	.s.i -8;insert	Add files new only
	.s
	Adds new files (not present in the archive)
	but does not modify files currently in
	the archive.  It would be an error to try
	modifying a currently archived file.
	.s.i -8;replace	Modify existing files only
	.s
	Modify files present in the archive, but do
	not add new files to the archive.
	.s.i -8;update	Modify existing, add new
	.s.lm -8
	This is simple to do, but would seem to be a rich
	source of user error.
	.lm -8

note

	Arch has been superseded for most uses by the much simpler,
	but functionally similar, archc and archx programs.

archive file format

	Archive files are standard text files.  Each archive element is
	preceeded by a line of the format:
	.s.nf
	-h-	file.name	date	true_name
	.s.f
	Note that there is no line or byte count.  To prevent problems,
	a '-' at the beginning of a record within a user file or embedded
	archive will be "quoted" by doubling it.  The date and true filename
	fields are ignored.  On Dec operating systems, file.name is
	forced to lowercase.

diagnostics

	Diagnostic messages should be self-explanatory

author

	Martin Minow

bugs

	Arch used to be called ar.  The name was changed to avoid
	conflict with the Unix tool.

#endif

#include	<time.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<ctype.h>
#include	<malloc.h>
#include	<string.h>


#define EOS		0
#define	FALSE		0
#define	TRUE		1
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

/*
 * These two routines are faked on native Unix
 */
extern	FILE		*fwild();	/* Wild card file lookup	*/
extern	FILE		*fnext();	/* Open next wild card file	*/
#ifdef	unix
#define	FAKEFWILD
//#define	strrchr	rindex			/* Wrong on 4.2bsd		*/
#endif

/* z88dk */
#ifdef Z80
#define unlink(a) remove(a)
#endif

#ifdef	tolower
#undef	tolower
#endif

#define	TEMPNAME	"ar.tmp"

typedef struct filename {
	char namepart[10];
	char typepart[4];
} FILENAME;

/*
 * List chains together strings of text.
 */

typedef struct List {
	struct List	*l_next;	/* -> next list element		*/
	int		l_flag;		/* mark if found in archive	*/
	char		*l_arname;	/* archive name argument	*/
	char		*l_filename;	/* directory file name		*/
} LIST;

/*
 * Global storage
 */
FILE		*arfd		= NULL;	/* Archive			*/
FILE		*newfd		= NULL;	/* New archive			*/
FILE		*logfd;			/* Log output			*/
int		newarchive	= FALSE; /* True if create from scratch	*/
int		logging		= FALSE; /* True if log file enabled	*/
char		text[513];		/* Working text			*/
char		arname[81];		/* Current archive member name	*/
char		filename[81];		/* Working file name		*/
char		arfilename[81];		/* Archive file name		*/
char		fullname[81];		/* Output for argetname()	*/
char		*timetext;		/* Time of day text		*/
int		verbose		= FALSE; /* TRUE for verbosity		*/
int		delflag		= 0;	/* Delete files			*/
int		directory	= 0;	/* Table of contents if lit	*/
int		update		= 0;	/* Update files if lit		*/
int		extract		= 0;	/* Get files from archive	*/
int		print		= 0;	/* Write files to stdout	*/
int		errorflag	= 0;	/* Set on fatal error		*/
LIST		*list		= NULL;	/* String list header		*/

arcopy(infd, outfd)
register FILE	*infd;
register FILE	*outfd;
/*
 * Copy (or skip if outfd == NULL) to next header
 */
{
	while (fgets(text, sizeof text, infd) != NULL) {
		if (text[0] == '-' && text[1] != '-')
			return;
		if (outfd != NULL)
			do_fputs(text, outfd);
	}
	text[0] = EOS;				/* EOF signal		*/
}

fatal(message)
char		*message;
/*
 * Fatal error
 */
{
	fprintf(stderr, message);
	fprintf(stderr, "\n");
	exit(IO_ERROR);
}

cant(fname, why)
char		*fname;
char		*why;
/*
 * Can't open a file, fatal.
 */
{
	fprintf(stderr, "Can't %s: \"%s\"\n", why, fname);
	fatal("Can't continue");
}

notfound()
/*
 * Called from dodelete() to warn the user about files that were
 * to be deleted, but which were not in the archive.
 */
{
	register LIST	*lp;

	for (lp = list; lp != NULL; lp = lp->l_next) {
		if (!lp->l_flag) {
			fprintf(stderr, "Can't delete \"%s\" -- not found\n",
				lp->l_arname);
			if (logging) {
				fprintf(logfd,
				"Can't delete \"%s\" -- not found\n",
				lp->l_arname);
			}
		}
	}
}

int
gethdr(fd)
FILE		*fd;
/*
 * If text is null, read a record, returning TRUE if text contains a header.
 * Parse the header into arname.
 */
{
	register char	*tp;
	register char	*np;

	if (text[0] == EOS && fgets(text, sizeof text, fd) == NULL) {
		return (FALSE);
	}
	if (text[0] != '-'
	 || text[1] != 'h'
	 || text[2] != '-')
		return (FALSE);
	for (tp = &text[3]; *tp && *tp <= ' '; tp++);
	for (np = &arname[0]; *tp > ' '; *np++ = *tp++);
	*np = EOS;
	return	(TRUE);
}

int
breakout(arg, buff)
char		*arg;
FILENAME	*buff;
/*
 * Parse arg ("foo.bar") into "foo" and "bar"
 * Return TRUE if trouble.
 */
{
	register char	*ap;
	register char	*bp;
	register int	dotseen;
	int		size;

	dotseen = FALSE;
	ap = arg;
	bp = buff->namepart;
	buff->typepart[0] = EOS;
	size = (sizeof buff->namepart) - 1;
	while (*ap != EOS) {
		if (*ap == '.') {
			if (dotseen++)			/* 2 dots	*/
				return (TRUE);
			else {
				ap++;
				*bp = EOS;
				bp = buff->typepart;
				size = (sizeof buff->typepart) -1;
				continue;
			}
		}
		if (size-- <= 0)			/* 2 big	*/
			return (TRUE);
		*bp++ = *ap++;
	}
	*bp = EOS;
	return (FALSE);
}

int
match1(name, pattern)
register char	*name;		/* What to look for			*/
register char	*pattern;	/* May have wildcard			*/
/*
 * Recursive routine to match "name" against "pattern".
 * Returns TRUE if successful.
 */
{
	register char	pattbyte;
	char		namebyte;

	for (;;) {
		/*
		 * First check for pattern ending in '*' -- this has to succeed
		 */
		if ((pattbyte = *pattern++) == '*' && *pattern == EOS)
			return (TRUE);
		/*
	 	 * If not, then if both strings finish equally, it succeeds.
	 	 */
		if ((namebyte = *name) == EOS && pattbyte == EOS)
			return (TRUE);
		/*
		 * Not at end of the name string.
		 */
		switch (pattbyte) {
		case EOS:		/* End of pattern -> failure	*/
			return (FALSE);
	
		case '*':		/* Wild card means "advance"	*/
			do {
				if (match1(name, pattern))
					return (TRUE);
			} while (*name++ != EOS);
			return (FALSE);	/* Did our best			*/

		default:
			if (tolower(namebyte) != tolower(pattbyte))
				return (FALSE);
		case '?':		/* One byte joker		*/
		case '%':		/* RT11 one byte joker		*/
			name++;		/* Matched this one		*/
		}
	}
}

int
match(name, pattern)
register char	*name;
register char	*pattern;
/*
 * Pattern match between
 *	name	string argument (FILE.EXT format)
 *	pattern	which may contain wildcards.
 *
 * Note: '*' matches all but '.' separator between file and ext.
 *	'?' matches one character, but not '.'
 *
 */
{
	register int	i;
	FILENAME	namebuff;
	FILENAME	patternbuff;
	
	if (breakout(name, &namebuff) || breakout(pattern, &patternbuff))
		return (FALSE);
	return (match1(namebuff.namepart, patternbuff.namepart)
	   &&	match1(namebuff.typepart, patternbuff.typepart));
}	

int
findarg(name, fname)
char		*name;
char		*fname;			/* Gets full file name		*/
/*
 * If name is in the list, mark it as "found" and return TRUE.
 * If true, and fname is not NULL, fname will have the file argument.
 */
{
	register LIST	*lp;
	register int	flag;

	if ((lp = list) == NULL) {
		if (fname != NULL)
			fname[0] = EOS;
		return (TRUE);
	}
	while (lp != NULL) {
		if (match(name, lp->l_arname)) {
			lp->l_flag = TRUE;
			if (fname != NULL) {
				if (lp->l_filename == NULL)
					fname[0] = EOS;
				else {
					strcpy(fname, lp->l_filename);
				}
			}
			return (TRUE);
		}
		lp = lp->l_next;
	}
	return (FALSE);
}

dodirectory()
/*
 * Write a table of contents
 */
{
	text[0] = EOS;
	while (gethdr(arfd)) {
		if (findarg(arname, NULL)) {
			printf(text);
		}
		arcopy(arfd, NULL);		/* Skip file		*/
	}
}

filemove(inname, outname)
char		*inname;
char		*outname;
/*
 * "Rename" inname to outname the hard way.
 */
{
	register FILE		*infd;
	register FILE		*outfd;
	long int		nrecords;

	if (verbose)
		fprintf(logfd, "Copying %s to %s\n", inname, outname);
	if ((infd = fopen(inname, "r")) == NULL)
		cant(inname, "open for input");
	if ((outfd = fopen(outname, "w")) == NULL)
		cant(outname, "create");
	for (nrecords = 0; fgets(text, sizeof text, infd) != NULL; nrecords++)
		do_fputs(text, outfd);
#ifdef	unix
	fclose(infd);
	fclose(outfd);
	unlink(inname);
#else
	fgetname(infd, text);
	fclose(infd);
	fclose(outfd);
	delete(text);
#endif
	if (verbose)
		fprintf(logfd, "Archive %s contains %ld records.\n",
			outname, nrecords);
}

arexport(infd, outfd)
register FILE	*infd;
register FILE	*outfd;
/*
 * Read secret archive format, writing archived data to outfd.
 * Clean out extraneous <cr>,<lf>'s
 */
{
	register char	*tp;

	while (fgets(text, sizeof text, infd) != NULL) {
		tp = &text[strlen(text)];
		if (tp > &text[1] && *--tp == '\n' && *--tp == '\r') {
			*tp++ = '\n';
			*tp = EOS;
		}
		if (text[0] == '-') {
			if (text[1] != '-')
				return;
			do_fputs(text+1, outfd);
		}
		else {
			do_fputs(text, outfd);
		}
	}
	text[0] = EOS;
}

doextract(printflag)
int		printflag;	/* TRUE to print, FALSE to extract	*/
/*
 * Extract or print named files
 */
{
	register FILE		*outfd;

	outfd = (printflag) ? stdout : NULL;
	text[0] = EOS;
	while (gethdr(arfd)) {
		if (!findarg(arname, NULL)) {
			if (verbose) {
				fprintf(logfd, "Skipping \"%s\"\n", arname);
			}
			arcopy(arfd, NULL);		/* Skip		*/
		}
		else {
			if (outfd != stdout) {
				if ((outfd = fopen(arname, "w")) == NULL) {
					//perror(arname);
					fprintf(stderr,
						"Can't create \"%s\"\n",
						arname);
					if (logging)
						fprintf(logfd,
						"Can't create \"%s\"\n",
						arname);
					arcopy(arfd, NULL);
					continue;
				}
			}
			if (verbose) {
				fprintf(logfd, "Creating \"%s\"\n", arname);
			}
			arexport(arfd, outfd);
			fclose(outfd);
			outfd = NULL;
		}
	}
}

arimport(infd, outfd, fname)
register FILE	*infd;
register FILE	*outfd;
char		*fname;			/* Input file name		*/
/*
 * Import text, writing it in the secret ar format.
 */
{
	unsigned int	nrecords;

	nrecords = 0;
	while (fgets(text, sizeof text, infd) != NULL) {
		if (text[0] == '-') {
			putc('-', outfd);		/* Quote	*/
		}
		do_fputs(text, outfd);
		nrecords++;
	}
	if (ferror(infd)) {
		//perror(fname);
		fprintf(stderr, "Error when importing a file\n");
	}
	if (verbose) {
		fprintf(stderr,
			"%u records read from %s\n", nrecords, fname);
	}
}

int
addfile(name, fname, outfd, ecount, why)
char		*name;		/* Archive element name			*/
char		*fname;		/* Archive file name			*/
FILE		*outfd;		/* Output file, already open		*/
int		ecount;		/* Current error count (updated)	*/
char		*why;		/* Why are we here -- for verbosity	*/
/*
 * Add file "fname" (archive element "name") to the archive
 */
{
	register FILE		*infd;

	if ((infd = fopen(fname, "r")) == NULL) {
		//perror(fname);
		fprintf(stderr, "%s archive member \"%s\" not found\n",
				why,
				(fname == NULL) ? "{Null}" : fname);
		if (logging) {
			fprintf(logfd,
				"%s archive member \"%s\" not found\n",
				why,
				(fname == NULL) ? "{Null}" : fname);
		}
		ecount++;
	}
	else {
#ifdef	unix
		strcpy(filename, fname);
#else
		fgetname(infd, filename);
#endif
		if (verbose) {
			fprintf(logfd, "%s archive member \"%s\" (%s)\n",
				why, name, filename);
		}
		fprintf(outfd, "-h- %s\t%s\t%s\n", name, timetext, filename);
		arimport(infd, outfd, filename);
		fclose(infd);
	}
	return (ecount);
}

int
replace(infd, outfd, updateflag, ecount)
FILE		*infd;		/* Reading files from here		*/
FILE		*outfd;		/* Writing files here			*/
int		updateflag;	/* TRUE to update, FALSE to remove	*/
int		ecount;
/*
 * Replace or delete files from the archive.  The updated archive
 * is written to outfd.
 */
{
	text[0] = EOS;			/* Signal gethdr initialization	*/
	while (gethdr(infd)) {
		/*
		 * We have a file, is it selected?
		 */
		if (findarg(arname, filename)) {
			if (updateflag) {
				ecount += addfile(arname, filename,
						outfd, ecount, "Replaced");
			}
			arcopy(infd, NULL);
		}
		else {
			/*
			 * Not selected for update, copy to the new archive
			 */
			do_fputs(text, outfd);
			arcopy(infd, outfd);
		}
	}
	return (ecount);
}

dodelete()
/*
 * Delete named files -- gotta have a name list
 */
{
	register int		ecount;

	if (list == NULL) {
		fatal("Delete by name only");
	}
	ecount = replace(arfd, newfd, FALSE, 0);
	notfound();
	fclose(arfd);
	fclose(newfd);
	if (ecount == 0) {
		filemove(TEMPNAME, arfilename);
	}
	else {
		fprintf(stderr, "Errors prevent deletion of archive\n");
		if (logging)
			fprintf(logfd, "Errors prevent deletion of archive\n");
#ifdef	unix
		unlink(TEMPNAME);
#else
		delete(TEMPNAME);
#endif
	}
}

int
compare(string1, string2)
register char	*string1;
register char	*string2;
/*
 * Compare strings (note: case insensitive)
 */
{
	while (tolower(*string1) == tolower(*string2)) {
		if (*string1 == NULL)
			return (0);
		string1++;
		string2++;
	}
	return ((tolower(*string1) > tolower(*string2)) ? 1 : -1);
}

int
savestring(datum, file)
char		*datum;		/* Archive element name			*/
char		*file;		/* May be NULL if not necessary		*/
/*
 * Insert text into the list in sorted order (on datum).
 * Warn (and fail on) duplicates.
 */
{
	register	LIST	*next;
	register	LIST	**prev;
	register	LIST	*new;
	char			*ardatum;
	char			*arfile;
	int			comp;

	arfile = NULL;
	if (file != NULL) {
		if ((arfile = (char *)malloc(strlen(file) + 1)) == NULL)
			fatal("Out of memory in savestring");
		strcpy(arfile, file);
	}
	if ((ardatum = (char *)malloc(strlen(datum) + 1)) == NULL
	 || (new     = (LIST *)malloc(sizeof(LIST)     )) == NULL)
		fatal("Out of memory in savestring");
	strcpy(ardatum, datum);
	new->l_flag = FALSE;
	new->l_arname = ardatum;
	new->l_filename = arfile;
	prev = &list;
	next = list;
	while (next != NULL && (comp = compare(datum, next->l_arname)) > 0) {
		if (comp == 0) {
			fprintf(stderr, "duplicate argument \"%s\"\n", datum);
			if (arfile) {
				free(arfile);
			}
			free(ardatum);
			free(new);
			return (TRUE);
		}
		prev = &next->l_next;
		next = *prev;
	}
	*prev = new;
	new->l_next = next;
	return (FALSE);
}	

argetname(fd, wildname, outname, outfilename)
FILE		*fd;
char		*wildname;	/* Arg to fwild (for Unix)		*/
char		*outname;	/* Archive name				*/
char		*outfilename;	/* Full file name			*/
/*
 * Get file name, stripping off device:[directory] and ;version.
 * The archive name ("FILE.EXT" is written to outname, while the
 * full file name is written to outfilename.  On a dec operating system,
 * outname is forced to lowercase.
 */
{
	register char	*tp;
	register char	*ip;
	char		bracket;
//	extern char	*strrchr();

#ifdef	unix
	strcpy(outname, wildname);
	/*
	 * outname is after all directory information
	 */
	if ((tp = strrchr(outname, '/')) != NULL)
		strcpy(outname, tp + 1);
	strcpy(outfilename, outname);
#else
#ifdef	decus
	wildname = wildname;			/* Prevent warning msg	*/
#endif
	fgetname(fd, outfilename);
	strcpy(outname, outfilename);
	if ((tp = strrchr(outname, ';')) != NULL)
		*tp = EOS;
	while ((tp = strchr(outname, ':')) != NULL)
		strcpy(outname, tp + 1);
	switch (outname[0]) {
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
		if ((tp = strchr(outname, bracket)) == NULL) {
			fprintf(stderr, "? Illegal file name \"%s\"\n",
				outfilename);
		}
		else {
			strcpy(outname, tp + 1);
		}
	}
	for (tp = outname; *tp != EOS; tp++)
		*tp = tolower(*tp);
#endif
}

int
findfiles(fname)
char		*fname;
/*
 * Archive element names, do fwild lookup to expand wildcards where possible.
 */
{
	register int	i;
	register FILE	*fd;

	if ((fd = fwild(fname, "r")) == NULL) {
		fprintf(stderr,
			"Can't open directory or wildcard file \"%s\"\n",
			fname);
		if (logging){
			fprintf(logfd,
			"Can't open directory or wildcard file \"%s\"\n",
			fname);
		}
		return (1);
	}
	/*
	 * Locate each file, then save archive and file names
	 */
	for (i = 0; fnext(fd) != NULL; i++) {
		argetname(fd, fname, arname, filename);
		savestring(arname, filename);
	}
	if (i == 0) {
		fprintf(stderr, "Warning, no match for \"%s\"\n",
			fname);
		if (logging) {
			fprintf(logfd, "Warning, no match for \"%s\"\n",
				fname);
		}
		return (1);
	}
	else if (verbose) {
		fprintf(logfd, "%d file%s in your directory match%s \"%s\"\n",
			i,
			(i > 1)  ? "s"  : "",
			(i == 1) ? "es" : "",
			fname);
		return (0);
	}
}

int
expandargs(argc, argv, updateflag)
int		argc;		/* Number of arguments			*/
char		*argv[];	/* Arg vector				*/
int		updateflag;	/* TRUE to trigger file search		*/
/*
 * Process the argv[] vector, building the argument list.
 * Note: argv[1] is the first argument -- argv[0] is untouched and
 * NULL entries in argv[] are ignored.
 *
 * If updateflag is TRUE, arguments are expanded against the file
 * directory (using fwild/fnext).  If FALSE, they are used as is.
 *
 * Return TRUE if errors occurred.
 */
{
	register int	in;
	register int	eflag;

	eflag = 0;
	for (in = 1; in < argc; in++) {
		if (argv[in] != NULL) {
			if (updateflag) {
				eflag += findfiles(argv[in]);
			}
			else {
				eflag += savestring(argv[in], NULL);
			}
		}
	}
	return (eflag != 0);
}

dumplist()
/*
 * Dump archive name list -- used for debugging only
 */
{
	register LIST	*lp;

	if ((lp = list) == NULL)
		fprintf(stderr, "List is empty\n");
	else while (lp != NULL) {
		fprintf(stderr, "%s, \"%s\"",
			(lp->l_flag) ? "    found" : "not found",
			lp->l_arname);
		if (lp->l_filename == NULL)
			fprintf(stderr, "\n");
		else
			fprintf(stderr, "%s\n", lp->l_filename);
		lp = lp->l_next;
	}
}

usage()
/*
 * Fatal help message
 */
{
	fatal("Usage \"ar -dilpruvx archive files\"\n\
\td\tDelete named files\n\
\ti\tInsert named files\n\
\tl\tList archive directory\n\
\tp\tPrint named files on standard output\n\
\tr\tReplace named files\n\
\tu\tUpdate -- replace named files\n\
\tv\tVerbose -- give running commentary\n\
\tx\tExtract -- copy named files to current directory\n\
i, r, and u, are identical.\n");
}

#ifdef	FAKEFWILD

static int	fake_flag = 0;		/* Set if a file is open	*/
					/*  0	nothing open		*/
					/* +1	open, fnext not called	*/
					/* +2	fnext called once	*/

FILE *
fwild(fname, mode)
char		*fname;
char		*mode;
/*
 * "setup" to open a wildcard file name
 */
{
	register FILE	*fd;

	if (fake_flag != 0) {
		fprintf(stderr, "fwild/fnext out of sync.");
	}
	fake_flag = 0;
	if ((fd = fopen(fname, mode)) != NULL)
		fake_flag++;
	return (fd);
}

FILE *
fnext(fd)
FILE		*fd;
{
	switch (fake_flag) {
	case 1:
		fake_flag++;		/* First call after fwild	*/
		return (fd);		/* File is "open"		*/
	case 2:
		fake_flag = 0;		/* Second call of fnext		*/
		fclose(fd);		/* Close existing file		*/
		return (NULL);		/* No more files left		*/
	default:
		fprintf(stderr, "fnext called without calling fwild\n");
		return (NULL);
	}
}
#endif

/*
 * Convert to lowercase -- Macro doesn't work on Unix
 */
 /*
int
tolower(c)
register int	c;
{
	return ((isupper(c)) ? c + ('a' - 'A') : c);
}*/

doupdate()
/*
 * Update existing files, add argv[1]..argv[argc-1] at end
 */
{
	register int	ecount;
	register LIST	*lp;

	ecount = 0;
	if (!newarchive) {
		ecount = replace(arfd, newfd, TRUE, 0);
	}
	for (lp = list; lp != NULL; lp = lp->l_next) {
		if (!lp->l_flag) {
			ecount += addfile(lp->l_arname, lp->l_filename,
					newfd, ecount, "Added");
			lp->l_flag = TRUE;
		}
	}
	if (newarchive) {
		fclose(newfd);
		if (ecount) {
			fprintf(stderr, "completed with %d errors\n", ecount);
			if (logging) {
				fprintf(logfd, "completed with %d errors\n",
					ecount);
			}
		}
	}
	else {
		fclose(arfd);
		fclose(newfd);
		if (ecount == 0) {
			filemove(TEMPNAME, arfilename);
		}
		else {
			fprintf(stderr,
				"Move of %s to %s supressed because of errors\n",
				TEMPNAME, arfilename);
			if (logging)
				fprintf(logfd,
				"Move of %s to %s supressed because of errors\n",
				TEMPNAME, arfilename);
		}
	}
}

main(argc, argv)
int		argc;			/* Arg count			*/
char		*argv[];		/* Arg vector			*/
{
	register int		i;	/* Random counter		*/
	register char		*fn;	/* File name pointer		*/
	register char		*argp;	/* Arg pointer			*/
	//char			*ctime();
	//long			time();
	long			timval;

#ifdef vms
	argc = getredirection(argc,argv);
#endif

	/*
	 * Setup the time of day, erasing trailing '\n'
	 */
	time(&timval);
	timetext = ctime(&timval);
	logfd = stderr;
	argp = timetext + strlen(timetext);
	while (argp > timetext && *--argp <= ' ')
		;
	argp[1] = EOS;

	for (i = 1; i < argc; i++) {
		if ((argp = argv[i]) == NULL)
			continue;	/* From log file writer		*/
		if (*argp == '-') {
			/*
			 * Process options
			 */
			argv[i] = NULL;	/* Erase it from file stuff	*/
			while (*++argp != EOS) {
				switch (tolower(*argp)) {
				case 'c':
					newarchive = TRUE;
					break;

				case 'd':	/* Delete from archive	*/
					delflag = 1;
					break;

				case 'p':	/* Print on stdout	*/
					print = 1;
					break;

				case 'l':	/* List directory	*/
					directory = 1;
					break;

				case 'i':	/* Insert		*/
				case 'r':	/* Replace		*/
				case 'u':	/* Update modified	*/
					update = 1;
					break;

				case 'v':	/* Verbose		*/
					verbose = 1;
					break;

				case 'x':
						/* Extract		*/
					extract = 1;
					break;

				case 'z':	/* Log file		*/
					if ((logfd = fopen(argv[i+1], "w"))
							== NULL)
						cant(argv[i+1], "create log");
					if (verbose) {
						fprintf(stderr,
							"writing log to %s\n",
							argv[i+1]);
					}
					logging = TRUE;
					argv[i+1] = NULL;
					break;

				default:
					fprintf(stderr,
						"Illegal option '%c'\n",
						*argp);
					usage();
				}		/* Switch		*/
			}			/* While		*/
			argv[i] = NULL;		/* Erase argument	*/
		}				/* If "-foo"		*/
		else if (arfd == NULL && newfd == NULL) {
			/*
			 * First file is the archive name
			 */
			if (newarchive || (arfd = fopen(argp, "r")) == NULL) {
				if ((newfd = fopen(argp, "w")) == NULL) {
					cant(argp, "create new archive file");
				}
				else {
					newarchive = TRUE;
					if (verbose)
						fprintf(logfd,
					  "Creating new archive \"%s\"\n",
						argp);
				}
			}
			argv[i] = NULL;		/* Erase argument	*/
			strcpy(arfilename, argp);
		}
	}
	if (errorflag)
		fatal("Previous error prevents continuation");
	if (!newarchive && arfd == NULL)
		fatal("No archive file specified");
	/*
	 * Got all arguments.
	 */
	if (!newarchive && (newfd = fopen(TEMPNAME, "w")) == NULL)
		cant("ar.tmp", "create archive work file");
	if ((i = delflag+directory+extract+print+update) > 1)
		fatal("Illogical option combination");
	else if (i == 0) {
		if (verbose)
			fprintf(logfd, "Update selected by default\n");
		update = 1;
	}
	/*
	 * Debugging verbosity.
	 */
	if (verbose) {
		fprintf(logfd, "You have selected:");
		if (directory)
			fprintf(logfd, " directory");
		if (delflag)
			fprintf(logfd, " delete");
		if (extract)
			fprintf(logfd, " extract");
		if (print)
			fprintf(logfd, " print");
		if (update)
			fprintf(logfd, " update");
		if (verbose)
			fprintf(logfd, " and verbosity");
		fprintf(logfd, ".\nArchive file is \"%s\".\n", arfilename);
	}
	if (expandargs(argc, argv, update)) {
		fprintf(stderr, "Warning, Errors found in arg. expansion\n");
	}
	if (newarchive && !update) {
		fprintf(logfd, "Dummy archive created\n");
		fclose(newfd);
	}
	else if (directory)
		dodirectory();
	else if (delflag)
		dodelete();
	else if (extract || print)
		doextract(print);
	else if (update)
		doupdate();
	else {
		fprintf(stderr, "No command was provided");
		usage();
	}
}

