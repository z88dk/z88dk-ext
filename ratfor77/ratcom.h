int bp;			/*   next available char; init = 0 */
S_CHAR buf[BUFSIZE];	/*   pushed-back chars */
S_CHAR fcname[MAXNAME];	/*   text of current function name */
int fordep;		/*   current depth of for statements */
S_CHAR forstk[MAXFORSTK];	/*   stack of reinit strings */
int swtop;		/*   current switch entry; init=0              */
int swlast;		/*   next available position; init=1           */
int swstak[MAXSWITCH];	/*   switch information stack                  */
int xfer;		/*   YES if just made transfer, NO otherwise */
int label;		/*   next label returned by labgen */
int level ;		/*   level of file inclusion; init = 1 */
int linect[NFILES];	/*   line count on input file[level]; init = 1 */
FILE *infile[NFILES];	/*   file number[level]; init infile[1] = STDIN */
int fnamp;		/*   next free slot in fnames; init = 2 */
S_CHAR fnames[MAXFNAMES];	/*   stack of include names; init fnames[1] = EOS */
int avail;		/*   first first location in table; init = 1 */
int tabptr[127];	/*   name pointers; init = 0 */
int r_outp;		/*   last position filled in outbuf; init = 0 */
S_CHAR outbuf[82];	/*   output lines collected here */
S_CHAR fname[MAXNAME][NFILES];	/*   file names */
int nfiles;		/*   number of files */
