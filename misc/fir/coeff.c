
/* File coeff.c

/* Estimates required number of FIR filter coefficients

/* Based on formula in Rabiner & Gold, p. 156
/* Passband ripple entered in dB instead of %
/* where dB = 20*log10(1+%)

/* Adapted by Bob Briggs
/* 1 December 1989

 */


//  zcc +zx -lndos -create-app -lm coeff.c
//  zcc +cpm -lndos -create-app -lm coeff.c


 #include "stdio.h"
 #include "math.h"

void main(){
   char c;
   int r;
   double a1,a2,a3,a4,a5,a6,d0,d1,d2,d3,d4,f,f1,n;
   double fpass,fsamp,fstop;

   a1 = 0.005309;
   a2 = 0.07114;
   a3 = -0.4761;
   a4 = -0.00266;
   a5 = -0.5941;
   a6 = -0.4278;

   putchar('\n');
   printf("******* FIR filter coefficient number estimator *******\n");
   while(1){
      putchar('\n');
      printf("Enter passband ripple (in dB)\n");
      scanf("%lf",&d1);
      if(d1 <= 0.0){printf("Error: ripple must be > zero dB\n"); continue;}
      printf("Enter stopband attenuation (in dB)\n");
      scanf("%lf",&d2);
      printf("Enter transition width (normalized Hz, Nyquist = 0 to 0.5 Hz)\n");
		putchar('\n');
		printf("     [To enter frequencies instead, enter a negative number.\n");
		printf("         Program will prompt for frequencies and\n");
		printf("         calculate transition width as:\n");
		printf("         tw = (fstop - fpass)/fsamp ]\n");
      scanf("%lf",&f);

      if(f<0){
      	printf("Enter stopband edge frequency, fstop\n");
      	scanf("%lf",&fstop);
      	printf("Enter passband edge frequency, fpass\n");
      	scanf("%lf",&fpass);
      	printf("Enter sampling frequency, fsamp\n");
      	scanf("%lf",&fsamp);
      }

      putchar('\n');
      printf("Ripple = %g dB\n",d1);
      printf("Stopband attenuation = %g dB\n",d2);
      if(f<0){
         printf("fstop = %g\n",fstop);
      	printf("fpass = %g\n",fpass);
      	printf("fsamp = %g\n",fsamp);
        	printf("Transition width = %g\n",(fstop-fpass)/fsamp);
      }
      else printf("Transition width = %g (normalized Hz)\n",f);

      printf("Enter N or n to change data, RETURN if OK\n");
      while((c = getchar()) != '\n');  /* flush buffer */
      c = getchar();
      if(c != 'n' && c != 'N') break;
   }

   if(f<0) f = (fstop - fpass)/fsamp;
   d1 /= 20;
   d1 = pow(10,d1);
   d1 = (d1-1)/(d1+1);
   d3 = log10(d1);
   d2 /= -20;
   f1 = 11.01217 + 0.51244*(d3 - d2);
   d4 = d3*d3;
   d0 = a1*d4 + a2*d3 +a3;
   d0 *= d2;
   d0 += a4*d4 + a5*d3 + a6;
   n = d0/f - f1*f + 1;
   r = (int)(n + 0.51);
   printf("The required number of coefficients is %d\n",r);
}
