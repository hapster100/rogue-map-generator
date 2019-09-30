#include <time.h>

#include "./room_generator.h"

void set_doors(level* l, vec beg) {
  vec end = find_room_end(l, beg);
  int h = end.x - beg.x;
  int w = end.y - beg.y;
  int walln = (h + w) * 2;
  vec* walls = malloc(sizeof(vec) * walln);
  for (int i = 0; i < walln/2; i++) {
    if(between(i, 0, w)) {
      walls[i] = v(beg.x, beg.y + i);
      walls[walln/2 + i] = v(end.x, beg.y + i);
    } else {
      walls[i] = v(beg.x + i - w, end.y);
      walls[walln/2 + i] = v(beg.x + i - w, beg.y);
    }
  }
  for (int i = 0; i < walln; i++) {
    vec wv = walls[i];
    if(!vec_in_area(wv, v(1, 1), v(l->heigth-2, l->width-2))) continue;
    if(count_level_area_xy(l, wv.x-1, wv.y-1, 3, 3, FLOOR) != 3) continue;
    set_lvl_xy(l, wv.x, wv.y, 'D');
  }
  free(walls);
}

vec* get_xy_array(level* l, int num, char item) {
  int count = 0;
  vec* arr = malloc(sizeof(vec) * num);
  for (int i = 0; i < l->heigth; i++) {
    for(int j = 0; j < l->width; j++) {
      if(get_lvl_xy(l, i, j) == item) arr[count++] = v(i, j);
    }
  }
  return arr;
}

void delete_doors(level* l) {
  for (int i = 1; i < l->heigth - 2; i++) {
    for(int j = 1; j < l->width - 2; j++) {
      while(count_level_area_xy(l, i, j, 3, 3, DOOR) > 1) {
        int dx = rand()%3;
        int dy = rand()%3;
        while(get_lvl_xy(l, i + dx, j + dy) != DOOR) {
          dx++;
          if(dx == 3) {
            dx = 0;
            dy++;
            if(dy == 3) {
              dy = 0;
            }
          }
        }
        set_lvl_xy(l, i+dx, j+dy, WALL);
      }
    }
  }
}

int count_in_rad(level* l, int x, int y, int r, char item) {
  return count_level_area_xy(l, x-r, y-r, r*2+1, r*2+1, item);
}

void set_tube(level* l) {

  /*
   WALL WALL WALL WALL WALL 
   EMPTY EMPTY EMPTY EMPTY 0
   SMALL_TUBE SMALL_TUBE 1
   EMPTY EMPTY EMPTY EMPTY 2
   TUBE TUBE TUBE TUBE TUBE 3
   EMPTY EMPTY EMPTY EMPTY 4
   LARGE_TUBE LARGE_TUBE 5
   EMPTY EMPTY EMPTY EMPTY 6
  */
  int* w = malloc(sizeof(int) * 11);
  for (int i = 0; i < l->heigth; i++)
  {
    for (int j = 0; j < l->width; j++)
    {
      int item = get_lvl_xy(l, i, j);
      if(item != EMPTY) continue;

      for (int r = 0; r < 11; r++) {
        w[r] = count_in_rad(l, i, j, r+1, WALL);
      }

      int d =  count_level_area_xy(l, i-1, j, 3, 1, DOOR);
          d += count_level_area_xy(l, i, j-1, 1, 3, DOOR);
      
      if(w[0] == 2 && d) set_lvl_xy(l,i,j, SMALL_TUBE);
      if(!w[0] && w[1]) set_lvl_xy(l, i, j, SMALL_TUBE);
      if(!w[3] && w[4]) set_lvl_xy(l, i, j, TUBE);
      for (int k = 5; k <= 10; k+=2)
      {
        if(!w[k] && w[k+1]) set_lvl_xy(l, i, j, LARGE_TUBE);
      }
      if((!w[1] && w[2]) || (!w[2] && w[3])) set_lvl_xy(l,i,j,WATER);

    }
    
  }
  free(w);


}

