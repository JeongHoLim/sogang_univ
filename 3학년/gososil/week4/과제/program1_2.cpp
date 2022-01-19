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
		temp = x1;
		x1 = x1 - _f(x1) * ((x0-x1)/ (_f(x0) - _f(x1)));
		
		printf("%-5d%30.15e    %30.15e\n", n, x1, fabs(_f(x1)));
		fprintf(fp, "%-5d%30.15e    %30.15e\n", n, x1, fabs(_f(x1)));
		if (fabs(_f(x1)) < DELTA || fabs(x0 - x1) < EPSILON)
			break;
		x0 = temp;

		n++;

	}


}
