#define _USE_MATH_DEFINES
#include <math.h>

double _f1(double x) {
	return pow(x,2) - 4*x + 4 - log(x);
}

double _fp1(double x) {
	return 2*x -4 - 1/x;
}

double _f2(double x) {

	return x + 1 - 2* sin(M_PI*x);
}

double _fp2(double x) {

	return 1 - 2 * M_PI * cos(M_PI * x);
}

double _f3(double x) {
	return pow(x, 4.0) - 11 * pow(x, 3.0) + 42.35 * pow(x, 2.0) - 66.55 * x + 35.1384;
}

double _fp3(double x) {
	return 4 * pow(x, 3) - 33 * pow(x, 2) + 84.7 * x - 66.55;
}

double _f_sqrt(double x) {
	return x * x - 2.0;
}

double _fp_sqrt(double x) {
	return 2.0 * x;
}

double _f_vehicle(double x) {
	
	double A, B, C, E;
	double l, h, D, beta;
	l = 89; h = 49;
	D = 55; beta = (11.5/180)*M_PI;
	A = l * sin(beta);
	B = l * cos(beta);
	C = (h + 0.5 * D) * sin(beta) - 0.5 * D * tan(beta);
	E = (h + 0.5 * D) * cos(beta) - 0.5 * D;
	
	return A*sin(x * (M_PI / 180))*cos(x * (M_PI / 180)) + B*pow(sin(x * (M_PI / 180)),2)-C*cos(x * (M_PI / 180))-E*sin(x * (M_PI / 180));
}

double _fp_vehicle(double x) {
	double A, B, C, E;
	double l, h, D, beta;
	l = 89; h = 49;
	D = 55; beta = (11.5 / 180) * M_PI;
	A = l * sin(beta);
	B = l * cos(beta);
	C = (h + 0.5 * D) * sin(beta) - 0.5 * D * tan(beta);
	E = (h + 0.5 * D) * cos(beta) - 0.5 * D;

	return -A*pow(sin(x*M_PI/180),2) + A*pow(cos(x * (M_PI / 180)),2) + B*sin(2*(x*M_PI / 180)) + C*sin(x * (M_PI / 180))-E*cos(x * (M_PI / 180));
}

double _f_comp(double x) {
	return log(x) - 1;
}

double _fp_comp(double x) {
	return 1/x;
}