int get_way_cost(char c) {
  switch (c)
  {
  case FLOOR: return 1;
  case HALLWAY: return 2;
  case DOOR: return 3;
  case BRIDGE: return 4;

  case LARGE_TUBE: return 30;
  case TUBE: return 20;
  case SMALL_TUBE: return 10;
  
  case WATER: return 100;
  
  case WALL: return 2000;
  case EMPTY: return 1000;
  
  default:
    return 1024*1024;
  }
}

int i_from_xy(int x, int y, int w) {
  return y + x*w;
}

int get_cost(vec v, int* costs, int lw) {
  return costs[i_from_xy(v.x, v.y, lw)];
}

void set_cost(int* costs, vec v, int lw, int cost) {
  costs[i_from_xy(v.x, v.y, lw)] = cost;
}

vec get_min_cand(vlist* cands, int* costs, int lw) {

  int min_cost = 1024*1024;
  vlist* p = cands;
  vec min_cand;
  while(p) {
    int cost = get_cost(*p->val, costs, lw);
    if (cost < min_cost)
    {
      min_cost = cost;
      min_cand = *p->val;
    }
    
    p = p->next;
  }
  return min_cand;
}

void add_cands(level* l, vlist* cs, int* costs, vlist* opens, vec base, vec end) {
  vec lb = v(0,0);
  vec le = v(l->heigth-1, l->width-1);

  int base_cost = get_cost(base, costs, l->width);

  vlist* pos = create_vlist();

  if(vec_in_area(v(base.x+1, base.y), lb, le)) vl_push(pos, base.x+1, base.y);
  if(vec_in_area(v(base.x-1, base.y), lb, le)) vl_push(pos, base.x-1, base.y);
  if(vec_in_area(v(base.x, base.y+1), lb, le)) vl_push(pos, base.x, base.y+1);
  if(vec_in_area(v(base.x, base.y-1), lb, le)) vl_push(pos, base.x, base.y-1);

  while(pos->val) {
    vec cand = *pos->val;
    
    if(!vl_has(opens, cand)) {
      int way_cost = get_way_cost(get_lvl_xy(l, cand.x, cand.y));
      int dist_cost = abs(cand.x-end.x) + abs(cand.y-end.y);

      int sum_cost = way_cost + base_cost + (dist_cost*5)/7;

      if(vl_has(cs, cand)) 
      {  
        int cand_cost = get_cost(cand, costs, l->width);
        if(cand_cost > sum_cost) {
          costs[i_from_xy(cand.x, cand.y, l->width)] = sum_cost;
        }

      } else {
        vl_push(cs, cand.x, cand.y);
        set_cost(costs, cand, l->width, sum_cost);
      }

    }
    vl_del(pos, cand, NULL);
  }

  vl_free(pos);

}

