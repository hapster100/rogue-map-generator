#ifndef _LEVEL_LIB_
#define _LEVEL_LIB_

#include <stdio.h>
#include <malloc.h>

#define EMPTY 'E'
#define WALL '#'
#define FLOOR 'F'
#define DOOR 'D'
#define TUBE '|'
#define SMALL_TUBE '-'
#define LARGE_TUBE '='
#define HALLWAY 'H'
#define WATER '~'
#define BRIDGE 'B'

#define XY(l, x, y) (get_lvl_xy((l),(x),(y)))

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
int count_level_area_xy(level* l, int x, int y, int h, int w, char item);
void level_free(level* l);
level* copy_level(level* l);

#endif