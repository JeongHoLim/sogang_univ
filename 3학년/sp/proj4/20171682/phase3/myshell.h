#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <setjmp.h>
#include <signal.h>
#include <dirent.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <math.h>
#include <errno.h>


/* Misc constants */
#define	MAXLINE	 8192  /* Max text line length */
#define MAXBUF   8192  /* Max I/O buffer size */
#define LISTENQ  1024  /* Second argument to listen() */
#define MAXARGS 128

/* Our own error-handling functions */

typedef struct node{
	
	int idx;
    int pid;
    char command[MAXLINE];	
	int run;
	char sym;
	struct node* link;

}Node;
/* background에서 관리하는 명령어를 저장하기 위한 구조체 */


typedef struct queue{

	Node* front;
	Node* rear;
	int idx;	
	int num;

}Queue;
/*Node를  queue의 형태로 구현하기 위한 구조체 */

typedef struct buf{

	char tcmd[MAXLINE];
	int idx;
	char sym;
	struct buf* link;

}Buf;

/*background에서 실행 완료된 명령어의 정보를 저장하기 위한 구조체 */

typedef struct _buf{
	Buf* front;
	Buf* rear;

}BUF;


/* Buf를 queue의 형태로 구현하기 위한 구조체 */

BUF* buffer;		//BUF 큐

typedef void handler_t(int);	
Queue* queue;		//Node 큐

int MPID;		//부모 프로세스 pid
char global_cmd[MAXLINE];	// 현재 수행하고 있는 명령어를 담고 있는 변수

void insdone(char* cmd,int idx);	// 백그라운드에서 종료된 명령어를 저장하는 함수
void printdone();					//buffer에 있는 명령어 정보를 출력하는 함수
void eval(char* cmdline);			//입력받은 command를 검사하는 함수
int parseline(char* buf,char** argv);		//입력받은 command를 parse 하는 함수
int builtin_command(char** argv);			//입력받은 command가 builtin_command이면 검사하는 함수
void redir_pip(char* s[][128],int idx,int read,int bg,char* save);		//builtin_command가 아닌 명령어를 수행하는 함수
void Init();				// buffer와 queue를 초기화하는 함수
void Enqueue(int pid,char* cmd,int* idx,int run);	// queue에 enqueue하는 함수
Node Dequeue();										// queue에서 dequeue하는 함수

void setsym(int p,int m,int mode);				// +, - symbol을 관리하는 함수
void jobs();									//jobs 명령을 수행하는 함수
void FreeJob();									//사용했던 리소스와 프로세스를 회수하는 함수
void back(char* str);							// & 기호를 발견하면 명령어와 & 사이에 공백을 추가하는 함수
void sigchld_handler(int sig);					//SIGCHLD handler
void sigtstp_handler(int sig);					//SIGTSTP handler
void sigint_handler(int sig);					//SIGINT handler
void kill_proc(int pid);						// kill command 수행하는 함수
void buf_print();								// buffer에 있는 내용을 출력한는 함수
void BG(int target);							// bg command를 수행하는 함수
void FG(int target);							// fg command를 수행하는 함수


/*
 * 
 * csapp.c에 제공된 baseline 함수 
 *
 * */
handler_t *Signal(int signum, handler_t *handler);
void unix_error(char* msg);
pid_t Fork();
void Kill(pid_t pid,int signum);
void Sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
void Sigemptyset(sigset_t *set);
void Sigfillset(sigset_t *set);
void Sigaddset(sigset_t *set, int signum);	
void Sigdelset(sigset_t *set, int signum);	
int Sigismember(const sigset_t *set, int signum);
int Sigsuspend(const sigset_t *set);
size_t sio_puts(char s[]); /* Put string */
ssize_t sio_putl(long v); /* Put long */

void sio_error(char s[]); /* Put error message and exit */
/* $end siopublic */

/*******************************
 * Wrappers for the SIO routines
 ******************************/
ssize_t Sio_putl(long v);
ssize_t Sio_puts(char s[]);
void Sio_error(char s[]);
static void sio_ltoa(long v,char s[],int b);
static size_t sio_strlen(char s[]);
static void sio_reverse(char s[]);
