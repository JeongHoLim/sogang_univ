#include "my_solver.h"
#define index_2d(row, col, dim) row*dim+col


void g_3_4() {

    int i, ia, j, n;

    float* a, * b, * x, * s;
    int *l;
    char readfile[256], writefile[256];
    //int op;
    //int file_num;
 
    /*printf("G -> 0, H -> 1 : ");
    scanf("%d", &op);
    printf("file_num: ");
    scanf("%d", &file_num);

    if (op == 0) {
        sprintf(readfile, "General_%d.txt",file_num);
        sprintf(writefile, "output_General_%d.txt", file_num);
    }
    else {
        sprintf(readfile, "Hilbert_%d.txt", file_num);
        sprintf(writefile, "output_Hilbert_%d.txt", file_num);
    }*/
    sprintf(readfile, "linear_system_3-4.txt");
    sprintf(writefile, "solution_3-4.txt");
    
    FILE* fp_r = fopen(readfile, "r");
    FILE* fp_w = fopen(writefile, "w");

    if (fp_r == NULL) {
        printf("file open error...\n");
        return;
    }
    if (fp_w == NULL) {
        printf("file open error...\n");
        return;
    }

    fscanf(fp_r, "%d", &n);

    a = (float*)malloc(sizeof(float)*(n*n));
    b = (float*)malloc(sizeof(float) * n);
    s = (float*)malloc(sizeof(float) * n);
    x = (float*)malloc(sizeof(float) * n);
    l = (int*)malloc(sizeof(int) * n);

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            fscanf(fp_r, "%f", &a[index_2d(j, i, n)]);
        }
    }
    for (int i = 0; i < n; i++)
        fscanf(fp_r, "%f", &b[i]);

    ia = n;

    gespp_(&n, a, &ia, l, s);

    solve_(&n, a, &ia, l, b, x);

    fprintf(fp_w, "n : %d\n", n);
    for (i = 0; i < n; i++)
        fprintf(fp_w, "%f\n", x[i]);

    float B = 0;
    float Ax_b = 0;
    float tmp = 0;
    float tmpA = 0;
    float tmpB = 0;
    for (i = 0; i < n; i++) {
        tmpB += powf(b[i], 2.0);
        for (j = 0; j < n; j++) {
            tmp += a[index_2d(j,i,n)] * x[j];
        }
        tmp -= b[i];
        tmpA +=powf(tmp,2.0);
        tmp = 0;
    }
    Ax_b = sqrtf(tmpA);
    B = sqrtf(tmpB);
    fprintf(fp_w, "Error : %.6f\n", Ax_b / B);

    /*printf("***** Solution *****\n");
    
    for (i = 0; i < n; i++)
        printf("%f ", x[i]);*/

       
    free(a);
    free(b);
    free(s);
    free(x);
    free(l);
    fclose(fp_r);
    fclose(fp_w);
 
}