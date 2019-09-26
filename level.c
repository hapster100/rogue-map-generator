#include "./level.h"

level* create_empty_level(int h, int w) {
  level* new_lvl = malloc(sizeof(level));
  new_lvl->heigth = h;
  new_lvl->width = w;
  new_lvl->map = malloc(h*w);
  for(int i = 0; i < h*w; i++) {
    new_lvl->map[i] = EMPTY;
  }
  return new_lvl;
}

void print_level(level* lvl) {
  for(int i = 0; i < lvl->heigth; i++) {
    for(int j = 0; j < lvl->width; j++) {
      printf("%c", lvl->map[i * lvl->width + j]);
    }
    printf("\n");
  }
}

void set_lvl_xy(level* lvl, int x, int y, char c) {
  int h = lvl->heigth, w = lvl->width;
  if(h > x && w > y) {
    lvl->map[(h-x-1) * w + y] = c;
  } else {
    printf("error in set_lvl_xy\n");
  }
}

char get_lvl_xy(level* lvl, int x, int y) {
  int h = lvl->heigth, w = lvl->width;
  if(h > x && w > y) {
    return lvl->map[(h-x-1) * w + y];
  } else {
    printf("error in get_lvl_xy(%d, %d)\n", x, y);
  }
}

void add_room(level* l, int x, int y, int h, int w) {
  printf("Add room x: %d y: %d heigth: %d width %d\n", x, y, h, w);
  int lh = l->heigth, lw = l->width;
  if(lh >= x + h && lw >= y + w && x >= 0 && y >= 0) {
    for(int i = x; i < x + h; i++) {
      set_lvl_xy(l, i, y, WALL);
      set_lvl_xy(l, i, y + w - 1, WALL);
    }
    for(int i = y; i < y + w; i++) {
      set_lvl_xy(l, x, i, WALL);
      set_lvl_xy(l, x + h - 1, i, WALL);
    }
    fill_level_area(l, x+1, y+1, h-2, w-2, FLOOR);
  } else {
    printf("error in add_room\n");
  }
}

void fill_level(level* l, char filler) {
  for(int i = 0; i < l->heigth * l->width; i++) {
    l->map[i] = filler;
  }
}

void fill_level_area(level* l, int x, int y, int h, int w, char filler) {
  for(int i = x; i < x + h; i++) {
    for(int j = y; j < y + w; j++) {
      set_lvl_xy(l, i, j, filler);
    }
  }
}

int count_level_xy(level* l, char item) {
  int counter = 0;
  for(int i = 0; i < l->heigth * l->width; i++) {
    if(l->map[i] == item) counter++;
  }
  return counter;
}