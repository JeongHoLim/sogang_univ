/* 
 * echoserveri.c - An iterative echo server 
 */ 
/* $begin echoserverimain */
#include "csapp.h"
typedef struct item{
	int id;
	int left_stock;
	int price;
	int readcnt;
	struct item* left;
	struct item* right;

}Item;
Item* bstRoot = NULL;
int current_client=0;
int byte_cnt = 0;
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
	return;
}
void traverse(FILE* fp,Item* ptr){

	if(ptr == NULL) return;

	traverse(fp,ptr->left);
	
	ptr->readcnt += 1;

	fprintf(fp,"%d %d %d\n",ptr->id,ptr->left_stock,ptr->price);

	ptr->readcnt -= 1;
	
	traverse(fp,ptr->right);
	return;
}
void update_stock_txt(){

	FILE* fp = fopen("stock.txt","w");
	
	traverse(fp,bstRoot);
	
	fclose(fp);
	return;
}
void update_stock_bst(Item* target,char* toSend,int new_id,int new_count,int mode){

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

}
void BSTSearch(char* toSend,int id,int count,int mode){
	
	Item* cNode = bstRoot;
	int temp;
	while(cNode != NULL){
		temp = cNode->id;
		cNode->readcnt += 1;

		if(temp == id){
			cNode->readcnt -= 1;
			update_stock_bst(cNode,toSend,id,count,mode);

			break;
		}
		else{
			cNode->readcnt -= 1;
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
void PostOrder(Item* bt){
	if(bt == NULL) return;
	PostOrder(bt->left);
	PostOrder(bt->right);
	printf("id: %d, left_stock : %d, price: %d\n",bt->id,bt->left_stock,bt->price);
	return;
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
	
	ptr->readcnt += 1;

	sprintf(localtemp,"%d %d %d\n",ptr->id,ptr->left_stock,ptr->price);

	ptr->readcnt -= 1;
	
	n = strlen(localtemp);
	memcpy(toSend+st+diff1,localtemp,n);
	diff1 += n;
	diff2 = Show(ptr->right,toSend,st+diff1);

	return diff1+diff2;
}
void init_pool(int listenfd, pool* p){
	
	int i;
	p->maxi = -1;
	for(i=0;i<FD_SETSIZE;i++)
		p->clientfd[i] = -1;

	p->maxfd = listenfd;
	FD_ZERO(&p->read_set);
	FD_SET(listenfd,&p->read_set);
	return;
}
void add_client(int connfd, pool* p){

	int i;
	p->nready--;
	for(i=0;i<FD_SETSIZE;i++){
		if(p->clientfd[i]<0){
			p->clientfd[i] = connfd;
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

				if(!strcmp(cmd,"show")){
					ptr = bstRoot;
					Show(ptr,buf,0);
					buf[strlen(buf)]='\0';
				}
				else if(!strcmp(cmd,"exit")){
					sprintf(buf,"exit");
				}
				else{
					sscanf(buf,"%s %d %d",cmd,&id,&cnt);
					if(!strcmp(cmd,"buy")){
						BSTSearch(buf,id,cnt,0);
					}
					else if(!strcmp(cmd,"sell")){
						BSTSearch(buf,id,cnt,1);
					}
					else{
						//input error
					}
				}
				Rio_writen(connfd,buf,sizeof(buf));
				memset(buf,0,sizeof(buf));
			}
			else{
				Close(connfd);
				printf("%d exit !\n",connfd);
				
				current_client -= 1;
				if(current_client==0) 
					update_stock_txt();

				FD_CLR(connfd,&p->read_set);
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
	Init();

    listenfd = Open_listenfd(argv[1]);
    init_pool(listenfd,&pool);


	while (1) {
		
		pool.ready_set = pool.read_set;
		pool.nready = Select(pool.maxfd+1,&pool.ready_set,NULL,NULL,NULL);
		
		if(FD_ISSET(listenfd,&pool.ready_set)){
			clientlen = sizeof(struct sockaddr_storage);
			connfd = Accept(listenfd,(SA*)&clientaddr,&clientlen);
			add_client(connfd,&pool);
		}
		check_clients(&pool);
    }
	Free_bst();
    exit(0);
}
/* $end echoserverimain */
