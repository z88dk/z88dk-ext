
// Only SDCC creates a valid program !!
//  zcc +cpm -create-app -DAMALLOC -compiler=sdcc FGREP.c

// With optimizations:
//  zcc +cpm -create-app -DAMALLOC -compiler=sdcc -SO3 --max-allocs-per-node400000 -pragma-define:CRT_INITIALIZE_BSS=0 FGREP.c


/* 
HEADER: 	CUG
TITLE:		FGREP.C - Search File(s) for Fixed Patten(s)
VERSION:	1.03
DATE:		02/11/85
DESCRIPTION:	"A full implementation of the UNIX 'fgrep'
		utility. The algorithm used in this program
		constructs a deterministic finite state automaton
		(FSA) for pattern matching from the substrings,
		then uses the FSA to process the text string in
		one pass. The time taken to construct the FSA is
		proportional to the sum of the lengths of the the
		substrings. The number of state transitions made
		by the FSA in processing the text string is
		independent of the number of substrings."
KEYWORDS:	fgrep, grep, filter, UNIX, pattern-matching
SYSTEM:		Any
FILENAME:	FGREP.C
WARNINGS:	"The '-s' option may not be consistently
		supported by various non-Unix operating systems
		and compilers. Also, the Unix-specific '-b'
		option of 'fgrep' is not supported. Finally,
		non-Unix operating systems may not accept
		lowercase character strings on the command line,
		although these can be entered through files."
CRC:		xxxx
SEE-ALSO:	None
AUTHORS:	Ian Ashdown - byHeart Software
COMPILERS:	Any C compiler
REFERENCES:	AUTHORS: Bell Telephone Laboratories;
		TITLE:	 UNIX Programmer's Manual Vol. 1, p. 166;
		AUTHORS: A.V. Aho & M.J. Corasick;
		TITLE:	 'Efficient String Matching: An Aid to
			 Bibliographic Search'
			 Communications of the ACM
			 pp. 333 - 340, Vol. 18 No. 6 (June '75);
ENDREF
*/

/*-------------------------------------------------------------*/

/* FGREP.C - Search File(s) For Fixed Pattern(s)
 *
 * Version 1.04	      February 11th, 1985
 *
 * Modifications:
 *
 *   V1.00 (84/12/01)	- beta test release
 *   V1.01 (85/01/01)	- added -P option
 *			- improved command line validation
 *   V1.02 (85/01/06)	- modified "run_fsa()" and "bd_move()"
 *   V1.03 (85/02/11)	- added -S option
 *
 * Copyright 1985:    Ian Ashdown
 *		      byHeart Software
 *		      2 - 2016 West First Avenue
 *		      Vancouver, B.C. V6J 1G8
 *		      Canada
 *
 * This program may be copied for personal, non-commercial use
 * only, provided that the above copyright notice is included in
 * all copies of the source code. Copying for any other use
 * without previously obtaining the written permission of the
 * author is prohibited.
 *
 * pHILANTHROPICAL nOTES:
 *
 * Considerable time and effort went into the development of this
 * software, which was expressly written for the public domain.
 * The author will gladly accept any and all monetary
 * contributions for the purpose of continuing such work!
 *
 * Notes:
 *
 * The algorithm used in this program constructs a deterministic
 * finite state automaton (FSA) for pattern matching from the sub-
 * strings, then uses the FSA to process the text string in one
 * pass. The time taken to construct the FSA is proportional to
 * the sum of the lengths of the the substrings. The number of
 * state transitions made by the FSA in processing the text
 * string is independent of the number of substrings.
 *
 * Algorithm Source:
 *
 * "Efficient String Matching: An Aid to Bibliographic Search"
 * Alfred V. Aho & Margaret J. Corasick
 * Communications of the ACM
 * pp. 333 - 340, Vol. 18 No. 6 (June '75)
 *
 * USAGE: fgrep [-vclnhyefxps] [strings] <files>
 *
 * where:
 *
 *	-v	All lines but those matching are printed.
 *	-c	Only a count of the matching lines is printed.
 *	-l	The names of the files with matching lines are
 *		listed (once), separated by newlines.
 *	-n	Each line is preceded by its line number in the
 *		file.
 *	-h	Do not print filename headers with output lines.
 *	-y	All characters in the file are mapped to upper
 *		case before matching. (This is the default if the
 *		string is given in the command line under CP/M,
 *		as CP/M maps everything on the command line to
 *		upper case. Use the -f option if you need both
 *		lower and upper case.) Not a true UNIX "fgrep"
 *		option (normally available under "grep" only),
 *		but too useful to leave out.
 *	-e	<string>. Same as a string argument, but useful
 *		when the string begins with a '-'.
 *	-f	<file>. The strings (separated by newlines) are
 *		taken from a file. If several strings are listed
 *		in the file, then a match is flagged if any of
 *		the strings are matched. If -f is given, any
 *		following argument on the command line is taken
 *		to be a filename.
 *	-x	Only lines matched in their entirety are printed.
 *	-p	Each matched line is preceded by the matching
 *		substring(s). Not a UNIX "fgrep" option, but too
 *		useful to leave out.
 *	-s	No output is produced, only status. Used when
 *		when "fgrep" is run as a process that returns a
 *		status value to its parent process. Under CP/M, a
 *		non-zero value returned by "exit()" may terminate
 *		a submit file that initiated the program,
 *		although this is implementation-dependent.
 *
 * DIAGNOSTICS:
 *
 * Exit status is 0 if any matches are found, 1 if none, 2 for
 * error condition.
 *
 * BUGS:
 *
 * The following UNIX-specific option is not supported:
 *
 *	-b	Each line is preceded by the block number in
 *		which it was found
 *
 * Lines are limited to 256 characters.
 *
 */



