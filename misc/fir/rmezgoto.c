/* FILE RMEZGOTO.C */
/* 7 October 1989 */

/* Remez Exchange FIR Filter Design Program
/* Translated into C by Bob Briggs
/* Based on FORTRAN program by Parks McClellan as
/* listed in Prentice Hall book by Rabiner & Gold
/* "Theory and Application of Digital Signal Processing"
/* and BASIC translation by N. Loy in Prentice Hall book
/* "An Engineer's Guide to FIR Digital Filters"
/* Writes file "impulse.rmz"
*/

//  zcc +cpm -create-app -lm -O3 rmezgoto.c


#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#define PI  (3.141592653589793)
#define PI2 (6.283185307179586)
#define NFMAX (128)  /* maximum filter length */
#define EPS (1e-24)  /* small number */
#define ITRMAX (25)  /* maximum number of iterations */
#define NBMAX  (10)  /* maximum number of bands */

int nfcns,ngrid;
int iext[(NFMAX/2+2)+1];
double alpha[(NFMAX/2+2)+1],des[16*(NFMAX/2+2)+1];
double grid[16*(NFMAX/2+2)+1];
double wt[16*(NFMAX/2+2)+1];
double dev;
double ad[(NFMAX/2+2)+1],x[(NFMAX/2+2)+1],y[(NFMAX/2+2)+1];

