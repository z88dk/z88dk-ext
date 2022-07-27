

/********************************************************/
/*                                                      */
/*      copyright(c) 1981 By Mike Bernson               */
/*                                                      */
/*      submit command for cpm 2.x                      */
/*      read input file and convert to $$$.sub with     */
/*      $1-$9 to pram from comand line and convert      */
/*      ^a to control character                         */
/*                                                      */
/********************************************************/


// This program was originally written for a customized SMALL C variant, later known as MESCC
// Such compiler had its own esoteric way to access the CP/M files
// At the time the compiler was in turn built with BDS C v1.4
// (which had its original way to deal with files, yet different !)

// Adapted to z88dk by Stefano Bodrato in 2022

// The original program was probably bigger but had the advantage to write
// the final SUB file only after having completed it in memory.
// This version deletes the intermediate file if an error occurs.
// I tested the results on MAME, (NCR DecisionMate V)
// and it seems to work as expected

// zcc +cpm -create-app submit.c


#include <stdio.h>
#include <fcntl.h>
#include <string.h>


// exit() must force a warm boot to permit the SUBMIT trick to work
#define exit() asm("rst\t0\n");


#define ERROR   -1              /* error return value */
#define LF      0x0a            /* value used for line feed */
#define CR      0x0d            /* value used for return */
#define ENDFILE 0x1a            /* value used for end of file */

//char infcb[36];                       /* pointer to input fcb */
//char outfcb[36];                      /* output fcb */
FILE *infcb;            /* input file */
FILE *outfcb;           /* output file */
long fsize;

char *prt;                      /* just a spare pointer */
//int  inpoff;                    /* input buffer offset */
int  outoff;                    /* output buffer offset */

int  line;                      /* current line number */

//char inbuff[128];               /* input buffer */
//char outbuff[17000];            /* output buffer */

//char *outbuff;



/********************************************************/
/*                                                      */
/*      getbuff                                         */
/*                                                      */
/*      function:       to getn next character from     */
/*                      from input buffer and convert   */
/*                      to upper case                   */
/*                                                      */
/*      date written:   Dec 16, 1980 by Mike Bernson    */
/*                                                      */
/********************************************************/
/*
int getbuff()
{
        if (inpoff == 128) {
                //if (read(infcb,inbuff,1) == ERROR) return ENDFILE;
                fseek(infcb, 0, SEEK_END);
                fsize = ftell(infcb);
                fseek(infcb, 0, SEEK_SET);
                if (fread(inbuff, fsize, 1, infcb)!=fsize) return ENDFILE;
                inpoff=0;
                }
        return toupper(inbuff[inpoff++]);
        }
*/
int getbuff()
{
	return strupr(fgetc(infcb));
	}

/********************************************************
getbuff()
{
	if (inpoff == 128) {
		if (read(infcb,inbuff,1) == ERROR) return ENDFILE;
		inpoff=0;
		}
	return toupper(inbuff[inpoff++]);
	}
********************************************************/



/********************************************************/
/*                                                      */
/*      putbuff                                         */
/*                                                      */
/*      function:       to write chracter to output     */
/*                      buff and inc offset             */
/*                                                      */
/*      input:          character to write              */
/*                                                      */
/*      date written:   Dec 16, 1980 By Mike Bernson    */
/*                                                      */
/********************************************************/
putbuff(data)
char data;
{
        //outbuff[(line-1)*128+outoff++]=data;
		fputc(data,outfcb);
        }




/********************************************************/
/*                                                      */
/*      outdec                                          */
/*                                                      */
/*      function:       to output number in dec from    */
/*                      to console                      */
/*                                                      */
/*      date written:   Dec 16, 1980 By Mike Bernson    */
/*                                                      */
/********************************************************/
outdec(int number)
{
        char zero,num;
        int  place;

        place=10000;
        zero=0;

        while(place>0) {
                num=number/place+'0'; /* get current digit */
                if (num != '0' || place ==1 || zero) {
                        zero=1; /* set zero suppress */
                        //putch(num); /* print character */
						fputc_cons(num); /* print character */
                        }
                number=number % place;
                place=place/10;
                }
        }



// Customized puts, to avoid the final NEWLINE
// outec() and my_puts() replace printf() and save a little bit of memory
int my_puts(char *s) {
	int i;
    for (i = 0; s[i]; ++i)
        fputc_cons (s[i]);
}


/********************************************************/
/*                                                      */
/*      error                                           */
/*                                                      */
/*      function:       to print error message follow   */
/*                      by at line (line number)        */
/*                                                      */
/*      date written:   dec 16, 1980 By Mike Bernson    */
/*                                                      */
/********************************************************/
error(char *msg)
{
        //printf("%s at line %u",msg,line);
        my_puts(msg);              /* print error message */
        my_puts(" at line ");      /* print "at line" */
        outdec(line);           /* print line number */
		fclose(outfcb);
		remove("A:$$$.SUB");
        exit() ;                /* end back to cpm */
        }


