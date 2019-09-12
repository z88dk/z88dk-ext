/* Ported to Z88DK  in 2019 */

/* +++Date last modified: 05-Jul-1997 */

/*
** by: John Lots
** patched up for BC++ 3.1 by Alan Eldridge 10/12/92
**     (UUCP: alane@wozzle.linet.org, FIDO: 1:272/38.473)
*/

#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#define         DEC(c)  (char)(((c)-' ')&077)

int main()
{
      int     n;
      char    buf[128],a,b,c,d;
	  char	*token;

      //scanf("begin %o ", &n);
      gets(buf);                              /* filename */
	  strtok(buf, " ");
	  token = strtok(NULL, " ");
	  token = strtok(NULL, " ");
	  printf ("%s/n",token);
	  
      if (!freopen(token, "wb", stdout))         /* oops.. */
      {
			printf("Error creating output file.. %s\n",token);
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