/*** Definitions ***/

#define TRUE		   1
#define FALSE		   -1
#define MAX_LINE	 257  /* Maximum number of characters */
			      /* per line plus NULL delimiter */
#define CP_M		      /* Comment out for compilation */
			      /* under UNIX */

#define CMD_ERR	   0	/* Error codes */
#define OPT_ERR	   1
#define INP_ERR	   2
#define STR_ERR	   3
#define MEM_ERR	   4



/*** Typedefs ***/

typedef signed char BOOL;	/* Boolean flag */



/*** Data Structures ***/

/* Queue element */

typedef struct queue
{
  struct state_el *st_ptr;
  struct queue *next_el;
} QUEUE;

/* Transition element */

typedef struct transition
{
  char lchar;			/* Transition character */
  struct state_el *nextst_ptr;	/* Transition state pointer */
  struct transition *next_el;
} TRANSITION;

/* FSA state element */

typedef struct state_el
{
  TRANSITION *go_ls;	/* Pointer to head of "go" list */
  TRANSITION *mv_ls;	/* Pointer to head of "move" list */
  struct state_el *fail_state;	/* "failure" transition state */
  char *out_str;	/* Terminal state message (if any) */
} FSA;



/*** Global Variables and Structures ***/

/* Dummy "failure" state */

FSA FAIL_STATE;

/* Define a separate data structure for State 0 of the FSA to
 * speed processing of the input while the FSA is in that state.
 * Since the Aho-Corasick algorithm only defines "go" transitions
 * for this state (one for each valid input character) and no
 * "failure" transitions or output messages, only an array of
 * "go" transition state numbers is needed. The array is accessed
 * directly, using the input character as the index.
 */

FSA *MZ[128];	/* State 0 of FSA */

BOOL vflag = FALSE,	/* Command-line option flags */
     cflag = FALSE,
     lflag = FALSE,
     nflag = FALSE,
     hflag = FALSE,
     yflag = FALSE,
     eflag = FALSE,
     fflag = FALSE,
     xflag = FALSE,
     pflag = FALSE,
     sflag = FALSE;



/*** Include Files ***/

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <ctype.h>
#include <string.h>


/* ERROR() - Error reporting. Returns an exit status of 2 to the
 *	     parent process.
 */