/********************************************************/
/*                                                      */
/*      done                                            */
/*                                                      */
/*      function:       to write output buffer out      */
/*                      in backward record format       */
/*                                                      */
/*      Date written:   Dec 16, 1980 By Mike Bernson    */
/*                                                      */
/********************************************************/
done()
{
        char temp;

        /* create output file on disk a */
/*
        //if (open(outfcb,"A:$$$.SUB") != ERROR) 
		if((outfcb=fopen("A:$$$.SUB","wb"))==0) {
                //outfcb[32]=outfcb[15];
        //else if (create(outfcb,"A:$$$.SUB") == ERROR) {
                my_puts("Output File not created");
                exit();
                }
*/

/*
        while(--line) {
                //if (write(outfcb,outbuff+128*(line-1),1) == ERROR) {
				if (fwrite(outbuff+128*(line-1), 128, 1, outfcb) < 128) {
                        error("Disk is Full");
                        exit();
                        }
                }
*/
		//fclose(infcb);
        fclose(outfcb);
        prt=8;
        *prt=255;
        exit();
        }



/********************************************************/
/*                                                      */
/*      doinput                                         */
/*                                                      */
/*      function:       to proccess input buffer and    */
/*                      change $1-$9 to text needed and */
/*                      convert '^'a to control char    */
/*                                                      */
/*      date written    Jan 28, 1981 By Mike Bernson    */
/*                                                      */
/********************************************************/
doinput(char *parm[])
{
        int current;   /* current character working on */

        outoff=1;       /* set output offset to zero */

        /* pick up 1 line of input */
        while(outoff<120) {


                /* get charcter and see what need to be done */
                switch(current=getbuff()) {

                /* end of file  */
                case ENDFILE :
                        done();
        
                /* '$' can be $1-$9 or $$ */
                case '$' :
                        if ((current=getbuff())=='$') {
                                putbuff('$');
                                break;
                                }
                        if (current>='0' && current<='9') {
                                prt=parm[1+current-'0'];
                                while(*prt) putbuff(*prt++);
                                break;
                                }
                        error("Invalid parmeter");
                        break;

                /* check for control chraracter */
                case '^' :
                        current=getbuff();
                        if (current>='A' && current<='Z')
                                putbuff(current-'@');
                                else error("Invalid control character ");
                        break;
                
                /* check for end of line */
                case CR :
                        return;
        
                /* line feed ingore */
                case LF :
						putbuff('\n');
						line++;
                        break;
                
                /* must be a valid character put in output buffer */
                default:
                        putbuff(current);
                }}
        }

/********************************************************/
/*                                                      */
/*      dooutput                                        */
/*                                                      */
/*      function:       to write output record to disk  */
/*                      put character count at offset 0 */
/*                                                      */
/*      date written:   dec 16, 1980 By Mike Bernson    */
/*                                                      */
/********************************************************/
//dooutput()
//{
//        outbuff[(line-1)*128]=outoff-1; /* character count */
//
//        /* zero buffer till end */
//
//        while(outoff<128) putbuff(0);
//        if (++line>148) error("Too many lines");
//
//        }




/********************************************************/
/*                                                      */
/*      main                                            */
/*                                                      */
/*      function:       open input and output data file */
/*                      also process data and output    */
/*                      data to $$$.sub and set loc 8 ff*/
/*                                                      */
/*      Date written:   Jan 28, 1981 by Mike Bernson    */
/*                                                      */
/********************************************************/
main(int argc, char *argv[])
//int argc;             /* number of arg on command line +1 */
//int argv[];           /* pointer to each arg on command line */
{
        char filename[132];     /* hold input file name */
        char filename2[134];    /* hold input file name */
		
		
		//outbuff=malloc(17001);
        
        strcpy(filename,argv[1]);
        strcat(filename,".sub");

        //if (open(infcb,filename) == ERROR) {
        if((infcb=fopen(filename,"rb"))==0) {
                //infcb[0]=1;           /* try drive a */
                //if (bdos(15,infcb) == 0xff) {
                strcpy(filename2, "A:");
                strcat(filename2, filename);
                if((infcb=fopen(filename2,"rb"))==0) {
                        my_puts("Input file not found");
                        exit();
                        }
                }

        /* create output file on disk a */
        //if (open(outfcb,"A:$$$.SUB") != ERROR) 
		if((outfcb=fopen("A:$$$.SUB","wb"))==0) {
                my_puts("Output File not created");
                exit();
                }

        for(line=argc; line<12; argv[line++]="");
        //inpoff=128;
        line=1;

        /* process input file and write output */
        while(1) {
                doinput(argv);
                //dooutput();
                }
        }


