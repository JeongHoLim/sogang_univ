#include "lib/stdbool.h"
#include<stdint.h>
#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
void syscall_init (void);
typedef int pid_t;
typedef int32_t off_t;
// project 1
bool check_address(void* addr);
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

// project5

bool chdir(const char*);
bool mkdir(const char*);
bool readdir(int fd,char* name);
bool isdir(int fd);
int inumber(int );
/*
struct file_ds{
	struct file* file;
	struct dir* dir;
};
*/
struct file{
	struct inode* inode;
	off_t pos;
	bool deny_write;
};