void error(char n, char *str)
{
  fprintf(stderr,"\007\n*** ERROR - ");
  switch(n)
  {
    case CMD_ERR:
      fprintf(stderr,"Illegal command line");
      break;
    case OPT_ERR:
      fprintf(stderr,"Illegal command line option");
      break;
    case INP_ERR:
      fprintf(stderr,"Can't open input file %s",str);
      break;
    case STR_ERR:
      fprintf(stderr,"Can't open string file %s",str);
      break;
    case MEM_ERR:
      fprintf(stderr,"Out of memory");
      break;
    default:
      break;
  }
  fprintf(stderr," ***\n\nUsage: fgrep [-vclnhyefxps]");
  fprintf(stderr," [strings] <files>\n");
  exit(2);
}


/* STOUPPER() - Map entire string pointed to by "str" to upper
 *		case.
 */

#define stoupper strupr

//char *stoupper(char *str)
//{
//  register char *temp;
//
//  temp = str;
//  while(*temp)
//    *temp++ = toupper(*temp);
//  return str;
//}


/*** Functions and Procedures ***/


/* MOVE() - Process "litchar" and return a pointer to the FSA's
 *	    corresponding "move" transition state.
 */

FSA *move(FSA *st_ptr,char litchar)
{
  register TRANSITION *current;

  /* If State 0, then access separate State 0 data structure of
   * the FSA.
   */

  if(!st_ptr)
    return MZ[litchar];
  else
  {
    /* Point to the head of the linked list of "move" transitions
     * associated with the state.
     */

    current = st_ptr->mv_ls;

    /* Transverse the list looking for a match to the input
     * character.
     */

    while(current)
    {
      if(current->lchar == litchar)
	break;
      current = current->next_el;
    }

    /* Null value for "current" indicates end of list was reached
     * without having found match to input character. The
     * returned pointer is then to State 0.
     */

    return current ? current->nextst_ptr : NULL;
  }
}


/* GO() - Process "litchar" and return a pointer to the FSA's
 *	  corresponding "go" transition state. If the character
 *	  is not in the FSA state's "go" transition list, then
 *	  return a pointer to FAIL_STATE.
 */

FSA *go(FSA *st_ptr,char litchar)
{
  register TRANSITION *current;

  /* If State 0, then access separate State 0 data structure of
   * the FSA. Note that there are no failure states defined for
   * any input to FSA State 0.
   */

  if(!st_ptr)
    return MZ[litchar];
  else
  {
    /* Point to the head of the linked list of "go" transitions
     * associated with the state.
     */

    current = st_ptr->go_ls;

    /* Transverse the list looking for a match to the input
     * character.
     */

    while(current)
    {
      if(current->lchar == litchar)
	break;
      current = current->next_el;
    }

    /* Null value for "current" indicates end of list was reached
     * without having found match to input character.
     */

    return current ? current->nextst_ptr : &FAIL_STATE;
  }
}


/* RUN_FSA() - Run the finite state automaton with string "str"
 *	       as input. Return TRUE if match, FALSE otherwise.
 */

BOOL run_fsa(char *str)
{
  register FSA *st_ptr;
  char *message = NULL;
  BOOL msg_flag = FALSE;

  st_ptr = NULL;	/* Initialize FSA */
  if(xflag == FALSE)
  {
    /* Process the next input character in the string */

    while(*str)
    {
      st_ptr = move(st_ptr,*str);

      /* Print terminal state message and update FSA */

      if(st_ptr == 0 && message)
      {
	printf("--> %s\n",message);
	message = NULL;
	st_ptr = move(st_ptr,*str);
      }
      str++;
      if(st_ptr)
	if(st_ptr->out_str)	/* Terminal state? */
	  if(pflag == TRUE)
	  {
	    /* Save terminal state message */

	    message = st_ptr->out_str;
	    msg_flag = TRUE;
	  }
	  else
	    return TRUE;
    }
    if(message)  /* Print any remaining terminal state message */
      printf("--> %s\n",message);
    return msg_flag;
  }
  else	/* Match exact lines only */
  {
    while(*str)
    {
      st_ptr = go(st_ptr,*str++);
      if(!st_ptr || st_ptr == &FAIL_STATE)
	return FALSE;	/* Line not matched */
    }
    return TRUE;	/* Exact line matched */
  }
}


