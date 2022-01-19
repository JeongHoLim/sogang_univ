#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

#include "devices/shutdown.h"
#include "devices/input.h"
#include "threads/vaddr.h"
#include "process.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}
void 
check_address(void* addr){		//kernel 영역이면 -1 뿜고 exit
	if(is_kernel_vaddr(addr)){		
		exit(-1);
	}

}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  //printf ("system call!\n");
	//Two new system calls(fibonacci,max_of_four_int)
	//halt, exit,exec,wait,read(stdin), write(stdout)
	
	//printf("num : %d\n",*(int*)(f->esp));

	//f->esp로 함수에 값을 넘겨주기 전에 valid한 주소인지 확인 
	check_address(f->esp);
	switch(*(int*)(f->esp)){		//syscall number 확인

		case SYS_HALT:		//halt
			//0
			halt();
			break;

		case SYS_EXIT:		//exit
			//1	
			check_address(f->esp+4);
			exit(*(int*)(f->esp+4));
			break;

		case SYS_EXEC:		//exec
			//1
			check_address(f->esp+4);
			//printf("======%s =====\n",(char*)(uint8_t*)(f->esp+4));
			f->eax = exec((const char*)*(uint32_t*)(f->esp+4));		//리턴값은 eax에 저장
			break;

		case SYS_WAIT:		//wait
			//1
			check_address(f->esp+4);
			f->eax = wait(*(int*)(f->esp+4));
			break;

		case SYS_CREATE:
			//2
			break;

		case SYS_REMOVE:
			//1
			break;
		
		case SYS_OPEN:	
			//1 65 63 68 6f-20
			//invalid pointer reject 해야됨
			break;

		case SYS_FILESIZE:	
			//1
			break;

		case SYS_READ:
			//3
			// 주소 확인
			check_address(f->esp+4);
			check_address(f->esp+8);
			check_address(f->esp+12);

			f->eax = read(*(int*)(f->esp+4),(void*)*(uint32_t*)(f->esp+8),*(unsigned*)(f->esp+12));
			break;
		
		case SYS_WRITE:
			//3
			//주소 확인
			check_address(f->esp+4);
			check_address(f->esp+8);
			check_address(f->esp+12);
			f->eax = write(*(int*)(f->esp+4),(const void*)*(uint32_t*)(f->esp+8),(unsigned)*(uint32_t*)(f->esp+12));
			break;
		
		case SYS_SEEK:
			//2
			break;

		case SYS_TELL:
			//1
			break;

		case SYS_CLOSE:
			//1
			break;		
		case SYS_FIB:		//additional의 fibonacci()호출
//	hex_dump(f->esp,f->esp,100,1);
			check_address(f->esp+4);
			f->eax = fibonacci(*(int*)(f->esp+4));
			break;
		case SYS_MAF:		//additional의 max_of_four_int() 호출
//	hex_dump(f->esp,f->esp,100,1);
			check_address(f->esp+4);
			check_address(f->esp+8);
			check_address(f->esp+12);
			check_address(f->esp+16);
			f->eax = max_of_four_int(*(int*)(f->esp+4),*(int*)(f->esp+8),*(int*)(f->esp+12),*(int*)(f->esp+16));

			break;
		//defalut:
		//	break;

	}

//	thread_exit ();
}


/* 핀토스 종료 */
void halt(){

	shutdown_power_off();
}
/* 현재 종료되는 쓰레드의 exit status를 받아, 쓰레드의 이름과 exit status 출력
	쓰레드 구조체의 exit_status 저장 -> process_wait()에서 사용하기 위함
*/
void exit(int status){
	printf("%s: exit(%d)\n",thread_name(),status);	
	thread_current()->exit_status = status;
	thread_exit();
}

/*
	fd가 stdout일 때,  buffer에 있는 내용을 size만큼 출력
*/
int write(int fd,const void* buffer, unsigned size){
	if(fd == 1){
		putbuf(buffer,size);
		return size;
	}
	return -1;
}
/*
	fd가 stdin일 때, buffer에 size만큼 입력
*/
int read(int fd,void* buffer, unsigned size){
	char x;
	unsigned int cnt;
	if(fd == 0){
		for(cnt =0;cnt<size;cnt++){
			x = input_getc();
			if( x == '\0') break;
			*(char*)buffer++ = x;
		}
		return cnt;
	}
	return -1;

}
/*
	매개변수로 받은 파일 실행하여 process id를 리턴
*/
pid_t exec(const char* file){

	return process_execute(file);

}
/*
	매개변수로 받은 pid를 wait한다.
	기다린 프로세스의 exit status를 리턴한다.
*/
int wait(pid_t pid){
	
	return process_wait(pid);

}
// 피보나치 함수
int fibonacci(int n){

	if(n<=1)
		return n;
	else
		return fibonacci(n-1)+fibonacci(n-2);
}
//4개의 정수에서 가장 큰 값을 리턴
int max_of_four_int(int a,int b,int c,int d){

	int big;
	big = a > b ? a: b;
	big = big > c ? big : c;
	big = big > d ? big : d;
	return big;

}





