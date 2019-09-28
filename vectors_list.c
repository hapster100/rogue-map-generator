#include "./vectors_list.h"

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

void vl_del(vlist* vl, vec v, vlist* prev) {
  if(v_equal(v, *(vl->val))) {
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
    printf("->");
    print_vlist(vl->next);
  } else {
    printf("\n");
  }
}

int v_equal(vec v1, vec v2) {
  return v1.x == v2.x && v1.y == v2.y;
}

int vl_has(vlist* vl, vec v) {
  if(!vl->val) return 0;
  else if(v_equal(*(vl->val), v)) return 1;
  if(!vl->next) return 0;
  else return vl_has(vl->next, v);
}

vlist* vl_copy(vlist* vl) {
  vlist* copy = create_vlist();
  while(vl) {
    vec v = *vl->val;
    vl_push(copy, v.x, v.y);
    vl = vl->next;
  }
  return copy;
}