/* PROC_FILE() - Run the FSA on the input file "in_file". Returns
 *		 TRUE if a match was found, FALSE otherwise.
 */

BOOL proc_file(char *in_file,BOOL prt_flag)
{
  char buffer[MAX_LINE],	/* Input string buffer */
       *nl;
  long line_cnt = 0L,	/* Line counter */
       mtch_cnt = 0L;	/* Matched line counter */
  BOOL mtch_flag;	/* Matched line flag */
  FILE *in_fd;

  if(in_file != NULL)	/* A file was specified as the input */
  {
    if(!(in_fd = fopen(in_file,"r")))
      error(INP_ERR,in_file);
  }
  else
    in_fd = stdin;

  /* Read in a line at a time for processing */

  while(fgets(buffer,MAX_LINE,in_fd))
  {
    if(nl = index(buffer,'\n'))  /* Remove newline */
      *nl = '\0';
#ifdef CP_M
    if(fflag == FALSE || yflag == TRUE)
      stoupper(buffer);
#else
    if(yflag == TRUE)
      stoupper(buffer);
#endif
    line_cnt++;		/* Increment the line counter */
    if((mtch_flag = run_fsa(buffer)) == TRUE)
      mtch_cnt++;	/* Increment matched line counter */
    if(cflag == FALSE && lflag == FALSE && sflag == FALSE &&
	((mtch_flag == TRUE && vflag == FALSE) ||
	(mtch_flag == FALSE && vflag == TRUE)))
    {
      if(hflag == FALSE && prt_flag == TRUE)
	printf("%s: ",in_file);
      if(nflag == TRUE)
	printf("%05ld: ",line_cnt);
      puts(buffer);
    }
  }
  if(lflag == TRUE && mtch_cnt > 0)
    printf("%s\n",in_file);
  else if(cflag == TRUE && sflag == FALSE)
    printf("%ld\n",mtch_cnt);
  if(in_file != NULL)
    fclose(in_fd);
  if(mtch_cnt)		/* Match found */
    return TRUE;
  else			/* No match found */
    return FALSE;
}



/* STRSAVE() - Save a string somewhere in memory */

char *strsave(char *str)
{
  char *p;

  if(p = malloc(strlen(str) + 1))
    strcpy(p,str);
  else
    error(MEM_ERR,NULL);
  return p;
}


/* INSERT() - Create a new "go" transition and return a pointer
 *	      to it.
 */

TRANSITION *insert(FSA *st_ptr,char litchar)
{
  TRANSITION *current;
    
  if(!(current = (TRANSITION *)malloc(sizeof(TRANSITION))))
    error(MEM_ERR,NULL);
  current->lchar = litchar;
  current->nextst_ptr = st_ptr;
  current->next_el = NULL;
  return current;
}


/* CREATE() - Create an FSA state and return a pointer to it. */

FSA *create()
{
  FSA *st_ptr;

  if(!(st_ptr = (FSA *)malloc(sizeof(FSA))))
    error(MEM_ERR,NULL);
  st_ptr->go_ls = st_ptr->mv_ls = NULL;
  st_ptr->fail_state = NULL;
  st_ptr->out_str = NULL;
  return st_ptr;
}


/* ENTER() - Enter a string into the FSA by running each
 *	     character in turn through the current partially-
 *	     built FSA. When a failure occurs, add the remainder
 *	     of the string to the FSA as one new state per
 *	     character. (Note that '\0' can never be a valid
 *	     character - C uses it to terminate a string.)
 */

