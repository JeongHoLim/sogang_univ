#include "my_solver.h"
#include<time.h>
#include<stdlib.h>
#include<math.h>

int num1;
double h1;

double _F_exp(double l, double x) {
	return x >= 0 ? 1 - exp(-l * x) : 0;
}

double _f1(double X, double l,double U) {

	return _F_exp(l,X) - U;
}
double bisection(double U,double l)
{
	double x0, temp;
	int n = 0;

	double a0 = 0, b0 = 1;
	x0 = a0;

	while (n < Nmax) {
		temp = a0 + (b0 - a0) / 2;
		if (_f1( a0,l ,U ) * _f1( temp,l,U) > 0) {
			a0 = temp;
		}
		else {
			b0 = temp;
		}
		if (fabs(_f1(temp,l, U)) < DELTA || fabs(x0 - temp) < EPSILON)
			break;
		x0 = temp;
		n++;
	}
	return temp;
}


// HOMEWORK
void program2_3_1()
{
	
	double lambda ;
	srand(time(NULL));
	int num,i;
	double th_E_X, th_V_X;
	double ret;
	double E_X, V_X ;

	for (int j = 0; j < 3; j++) {

		printf("람다 값 : ");
		scanf("%lf", &lambda);
		printf("난수 개수 : ");
		scanf("%d", &num);
		E_X = 0;
		V_X = 0;
		for (i = 0; i < num; i++) {
			ret = bisection(my_rand(), lambda);
			E_X += ret;
			V_X += (ret * ret);
		}

		E_X /= num;
		V_X /= num;
		V_X -= (E_X * E_X);

		th_E_X = (double)1 / lambda;
		th_V_X = (double)1 / (lambda * lambda);

		printf("\ncomputed E(X) : %lf\t", E_X);
		printf("computed V(X) : %lf\n", V_X);
		
		printf("ideal E(X)    : %lf\t", th_E_X);
		printf("ideal V(X)    : %lf\n\n", th_V_X);
		
		printf("E(X) diff     : %lf%%\tV(X) diff     : %lf%%\n", fabs(E_X - th_E_X) / th_E_X * 100, fabs(V_X - th_V_X) / th_V_X * 100);

		printf("\n************************************************\n");
	}
}
double _F2(double X,double* x,double* fx) {

	int i;

	double ret = 0;
	double plus = 0;
	int m = 0;
	i = 0;
	for (i = 0; i < num1; i++) {
		if (x[i] < X) continue;
		if (x[i] == X) {
			ret += fx[i];
			if (i > 1) ret += (fx[i - 1] - fx[0]);
			ret *= (h1 / 2);
		}
		else {
			ret += fx[i - 1];
			if (i > 2) ret += (fx[i - 2] - fx[0]);
			ret *= (h1 / 2);

			if (i == 1) {
				ret += (fx[0] + ((fx[1]-fx[0] - fx[0]) / (x[i] - x[i-1])) * ((X - x[i-1]) / 2)) * (X - x[i-1]);
			}
			else {
				ret += (fx[i-1]-fx[i-2] + ((fx[i]-fx[i-1] - (fx[i-1]-fx[i-2])) / (x[i] - x[i-1])) * ((X - x[i-1]) / 2)) * (X - x[i-1]);
			}
		}
		break;
	}
	return ret;
}
double _f2(double X, double U,double*x,double*fx) {

	return _F2(X,x,fx) - U;
}
// HOMEWORK
void program2_2_a()
{
	__int64 start, freq, end;
	float resultTime = 0;
	
	double* x, * fx;
	double a0, b0,x0;
	int n;
	int i, nr;
	double U, ret;
	FILE* fp_r, * fp_w;

	fp_r = fopen("pdf_table.txt", "r");	
	fp_w = fopen("random_event_table_2_2_a.txt", "w");

	srand(time(NULL));
	fscanf(fp_r, "%d %lf\n", &num1, &h1);

	x = (double*)malloc(sizeof(double) * num1);
	fx = (double*)malloc(sizeof(double) * num1);

	for (i = 0; i < num1; i++) {
		fscanf(fp_r, "%lf %lf\n", &x[i], &fx[i]);
		if (i > 0) fx[i] += fx[i - 1];
	}

	printf("\n***********program 2-2-a **********\n\n");
	printf("난수 개수 : ");
	scanf("%d", &nr);

	fprintf(fp_w, "%d\n", nr);

	CHECK_TIME_START;

	for (i = 0; i < nr; i++) {
		U = my_rand();
		n = 0;
		a0 = 0; b0 = 1; x0 = a0;
		while (n < Nmax) {
			ret = a0 + (b0 - a0) / 2;
			if (fabs(_f2(ret, U,x,fx)) < DELTA || fabs(x0 - ret) < EPSILON)
				break;
			
			if (_f2(a0, U,x,fx) * _f2(ret, U,x,fx) > 0) {
				a0 = ret;
			}
			else {
				b0 = ret;
			}
			x0 = ret;
			n++;
		}
		fprintf(fp_w, "%15.14e\n", ret);
	}

	free(x);
	free(fx);

	if (fp_r != NULL) fclose(fp_r);
	if (fp_w != NULL) fclose(fp_w);


	CHECK_TIME_END(resultTime);
	
	printf("The program2_2_a run time is %f(ms)..\n", resultTime * 1000.0);
}
void program2_2_b_1() {
	//secant  method
	__int64 start, freq, end;
	float resultTime = 0;

	double* x, * fx;
	double x0, x2, a0, b0;
	int n;
	int i, nr;
	double U, ret;

	FILE* fp_r, * fp_w;

	fp_r = fopen("pdf_table.txt", "r");
	fp_w = fopen("random_event_table_2_2_b_1.txt", "w");

	srand(time(NULL));
	fscanf(fp_r, "%d %lf\n", &num1, &h1);

	x = (double*)malloc(sizeof(double) * num1);
	fx = (double*)malloc(sizeof(double) * num1);

	for (i = 0; i < num1; i++) {
		fscanf(fp_r, "%lf %lf\n", &x[i], &fx[i]);
		if (i > 0) fx[i] += fx[i - 1];
	}

	printf("\n***********program 2-2-b_1(secant) **********\n\n");
	printf("난수 개수 : ");
	scanf("%d", &nr);

	fprintf(fp_w, "%d\n", nr);
	CHECK_TIME_START;

	for (i = 0; i < nr; i++) {
		U = my_rand();
		x0 = 0;
		a0 = 0; b0 = 1; x2 = 1;
		
		n = 0;
		while (n < Nmax) {
			ret = x0;
			x0 = x0 - _f2(x0, U, x, fx) * ((x0 - x2) / (_f2(x0, U, x, fx) - _f2(x2, U, x, fx)));
			if (fabs(_f2(x0, U, x, fx)) < DELTA || fabs(x0 - x2) < EPSILON)
				break;

			x2 = ret;
			n++;
		}
		ret = x0;
		fprintf(fp_w, "%15.14e\n", ret);
	}

	free(x);
	free(fx);

	if (fp_r != NULL) fclose(fp_r);
	if (fp_w != NULL) fclose(fp_w);


	CHECK_TIME_END(resultTime);

	printf("The program2_2_b_1(secant) run time is %f(ms)..\n", resultTime * 1000.0);
}

