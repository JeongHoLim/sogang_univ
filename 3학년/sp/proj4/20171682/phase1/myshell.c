#include "myshell.h"
volatile sig_atomic_t tid;
volatile sig_atomic_t F;
int main(){

	char cmdline[MAXLINE];

	Init();
	MPID = getpid();	//부모 pid 저장
	

	//signal handler 등록
	Signal(SIGCHLD,sigchld_handler);
	Signal(SIGTSTP,sigtstp_handler);
	Signal(SIGINT,sigint_handler);

	while(1){
		F=1;		// prompt 창이면 F = 1
		printf("CSE4100-SP-P4> ");
		if(fgets(cmdline,MAXLINE,stdin)==NULL) exit(0);
		if(cmdline[0]=='\n'){
			printdone();
			continue;	
		}
		if(feof(stdin)) exit(0);
		back(cmdline);
		strcpy(global_cmd,cmdline);
		eval(cmdline);		// 입력받은 commandline 검사
		memset(cmdline,0,MAXLINE);
		memset(global_cmd,0,MAXLINE);
		printdone();		//background에서 실행완료된 명령들 출력
	}
	return 0;
}
void sigint_handler(int sig){	//ctrl+c 처리하는 핸들러
	int olderrno = errno;
	if(getpid() == MPID){
		Sio_puts("\n");
		if(F==1) Sio_puts("CSE4100-SP-P4> ");	// prompt창이면 무시하고 계속 출력
		else{
			Kill(tid,SIGKILL);			//tid에 저장되어 있는 프로세스 kill
		}
	}
	errno = olderrno;
}
void sigtstp_handler(int sig){
	int olderrno = errno;
	if(getpid() == MPID){
		Sio_puts("\n");
		if(F==1)Sio_puts("CSE4100-SP-P4> ");	//prompt창이면 무시하고, 계속 출력
		else {
			Kill(tid,SIGSTOP);			//tid에 저장되어 있는 프로세스 stop
		}
	}
	errno = olderrno;
}
void sigchld_handler(int sig){
	int olderrno = errno;
	int target,status;
	pid_t wpid;
	Node ret;
	Node* ptr = queue->front;
	char cmd[MAXLINE];

	while(ptr!=NULL){
		target = ptr->pid;
		wpid = waitpid(target,&status,WNOHANG);		//SIGCHLD 시그널 기다리는 중
		if(wpid<0) break;
		strcpy(cmd,ptr->command);
		ret = Dequeue(wpid);						// queue에서 해당 pid에 해당하는 노드 제거
		if(ret.idx >0){
			insdone(cmd,ret.idx);	//실행 완료되면 buffer에 enqueue
			break;
		}
		ptr = ptr->link;
	}
	errno = olderrno;
	return;
}
void insdone(char* cmd,int idx){
	
	Buf* new = (Buf*)malloc(sizeof(Buf));
	strcpy(new->tcmd,cmd);
	new->link = NULL;
	new->idx = idx;
	new->sym = ' ';
	if(buffer->front == NULL){	// buffer에 아무것도 없는 경우
		buffer->front = new;
		buffer->rear = new;
	}
	else{
		buffer->rear->link = new;	// buffer->rear에 추가
		buffer->rear = new;
	}
	setsym(idx,-1,0);
	return;

}
void setsym(int p,int m,int mode){

	if(mode == 1){		//background node  queue
		Node* nptr = queue->front;
		int pflg=0,nflg=0;
		while(nptr != NULL){
			if(nptr->idx == p){		//새로 들어온 명령이 background 실행으로 들어온 경우
				nptr->sym = '+';
				pflg=1;
			}
			else if(nptr->sym == '+'){	
				if(m==-1){			//fg에서 ctrl+z가 입력된 경우
					nptr->sym = '-';
					nflg=1;
				}
				else{
					nptr->sym = '+';	// +인 명령이 그대로 +이어야 하는 경우
					pflg=1;
				}
			}
			else if(nptr->sym == '-'){
				if(m==-1){				//-가없어져야 하는 경우
					nptr->sym = ' ';
				}
				else if(m==-2){
					if(pflg==1) {		//-가 -로 남아야하는 경우
						nptr->sym = '-';
						nflg=1;
					}else {
						nptr->sym = '+';	//-가 +로 바귀어야 하는 경우
						pflg = 1;
					}
				}
			}
			nptr = nptr->link;
		}
		if(pflg==0){
			if(queue->rear != NULL) queue->rear->sym = '+';		//jobqueue에 + 명령이 하나도 없는 경우
		}
		if(nflg==0){			
			nptr = queue->front;
			while(nptr != NULL){
				if(nptr->link==queue->rear){			//job queue에서 -명령이 없으면 뒤에서 2번째를 -로 설정
						nptr->sym = '-';			
						break;
				}
				nptr = nptr->link;
			}
		}
	}
	else{				// done node   buffer의 노드들에게 symbal 설정하는 부분
		Buf* bptr = buffer->front;
		while(bptr != NULL){
			if(bptr->idx == p){			
				bptr->sym = '+';
			}
			else if(bptr->sym == '+'){
				bptr->sym = '-';
			}
			else if(bptr->sym == '-'){
				bptr->sym = ' ';
			}
			bptr = bptr->link;
		}
	}
	return;
}
void printdone(){			//background 에서 실행 완료된 명령어가 종료된 경우
	
	Buf* cur,*prev;
	cur = buffer->front;
	int i=0;
	while(cur != NULL){
		printf("[%d]%c  Done\t\t%s",cur->idx,cur->sym,cur->tcmd);
		i++;
		prev = cur;
		cur = cur->link;
		free(prev);
	}
	buffer->front = buffer->rear = NULL;

	return;
}
void Kill(pid_t pid,int signum){	

	int rc;
	if((rc = kill(pid,signum))<0)
		unix_error("Kill error");	
}
void back(char* str){	//입력받은 명령에 &가 있는 경우
	char* delim;
	if((delim = strchr(str,'&'))){		// 명령과 &사이에 공백을 넣어준다
		*delim =' ';
		*(delim+1) = '&';
		*(delim+2) = '\n';
		*(delim+3) = '\0';
		str = delim + 1;
	}
}
void unix_error(char* msg){
	fprintf(stderr,"%s: %s\n",msg,strerror(errno));
	exit(0);
}
pid_t Fork(){
	pid_t pid;

	if((pid=fork())<0)
		unix_error("Fork error");
	return pid;

}
/* paseline - Parse the command line and build the argv array */
int parseline(char* buf,char ** argv){

	char* delim;		/* Points to first space delimiter */
	int argc;			/* Number of args */
	int bg;				/* Background job? */
	int i;
	while(*buf && (*buf == ' '))  /* Ignore leading spaces */
		buf++;
	argc = 0;
	i = strlen(buf)-1;
	
	while(buf[i]==' '){
		i--;
	}
	buf[i+1]='\0';
	
	while((delim = strchr(buf,' '))){
		*delim = 0;
		argv[argc++] = buf;
		buf = delim + 1;
		while(*buf && (*buf == ' ')){  /* Ignore spaces */
			buf ++;
		}
	}
	if(buf!=NULL) argv[argc++] = buf;
	argv[argc]=NULL;
	
	if(argc ==0)	/* Ignore blank line */
		return 1;
	/* Should the job run in the background? */

	if(!strcmp(argv[argc-1],"&")){
		bg = 1;
		argv[argc-1]=NULL;
	}
	else bg=0;

	return bg;
	
}
void eval(char* cmdline){

	char buf[MAXLINE];	  /* Holds modified command line */
	int bg=0;				  /* Should the job run in bg or fg? */
	int len;
	char* pipcmd[6][MAXARGS];
	int i=0,j=0;
	int st;
	char save[MAXLINE];	
	F=0;
	strcpy(buf,cmdline);
	len =strlen(buf);
	for(i=len;i>0;i--)
		if(buf[i] == '&'){			//백그라운드 실행인 경우
			strncpy(save,cmdline,i);
			save[i]='\n';
			save[i+1]='\0';
			break;
		}
	for(i=0,j=0,st=0;i<len;i++){
		if(buf[i]=='|'){			//파이프 기준으로 parsing하는 부분
			buf[i]='\0';
			bg = parseline(buf+st,pipcmd[j]);
			j++;
			st=i+1;
		}
		else if(i+1==len){
			buf[i]='\0';
			bg = parseline(buf+st,pipcmd[j]);
		}
	}
	//B = bg;
	if(pipcmd[0] == NULL) 
		return;			/*Ignore empty lines */
	if(!builtin_command(pipcmd[0])){
		redir_pip(pipcmd,0,0,bg,save);		//builtin_command가 아닌 경우 호출
	}
	
	for(i=0;i<6;i++){
		memset(pipcmd[i],0,MAXARGS);
	}
	return;

}
void redir_pip(char* pipcmd[][MAXARGS],int pipidx,int rd,int bg,char* save){
	
	pid_t pid;
	int status;
	int fd[2];	
	int idx;
	char command_path[MAXLINE];
	strcpy(command_path,pipcmd[pipidx][0]);
	if(pipe(fd)){
		unix_error("pipe()");
	}

	if(pipcmd[pipidx+1][0] == NULL){				//마지막 명령어
		pid = Fork();
		if(pid==0){
			Signal(SIGTSTP,SIG_IGN);				//자식 프로세스에서 ctrl+c와 ctrl+z를 handle하지 않도록 하기 위함
			Signal(SIGINT,SIG_IGN);
			close(fd[1]);
			close(fd[0]);
			dup2(rd,STDIN_FILENO);					//이전 파이프의 명령을 STDIN으로 읽는다.
		
			if(execvp(command_path,pipcmd[pipidx])<0){	
				printf("%s: Command not found.\n",pipcmd[pipidx][0]);
				exit(0);
			}
		}
		else {
			tid = pid;
			close(fd[0]);
			// Parent waits for foreground job to terminate 
			if(!bg){	
				if(waitpid(pid,&status,WUNTRACED)<0){
					unix_error("waitpid()");
				}
				else{
					if(WIFSTOPPED(status)){			//foreground 명령이 실행되던 중 ctrl+z가 입력된 경우
						Enqueue(pid,global_cmd,&idx,0);
						printf("[%d]+  Stopped\t\t%s",idx,global_cmd);
						setsym(idx,-1,1);
					}
				}
			}
			else{	// when there is background process!
				
				Enqueue(pid,save,&idx,1);
				printf("[%d] %d\n",idx,pid);
			}
			return;
		}
	}
	else{
		pid = Fork();
		if(pid ==0){		//child
			Signal(SIGTSTP,SIG_IGN);		//자식 프로세스에서 ctrl+c와 ctrl+z를 handle하지 않도록 하기 위함
			Signal(SIGINT,SIG_IGN);
			close(fd[0]);
			dup2(rd,STDIN_FILENO);			//그 전 파이프 명령의 입력을 STDIN으로 읽는다.
			dup2(fd[1],STDOUT_FILENO);		//현재 파이프 명령의 출력을 pipe에 하기 위함
			close(fd[1]);
			if(execvp(command_path,pipcmd[pipidx])<0){		
				printf("%s: Command not found.\n",pipcmd[pipidx][0]);
				exit(0);
			}
		}
		else{
			//parent
			tid = pid;						
			close(fd[1]);
			pid_t wpid;
			
			if(!bg){
				wpid = waitpid(pid,&status,WUNTRACED);	//foreground에서 실행되던 명령이 멈춘 경우 이전 명령을 reap하기 위함
				if(wpid<0)
					unix_error("wait()");
			}
			else{
				wpid = waitpid(pid,&status,WUNTRACED);		
				//background에서 실행되던 파이프 명령 중 마지막 명령 제외한 모두 reap하기 위함
				if(wpid<0) unix_error("waitpid()");
			}
			redir_pip(pipcmd,pipidx+1,fd[0],bg,save);	//다음 파이프 명령을 수행하기 위해 재귀 호출
		}
	}
	return;
}
/* If first arg is a builtin_command, run it and return true */
int builtin_command(char** argv){
	if(!strcmp(argv[0],"quit")){/* quit command */
		FreeJob();
		exit(0);
	}
	else if(!strcmp(argv[0],"exit")){		//exit command
		FreeJob();
		exit(0);
	}
	else if(!strcmp(argv[0],"&"))	/* Ignore singleton & */
		return 1;
	else if(!strcmp(argv[0],"cd")){	// cd command 
		char command_path[MAXLINE];
		if(argv[1]==NULL){
			strcpy(command_path,getenv("HOME"));		//cd만 입력한 경우
		}
		else{
			strcpy(command_path,argv[1]);			//cd directory
		}
		
		if(chdir(command_path)<0)			//현재 디렉토리 정보를 변경한다
			unix_error("cd error");
		return 1;
		
	}
	else if(!strcmp(argv[0],"jobs")){	//jobs command
		jobs();
		return 1;
	}
	else if(!strcmp(argv[0],"kill")){	//kill command
		int target;
		sscanf(argv[1],"%*c%d",&target);
		kill_proc(target);
		return 1;
	}
	else if(!strcmp(argv[0],"fg")){		//fg command
			
		int target;
		sscanf(argv[1],"%*c%d",&target);
		FG(target);
		return 1;
	}
	else if(!strcmp(argv[0],"bg")){		//bg command
		
		int target;
		sscanf(argv[1],"%*c%d",&target);
		BG(target);
		return 1;

	}
	return 0;					/* Not a builtin command */
}
void Init(){		//queue와 buffer를 초기화하는 함수
	queue = (Queue*)malloc(sizeof(Queue));
	queue->front = NULL;
	queue->rear = NULL;
	queue->idx = 0;
	queue->num =0;
	
	
	buffer = (BUF*)malloc(sizeof(BUF));
	buffer->front = NULL;
	buffer->rear = NULL;
	return;
}
void FreeJob(){			//함수가 종료될 때 사용했던 프로세스및 리소스를 모두 회수하기 위한 함수
	
	Node tmp;
	int target;
	while(queue->front != NULL){
		target = queue->front->pid;
		kill(target,SIGKILL);
		tmp = Dequeue(target);
	}
	free(queue);
	free(buffer);
	return;
}
void jobs(){		//jobs command 수행하는 함수
	int i=1;
	Node* cur = queue->front;
	char temp[MAXLINE];
	while(cur!=NULL){
		if(cur->run == 1){			//실행 중인 상태면 Running 출력
			strcpy(temp,cur->command);
			temp[strlen(temp)-1]=0;
			printf("[%d]%c  Running\t\t%s&\n",cur->idx,cur->sym,temp);
		}								//멈춘 상태면 Stopped 출력	
		else printf("[%d]%c  Stopped\t\t%s",cur->idx,cur->sym,cur->command);
		cur = cur->link;
		i++;
	}
	return;
}
void FG(int idx){		//fg command 수행하는 함수
	Node* ptr = queue->front;	
	int wpid;
	int ret,status;
	char cmd[MAXLINE];
	
	while(ptr != NULL){
		if(ptr->idx == idx){		//입력받은 job 번호와 일치하는 노드를 탐색
			wpid = ptr->pid;
			tid = ptr->pid;
			strcpy(cmd,ptr->command);
			printf("%s",cmd);			
			Kill(ptr->pid,SIGCONT);
			ptr->run=1;
			ret = waitpid(wpid,&status,WUNTRACED);
			if(ret>0){
				if(WIFSTOPPED(status)){
					ptr->run = 0;
					printf("[%d]+  Stopped\t\t%s",idx,cmd);			//foreground로 실행한 명령 수행 도중 ctrl+z가 입력된 경우
					setsym(idx,-1,1);
				}
				else {
					Dequeue(ptr->pid);				//fg로 가져온 명령이 종료된 경우 queue에서 제거
					setsym(-1,-2,1);
				}
				
			}
			else unix_error("wait()");
			break;
		}
		ptr = ptr->link;
	}
	return;

}
void BG(int idx){			//bg command 수행하는 함수
	
	Node* ptr = queue->front;
	char temp[MAXLINE];
	while(ptr != NULL){
		if(ptr->idx == idx){		//입력받은 job 번호와 일치하는 노드를 탐색
			tid = ptr->pid;
			Kill(ptr->pid,SIGCONT);
			ptr->run = 1;
			strcpy(temp,ptr->command);
			temp[strlen(temp)-1]=0;
			printf("[%d]%c %s&\n",ptr->idx,ptr->sym,temp);
			break;
		}
		ptr = ptr->link;
	}
	return;
	
}
void kill_proc(int idx){		// kill command 수행하는 함수
	Node* ptr = queue->front;
	while(ptr!=NULL){
		if(ptr->idx == idx){		//입력받은 job번호와 일치하는 노드 탐색
			Kill(ptr->pid,SIGKILL);		//해당 포르세스 kill
			waitpid(ptr->pid,0,0);		//reap해주는 부분
			printf("[%d]%c  Terminated\t\t%s",idx,ptr->sym,ptr->command);	
			Dequeue(ptr->pid);
			setsym(-1,-2,1);
			break;
		}
		ptr = ptr->link;
	}
	return;
}
Node Dequeue(int target){		// job queue에서 특정 pid의 노드를 제거하는 함수
	
	Node ret;
	Node* dNode = queue->front;
	Node* prev = NULL;
	ret.idx = ret.pid = -1;		//못 찾으면 ret의 idx와 pid는 -1을 가진다.
	int flg=0;
	while(dNode!=NULL){
		if(dNode->pid == target){		//queue에서 찾은 경우
			flg=1;
			break;
		}
		prev = dNode;
		dNode = dNode->link;
	}
	if(flg==1){		//background 작업 중 찾운 경우
		ret.idx = dNode->idx;
		ret.pid = dNode->pid;
		strncpy(ret.command,dNode->command,strlen(dNode->command));
		
		if(dNode == queue->front)	//처음 node를 제거하는 경우
			queue->front = dNode->link;
		if(dNode == queue->rear)		//마지막 노드를 제거하는 경우
			queue->rear = prev;
		
		if(prev != NULL) prev->link = dNode->link;	// 삭제할 노드의 이전 노드와 다음 노드 연결
		free(dNode);	
		if(queue->num == ret.idx) queue->idx -= 1;	//job 번호 조정
		queue->num -= 1;			//queue에서 관리하는 노드 개수 조정
		if(queue->num ==0) queue->idx =0;	
	}

	return ret;
}
void Enqueue(int pid,char* command,int* index,int run){		//job queue에 새로운 node를 추가하는 함수
	
	Node* new = (Node*)malloc(sizeof(Node));
	new->pid = pid;
	new->link = NULL;
	new->idx = queue->idx+1;
	*index = new->idx;
	new->run = run;			
	new->sym = ' ';			// 기본 symbol은 ' '로 설정
	queue->idx +=1;
	queue->num += 1;
	strcpy(new->command,command);
	if(queue->front == NULL){		//queue에 노드가 없던 경우
		queue->rear = new;
		queue->front = new;
	}
	else{						
		queue->rear->link = new;
		queue->rear = new;
	}
	return;
}

