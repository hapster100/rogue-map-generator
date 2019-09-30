#include "./room_generator.h"


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
  vl_free(for_del);
}

void generate_rooms(level* l) {
  separate(l, 0, 0, l->heigth, l->width);
  level* room_map = copy_level(l);
  
  vlist* rooms_beg = find_all_room_begin(room_map);
  delete_small_rooms(room_map, rooms_beg);

  vgraph* rooms_graph = create_vgraph();
  vg_add_nodes(rooms_graph, rooms_beg);

  add_rooms_conects(room_map, rooms_graph);
  delete_conected_rooms(room_map, rooms_graph);

  fill_level(l, EMPTY);

  add_map_rooms(l, room_map, rooms_graph);
  
  level_free(room_map);
  vl_free(rooms_beg);
  vg_free(rooms_graph);
}