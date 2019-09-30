#ifndef _VECTORS_GRAPH_LIB_
#define _VECTORS_GRAPH_LIB_

#include "./vectors_list.h"

typedef struct VECTOR_GRAPH {
  int buffer_size;
  int num_nodes;
  vlist** nodes;
} vgraph;

vgraph* create_vgraph();
int vg_has_node(vgraph* vg, vec v);
void vg_add_buffer(vgraph* vg, int add);
void print_vgraph(vgraph* vg);
void vg_add_node(vgraph* vg, vec v);
void vg_add_nodes(vgraph* vg, vlist* vl);
void vg_add_edge(vgraph* vg, vec v1, vec v2);
void vg_del_node(vgraph* vg, vec v);
void vg_free(vgraph* vg);

#endif