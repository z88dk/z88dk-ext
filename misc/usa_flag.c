#include <stdio.h>
#include <string.h>

const char *S1 = "* * * * * * ";
const char *S2 = " * * * * * *";
const char *L = "---------------------------------------------";

char line[80];
int i, j;

int main(void) {

    // Stars
    for (i = 0; i < 9; i++) {
        if (i % 2 == 0)
            snprintf(line, sizeof(line), "%s %.*s", S1, 45 - (int)strlen(S1), L);
        else
            snprintf(line, sizeof(line), "%s %.*s", S2, 45 - (int)strlen(S2), L);

        printf("%s\n", line);
    }

    // Stripes
    for (i = 0; i < 13; i++) {
        printf("%s\n", L);
    }

    return 0;
}
