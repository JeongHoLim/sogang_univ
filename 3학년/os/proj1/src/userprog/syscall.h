#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
void syscall_init (void);
typedef int pid_t;

// syscall.c에서 구현한 함수들 선언
void check_address(void* addr);
void halt(void);
void exit(int status);
int write(int fd,const void* buffer,unsigned size);
int read(int fd,void* buffer,unsigned size);
pid_t exec(const char* file);
int wait(pid_t pid);

int fibonacci(int n);
int max_of_four_int(int a,int b,int c,int d);

#endif /* userprog/syscall.h */
