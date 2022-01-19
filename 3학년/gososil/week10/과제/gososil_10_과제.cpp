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
float compute_time1, compute_time2;
float compute_time3_1, compute_time3_2;


#define MATDIM 1024
#define HW1_N 100000
#define HW3_N (1<<23)

#define mac(x,y) (x) > (y) ? (x) : (y)
float *hw1_x, hw1_E, hw1_var1, hw1_var2;
float hw2_a, hw2_b, hw2_c, hw2_naive_ans[2], hw2_pre_ans[2];
float* hw3_x;

/* hw1 */
void init_hw1(int fixed);
void hw1_calc_e();
void hw1_calc_var1();
void hw1_calc_var2();
/* hw2 */
void hw2_naive();
void hw2_safe();
float hw2_verify(float x);

/* hw3 */
void init_hw3();
void test1_1();
void test2_1();
void test3_1();
void test4_1();
void test5_1();

void test1_2();
void test2_2();
void test3_2();
void test4_2();
void test5_2();

float func(float, float);

void main(void)
{
	srand((unsigned)time(NULL));

	/* hw1 */
	puts("====== hw1 ======");
	init_hw1(1);

	// Æò±Õ °è»ê
	hw1_calc_e();

	CHECK_TIME_START;
	hw1_calc_var1();
	CHECK_TIME_END(compute_time);
	compute_time1 = compute_time;
	printf("hw1_calc_var1 = %f ms\n", compute_time);
	printf("hw1_calc_var1 value = %.15f\n", hw1_var1);


	CHECK_TIME_START;
	hw1_calc_var2();
	CHECK_TIME_END(compute_time);
	compute_time2 = compute_time;
	printf("hw1_calc_var2 = %f ms\n", compute_time);
	printf("hw1_calc_var2 value = %.15f\n", hw1_var2);
	puts("");
	
	/* hw2 */
	puts("====== hw2 ======");
	printf("a, b, c : ");
	scanf("%f %f %f", &hw2_a, &hw2_b, &hw2_c);
	hw2_naive();
	hw2_safe();

	printf("naive result    : %.15f, %.15f\n", hw2_naive_ans[0], hw2_naive_ans[1]);
	printf("advanced result : %.15f, %.15f\n", hw2_pre_ans[0], hw2_pre_ans[1]);
	puts("");

	printf("Verifying naive ans    : %.15f, %.15f\n", hw2_verify(hw2_naive_ans[0]), hw2_verify(hw2_naive_ans[1]));
	printf("Verifying advanced ans : %.15f, %.15f\n", hw2_verify(hw2_pre_ans[0]), hw2_verify(hw2_pre_ans[1]));
	puts("");

	/* hw3 */

	init_hw3();
	int i;
	void (*fp1[5])() = { test1_1,test2_1,test3_1,test4_1,test5_1 };
	void (*fp2[5])() = { test1_2,test2_2,test3_2,test4_2,test5_2 };
	const char* methods[5] = { "1. sum of (hw3_x^3)\n","2. sum of (hw3_x + noneed[0:10])\n","3. sum of hw3_x*2\n","4. sum of max(hw3_x[i],hw3_x[i+1])\n","5. sum of hw3_x[:10] iter 1<<23\n" };
	for (i = 0; i < 5; i++) {
		printf("%s", methods[i]);
		CHECK_TIME_START;
		fp1[i]();
		CHECK_TIME_END(compute_time);
		compute_time3_1 = compute_time;
		printf("test%d_1 = %f ms\n", i + 1, compute_time3_1);
		CHECK_TIME_START;
		fp2[i]();
		CHECK_TIME_END(compute_time);
		compute_time3_2 = compute_time;

		printf("test%d_2 = %f ms\n\n", i+1, compute_time3_2);

	}


}
// 1. Strength reduction 
void test1_1() {
	
	int i;
	float sum = 0;
	for (i = 0; i < HW3_N;i++) {
		sum += powf(hw3_x[i],3);
	}
	printf("\nResult without optimization1: %f\n", sum);
}
void test1_2() {

	int i;
	float sum = 0;
	for (i = 0; i < HW3_N; i++) {
		sum += hw3_x[i] * hw3_x[i] * hw3_x[i];
	}
	printf("Result with optimization1 : %f\n", sum);

}

// Loop-invariant code motion
void test2_1() {

	int i;
	float sum = 0;
	float noneed[10] = { 1,2,3,4,5,6,7,8,9,10 };
	for (i = 0; i < HW3_N; i++) {
		sum += hw3_x[i] + noneed[0] + noneed[1] + noneed[2] + noneed[3] + noneed[4] + noneed[5] + noneed[6] + noneed[7] + noneed[8] + noneed[9];
	}
	printf("\nResult without optimization2 : %f\n", sum);


}
void test2_2() {
	int i;
	float sum = 0;
	float noneed[10] = { 1,2,3,4,5,6,7,8,9,10 };
	float const opt = noneed[0] + noneed[1] + noneed[2] + noneed[3] + noneed[4] + noneed[5] + noneed[6] + noneed[7] + noneed[8] + noneed[9];
	for (i = 0; i < HW3_N; i++) {
		sum += hw3_x[i] + opt;
	}

	printf("Result with optimization2 : %f\n", sum);
}

