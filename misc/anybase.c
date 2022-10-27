/**************************** ANYBASE *******************************

                           by dAN mYERS

ANYBASE performs numeric conversion between two positive integers in
any base. Because the program uses the ASCII representation of a
number for I/O, litterally ANY base can be figured; interpretation
however, is up to you! Note that ANYBASE uses type "long int" for
calculations which should afford plenty of headroom for conversions.

********************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_ARRAY 64

int       i, base, adj_value;
char  place_value[MAX_ARRAY], *place_ptr;
long int base10_value;
char      num_to_convert[MAX_ARRAY], *num_ptr;

main () {
    place_value[0] = 1;
    base10_value = i = 0;

/******************** CONVERT TO BASE 10 **********************/
    printf ("What base is the number to convert in?\n");
    scanf ("%d", &base);
    printf ("Enter the base %d number\n", base);
    scanf ("%s", num_to_convert);

    num_ptr = &num_to_convert[strlen(num_to_convert)];
    while (--num_ptr >= num_to_convert) {
         if (*num_ptr < 58)
              adj_value = *num_ptr - '0';
         else
              adj_value = toupper(*num_ptr) - ('A' - 10);
         base10_value += adj_value * place_value[i];
         place_value[++i] = place_value[i-1] * base;
    }
/******************* CONVERT TO REQUESTED BASE ****************/
    printf ("What base do you want to convert to?\n");
    scanf  ("%d", &base);

    place_ptr = place_value;
    do {
         *place_ptr++ = base10_value % base;
         base10_value /= base;
    } while (base10_value >= base);
    *place_ptr = base10_value;

    printf ("The base %d equivilent is:\n", base);
    do {
         if (*place_ptr <= 9)
              printf ("%c", *place_ptr + '0');
         else
              printf ("%c", *place_ptr + ('A' - 10));
    } while (--place_ptr >= place_value);

    printf ("\n\n");
    main ();
}
