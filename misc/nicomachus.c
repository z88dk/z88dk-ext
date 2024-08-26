// Nicomachus
// Basic Computer Games
// Workman Publishing
// Converted to C by Michael Stroucken
// Original by David Ahl

// zcc +zx -lndos -create-app -DAMALLOC nicomachus.c

// This puzzle was first derived by Nicomachos in 90 CE.
// and published in his "Arithmetica"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

// The original apparently didn't exercise the codepath
// when a user does not answer with yes or no when asked.
// The original has the code jump back to a non-existent
// line number in that case.
// This functionality has been fixed, along with missing
// punctuation added when the user's input is quoted back
// to him.

void tab(int num) {
  int i;
  for (i=0; i<num; i++) {
    printf(" ");
  }
  return;
}


void game() {
  //char *line = NULL;
  //size_t linecap = 0;
  char line[256];

  printf("Please think of a number between 1 and 100.\n");
  printf("Your number divided by 3 has a remainder of? ");

  //getline(&line, &linecap, stdin);
  gets(line);
  int mod3 = atoi(line);
  free(line);

  //line = NULL;
  //linecap = 0;
  printf("Your number divided by 5 has a remainder of? ");

  //getline(&line, &linecap, stdin);
  gets(line);
  int mod5 = atoi(line);
  free(line);

  //line = NULL;
  //linecap = 0;
  printf("Your number divided by 7 has a remainder of? ");

  //getline(&line, &linecap, stdin);
  gets(line);
  int mod7 = atoi(line);
  free(line);

  printf("\n");
  printf("Let me think a moment...\n");

  sleep(1);

  int num = 70*mod3 + 21*mod5 + 15*mod7;
  num %= 105;

  printf("Your number was %d, right? ", num);

  int isYes, isNo, isAnswered = 0;
  do {
    //line = NULL;
    //linecap = 0;
    //getline(&line, &linecap, stdin);
    gets(line);
    isYes = !strncasecmp(line, "yes", 3);
    isNo = !strncasecmp(line, "no", 2);
    isAnswered = (isYes || isNo);
    if (!isAnswered) {
      char *chopline = strdup(line);
      chopline[strlen(line) - 1] = 0;
      printf("Eh? I don't understand '%s'. Try 'yes' or 'no'.\n", chopline);
      free(chopline);
    }
    free(line);
  } while (!isAnswered);

  if (isYes) {
    printf("How about that!!\n");
  }

  if (isNo) {
    printf("I feel your arithmetic is in error.\n");
  }

  printf("\n");
  printf("Let's try another.\n");
}
    
  
  

int main() {
  tab(33);printf("Nicoma\n");
  tab(15);printf("Creative Computing  Morristown, New Jersey\n");
  printf("\n\n\n");
  printf("Boomerang puzzle from Arithmetica of Nicomachus -- A.D. 90!\n");
  printf("\n");

  while(1) {
    game();
  }
  
  return 0;
}
