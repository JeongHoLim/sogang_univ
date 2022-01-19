#include "my_solver.h"

void practice3_2() {
	char readfile[256];
	char writefile[256];

	int NCOEF, DEGREE;

	double* poly;
	double* zeror, * zeroi;
	long int fail;
	int i;
	
	for (int t = 1; t <= 6; t++) {

		sprintf(readfile, "polynomial_3-2_%d.txt", t);
		sprintf(writefile, "roots_3-2_%d.txt", t);

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

		fscanf(fp_r, "%d", &DEGREE);
		NCOEF = DEGREE + 1;

		poly = (double*)malloc(sizeof(double) * NCOEF);
		zeror = (double*)malloc(sizeof(double) * DEGREE);
		zeroi = (double*)malloc(sizeof(double) * DEGREE);


		/********************************/
		i = 0;
		int j;
		double r, theta;
		double* output_i = (double*)malloc(sizeof(double) * NCOEF);
		double* output_r = (double*)malloc(sizeof(double) * NCOEF);
		while (fscanf(fp_r, "%lf", &poly[i++]) != EOF);
		for (i = 0; i < NCOEF; i++) {
			output_r[i] = 0;
			output_i[i] = 0;
		}
	
		rpoly_(poly, &DEGREE, zeror, zeroi, &fail);

		if (fail) {
			fprintf(fp_w,"num[%d] wrong\n\n\n",t);
		}
		else {
			for (i = 0; i < DEGREE; i++) {
				double r0 = sqrt(zeror[i] * zeror[i] + zeroi[i] * zeroi[i]);
				theta = atan((zeroi[i]) / (zeror[i]));
				if (zeror[i] < 0) theta = theta + M_PI;
	
				for (j = 0; j < NCOEF; j++) {
					r = pow(r0, j);
					output_r[i] += r * cos(j * theta) * poly[NCOEF - 1 - j];
					output_i[i] += r * sin(j * theta) * poly[NCOEF - 1 - j];
				}
			}
			fprintf(fp_w, "num[%d] success\n", t);
			for (i = 0; i < DEGREE; i++) {
				fprintf(fp_w,"%lf %lfi\n", zeror[i], zeroi[i]);
				fprintf(fp_w, "F(X) : %6lf + %6lfi\n", fabs(output_r[i]), output_i[i]);
				//printf("%10f+%10fi\nresult_%d : %lf+%lfi\n", zeror[i], zeroi[i], i, fabs(output[i]), output[i]);
			}
			fprintf(fp_w, "\n");
			/********************************/
		}

		free(zeroi);
		free(zeror);
		free(poly);
		free(output_r);
		free(output_i);
		if (fp_r != NULL) fclose(fp_r);
		if (fp_w != NULL) fclose(fp_w);
	}
}