void enter(char *str)
{
  FSA *s;
  TRANSITION *current;
  register char *temp;
  register FSA *st_ptr = NULL;	/* Start in FSA State 0 */
  register FSA *nextst_ptr;

  /* Run each character in turn through partially-built FSA until
   * a failure occurs.
   */  

  temp = str;
  while((s = go(st_ptr,*temp)) != &FAIL_STATE)
  {
    temp++;
    st_ptr = s;
  }

  /* Process the remainder of the string */

  while(*temp)
  {
    /* If a new state, then create a new state and insert
     * transition character and "go" transition in current
     * state. (Note special case of FSA State 0.)
     */

    if(!st_ptr)
      nextst_ptr = MZ[*temp++] = create();
    else if(!(current = st_ptr->go_ls))
    {
      nextst_ptr = create();
      st_ptr->go_ls = insert(nextst_ptr,*temp++);
    }
    else
    {
      /* ... or it was the character that the FSA returned a
       * "failure" for. Find the tail of the current state's list
       * of "go" transitions, create a new state and append it
       * to the current state's "go" list.
       */

      while(current->next_el)
	current = current->next_el;
      nextst_ptr = create();
      current->next_el = insert(nextst_ptr,*temp++);
    }
    st_ptr = nextst_ptr;
  }

  /* Make string terminal state's output message */

  st_ptr->out_str = strsave(str);
}


/* BD_GO() - Build the "go" transitions for each state from the
 *	     command-line arguments.
 */ 

void bd_go(char *str)
{
  register char litchar;
  char *nl,
       buffer[MAX_LINE];	/* Input string buffer */
  FILE *str_fd;

  /* Initialize FSA State 0 "go" transition array so that every
   * invocation of "go()" with "state" = 0 initially returns a
   * pointer to FAIL_STATE.
   */

  for(litchar = 1; litchar <= 127; litchar++)
    MZ[litchar] = &FAIL_STATE;

  /* If the -f option was selected, get the newline-separated
   * strings from the file "str" one at a time and enter them
   * into the FSA. Otherwise, enter the string "str" into the
   * FSA.
   */

  if(fflag == TRUE)
  {
    if(!(str_fd = fopen(str,"r")))
      error(STR_ERR,str);

    while(fgets(buffer,MAX_LINE,str_fd))
    {
      if(nl = index(buffer,'\n'))	/* Remove the newline */
	*nl = '\0';
      if(yflag == TRUE)
	stoupper(buffer);
      enter(buffer);
    }
    fclose(str_fd);
  }
  else
  {
    if(yflag == TRUE)
      stoupper(buffer);
    enter(str);
  }

  /* For every input character that does not lead to a defined
   * "go" transition from FSA State 0, set the corresponding
   * element in the State 0 "go" transition array to indicate
   * a "go" transition to State 0.
   */

  for(litchar = 1; litchar <= 127; litchar++)
    if(MZ[litchar] == &FAIL_STATE)
      MZ[litchar] = NULL;
}



/* DELETE_QUEUE() - Delete an instance from the head of queue */

void delete_queue(QUEUE **head_ptr)
{
  *head_ptr = (*head_ptr)->next_el;
}



/* ADD_QUEUE() - Add an instance to the tail of a queue */

void add_queue(QUEUE **head_ptr,QUEUE **tail_ptr,FSA *st_ptr)
{
  QUEUE *pq;

  /* Allocate the necessary memory and set the variables. */

  if(!(pq = (QUEUE *)malloc(sizeof(QUEUE))))
    error(MEM_ERR,NULL);

  pq->st_ptr = st_ptr;
  pq->next_el = NULL;

  if(!*head_ptr)	/* First instance of the queue? */
    *tail_ptr = *head_ptr = pq;
  else			/* No, just another one ... */
    *tail_ptr = (*tail_ptr)->next_el = pq;
}



/* BD_MOVE() - Build the "failure" and "move" transitions for
 *	       each state from the "go" transitions.
 */

