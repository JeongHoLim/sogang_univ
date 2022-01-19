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
	sem_t mutex;
	sem_t r;
	struct item* left;
	struct item* right;

}Item;
Item* bstRoot = NULL;
sem_t u;
int current_client=0;
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
	Sem_init(&(new->mutex),0,1);
	Sem_init(&(new->r),0,1);
	return;
}
void traverse(FILE* fp, Item* ptr){

	if(ptr == NULL) return;
	traverse(fp,ptr->left);
	P(&(ptr->r));
	ptr->readcnt += 1;
	if(ptr->readcnt == 1)
		P(&(ptr->mutex));
	V(&(ptr->r));

	fprintf(fp,"%d %d %d\n",ptr->id,ptr->left_stock,ptr->price);

	P(&(ptr->r));
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

	P(&(target->mutex));
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
	V(&(target->mutex));	

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
	Sem_init(&u,0,1);
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
	
	P(&(ptr->r));
	ptr->readcnt += 1;
	if(ptr->readcnt==1)
		P(&(ptr->mutex));
	V(&(ptr->r));

	sprintf(localtemp,"%d %d %d\n",ptr->id,ptr->left_stock,ptr->price);

	P(&(ptr->r));
	ptr->readcnt -= 1;
	if(ptr->readcnt==0)
		V(&(ptr->mutex));
	V(&(ptr->r));
	
	n = strlen(localtemp);
	strncpy(toSend+st+diff1,localtemp,n);
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
	Rio_readinitb(&rio,connfd);
	
	while((n = Rio_readlineb(&rio,toSend,MAXLINE))!=0){
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

			}
		}
	
		Rio_writen(connfd,toSend,sizeof(toSend));
		memset(toSend,0,sizeof(toSend));
	}
	printf("%d exit !\n",connfd);
	return;
}
void *thread(void* vargp){
	int connfd = *((int*)vargp);
	Pthread_detach(pthread_self());
	P(&u);
	current_client += 1;
	V(&u);

	Free(vargp);
	func(connfd);
	Close(connfd);
	P(&u);
	current_client -=1;
	if(current_client ==0)
		update_stock_txt();
	V(&u);

	return NULL;
}

int main(int argc, char **argv) 
{
    int listenfd, *connfdp;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;  /* Enough space for any address */  //line:netp:echoserveri:sockaddrstorage
    char client_hostname[MAXLINE], client_port[MAXLINE];

	pthread_t tid;
    if (argc != 2) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(0);
    }
	Init();
	

    listenfd = Open_listenfd(argv[1]);
    while (1) {
		clientlen = sizeof(struct sockaddr_storage); 
		connfdp = Malloc(sizeof(int));
		*connfdp = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        Getnameinfo((SA *) &clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0);
        printf("Connected to (%s, %s)\n", client_hostname, client_port);
		Pthread_create(&tid,NULL,thread,connfdp);

    }
	Free_bst();
    exit(0);
}
/* $end echoserverimain */
