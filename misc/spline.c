
// zcc +cpm -create-app -lm -O3 spline.c

/* 
HEADER: 	CUG
TITLE:		SPLINE.C - Interpolate Smooth Curve;
VERSION:	3.00;
DATE:		09/26/86;
DESCRIPTION:	"SPLINE takes pairs of numbers from the standard
		input as abscissae and ordinates of a function.
		It produces a similar set, which is approximately
		equally spaced and includes the input set, on the
		standard output. The cubic spline output has two
		continuous derivatives and sufficiently many
		points to look smooth when plotted.";
KEYWORDS:	spline, graphics, plot, filter, UNIX;
SYSTEM:		ANY;
FILENAME:	SPLINE.DOC;
WARNINGS:	NONE;
CRC:		xxxx
SEE-ALSO:	SPLINE.C;
AUTHORS:	Ian Ashdown - byHeart Software;
COMPILERS:	ANY;
REFERENCES:	AUTHORS: Bell Laboratories;
		TITLE:	 UNIX Programmer's Manual, Volume 1
			 p. 145, Holt, Rinehart and Winston;
		AUTHORS: R.W. Hamming;
		TITLE:	 Numerical Methods for Scientists and
			 Engineers, 2nd Edition
			 pp. 349 - 355, McGraw-Hill (1973);
		AUTHORS: Forsythe, Malcolm & Moler;
		TITLE:	 Computer Methods for Mathematical
			 Computations
			 pp. 70 - 83, Prentice-Hall;
ENDREF
*/

/*-------------------------------------------------------------*/

/* SPLINE.C - Interpolate Smooth Curve
 *
 * Version 3.00		September 26th, 1986
 *
 * Modifications:
 *
 *   V1.00 (85/11/01)	- beta test release
 *   V2.00 (85/12/25)	- general revision
 *   V2.01 (86/09/13)	- corrected command-line option bug for
 *			  Microsoft C compiler
 *   V3.00 (86/09/26)	- added non-uniform abscissa spacing
 *			  capability 
 *
 * Copyright 1985,1986:	Ian Ashdown
 *			byHeart Software
 *			620 Ballantree Road
 *			West Vancouver, B.C.
 *			Canada V7S 1W3
 *
 * This program may be copied for personal, non-commercial use
 * only, provided that the above copyright notice is included in
 * all copies of the source code. Copying for any other use
 * without previously obtaining the written permission of the
 * author is prohibited.
 *
 * Synopsis:	SPLINE [option] ...
 *
 * Description:	SPLINE takes pairs of numbers from the standard
 *		input as abscissae and ordinates of a function.
 *		(A minimum of four pairs is required.) It
 *		produces a similar set, which is approximately
 *		equally spaced and includes the input set, on the
 *		standard output. The cubic spline output (R.W.
 *		Hamming, "Numerical Methods for Scientists and
 *		Engineers", 2nd ed. 349ff) has two continuous
 *		derivatives and sufficiently many points to look
 *		smooth when plotted.
 *
 *		The following options are recognized, each as a
 *		separate argument:
 *
 *		-a  Supply abscissae automatically (they are
 *		    missing from the input); spacing is given by
 *		    the next argument or is assumed to be 1 if
 *		    next argument is not a number.
 *
 *		-k  The constant "k" is used in the boundary
 *		    value computation
 *
 *			y " = ky " , y " = ky "
 *			 0	1     n	     n-1
 *
 *		    is set by the next argument. By default,
 *		    k = 0. A value of k = 0.5 often results in a
 *		    smoother curve at the endpoints than the
 *		    default value. Negative values for k are not
 *		    allowed. Cannot be used with -p option.
 *
 *		-n  Next argument (which must be an integer)
 *		    specifies the number of intervals that are to
 *		    occur between the lower and upper "x" limits.
 *		    If -n option is not given, default spacing is
 *		    100 intervals.
 *
 *		-p  Make output periodic, i.e. match derivatives
 *		    at ends. First and last input values must
 *		    agree. Cannot be used with -k option.
 *
 *		-x  Next 1 (or 2) arguments are lower (and upper)
 *		    "x" limits. Normally these limits are
 *		    calculated from the data. Automatic abscissae
 *		    start at lower limit (default 0). If either
 *		    argument is outside of the range of
 *		    abscissae, it is ignored.
 *
 * Diagnostics:	When data is not strictly monotone in "x", SPLINE
 *		reproduces the input without interpolating extra
 *		points.
 *
 * Bugs:	A limit of 1000 input points is silently
 *		enforced.
 *
 *		The -n option has not been implemented in
 *		accordance with the "UNIX Programmer's Manual"
 *		specification. This was done to avoid ambiguities
 *		when the -n option follows the -x option with one
 *		argument.
 *
 *		At certain negative values for the -k option (for
 *		example, k equals -4.0), the curve becomes
 *		discontinuous. The -k option value has thus been
 *		arbitrarily constrained to be greater than or
 *		equal to zero.
 *
 * Credits:	The above description is a reworded and expanded
 *		version of that appearing in the "UNIX Programmer's
 *		Manual", copyright 1979, 1983 Bell Laboratories.
 */