main(){
void rerror(),remez();
int i,j,k,l;
int keyboard, example1, example2, example3, example4;
int jtype,kup,lband,lgrid,nfilt,nbands,result;
int jb,neg,nodd,nm1,nz;
double delf,fup,temp,change;
double d(),gee();
double deviat[NBMAX+1],wtx[NBMAX+1];
double edge[2*NBMAX+1],fx[NBMAX+1],h[(NFMAX/2+2)+1];
FILE *chan;

keyboard = 0;
example1 = 0;
example2 = 0;
example3 = 0;
example4 = 0;

jtype = 0;

printf("  ...   REMEZ EXCHANGE FIR FILTER DESIGN PROGRAM ... \n");
putchar('\n');

result = 0;
while(!result){
   printf("Select 1 to 5:   ---  then press ENTER\n");
   printf("1:  EXAMPLE1 --- LOWPASS FILTER\n");
   printf("2:  EXAMPLE2 --- BANDPASS FILTER\n");
   printf("3:  EXAMPLE3 --- DIFFERENTIATOR\n");
   printf("4:  EXAMPLE4 --- HILBERT TRANSFORMER\n");
   printf("5:  KEYBOARD --- GET INPUT PARAMETERS FROM KEYBOARD\n");
   putchar('\n');
   result = scanf("%d",&i);

   switch(i){
      case 1:  example1 = 1;
               break;
      case 2:  example2 = 1;
               break;
      case 3:  example3 = 1;
               break;
      case 4:  example4 = 1;
               break;
      case 5:  keyboard = 1;
               break;
      default: result = 0;
   }
}

if(keyboard){     /* GET DATA FROM KEYBOARD */
printf("Filter types are:  1=Bandpass, 2=Differentiator, 3=Hilbert\n");

/* GET INPUT DATA FROM USER */
printf("Input: # coeff, Filter type, # bands, Grid density\n");
printf("Use tab, space, or return to separate each input ...\n");
result = scanf("%d%d%d%d",&nfilt,&jtype,&nbands,&lgrid);

putchar('\n');
putchar('\n');
printf("Band and edge (corner) definition:\n");
putchar('\n');
printf("Edge #:  1   2   3     4   5           6\n");
printf("Band #:    1        2           3\n");
printf("        |        -------\n");
printf("        |       *       *\n");
printf("    Mag |      *         *\n");
printf("        |     *           *\n");
printf("        |-----             -------------\n");
printf("        |_______________________________\n");
printf("                 Frequency\n");

jb = 2 * nbands;
printf("Now inputting edge (corner) frequencies for %d band edges\n",jb);
for(i=1;i<=jb;i++){
  printf("Input edge frequency for edge (corner) # %d:\n",i);
  result = scanf("%lf",&edge[i]);   /* lf = long float = double */
}

for(i=1;i<=nbands;i++){
  printf("Input gain, weight of band # %d:",i);
  result = scanf("%lf%lf",&fx[i],&wtx[i]);
  putchar('\n');
}
}  /* end if(keyboard) */

else if(example1){      /* LOWPASS FILTER */
nfilt = 24;
jtype = 1;
nbands = 2;
lgrid = 16;
edge[1] = 0;
edge[2] = 0.08;
edge[3] = 0.16;
edge[4] = 0.5;
fx[1] = 1;
fx[2] = 0;
wtx[1] = 1;
wtx[2] = 1;
}

else if(example2){         /* BANDPASS FILTER */
nfilt = 32;
jtype = 1;
nbands = 3;
lgrid = 16;
edge[1] = 0;
edge[2] = 0.1;
edge[3] = 0.2;
edge[4] = 0.35;
edge[5] = 0.425;
edge[6] = 0.5;
fx[1] = 0;
fx[2] = 1;
fx[3] = 0;
wtx[1] = 10;
wtx[2] = 1;
wtx[3] = 10;
}

else if(example3){          /* DIFFERENTIATOR */
nfilt = 32;
jtype = 2;
nbands = 1;
lgrid = 16;
edge[1] = 0;
edge[2] = 0.5;
fx[1] = 1;
wtx[1] = 1;
}

else if(example4){         /* HILBERT TRANSFORMER */
nfilt = 20;
jtype = 3;
nbands = 1;
lgrid = 16;
edge[1] = 0.05;
edge[2] = 0.5;
fx[1] = 1;
wtx[1] = 1;
}
/* END INPUT DATA SECTION */

   if(nfilt > NFMAX){
      printf("Error: # coeff > %d\n...now exiting to system...\n",NFMAX);
      exit(1);
   }

   if(nfilt < 3) rerror("Error: # coeff < 3");
   if(nbands <= 0) nbands = 1;
   if(lgrid <= 0) lgrid = 16;

   printf("#coeff = %d\nType = %d\n#bands =  %d\nGrid = %d\n",
      nfilt,jtype,nbands,lgrid);
   for(i=1;i<=2*nbands;i++) printf("E[%d] = %.2lf\n",i,edge[i]);
   for(i=1;i<=nbands;i++) printf("Gain, wt[%d] = %.2lf  %.2lf\n",i,fx[i],wtx[i]);
   putchar('\n');

   if(jtype == 0) rerror("Filter type = 0 not valid\n");

   neg = 1;
   if(jtype == 1) neg = 0;
   nodd  = nfilt % 2;
   nfcns = nfilt / 2;
   if(nodd == 1 && neg == 0) nfcns++;

/* SET UP THE DENSE GRID.  THE NUMBER OF POINTS IN THE GRID
/* IS (FILTER LENGTH + 1)*GRID DENSITY/2 */
   grid[1] = edge[1];
   delf = lgrid * nfcns;
   delf = 0.5/delf;
   if(neg == 0) goto lable135;
   if(edge[1] < delf) grid[1] = delf;
lable135:   j = 1;
   l = 1;
   lband = 1;
lable140:  fup = edge[l+1];
lable145:  temp = grid[j];

/* CALCULATE THE DESIRED MAGNITUDE RESPONSE AND THE WEIGHT
/* FUNCTION ON THE GRID */
   des[j] = fx[lband];
   if(jtype == 2) des[j] *= temp;
   wt[j] = wtx[lband];
   if(jtype == 2 && fx[lband] >= 0.0001) wt[j] /= temp;
   j++;
   grid[j] = temp + delf;
   if(grid[j] > fup) goto lable150;
   goto lable145;
lable150:   grid[j-1] = fup;
   des[j-1] = fx[lband];
   if(jtype == 2) des[j-1] *= fup;
   wt[j-1] = wtx[lband];
   if(jtype == 2 && fx[lband] >= 0.0001) wt[j-1] /= fup;
   lband++;
   l += 2;
   if(lband > nbands) goto lable160;
   grid[j] = edge[l];
   goto lable140;
lable160: ngrid = j-1;
   if(neg != nodd) goto lable165;
   if(grid[ngrid] > (0.5 - delf)) ngrid--;
lable165:  ;

/* SET UP A NEW APPROXIMATION PROBLEM WHICH IS EQUIVALENT
/* TO THE ORIGINAL PROBLEM */
   if(neg > 0) goto lable180;
   if(nodd == 1) goto lable200;    /* DO NOTHING */
   for(j=1;j <= ngrid;j++){
      change = cos(PI * grid[j]);
      if(change == 0) change = EPS;
      des[j] = des[j]/change;
      wt[j] *= change;
   }
   goto lable200;
lable180:   if(nodd == 1) goto lable190;
      for(j=1;j <= ngrid;j++){
         change = sin(PI * grid[j]);
         if(change == 0) change = EPS;
         des[j] = des[j]/change;
         wt[j] *= change;
      }
   goto lable200;
lable190:   for(j=1;j <= ngrid;j++){
               change = sin(PI2 * grid[j]);
               if(change == 0) change = EPS;
               des[j] = des[j]/change;
               wt[j] *= change;
            }

/* INITIAL GUESS FOR THE EXTREMAL FREQUENCIES -- EQUALLY
/* SPACED ALONG THE GRID */
lable200:   temp = (ngrid - 1)/nfcns;
for(j=1;j<=nfcns;j++) iext[j] = (j-1) * temp + 1;
iext[nfcns+1] = ngrid;
nm1 = nfcns - 1;
nz = nfcns + 1;

/* CALL THE REMEZ EXCHANGE ALGORITHM TO DO THE APPROXIMATION PROBLEM */
remez(edge,nbands);

if(neg <= 0){
   if(nodd == 0){
      h[1] = 0.25 * alpha[nfcns];
      for(j=2;j<=nm1;j++)
         h[j] = 0.25 * (alpha[nz - j] + alpha[nfcns + 2 - j]);
      h[nfcns] = 0.5 * alpha[1] + 0.25 * alpha[2];
   }
   else{
      for(j=1;j<=nm1;j++) h[j] = 0.5 * alpha[nz - j];
      h[nfcns] = alpha[1];
   }
}
else{
   if(nodd == 0){
      h[1] = 0.25 * alpha[nfcns];
      for(j=2;j<=nm1;j++)
         h[j] = 0.25 * (alpha[nz - j] - alpha[nfcns + 2 - j]);
      h[nfcns] = 0.5 * alpha[1] - 0.25 * alpha[2];
   }
   else{
      h[1] = 0.25 * alpha[nfcns];
      h[2] = 0.25 * alpha[nm1];
      for(j=3;j<=nm1;j++)
         h[j] = 0.25 * (alpha[nz-j] - alpha[nfcns + 3 - j]);
      h[nfcns] = 0.5 * alpha[1] - 0.25 * alpha[3];
      h[nz] = 0.0;
   }
}

/* PROGRAM OUTPUT SECTION */

putchar('\n');
for(j=1;j<=70;j++) putchar('*');
putchar('\n');
printf("                        FINITE IMPULSE RESPONSE (FIR)\n");
printf("                        LINEAR PHASE DIGITAL FILTER DESIGN\n");
printf("                        REMEZ EXCHANGE ALGORITHM\n");
switch(jtype){
   case 1: printf("                        BANDPASS FILTER\n");
           break;
   case 2: printf("                        DIFFERENTIATOR\n");
           break;
   case 3: printf("                        HILBERT TRANSFORMER\n");
           break;
}
printf("              FILTER LENGTH = %d\n",nfilt);
printf("              ***** IMPULSE RESPONSE *****\n");
for(j=1;j<=nfcns;j++){
   k = nfilt + 1 - j;
   if(neg == 0)
      printf("                   H(%3d) = %15.9e = H(%4d)\n",j,h[j],k);
   if(neg == 1)
      printf("                   H(%3d) = %15.9e = -H(%4d)\n",j,h[j],k);
}
if(neg == 1 && nodd == 1) printf("H(%3d) = 0.0\n",nz);
putchar('\n');
for(k=1;k<=nbands;k+=4){
   kup = k + 3;
   if(kup > nbands) kup = nbands;
   for(j=1;j<=23;j++) putchar(' ');
   for(j=k;j<=kup;j++) printf("BAND%2d         ",j);
   putchar('\n');
   printf(" LOWER BAND EDGE");
   for(j=k;j<=kup;j++) printf("%15.8f",edge[2*j-1]);
   putchar('\n');
   printf(" UPPER BAND EDGE");
   for(j=k;j<=kup;j++) printf("%15.8f",edge[2*j]);
   putchar('\n');
   if(jtype != 2){
      printf(" DESIRED VALUE  ");
      for(j=k;j<=kup;j++) printf("%15.8f",fx[j]);
      putchar('\n');
   }
   if(jtype == 2){
      printf(" DESIRED SLOPE  ");
      for(j=k;j<=kup;j++) printf("%15.8f",fx[j]);
      putchar('\n');
   }
   printf(" WEIGHTING      ");
   for(j=k;j<=kup;j++) printf("%15.8f",wtx[j]);
   putchar('\n');
   for(j=k;j<=kup;j++) deviat[j] = dev/wtx[j];
   printf(" DEVIATION      ");
   for(j=k;j<=kup;j++) printf("%15.8f",deviat[j]);
   putchar('\n');
   if(jtype != 1) continue;
   for(j=k;j<=kup;j++) deviat[j] = 20.0 * log10(deviat[j]);
   printf(" DEVIATION IN DB");
   for(j=k;j<=kup;j++) printf("%15.8f",deviat[j]);
   putchar('\n');
}
putchar('\n');
printf(" EXTREMAL FREQUENCIES\n");
putchar(' ');
for(j=1;j<=nz;j++){
   printf("%12.7f",grid[iext[j]]);
   if(!(j%5)){ putchar('\n');putchar(' ');}  /* CR every 5 columns */
}
putchar('\n');
for(j=1;j<=70;j++) putchar('*');
putchar('\n');

/* NOW WRITE DATA TO FILE "IMPULSE.RMZ" */
chan = fopen("impulse.rmz","wb");  /* opens file for write binary */
if(chan == 0){printf("Cannot open file %s\n","impulse.rmz");exit(0);}
for(j=1;j<=nfcns;j++){
   fprintf(chan,"%f ",h[j]);
   if(!(j%4)){fputc('\r',chan);fputc('\n',chan);}  /* CRLF every 4 values */
}
if(neg == 1 && nodd == 1) fprintf(chan,"0.0 ");
for(j=nfcns;j>=1;j--){
   if(neg == 0) fprintf(chan,"%f ",h[j]);
   if(neg == 1) fprintf(chan,"%f ",-h[j]);
   if(!((nfcns-j+1)%4)){fputc('\r',chan);fputc('\n',chan);}
     /* CRLF every 4 values */
}
fclose(chan);

}   /* END main() */

