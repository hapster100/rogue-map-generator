#include <stdlib.h>
#include <time.h>

#include "./level.h"
#include "./vectors_list.h"

void separate(level* l, int x, int y, int h, int w) {
  if( h < MIN_ROOM_HEIGTH || w < MIN_ROOM_WIDTH) return;
  add_room(l, x, y, h, w);
  if(h > w) {
    if(h < MIN_ROOM_HEIGTH*3) return;
    int r = rand()%h;
    int edge = (h/2 + r)/2;
    separate(l, x, y, edge, w);
    separate(l, x+edge-1, y, h-edge+1, w);
  } else {
    if(w < MIN_ROOM_WIDTH*3) return;
    int r = rand()%w;
    int edge = (w/2 + r)/2;
    separate(l, x, y, h, edge);
    separate(l, x, y+edge-1, h, w-edge+1);
  }
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

level* copy_level(level* l) {
  level* copy = create_empty_level(l->heigth, l->width);
  for (int i = 0; i < l->heigth * l->width; i++) {
    copy->map[i] = l->map[i];
  }
  return copy;
}

int between(int x, int min, int max) {
  if(x>=min && x<=max) return 1;
  return 0;
}

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

void label_rooms(level* l) {
  char label = '0';
  while(count_level_xy(l, FLOOR) > 0) {
    vec v = find_level_xy(l, FLOOR);
    vec beg = find_room_begin(l, v);
    vec end = find_room_end(l, beg);
    int h = end.x - beg.x; 
    int w = end.y - beg.y;
    fill_level_area(l, beg.x+1, beg.y+1, h-1, w-1, label);
    if(label == '9') label = '0';
    else label++;
  }
}

typedef struct VECTOR_GRAPH {
  int buffer_size;
  int num_nodes;
  vlist** nodes;
} vgraph;

vgraph* create_vgraph() {
  vgraph* vg = malloc(sizeof(vgraph));
  vg->num_nodes = 0;
  vg->buffer_size = 0;
  vg->nodes = NULL;
  return vg;
}

int vg_has_node(vgraph* vg, vec v) {
  for (int i = 0; i < vg->num_nodes; i++) {
    vec node = *(vg->nodes[i]->val);
    if(v_equal(v, node)) return 1;
  }
  return 0;
}

void vg_add_buffer(vgraph* vg, int add) {
  int new_buffer_size = vg->buffer_size + add;
  vlist** resize_nodes = malloc(sizeof(vlist*) * new_buffer_size);
  for (int i = 0; i < vg->num_nodes; i++) {
    resize_nodes[i] = vg->nodes[i];
  }
  free(vg->nodes);
  vg->buffer_size = new_buffer_size;
  vg->nodes = resize_nodes;
}

void print_vgraph(vgraph* vg) {
  printf("num nodes: %d\n", vg->num_nodes);
  printf("buffer_size: %d\n", vg->buffer_size);
  for (int i = 0; i < vg->num_nodes; i++) {
    printf("%d: ", i);
    print_vlist(vg->nodes[i]);
  }
}

void vg_add_node(vgraph* vg, vec v) {
  if(vg_has_node(vg, v)) return;
  
  if(vg->buffer_size <= vg->num_nodes) {
    vg_add_buffer(vg, 10);
  }
  vlist* new_node = create_vlist();
  vl_push(new_node, v.x, v.y);
  vg->nodes[vg->num_nodes] = new_node;
  vg->num_nodes++;
}

void vg_add_nodes(vgraph* vg, vlist* vl) {
  if(!vl) return;
  if(vl->val) vg_add_node(vg, *(vl->val));
  if(vl->next) vg_add_nodes(vg, vl->next);
}

void vg_add_edge(vgraph* vg, vec v1, vec v2) {
  if(!vg_has_node(vg, v1) || !vg_has_node(vg, v2)) return;
  vlist* nod = NULL;
  for (int i = 0; i < vg->num_nodes; i++) {
    vlist* vl = vg->nodes[i];
    vec v = *(vl->val);
    if(v_equal(v, v1)) {
      if(vl_has(vl, v2)) return;
      vl_push(vg->nodes[i], v2.x, v2.y);
      break;
    }
  }
  vg_add_edge(vg, v2, v1);
}

void vg_del_node(vgraph* vg, vec v) {
  int nn = vg->num_nodes;
  for(int i = 0; i < nn; i++) {
    vlist* node = vg->nodes[i];
    if(!v_equal(v, *(node->val)) && vl_has(node, v)) {
      vl_del(node, v, NULL);
    }
  }

  for(int i = 0; i < nn-1; i++) {
    vlist* tmp = vg->nodes[i];
    if(v_equal(*(tmp->val), v)) {
      vg->nodes[i] = vg->nodes[nn-1];
      vg->nodes[nn-1] = tmp;
      break;
    }
  }

  while(vg->nodes[nn-1]->next) {
    vl_del(vg->nodes[nn-1], *(vg->nodes[nn-1]->next->val), NULL);
  }
  free(vg->nodes[nn-1]);
  vg->nodes[nn-1] = NULL;
  vg->num_nodes--;
}

int vec_in_area(vec v, vec a_beg, vec a_end) {
  return between(v.x, a_beg.x, a_end.x) && between(v.y, a_beg.y, a_end.y);
}

int rooms_has_conect(level* l, vec v1, vec v2) {
  vec v1_e = find_room_end(l, v1);
  vec v2_e = find_room_end(l, v2);
  int v1_h = v1_e.x - v1.x;
  int v1_w = v1_e.y - v1.y;
  int v1_walls_num = (v1_h + v1_w) * 2;
  vec* v1_walls = malloc(sizeof(vec) * v1_walls_num);
  for (int i = 0; i < v1_walls_num/2; i++) {
    if(between(i, 0, v1_w)) {
      v1_walls[i] = v(v1.x, v1.y + i);
      v1_walls[v1_walls_num/2 + i] = v(v1_e.x, v1.y + i);
    } else {
      v1_walls[i] = v(v1.x + i - v1_w, v1_e.y);
      v1_walls[v1_walls_num/2 + i] = v(v1.x + i - v1_w, v1.y);
    }
  }

  for(int i = 0; i < v1_walls_num; i++) {
    if(vec_in_area(v1_walls[i], v2, v2_e)) {
      free(v1_walls);
      return 1;
    } 
  }

  free(v1_walls);

  return 0;
}

void add_rooms_conects(level* l, vgraph* vg) {
  int nn = vg->num_nodes;
  for (int i = 0; i < nn; i++) {
    vec room1 = *(vg->nodes[i]->val);
    for(int j = i+1; j < nn; j++) {
      vec room2 = *(vg->nodes[j]->val);
      if(rooms_has_conect(l, room1, room2)) vg_add_edge(vg, room1, room2);
    }
  }
  
}

int has_conected_rooms(vgraph* vg) {
  for(int i = 0; i < vg->num_nodes; i++) {
    if(vg->nodes[i]->next) return 1;
  }
  return 0;
}

void delete_conected_rooms(level* l, vgraph* vg) {
  while(has_conected_rooms(vg)) {

    int r1 = rand()%vg->num_nodes;
    int r2 = rand()%vg->num_nodes;
    int i = (r1+r2)/2;
    vlist* room_conections = vg->nodes[i];


    while(!room_conections->next) {
      room_conections = vg->nodes[(++i)%vg->num_nodes];
    }
    
    while(room_conections->next) {
      vec del_beg = *(room_conections->next->val);
      vec del_end = find_room_end(l, del_beg);
      vg_del_node(vg, del_beg);
      int x = del_beg.x + 1;
      int y = del_beg.y + 1;
      int h = del_end.x - x;
      int w = del_end.y - y;
      fill_level_area(l, x, y, h, w, EMPTY);
    }
  }
}

void add_map_rooms(level* l, level* map, vgraph* vg) {
  for (int i = 0; i < vg->num_nodes; i++) {
    vec beg = *vg->nodes[i]->val;
    vec end = find_room_end(map, beg);
    int x = beg.x;
    int y = beg.y;
    int h = end.x - x + 1;
    int w = end.y - y + 1;
    while(h > 1.5 * w) {
      h--;
      if(rand()%100 > 50) {
        x++;
      }
    }
    while(w > 1.5 * h) {
      w--;
      if(rand()%100 > 50) {
        y++;
      }
    }
    add_room(l, x, y, h, w);
  }
  
}

void delete_small_rooms(level* l, vlist* vl) {
  vlist* for_del = create_vlist();
  vlist* list_ptr = vl;
  while(list_ptr) {
    vec beg = *(list_ptr->val);
    vec end = find_room_end(l, beg);
    int h = end.x - beg.x + 1;
    int w = end.y - beg.y + 1;
    if(h < MIN_ROOM_HEIGTH || w < MIN_ROOM_WIDTH) {
      vl_push(for_del, beg.x, beg.y);
    }
    list_ptr = list_ptr->next;
  }
  while(for_del && for_del->val) {
    vec v = *for_del->val;
    vl_del(for_del, v, NULL);
    vl_del(vl, v, NULL);
  }
}

void delete_bad_rooms(level* l) {
  level* room_map = copy_level(l);
  // label_rooms(room_map);
  
  vlist* rooms_beg = find_all_room_begin(room_map);
  delete_small_rooms(room_map, rooms_beg);

  vgraph* rooms_graph = create_vgraph();
  vg_add_nodes(rooms_graph, rooms_beg);

  add_rooms_conects(room_map, rooms_graph);
  delete_conected_rooms(room_map, rooms_graph);

  fill_level(l, EMPTY);

  add_map_rooms(l, room_map, rooms_graph);
}

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
    if(!vec_in_area(wv, v(4, 4), v(l->heigth-5, l->width-5))) continue;
    if(count_level_area_xy(l, wv.x-1, wv.y-1, 3, 3, FLOOR) != 3) continue;
    set_lvl_xy(l, wv.x, wv.y, 'D');
  }
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
  int* w = malloc(sizeof(int) * 6);
  for (int i = 0; i < l->heigth; i++)
  {
    for (int j = 0; j < l->width; j++)
    {
      int item = get_lvl_xy(l, i, j);
      if(item != EMPTY) continue;

      for (int r = 0; r < 6; r++) {
        w[r] = count_in_rad(l, i, j, r+1, WALL);
      }
      
      if(!w[0] && w[1]) set_lvl_xy(l, i, j, SMALL_TUBE);
      if(!w[2] && w[3]) set_lvl_xy(l, i, j, TUBE);
      if(!w[4] && w[5]) set_lvl_xy(l, i, j, LARGE_TUBE);
      if((!w[1] && w[2]) || (!w[3] && w[4])) set_lvl_xy(l,i,j,WATER);

    }
    
  }
  free(w);

}