/* 
 *
 *
 *
 *
 *
 * csapp.c에서 제공한 baseline 함수
 *
 *
 *
 *
 *
 * */

handler_t *Signal(int signum, handler_t *handler)
{
    struct sigaction action, old_action;

    action.sa_handler = handler;
    sigemptyset(&action.sa_mask); /* Block sigs of type being handled */
    action.sa_flags = SA_RESTART; /* Restart syscalls if possible */

    if (sigaction(signum, &action, &old_action) < 0)
    unix_error("Signal error");
    return (old_action.sa_handler);
}
/* $end sigaction */

void Sigprocmask(int how, const sigset_t *set, sigset_t *oldset)
{
    if (sigprocmask(how, set, oldset) < 0)
    unix_error("Sigprocmask error");
    return;
}

void Sigemptyset(sigset_t *set)
{
    if (sigemptyset(set) < 0)
    unix_error("Sigemptyset error");
    return;
}

void Sigfillset(sigset_t *set)
{
    if (sigfillset(set) < 0)
    unix_error("Sigfillset error");
    return;
}

void Sigaddset(sigset_t *set, int signum)
{
    if (sigaddset(set, signum) < 0)
    unix_error("Sigaddset error");
    return;
}

void Sigdelset(sigset_t *set, int signum)
{
    if (sigdelset(set, signum) < 0)
    unix_error("Sigdelset error");
    return;
}
int Sigismember(const sigset_t *set, int signum)
{
    int rc;
    if ((rc = sigismember(set, signum)) < 0)
    unix_error("Sigismember error");
    return rc;
}