/* REMEZ EXCHANGE ALGORITHM */
void remez(edge,nbands) double edge[];int nbands;{
int j,k,l;
int jchnge,jet,jm1,jp1;
int k1,kkk,klow,kn,knz,kup;
int loop1,luck;
int niter,nm1,nu,nut,nut1,nz,nzz;
int out1,out2,out3;
double cn,fsh,gtemp;
double delf,tmp;
double devl,dtemp,dnum,dden,err;
double comp,y1,ynz,aa,bb,ft,xt,xe;
double d(),gee();
double a[67],p[67],q[67];

   devl = -1.0;
   nz = nfcns + 1;
   nzz = nfcns + 2;
   niter = 0;
   comp = 0;

   printf("Iteration");
lable100:
   iext[nzz] = ngrid + 1;
   niter++;
   if(niter > ITRMAX) goto lable400;
   printf(" %d",niter);
   if(!(niter%20)) putchar('\n');   /* newline every 20 iterations */
   for(j=1;j<=nz;j++){
      dtemp = cos(PI2 * grid[iext[j]]);
      x[j] = dtemp;
   }
   jet = (int)((nfcns - 1)/15 +1);
   for(j=1;j<=nz;j++) ad[j] = d(j,nz,jet);
   dnum = 0.0;
   dden = 0.0;
   k=1;
   for(j=1;j<=nz;j++){
      l=iext[j];
      dnum += ad[j] * des[l];
      if(wt[l] == 0.0) wt[l] = EPS;
      dden += k*ad[j]/wt[l];
      k = -k;
   }
   dev = dnum/dden;
   nu = 1;
   if(dev > 0.0) nu = -1;
   dev *= -nu;
   k = nu;
   for(j=1;j<=nz;j++){
      l = iext[j];
      if(wt[l] == 0.0) wt[l] = EPS;
      dtemp = k*dev/wt[l];
      y[j] = des[l] + dtemp;
      k = -k;
   }
   if(dev > devl) goto lable150;
      printf("   ********* FAILURE TO CONVERGE **********\n");
      printf("Probable cause is machine rounding error\n");
      printf("The impulse response may be correct\n");
      printf("Check with frequency response\n");
   goto lable400;
lable150:   devl = dev;
   jchnge = 0;
   k1 = iext[1];
   knz = iext[nz];
   klow = 0;
   nut = -nu;
   j = 1;

/* SEARCH FOR THE EXTREMAL FREQUENCIES OF THE BEST APPROXIMATION */

lable200:
   if(j == nzz) ynz = comp;
   if(j >= nzz) goto lable300;
   kup = iext[j+1];
   l = iext[j] + 1;
   nut = - nut;
   if(j == 2) y1 = comp;
   comp = dev;
   if(l >= kup) goto lable220;
   err = (gee(l,nz) - des[l]) * wt[l];
   dtemp = nut * err - comp;
   if(dtemp <= 0.0) goto lable220;
   comp = nut * err;
lable210:
   l++;
   if(l >= kup) goto lable215;
   err = (gee(l,nz) - des[l]) * wt[l];
   dtemp = nut * err - comp;
   if(dtemp <= 0.0) goto lable215;
   comp = nut * err;
   goto lable210;
lable215:
   iext[j] = l - 1;
   j++;
   klow = l - 1;
   jchnge++;
   goto lable200;
lable220:
   l--;
lable225:
   l--;
   if(l <= klow) goto lable250;
   err = (gee(l,nz) - des[l]) * wt[l];
   dtemp = nut * err - comp;
   if(dtemp > 0.0) goto lable230;
   if(jchnge <= 0) goto lable225;
   goto lable260;
lable230:
   comp = nut * err;
lable235:
   l--;
   if(l <= klow) goto lable240;
   err = (gee(l,nz) - des[l]) * wt[l];
   dtemp = nut * err - comp;
   if(dtemp <= 0.0) goto lable240;
   comp = nut * err;
   goto lable235;
lable240:   klow = iext[j];
   iext[j] = l+1;
   j++;
   jchnge++;
   goto lable200;
lable250:
   l = iext[j] + 1;
   if(jchnge > 0) goto lable215;
lable255:
   l++;
   if(l >= kup) goto lable260;
   err = (gee(l,nz) - des[l]) * wt[l];
   dtemp = nut * err - comp;
   if(dtemp <= 0.0) goto lable255;
   comp = nut * err;
   goto lable210;
lable260:
   klow = iext[j];
   j++;
   goto lable200;
lable300:
   if(j > nzz) goto lable320;
   if(k1 > iext[1]) k1 = iext[1];
   if(knz < iext[nz]) knz = iext[nz];
   nut1 = nut;
   nut = -nu;
   l = 0;
   kup = k1;
   comp = ynz * (1.00001);
   luck = 1;
lable310:
   l++;
   if(l >= kup) goto lable315;
   err = (gee(l,nz) - des[l]) * wt[l];
   dtemp = nut * err - comp;
   if(dtemp <= 0.0) goto lable310;
   comp = nut * err;
   j = nzz;
   goto lable210;
lable315:
   luck = 6;
   goto lable325;
lable320:
   if(luck > 9) goto lable350;
   if(comp > y1) y1 = comp;
   k1 = iext[nzz];
lable325:
   l = ngrid + 1;
   klow = knz;
   nut = -nut1;
   comp = y1 * (1.00001);
lable330:
   l--;
   if(l <= klow) goto lable340;
   err = (gee(l,nz) - des[l]) * wt[l];
   dtemp = nut * err - comp;
   if(dtemp <= 0.0) goto lable330;
   j = nzz;
   comp = nut * err;
   luck += 10;
   goto lable235;
lable340:
   if(luck == 6) goto lable370;
   for(j=1;j<=nfcns;j++) iext[nzz - j] = iext[nz - j];
   iext[1] = k1;
   goto lable100;
lable350:
   kn = iext[nzz];
   for(j=1;j<=nfcns;j++) iext[j] = iext[j+1];
   iext[nz] = kn;
   goto lable100;
lable370:
   if(jchnge > 0) goto lable100;

/* CALCULATION OF THE COEFFICIENTS OF THE BEST APPROXIMATION
/* USING THE INVERSE DISCRETE FOURIER TRANSFORM */

lable400:
   nm1 = nfcns - 1;
   fsh = 1.0e-6;
   gtemp = grid[1];
   x[nzz] = -2.0;
   cn = 2 * nfcns - 1;
   delf = 1.0 / cn;
   l = 1;
   kkk = 0;
   if(edge[1] < 0.01 && edge[2*nbands] > 0.49) kkk = 1;
   if(nfcns <= 3) kkk = 1;
if(kkk != 1) {
   dtemp = cos(PI2 * grid[1]);
   dnum = cos(PI2 * grid[ngrid]);
   tmp = dtemp - dnum;
   if(tmp == 0.0) tmp = EPS;
   aa = 2.0/tmp;
   bb = -(dtemp + dnum)/tmp;
}

for(j=1;j<=nfcns;j++){
   ft = (j - 1) * delf;
   xt = cos(PI2 * ft);
   if(kkk != 1){
      xt = (xt - bb)/aa;
      ft = acos(xt)/PI2;
   }
   out1 = 0;
   out2 = 0;
   while(1){         /* LOOP 9 */
      xe = x[l];
      if(xt > xe){
         out1 = 1;
         break;
      }
      if((xe - xt) < fsh){
         out2 = 1;
         break;
      }
      l++;
   }     /* END OF while LOOP 9 */
   if(out1){
      if((xt - xe) < fsh) a[j] = y[l];
      else{
         grid[1] = ft;
         a[j] = gee(1,nz);
      }
   }
   if(out2) a[j] = y[l];
   if(l > 1) l--;
}  /* END for() LOOP */

grid[1] = gtemp;
dden = PI2/cn;

for(j=1;j<=nfcns;j++){
   dtemp = 0.0;
   dnum = (j-1) * dden;
   if(nm1 >= 1) for(k=1;k<=nm1;k++)
      dtemp += a[k+1] * cos(dnum * k);
   alpha[j] = 2 * dtemp + a[1];
}  /* END for() LOOP */

for(j=2;j<=nfcns;j++) alpha[j] *= 2/cn;
alpha[1] /= cn;
if(kkk != 1){
   p[1] = 2.0 * alpha[nfcns] * bb + alpha[nm1];
   p[2] = 2.0 * alpha[nfcns] * aa;
   q[1] = alpha[nfcns - 2] - alpha[nfcns];
   for(j=2;j<=nm1;j++){
      if(j >= nm1){
         aa *= 0.5;
         bb *= 0.5;
      }
      p[j+1] = 0.0;
      for(k=1;k<=j;k++){
         a[k] = p[k];
         p[k] = 2.0 * bb * a[k];
      }
      p[2] += 2.0 * aa * a[1];
      jm1 = j-1;
      for(k=1;k<=jm1;k++) p[k] += q[k] + aa * a[k+1];
      jp1 = j + 1;
      for(k=3;k<=jp1;k++) p[k] += aa * a[k-1];
      if(j != nm1){
         for(k=1;k<=j;k++) q[k] = - a[k];
         q[1] += alpha[nfcns - 1 - j];
      }
   }     /* END OF for() LOOP */

   for(j=1;j<=nfcns;j++) alpha[j] = p[j];
}     /* END OF if(kkk != 1) */

if(nfcns <= 3){
   alpha[nfcns + 1] = 0.0;
   alpha[nfcns + 2] = 0.0;
}
}     /* END REMEZ EXCHANGE ALGORITHM */

/* FUNCTION TO CALCULATE THE LAGRANGE INTERPOLATION
/* COEFFICIENTS FOR USE IN THE FUNCTION GEE */
double d(k,n,m) int k,n,m;{
   int j,el;
   double de,q;

   de = 1.0;
   q = x[k];

   for(el=1;el<=m;el++)
      for(j=el;j<=n;j+=m)
         if(j != k) de *= 2.0 * (q - x[j]);

   if(de == 0.0) de = EPS;
   return(1.0/de);
}

/* FUNCTION TO EVALUATE THE FREQUENCY RESPONSE USING THE
/* LAGRANGE INTERPOLATION FORMULA IN THE BARYCENTRIC FORM */
double gee(k,n) int k,n;{
   int j;
   double c,de,p,xf;

   de = 0.0;
   p = 0.0;
   xf = cos(PI2*grid[k]);

   for(j=1;j<=n;j++){
      c = xf - x[j];
      if(c == 0.0) c = EPS;
      c = ad[j]/c;
      de += c;
      p += c * y[j];
   }
   if(de == 0.0) de = EPS;
   return(p/de);
}

void rerror(error_text) char error_text[];{
   printf("%s\n",error_text);
   printf("...now exiting to system...\n");
   exit(1);
}
