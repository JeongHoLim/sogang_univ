#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
int max3(int a, int b, int c, int* idx) {
	if (a >= b) {
		if (a >= c) {
			*idx = 0;
			return a;
		}
		else {
			*idx = 2;
			return c;
		}
	}
	else {
		if (b > c) {
			*idx = 1;
			return b;
		}
		else {
			*idx = 2;
			return c;
		}
	}
}
int AreTheyMatch(char x, char y, int f, int s) {
	return x == y ? s : f;
}
void gapedAllignment(char* A, char* B, int m, int n, int p, int s, int f, FILE* fp) {

	int i, j, idx, temp;
	int delCount = 0;
	int len = 0,delA = 0, delB = 0, tJ = 0, tI = 0;
	int* Iarr = NULL;
	int* Jarr = NULL;
	int** S = (int**)malloc(sizeof(int*) * (m + 1));
	int** I = (int**)malloc(sizeof(int*) * (m + 1));

	for (i = 0; i <= m; i++) {
		S[i] = (int*)malloc(sizeof(int) * (n + 1));
		I[i] = (int*)malloc(sizeof(int) * (n + 1));
	}

	for (i = 0; i <= m; i++) {
		for (j = 0; j <= n; j++) {
			if (i == 0) {
				S[i][j] = p * j;
				I[i][j] = 1;
			}
			else if (j == 0) {
				S[i][j] = p * i;
				I[i][j] = 2;

			}
			else {
				S[i][j] = max3(S[i - 1][j - 1] + AreTheyMatch(A[i - 1], B[j - 1], f, s), S[i][j - 1] + p, S[i - 1][j] + p, &idx);
				I[i][j] = idx;
			}
		}
	}
	i = m; j = n;
	while (1) {
		if (I[i][j] == 0) {
			i--; j--;
		}
		else if (I[i][j] == 1) {
			delA++;
			j--;
		}
		else {
			delB++;
			i--;
		}
		len++;
		if (i == 0 && j == 0) break;
	}
	i = m; j = n;
	Iarr = (int*)malloc(sizeof(int) * len);
	Jarr = (int*)malloc(sizeof(int) * len);
	temp = len;
	while (temp--) {
		if (I[i][j] == 0) {
			i--; j--;
		}
		else if (I[i][j] == 1) {
			Iarr[delA - 1 - tI] = temp+1;
			tI++;
			j--;
		}
		else {
			Jarr[delB - 1 - tJ] = temp+1;
			tJ++;
			i--;
		}
	}
	
	fprintf(fp, "%d\n", S[m][n]);
	fprintf(fp, "%d\n", len);
	fprintf(fp, "%d\n", delA);

	for (i = 0; i < delA; i++) {
		fprintf(fp, "%d\n", Iarr[i]);
	}
	fprintf(fp, "%d\n", delB);

	for (i = 0; i < delB; i++) {
		fprintf(fp, "%d\n", Jarr[i]);
	}
	free(Iarr);
	free(Jarr);
	for (i = 0; i <= m; i++) {
		free(S[i]);
		free(I[i]);
	}
	free(S);
	free(I);
	return;
}
int main() {
	FILE* fp = fopen("input.txt", "rt");
	if (fp == NULL) {
		printf("File error!\n");
		exit(1);
	}
	FILE* Ip, * Op;
	int S, F, P;
	int m, n,res;
	char fileName[32];
	char* A, * B;
	fscanf(fp, "%s", fileName);
	fscanf(fp, "%d %d %d", &S, &F, &P);
	res = fclose(fp);
	if (res != 0) {
		printf("File close error!\n");
		exit(1);
	}
	Ip = fopen(fileName, "rb");
	if (Ip == NULL) {
		printf("File error!\n");
		exit(1);
	}
	fread(&m, sizeof(int), 1, Ip);
	fread(&n, sizeof(int), 1, Ip);
	A = (char*)malloc(sizeof(char) * (m + 1));
	B = (char*)malloc(sizeof(char) * (n + 1));

	fread(A, sizeof(char), m, Ip);
	fread(B, sizeof(char), n, Ip);
	A[m] = 0; B[n] = 0;


	Op = fopen("output.txt", "wt");
	if (Op == NULL) {
		printf("File error!\n");
		exit(1);
	}
	P *= -1; F *= -1;
	gapedAllignment(A, B, m, n, P, S, F, Op);

	free(A);
	free(B);
	res = fclose(Ip);
	if (res != 0) {
		printf("File close error!\n");
		exit(1);
	}
	res = fclose(Op);
	if (res != 0) {
		printf("File close error!\n");
		exit(1);
	}
	return 0;
}