#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include "config.h"

int rand_int(int range = 128) {
	return rand() % range;
}

void dataComplete(const char *filename) {
	srand((unsigned)time(NULL));
	std::ifstream in;
	in.open(filename, std::ios::in);
	if(in.is_open()) {
		int E = -1, V = -1;
		int I_size = 0, M_size = 0, O_size = 0;
		in >> V >> E;	
		int * in_cnt = new int[V+1];
		int *out_cnt = new int[V+1];
		memset( in_cnt, 0, sizeof(int)*(V+1));
		memset(out_cnt, 0, sizeof(int)*(V+1));	

		for(int e = 0; e < E; ++e) {
			int from = -1, to = -1, weight = 1;
			in >> from >> to >> weight;
			++in_cnt[to];
			++out_cnt[from];	
		}

		in.close();

		std::ofstream app;
		app.open(filename, std::ios::app);
		if(app.is_open()) {
			for(int v = 1; v <= V; ++v) {
				if(in_cnt[v] == 0) {//source
					++I_size;
					app << 0 << std::endl;
				}
				else if(out_cnt[v] == 0) {
					++O_size;
					app << 0 << std::endl;
				}
				else {
					++M_size;
					app << rand_int() << std::endl;
				}
			}
			for(int I = 0; I < I_size; ++I) {
				for(int O = 0; O < O_size; ++O) {
					app << rand_int() << " ";
				}
				app << std::endl;
			}

		}

		delete [] in_cnt;
		delete []out_cnt;
		
	}
	else  {
		printf("Open file %s error!\n", filename);
	}
}

