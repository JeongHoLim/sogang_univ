#include "my_solver.h"

#define SOLNUMS 4
#define MATCOLS SOLNUMS
#define MATROWS SOLNUMS
#define TOLERANCE 0.0000001

void g_f_3_2(int* n, double* x, double* fvec, int* iflag) {

	/********************************/
	fvec[0] = x[0] + 10 * x[1] + 9;
	fvec[1] = sqrt(5.0) * (x[2] - x[3]) - 2 * sqrt(5.0);
	fvec[2] = pow((x[1] - 2 * x[2]), 2.0) - 9;
	fvec[3] = sqrt(10.0) * pow((x[0] - x[3]), 2.0) - 2 * sqrt(10.0);

	/********************************/
}

void g_3_2() {
	
	int n = SOLNUMS;
	double x[SOLNUMS] = { 0.9, -0.9, 1.25,-1.25 };	//need to initilize x0
	double fvec[SOLNUMS];
	double tol = TOLERANCE;
	int info;
	double wa[(SOLNUMS * (3 * SOLNUMS + 13)) / 2];
	int lwa = (SOLNUMS * (3 * SOLNUMS + 13)) / 2;
	int i;
	FILE* fp_w = fopen("roots_found_3-2.txt", "w");

	if (fp_w == NULL) {
		printf("file open error...\n");
		return;
	}
	fprintf(fp_w, "Homework 3-2\n\n");
	fprintf(fp_w,"Initial x: ");
	for (i = 0; i < SOLNUMS; i++)
		fprintf(fp_w,"%lf ", x[i]);
	
	fprintf(fp_w,"\n\n");
	hybrd1_(g_f_3_2, &n, x, fvec, &tol, &info, wa, &lwa);
	int k;
	fprintf(fp_w,"x : ");
	for (k = 0; k < SOLNUMS; k++)
		fprintf(fp_w,"%6lf ", x[k]);
	fprintf(fp_w,"\n");


	fprintf(fp_w,"fvec : ");
	for (k = 0; k < SOLNUMS; k++)
		fprintf(fp_w,"%6lf ", fvec[k]);
	printf("\n");


	fclose(fp_w);

	return;
}