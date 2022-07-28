
/*
 * Fractal tree
 * zcc +zx -lndos -create-app -lm fractal_tree.c
 */


#include <graphics.h>
#include <stdio.h>
#include <stdlib.h> // for random()
#include <math.h>
//#include <time.h>


#define ROTATION_SCALE 0.75  // determines how slowly the angle between branches shrinks (higher value means slower shrinking)

#define SCALE          50     // determines how quickly branches shrink (higher value means faster shrinking)
#define BRANCHES       8    // number of branches
#define INITIAL_LENGTH 20    // length of first branch

 
double rand_fl(){
  return (double)rand() / (double)RAND_MAX;
}
 
void draw_tree(double offsetx, double offsety,
               double directionx, double directiony, double size,
               double rotation, int depth) {
  draw(
      (int)offsetx, (int)offsety,
      (int)(offsetx + directionx * size), (int)(offsety + directiony * size));

  if (depth > 0){
    // draw left branch
    draw_tree(
        offsetx + directionx * size,
        offsety + directiony * size,
        directionx * cos(rotation) + directiony * sin(rotation),
        directionx * -sin(rotation) + directiony * cos(rotation),
        size * rand_fl() / SCALE + size * (SCALE - 1) / SCALE,
        rotation * ROTATION_SCALE,
        depth - 1);
 
    // draw right branch
    draw_tree(
        offsetx + directionx * size,
        offsety + directiony * size,
        directionx * cos(-rotation) + directiony * sin(-rotation),
        directionx * -sin(-rotation) + directiony * cos(-rotation),
        size * rand_fl() / SCALE + size * (SCALE - 1) / SCALE,
        rotation * ROTATION_SCALE,
        depth - 1);
  }
}
 
 
int main(){
 
  //srand((unsigned)time(NULL));
 
  clg();
 
  draw_tree(
      getmaxx() / 2.0,
      getmaxy() - 10.0,
      0.0, -1.0,
      INITIAL_LENGTH,
      M_PI / 8,
      BRANCHES);

  return 0;
}

