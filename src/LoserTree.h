//
// Created by JensenJiang on 2017/6/27.
//

#ifndef PARALLEL_DAG_TRAVERSAL_LOSERTREE_H
#define PARALLEL_DAG_TRAVERSAL_LOSERTREE_H
class LoserTree {
private:
    int *cur;
    int *cur_size;
    int **pre;
    int *pre_size;
    int pre_num, tot;
    int *tree, *cur_index, *win;
public:
    LoserTree(int);

    void set_cur(int*, int*);

    void set_pre_num(const int&);

    void set_pre(const int&, int*, const int&);

    void cur_top_push(const int&);

    int get_value(const int&);

    void sort_all();

    void init_tree();

    void update_tree();
};
#endif //PARALLEL_DAG_TRAVERSAL_LOSERTREE_H
