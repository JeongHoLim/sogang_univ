#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

#include "devices/shutdown.h"
#include "devices/input.h"
#include "threads/vaddr.h"
#include "process.h"
#include "filesys/inode.h"
#include "filesys/filesys.h"
#include "filesys/file.h"

#include "threads/palloc.h"

#define STACK_LIMIT (uint8_t*)((uint8_t*)(PHYS_BASE)-(uint8_t*)(8*1024*1024))

struct lock file_lock;
static void syscall_handler (struct intr_frame *);

struct intr_frame* gf;
void
syscall_init (void) 
{
	lock_init(&file_lock);
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}
bool 
check_address(void* addr){		//kernel 영역이면 -1 뿜고 exit
	if(!is_user_vaddr(addr)){		
		return false;
	}
	return true;

}
void check(void* addr,int argc){
	
	for(int i=0;i<argc;i++){
		if(!check_address(addr+(4*i)))
			exit(-1);
	}

}
static void
syscall_handler (struct intr_frame *f UNUSED) 
{

	gf = f;
	//f->esp로 함수에 값을 넘겨주기 전에 valid한 주소인지 확인 
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
		
		case SYS_CHDIR:
			
			check(f->esp+4,1);
			f->eax = chdir((const char*)*(uint32_t*)(f->esp+4));		//리턴값은 eax에 저장

			break;

		case SYS_MKDIR:
			
			check(f->esp+4,1);
			f->eax = mkdir((const char*)*(uint32_t*)(f->esp+4));
			break;	

		case SYS_READDIR:
			
			check(f->esp+4,2);
			f->eax = readdir(*(int*)(f->esp+4),(char*)*(uint32_t*)(f->esp+8));
			break;

		case SYS_ISDIR:
			check(f->esp+4,1);

			f->eax = isdir(*(int*)(f->esp+4));
			break;
		case SYS_INUMBER:

			check(f->esp+4,1);
			f->eax = inumber(*(int*)(f->esp+4)); 
			break;
			//defalut:
		//	break;

	}

}
bool chdir(const char* dir){

	bool ret = false;
	lock_acquire(&file_lock);

	ret = filesys_chdir(dir);

	lock_release(&file_lock);

	return ret;

}
bool mkdir(const char* dir){
	
	bool ret;
	lock_acquire(&file_lock);
		
	ret = filesys_create(dir,0,true);
	lock_release(&file_lock);

	return ret;
}
bool readdir(int fd,char* name){

	bool ret = false;

	lock_acquire(&file_lock);
		
	struct file_ds* fds = thread_current()->fd[fd];

	if(fds == NULL){
		goto done;
	}
	struct inode* inode = file_get_inode(fds->file);
	if(inode == NULL){
		goto done;
	}
	if(!inode_is_directory(inode)){
		goto done;
	}
	struct dir* dir = fds->dir;
	ret = dir_readdir(dir,name);

done:
	lock_release(&file_lock);

	return ret;
}
bool isdir(int fd){

	bool ret = false;
	lock_acquire(&file_lock);

	struct file_ds* fds = thread_current()->fd[fd];
	if(fds == NULL)
		goto done;
	struct inode* inode = file_get_inode(fds->file);
	if(inode == NULL)
		goto done;
	ret = inode_is_directory(inode);

done:
	lock_release(&file_lock);
	
	return ret;
}
int inumber(int fd){

	int ret;

	lock_acquire(&file_lock);

	struct file_ds* fds = thread_current()->fd[fd];
	if(fds == NULL)
		return -1;
	struct inode* inode = file_get_inode(fds->file);
	if(inode == NULL)
		return -1;

	ret = inode_get_inumber(inode);

	lock_release(&file_lock);

	return ret;
}
/* file descriptor가 fd인 파일의 파일 포인터 위치를 리턴 */
unsigned tell(int fd){
	
	struct file_ds* t = thread_current()->fd[fd];
	int ret= -1;
	lock_acquire(&file_lock);
		
	if(t && t->file)
		ret = file_tell(t->file);
	
	lock_release(&file_lock);
	return ret;
}

/* fd가 file에서 읽거나 쓸 파일  포인터의 위치를 position으로 변경 */
void seek(int fd,unsigned position){

	lock_acquire(&file_lock);
	struct file_ds* t = thread_current()->fd[fd];
	if(t && t->file)
		file_seek(t->file,position);

	lock_release(&file_lock);
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
	struct file_ds* tmp = (struct file_ds*)malloc(sizeof(struct file_ds));

	tmp->file = open_file;

	struct inode* inode = file_get_inode(tmp->file);
	if(inode != NULL && inode_is_directory(inode)){
		tmp->dir = dir_open(inode_reopen(inode));
	}
	else
		tmp->dir = NULL;

	for(int i=3;i<128;i++){
		if(running->fd[i] == NULL){
			if(!strcmp(file,thread_name()))
				file_deny_write(open_file);
			running->fd[i] = tmp;
		
			ret = i;
			break;
		}
	}

	lock_release(&file_lock);
	return ret;
}
/* file descriptor에 해당하는 file close */
void close(int fd){

	lock_acquire(&file_lock);
	struct file_ds* fds = thread_current()->fd[fd];

	if(fds){
		if(fds->dir)
			dir_close(fds->dir);
		if(fds->file)
			file_close(fds->file);
		free(fds);
		thread_current()->fd[fd] = NULL;
	}
	lock_release(&file_lock);
	return;
}

/* 매개변수로 받아온 file descriptor에 해당하는 파일의 사이즈를 리턴 */

int filesize(int fd){

	return file_length(thread_current()->fd[fd]->file);
	 
}

/*매개변수의 이름을 가진 file을 remove, 성공시 true리턴 */
bool remove(const char* file){
	
	bool ret;
	lock_acquire(&file_lock);
	ret =  filesys_remove(file);
	lock_release(&file_lock);

	return ret;

}
/* 매개변수로 받은 정보로 file을 생성, 성공시 true 리턴 */
bool create(const char* file,unsigned initial_size){
	if(file == NULL)
		exit(-1);
	lock_acquire(&file_lock);
	bool ret ;
	ret =  filesys_create(file,initial_size,false);	
	lock_release(&file_lock);
	return ret;
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
	struct thread* cur = thread_current();
	struct file_ds* fds = cur->fd[fd];

	lock_acquire(&file_lock);
	if(fd == 1){
		putbuf(buffer,size);
		ret =  size;
	}
	else if(fd>2){
		if(fds){
			if(inode_is_directory(file_get_inode(fds->file)))
				ret = -1;
			else if(fds->file)
				ret =  file_write(fds->file,buffer,size);
		}
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

	if(!check_address(buffer)){
		lock_release(&file_lock);
		exit(-1);		//read-bad-ptr 해결 buffer로 넘어오는 값이 invalid한 주소였음 ㅂㄷㅂㄷ
	
	}
	if(fd == 0){
		for(cnt =0;cnt<size;cnt++){
			x = input_getc();
			if( x == '\0') break;
			*(char*)buffer++ = x;
		}
		ret =  cnt;
	}
	else if(fd > 2){
		struct file_ds* tmp = thread_current()->fd[fd];
		if(tmp && tmp->file)
			ret = file_read(tmp->file,buffer,size);
		
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

