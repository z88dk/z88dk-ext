/* File pad.c

/* Reads commandline file
/* Writes file "padded.dat" with commandline number of data points
/* Writes "0.0"s if needed to create sufficient points
/* Useful for zero padding short impulse response sequences so
/* that long FFT can be used to obtain good frequency resolution
 */

// zcc +cpm -create-app -O3 pad.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

main(argc, argv) int argc; char *argv[];{
char nam[16];
char temp[80];
static char nam1[16] = "padded.dat";
int i,j,size;
float *p;
FILE *chan;
FILE *chan1;

if(argc < 3) {
   printf("Usage:    pad [filename] [total data points]\n");
   printf("Example:  pad impulse.rmz 512\n");
   exit(0);
}

else strcpy (nam,argv[1]); /* get commandline filename into nam */
size = atoi(argv[2]);

chan = fopen(nam,"r");     /* opens file for read */
if (chan == 0){
   printf("Cannot open file %s\n",nam);
   return;           /* bail out if can't open file */
}

/* OPEN FILE FOR WRITE */
chan1 = 0;
while(chan1 == 0){
      chan1 = fopen(nam1,"wb");  /* opens file for write binary */
      if (chan1 == 0){printf("Cannot open file %s\n",nam1); exit(0);}
}

i = 1;
do{
   j = fscanf(chan,"%s",temp);
   if(j == 0){printf("Error reading %s\n",nam); exit(0);}
   if(j == EOF || i > size) break;
   fprintf(chan1,"%s ",temp);
   if(!(i%6)){fprintf(chan1,"\r"); fprintf(chan1,"\n");}
}while(i++);

i--;
while(i++ < size){
   fprintf(chan1,"%s ","0.0");
   if(!(i%6)){fprintf(chan1,"\r"); fprintf(chan1,"\n");}
}

printf("Done writing padded.dat\n");
fclose(chan);
fclose(chan1);
}
