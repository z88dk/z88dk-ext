// Column ("Saeule")
// Basic-Computer-Spiele Band 2
// Sybex Verlag
// Converted to C by Michael Stroucken
// Original by Alan Barnes

// The trick behind this game is that after the first round,
// the matrix is read in column-major order and replaced in
// row major order. Each new column of 9 cards contains 3
// cards of the column that was chosen in the first round.
// The process is repeated after the second round, after which
// each column will contain one card from the 3 that shared
// the column the user picked in the second round. The third
// choice uniquely identifies the chosen card.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// In the original, the user chose from 21 cards in the deck, using
// 3 iterations choosing from 3 options. 3^3 is 27, however, so
// the game could also be played with a deck of 27 cards. If playing
// with 21 cards, the user can lie and will receive a card in the
// end anyway.

#define DECKSIZE 27

// Choose from all standard cards.
#define NUMCARDS 52

int deck[DECKSIZE];
char line[100];

void tab(int num) {
  int i;
  for (i=0; i<num; i++) {
    printf(" ");
  }
  return;
}

char *cardname(int card) {
  int suit, value;
  int length;
  char *name;

  const char *valuestring[] = {"Ace", "2", "3", "4", "5", "6", "7", "8",
                               "9", "10", "Jack", "Queen", "King"};
  const char *suitstring[] = {"Clubs", "Spades", "Hearts", "Diamonds"};

  suit = card % 4;
  value = card / 4;

  length = strlen(valuestring[value]) + strlen(suitstring[suit]) + 4 + 1;

  name = (char *)malloc(length);
    
  snprintf(name, length, "%s of %s", valuestring[value], suitstring[suit]);
  return name;
}

void pickdeck() {
  int state[NUMCARDS];
  int card;
  int choice;

  for (card=0; card < DECKSIZE; card++) {
    //while (state[choice = arc4random_uniform(NUMCARDS)] == 1);
	while (state[choice = rand()%NUMCARDS] == 1);
    state[choice] = 1;
    deck[card] = choice;
  }
  return;
}

void redeal(int *transform) {
  int column, row, pos;
  int newdeck[DECKSIZE];
  int numColumns = 3;
  int numRows = DECKSIZE / numColumns;

  pos = 0;
  for(column = 0; column < numColumns; column++) {
    for (row = 0; row < numRows; row++) {
      newdeck[pos] = deck[transform[column] - 1 + row*3];
      pos++;
    }
  }

  for(pos=0; pos < DECKSIZE; pos++) {
    deck[pos] = newdeck[pos];
  }

  return;
}

void showdeck() {
  int column;
  int card;
  int linepos;
  int spaces;
  char *name;

  column = 1;
  linepos = 0;
  for(card = 0; card < DECKSIZE; card++) {
    spaces = (column - 1) * 25 - linepos;
    tab(spaces);
    linepos += spaces;
    name = cardname(deck[card]);
    printf("%s", name);
    linepos += strlen(name);
    free(name);
    column = (column % 3) + 1;
    if (column == 1) {
      printf("\n");
      linepos = 0;
    }
  }
  printf("\n");
  return;
}

void showround() {
  //char *line = NULL;
  //size_t linecap = 0;
  int column = 0;
  int transform[3];

  showdeck();
  //printf("\n\n");
  do {
    printf("Which column contains your card? ");
    //line = NULL;
    //linecap = 0;
    //getline(&line, &linecap, stdin);
    gets(line);
    column = atoi(line);
    //free(line);
    if (column < 1 || column > 3) {
      printf("(1-3)\n");
    }
  } while (column < 1 || column > 3);

  transform[1] = column;
  transform[2] = (column = (column % 3) + 1);
  transform[0] = (column = (column % 3) + 1);

  redeal(transform);
  return;
}

  
void game() {
  int round;
  int i;
  pickdeck();
  for(round=1; round<=3; round++) {
    showround();
  }

  int posSolution = DECKSIZE / 2;
  // originally: for(i=0; i<11+(random() % 10)+1; i++) {
  for(i=0; i < posSolution; i++) {
    printf("%s\n", cardname(deck[i]));
  }

  printf("\n");
  printf("Oops!!! Your card is the ");
  printf("%s.\n", cardname(deck[posSolution]));
  
  return;
}

int main() {
  //char *line = NULL;
  //size_t linecap = 0;

  fputc_cons(12);   // CLS

  tab(26);printf("Column\n");
  //tab(20);printf("Creative Computing\n");
  //tab(18);printf("Morristown, New Jersey\n");
  printf("\n");
  printf("This program will show you a card trick, after the first deal\n");
  printf("pick a card and type the number of the column containing it.\n");
  printf("The dealer will then pick up the cards, a column at a time,\n");
  printf("and will deal them out again horizontally. When he finishes\n");
  printf("each time, type the number of the new column containing your\n");
  printf("card, following the last deal the dealer will turn over the\n");
  printf("cards, one at a time, until he reaches the one you picked.\n");
  printf("\n");

  int compare = 0;
  do {
    game();
    printf("Do you want to see it again? ");
    //line = NULL;
    //linecap = 0;
    //getline(&line, &linecap, stdin);
    gets(line);
	srand(clock());
    //compare = strncasecmp(line, "yes", 3);
    //free(line);
  } while ((line[0]=='y')||(line[0]=='Y'));

  return 0;
}
