/*
 * FIND is a progam was writen to help maintain files on my hard disc.
 * This program should run on any CP/M 2.2 or higher revision machine.
 * It was developed using CP/M 3.0 and the AZTEC C compiler vers. 1.06B.
 *
 * (This file is a quick z88dk conversion by Stefano Bodrato, Nov 13th, 2020)
 *
 * To compile on z88dk:
 *            zcc +cpm -create-app -ofind find.c
 * 
 *
 * As this program was a quick ditch effort, I welcome any and all 
 * modifications that anyone makes to it.  For example, anbiguous(* , ?)
 * filenames are not extracted from the DMA address after the bdos
 * call to "search filename".  The program also does not allow the
 * user to search specific user areas, it assumes a scan of user 0
 * thru user 15.
 *
 * Have fun with it, but this program is not for resale in any form.
 *
 * Please send any ideas/changes/reactions to me in mail.  Do not post.
 *
 * Jeff Gibson                     UUCP: {cepu,ihnp4,noao,uiucdcs}!bradley!jmg
 * Bradley University              ARPA: cepu!bradley!jmg@UCLA-LOCUS
 * Peoria, IL 61625                PH: (309) 692-9069
 *
 */

#include "stdio.h"
#include "stdlib.h"
#include "ctype.h"
#include "cpm.h"
#include "string.h"

#define FALSE 0
#define TRUE 1

int  drive_range[17],
     DRIVES;

char filename[16];

void check(char argv[])
{
    if  (tolower(argv[1]) == 'd')
       DRIVES = TRUE;
    else
      {
        printf ("\nERROR in argument\n");
        exit(0);
      }
}

            
void com_line(char line[])
{
   int i, j;
     
   if  (DRIVES)
    {
      i = 0;
      j = 0;
      while  (tolower(line[i]) >= 'a' && tolower(line[i]) <= 'p')
       {
           drive_range[j] = (int)(tolower(line[i++]) - 0x60);
           j++;
       } 
      drive_range[j] = -1;
    }
   else
     {
        printf("ERROR --- illegal drive specification.\n");
        exit(0);
     }
}



int main(int argc, char *argv[])
{
  struct fcb *fn;
  
  char  drive_name;
        
  int   i,
        old_user,
        new_user,
        found, 
        drive;
  

  if (--argc < 1)
   {
     printf("\nusage:  find -drives abcdefghijklmnop filename\n");
     exit(0);
   }
  
  DRIVES = found = FALSE;
  
  *++argv;
  if  (*argv[0] == '-')
   {
     check(*argv);
     com_line(*++argv);
   }
  else
    {
      *--argv;
      drive_range[0] = 0; 
      drive_range[1] = -1;
    }

  strcpy(filename,*++argv);
  setfcb(fn, filename);
    
  old_user = getuid();
  i = 0;
  putchar('\n');

  while (drive_range[i] != -1)
   {
	  fn->drive = drive_range[i];
      for (new_user=0; new_user < 16; new_user++)
       {
          if  (bdos(CPM_ICON, 0) == 1)                 /* check for key strike */
             if  (getchar() == 0x03)          /* abort on ^C */
                exit(0);
		  setuid(new_user);
			  if  (bdos(CPM_FFST, fn) != -1)
           {
             found = TRUE;
             if  (drive_range[i] == 0)
                drive_name = (char)(drive_range[i] + 65);
             else
                drive_name = (char)(drive_range[i] + 64);
             printf("%s: USER %d  DRIVE %c\n", fn->name, new_user, drive_name);
           }
       }
      i++;
   }

 if  (!found)
    printf("%s: was not found\n", filename);

 setuid(old_user);

}

