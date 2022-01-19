#include "my_solver.h"

extern double (*_f)(double);
extern double (*_fp)(double);

/*********************************************
  Secant Method
**********************************************/
void program1_2(FILE* fp) {
	double x0, x1, temp;
	int n;

	if (fp == NULL)
		return;

	printf("init value(x0 x1) : ");
	scanf("%lf %lf", &x0, &x1);

	n = 0;
	printf("i                     xn1                             |f(xn1)| \n");
	fprintf(fp, "i                     xn1                             |f(xn1)| \n");
	while (n < Nmax) {
		temp = x0 - _f(x0) / ((_f(x0)-_f(x1))/(x0-x1));
		if (temp < EPSILON || fabs(x0 - temp) < DELTA)
			break;
		printf("%d   %30.15e    %30.15e\n", n, x0, fabs(_f(x0)));
		fprintf(fp, "%d   %30.15e    %30.15e\n", n, x0, fabs(_f(x0)));
		x0 = temp;
		n++;
	}


}
