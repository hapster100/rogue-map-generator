#ifndef _ROOM_TOOLS_LIB_
#define _ROOM_TOOLS_LIB_

#include "./level.h"
#include "./vectors_graph.h"

vec find_room_begin(level* l, vec v);
vec find_room_end(level* l, vec begin);
vlist* find_all_room_begin(level* l);

#endif