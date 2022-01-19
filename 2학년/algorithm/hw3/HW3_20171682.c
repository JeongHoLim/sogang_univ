#define _CRT_SECURE_NO_WARNINGS
#define SWAP(a,b) { Node t; t=a; a=b; b=t;}
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
int V;
unsigned long long int E, max_weight;
unsigned long long int n;
typedef struct _node {
	int from;
	int to;
	unsigned long long int weight;
}Node;
typedef struct _vertex {
	unsigned long long int mstWeight;
	int parent;
	int rank;
	int count;
}Vertex;
void adjust(Node* heap,int i,int N) {
	
	int child;
	child = 2*i;
	Node tmp = heap[i];
	while (child <= N) {
		if (child < N && (heap[child].weight > heap[child + 1].weight)) child++;

		if (heap[child].weight > tmp.weight) break;
		else {
			heap[child / 2] = heap[child];
			child *= 2;
		}
	} // O(logN)
	heap[child/2] = tmp;
	return;
}
Node delMin(Node* heap) { //O(logE)
	Node tmp = heap[1];
	SWAP(heap[1], heap[n]);
	n--;
	adjust(heap, 1, n);
	return tmp;
}
void makeHeap(Node* heap, int N) {
	int i;
	for (i = N / 2; i > 0; i--) {
		adjust(heap, i, N);
	}
}
int find(Vertex* s,int v) {
	if (s[v].parent== v) return v;
	else {
		return find(s, s[v].parent);
	}
}
void Union(Vertex* s,int x,int y,int w) {
	int xRoot = find(s, x);
	int yRoot = find(s,y);

	if (s[xRoot].rank == s[yRoot].rank) {
		s[yRoot].parent = xRoot;
		s[xRoot].rank++;
		s[xRoot].mstWeight += s[yRoot].mstWeight + w;
		s[xRoot].count += s[yRoot].count;
	}
	else if (s[xRoot].rank > s[yRoot].rank) {
		s[yRoot].parent = xRoot;
		s[xRoot].mstWeight += s[yRoot].mstWeight + w;
		s[xRoot].count += s[yRoot].count;
	}
	else {
		s[xRoot].parent = yRoot;
		s[yRoot].mstWeight += s[xRoot].mstWeight + w;
		s[yRoot].count += s[xRoot].count;
	}
	return;
}
void printMst(Vertex* set,FILE* op) {

	int i,j;
	int conCom = 0;

	for (i = 0; i < V; i++) {
		if (set[i].parent == i) {
			conCom++;
		}
	}	//O(V)
	unsigned long long int* arr = (unsigned long long int*)malloc(sizeof(unsigned long long int) * (conCom));
	int* arr2 = (int*)malloc(sizeof(int) * conCom);

	for (i = 0,j=0; i < V; i++) {
		if (set[i].parent == i) {
			arr[j] = set[i].mstWeight;
			arr2[j] = set[i].count;
			j++;
		}
	}	// O(V)

	fprintf(op,"%d\n", conCom);
	for (i = 0; i < conCom; i++) {
		fprintf(op,"%d %lld\n", arr2[i], arr[i]);
	}  // O(V)
	free(arr);
	free(arr2);
	return;
}

void KruskalMST(Node* heap,Vertex* set) {
	int i;
	int x, y, w, s = 0;
	Node tmp;
	for (i = 0; i < V; i++) {
		set[i].parent = i;
		set[i].rank = 0;
		set[i].mstWeight = 0;
		set[i].count = 1;
	}		//O(V)
	while (s < V-1) {
		
		if (n == 0) break;
		tmp = delMin(heap);
		x = find(set,tmp.from);
		y = find(set, tmp.to);	// O(logV)
		if (x != y) {
			w = tmp.weight;
			Union(set, x, y,w);	//O(logV)
			s++;
		}
		
	}	//  -> O(E*logV)
	return;
}

int main() {

	//clock_t start1, finish1;
	//clock_t start2, finish2;
	//double duration1,duration2;

	FILE* ip, * op;
	char input[128], output[128];
	int i,res;
	ip = fopen("commands.txt","rt");
	if (!ip) {
		fprintf(stderr, "Error: cannot open the file\n");
		exit(-1);
	}
	fscanf(ip, "%s",input);
	fscanf(ip, "%s", output);
	res = fclose(ip);
	if (res != 0) {
		fprintf(stderr, "Error: cannot close the file\n");
	}
	ip = fopen(input, "rt");
	if (!ip) {
		fprintf(stderr, "Error: cannot open the file\n");
		exit(-1);
	}

	op = fopen(output, "wt");
	if (!op) {
		fprintf(stderr, "Error: cannot open the file\n");
		exit(-1);
	}	int v, e;
	unsigned long long int w;
	fscanf(ip, "%d %lld %lld", &V, &E, &max_weight);
	Node* heap = (Node*)malloc(sizeof(Node) * (E + 1));
	Node tmp;
	for (i = 0; i < E; i++) {
		fscanf(ip, "%d %d %lld", &v, &e, &w);
		tmp.from = v;	tmp.to = e; tmp.weight = w;
		heap[i + 1] = tmp;
	}	// O(E)
	n = E;
	Vertex* set = (Vertex*)malloc(sizeof(Vertex) * (V));
	//start1 = clock();
	makeHeap(heap, E);		//O(E)
	//finish1 = clock();
	//start2 = clock();
	KruskalMST(heap,set);	//O(E*logV)
	//finish2 = clock(); 
	//duration2 = (double)(finish2 - start2) / CLOCKS_PER_SEC;
	//duration1 = (double)(finish1 - start1) / CLOCKS_PER_SEC;
	res = fclose(ip);
	if (res != 0)	fprintf(stderr, "Error: cannot close the file\n");
	
	printMst(set,op);	//O(V)
	res = fclose(op);
	if (res != 0) {
		fprintf(stderr, "Error: cannot close the file\n");
	}
	free(set);
	free(heap);
	return 0;
}