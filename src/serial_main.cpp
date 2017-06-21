#include <cstdio>
#include <queue>
#include "config.h"
#define MINCOST -1
using namespace std;

Graph *global_graph;

class LoserTree {
private:
    int *cur;
    int *cur_size;
    int **pre;
    int *pre_size;
    int pre_num, tot;
    int *tree, *cur_index, *win;
public:
    LoserTree(int _max_size) {
        this->pre = new int*[_max_size];
        this->cur_index = new int[_max_size];
        this->pre_size = new int[_max_size];
        this->tree = new int[_max_size * 2];
        this->win = new int[_max_size * 2];
    }

    void set_cur(int* _cur, int* _cur_size) {
        this->cur = _cur;
        this->cur_size = _cur_size;
        *this->cur_size = 0;
    }

    void set_pre_num(const int& _num) {
        this->pre_num = _num;
    }

    void set_pre(const int& id, int* _pre, const int& _pre_size) {
        this->pre[id] = _pre;
        this->pre_size[id] = _pre_size;
    }

    void cur_top_push(const int& val) {
        this->cur[*(this->cur_size)] = val;
        (*(this->cur_size))++;
    }

    int get_value(const int& id) {
        return this->pre[id][this->cur_index[id]];
    }

    void sort_all() {
        /* copy directly */
        if(pre_num == 1) {
            for(int j = 0;j < this->pre_size[0];j++) {
                this->cur[j] = this->pre[0][j];
            }
            *(this->cur_size) = this->pre_size[0];
        }
        else {
            this->init_tree();
            this->update_tree();
        }

        /* set last as zero */
        this->cur[*(this->cur_size)] = MINCOST;
    }

    void init_tree() {
        this->tot = 0;
        for(int i = 0;i < this->pre_num;i++) {
            this->tot += this->pre_size[i];
            this->cur_index[i] = 0;
        }
        if(this->tot == 0) return;

        /* Init leaf nodes */
        for(int i = this->pre_num;i < this->pre_num*2;i++) {
            this->tree[i] = i - pre_num;
            this->win[i] = i - pre_num;
        }

        /* Init internal nodes */
        for(int i = this->pre_num - 1;i > 0;i--) {
            int l = i << 1;
            int l_val = this->get_value(this->win[l]);
            int r_val = this->get_value(this->win[l+1]);
            /* Left is winner */
            if(l_val > r_val) {
                this->win[i] = this->win[l];
                this->tree[i] = this->win[l+1];
            }
            /* Right is winner */
            else {
                this->win[i] = this->win[l+1];
                this->tree[i] = this->win[l];
            }
        }

        this->tree[0] = this->win[1];   // set the root as the final winner

        /* update top */
        this->cur_top_push(this->get_value(this->tree[0]));
        this->tot--;
        this->cur_index[this->tree[0]]++;
    }

    void update_tree() {
        while(this->tot > 0 && *(this->cur_size) < 10) {
            int cur_win = this->tree[0];
            for(int i = (pre_num + cur_win) >> 1;i > 0;i >>= 1) {
                int o_val = this->get_value(this->tree[i]);
                int n_val = this->get_value(cur_win);
                if(o_val > n_val) {
                    int temp = cur_win;
                    cur_win = this->tree[i];
                    this->tree[i] = temp;
                }
            }
            this->tree[0] = cur_win;

            /* update top */
            this->cur_top_push(this->get_value(cur_win));
            this->tot--;
            this->cur_index[cur_win]++;
        }
    }
};

int count_fa() {    // return max
    queue<Node*> q;
    bool *is_visit = new bool[global_graph->nodes_size];
    int max_fa = 0;
    for(int i = 0;i < global_graph->nodes_size;i++) is_visit[i] = false;

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
            if(!is_visit[i->to->nid]) q.push(i->to);
        }
    }

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

void solve() {
    queue<Node*> iter_queue;
    LoserTree *lt;
    int ans[11];
    int ans_size;

    /* Init */
    lt = new LoserTree(count_fa());
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
    while(!iter_queue.empty()) {
        Node *cur = iter_queue.front();
        iter_queue.pop();

        Node_sort_all(cur, lt);
        add_cost(cur);

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
}

void test_loser_tree() {
    LoserTree *tree = new LoserTree(10);
    puts("here!");
    int cur[12];
    int size;
    int pre1[5] = {5,4,3,2};
    int pre2[5] = {10,8,6,4};
    int pre3[5] = {7,4,2,1};
    int pre4[5] = {20, 5,4,1};

    tree->set_cur(cur, &size);
    tree->set_pre_num(4);
    tree->set_pre(0, pre1, 4);
    tree->set_pre(1, pre2, 4);
    tree->set_pre(2, pre3, 4);
    tree->set_pre(3, pre4, 4);

    tree->sort_all();
    for(int i = 0;i < 10;i++) printf("%d\n", cur[i]);
}

int main() {
    test_loser_tree();
}