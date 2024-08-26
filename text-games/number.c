// Number
// Basic Computer Games
// Workman Publishing
// Converted to C by Michael Stroucken
// Original by Tom Adametx

// Tom Adametx wrote this program while a student at
// Curtis Junior High School in Sudbury, Massachusetts.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// A check was added to check for invalid guesses below 1.
// It seems the game actually picks 5 random values, and
// does stuff to the point balance depending on which one
// matches the user's guess.
// There is no consideration of the game's guesses being
// distinct.
// Added a check if points are zero or less.


void tab(int num) {
  int i;
  for (i=0; i<num; i++) {
    printf(" ");
  }
  return;
}

int fnr() {
  //return arc4random_uniform(5) + 1;
  return 1+rand()%5;
}

void game() {
  char line[256];
  int isDone = 0;
  int p = 100;
  do {
    //char *line = NULL;
    //size_t linecap = 0;

inputGuess:
    //line = NULL;
    //linecap = 0;

    printf("Guess a number from 1 to 5? ");
    //getline(&line, &linecap, stdin);
	gets(line);
	
    int guess = atoi(line);
    //free(line);
	srand(clock());

    if (guess > 5 || guess < 1) {
      goto inputGuess;
    }

    int r = fnr();
    int s = fnr();
    int t = fnr();
    int u = fnr();
    int v = fnr();

    int checked = 0;
    if (guess == r) {
      p -= 5;
      checked = 1;
    }

    if (!checked && guess == s) {
      p += 5;
      checked = 1;
    }

    if (!checked && guess == t) {
      p *= 2;
      printf("You hit the jackpot!!!\n");
      checked = 1;
    }

    if (!checked && guess == u) {
      p += 1;
      checked = 1;
    }

    if (!checked && guess == v) {
      p /= 2;
      checked = 1;
    }

    if (!checked) {
      // what now?
    }

    if (p > 500) {
      printf("!!!!You win!!!! with %d points.", p);
      isDone = 1;
    }

    if (p <= 0) {
      printf("You have no more points to play with.\n");
      isDone = 1;
    }

    if (!isDone) {
      printf("You have %d points.\n", p);
    }
  } while (!isDone);
}
    
  
  

int main() {

  tab(33);printf("Number\n");
  tab(15);printf("Creative Computing  Morristown, New Jersey\n");
  printf("\n\n\n");
  printf("You have 100 points. By guessing numbers from 1 to 5, you\n");
  printf("can gain or lose points depending upon how close you get to\n");
  printf("a random number selected by the computer.\n");
  printf("\n");
  printf("You occasionally will get a jackpot which will double(!)\n");
  printf("your point count. You win when you get 500 points.\n");

  game();

  printf("\n\n");
  return 0;
}
