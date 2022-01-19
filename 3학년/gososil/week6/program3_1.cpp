#include "my_solver.h"

#define SOLNUMS 4
#define MATCOLS SOLNUMS
#define MATROWS SOLNUMS
#define TOLERANCE 0.0000001

double p[4][3];
double t[4];
double tr[4];
double C;
double b;
void g_f_3_i(int* n, double* x, double* fvec, double* fjac, int* ldfjac, int* iflag)
{
	int i,j;
	// origin function F(x)
	if (*iflag == 1) {
		/********************************/
		for (i = 0; i < 4; i++) {
			fvec[i] = 0;
			for (j = 0; j < 3; j++)
				fvec[i] += pow((x[j] - p[i][j]), 2.0);
			fvec[i] -= pow((C * (tr[i] + x[3] - t[i])),2.0);
		}
		/********************************/
	}
	// Jacobi matrix J(x)
	else if (*iflag == 2) {
		/********************************/

		for (i = 0; i < 3; i++) {
			for (j = 0; j < 4; j++) {
				fjac[4 * i + j] = 2 * (x[i] - p[j][i]);
			}
		}
		for (i = 0; i < 4; i++) {
			fjac[12 + i] = -2 * C*C * (tr[i] + x[3] - t[i]);
		}

		/********************************/
	}
}
void g_f_3_ii(int* n, double* x, double* fvec, int* iflag){

	/********************************/
	int i, j;
	for (i = 0; i < 4; i++) {
		fvec[i] = 0;
		for (j = 0; j < 3; j++)
			fvec[i] += pow((x[j] - p[i][j]), 2.0);
		fvec[i] -= pow((C * (tr[i] + x[3] - t[i])), 2.0);
	}
	/********************************/
}
void g_3_1_ii(void) {

	int n = SOLNUMS;
	double x[SOLNUMS] = { -4.0, 5.0 };	//need to initilize x0
	double fvec[SOLNUMS];
	double tol = TOLERANCE;
	int info;
	double wa[(SOLNUMS * (3 * SOLNUMS + 13)) / 2];
	int lwa = (SOLNUMS * (3 * SOLNUMS + 13)) / 2;
	FILE* fp_w = NULL;

	char readfile[256];
	char writefile[256];

	int i, k;
	
	printf("\n\nHYBRD1\n=========3-1-(ii)========\n");

	printf("Initial x0 x1 x2: ");
	scanf("%lf %lf %lf", &x[0], &x[1], &x[2]);
	
	for (i = 0; i < 3; i++) {

		sprintf(readfile, "GPS_signal_%d.txt", i);
		sprintf(writefile, "GPS_position_3-2_%d.txt", i);

		FILE* fp_r = fopen(readfile, "r");
		FILE* fp_w = fopen(writefile, "w");

		if (fp_r == NULL) {
			printf("%s file open error...\n", readfile);
			return;
		}

		if (fp_w == NULL) {
			printf("%s file open error...\n", writefile);
			return;
		}

		fscanf(fp_r, "%lf %lf", &C, &b);
		x[3] = b;

		for (k = 0; k < 4; k++)
			fscanf(fp_r, "%lf %lf %lf %lf %lf", &p[k][0], &p[k][1], &p[k][2], &t[k], &tr[k]);

		hybrd1_(g_f_3_ii, &n, x, fvec, &tol, &info, wa, &lwa);

		fprintf(fp_w, "info : %d\n", info);

		fprintf(fp_w, "x : ");
		for (k = 0; k < SOLNUMS; k++)
			fprintf(fp_w, "%lf ", x[k]);
		fprintf(fp_w, "\n");

		fprintf(fp_w, "fvec : ");
		for (k = 0; k < SOLNUMS; k++)
			fprintf(fp_w, "%lf ", fvec[k]);
		fprintf(fp_w, "\n");



		/********************************/
		fclose(fp_r);
		fclose(fp_w);
	}
}


void g_3_1_i(void) {
	int n = SOLNUMS;
	double x[SOLNUMS];	//need to initilize x0
	double fvec[SOLNUMS], fjac[MATCOLS * MATROWS];
	int ldfjac = SOLNUMS;
	double tol = TOLERANCE;
	int info;
	double wa[(SOLNUMS * (SOLNUMS + 13)) / 2];
	int lwa = (SOLNUMS * (SOLNUMS + 13)) / 2;
	FILE* fp_w = NULL;
	
	char readfile[256];
	char writefile[256];

	int i,k;
	printf("HYBRJ1\n========3-1-(i)========\n");
	printf("Initial x0 x1 x2: ");
	scanf("%lf %lf %lf", &x[0], &x[1], &x[2]);

	for (i = 0; i < 3; i++) {

		sprintf(readfile, "GPS_signal_%d.txt", i);
		sprintf(writefile, "GPS_position_3-1_%d.txt", i);

		FILE* fp_r = fopen(readfile, "r");
		FILE* fp_w = fopen(writefile, "w");

		if (fp_r == NULL) {
			printf("%s file open error...\n", readfile);
			return;
		}

		if (fp_w == NULL) {
			printf("%s file open error...\n", writefile);
			return;
		}

		fscanf(fp_r, "%lf %lf", &C, &b);
		x[3] = b;
		for (k = 0; k < 4; k++)
			fscanf(fp_r, "%lf %lf %lf %lf %lf", &p[k][0], &p[k][1], &p[k][2], &t[k], &tr[k]);


		hybrj1_(g_f_3_i, &n, x, fvec, fjac, &ldfjac, &tol, &info, wa, &lwa);


		fprintf(fp_w, "info : %d\n", info);
		
		fprintf(fp_w, "x : ");
		for (k = 0; k < SOLNUMS; k++)
			fprintf(fp_w, "%lf ", x[k]);
		fprintf(fp_w, "\n");

		fprintf(fp_w,"fvec : ");
		for (k = 0; k < SOLNUMS; k++)
			fprintf(fp_w, "%lf ", fvec[k]);
		fprintf(fp_w, "\n");




		/********************************/
		fclose(fp_r);
		fclose(fp_w);
	}

}
void g_3_1() {
	g_3_1_i();
	g_3_1_ii();
}