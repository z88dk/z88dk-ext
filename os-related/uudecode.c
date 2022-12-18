/*  Ported to Z88DK  in 2019  */

/*  zcc +cpm -create-app -O3 -ouudecode uudecode.c  */



/* +++Date last modified: 05-Jul-1997 */

/*
** by: John Lots
** patched up for BC++ 3.1 by Alan Eldridge 10/12/92
**     (UUCP: alane@wozzle.linet.org, FIDO: 1:272/38.473)
*/

#if defined(MSDOS) || defined(__MSDOS__) || defined(Z80)
 #define READ_BINARY "rb"
#else
 #define READ_BINARY "r"
#endif


#include        <stdio.h>
#include        <stdlib.h>

#define         DEC(c)  (char)(((c)-' ')&077)

int main(int argc, char *argv[])
{
      int     n;
      char    buf[128],a,b,c,d;

      if (argc == 2)
      {
            /* Use binary mode */
            if ((freopen(argv[1], READ_BINARY, stdin)) == NULL)
            {
                  //perror(argv[1]);
                  printf("Error opening file: %s\n",argv[1]);
                  exit(1);
            }
      }

      scanf("begin %o ", &n);
      gets(buf);                              /* filename */
      printf ("Creating file: %s\n",buf);
      
      if (!freopen(buf, "wb", stdout))         /* oops.. */
      {
            printf("Error creating output file.. %s\n",buf);
            exit(1);
      }

      while ((n=getchar())!=EOF&&((n=DEC(n))!=0))
      {
            while (n>0)
            {
                  a=DEC(getchar());
                  b=DEC(getchar());
                  c=DEC(getchar());
                  d=DEC(getchar());
                  if (n-->0)
                        putchar((a<<2)|(b>>4));
                  if (n-->0)
                        putchar((b<<4)|(c>>2));
                  if (n-->0)
                        putchar((c<<6)|d);
            }
            n=getchar();                    /* skip \n */
      }
      return 0;
}
