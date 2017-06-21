#include "fstream"
#include "config.h"

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
			Edge **cur = &graph->nodes[v]->first;
			//printf("cost:%d edges:%d\n", cost, edges);	
			for(int e = 0; e < edges; ++e) {
				//printf("cur:%p\n", cur);
				int to = -1;
				in >> to;
				//printf("%d->%d\n", v, to);
				//build edge
				*cur = new Edge;
				//printf("cur:%p\n", cur);
				(*cur)->to = graph->nodes[to];
				cur = &((*cur)->next);
				//printf("cur:%p\n", cur);
				//printf("%p\n", graph->nodes[v]->first);
				//build reverse edge
				Edge *&rcur = graph->nodes[to]->rfirst;
				while(rcur != NULL)
					rcur = rcur->next;
				rcur = new Edge;
				rcur->to = graph->nodes[v];
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
	
	return;
}

void printDAG(Graph *graph) {
	printf("I_size: %d, M_size: %d, O_size: %d\n", graph->I_size, 
		graph->nodes_size - graph->I_size - graph->O_size, graph->O_size);
	for(int v = 1; v <= graph->nodes_size; ++v) {
		Edge *cur = graph->nodes[v]->first;
		printf("node %d\n", v);
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

int main(int argc, char **argv) {
	if(argc < 2) {
		printf("usage: build <file>\n");
	}
	else  {
		char *filename = argv[1];
		Graph graph;
		printf("Now build DAG...\n");
		buildDAG(filename, &graph);
		printf("Build Result:\n");
		printDAG(&graph);
	}
	return 0;
}
