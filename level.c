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

void textColor(int r, int g, int b) {
  printf("\e[38;2;%d;%d;%dm", r, g, b);
}

void backColor(int r, int g, int b) {
  printf("\e[48;2;%d;%d;%dm", r, g, b);
}

void resetColor() {
  printf("\e[0m");
}

void print_level(level* lvl) {
  textColor(200, 200, 0);
  printf(" ");
  for(int i = 0; i < lvl->width; i++) {
    printf("_");
  }
  resetColor();
  printf("\n");
  textColor(200, 200, 0);
  backColor(0,0,0);
  printf("/");
  for(int i = 0; i < lvl->width; i++) {
    printf(" ");
  }
  printf("\\");
  resetColor();
  printf("\n");

  for(int i = 0; i < lvl->heigth; i++) {
    textColor(200, 200, 0);
    backColor(0,0,0);
    printf("|");
    resetColor();
    for(int j = 0; j < lvl->width; j++) {
      char c = lvl->map[i * lvl->width + j];
      switch (c) {
      case WALL:
        textColor(10, 10, 10);
        backColor(60, 30, 30);
        break;
      case FLOOR:
        textColor(30, 20, 20);
        backColor(120, 80, 80);
        break;
      case DOOR:
        textColor(20, 20, 20);
        backColor(100,50,0);
        break;
      case EMPTY:
        textColor(10, 10, 10);
        backColor(0,0,0); 
        break;
      case TUBE:
        textColor(70,70,10);
        backColor(0,0,0);
        break;
      case SMALL_TUBE:
        textColor(100,10,10);
        backColor(0,0,0);
        break;
      case LARGE_TUBE:
        textColor(10, 100, 10);
        backColor(0,0,0);
        break;
      case HALLWAY:
        textColor(40, 30, 30);
        backColor(60, 40, 40);
        break;
      case WATER:
        textColor(0, 10, 30);
        backColor(0, 0, 0);
        break;
      default:
        break;
      }
      printf("%c", c);
      resetColor();
    }
    textColor(200, 200, 0);
    backColor(0,0,0);
    printf("|");
    resetColor();
    printf("\n");
  }
  
  textColor(200, 200, 0);
  backColor(0,0,0);
  printf("\\");
  for(int i = 0; i < lvl->width; i++) {
    printf("_");
  }
  printf("/");
  resetColor();
  printf("\n");
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

int count_level_area_xy(level* l, int x, int y, int h, int w, char item) {
  int counter = 0;
  if(x >= l->heigth || y >= l->width) return 0;
  while(x < 0) {x++; h--;}
  while(y < 0) {y++; w--;}
  while(x+h > l->heigth) h--;
  while(y+w > l->width) w--;
  
  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
     if(get_lvl_xy(l, x+i, y+j) == item) counter++;
    }
  }
  return counter;
}