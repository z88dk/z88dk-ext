#include <stdio.h>

    unsigned int r[4201];
    unsigned long i
	unsigned int k;
    unsigned long d;
	unsigned int b;
    unsigned int c;


int main() {

	c = 0;
	
    for (k = 0; k < 4200; k++) {
        r[k] = 2000L;
    }

    for (k = 4200; k > 0; k -= 14) {
        d = 0L;

        i = k;
        for (;;) {
            d += r[i] * 10000L;
            b = 2L * i - 1L;

            r[i] = d % b;
            d /= b;
            i--;
            if (i == 0L) break;
            d *= i;
        }
        printf("%04u", c + (unsigned int) (d / 10000L));
        c = d % 10000L;
    }

    return 0;
	
}
