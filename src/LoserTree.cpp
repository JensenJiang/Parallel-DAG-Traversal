//
// Created by JensenJiang on 2017/6/27.
//
#include "LoserTree.h"
#include "parameters.h"

LoserTree::LoserTree(int _max_size) {
    this->pre = new int*[_max_size];
    this->cur_index = new int[_max_size];
    this->pre_size = new int[_max_size];
    this->tree = new int[_max_size * 2];
    this->win = new int[_max_size * 2];
}

void LoserTree::set_cur(int* _cur, int* _cur_size) {
    this->cur = _cur;
    this->cur_size = _cur_size;
    *this->cur_size = 0;
}

void LoserTree::set_pre_num(const int& _num) {
    this->pre_num = _num;
}

void LoserTree::set_pre(const int& id, int* _pre, const int& _pre_size) {
    this->pre[id] = _pre;
    this->pre_size[id] = _pre_size;
}

void LoserTree::cur_top_push(const int& val) {
    this->cur[*(this->cur_size)] = val;
    (*(this->cur_size))++;
}

int LoserTree::get_value(const int& id) {
    return this->pre[id][this->cur_index[id]];
}

void LoserTree::sort_all() {
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

void LoserTree::init_tree() {
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

void LoserTree::update_tree() {
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