/********************************************************/
/*                                                      */
/*      copyright March ,1981 By Mike Bernson           */
/*                                                      */
/*      program used to set and display time on QT      */
/*      s-100 clock calendar board                      */
/*                                                      */
/*      command format:                                 */
/*      time [hh:mm:ss] [am,pm] [mm/dd/yy] [day]        */
/*                                                      */
/********************************************************/


// Ported to z88dk in 2002 by Stefano Bodrato - NOT TESTED !

// This tool was written to deal with the CompuTime/QT Clock Boards
// The circuit was based on the OKI MSM5832 chip and could also produce
// four interrupt triggers (hour, minute, second and millisecond).

// zcc +cpm -create-app qt_time.c


#include <cpm.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#define CONTROL 129             /* address of control port */
#define ADDRESS 129             /* address port on clock board */
#define DATA    130             /* data port for clock board */
#define HOLD    0x10            /* bit for hold line on clock */
#define READ    0x20            /* bit for read line on clock */
#define WRITE   0x10            /* bit for write line on clock */
#define TRUE    1
#define FALSE   0


// Customized puts, to avoid the final NEWLINE
int my_puts(char *s) {
	int i;
    for (i = 0; s[i]; ++i)
        fputc_cons (s[i]);
    }



/********************************************************/
/*                                                      */
/*      time_display                                    */
/*                                                      */
/*      function:       to build a string for current   */
/*                      time in format hh:mm:ss [am,pm] */
/*                                                      */
/*                                                      */
/*      input:          string to put current time in   */
/*                                                      */
/*      return:         pointer to input string         */
/*                                                      */
/*      date written:   March 8, 1981 By Mike Bernson   */
/*                                                      */
/********************************************************/
time_display(string)
char *string;
{
        char clock[6],*temp1;
        int  temp,am;

        /* set hold on clock and read clock time */
        outp(CONTROL,HOLD);
        temp=-1;
        while(++temp<6) {
                outp(DATA,temp | READ);
                clock[temp]=inp(DATA);
                }
        outp(CONTROL,0);

        am=-1;
        if ((clock[5] & 8) == 0) am=(clock[5] & 4)/4;
        clock[5] &= 3;
        
        /* convert time to printable string */
        temp1=string;
        temp=6;
        while(--temp >= 0 ) {
                if (temp==1 || temp==3) *temp1++=':';
                *temp1++=clock[temp]+'0';
                }
        /* add am or pm if needed */
        switch(am) {

                case -1 :
                        strcpy(temp1," ");
                        break;

                case 0 :
                        strcpy(temp1," AM ");
                        break;

                case 1 : 
                        strcpy(temp1," PM ");
                        break;
                }
        return string;
        }

/********************************************************/
/*                                                      */
/*      time_set                                        */
/*                                                      */
/*      function:       to set the time from input      */
/*                                                      */
/*      input:          string that has time to set     */
/*                      am/pm/24 switch                 */
/*                                                      */
/*      date_written:   March 8, 1981 By Mike Bernson   */
/*                                                      */
/********************************************************/
time_set(string,am)
char *string;
int  am;
{
        char clock[13];
        int  temp;
        
        /* build array to send to clock for time */
        for(temp=5; temp>=2; temp--) {
                if (temp==5 && string[1] == ':') {
                        clock[5]=0;
                        continue;
                        }
                if (temp==3)
                        if (*string++ != ':') {
                                my_puts("Bad time format missing \":\"");
                                my_puts("(HH:MM [AM || PM])");
                                return TRUE;
                                }
                if (!isdigit(*string)) {
                                my_puts("Bad digit in time format");
                                return TRUE;
                                }
                clock[temp]=(*string++) & 0x0f;
                }
        clock[1]=clock[0]=0;

        if (*string !=0) {
                my_puts("Bad time format too many digits (HH:MM [AM || PM])");
                return TRUE;
                }
        
        /* check to see that ten second and tens of hours less then 7 */
        if (clock[3]>5) {
                my_puts("Bad tens of minutes digit");
                return TRUE;
                }

        /* check hours dights */
        temp=clock[5]*10+clock[4];
        if ((temp<1 || temp>24) && am<0) {
                my_puts("Bad hours range is 1-24 only");
                return TRUE;
                }
        if ((temp<1 || temp>12) && am>=0) {
                my_puts("Bad hours range is 1-12 only");
                return TRUE;
                }
        /* set 24 hour mode and am/pm bits */
        if (am<0) clock[5] |= 8;
        else clock[5] |= am * 4;

        /* wait for user to hit return before setting time */
        my_puts("Hit any key to set clock ");bios(3,0,0);

        /* send data to clock */
        for(temp=5; temp>=0; --temp) {
                outp(DATA,temp);
                outp(ADDRESS,clock[temp] | HOLD);
                outp(DATA,temp | WRITE);
                outp(DATA,temp);
                }
        outp(CONTROL,0);
        return FALSE;
        }


main(argc,argv)
int argc;
char *argv[];
{
        char string[80],*check;
        int  am;

        /* if no arg on command line display time */
        if (argc==1) {
                my_puts("Current time is ");
                my_puts(time_display(string));
                exit(0);
                }

        am=-1;
        check=argv[2];

        if (argc==3) {
                if (isalpha(check[0])) {
                        if (check[0]=='A') am=0;
                        if (check[0]=='P') am=1;
                        if (am != -1 && (check[1] !='M' || check[2] != 0)) {
                                my_puts("Bad command line must be ");
                                my_puts(" (HH:MM [AM || PM]) [MM/DD/YY]");
                                exit(0);
                                }
                        }
                }

        /* set time value from input */
        if (time_set(argv[1],am)) exit(0);
        my_puts("\nTime set to ");my_puts(time_display(string));
        exit(0);
        }

