#include <cstdio>
#include <queue>
#include <omp.h>
#include "buildDAG.h"
#include "config.h"
#include "LoserTree.h"
#include "parameters.h"

using namespace std;

Graph *global_graph;
double start_t, read_data_t, end_t;

int count_fa() {    // return max
    queue<Node*> q;
    bool *is_visit = new bool[global_graph->nodes_size + 1];
    int max_fa = 0;
    for(int i = 1;i <= global_graph->nodes_size;i++) is_visit[i] = false;

    for(int i = 0;i < global_graph->I_size;i++) {
        global_graph->I[i]->fa_num = 0;
        q.push(global_graph->I[i]);
        is_visit[global_graph->I[i]->nid] = true;
    }

    while(!q.empty()) {
        Node *cur = q.front();
        q.pop();
        for(Edge *i = cur->first;i != NULL;i = i->next) {
            i->to->fa_num++;
            if(i->to->fa_num > max_fa) max_fa = i->to->fa_num;
            if(!is_visit[i->to->nid]) {
                q.push(i->to);
                is_visit[i->to->nid] = true;
            }
        }
    }

    return max_fa;
}

int count_fa_max() {
    int max_fa = 0;
    for(int i = 1;i <= global_graph->nodes_size;i++)
        if(global_graph->nodes[i]->fa_num > max_fa) max_fa = global_graph->nodes[i]->fa_num;
    return max_fa;
}

void Node_sort_all(Node *node, LoserTree *lt) {
    // parallel?
    for(int i = 0;i < global_graph->I_size;i++) {
        int pre_size = 0;
        lt->set_cur(node->top[i], &node->top_len[i]);
        for(Edge *e = node->rfirst;e != NULL;e = e->next) {
            lt->set_pre(pre_size, e->to->top[i], e->to->top_len[i]);
            pre_size++;
        }
        lt->set_pre_num(pre_size);
        lt->sort_all();
    }
}

void add_pair_cost(int oid) {
    Node *cur = global_graph->O[oid];
    for(int i = 0;i < global_graph->I_size;i++) {
        for(int j = 0;j < cur->top_len[i];j++) cur->top[i][j] += global_graph->pair_cost[i][oid];
    }
}

void add_cost(Node *cur) {
    for(int i = 0;i < global_graph->I_size;i++) {
        for(int j = 0;j < cur->top_len[i];j++) cur->top[i][j] += cur->cost;
    }
}

void test_fa_num() {
    for(int i = 1;i <= global_graph->nodes_size;i++) {
        printf("Node %d: %d\n", global_graph->nodes[i]->nid, global_graph->nodes[i]->fa_num);
    }
}

void print_top(Node *cur) {
    printf("Node %d top:\n", cur->nid);
    for(int i = 0;i < global_graph->I_size;i++) {
        printf("Src %d: ", i);
        for(int j = 0;j < cur->top_len[i];j++) printf("%d ", cur->top[i][j]);
        puts("");
    }
}

void solve() {
    queue<Node*> iter_queue;
    LoserTree *lt;
    int ans[11];
    int ans_size;

    /* Init */
    lt = new LoserTree(count_fa_max());
    // test_fa_num();
    for(int i = 0;i < global_graph->I_size;i++) {
        for(int j = 0;j < global_graph->I_size;j++) {
            global_graph->I[i]->top_len[j] = 0;
            global_graph->I[i]->top[j][0] = MINCOST;
        }
        global_graph->I[i]->top[i][1] = MINCOST;
        global_graph->I[i]->top[i][0] = 0;
        global_graph->I[i]->top_len[i] = 1;
        for(Edge *e = global_graph->I[i]->first;e != NULL;e = e->next) {
            e->to->fa_num--;
            if(e->to->fa_num == 0) iter_queue.push(e->to);
        }
    }

    /* Compute */
    int temp_cnt = 0;
    while(!iter_queue.empty()) {
        Node *cur = iter_queue.front();
        iter_queue.pop();

        Node_sort_all(cur, lt);
        add_cost(cur);
        // print_top(cur);

        for(Edge *e = cur->first;e != NULL;e = e->next) {
            e->to->fa_num--;
            if(e->to->fa_num == 0) iter_queue.push(e->to);
        }
    }

    /* Finally */
    for(int i = 0;i < global_graph->O_size;i++) {
        int pre_size = 0;
        Node* cur = global_graph->O[i];
        add_pair_cost(i);
        lt->set_cur(ans, &ans_size);
        for(int j = 0;j < global_graph->I_size;j++) {
            lt->set_pre(pre_size, cur->top[j], cur->top_len[j]);
            pre_size++;
        }
        lt->set_pre_num(pre_size);
        lt->sort_all();

        /* Print Ans */
        printf("Sink Node %d: ", i);
        for(int j = 0;j < ans_size;j++) {
            printf("%d ", ans[j]);
        }
        puts("");
    }
    end_t = omp_get_wtime();

    fprintf(stderr, "%lf\n", read_data_t - start_t);
    fprintf(stderr, "%lf\n", end_t - read_data_t);
}

void test_loser_tree() {
    LoserTree *tree = new LoserTree(10);
    int cur[12];
    int size;
    int pre1[5] = {0};
    int pre2[5] = {0};
    int pre3[5] = {7};
    int pre4[5] = {20};

    tree->set_cur(cur, &size);
    tree->set_pre_num(2);
    tree->set_pre(0, pre1, 1);
    tree->set_pre(1, pre2, 1);
    /*
    tree->set_pre(2, pre3, 1);
    tree->set_pre(3, pre4, 1);
     */

    tree->sort_all();
    for(int i = 0;i < size;i++) printf("%d\n", cur[i]);
}

int main(int argc, char **argv) {
    start_t = omp_get_wtime();
    global_graph = new Graph();
    buildDAG2(argv[1], global_graph);
    printDAG(global_graph);
    read_data_t = omp_get_wtime();
    // printDAG(global_graph);
    solve();
}