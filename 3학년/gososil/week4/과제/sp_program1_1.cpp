#include "my_solver.h"

extern float (*_sp_f)(float);
extern float (*_sp_fp)(float);

/*********************************************
  Newton-Rapson Method
**********************************************/
void sp_program1_1(FILE* fp) {
	float x0, x1;
	int n;

	if (fp == NULL)
		return;

	printf("init value : ");
	scanf("%f", &x0);

	n = 0;
	printf("i                     xn1                             |f(xn1)| \n");
	fprintf(fp, "i                     xn1                             |f(xn1)| \n");
	while (n < Nmax) {
		x1 = x0 - _sp_f(x0) / _sp_fp(x0);
		if (x1 < EPSILON || fabs(x0 - x1) < DELTA)
			break;
		printf("%-5d%30.15e    %30.15e\n", n, x0, (float)fabs(_sp_f(x0)));
		fprintf(fp, "%-5d%30.15e    %30.15e\n", n, x0, (float)fabs(_sp_f(x0)));
		x0 = x1;
		n++;
	}

}
