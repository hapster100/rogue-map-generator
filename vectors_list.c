#include "./vectors_list.h"

vlist* create_vlist() {
  vlist* vl = malloc(sizeof(vlist));
  vl->val = NULL;
  vl->next = NULL;
  return vl;
}

void vl_push_end(vlist* vl, vec v) {
  if(vl->val) {
    if(!vl->next) vl->next = create_vlist();
    vl_push_end(vl->next, v);
  } else {
    vl->val = malloc(sizeof(vec));
    vl->val->x = v.x;
    vl->val->y = v.y;
  }
}

void vl_push(vlist* vl, int x, int y) {
  
  if(vl->val) {
    vlist* new = create_vlist();
    new->val = vl->val;
    new->next = vl->next;
    vl->next = new;
    vl->val = malloc(sizeof(vec));
    vl->val->x = x;
    vl->val->y = y;
  } else {
    vl->val = malloc(sizeof(vec));
    vl->val->x = x;
    vl->val->y = y;
  }
}

void vl_del(vlist* vl, vec v, vlist* prev) {
  if(v_equal(v, *vl->val)) {
    free(vl->val);
    vl->val = NULL;
    if(prev) {
      prev->next = vl->next;
      free(vl);
    } else if(vl->next){
      vlist* next = vl->next->next;
      vl->val = vl->next->val;
      free(vl->next);
      vl->next = next;
    }
  } else if(vl->next){
    vl_del(vl->next, v, vl);
  }
}

void print_vlist(vlist* vl) {
  if(vl->val) printf("(%d, %d)", vl->val->x, vl->val->y);
  if(vl->next) {
    printf("->");
    print_vlist(vl->next);
  } else {
    printf("\n");
  }
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

void vl_free(vlist* vl) {
  if(vl->next) vl_free(vl->next);
  if(vl->val) free(vl->val);
  free(vl);
}