void build_way(level* l, vec start, vec end) {
  int lh = l->heigth;
  int lw = l->width;
  
  // ///////////////////////////////
  // level* opl = copy_level(l);
  // set_lvl_xy(opl, start.x, start.y, 'S');
  // set_lvl_xy(opl, end.x, end.y, 'E');
  // print_level(opl);
  // getchar();
  // /////////////////////////////
  int* costs = malloc(lh*lw*sizeof(int));
  for (int i = 0; i < lh*lw; i++)
  {
    costs[i] = 1024*1024*1024;
  }
  
  vlist* opens = create_vlist();
  vlist* cands = create_vlist(); 
  
  vl_push(opens, start.x, start.y);
  costs[i_from_xy(start.x, start.y, lw)] = 0;
  add_cands(l, cands, costs, opens, start, end);

  while(cands->val) {
    vec to_open = get_min_cand(cands, costs, lw);
    vl_push(opens, to_open.x, to_open.y);
    vl_del(cands, to_open, NULL);
    add_cands(l, cands, costs, opens, to_open, end);
    if(v_equal(to_open, end)) break;
  }
  
  vlist* way = create_vlist();
  vl_push(way, end.x, end.y);
  vl_del(opens, end, NULL);
  vec lb = v(0,0);
  vec le = v(l->heigth-1, l->width-1);

  vec last = end;
  while (opens->val)
  {
    vlist* pos = create_vlist();

    vec v_u = v(last.x+1, last.y);
    vec v_d = v(last.x-1, last.y);
    vec v_r = v(last.x, last.y+1);
    vec v_l = v(last.x, last.y-1);
    if(vec_in_area(v_u, lb, le)) 
    {
      vl_push(pos, v_u.x, v_u.y);
    }
    if(vec_in_area(v_d, lb, le)) 
    {
      vl_push(pos, v_d.x, v_d.y);
    }
    if(vec_in_area(v_r, lb, le)) 
    {
      vl_push(pos, v_r.x, v_r.y);
    }
    if(vec_in_area(v_l, lb, le)) 
    {
      vl_push(pos, v_l.x, v_l.y);
    }
    
    vec next = get_min_cand(pos, costs, lw);

    vl_push(way, next.x, next.y);
    vl_del(opens, next, NULL);
    vl_free(pos);
    if(v_equal(next, start)) break;
    last = next;
  }
  
  while(way->val) {
    vec v = *way->val;
    char item = get_lvl_xy(l, v.x, v.y);
    if (item == WATER)
    {
      set_lvl_xy(l, v.x, v.y, BRIDGE);
    }else if(item == WALL) {
      set_lvl_xy(l, v.x, v.y, DOOR);
    }else if(item != FLOOR && item != DOOR && item != BRIDGE) {
      set_lvl_xy(l, v.x, v.y, HALLWAY);
    }

    vl_del(way, v, NULL);
  } 
  

  free(costs);
  vl_free(cands);
  vl_free(opens);
  vl_free(way);
}

void add_ways(level* l) {
  vlist* room_list = find_all_room_begin(l);
  while(room_list->val) {
    vec v = *room_list->val;
    set_doors(l ,v);
    vl_del(room_list, v, NULL);
  }
  vl_free(room_list);


  delete_doors(l);
  set_tube(l);

  vlist* beg = find_all_room_begin(l);
  vlist* p = beg;

  while (p && p->val)
  {
    vec beg = *p->val;
    vec end = find_room_end(l, beg);
    p->val->x = (end.x + beg.x)/2;
    p->val->y = (end.y + beg.y)/2;
    p = p->next;
  }

  vlist* conected = create_vlist();
  vec first = *beg->val;
  vl_push(conected, first.x, first.y);
  vl_del(beg, first, NULL);

  while(beg->val) {
   
    int r = rand()%32; 
    vlist* beg_p = beg;
    while(r--)
      if(beg_p->next)
        beg_p = beg_p->next;
      else 
        beg_p = beg;

    vec to = *beg_p->val;
    int min_dist = l->heigth*l->width;
    vlist* p = conected;
    vec from = *p->val;
    while (p) {
      vec v = *p->val;
      int dist = abs(v.x - to.x) + abs(v.y - to.y);
      if(dist < min_dist) {
        min_dist = dist;
        from = v;
      }
      p = p->next;
    }
    build_way(l, from, to);
    vl_del(beg, to, NULL);
    vl_push(conected, to.x, to.y);
  }

  for (int i = 0; i < l->heigth; i++)
    for (int j = 0; j < l->width; j++)
      if (get_lvl_xy(l,i,j) == DOOR)
        if (!count_in_rad(l,i,j, 1, HALLWAY))
          set_lvl_xy(l,i,j, WALL);
  

  vl_free(beg);
  vl_free(conected);
}

void main() {
  srand(time(0));

  level* l = create_empty_level(50, 100);

  generate_rooms(l);
  add_ways(l);
  print_level(l);

  level_free(l);
}