/*** Definitions ***/

#define FALSE	     0
#define TRUE	     1
#define MAX_SIZE  1000	/* Input point array limit */

#define ILL_ARG	     0	/* Error codes */
#define ILL_CMB      1
#define ILL_KVL	     2
#define ILL_NVL	     3
#define ILL_OPT      4
#define ILL_XVL	     5
#define INS_INP      6
#define MIS_KVL      7
#define MIS_NVL      8
#define MIS_XVL      9
#define MIS_YVL	    10
#define NMT_ORD	    11

#define SQUARE(a) a*a
#define CUBE(a) a*a*a

/*** Typedefs ***/

typedef int BOOL;	/* Boolean flag */

/*** Include Files ***/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>


/*** Functions ***/


/* ERROR() - Error reporting. Returns an exit status of 2 to the
 *	     parent process.
 */

void error(n,str)
int n;
char *str;
{
  fprintf(stderr,"\007\n*** ERROR - ");
  switch(n)
  {
    case ILL_ARG:
      fprintf(stderr,"Illegal argument: %s",str);
      break;
    case ILL_OPT:
      fprintf(stderr,"Illegal command line option: %s",str);
      break;
    case INS_INP:
      fprintf(stderr,"Insufficient input data");
      break;
    default:
      fprintf(stderr,"Exiting with error code %u - %s",n,str);
      break;
  }
  //fprintf(stderr," ***\n\nUsage: spline ...\n");
  exit(2);
}


/* SPL_COEFF() - Calculate spline coefficients and return in
 *		 vector "d2y[]". Matrix to be solved has the
 *		 typical form:
 *
 * +-					     -++-       -+   +-      -+
 * | c[1]  h[1]  0     ....    0       0      || y"[1]   | = | d[1]   |
 * | h[1]  c[2]  h[2]  ....    0       0      || y"[2]   |   | d[2]   |
 * | 0     h[2]  c[3]  ....    0       0      || y"[3]   |   | d[3]   |
 * | ....  ....  ....  ....    ....    ....   || .....   |   | ....   |
 * | 0     0     ....  h[n-4]  c[n-3]  h[n-3] || y"[n-3] |   | d[n-3] |
 * | 0     0     ....  0       h[n-3]  c[n-2] || y"[n-2] |   | d[n-2] |
 * +-					     -++-       -+   +-      -+
 *
 * where c[1] = (2 + k_val) * h[0] + 2 * h[1]
 *       c[i] = 2 * (h[i-1] + h[i]), i = 2,...,n-3
 *       c[n-2] = 2 * h[n-3] + (2 + k_val) * h[n-2]
 *
 *		    +-				   -+
 *	            | y[i+1] - y[i]   y[i] - y[i-1] |
 *       d[i] = 6 * | ------------- - ------------- | , i = 1,...,n-2
 *		    |     h[i]	         h[i-1]	    |
 *		    +-				   -+
 */

void spl_coeff(y,d2y,h,n,k_val)
float y[],
      h[];
double d2y[],
       k_val;
