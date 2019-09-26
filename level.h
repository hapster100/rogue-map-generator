#ifndef _LEVEL_LIB_
#define _LEVEL_LIB_

#include <stdio.h>
#include <malloc.h>

#define EMPTY ' '
#define WALL '#'
#define FLOOR '.'

#define MIN_ROOM_WIDTH 5
#define MIN_ROOM_HEIGTH 5

typedef struct LEVEL {
  
  //   x   map
  // h |.........
  // e |.........
  // i |.........
  // g |.........
  // t |.........
  // h +---------y
  //   w i d t h
  
  int heigth;
  int width;
  char* map;

} level;

level* create_empty_level(int h, int w);
void print_level(level* lvl);
void set_lvl_xy(level* lvl, int x, int y, char c);
char get_lvl_xy(level* lvl, int x, int y);
void add_room(level* l, int x, int y, int h, int w);
void fill_level(level* l, char filler);
void fill_level_area(level* l, int x, int y, int h, int w, char filler);
int count_level_xy(level* l, char item);

#endif