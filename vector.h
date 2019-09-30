#ifndef _VECTOR_LIB_
#define _VECTOR_LIB_

typedef struct VECTOR {
  short x;
  short y;
} vec;

int v_equal(vec v1, vec v2);
int vec_in_area(vec v, vec v1, vec v2);
vec v(int x, int y);
/////////////////
int between(int x, int min, int max);

#endif