void buildDAG2(const char *filename, Graph *graph) {
	std::ifstream in;
	in.open(filename, std::ios::in);
	if(in.is_open()) {
		int E = -1, V = -1;
		int I_size = 0, M_size = 0, O_size = 0;
		in >> V >> E;
		graph->nodes_size = V;
		
		printf("	allocating...\n");
		int * in_cnt = new int[V+1];
		int *out_cnt = new int[V+1];
		memset( in_cnt, 0, sizeof(int)*(V+1));
		memset(out_cnt, 0, sizeof(int)*(V+1));
 		graph->nodes = new Node *[V+1];
		
		//build node
		printf("	building nodes...\n");
		//printf("V:%d\n", V);
		for(int v = 1; v <= V; ++v) {
			//printf("v:%d\n", v);
			graph->nodes[v] = new Node;
			graph->nodes[v] -> nid = v;	

		}
		//build edges
		printf("	building edges...\n");
		for(int e = 0; e < E; ++e) {
			int from = -1, to = -1, weight = 1;
			in >> from >> to >> weight;
			//printf("%d->%d\n", v, to);
			//build edge
			Edge *cur = new Edge;
			cur->to = graph->nodes[to];
			cur->next = graph->nodes[from]->first;
			graph->nodes[from]->first = cur;
			//build reverse edge
			Edge *rcur = new Edge;
			rcur->to = graph->nodes[from];
			rcur->next = graph->nodes[to]->rfirst;
			graph->nodes[to]->rfirst = rcur;
			++in_cnt[to];
			++out_cnt[from];	
		}
		for(int v = 1; v <= V; ++v) {
			graph->nodes[v]->fa_num = in_cnt[v];
			if(in_cnt[v] == 0) {//source
				++I_size;
			}
			else if(out_cnt[v] == 0) {
				++O_size;
			}
			else {
				++M_size;
			}
		}
		graph->I_size = I_size;
		graph->O_size = O_size;
		graph->I = new Node *[I_size];
		graph->M = new Node *[M_size];
		graph->O = new Node *[O_size];
		graph->pair_cost = new int *[I_size];



		for(int in = 0; in < I_size; ++in)
			graph->pair_cost[in] = new int[O_size];
		int tmp_I = 0, tmp_M = 0, tmp_O = 0;
		for(int v = 1; v <= V; ++v) {
			printf("I_size:%d\n", I_size);
			graph->nodes[v]->top = new int *[I_size];
			for(int in = 0; in < I_size; ++in)
				graph->nodes[v]->top[in] = new int[11];		
			graph->nodes[v]->top_len = new int[I_size];

			int cost = -1;
			in >> cost;
			graph->nodes[v]->cost = cost;

			if(in_cnt[v] == 0) {//source
				graph->I[tmp_I++] = graph->nodes[v];
			}
			else if(out_cnt[v] == 0) {
				graph->O[tmp_O++] = graph->nodes[v];			}
			else {
				graph->M[tmp_M++] = graph->nodes[v];
			}
		}

		//build pair_cost
		for(int I = 0; I < I_size; ++I) {
			for(int O = 0; O < O_size; ++O) {
				int weight = -1;
				in >> weight;
				graph->pair_cost[I][O] = weight;
			}
		} 
		delete [] in_cnt;
		delete []out_cnt;
		in.close();
	}
	else  {
		printf("Open file %s error!\n", filename);
	}	
	return;
}
void buildDAG(const char *filename, Graph *graph) {
	std::ifstream in;
	in.open(filename, std::ios::in);
	if(in.is_open()) {
		int E = -1, V = -1, I_size = -1, O_size = -1;
		in >> E >> V >> I_size >> O_size;
		graph->nodes_size = V;
		graph->I_size = I_size;
		graph->O_size = O_size;
		printf("	allocating...\n");
		graph->nodes = new Node *[V+1];
		graph->I = new Node *[I_size];
		graph->M = new Node *[V - I_size - O_size];
		graph->O = new Node *[O_size];
		graph->pair_cost = new int *[I_size];
		for(int in = 0; in < I_size; ++in)
			graph->pair_cost[in] = new int[O_size];
		//build node
		printf("	building nodes...\n");
		//printf("V:%d\n", V);
		for(int v = 1; v <= V; ++v) {
			//printf("v:%d\n", v);
			graph->nodes[v] = new Node;
			graph->nodes[v] -> nid = v;	
			graph->nodes[v]->top = new int *[I_size];
			for(int in = 0; in < I_size; ++in)
				graph->nodes[v]->top[in] = new int[11];		
			graph->nodes[v]->top_len = new int[I_size];
			if(v <= I_size)
				graph->I[v-1] = graph->nodes[v];
			else if(v > V - O_size)
				graph->O[v-(V-O_size)-1] = graph->nodes[v];
			else
				graph->M[v-I_size-1] = graph->nodes[v];
		}
		//build edges
		printf("	building edges...\n");
		for(int v = 1; v <= V; ++v) {
			int cost = -1, edges = -1;
			in >> cost >> edges;
			graph->nodes[v]->cost = cost;
			//printf("cost:%d edges:%d\n", cost, edges);	
			for(int e = 0; e < edges; ++e) {
				//printf("cur:%p\n", cur);
				int to = -1;
				in >> to;
				//printf("%d->%d\n", v, to);
				//build edge
				Edge *cur = new Edge;
				cur->to = graph->nodes[to];
				cur->next = graph->nodes[v]->first;
				graph->nodes[v]->first = cur;
				//build reverse edge
				Edge *rcur = new Edge;
				rcur->to = graph->nodes[v];
				rcur->next = graph->nodes[to]->rfirst;
				graph->nodes[to]->rfirst = rcur;
			}	
		}
		//build pair_cost
		for(int I = 0; I < I_size; ++I) {
			for(int O = 0; O < O_size; ++O) {
				int weight = -1;
				in >> weight;
				graph->pair_cost[I][O] = weight;
			}
		} 
		in.close();
	}
	else  {
		printf("Open file %s error!\n", filename);
	}	
	return;
}

void printDAG(Graph *graph) {
	printf("I_size: %d, M_size: %d, O_size: %d\n", graph->I_size, 
		graph->nodes_size - graph->I_size - graph->O_size, graph->O_size);
	for(int v = 1; v <= graph->nodes_size; ++v) {
		Edge *cur = graph->nodes[v]->first;
		printf("node %d, cost: %d, fa_num:%d\n", v, graph->nodes[v]->cost, graph->nodes[v]->fa_num);
		//printf("edges:\n");
		while(cur != NULL) {
			printf(" %d", cur->to->nid);
			cur = cur->next;
		}
		printf("\n");
		cur = graph->nodes[v]->rfirst;
		//printf("reges:\n");
		while(cur != NULL) {
			printf(" %d", cur->to->nid);
			cur = cur->next;
		}
		printf("\n");
	}
	return;
}

int tmain(int argc, char **argv) {
	if(argc < 2) {
		printf("usage: build <file>\n");
	}
	else  {
		char *filename = argv[1];
		// dataComplete(filename);
		Graph graph;
		printf("Now build DAG...\n");
		buildDAG2(filename, &graph);
		printf("Build Result:\n");
		printDAG(&graph);
	}
	return 0;
}
