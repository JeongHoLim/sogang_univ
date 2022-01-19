#include <stdio.h>
#include <string.h>
#include <random>
#include <time.h>

#include <math.h>
#include <time.h>
#include <Windows.h>

__int64 start, freq, end;
#define CHECK_TIME_START QueryPerformanceFrequency((LARGE_INTEGER*)&freq); QueryPerformanceCounter((LARGE_INTEGER*)&start)
#define CHECK_TIME_END(a) QueryPerformanceCounter((LARGE_INTEGER*)&end); a = (float)((float)(end - start) / (freq / 1000.0f))
float compute_time;


#define MATDIM 1024
double *pMatA, *pMatB, *pMatC;
void MultiplySquareMatrices_1(double *pDestMatrix, double *pLeftMatrix, double *pRightMatrix, int MatSize);
void MultiplySquareMatrices_2(double *pDestMatrix, double *pLeftMatrix, double *pRightMatrix, int MatSize);
void MultiplySquareMatrices_3(double *pDestMatrix, double *pLeftMatrix, double *pRightMatrix, int MatSize);
void MultiplySquareMatrices_4(double *pDestMatrix, double *pLeftMatrix, double *pRightMatrix, int MatSize);

void init_MatMat(void);

void main(void)
{
	init_MatMat();

	CHECK_TIME_START;
	MultiplySquareMatrices_1(pMatC, pMatA, pMatB, MATDIM);
	//Sleep(500);
	CHECK_TIME_END(compute_time);
	printf("MultiplySquareMatrices_1 : %f ms\n", compute_time);

	CHECK_TIME_START;
	MultiplySquareMatrices_2(pMatC, pMatA, pMatB, MATDIM);
	//Sleep(500);
	CHECK_TIME_END(compute_time);
	printf("MultiplySquareMatrices_2 = %f ms\n", compute_time);

	CHECK_TIME_START;
	MultiplySquareMatrices_3(pMatC, pMatA, pMatB, MATDIM);
	//Sleep(500);
	CHECK_TIME_END(compute_time);
	printf("MultiplySquareMatrices_3 = %f ms\n", compute_time);

	CHECK_TIME_START;
	MultiplySquareMatrices_4(pMatC, pMatA, pMatB, MATDIM);
	//Sleep(500);
	CHECK_TIME_END(compute_time);
	printf("MultiplySquareMatrices_4 = %f ms\n", compute_time);
}

void MultiplySquareMatrices_1(double *pDestMatrix, double *pLeftMatrix ,double *pRightMatrix, int MatSize)
{
	int i, j, k;
	for (i = 0; i < MatSize; i++) {
		for (j = 0; j < MatSize; j++) {
			pDestMatrix[i * MatSize + j] = 0;
			for (k = 0; k < MatSize; k++) {
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k] * pRightMatrix[k * MatSize + j];
			}
		}
	}

}

void MultiplySquareMatrices_2(double *pDestMatrix, double *pLeftMatrix, double *pRightMatrix, int MatSize)
{
	
	int i, j,k;
	//memset(pDestMatrix, 0, sizeof(double) * MatSize * MatSize);

	for (j = 0; j < MatSize; j++) {
		for (i = 0; i < MatSize; i++) {
			pDestMatrix[i * MatSize + j] = 0;
			for (k = 0; k < MatSize; k++) {
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k] * pRightMatrix[k * MatSize + j];
			}
		}
	}

}

void MultiplySquareMatrices_3(double *pDestMatrix, double *pLeftMatrix, double *pRightMatrix, int MatSize)
{

	int i, j, k;
	int m = 16;
	//memset(pDestMatrix, 0, sizeof(double) * MatSize * MatSize);

	for (j = 0; j < MatSize; j++) {
		for (i = 0; i < MatSize; i++) {
			pDestMatrix[i * MatSize + j] = 0;
			for (k = 0; k < MatSize; k += m) {
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k] * pRightMatrix[k * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 1) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 2] * pRightMatrix[(k + 2) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 3] * pRightMatrix[(k + 3) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 3] * pRightMatrix[(k + 4) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 3] * pRightMatrix[(k + 5) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 3] * pRightMatrix[(k + 6) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 3] * pRightMatrix[(k + 7) * MatSize + j];

				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 8) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 9) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 10) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 11) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 12) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 13) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 14) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 15) * MatSize + j];
			}

		}
	}


}

void MultiplySquareMatrices_4(double *pDestMatrix, double *pLeftMatrix, double *pRightMatrix, int MatSize){

	int i, j, k;
	int m = 32;
	//memset(pDestMatrix, 0, sizeof(double) * MatSize * MatSize);

	for (j = 0; j < MatSize; j++) {
		for (i = 0; i < MatSize; i++) {
			pDestMatrix[i * MatSize + j] = 0;
			for (k = 0; k < MatSize; k += m) {
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k] * pRightMatrix[k * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 1) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 2) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 3) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 4) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 5) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 6) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 7) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 8) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 9) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 10) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 11) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 12) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 13) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 14) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 15) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 16) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 17) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 18) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 19) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 20) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 21) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 22) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 23) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 24) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 25) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 26) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 27) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 28) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 29) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 30) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 31) * MatSize + j];
				/*pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 32) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 33) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 34) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 35) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 36) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 37) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 38) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 39) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 40) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 41) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 42) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 43) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 44) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 45) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 46) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 47) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 48) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 49) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 50) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 51) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 52) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 53) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 54) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 55) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 56) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 57) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 58) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 59) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 60) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 61) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 62) * MatSize + j];
				pDestMatrix[i * MatSize + j] += pLeftMatrix[i * MatSize + k + 1] * pRightMatrix[(k + 63) * MatSize + j];*/
			}

		}
	}
}

void init_MatMat(void)
{
	double *ptr;
	pMatA = (double *)malloc(sizeof(double)*MATDIM*MATDIM);
	pMatB = (double *)malloc(sizeof(double)*MATDIM*MATDIM);
	pMatC = (double *)malloc(sizeof(double)*MATDIM*MATDIM);
	srand((unsigned)time(NULL));
	ptr = pMatA;
	for (int i = 0; i < MATDIM*MATDIM; i++)
		*ptr++ = (double)rand() / ((double)RAND_MAX);
	ptr = pMatB;
	for (int i = 0; i < MATDIM*MATDIM; i++)
		*ptr++ = (double)rand() / ((double)RAND_MAX);
}
