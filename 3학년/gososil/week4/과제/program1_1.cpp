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
		
		printf("%-5d%30.15e    %30.15e\n", n, x1, fabs(_f(x1)));
		fprintf(fp, "%-5d%30.15e    %30.15e\n", n, x1, fabs(_f(x1)));
		if (fabs(_f(x1)) < DELTA || fabs(x0 - x1) < EPSILON)
			break;
		x0 = x1;
		n++;
	}
}
