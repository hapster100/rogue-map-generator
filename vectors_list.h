#ifndef _VECTOR_LIST_LIB_
#define _VECTOR_LIST_LIB_

#include <stdio.h>
#include <malloc.h> 

typedef struct VECTOR {
  int x;
  int y;
} vec;

typedef struct VEC_LIST {
  vec* val;
  struct VEC_LIST* next;
} vlist;

vlist* create_vlist();
vlist* vl_copy(vlist* vl);
void vl_push(vlist* vl, int x, int y);
void vl_del(vlist* vl, vec v, vlist* prev);
void print_vlist(vlist* vl);
int vl_has(vlist* vl, vec v);
int v_equal(vec v1, vec v2);

#endif