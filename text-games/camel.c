// Camel
// Basic-Computer-Spiele Band 2
// Sybex Verlag
// Converted to C by Michael Stroucken
// Original by the Heath Users Group

// Porter's notes:
// I think the pygmies are far too strong and the game is almost
// impossible without adjustments.
//
// I tried to remove gameplay bugs, but I kept the particular
// misspellings from the original.
//
// Gobi Desert, camel, pygmies, sheiks? Knock-kneed?

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


char line[10];


struct game_state_t {
  int water_level;
  int drinks_left;
  int camel_distance;
  int pygmy_distance;
  int berber_mode;
  int camel_stress;
  int turn_count;
};

typedef struct game_state_t game_state;

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

void init_game(game_state* state) {
  state->water_level = 4;
  state->drinks_left = 6;
  state->camel_distance = 0;
  state->pygmy_distance = 0;
  state->berber_mode = 0;
  state->camel_stress = 0;
  state->turn_count = 0;
}

void die() {
  printf("You died in the desert.\n");
  int val = fnr(10);
  if (val <= 1) {
    printf("The national camel's union is not attending your funeral!!!\n");
  } else if (val <= 3) {
    printf("Your body was eaten by vultures and imported canninbals !!!\n");
  } else if (val <= 5) {
    printf("The local sheik now uses your skull for a change purse !!!\n");
  } else if (val <= 7) {
    printf("People with little intelligence should stay out of the desert\n");
  } else {
    printf("Turkeys should fly, not ride camels !!!!!!!\n");
  }
}

int berber(game_state* state) {
  printf("#7 Attempt an escape\n");
  printf("#8 Wait for payment\n");
  printf("Your sub-command ? \n");

  //char *line = NULL;
  //size_t linecap = 0;
  //getline(&line, &linecap, stdin);
  gets(line);

  int value = atoi(line);
  //free(line);

  if (value == 8) {
    int val = fnr(100);
    if (val < 25) {
      printf("Your ransom has been paid and you are free to go.\n");
      state->berber_mode = 0;
      return 0;
    }

    printf("The local sultan is collecting.......Just wait.......\n");
    return 0;
  }

  int val = fnr(10);
  if (val < 5) {
    printf("You were mortally wounded by a pig stabber while escaping.\n");
    die();
    return 1;
  }

  printf("Congradulations, you successfully escaped !!!!\n");
  state->berber_mode = 0;
  return 0;
}

void random_events(game_state* state) {
  int val = fnr(100);
  if (val <= 5) {
    printf("Wild Berbers hidden in the sand have captured you.\n");
    printf("Luckily the local sheik has agreed to their ransom-\n");
    printf("demands.......but........watch for the pygmies !!!\n");
    printf("You have a new choice of sub-commands\n");
    state->berber_mode = 1;
    return;
  }

  val = fnr(10);
  if (val <= 2) {
    printf("You have arrived at an oases--------Your camel is\n");
    printf("filling your canteen and eating figs.\n");
    state->water_level = 4;
    state->drinks_left = 6;
    return;
  }

  val = fnr(100);
  if (val <= 5) {
    printf("You have been caught in a sandstorm.....Good luck!\n");
    int distance = fnr(10);
    int direction = fnr(10);
    if (direction & 1) {
      state->camel_distance += distance;
    } else {
      state->camel_distance -= distance;
    }

    printf("Your new position is %d miles so far!\n", state->camel_distance);
    return;
  }

  val = fnr(100);
  if (val <= 5) {
    state->pygmy_distance++;
    printf("Your camel hurt his hump\n");
    printf("Luckily the pygmies were footweary !!!\n");
    return;
  }

  return;
}

void camel_dead() {
  printf("You dirty rapscallion! You ran your poor camel to death !!\n");
  die();
}

