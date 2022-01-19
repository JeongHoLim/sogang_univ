#include "my_solver.h"
#include <time.h>
#include <math.h>
#include <stdlib.h>

double* x, * fx;
int num;
double h;
double my_rand() {
	
	double ret = double(rand()) / RAND_MAX;
	
	while (ret == 0.0 || ret == 1.0)
		ret = double(rand()) / RAND_MAX;
	
	return ret;
}
double _F(double X) {

	int i;

	double ret = 0;
	double plus = 0;
	int m = 0;
	for (i = 0; i < num; i++) {
		if (x[i] < X) {
			ret += fx[i];
			if (i > 0)
				plus += fx[i];
		}
		else if (x[i] > X) {
			plus -= fx[m];
			ret += plus;
			ret *= (h / 2);
			plus = (fx[m] + ((fx[m+1]-fx[m]) / (x[m+1]-x[m])) * ((X-x[m])/2)) * (X - x[m]);
			ret += plus;
			break;
		}
		else {
			ret += (fx[i] + plus);
			ret *= (h / 2);
			break;
		}
		m = i;
	}
	return ret;
}
double _f(double X,double U) {

	return _F(X)-U;
}
double bisection(double U)
{
	double x0, temp;
	int n = 0;

	double a0 = 0,b0 = 1;
	x0 = a0;

	while (n < Nmax) {
		temp = a0 + (b0 - a0) / 2;

		if (_f(a0,U) * _f(temp,U) > 0) {
			a0 = temp;
		}
		else {
			b0 = temp;
		}
		if (fabs(_f(temp,U)) < DELTA || fabs(x0 - temp) < EPSILON)
			break;
		x0 = temp;
		n++;
	}
	return temp;
}
void program2_2()
{
	FILE* fp_r, *fp_w;
	
	__int64 start, freq, end;
	float resultTime = 0;

	fp_r = fopen("pdf_table.txt", "r");
	fp_w = fopen("random_event_table.txt", "w");

	/***************************************************/

	int i,nr;
	double U,ret;

	srand(time(NULL));
	fscanf(fp_r, "%d %lf\n", &num, &h);

	x = (double*)malloc(sizeof(double) * num);
	fx = (double*)malloc(sizeof(double) * num);

	for (i = 0; i < num; i++)
		fscanf(fp_r, "%lf %lf\n", &x[i], &fx[i]);
	
	printf("난수 개수 : ");
	scanf("%d", &nr);

	CHECK_TIME_START;

	fprintf(fp_w, "%d\n", nr);
	for (i = 0; i < nr; i++) {

		U = my_rand();
		ret = bisection(U);
		fprintf(fp_w,"%15.14e\n", ret);
	}


	free(x);
	free(fx);

	/***************************************************/

	if (fp_r != NULL) fclose(fp_r);
	if (fp_w != NULL) fclose(fp_w);

	CHECK_TIME_END(resultTime);
	printf("The program2_2 run time is %f(ms)..\n", resultTime * 1000.0);

}
