#include "room_level_tools.h"

vec find_room_begin(level* l, vec v) {
  vec beg = v;
  while (get_lvl_xy(l, beg.x - 1, beg.y) != WALL && get_lvl_xy(l, beg.x - 1, beg.y) != DOOR) {
    beg.x--;
    if(beg.x <= 0) return v;
  }
  while (get_lvl_xy(l, beg.x, beg.y) != WALL) {
    beg.y--;
    if(beg.y < 0) return v; 
  }
  beg.x--;
  return beg;
}

vec find_room_end(level* l, vec begin) {
  vec end = begin;
  end.x++; end.y++;
  while(get_lvl_xy(l, end.x + 1, end.y) != WALL) {
    if(end.x < l->heigth) end.x++;
    else return begin;
  }
  while(get_lvl_xy(l, end.x, end.y) != WALL) {
    if(end.y < l->width) end.y++;
    else return begin;
  }
  end.x++;
  return end;
}

vec v(int x, int y) {
  vec v = {x: x, y: y};
  return v;
}

vlist* find_all_room_begin(level* l) {
  
  vlist* vl = create_vlist();
  
  for (int i = 0; i < l->heigth; i++){
    for (int j = 0; j < l->width; j++){
      char item = get_lvl_xy(l, i, j);
      if(item != FLOOR) continue;
      vec beg = find_room_begin(l, v(i, j));
      if(!v_equal(beg, v(i, j)) && !vl_has(vl, beg)) vl_push(vl, beg.x, beg.y);
    }
  }
  return vl;
}

vec find_level_xy(level* l, char item) {
  for(int i = 0; i < l->heigth; i++) {
    for(int j = 0; j < l->width; j++) {
      if(get_lvl_xy(l, i, j) == item) {
        vec v = {x:i, y:j};
        return v;
      }
    }
  }
}