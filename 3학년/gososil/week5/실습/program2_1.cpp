#include "my_solver.h"

double pdf(double* fx, int num ,double h, double s, double e) {

	double ret = 0;

	for (int i = int(num*s)+1; i < int(num*e)-1; i++)
		ret += fx[i];

	ret = (h / 2) * (fx[int(num*s)] + fx[int(num*e)-1] + ret * 2);

	return ret;
}

void program2_1()
{
	FILE* fp_r, *fp_w;
	__int64 start, freq, end;
	float resultTime = 0;

	fp_r = fopen("sampling_table.txt", "r");
	if (fp_r == NULL) {
		printf("input file not found....\n");
		exit(0);
	}

	fp_w = fopen("pdf_table.txt", "w");

	/***************************************************/
		
	double h;
	int num,i=0;
	fscanf(fp_r, "%d %lf", &num, &h);
	double* fx = (double*)malloc(sizeof(double) * num);
	double* x = (double*)malloc(sizeof(double) * num);
	while ((fscanf(fp_r, "%lf %lf", &x[i],&fx[i])) != EOF) i++;


	double x_diff = (x[num - 1] - x[0]);


		// find area 
	double trapezoidal_sum = 0;
	for (i = 1; i < num - 1; i++) {
		trapezoidal_sum += fx[i];
	}


	// normalize h
	h /= x_diff;

	trapezoidal_sum = (h / 2) * (trapezoidal_sum * 2 + fx[num - 1] + fx[0]);


	// normalize fx and x
	for (i = 0; i < num; i++) {
		fx[i] = fx[i]/ trapezoidal_sum;
		x[i] = i * h;
	}

	fprintf(fp_w, "%d %lf\n", num, h);

	for (i = 0; i < num; i++)
		fprintf(fp_w,"%lf %lf\n", h * i,fx[i]);

	printf("*** Integrating the pdf from 0.0 to 1.0 = %.6lf\n", pdf(fx,num,h,0, 1));
	printf("*** Integrating the pdf from 0.0 to 0.25 = %.6lf\n", pdf(fx, num, h, 0, 0.25));
	printf("*** Integrating the pdf from 0.25 to 0.5 = %.6lf\n", pdf(fx, num, h, 0.25, 0.5));
	printf("*** Integrating the pdf from 0.5 to 0.75 = %.6lf\n", pdf(fx, num, h, 0.5, 0.75));
	printf("*** Integrating the pdf from 0.75 to 1.0 = %.6lf\n", pdf(fx, num, h, 0.75, 1));

	free(fx);
	free(x);
	/***************************************************/

	if (fp_r != NULL) fclose(fp_r);
	if (fp_w != NULL) fclose(fp_w);
}
