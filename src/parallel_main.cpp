//
// Created by JensenJiang on 2017/6/27.
//
#include "LoserTree.h"
#include "parameters.h"
#include "config.h"
#include "buildDAG.h"
#include <omp.h>
#include <cstdio>
#include <queue>
#include <cstdlib>

using namespace std;

Graph *global_graph;
double start_t, read_data_t, end_t;
double temp_s, temp_t = 0;

int parallel_count_fa_max() {
    int max_fa = 0;
    // #pragma omp parallel for reduction(max:max_fa)
    for(int i = 1;i <= global_graph->nodes_size;i++)
        if(global_graph->nodes[i]->fa_num > max_fa) max_fa = global_graph->nodes[i]->fa_num;
    return max_fa;
}

void add_cost(Node *cur) {
    for(int i = 0;i < global_graph->I_size;i++) {
        for(int j = 0;j < cur->top_len[i];j++) cur->top[i][j] += cur->cost;
    }
}

void add_pair_cost(int oid) {
    Node *cur = global_graph->O[oid];
    for(int i = 0;i < global_graph->I_size;i++) {
        for(int j = 0;j < cur->top_len[i];j++) cur->top[i][j] += global_graph->pair_cost[i][oid];
    }
}

void solve(int threads_num) {
    queue<Node*> iter_queue;
    LoserTree **lt_arr;

    omp_set_num_threads(threads_num);
    int fa_max = parallel_count_fa_max();
    lt_arr = new LoserTree*[threads_num];
    for(int i = 0;i < threads_num;i++) lt_arr[i] = new LoserTree(fa_max);

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
            if(e->to->fa_num == 0)
                iter_queue.push(e->to);
        }
    }

    /* Compute */
    int temp_cnt = 0;
    while(!iter_queue.empty()) {
        int count;
        Node *cur[MAX_CUR];
        for(count = 0;count < MAX_CUR && !iter_queue.empty();count++) {
            cur[count] = iter_queue.front();
            iter_queue.pop();
        }
        // printf("%d\n", count);

        #pragma omp parallel shared(cur, lt_arr, count)
        {
            LoserTree *lt = lt_arr[omp_get_thread_num()];
            #pragma omp for schedule(dynamic)
            for(int i = 0;i < global_graph->I_size;i++) {
                for(int j = 0;j < count;j++) {
                    int pre_size = 0;
                    lt->set_cur(cur[j]->top[i], &cur[j]->top_len[i]);
                    for(Edge *e = cur[j]->rfirst;e != NULL;e = e->next) {
                        lt->set_pre(pre_size, e->to->top[i], e->to->top_len[i]);
                        pre_size++;
                    }
                    lt->set_pre_num(pre_size);
                    lt->sort_all();
                }
            }
        }

        for(int i = 0;i < count;i++) {
            add_cost(cur[i]);
            for(Edge *e = cur[i]->first;e != NULL;e = e->next) {
                e->to->fa_num--;
                if(e->to->fa_num == 0) iter_queue.push(e->to);
            }
        }
    }

    /* Finally 2 */
    int ans[11];
    int ans_size;
    LoserTree *lt = lt_arr[0];
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
    // fprintf(stderr, "Queue: %lf\n", temp_t);
}

int main(int argc, char **argv) {
    start_t = omp_get_wtime();
    omp_get_wtime();
    global_graph = new Graph();
    buildDAG2(argv[1], global_graph);
    read_data_t = omp_get_wtime();
    int threads_num = atoi(argv[2]);
    solve(threads_num);
}
