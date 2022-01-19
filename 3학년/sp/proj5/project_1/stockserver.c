/* 
 * echoserveri.c - An iterative echo server 
 */ 
/* $begin echoserverimain */
#include "csapp.h"

// 이진트리 노드 구조체
typedef struct item{
	int id;
	int left_stock;
	int price;
	int readcnt;
	struct item* left;
	struct item* right;

}Item;				
Item* bstRoot = NULL;		//이진트리의 rootnode를 가리키는 포인터
int current_client=0;		//현재 서버에 접속되어 있는데 client의 개수
int byte_cnt = 0;			//서버가 client으로부터 입력받은 누적된 byte개수
void BSTInsert(int id,int left_stock,int price){	// 새로운 주식 종목을 이진트리에 삽입
	
	Item* new = (Item*)malloc(sizeof(Item));
	new->left = NULL; new->right = NULL;
	new->id = id; new->left_stock = left_stock ; new->price = price;
	Item* cNode = bstRoot;
	Item* pNode = NULL;
	while(cNode != NULL){
		if(cNode->id == id)return;			//원래 있는 종목이면 새로 삽입x
		pNode = cNode;
		if(pNode->id > id) cNode = cNode->left;
		else cNode = cNode->right;
	}
	if(pNode != NULL){
		if(pNode->id < id) pNode->right = new;
		else pNode->left = new;
	}
	else bstRoot = new;
	return;
}
void traverse(FILE* fp,Item* ptr){			//stock.txt update하는 함수

	if(ptr == NULL) return;
	traverse(fp,ptr->left);					// 왼쪽 자식노드부터 방문
	fprintf(fp,"%d %d %d\n",ptr->id,ptr->left_stock,ptr->price);
	traverse(fp,ptr->right);			
	return;
}
void update_stock_txt(){

	FILE* fp = fopen("stock.txt","w");		//stock.txt를 w모드로 새로 연다.
	
	traverse(fp,bstRoot);					//traverse에 파일포인터와 루트노드의 포인터를 매개변수로 넘겨준다.
	
	fclose(fp);
	return;
}
void update_stock_bst(Item* target,char* toSend,int new_id,int new_count,int mode){

	if(mode == 0){		// buy 
		if(target->left_stock < new_count){		//매수하려는 수량보다 재고가 없는 경우
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

}
void BSTSearch(char* toSend,int id,int count,int mode){
	
	Item* cNode = bstRoot;
	int temp;
	while(cNode != NULL){
		temp = cNode->id;
		if(temp == id){							// client가 요청한 주식의 id를 찾은 경우
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
	fp = fopen("stock.txt","r");				//input.txt 파일 읽는다.
	if(fp == NULL){ printf("stock.txt file doesn't exits!\n");return;}
	
	while(1){
		ret = fscanf(fp,"%d %d %d\n",&id,&left_stock,&price);
		if(ret == EOF) break;
		BSTInsert(id,left_stock,price);			//txt파일에서 읽은 정보를 이진트리에 삽입
	}
	fclose(fp);
	return;
}
void PostOrder_free(Item* bt){				//후위순회로 이진트리 데이터 free한다.
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
	
	diff1 = Show(ptr->left,toSend,st);		//중위 순회로 toSend에 show정보 copy한다.
	
	sprintf(localtemp,"%d %d %d\n",ptr->id,ptr->left_stock,ptr->price);

	n = strlen(localtemp);
	memcpy(toSend+st+diff1,localtemp,n);	//현재 노드의 정보 toSend에 넣는다.
	diff1 += n;
	diff2 = Show(ptr->right,toSend,st+diff1);

	return diff1+diff2;
}
void init_pool(int listenfd, pool* p){		//pool 초기화
	
	int i;
	p->maxi = -1;
	for(i=0;i<FD_SETSIZE;i++)		// clientfd -1로 초기화
		p->clientfd[i] = -1;

	p->maxfd = listenfd;
	FD_ZERO(&p->read_set);			//read_set clear
	FD_SET(listenfd,&p->read_set);
	return;
}
void add_client(int connfd, pool* p){

	int i;
	p->nready--;
	for(i=0;i<FD_SETSIZE;i++){
		if(p->clientfd[i]<0){			//available한 clientfd 찾는다
			p->clientfd[i] = connfd;		//client 부여
			Rio_readinitb(&p->clientrio[i],connfd);
			
			FD_SET(connfd,&p->read_set);
			
			current_client += 1;		//stock.txt 업데이트 확인할 때 확인

			if(connfd > p->maxfd)
				p->maxfd = connfd;
			if(i > p->maxi)
				p->maxi = i;
			break;

		}
	}
	if(i==FD_SETSIZE)
		app_error("add_client error: Too many clients");
	return;
}

void check_clients(pool* p){
	int i,connfd,n;
	char buf[MAXLINE],cmd[MAXLINE];
	int id,cnt;
	Item* ptr;
	rio_t rio;
	for(i=0;(i<=p->maxi) && (p->nready >0);i++){
		connfd = p->clientfd[i];
		rio = p->clientrio[i];

		if((connfd>0) && (FD_ISSET(connfd,&p->ready_set))){
			p->nready--;
			if((n=Rio_readlineb(&rio,buf,MAXLINE))!=0){
				byte_cnt += n;
				printf("Server received %d (%d total) bytes on fd %d\n",n,byte_cnt,connfd);
				sscanf(buf,"%s",cmd);

				if(!strcmp(cmd,"show")){		//show 명령 수행
					ptr = bstRoot;
					Show(ptr,buf,0);
					buf[strlen(buf)]='\0';
				}
				else if(!strcmp(cmd,"exit")){	//exit 
					sprintf(buf,"exit");
				}
				else{
					sscanf(buf,"%s %d %d",cmd,&id,&cnt);
					if(!strcmp(cmd,"buy")){		//buy 명령 수행
						BSTSearch(buf,id,cnt,0);
					}
					else if(!strcmp(cmd,"sell")){		//sell 명령 수행
						BSTSearch(buf,id,cnt,1);
					}
					else{
						sprintf(buf,"input error!\n");
					}
				}
				Rio_writen(connfd,buf,sizeof(buf));		//buf에 있는 문자 client의 버퍼에 출력한다.
				memset(buf,0,sizeof(buf));
			}
			else{
				Close(connfd);		
				printf("%d exit !\n",connfd);
				
				current_client -= 1;
				if(current_client==0)				//모든 client가 접속을 끊으면 stock.txt 업데이트 
					update_stock_txt();

				FD_CLR(connfd,&p->read_set);		//clientfd 다시 초기화
				p->clientfd[i] = -1;
			}
		}
	}


}
int main(int argc, char **argv) 
{
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;  /* Enough space for any address */  //line:netp:echoserveri:sockaddrstorage
	static pool pool;			

    if (argc != 2) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(0);
    }
	Init();		//stock.txt에서 이진트리 생성

    listenfd = Open_listenfd(argv[1]);
    init_pool(listenfd,&pool);		// pool 초기화


	while (1) {
		
		pool.ready_set = pool.read_set;		//ready_set을 read_set 복사
		pool.nready = Select(pool.maxfd+1,&pool.ready_set,NULL,NULL,NULL);		//Select()로 현재 처리해야할 요청 개수 리턴
		
		if(FD_ISSET(listenfd,&pool.ready_set)){		// 새로운 client의 접속인 경우
			clientlen = sizeof(struct sockaddr_storage);
			connfd = Accept(listenfd,(SA*)&clientaddr,&clientlen);
			add_client(connfd,&pool);
		}
		check_clients(&pool);		//client 명령 처리
    }
	Free_bst();
    exit(0);
}
/* $end echoserverimain */