int n;
{
  double c[MAX_SIZE-1];
  int i;

  /* Set up the (symmetric tridiagonal) matrix. Array "d2y[]"
   * initially holds the constants vector, then is overlaid with
   * the calculated variables vector.
   */

  c[1] = (2.0 + k_val) * h[0] + 2.0 * h[1];
  c[n-2] = 2.0 * h[n-3] + (2.0 + k_val) * h[n-2];
  for(i = 2; i < n-2; i++)
    c[i] = 2.0 * (h[i-1] + h[i]);

  for(i = 1; i < n-1; i++)
    d2y[i] = 6.0 * ((y[i+1] - y[i])/h[i] - (y[i] - y[i-1])/h[i-1]);

  /* Reduce the matrix to upper triangular form */ 

  for(i = 1; i < n-2; i++)
  {
    c[i+1] -= h[i] * h[i]/c[i];
    d2y[i+1] -= d2y[i] * h[i]/c[i];
  }

  /* Solve using back substitution */

  d2y[n-2] /= c[n-2];
  for(i = n-3; i > 0; i--)
    d2y[i] = (d2y[i] - h[i] * d2y[i+1])/c[i];

  /* Solve for end conditions */

  d2y[0] = d2y[1] * k_val;
  d2y[n-1] = d2y[n-2] * k_val;
}

/* PSPL_COEFF() - Calculate periodic spline coefficients and
 *		  return in vector "d2y[]". Matrix to be solved
 *		  has the typical form:
 *
 * +-					     -++-       -+   +-      -+
 * | c[1]  h[1]  0     ....    0       h[0]   || y"[1]   | = | d[1]   |
 * | h[1]  c[2]  h[2]  ....    0       0      || y"[2]   |   | d[2]   |
 * | 0     h[2]  c[3]  ....    0       0      || y"[3]   |   | d[3]   |
 * | ....  ....  ....  ....    ....    ....   || .....   |   | ....   |
 * | 0     0     ....  h[n-3]  c[n-2]  h[n-2] || y"[n-2] |   | d[n-2] |
 * | h[0]  0     ....  0       h[n-2]  c[n-1] || y"[n-1] |   | d[n-1] |
 * +-					     -++-       -+   +-      -+
 *
 * where c[i] = 2 * (h[i-1] + h[i]), i = 1,...,n-2
 *       c[n-1] = 2 * (h[0] + h[n-2])
 *
 *		    +-				   -+
 *		    | y[i+1] - y[i]   y[i] - y[i-1] |
 *       d[i] = 6 * | ------------- - ------------- | , i = 1,...,n-2
 *		    |     h[i]	         h[i-1]	    |
 *		    +-				   -+
 *
 *		      +-			     -+
 *		      | y[1] - y[0]   y[n-1] - y[n-2] |
 *       d[n-1] = 6 * | ----------- - --------------- |
 *		      |    h[0]	           h[n-2]     |
 *		      +-			     -+
 *
 */

void pspl_coeff(y,d2y,h,n)
float y[],
      h[];
double d2y[];
int n;
{
  double a,
	 b,
	 f;
	 //fabs();
  static double c[MAX_SIZE-1],
		e[MAX_SIZE-1];
  int i;

  /* Check for matching end ordinates */

  if(fabs(y[n-1] - y[0]) > 0.0)
    error(NMT_ORD,NULL);

  /* Set up the matrix. Array "d2y[]" initially holds the constants
   * vector, then is overlaid with the calculated variables vector.
   */

  for(i = 1; i < n-1; i++)
  {
    c[i] = 2.0 * (h[i-1] + h[i]);
    d2y[i] = 6.0 * ((y[i+1] - y[i])/h[i] - (y[i] - y[i-1])/h[i-1]);
  }
  c[n-1] = 2.0 * (h[0] + h[n-2]);
  d2y[n-1] = 6.0 * ((y[1] - y[0])/h[0] - (y[n-1] - y[n-2])/h[n-2]);

  /* Initialize array e[] as nth column of matrix M[][]. */

  e[1] = h[0];
  for(i = 2; i < n-2; i++)
    e[i] = 0.0;
  e[n-2] = h[n-2];
  e[n-1] = c[n-1];

  /* Initialize variable f as matrix element M[n][1] */

  f = h[0];

  /* Reduce matrix to upper triangular form */

  for(i = 1; i < n-2; i++)
  {
    a = h[i]/c[i];
    b = f/c[i];
    c[i+1] -= h[i] * a;
    d2y[i+1] -= d2y[i] * a;
    e[i+1] -= e[i] * a;
    d2y[n-1] -= d2y[i] * b;
    e[n-1] -= e[i] * b;
    f = -f * a;		/* Now matrix element M[n-1][i] */
  }
  f = f + h[n-2];	/* Now matrix element M[n-1][n-2] */
  d2y[n-1] -= d2y[n-2] * f/c[n-2];
  e[n-1] -= e[n-2] * f/c[n-2];

/* Solve using back substitution */

  d2y[n-1] /= e[n-1];
  d2y[n-2] = (d2y[n-2] - e[n-2] * d2y[n-1])/c[n-2];
  for(i = n-3; i > 0; i--)
    d2y[i] = (d2y[i] - h[i] * d2y[i+1] - e[i] * d2y[n-1])/c[i];

  d2y[0] = d2y[n-1]; /* End condition */
}