void program2_2_b_2()
{

	//secant bisection method
	__int64 start, freq, end;
	float resultTime = 0;

	double* x, * fx;
	double x0,x2,a0,b0;
	int n;
	int i, nr;
	double U, ret;
	
	FILE* fp_r, * fp_w;

	fp_r = fopen("pdf_table.txt", "r");
	fp_w = fopen("random_event_table_2_2_b_2.txt", "w");

	srand(time(NULL));
	fscanf(fp_r, "%d %lf\n", &num1, &h1);

	x = (double*)malloc(sizeof(double) * num1);
	fx = (double*)malloc(sizeof(double) * num1);

	for (i = 0; i < num1; i++) {
		fscanf(fp_r, "%lf %lf\n", &x[i], &fx[i]);
		if (i > 0) fx[i] += fx[i - 1];
	}

	printf("\n***********program 2-2-b_2(secant bisection) **********\n\n");
	printf("난수 개수 : ");
	scanf("%d", &nr);

	fprintf(fp_w, "%d\n", nr);
	CHECK_TIME_START;

	for (i = 0; i < nr; i++) {
		U = my_rand();
		x0 = 0;
		a0 = 0; b0 = 1; x2 = 0;
		for (int j = 0; j < 3; j++) {
			ret = a0 + (b0 - a0) / 2;
			
			if (fabs(_f2(ret, U,x,fx)) < DELTA || fabs(x2 - ret) < EPSILON)
				break;
			
			if (_f2(a0, U,x,fx) * _f2(ret, U,x,fx) > 0) {
				a0 = ret;
			}
			else {
				b0 = ret;
			}
			x2 = ret;
		}
		x2 = ret;

		n = 0;
		while (n < Nmax) {
			ret = x0;
			x0 = x0 - _f2(x0,U,x,fx) *((x0-x2)/ (_f2(x0,U,x,fx) - _f2(x2,U,x,fx)));
			if (fabs(_f2(x0, U, x, fx)) < DELTA || fabs(x0 - x2) < EPSILON)
				break;
			
			x2 = ret;
			n++;
		}
		ret = x0;
		fprintf(fp_w, "%15.14e\n", ret);
	}

	free(x);
	free(fx);

	if (fp_r != NULL) fclose(fp_r);
	if (fp_w != NULL) fclose(fp_w);


	CHECK_TIME_END(resultTime);

	printf("The program2_2_b_2(secant bisection) run time is %f(ms)..\n", resultTime * 1000.0);
}
void program2_2_c_1() {
	//newton method

	__int64 start, freq, end;
	float resultTime = 0;


	double* x, * fx;
	int n;
	int i, nr;
	double U, ret;

	FILE* fp_r, * fp_w;

	fp_r = fopen("pdf_table.txt", "r");
	fp_w = fopen("random_event_table_2_2_c_1.txt", "w");

	srand(time(NULL));
	fscanf(fp_r, "%d %lf\n", &num1, &h1);

	x = (double*)malloc(sizeof(double) * num1);
	fx = (double*)malloc(sizeof(double) * num1);
	
	for (i = 0; i < num1; i++) {
		fscanf(fp_r, "%lf %lf\n", &x[i], &fx[i]);
		if (i > 0) fx[i] += fx[i - 1];
	}


	printf("\n***********program 2-2-c(newton) **********\n\n");
	printf("난수 개수 : ");
	scanf("%d", &nr);
	double x0, x1;
	int j = 0;
	double s;
	double pxi, pxi_1;
	fprintf(fp_w, "%d\n", nr);
	CHECK_TIME_START;

	int l = 0, r = num1 - 1;
	int mid;
	int S = 0;
	for (i = 0; i < nr; i++) {
		U = my_rand();		

		n = 0;
		x0 = 0.5;
		while (n < Nmax) {
			j = 0;
			l = 0; r = num1 - 1;
			S = 0;
			while (x0 > x[j])j++;
			
			mid = j - 1;;
			s = (x0 - x[mid]) / (x[mid+1] - x[mid]);
			if (mid > 0) S = 1;
			
			pxi = fx[mid] - fx[mid - S];
			pxi_1 = fx[mid + 1] - fx[mid];
			
			x1 = x0 - _f2(x0,U,x,fx) / ((1-s)*pxi + s*pxi_1);
			if (fabs(_f2(x1,U,x,fx)) < DELTA || fabs(x0 - x1) < EPSILON)
				break;
			x0 = x1;
			n++;
		}
		ret = x0;
		fprintf(fp_w, "%15.14e\n", ret);

	}


	free(x);
	free(fx);

	if (fp_r != NULL) fclose(fp_r);
	if (fp_w != NULL) fclose(fp_w);



	CHECK_TIME_END(resultTime);

	printf("The program2_2_c(newton) run time is %f(ms)..\n", resultTime * 1000.0);

}
void program2_2_c_2() {
	//newton bisection method

	__int64 start, freq, end;
	float resultTime = 0;


	double* x, * fx;
	int n;
	int i, nr;
	double U, ret;

	FILE* fp_r, * fp_w;

	fp_r = fopen("pdf_table.txt", "r");
	fp_w = fopen("random_event_table_2_2_c_2.txt", "w");

	srand(time(NULL));
	fscanf(fp_r, "%d %lf\n", &num1, &h1);

	x = (double*)malloc(sizeof(double) * num1);
	fx = (double*)malloc(sizeof(double) * num1);
	int max = 0;
	double target;
	for (i = 0; i < num1; i++) {
		fscanf(fp_r, "%lf %lf\n", &x[i], &fx[i]);
		if (fx[max] < fx[i]) max = i;

		if (i > 0) fx[i] += fx[i - 1];
	}
	target = _F2(x[max], x, fx);

	printf("\n***********program 2-2-c(newton-bisection) **********\n\n");
//	printf("target : %lf x : %lf y : %lf\n", target,x[max],fx[max]);

	printf("난수 개수 : ");
	scanf("%d", &nr);
	double x0, x1,x2;
	double a0, b0;
	int j = 0;
	double s;
	double pxi, pxi_1;
	fprintf(fp_w, "%d\n", nr);
	CHECK_TIME_START;

	int l = 0, r = num1 - 1;
	int mid;
	int S = 0;
	for (i = 0; i < nr; i++) {
		U = my_rand();

		x0 = 0;
		a0 = 0; b0 = 1; x2 = 0;
		for (int j = 0; j < 4; j++) {
			ret = a0 + (b0 - a0) / 2;

			if (_f2(a0, U, x, fx) * _f2(ret, U, x, fx) > 0) {
				a0 = ret;
			}
			else {
				b0 = ret;
			}
		}
		x0 = ret;
		n = 0;

		while (n < Nmax) {
			j = 0;

			S = 0;

			while (x0 > x[j])
				j++;
			mid = j - 1;

			s = (x0 - x[mid]) / (x[mid + 1] - x[mid]);
			
			if (mid == 0) pxi = fx[mid];
			else pxi = fx[mid] - fx[mid - 1];

			pxi_1 = fx[mid + 1] - fx[mid];
			x1 = x0 - _f2(x0, U, x, fx) / ((1 - s) * pxi + s * pxi_1);
			
			if (fabs(_f2(x1, U, x, fx)) < DELTA || fabs(x0 - x1) < EPSILON)
				break;
			x0 = x1;
			n++;
		}
		ret = x0;
		fprintf(fp_w, "%15.14e\n", ret);
		//printf("%15.14e\n", ret);

	}


	free(x);
	free(fx);

	if (fp_r != NULL) fclose(fp_r);
	if (fp_w != NULL) fclose(fp_w);

	CHECK_TIME_END(resultTime);

	printf("The program2_2_c(newton-bisection) run time is %f(ms)..\n", resultTime * 1000.0);

}
void program2_3() {
	
	program2_3_1();
	program2_2_a();
	program2_2_b_1();
	program2_2_b_2();
	program2_2_c_1();
	program2_2_c_2();
	FILE* fp_r, *fp_a,*fp_b_1,*fp_b_2,*fp_w_a,*fp_w_b_1,*fp_w_b_2;
	FILE* fp_w_c_1, * fp_w_c_2,*fp_c_1,*fp_c_2;
	fp_r = fopen("pdf_table.txt", "r");
	fp_a = fopen("random_event_table_2_2_a.txt", "r");
	fp_b_1 = fopen("random_event_table_2_2_b_1.txt", "r");
	fp_b_2 = fopen("random_event_table_2_2_b_2.txt", "r");
	fp_c_1 = fopen("random_event_table_2_2_c_1.txt", "r");
	fp_c_2 = fopen("random_event_table_2_2_c_2.txt", "r");

	fp_w_a = fopen("histogram_a.txt", "w");
	fp_w_b_1 = fopen("histogram_b_1.txt", "w");
	fp_w_b_2 = fopen("histogram_b_2.txt", "w");
	fp_w_c_1 = fopen("histogram_c_1.txt", "w");
	fp_w_c_2 = fopen("histogram_c_2.txt", "w");

	double x;
	int i,num;
	int N = 1000;
	int * count = (int*)malloc(sizeof(int)*N);


	// 2_2_a 먼저
	memset(count, 0, sizeof(int) * N);
	fscanf(fp_a, "%d", &num);
	int tmp;
	for (i = 0; i < num; i++) {
		fscanf(fp_a, "%lf", &x);
		tmp = int(x * N) % N;
		count[tmp] ++;
	}
	for (i = 0; i < N; i++) {
		fprintf(fp_w_a, "%d\n", count[i]);
	}


	// 2_2_b 차례
	memset(count, 0, sizeof(int) * N);
	
	fscanf(fp_b_1, "%d", &num);
	
	for (i = 0; i < num; i++) {
		fscanf(fp_b_1, "%lf", &x);
		tmp = int(x * N) % N;
		count[tmp] ++;
	}
	
	for (i = 0; i < N; i++) {
		fprintf(fp_w_b_1, "%d\n", count[i]);
	}
	memset(count, 0, sizeof(int) * N);

	fscanf(fp_b_2, "%d", &num);

	for (i = 0; i < num; i++) {
		fscanf(fp_b_2, "%lf", &x);
		tmp = int(x * N) % N;
		count[tmp] ++;
	}

	for (i = 0; i < N; i++) {
		fprintf(fp_w_b_2, "%d\n", count[i]);
	}

	memset(count, 0, sizeof(int) * N);

	fscanf(fp_c_1, "%d", &num);

	for (i = 0; i < num; i++) {
		fscanf(fp_c_1, "%lf", &x);
		tmp = int(x * N) % N;
		count[tmp] ++;
	}

	for (i = 0; i < N; i++) {
		fprintf(fp_w_c_1, "%d\n", count[i]);
	}


	memset(count, 0, sizeof(int) * N);

	fscanf(fp_c_2, "%d", &num);

	for (i = 0; i < num; i++) {
		fscanf(fp_c_2, "%lf", &x);
		tmp = int(x * N) % N;
		count[tmp] ++;
	}

	for (i = 0; i < N; i++) {
		fprintf(fp_w_c_2, "%d\n", count[i]);
	}



	if (fp_w_c_1 != NULL) fclose(fp_w_c_1);
	if (fp_w_c_2 != NULL) fclose(fp_w_c_2);


	free(count);
	if (fp_r != NULL) fclose(fp_r);
	if (fp_w_a != NULL) fclose(fp_w_a);
	if (fp_w_b_1 != NULL) fclose(fp_w_b_1);
	if (fp_w_b_2 != NULL) fclose(fp_w_b_2);
	if (fp_a != NULL) fclose(fp_a);	
	if (fp_b_1 != NULL) fclose(fp_b_1);
	if (fp_w_b_2 != NULL) fclose(fp_w_b_2);
}