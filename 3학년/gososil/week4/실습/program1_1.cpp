#include "my_solver.h"

extern double (*_f)(double);
extern double (*_fp)(double);

/*********************************************
  Newton-Rapson Method
**********************************************/
void program1_1(FILE* fp) {
	double x0, x1;
	int n;

	if (fp == NULL)
		return;

	printf("init value : ");
	scanf("%lf", &x0);
	
	n = 0;
	printf("i                     xn1                             |f(xn1)| \n");
	fprintf(fp, "i                     xn1                             |f(xn1)| \n");
	while (n < Nmax) {
		x1 = x0 - _f(x0) / _fp(x0);
		if (x1 < EPSILON || fabs(x0 - x1) < DELTA)
			break;
		printf("%d   %30.15e    %30.15e\n", n, x0, fabs(_f(x0)));
		fprintf(fp, "%d   %30.15e    %30.15e\n", n, x0, fabs(_f(x0)));
		x0 = x1;
		n++;
	}
}