/* SPL_INT - Interpolate points using spline function */

double spl_int(ix,x,y,d2y,h,i)
float x[],
      y[],
      h[];
double ix,
       d2y[];
int i;
{
  double iy,
	 t1,
	 t2;

  t1 = (x[i+1] - ix)/h[i];
  t2 = (ix - x[i])/h[i];
  iy = y[i] * t1 + y[i+1] * t2 - SQUARE(h[i]) * (d2y[i] * (t1 -
      CUBE(t1)) + d2y[i+1] * (t2 - CUBE(t2)))/6.0;
  return iy;
}

/* IS_FLOAT() - Check that character string is in correct floating
 *		point format. Return TRUE if correct, FALSE
 *		otherwise. The algorithm used is a deterministic
 *		finite state machine. Using the regular
 *		expression terminology of Unix's "lex", the
 *		character string must be of the form:
 *
 *			-?d*.?d*(e|E(\+|-)?d+)?
 *
 *		where d = 0|1|2|3|4|5|6|7|8|9
 */

BOOL is_float(str)
char *str;
{
  int c,		/* Next FSM input character */
      state = 0;	/* Current FSM state */

  while(c = *str++)
  {
    switch(state)
    {
      case 0:		/* FSM State 0 */
	switch(c)
	{
	  case '-':	
	    state = 1;
	    break;
	  case '.':
	    state = 3;
	    break;
	  default:
	    if(isdigit(c))
	      state = 2;
	    else
	      return FALSE;
	    break;
	}
	break;
      case 1:		/* FSM State 1 */
	switch(c)
	{
	  case '.':
	    state = 2;
	    break;
	  default:
	    if(isdigit(c))
	      state = 2;
	    else
	      return FALSE;
	    break;
	}
	break;
      case 2:		/* FSM State 2 */
	switch(c)
	{
	  case '.':
	    state = 4;
	    break;
	  case 'e':
	  case 'E':
	    state = 5;
	    break;
	  default:
	    if(isdigit(c))
	      state = 2;
	    else
	      return FALSE;
	    break;
	}
	break;
      case 3:		/* FSM State 3 */
	if(isdigit(c))
	  state = 4;
	else
	  return FALSE;
	break;
      case 4:		/* FSM State 4 */
	switch(c)
	{
	  case 'e':
	  case 'E':
	    state = 5;
	    break;
	  default:
	    if(isdigit(c))
	      state = 4;
	    else
	      return FALSE;
	    break;
	}
	break;
      case 5:		/* FSM State 5 */
	switch(c)
	{
	  case '+':
	  case '-':
	    state = 6;
	    break;
	  default:
	    if(isdigit(c))
	      state = 7;
	    else
	      return FALSE;
	    break;
	}
	break;
      case 6:		/* FSM State 6 */
	if(isdigit(c))
	  state = 7;
	else
	  return FALSE;
	break;
      case 7:		/* FSM State 7 */
	if(isdigit(c))
	  state = 7;
	else
	  return FALSE;
	break;
    }
  }
  return TRUE;
}


/*** Main Body of Program ***/

