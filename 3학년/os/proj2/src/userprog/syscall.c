#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

#include "devices/shutdown.h"
#include "devices/input.h"
#include "threads/vaddr.h"
#include "process.h"

#include "filesys/filesys.h"
#include "filesys/file.h"

struct lock file_lock;
static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
	lock_init(&file_lock);
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}
void 
check_address(void* addr){		//kernel 영역이면 -1 뿜고 exit
	if(is_kernel_vaddr(addr)){		
		exit(-1);
	}

}
void check(void* addr,int argc){
	
	for(int i=0;i<argc;i++){
		check_address(addr+(4*i));
	}

}
static void
syscall_handler (struct intr_frame *f UNUSED) 
{

	//f->esp로 함수에 값을 넘겨주기 전에 valid한 주소인지 확인 
//	check_address(f->esp);
	check(f->esp,1);
	switch(*(int*)(f->esp)){		//syscall number 확인

		case SYS_HALT:		//halt
			//0
			halt();
			break;

		case SYS_EXIT:		//exit
			//1	
			check(f->esp+4,1);
			exit(*(int*)(f->esp+4));
			break;

		case SYS_EXEC:		//exec
			//1
			check(f->esp+4,1);
			f->eax = exec((const char*)*(uint32_t*)(f->esp+4));		//리턴값은 eax에 저장
			break;

		case SYS_WAIT:		//wait
			//1
			check(f->esp+4,1);
			f->eax = wait(*(int*)(f->esp+4));
			break;

		case SYS_CREATE:
			//2
			check(f->esp+4,2);
			f->eax = create((const char*)*(uint32_t*)(f->esp+4),*(unsigned*)(f->esp+8));
			break;

		case SYS_REMOVE:
			//1
			check(f->esp+4,1);
			f->eax = remove((const char*)*(uint32_t*)(f->esp+4));
			break;
		
		case SYS_OPEN:	
			//1 65 63 68 6f-20
			//invalid pointer reject 해야됨
			check(f->esp+4,1);
			f->eax = open((const char*)*(uint32_t*)(f->esp+4));
			break;

		case SYS_FILESIZE:	
			//1
			check(f->esp+4,1);
			f->eax = filesize(*(int*)(f->esp+4));
			break;

		case SYS_READ:
			//3
			// 주소 확인
			check(f->esp+4,3);
			
			f->eax = read(*(int*)(f->esp+4),(void*)*(uint32_t*)(f->esp+8),*(unsigned*)(f->esp+12));
			break;
		
		case SYS_WRITE:
			//3
			//주소 확인
			check(f->esp+4,3);
			f->eax = write(*(int*)(f->esp+4),(const void*)*(uint32_t*)(f->esp+8),(unsigned)*(uint32_t*)(f->esp+12));
			break;
		
		case SYS_SEEK:
			//2
			check(f->esp+4,2);
			
			seek(*(int*)(f->esp+4),(unsigned)*(uint32_t*)(f->esp+8));

			break;

		case SYS_TELL:
			//1

			check(f->esp+4,1);
			f->eax = tell(*(int*)(f->esp+4));
			break;

		case SYS_CLOSE:
			//1
			check(f->esp+4,1);
			close(*(int*)(f->esp+4));

			break;		
		case SYS_FIB:		//additional의 fibonacci()호출
			check(f->esp+4,1);
			f->eax = fibonacci(*(int*)(f->esp+4));
			break;
		case SYS_MAF:		//additional의 max_of_four_int() 호출
			check(f->esp+4,4);
			f->eax = max_of_four_int(*(int*)(f->esp+4),*(int*)(f->esp+8),*(int*)(f->esp+12),*(int*)(f->esp+16));

			break;
		//defalut:
		//	break;

	}

}

/* file descriptor가 fd인 파일의 파일 포인터 위치를 리턴 */
unsigned tell(int fd){

	return file_tell(thread_current()->fd[fd]);
}

/* fd가 file에서 읽거나 쓸 파일  포인터의 위치를 position으로 변경 */
void seek(int fd,unsigned position){
	
	file_seek(thread_current()->fd[fd],position);
	return;
}

/* 매개변수의 이름의 file을 open하고, 그 file descriptor를 리턴, 실패시 -1 리턴 */
int open(const char* file){
	
	lock_acquire(&file_lock);
	int ret = -1;
	if(file == NULL){
		lock_release(&file_lock);
		exit(-1);
	}
	struct file* open_file = filesys_open(file);
	if(open_file == NULL){
		lock_release(&file_lock);
		return -1;
	}
	struct thread* running = thread_current();
	for(int i=3;i<128;i++){
		if(running->fd[i] == NULL){
			if(!strcmp(file,thread_name()))
				file_deny_write(open_file);
			running->fd[i] = open_file;
			ret = i;
			break;
		}
	}
	lock_release(&file_lock);
	return ret;
}
/* file descriptor에 해당하는 file close */
void close(int fd){
	
	struct file* cur = thread_current()->fd[fd];
	
	thread_current()->fd[fd] = NULL;
	file_close(cur);
	return;
}

/* 매개변수로 받아온 file descriptor에 해당하는 파일의 사이즈를 리턴 */

int filesize(int fd){

	return file_length(thread_current()->fd[fd]);
	 
}

/*매개변수의 이름을 가진 file을 remove, 성공시 true리턴 */
bool remove(const char* file){

	return filesys_remove(file);

}
/* 매개변수로 받은 정보로 file을 생성, 성공시 true 리턴 */
bool create(const char* file,unsigned initial_size){
	if(file == NULL) 
		exit(-1);
	return filesys_create(file,initial_size);	

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
	int ret = -1;

	lock_acquire(&file_lock);
	if(fd == 1){
		putbuf(buffer,size);
		ret =  size;
	}
	else if(fd>2){
		ret =  file_write(thread_current()->fd[fd],buffer,size);
	}
	lock_release(&file_lock);
	return ret;
}
/*
	fd가 stdin일 때, buffer에 size만큼 입력
*/
int read(int fd,void* buffer, unsigned size){
	char x;
	unsigned int cnt;
	int ret = -1;
	lock_acquire(&file_lock);
	check_address(buffer);		//read-bad-ptr 해결 buffer로 넘어오는 값이 invalid한 주소였음 ㅂㄷㅂㄷ
	if(fd == 0){
		for(cnt =0;cnt<size;cnt++){
			x = input_getc();
			if( x == '\0') break;
			*(char*)buffer++ = x;
		}
		ret =  cnt;
	}
	else if(fd > 2){
		ret = file_read(thread_current()->fd[fd],buffer,size);
	}
	lock_release(&file_lock);
	return ret;

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





