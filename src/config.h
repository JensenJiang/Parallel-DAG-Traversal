//
// Created by JensenJiang on 2017/6/21.
//

#ifndef PARALLEL_DAG_TRAVERSAL_CONFIG_H
#define PARALLEL_DAG_TRAVERSAL_CONFIG_H
class Node {
public:
    int cost, fa_num, nid;
    int **top;
    int *top_len;
    Edge *first, *rfirst;

    Node() {
        this->fa_num = 0;
    }
};

class Edge {
public:
    Node *to;
    Edge *next;
};

class Graph {
public:
    // Edge *first[], *rfirst[];
    int nodes_size, I_size, O_size;
    int **pair_cost;
    Node **nodes;
    Node **I, **M, **O;
};
#endif //PARALLEL_DAG_TRAVERSAL_CONFIG_H