int main(argc,argv)
int argc;
char **argv;
{
  int n = 0,
      i,
      j,
      n_val = 0;
      //atoi();
  float	x[MAX_SIZE],
	y[MAX_SIZE],
	h[MAX_SIZE-1];
  double a_val = 1.0,
	 k_val = 0.0,
	 x1_val = 0.0,
	 x2_val = 0.0,
	 x_intvl,
	 ix,
	 iy,
	 d2y[MAX_SIZE];
	 //atof(),
	 //spl_int();
  char buffer[257],
       *temp;
       //*gets();
  BOOL aflag = FALSE,	/* Command-line option flags */
       kflag = FALSE,
       pflag = FALSE,
       x1flag = FALSE,
       x2flag = FALSE;
       //is_float();
//  void spl_coeff(),
//       pspl_coeff(),
//       error();

  /* Parse the command line for user-selected options */

  while(--argc)
  {
    temp = *++argv;
    if(*temp != '-')	/* Check for legal option flag */
      error(ILL_OPT,*argv);
    else
    {
      temp++;
      switch(toupper(*temp))
      {
	case 'A':	/* "-a" option */
	  aflag = TRUE;
	  if(argc > 1 && is_float(*(argv+1)))
	  {
	    argc--;
	    argv++;
 	    if((a_val = atof(*argv)) <= 0.00)
	      error(ILL_ARG,*argv);
	  }
	  break;
	case 'K':	/* "-k" option */
	  if(pflag == TRUE)
	    error(ILL_CMB,NULL);
	  kflag = TRUE;
	  if(argc > 1 && is_float(*(argv+1)))
	  {
	    argc--;
	    argv++;
	    k_val = atof(*argv);
	    if(k_val < 0.00)
	      error(ILL_KVL,*argv);
	    break;
	  }
	  else
	    error(MIS_KVL,NULL);
	case 'N':	/* "-n" option */
	  if(argc > 1)
	  {
	    argc--;
	    argv++;
	    if((n_val = atoi(*argv)) < 1)
	      error(ILL_NVL,*argv);
	    else
	      break;
	  }
	  else
	    error(MIS_NVL,NULL);
	case 'P':	/* "-p" option */
	  if(kflag == TRUE)
	    error(ILL_CMB,NULL);
	  pflag = TRUE;
	  break;
	case 'X':	/* "-x" option */
	  x1flag = TRUE;
	  if(argc > 1 && is_float(*(argv+1)))
	  {
	    argc--;
	    argv++;
	    x1_val = atof(*argv);
	  }
	  else
	    error(MIS_XVL,NULL);
	  if(argc > 1 && is_float(*(argv+1)))
	  {
	    x2flag = TRUE;
	    argc--;
	    argv++;
	    x2_val = atof(*argv);
	    if(x2_val <= x1_val)
	      error(ILL_XVL,x2_val);
	  }
	  break;
	default:	/* "-n" option */
	  error(ILL_OPT,*argv);
      }
    }
  }
  if(n_val == 0)	/* Set "n_val" if not given */
    n_val = 100;

  /* Get the input data */

  while(1)		/* ... while there is more input data */
  {
    if(aflag == TRUE)	/* Automatic abscissae were called for */
    {
      if(n == 0)
	x[0] = x1_val;
      else
	x[n] = x[n-1] + a_val;
    }
    else		/* Abscissae supplied with input data */
    {
      if(gets(buffer))
	x[n] = atof(buffer);
      else
	break;
    }
    if(gets(buffer))	/* Read in the corresponding ordinate */
      y[n] = atof(buffer);
    else
    {
      if(aflag == TRUE)
	break;
      else
	error(MIS_YVL,NULL);
    }
    if(++n == MAX_SIZE)	/* Maximum amount of input data? */
      break;
  }
  if(n < 4)		/* Check for insufficient input data */
    error(INS_INP,NULL);

  /* Check for non-monotonic abscissae. Output input data set
   * without interpolation if true.
   */

  for(i = 0; i < n-1; i++)
    if((h[i] = (x[i+1] - x[i])) <= 0.0)
    {
      for(i = 0; i < n; i++)
	printf("%g\n%g\n",x[i],y[i]);
      exit(1);
    }

  /* Calculate abscissa interval. Use "-x" option values if
   * they were given unless they fall outside the range of
   * given (or calculated) abscissae.
   */ 

  if(x1flag == FALSE || x1_val < x[0])
    x1_val = x[0];
  if(x2flag == FALSE || x2_val > x[n-1])
    x2_val = x[n-1];
  x_intvl = (x2_val - x1_val)/n_val;

  /* Find the coefficients */

  if(pflag == FALSE)
    spl_coeff(y,d2y,h,n,k_val);
  else
    pspl_coeff(y,d2y,h,n);

  /* Interpolate and output results */

  ix = x1_val;
  i = 0;
  for(j = 0; j <= n_val; j++)
  {
    while(ix >= x[i+1] && i < n - 2)
      i++;
    iy = spl_int(ix,x,y,d2y,h,i);
    printf("%g\n%g\n",ix,iy);
    ix += x_intvl;
  }
}

/*** End of SPLINE.C ***/