int Sigsuspend(const sigset_t *set)
{
    int rc = sigsuspend(set); /* always returns -1 */
    if (errno != EINTR)
        unix_error("Sigsuspend error");
    return rc;
}

size_t sio_puts(char s[]) /* Put string */
{
    return write(STDOUT_FILENO, s, sio_strlen(s)); //line:csapp:siostrlen
}

ssize_t sio_putl(long v) /* Put long */
{
    char s[128];

    sio_ltoa(v, s, 10); /* Based on K&R itoa() */  //line:csapp:sioltoa
    return sio_puts(s);
}

void sio_error(char s[]) /* Put error message and exit */
{
    sio_puts(s);
    _exit(1);                                      //line:csapp:sioexit
}
/* $end siopublic */

/*******************************
 * Wrappers for the SIO routines
 ******************************/
ssize_t Sio_putl(long v)
{
    ssize_t n;

    if ((n = sio_putl(v)) < 0)
    sio_error("Sio_putl error");
    return n;
}

ssize_t Sio_puts(char s[])
{
    ssize_t n;

    if ((n = sio_puts(s)) < 0)
    sio_error("Sio_puts error");
    return n;
}

void Sio_error(char s[])
{
    sio_error(s);
}
static void sio_ltoa(long v, char s[], int b){
    int c,i=0;
	
	do{
		s[i++] = ((c=(v%b))<10)?c+'0':c-10+'a';
	}while((v/=b)>0);
	s[i] = '\0';
	sio_reverse(s);
}
static size_t sio_strlen(char s[]){
	int i=0;

	while(s[i]!='\0')
		++i;
	return i;

}
static void sio_reverse(char s[]){
	int c,i,j;
	
	for(i=0,j=strlen(s)-1;i<j;i++,j--){
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}


