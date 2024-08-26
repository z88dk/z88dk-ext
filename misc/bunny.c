// Bunny
// Basic Computer Games
// Workman Publishing
// Converted to C by Michael Stroucken

// cc -o bunny bunny.c

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Draws an ASCII art rendering of a men's magazine's bunny logo
// Not much of a game, but the picture encoding is interesting

int data1[] = { 2, 21, 14, 14, 25 }; // BUNNY ascii offsets

// -1 = newline, 4096 = end, otherwise pairs of start and end
// end horizontal position
int data2[] = {
  1, 2, -1, 0, 2, 45, 50, -1, 0, 5, 43, 52, -1, 0, 7, 41, 52, -1,
  1, 9, 37, 50, -1, 2, 11, 36, 50, -1, 3, 13, 34, 49, -1, 4, 14, 32, 48, -1,
  5, 15, 31, 47, -1, 6, 16, 30, 45, -1, 7, 17, 29, 44, -1, 8, 19, 28, 43, -1,
  9, 20, 27, 41, -1, 10, 21, 26, 40, -1, 11, 22, 25, 38, -1, 12, 22, 24, 36, -1,
  13, 34, -1, 14, 33, -1, 15, 31, -1, 17, 29, -1, 18, 27, -1,
  19, 26, -1, 16, 28, -1, 13, 30, -1, 11, 31, -1, 10, 32, -1,
  8, 33, -1, 7, 34, -1, 6, 13, 16, 34, -1, 5, 12, 16, 35, -1,
  4, 12, 16, 35, -1, 3, 12, 15, 35, -1, 2, 35, -1, 1, 35, -1,
  2, 34, -1, 3, 34, -1, 4, 33, -1, 6, 33, -1, 10, 32, 34, 34, -1,
  14, 17, 19, 25, 28, 31, 35, 35, -1, 15, 19, 23, 30, 36, 36, -1,
  14, 18, 21, 21, 24, 30, 37, 37, -1, 13, 18, 23, 29, 33, 38, -1,
  12, 29, 31, 33, -1, 11, 13, 17, 17, 19, 19, 22, 22, 24, 31, -1,
  10, 11, 17, 18, 22, 22, 24, 24, 29, 29, -1,
  22, 23, 26, 29, -1, 27, 29, -1, 28, 29, -1, 4096};

void tab(int num) {
  int i;
  for (i=0; i<num; i++) {
    printf(" ");
  }
  return;
}

void game() {
  // "Bunny" from Ahl's 'Basic Computer Games'
  printf("\n");

  int index = 0;
  int pos = 0;
  int x, y;
  loop:
  x = data2[index++];
  if (x < 0) {
    printf("\n");
    pos = 0;
  } else if (x > 128) {
    goto end;
  } else {
    tab(x - pos);
    pos = x;
    y = data2[index++];
    for (int i = x; i <= y; i ++) {
      int j = i - 5 * (i / 5);
      printf("%c", 64 + data1[j]);
      pos++;
    }
  }
  goto loop;

  end:
  printf("\n\n\n\n\n\n");
  return;
}
    
  
  

int main() {
  tab(33);printf("Bunny\n");
  tab(15);printf("Creative Computing  Morristown, New Jersey\n");
  printf("\n\n\n");

  game();
  
  return 0;
}