// 3. const propagation
void test3_1() {

	int i,mul = 2;
	float sum =0 ;
	for (i = 0; i < HW3_N; i++) {
		sum += hw3_x[i]*mul;
	}
	printf("\nResult without optimization3 : %f\n", sum);

}
void test3_2() {
	int i;
	float sum =0 ;
	for (i = 0; i < HW3_N; i++) {
		sum += hw3_x[i]*2;
	}
	printf("Result with optimization3 : %f\n",sum);

	
}


// 4. Macro
float func(float a, float b) {
	return a > b ? a : b;
}
void test4_1() {

	int i;
	float sum = 0;
	for (i = 0; i < HW3_N; i+=2) {
		sum += func(hw3_x[i],hw3_x[i+1]);
	}
	printf("\nResult without optimization4 : %f\n", sum);
}
void test4_2() {

	int i;
	float sum = 0;
	for (i = 0; i < HW3_N; i+=2) {
		sum += mac(hw3_x[i],hw3_x[i+1]);
	}
	printf("Result with optimization4 : %f\n", sum);
}


// 5. count to zero
void test5_1() {

	int i,to;
	float sum = 0;
	for (int j = 0; j < HW3_N; j++) {
		sum = 0;
		for (i = 0; i < 10; i++) {
			sum += hw3_x[i];
		}
	}
	printf("Result without optimization5 : %f\n", sum);
}
void test5_2() {
	int i;
	float sum = 0;

	for (int j = 0; j < HW3_N; j++) {
		sum = 0;
		for (i = 9; i>=0; i --) {
			sum += hw3_x[i];
		}
	}
	printf("Result with optimization5 : %f\n", sum);

}
void init_hw3()
{
	float* ptr;
	hw3_x = (float*)malloc(sizeof(float) * HW3_N);

	srand((unsigned)time(NULL));
	ptr = hw3_x;
	for (int i = 0; i < HW3_N; i++)
		*ptr++ = ((float)rand() / (float)RAND_MAX);

}

void init_hw1(int fixed)
{
	float *ptr;
	hw1_x = (float *)malloc(sizeof(float)*HW1_N);

	if (fixed)
	{
		float tmp = HW1_N;
		for (int i = 0; i < HW1_N; i++)
		{
			if( i & 1) tmp -= 0.0001;
			hw1_x[i] = tmp;
		}
	}
	else
	{
		srand((unsigned)time(NULL));
		ptr = hw1_x;
		for (int i = 0; i < HW1_N; i++)
			*ptr++ = ((float)rand() / (float)RAND_MAX) * 2;
	}
}
void hw1_calc_e()
{
	int i;
	hw1_E = 0;
	for (i = 0; i < HW1_N; i++)
		hw1_E += hw1_x[i];

	hw1_E /= HW1_N;

}
void hw1_calc_var2()
{

	hw1_var2 = 0;
	int i;
	for (i = 0; i < HW1_N; i++) {
		hw1_var2 += (hw1_x[i] - hw1_E) * (hw1_x[i] - hw1_E);
	}

	hw1_var2 /= (HW1_N - 1);	
	hw1_var2 = (float)sqrt(hw1_var2);
}
void hw1_calc_var1()
{
	int i;
	float s1, s2;
	s1 = s2 = 0;
	hw1_var1 = 0;
	for (i = 0; i < HW1_N; i++) {
		s1 += (hw1_x[i] * hw1_x[i]);
		s2 += hw1_x[i];
	}
	hw1_var1 = (float)sqrt((HW1_N*s1-(s2*s2)) / (HW1_N * (HW1_N - 1)));
}


/* hw2 */
void hw2_naive()
{
	// (b^2-4ac)^1/2
	float root = (float)sqrt(hw2_b * hw2_b - 4 * hw2_a * hw2_c);
	float bunmo = 2 * hw2_a;
	hw2_naive_ans[0] = (-hw2_b + root) / bunmo;
	hw2_naive_ans[1] = (-hw2_b - root) / bunmo;

}
//float hw2_a, hw2_b, hw2_c, hw2_naive_ans[2], hw2_pre_ans[2];

void hw2_safe()
{
	float bunza = -4 * hw2_a * hw2_c;
	float root = (float)sqrt(hw2_b * hw2_b + bunza);
	float bunmo = 2 * hw2_a;
	hw2_pre_ans[0] = (bunza / (hw2_b + root))/bunmo;
	hw2_pre_ans[1] = (-hw2_b - root)/bunmo;
	//hw2_pre_ans[1] = ((bunza * (hw2_b + root)) / (-bunza))/bunmo;
	//hw2_pre_ans[1] = (bunza / (hw2_b - root)) / bunmo;

}
float hw2_verify(float x)
{
	return hw2_a * x * x + hw2_b*x + hw2_c;
}
