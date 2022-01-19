#include<stdio.h>
#include<stdlib.h>
#include<syscall.h>
int main(int argc,char* argv[]){
	if(argc == 5){
		printf("%d %d\n",fibonacci(atoi(argv[1])),max_of_four_int(atoi(argv[1]),atoi(argv[2]),atoi(argv[3]),atoi(argv[4])));
	}
	else{
      printf ("usage: ./additional arg1 arg2 arg3 arg4\n");
      return EXIT_FAILURE;

	}

	return EXIT_SUCCESS;
}
