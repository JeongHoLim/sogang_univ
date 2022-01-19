#include "lib/stdbool.h"
#include<stdint.h>
#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
void syscall_init (void);
typedef int pid_t;
typedef int32_t off_t;
// project 1
void check_address(void* addr);
void halt(void);
void exit(int status);
int write(int fd,const void* buffer,unsigned size);
int read(int fd,void* buffer,unsigned size);
pid_t exec(const char* file);
int wait(pid_t pid);

int fibonacci(int n);
int max_of_four_int(int a,int b,int c,int d);

// project 2
bool create(const char* file,unsigned initial_size);
int open(const char* file);
int filesize(int fd);
bool remove(const char* file);
void close(int fd);
void seek(int fd,unsigned position);
unsigned tell(int fd);
#endif /* userprog/syscall.h */

struct file{
	struct inode* inode;
	off_t pos;
	bool deny_write;
};
