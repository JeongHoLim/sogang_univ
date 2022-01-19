#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int Max(int x, int y, int* flag) {
	if (x > y) {
		*flag = 0;
		return x;
	}
	else {
		*flag = 1;
		return y;
	}
}
int Function_one(int arr[], int low, int high, int* lIdx, int* rIdx) {

	int i, sum, mid, left, right, temp, max = INT_MIN;
	int local_left_idx=0, local_right_idx=0, flag;
	int check_lIdx[2] = { 0,0 };
	int check_rIdx[2] = { 0,0 }; 
	int cmp;
	if (low == high) {
		*lIdx = *rIdx = low;
		return arr[low];
	}

	mid = (low + high) / 2;

	sum = 0;
	for (i = mid; i >= low; i--) {
		sum += arr[i];
		if (sum > max) {
			max = sum;
			local_left_idx = i;
		}
	}
	left = max;
	sum = 0;
	max = INT_MIN;
	for (i = mid + 1; i <= high; i++) {
		sum += arr[i];
		if (sum > max) {
			max = sum;
			local_right_idx = i;
		}
	}
	right = max;

	temp = Max(Function_one(arr, low, mid, &check_lIdx[0], &check_rIdx[0]), Function_one(arr, mid + 1, high, &check_lIdx[1], &check_rIdx[1]), &flag);
	cmp = flag;
	temp = Max(temp, left + right, &flag);
	if (flag == 0) {
		*lIdx = check_lIdx[cmp];
		*rIdx = check_rIdx[cmp];
	}
	else {
		*lIdx = local_left_idx;
		*rIdx = local_right_idx;
	}
	return temp;

}
int Function_two(int* arr, int n, int* lIdx, int* rIdx) {
	int i, sum = 0, max = INT_MIN;
	int L = 0;
	int flag = 0;
	for (i = 0; i < n; i++) {
		sum += arr[i];
		if (sum < 0) {
			sum = 0;
			L = i + 1;
		}
		else if (sum > max) {
			max = sum;
			*lIdx = L;
			*rIdx = i;
			flag = 1;
		}
	}
	if (flag == 1) return max;
	max = arr[0];
	*lIdx = *rIdx = 0;
	for (i = 1; i < n; i++) {
		if (arr[i] > max) {
			*lIdx = *rIdx = i;
			max = arr[i];
		}
	}
	return max;
}
int Function_three(int** arr, int n, int* K, int* L, int* I, int* J) {
	int temp, i, j, max, k, l;
	int** T = (int**)malloc(sizeof(int*) * n);
	for (i = 0; i < n; i++) T[i] = (int*)malloc(sizeof(int) * n);

	for (i = 0; i < n; i++) {
		temp = 0;
		for (j = 0; j < n; j++) {
			temp += arr[i][j];
			if (i > 0) {
				T[i][j] = T[i - 1][j] + temp;
			}
			else {
				T[i][j] = temp;
			}
		}
	}
	max = INT_MIN;
	for (k = 0; k < n; k++) {
		for (l = k; l < n; l++) {
			for (i = 0; i < n; i++) {
				for (j = i; j < n; j++) {
					if (k > 0 && i > 0) {
						temp = T[l][j] - (T[k - 1][j] + T[l][i - 1]) + T[k - 1][i - 1];
					}
					else if (k > 0) {
						temp = T[l][j] - T[k - 1][j];

					}
					else if (i > 0) {
						temp = T[l][j] - T[l][i - 1];
					}
					else {
						temp = T[l][j];
					}
					if (max < temp) {
						max = temp;
						*K = k; *L = l; *I = i; *J = j;
					}
				}
			}

		}
	}
	for (i = 0; i < n; i++) free(T[i]);
	free(T);
	return max;
}
int Function_four(int** arr, int n, int* K, int* L, int* I, int* J) {

	
	int i, j, k, sum, max = INT_MIN;
	int l, s;
	int* temp = (int*)malloc(sizeof(int) * n);
	for (i = 0; i < n; i++) {
		memset(temp, 0, sizeof(int) * n);
		for (j = i; j < n; j++) {
			for (s = 0; s < n; s++)
				temp[s] += arr[s][j];
			sum = Function_one(temp, 0, n - 1, &k, &l);
			if (sum > max) {
				max = sum;
				*K = k; *L = l; *I = i; *J = j;
			}
		}
	}
	free(temp);
	return max;

}
int Function_five(int** arr, int n, int* K, int* L, int* I, int* J) {

	int i, j, k, sum, max = INT_MIN;
	int l, s;
	int* temp = (int*)malloc(sizeof(int) * n);
	for (i = 0; i < n; i++) {
		memset(temp, 0, sizeof(int) * n);
		for (j = i; j < n; j++) {
			for (s = 0; s < n; s++)
				temp[s] += arr[s][j];
			sum = Function_two(temp, n, &k, &l);
			if (sum > max) {
				max = sum;
				*K = k; *L = l; *I = i; *J = j;
			}
		}
	}
	free(temp);
	return max;
}
int main() {

	int num, i, j, opt, n;
	int S;
	int R,I, J, L, K;
	int* arr1, ** arr2;
	char input[35], output[35];

	FILE* fp = fopen("HW1_config.txt", "rt");
	FILE* Ip, * Op;

	fscanf(fp, "%d", &num);

	for (i = 0; i < num; i++) {
		fscanf(fp, "%d %s %s", &opt, input, output);
		Ip = fopen(input, "rb");
		Op = fopen(output, "wb");
		fread(&n, sizeof(int), 1, Ip);
		if (opt < 3) {
			arr1 = (int*)malloc(sizeof(int)*n);
			fread(arr1, sizeof(int), n, Ip);
		}
		else {
			arr2 = (int**)malloc(sizeof(int*) * n);
			for (int s = 0; s < n; s++) arr2[s] = (int*)malloc(sizeof(int) * n);
			for (j = 0; j < n; j++)
				fread(arr2[j], sizeof(int), n, Ip);
		}
		switch (opt)
		{
		case 1: {
			S = Function_one(arr1, 0, n - 1, &L, &R);
			break;
		}
		case 2: {
			S = Function_two(arr1, n, &L, &R);
			break;
		}
		case 3: {
			S = Function_three(arr2, n, &K, &L, &I, &J);
			break;
		}
		case 4: {
			S = Function_four(arr2, n, &K, &L, &I, &J);
			break;
		}
		case 5: {
			S = Function_five(arr2, n, &K, &L, &I, &J);
			break;
		}
		default:
			break;
		}
		if (opt < 3) {
			free(arr1);
			fwrite(&S, sizeof(int), 1, Op);
			fwrite(&L, sizeof(int), 1, Op);
			fwrite(&R, sizeof(int), 1, Op);
		}
		else {
			for (int x = 0; x < n; x++) {
				free(arr2[x]);
				arr2[x] = NULL;
			}
			free(arr2);
			arr2 = NULL;
			fwrite(&S, sizeof(int), 1, Op);
			fwrite(&K, sizeof(int), 1, Op);
			fwrite(&I, sizeof(int), 1, Op);
			fwrite(&L, sizeof(int), 1, Op);
			fwrite(&J, sizeof(int), 1, Op);
		}
		fclose(Ip);
		fclose(Op);
		
	}

	fclose(fp);
	return 0;
}