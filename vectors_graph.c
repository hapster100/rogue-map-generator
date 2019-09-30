#include "./vectors_graph.h"

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
      vl_push_end(vg->nodes[i], v2);
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
  vl_free(vg->nodes[nn-1]);
  vg->num_nodes--;
}

void vg_free(vgraph* vg) {
  for (int i = 0; i < vg->num_nodes; i++) {
    vl_free(vg->nodes[i]);
  }
  free(vg->nodes);
  free(vg);
}