void game() {
  game_state state;

  printf("Good luck and good cameling !!\n");
  printf("You are in the middle of the desert at an oasis.\n");

  init_game(&state);

  while (1) {
    if (state.camel_distance > 199) {
      printf("You win, a party is being given in your honor.......\n");
      printf(".......The pygmies are planning to attend.......\n");
      return;
    }

    state.water_level--;
    if (state.water_level == 1) {
      printf("----------WARNING---------- Get a drink\n");
    }

    if (state.water_level < 0) {
      printf("You ran out of water...... Sorry chum!!!\n");
      return;
    }

    state.turn_count++;

    // pygmies are a bit faster than moderate camel speed
    //int pygmy_advance = (fnr(100)+25)/10;
	// Let's make them weaker
	int pygmy_advance = (fnr(100)+25)/20;

    // pygmies start after turn 3
    if (state.turn_count >= 4) {
      state.pygmy_distance += pygmy_advance;
    }

    // by source, the pygmies don't advance in Berber mode
    // by source, Berber mode is never set
    if (state.berber_mode) {
      if (berber(&state)) {
        return;
      }

      continue;
    }

    if (state.turn_count >= 4) {
      if (state.pygmy_distance > state.camel_distance) {
        printf("The pygmies have captured you.  Camel and people soup is\n");
        printf("their favorite dish !!!!!\n");
        return;
      }

      int separation = state.camel_distance - state.pygmy_distance;
      printf("The pygmies are %d miles behind you.\n", separation);
    }

    printf("You have travelled %d miles alltogether.\n", state.camel_distance);
    printf("What is your command? ");

    //char *line = NULL;
    //size_t linecap = 0;
    //getline(&line, &linecap, stdin);
    gets(line);

    int value = atoi(line);
    //free(line);

    // source didn't try to fix errors

    if (value == 1) {
      state.drinks_left--;
      if (state.drinks_left < 0) {
        die();
        return;
      }

      printf("Better watch for an oases !\n");
      state.water_level = 4 + 1; // +1 for the turn cost
      continue;
    } else if (value == 2) {
      state.camel_stress++;
      if (state.camel_stress == 8) {
        camel_dead();
        return;
      }

      // in source, game random events happened before movement
      int camel_advance = fnr(10);
      state.camel_distance += camel_advance;
      printf("Your camel likes this pace.\n");

      random_events(&state);
      continue;
    } else if (value == 3) {
      state.camel_stress += 3;
      if (state.camel_stress >= 8) {
        camel_dead();
        return;
      }

      // in source, game random events happened before movement
      int camel_advance = 2*fnr(10);
      state.camel_distance += camel_advance;
      printf("Your camel is burning across the desert sands.\n");

      random_events(&state);
      continue;
    } else if (value == 4) {
      printf("Your camel thanks you!\n");
      state.camel_stress = 0;
      continue;
    } else if (value == 5) {
      printf("Your camel has %d good days left.\n", 7 - state.camel_stress);
      printf("You have %d drinks left in your canteen.\n", state.drinks_left);
      printf("You can go %d commands without drinking.\n", state.water_level);
      // but pygmies still advance and water level decreases
      continue;
    } else {
      int chance = fnr(10);
      if (chance != 1) {
        die();
        return;
      }

      // only 10% chance to live
      printf("Help has found you in a state of unconsciousness.\n");
      state.drinks_left = 3;
      state.water_level = 4;
      continue;
    }
  }
}

int main() {
  //char *line = NULL;
  size_t linecap = 0;

  tab(26);printf("Camel\n");
  tab(20);printf("Creative Computing\n");
  tab(18);printf("Morristown, New Jersey\n");
  printf("\n\n\n");
  printf("Would you like instructions? ");

  //line = NULL;
  //linecap = 0;
  //getline(&line, &linecap, stdin);
  gets(line);

  srand(clock());
  //free(line);
  fputc_cons(12);   // Clear screen

  if ((line[0]=='y')||(line[0]=='Y')) {
    printf("Welcome to Camel. The object is to travel\n");
    printf("200 miles across the great Gobi Desert.\n");
    printf("A tribe of knock kneed pigmies will be chasing you.\n");
    printf("You will be asked for commands every so often.\n");
    printf("\n\n");
    printf("C O M M A N D S :\n");
    printf("#1 Drink from your canteen\n");
    printf("#2 Ahead moderate speed\n");
    printf("#3 Ahead full speed\n");
    printf("#4 Stop for the night\n");
    printf("#5 Status check\n");
    printf("#6 Hope for help\n");
    printf("\n\n");
    printf("You have one quart of water which will last you six drinks.\n");
    printf("You may renew your water supply completely at an oases.\n");
    printf("You get a half a quart if found by help.\n");
    printf("If help does not find you after command six, you lose.\n");
  }

  do {
    game();
    printf("\n\n");
    printf("Want a new camel and a new game ?\n");
    //line = NULL;
    //linecap = 0;
    //getline(&line, &linecap, stdin);
    gets(line);
    //free(line);
  } while ((line[0]=='y')||(line[0]=='Y'));

  printf("-----------------\n");
  printf("     CHICKEN\n");
  printf("-----------------\n");
  printf("\n\n");

  return 0;
}
