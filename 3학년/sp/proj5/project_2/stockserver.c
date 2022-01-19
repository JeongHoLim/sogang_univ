/* 
 * echoserveri.c - An iterative echo server 
 */ 
/* $begin echoserverimain */
#include "csapp.h"
#define NTHREADS 20
#define SBUFSIZE 20
typedef struct item{	
	int id;
	int left_stock;
	int price;
	int readcnt;
	sem_t mutex;		
	sem_t r;
	struct item* left;
	struct item* right;

}Item;
typedef struct{		//sbuf를 위한 구조체
	int* buf;
	int n;
	int front;
	int rear;
	sem_t m;
	sem_t s;
	sem_t i;
}sbuf_t;
sbuf_t sbuf;		
Item* bstRoot = NULL;
sem_t u;			//current_client update할 때 사용되는 semaphore 변수
int current_client=0;
static sem_t p;
void init_func(){
	Sem_init(&p,0,1);
}
void BSTInsert(int id,int left_stock,int price){
	
	Item* new = (Item*)malloc(sizeof(Item));
	new->left = NULL; new->right = NULL;
	new->id = id; new->left_stock = left_stock ; new->price = price;
	Item* cNode = bstRoot;
	Item* pNode = NULL;
	while(cNode != NULL){
		if(cNode->id == id)return;
		pNode = cNode;
		if(pNode->id > id) cNode = cNode->left;
		else cNode = cNode->right;
	}
	if(pNode != NULL){
		if(pNode->id < id) pNode->right = new;
		else pNode->left = new;
	}
	else bstRoot = new;
	Sem_init(&(new->mutex),0,1);		// 해당 노드 정보 수정할 때 사용되는 semaphore 변수
	Sem_init(&(new->r),0,1);			// 해당 노드의 readcnt 수정할 때 사용되는 semaphore 변수
	return;
}
void traverse(FILE* fp, Item* ptr){

	if(ptr == NULL) return;
	traverse(fp,ptr->left);

	P(&(ptr->r));		// readcnt 수정하기 위함
	ptr->readcnt += 1;
	if(ptr->readcnt == 1)
		P(&(ptr->mutex));		//node 정보 읽고 있을 때, 다른 thread가 쓰는 것을 방지
	V(&(ptr->r));

	fprintf(fp,"%d %d %d\n",ptr->id,ptr->left_stock,ptr->price);

	P(&(ptr->r));			//readcnt 감소
	ptr->readcnt -= 1;
	if(ptr->readcnt == 0)
		V(&(ptr->mutex));
	V(&(ptr->r));
	traverse(fp,ptr->right);

	return;
}
void update_stock_txt(){

	FILE* fp = fopen("stock.txt","r+");
	
	traverse(fp,bstRoot);

	fclose(fp);
	return;
}
void update_stock_bst(Item* target,char* toSend,int new_id,int new_count,int mode){

	P(&(target->mutex));			//node의 critical section 진입
	if(mode == 0){		// buy 
		if(target->left_stock < new_count){
			sprintf(toSend,"Not enough left stock\n");
		}
		else{
			target->left_stock -= new_count;
			sprintf(toSend,"[buy] success\n");
		}
	}
	else{				//sell
		target->left_stock += new_count;
		sprintf(toSend,"[sell] success\n");
	}
	V(&(target->mutex));		//critical section 끝

}
void BSTSearch(char* toSend,int id,int count,int mode){
	
	Item* cNode = bstRoot;
	int temp;
	while(cNode != NULL){
		temp = cNode->id;
		if(temp == id){
			update_stock_bst(cNode,toSend,id,count,mode);
			break;
		}
		else{
			if(temp > id){
				cNode = cNode->left;
			}
			else{
				cNode = cNode->right;
			}
		}
	}
	return ;
}
void Init(){
	FILE* fp;
	int id,left_stock,price;
	int ret;
	fp = fopen("stock.txt","r");
	if(fp == NULL){ printf("stock.txt file doesn't exits!\n");return;}
	
	while(1){
		ret = fscanf(fp,"%d %d %d\n",&id,&left_stock,&price);
		if(ret == EOF) break;
		BSTInsert(id,left_stock,price);
	}
	Sem_init(&u,0,1);		//current_client semaphore 변수 
	fclose(fp);
	return;
}
void PostOrder_free(Item* bt){
	if(bt == NULL) return;
	PostOrder_free(bt->left);
	PostOrder_free(bt->right);
	free(bt);
}
void Free_bst(){
	Item* ptr = bstRoot;
	PostOrder_free(ptr);
}
int Show(Item* ptr,char* toSend,int st){

	char localtemp[100];
	int n,diff1,diff2;
	if(ptr == NULL) return 0;
	
	diff1 = Show(ptr->left,toSend,st);
	
	P(&(ptr->r));		//readcnt 수정하기 위한 semaphore 변수 사용
	ptr->readcnt += 1;	
	if(ptr->readcnt==1)	//reader and writer 문제 해소
		P(&(ptr->mutex));
	V(&(ptr->r));

	sprintf(localtemp,"%d %d %d\n",ptr->id,ptr->left_stock,ptr->price);

	P(&(ptr->r));		//readcnt 수정하기 위한 sempaphore 변수 사용
	ptr->readcnt -= 1;
	if(ptr->readcnt==0)		//reader and writer 문제 해결 위함
		V(&(ptr->mutex));
	V(&(ptr->r));
	
	n = strlen(localtemp);
	memcpy(toSend+st+diff1,localtemp,n);
	diff1 += n;
	diff2 = Show(ptr->right,toSend,st+diff1);

	return diff1+diff2;
}
void func(int connfd){
	int n;
	char toSend[MAXLINE];
	rio_t rio;
	char cmd[MAXLINE];
	int id, cnt;
	Item* ptr;
	
	static pthread_once_t once = PTHREAD_ONCE_INIT;
	Pthread_once(&once,init_func);
	Rio_readinitb(&rio,connfd);

	while((n = Rio_readlineb(&rio,toSend,MAXLINE))!=0){		// connfd client가 입력한 요청은 toSend에 담겨 있다.
		printf("Server received %d bytes\n",n);
		sscanf(toSend,"%s",cmd);
		if(!strcmp(cmd,"show")){		
			ptr = bstRoot;
			Show(ptr,toSend,0);
			toSend[strlen(toSend)]='\0';
		}
		else if(!strcmp(cmd,"exit")){	
			sprintf(toSend,"exit");
		}
		else{		
			sscanf(toSend,"%s %d %d",cmd,&id,&cnt);
			if(!strcmp(cmd,"buy")){	
				BSTSearch(toSend,id,cnt,0);
				
			}
			else if(!strcmp(cmd,"sell")){
				BSTSearch(toSend,id,cnt,1);
			}
			else{		//input error!
				sprintf(toSend,"Input error!");
			}
		}
	
		Rio_writen(connfd,toSend,sizeof(toSend));		//처리 결과 client 버퍼에 출력한다.
		memset(toSend,0,sizeof(toSend));
	}
	printf("%d exit !\n",connfd);
	return;
}
void sbuf_init(sbuf_t *sp, int n){			// sbuf 초기화하는 함수
	sp->buf = Calloc(n,sizeof(int));		
	sp->n = n;
	sp->front = sp->rear = 0;
	Sem_init(&sp->m,0,1);					// thread한테 client 부여할 때 사용
	Sem_init(&sp->s,0,n);					// 가용한 thread 파악
	Sem_init(&sp->i,0,0);					// 접속한 client 파악
	return;
}
void sbuf_deinit(sbuf_t* sp){
	Free(sp->buf);					//sbuf free
	return;
}
void sbuf_insert(sbuf_t* sp,int item){
	
	P(&sp->s);				// 가용한 thread 하나 감소
	P(&sp->m);				// sp->buf 접근
	sp->buf[(++sp->rear)%(sp->n)] = item;		
	V(&sp->m);
	V(&sp->i);				// 접속한 client, thread 할당 끝남

	return;
}
int sbuf_remove(sbuf_t* sp){
	
	int item;
	P(&sp->i);				// 새로 접속한 client가 없으면 remove 못 한다.
	P(&sp->m);				// sp->buf 접근
	item = sp->buf[(++sp->front)%(sp->n)];
	V(&sp->m);				
	V(&sp->s);				// 가용한 thread 개수 하나 증가
	
	return item;
}
void *thread(void* vargp){		//thread의 work routine인 함수
	Pthread_detach(pthread_self());
	
	while(1){
		int connfd = sbuf_remove(&sbuf);			//부여받은 client fd 받는다.
		P(&u);
		current_client += 1;				//current_client 접근, 전후로 semaphore 변수 lock 걸고 해제한다.
		V(&u);
		func(connfd);					//func()호출하여 client가 요청한 명령 수행
		Close(connfd);					// client와 접속 종료
		P(&u);
		current_client -=1;				// 접속한 client 수 감소
		if(current_client ==0)
			update_stock_txt();			// 모든 client가 접속 종료하면 stock.txt update한다.
		V(&u);
	}
}
int main(int argc, char **argv) 
{
    int i,listenfd, connfd;

    socklen_t clientlen;
    struct sockaddr_storage clientaddr;  /* Enough space for any address */  //line:netp:echoserveri:sockaddrstorage

	pthread_t tid;
    if (argc != 2) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(0);
    }
	Init();
	

    listenfd = Open_listenfd(argv[1]);
	sbuf_init(&sbuf,SBUFSIZE);			//sbuf 초기화
	
	for(i=0;i<NTHREADS;i++)				//NTHREADS 만큼 쓰레드 미리 생성
		Pthread_create(&tid,NULL,thread,NULL);

    while (1) {
		clientlen = sizeof(struct sockaddr_storage); 
		connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
		printf("%d connected\n",connfd);
		sbuf_insert(&sbuf,connfd);		// 접속한 client를 처리할 thread를 찾기 위해 sbuf에 삽입

    }
	Free_bst();
	sbuf_deinit(&sbuf);
    exit(0);
}
/* $end echoserverimain */
