// Hamurabi
// Basic Computer Games
// Workman Publishing
// Converted to C by Michael Stroucken

// Adapted to z88dk by Stefano Bodrato
//  zcc +zx -lndos -create-app -lm hamurabi.c

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// David Ahl's note:
// This program was originally written in Focal at DEC; author unknown.
// David Ahl converted it to BASIC and added the 10-year performance
// accessment. If you wish to change any of the factors, the extensive
// remarks in the program should make modification fairly straightforward.
// Note for trivia buffs: Somewhere along the line an m was dropped out of
// the spelling of Hammurabi in the Ahl version of the computer program.
// This error has spread far and wide until a generation of students
// who have used this program now think that Hammurabi is the incorrect
// spelling.

// Porter's note:
// The particular impetus to port this game came from a side discussion
// held at Demosplash 2017 at Carnegie-Mellon University 
// in Pittsburgh PA US.

void tab(int num) {
  int i;
  for (i=0; i<num; i++) {
    printf(" ");
  }
  return;
}

int fnr(unsigned int p) {
  //return arc4random_uniform(p);
  return rand()%p;
}

int population = 95;
int grain = 2800;
int harvest = 3000;
int bushelAcre = 3;
int immigration = 5;
int plague = 1;
int year = 0;
int dead = 0;
int deathrate = 0;
int totalDead = 0;
int val = 0;
int ratfood;
int acres;


void game() {
  ratfood = harvest - grain;
  acres = harvest / bushelAcre;

  //char *line = NULL;
  //size_t linecap = 0;
  char *line[256];

  round:
  srand(clock());
  year++;

  printf("\n\n\n");
  printf("Hamurabi:  I beg to report to you,\n");
  printf("in year %d, %d people starved, %d came to the city.\n", year, dead, immigration);

  population += immigration;

  if (0 > plague) {
    printf("A horrible plague struck!  Half the people died.\n");
    population >>= 1;
  }

  printf("Population is now %d.\n", population);
  printf("The city now owns %d acres.\n", acres);
  printf("You harvested %d bushels per acre.\n", bushelAcre);
  printf("Rats ate %d bushels.\n", ratfood);
  printf("You now have %d bushels in store.\n", grain);
  printf("\n");

  if (year == 11) goto endOfTerm;

  bushelAcre = 17 + fnr(10);

  printf("Land is trading at %d bushels per acre.\n", bushelAcre);

  buy:
  printf("How many acres do you wish to buy? ");
  //getline(&line, &linecap, stdin);
  gets(line);
  val = atoi(line);
  if (val < 0) goto badInputExit;

  if (val * bushelAcre > grain) {
    printf("Hamurabi:  Think again.  You have only\n");
    printf("%d bushels of grain.  Now then:\n", grain);
    goto buy;
  }

  acres += val; grain -= val * bushelAcre;

  sell:
  printf("How many acres do you wish to sell? ");
  //getline(&line, &linecap, stdin);
  gets(line);
  val = atoi(line);
  if (val < 0) goto badInputExit;

  if (val >= acres) {
    printf("Hamurabi:  Think again.  You only own ");
    printf("%d acres.  Now then:\n", acres);
    goto sell;
  }

  acres -= val; grain += val * bushelAcre;
  printf("\n");

  feed:
  printf("How many bushels do you wish to feed your people? ");
  //getline(&line, &linecap, stdin);
  gets(line);
  val = atoi(line);
  if (val < 0) goto badInputExit;

  if (val > grain) {
    // Trying to use more grain than is in silos?
    printf("Hamurabi:  Think again.  You have only\n");
    printf("%d bushels of grain.  Now then:\n", grain);
    goto feed;
  }

  int food = val;
  grain -= food;
  printf("\n");

  plant:
  printf("How many acres do you wish to plant with seed? ");
  //getline(&line, &linecap, stdin);
  gets(line);
  val = atoi(line);
  if (val < 0) goto badInputExit;

  if (val > acres) {
    // Trying to plant more acres than you own?
    printf("Hamurabi:  Think again.  You only own ");
    printf("%d acres.  Now then:\n", acres);
    goto plant;
  }

  if (val/2 > grain) {
    // Enough grain for seed?
    printf("Hamurabi:  Think again.  You have only\n");
    printf("%d bushels of grain.  Now then:\n", grain);
    goto plant;
  }

  if (val > population * 10) {
    // Enough people to tend the crops?
    printf("But you have only %d people to tend the fields!  Now then:\n", population);
    goto plant;
  }

  int planted = val;
  grain -= planted/2;

  int coin = fnr(5) + 1;

  // A bountiful harvest!
  bushelAcre = coin;
  harvest = planted * bushelAcre;

  coin = fnr(5) + 1;
  ratfood = 0;

  if (!(coin & 1)) {
    // rats are running wild!!
    ratfood = grain / coin;
  }

  grain -= ratfood;
  grain += harvest;

  coin = fnr(5) + 1;
  // let's have some babies
  immigration = coin * (20 * acres + grain) / population / 100 + 1;

  // how many people had full tummies?
  int fedPeople = food / 20;

  // Horror, a 15% chance of plague
  plague = fnr(100) - 15;

  if (population >= fedPeople) {
    // Starve enough for impeachment?
    dead = population - fedPeople;
    if (dead > 0.45* population) {
      printf("\n");
      printf("You starved %d people in one year!!!\n", dead);
      goto impeachment;
    }
  }

  deathrate = ((year-1) * deathrate + dead * 100 / population) / year;
  population = fedPeople;
  totalDead += dead;
  goto round;

  impeachment:
  printf("Due to this extreme mismanagement you have not only\n");
  printf("been impeached and thrown out of office but you have\n");
  printf("also been declared national fink!!!!\n");
  goto end;

  badInputExit:
  printf("Hamurabi:  I cannot do what you wish.\n");
  printf("Get yourself another steward!!!!!\n");
  goto end;

  endOfTerm:
  printf("In your 10-year term of office, %d percent of the\n", deathrate);
  printf("population starved per year on the average, i.e. a total of\n");
  printf("%d people died!!\n", totalDead);

  int acresPerson = acres / population;
  printf("You started with 10 acres per person and ended with\n");
  printf("%d acres per person.\n", acresPerson);

  printf("\n");

  if (deathrate > 33 || acresPerson < 7) goto impeachment;

  if (deathrate > 10 || acresPerson < 9) {
    printf("Your heavy-handed performance smacks of Nero and Ivan IV.\n");
    printf("The people (remaining) find you an unpleasant ruler, and,\n");
    printf("frankly, hate your guts!!\n");
  } else if (deathrate > 3 || acresPerson < 10) {
    printf("Your performance could have been somewhat better, but\n");
    printf("really wasn't too bad at all. ");
    int num = fnr(80) * population / 100;
    printf("%d people\n", num);
    printf("dearly like to see you assassinated but we all have our\n");
    printf("trivial problems.\n");
  } else {
    printf("A fantastic performance!!!  Charlemagne, Disraeli, and\n");
    printf("Jefferson combined could not have done better!\n");
  }

  end:
  printf("\n");
  // original rang bell 10 times
  printf("So long for now.\n");
  return;
}

int main() {
  fputc_cons(12);	// Clear Screen
  tab(32);printf("Hamurabi\n");
  tab(15);printf("Creative Computing  Morristown, New Jersey\n");
  printf("\n\n\n");
  printf("Try your hand at governing ancient Sumeria\n");
  printf("for a ten-year term of office.\n");

  game();
  
  return 0;
}