void bd_move()
{
  register char litchar;
  register FSA *r,	/* Temporary FSA state pointers */
	       *s,
	       *t;
  TRANSITION *current;
  QUEUE *first,		/* Pointer to head of queue */
	*last;		/* Pointer to tail of queue */

  last = first = NULL;	/* Initialize the queue of FSA states */

  /* For each input character with a "go" transition out of FSA
   * State 0, add a pointer to the "go" state to the queue. Note
   * that this will also serve as the "move" transition list for
   * State 0.
   */

  for(litchar = 1; litchar <= 127; litchar++)
    if(s = go(NULL,litchar))
      add_queue(&first,&last,s);

  /* While there are still state pointers in the queue, do ... */

  while(first)
  {
    /* Remove State "r" pointer from the head of the queue. */

    r = first->st_ptr;
    delete_queue(&first);

    /* Skip (terminal) state with no "go" transitions" */

    if(!r->go_ls)
      continue;

    /* Make "move" transition list for terminal state same as its
     * "go" transition list.
     */

    if(r->out_str)
      r->mv_ls = r->go_ls;

    /* For every input to State "r" that has a "go" transition to
     * State "s", do ...
     */

    for(litchar = 1; litchar <= 127; litchar++)
    {
      if((s = go(r,litchar)) != &FAIL_STATE)
      {
	/* If a defined "go" transition exists for State "r" on
	 * on input "litchar", add a pointer to State "s" to the
	 * end of the queue.
	 */

	add_queue(&first,&last,s);

	/* Calculate the "failure" transition of State "s" using
	 * the following algorithm.
	 */

	t = r->fail_state;
	while(go(t,litchar) == &FAIL_STATE)
	  t = t->fail_state;
	s->fail_state = go(t,litchar);
      }
      else
      {
	/* ... otherwise set the pointer to State "s" to a
	 * pointer to the precalculated "move" transition of
	 * State "r"'s failure state on input "litchar".
	 */

	s = move(r->fail_state,litchar);
      }

      /* Add State "s" as the "move" transition for State "r" on
       * input "litchar" only if it is not State 0 and "r" is not
       * a terminal state.
       */

      if(s && !r->out_str)
	if(!r->mv_ls)	/* First instance of the list? */
	  current = r->mv_ls = insert(s,litchar);
	else		/* No, just another one ... */
	  current = current->next_el = insert(s,litchar);
    }
  }
}


/*** Main Body of Program ***/

void main(int argc,char **argv)
{
  char *temp;
  BOOL match_flag = FALSE;

  /* Check for minimum number of command line arguments */

  if(argc < 2)
    error(CMD_ERR,NULL);

  /* Parse the command line for user-selected options */

  while(--argc && (*++argv)[0] == '-' && eflag == FALSE)
    for(temp = argv[0]+1; *temp != '\0'; temp++)    
      switch(toupper(*temp))
      {
	case 'V':
	  vflag = TRUE;
	  break;
	case 'C':
	  cflag = TRUE;
	  break;
	case 'L':
	  lflag = TRUE;
	  break;
	case 'N':
	  nflag = TRUE;
	  break;
	case 'H':
	  hflag = TRUE;
	  break;
	case 'Y':
	  yflag = TRUE;
	  break;
	case 'E':
	  eflag = TRUE;
	  break;
	case 'F':
	  fflag = TRUE;
	  break;
	case 'X':
	  xflag = TRUE;
	  break;
	case 'P':
	  pflag = TRUE;
	  break;
	case 'S':
	  sflag = TRUE;
	  break;
	default:
	  error(OPT_ERR,NULL);
      }

  /* "pflag" can only be TRUE if the following flags are FALSE */

  if(vflag == TRUE || cflag == TRUE || lflag == TRUE ||
      xflag == TRUE || sflag == TRUE)
    pflag = FALSE;

  /* Check for string (or string file) argument */

  if(!argc)
    error(CMD_ERR,NULL);

  /* Build the "go" transitions. */

  bd_go(*argv++);
  argc--;

  /* Build the "failure" and "move" transitions */

  bd_move();

  /* Process each of the input files if not "stdin". */

  if(argc < 2)
    hflag = TRUE;
  if(!argc)
  {
    if(proc_file(NULL,FALSE) == TRUE && match_flag == FALSE)
      match_flag = TRUE;
  }
  else
    while(argc--)
      if(proc_file(*argv++,TRUE) == TRUE && match_flag == FALSE)
	match_flag = TRUE;

  /* Return status to the parent process. Status is zero if any
   * matches are found, 1 if none. */

  if(match_flag == TRUE)
    exit(0);
  else
    exit(1);
}


/*** End of FGREP.C ***/