int get_way_cost(char c) {
  switch (c)
  {
  case HALLWAY:
    return 1;
    break;
  case FLOOR:
    return 64;
    break;
  case DOOR:
    return 64;
    break;
  case LARGE_TUBE:
    return 2;
    break;
  case TUBE:
    return 4;
    break;
  case SMALL_TUBE:
    return 8;
    break;
  case WATER:
    return 32;
    break;
  case EMPTY:
    return 128;
    break;
  default:
    return 1024*1024;
    break;
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

void add_cands(level* l, vlist* cs, int* costs, vlist* opens, vec base) {
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
    
    if(!vl_has(opens, cand) && get_lvl_xy(l, cand.x, cand.y) != WALL) {
      int way_cost = get_way_cost(get_lvl_xy(l, cand.x, cand.y));
      
      if(vl_has(cs, cand)) 
      {  
        int cand_cost = get_cost(cand, costs, l->width);
        if(cand_cost > way_cost + base_cost) {
          costs[i_from_xy(cand.x, cand.y, l->width)] = way_cost+base_cost;
        }

      } else {
        vl_push(cs, cand.x, cand.y);
        set_cost(costs, cand, l->width, way_cost+base_cost);
      }

    }
    vl_del(pos, cand, NULL);
  }

}

void build_way(level* l, vec start, vec end) {
  int lh = l->heigth;
  int lw = l->width;
  
  int* costs = malloc(lh*lw*sizeof(int));
  vlist* opens = create_vlist();
  vlist* cands = create_vlist(); 
  
  vl_push(opens, start.x, start.y);
  costs[i_from_xy(start.x, start.y, lw)] = 0;
  add_cands(l, cands, costs, opens, start);

  while(!vl_has(opens, end)) {
    vec to_open = get_min_cand(cands, costs, lw);
    vl_push(opens, to_open.x, to_open.y);
    vl_del(cands, to_open, NULL);
    add_cands(l, cands, costs, opens, to_open);
  }
  
  vlist* way = create_vlist();
  vl_push(way, end.x, end.y);
  vl_del(opens, end, NULL);
  vec lb = v(0,0);
  vec le = v(l->heigth-1, l->width-1);

  vec last = end;
  while (vl_has(opens, start))
  {
    vlist* pos = create_vlist();

    vec v_u = v(last.x+1, last.y);
    vec v_d = v(last.x-1, last.y);
    vec v_r = v(last.x, last.y+1);
    vec v_l = v(last.x, last.y-1);
    if(vec_in_area(v_u, lb, le) && vl_has(opens, v_u)) 
    {
      vl_push(pos, v_u.x, v_u.y);
    }
    if(vec_in_area(v_d, lb, le) && vl_has(opens, v_d)) 
    {
      vl_push(pos, v_d.x, v_d.y);
    }
    if(vec_in_area(v_r, lb, le) && vl_has(opens, v_r)) 
    {
      vl_push(pos, v_r.x, v_r.y);
    }
    if(vec_in_area(v_l, lb, le) && vl_has(opens, v_l)) 
    {
      vl_push(pos, v_l.x, v_l.y);
    }
    
    vec next = get_min_cand(pos, costs, lw);

    vl_push(way, next.x, next.y);
    vl_del(opens, next, NULL);
    last = next;
  }

  while(way->val) {
    vec v = *way->val;
    char item = get_lvl_xy(l, v.x, v.y);
    if(item != FLOOR && item != DOOR) {
      set_lvl_xy(l, v.x, v.y, HALLWAY);
    }

    vl_del(way, v, NULL);
  } 
  

  free(costs);
}

void add_ways(level* l) {
  vlist* room_list = find_all_room_begin(l);
  while(room_list->val) {
    vec v = *room_list->val;
    set_doors(l ,v);
    vl_del(room_list, v, NULL);
  }
  delete_doors(l);
  set_tube(l);

  vlist* beg = find_all_room_begin(l);
  vlist* p = beg;

  while (p && p->val)
  {
    p->val->x++;
    p->val->y++;
    p = p->next;
  }
  while(beg && beg->val && beg->next && beg->next->val) {
    build_way(l, *beg->val, *beg->next->val);
    vl_del(beg, *beg->val, NULL);
  }
  

}

void main() {
  int heigth = 40;
  int width = 130;
  srand(time(0));
  level* lvl = create_empty_level(heigth, width);
  separate(lvl, 0, 0, heigth, width);
  delete_bad_rooms(lvl);
  add_ways(lvl);

  print_level(lvl);
}
