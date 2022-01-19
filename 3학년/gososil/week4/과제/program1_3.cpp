#include "my_solver.h"

extern double (*_f)(double);
extern double (*_fp)(double);

/*********************************************
  Bisection Method -- HOMEWORK
**********************************************/
void program1_3(FILE *fp)
{
	double a0, b0, x0, x1 , temp;
	int n;

	printf("input(a0 b0): ");
	scanf("%lf %lf", &a0, &b0);

	n = 0;
	printf("i                     xn1                             |f(xn1)| \n");
	fprintf(fp, "i                     xn1                             |f(xn1)| \n");
	x0 = b0;
	while (n < Nmax) {
		temp = a0 +  (b0-a0) / 2;

		if (_f(a0)*_f(temp) > 0) {
			a0 = temp;
		}
		else {
			b0 = temp;
		}
		
		printf("%-5d%30.15e    %30.15e\n", n, x0, fabs(_f(x0)));
		fprintf(fp, "%-5d%30.15e    %30.15e\n", n, x0, fabs(_f(x0)));
		if (fabs(_f(temp)) < DELTA|| fabs(x0 - temp) < EPSILON)
			break;
		x0 = temp;
		n++;
	}


}