#include <stdlib.h>
#include <time.h>

#include "./level.h"

typedef struct VECTOR {
  int x;
  int y;
} vec;

typedef struct VEC_LIST {
  vec* val;
  struct VEC_LIST* next;
} vlist;

vlist* create_vlist() {
  vlist* vl = malloc(sizeof(vlist));
  vl->val = NULL;
  vl->next = NULL;
}

void vl_push(vlist* vl, int x, int y) {
  if(vl->val) {
    if(!vl->next) vl->next = create_vlist();
    vl_push(vl->next, x, y);
  } else {
    vl->val = malloc(sizeof(vec));
    vl->val->x = x;
    vl->val->y = y;
  }
}

void vl_del(vlist* vl, vec* v, vlist* prev) {
  printf("delete (%d %d)\n", v->x, v->y);
  if(vl->val == v) {
    free(vl->val);
    if(prev) {
      prev->next = vl->next;
      free(vl);
    } else if(vl->next) {
      vl->val = vl->next->val;
      vl->next = vl->next->next;
    } else {
      vl->val = NULL;
    }
  } else {
    vl_del(vl->next, v, vl);
  }
}

void print_vlist(vlist* vl) {
  if(vl->val) printf("(%d, %d)", vl->val->x, vl->val->y);
  if(vl->next && vl->next->val) {
    printf(" -> ");
    print_vlist(vl->next);
  } else {
    printf("\n");
  }
}

vec* vl_find(vlist* vl, int (*find_func)(vec*)) {
  if(vl->val) {
    if(find_func(vl->val)) {
      return vl->val;
    } else if(vl->next){
      return vl_find(vl->next, find_func);
    } else {
      return NULL;
    }
  } else {
    return NULL;
  }
}

vec* find_second_corner(vlist* vl, vec* first) {
  vec* second = NULL;

  int find(vec* v) {
    int h = v->x - first->x;
    int w = v->y - first->y;
    if(h < 0) h*=-1;
    if(w < 0) w*=-1;
    if(h >= MIN_ROOM_HEIGTH && w >= MIN_ROOM_WIDTH) return 1;
    return 0;
  }

  second = vl_find(vl, find);

  return second;
}

void separate(level* l, int x, int y, int h, int w) {
  if( h < MIN_ROOM_HEIGTH || w < MIN_ROOM_WIDTH) return;
  add_room(l, x, y, h, w);
  if(h > w) {
    if(h < MIN_ROOM_HEIGTH*3) return;
    int edge = MIN_ROOM_HEIGTH;
    for(int i=0; i < 3; i++) {
      int r = rand()%h;
      if(abs(h/2 - r) < abs(h/2 - edge)) edge = r;
    }
    separate(l, x, y, edge, w);
    separate(l, x+edge-1, y, h-edge+1, w);
  } else {
    if(w < MIN_ROOM_WIDTH*3) return;
    int edge = MIN_ROOM_WIDTH;
    for(int j=0; j < 3; j++) {
      int r = rand()%w;
      if(abs(w/2 - r) < abs(w/2 - edge)) edge = r;
    }
    separate(l, x, y, h, edge);
    separate(l, x, y+edge-1, h, w-edge+1);
  }
}

void empty_bad_rooms(level)

void main() {
  int heigth = 30;
  int width = 50;
  srand(time(0));
  level* lvl = create_empty_level(heigth, width);
  separate(lvl, 0, 0, heigth, width);
  empty_bad_rooms(lvl);
  print_level(lvl);
}
