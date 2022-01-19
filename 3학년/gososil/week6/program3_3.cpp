#include "my_solver.h"

#define SOLNUMS 2
#define MATCOLS SOLNUMS
#define MATROWS SOLNUMS
#define TOLERANCE 0.0000001

void g_f_3_3(int* n, double* x, double* fvec, int* iflag) {

	/********************************/
	fvec[0] = (sin(x[0]*x[1]+M_PI/6) + sqrt(pow(x[0],2.0)*pow(x[1],2.0)+1))+ 2.8*(cos(x[0]-x[1]));

	fvec[1] = (x[0] * exp(x[0] * x[1] + M_PI / 6) - sin(x[0] - x[1])) - 1.66* (sqrt(pow(x[0], 2.0) * pow(x[1], 2.0) + 1));
	
	/********************************/
}
void g_3_3() {
	

	int n = SOLNUMS;
	double x[SOLNUMS] = { 20.0,0.0};	//need to initilize x0
	double fvec[SOLNUMS];
	double tol = TOLERANCE;
	int info;
	double wa[(SOLNUMS * (3 * SOLNUMS + 13)) / 2];
	int lwa = (SOLNUMS * (3 * SOLNUMS + 13)) / 2;
	int i;
	FILE* fp_w = fopen("roots_found_3-3.txt", "w");
	if (fp_w == NULL) {
		printf("file open error...\n");
		return;
	}
	fprintf(fp_w, "Homework 3-3\n\n");
	fprintf(fp_w, "Initial x: ");
	for (i = 0; i < SOLNUMS; i++)
		fprintf(fp_w, "%lf ", x[i]);

	fprintf(fp_w, "\n\n");
	hybrd1_(g_f_3_3, &n, x, fvec, &tol, &info, wa, &lwa);
	int k;
	fprintf(fp_w, "x : ");
	for (k = 0; k < SOLNUMS; k++)
		fprintf(fp_w, "%6lf ", x[k]);
	fprintf(fp_w, "\n");


	fprintf(fp_w, "fvec : ");
	for (k = 0; k < SOLNUMS; k++)
		fprintf(fp_w, "%6lf ", fvec[k]);
	

	fclose(fp_w);

	
	return;
}
