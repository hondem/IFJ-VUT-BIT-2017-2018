#ifndef ORIENTEDGRAPH_H
#define ORIENTEDGRAPH_H

#include <stdlib.h>
#include "set_int.h"

typedef struct {
    unsigned int id;
    SetInt* out_edges;
    SetInt* in_edges;
} GraphNodeBase;

typedef void(*oriented_graph_init_data_callback_f)(GraphNodeBase*);
typedef void(*oriented_graph_free_data_callback_f)(GraphNodeBase*);

typedef struct {
    size_t capacity;
    size_t nodes_count;
    size_t item_size;
    GraphNodeBase** nodes;
    oriented_graph_init_data_callback_f init_data_callback;
    oriented_graph_free_data_callback_f free_data_callback;
} OrientedGraph;

OrientedGraph* oriented_graph_init(size_t item_size, oriented_graph_init_data_callback_f init_callback, oriented_graph_free_data_callback_f free_callback);
void oriented_graph_free(OrientedGraph** graph);

GraphNodeBase* oriented_graph_new_node(OrientedGraph* graph);
void oriented_graph_remove_node(OrientedGraph* graph, unsigned int node_id);

GraphNodeBase* oriented_graph_node(OrientedGraph* graph, unsigned int node_id);
void oriented_graph_connect_nodes(OrientedGraph* graph, GraphNodeBase* from, GraphNodeBase* to);

void oriented_graph_connect_nodes_by_ids(OrientedGraph* graph, unsigned int from, unsigned int to);

void oriented_graph_clear(OrientedGraph* graph);

void oriented_graph_print(OrientedGraph* graph);

bool oriented_graph_node_is_in_cycle_by_id(OrientedGraph* graph, unsigned int id);
bool oriented_graph_node_is_in_cycle(OrientedGraph* graph, GraphNodeBase* node);
void _oriented_graph_expand_nodes(OrientedGraph* graph, SetInt* layer);


#endif // ORIENTEDGRAPH_H
