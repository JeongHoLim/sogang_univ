#include "20171682.h"

int main() {

	char input[102];
	int op;
	int flg=1;						//0이면 종료
	int add1,add2,value;			// 16진수 형태의 주소를 입력받기 위한 변수들			
	char com1, com2;				//쉼표를 입력 받기 위한 변수
	char instruction[102],check[102],mnemonic[15];				// 명령어를 구분하기 위한 변수들
	char history_input[102],add[10];									//정제된 명령어 저장용 변수
	make_hash();												//opcode.txt 를 읽어서 hashtable에 명령어들을 저장시킨다.
	while (1) {

		printf("sicsim> ");
		fgets(input,100, stdin);				// input에 입력을 모두 저장한 후, sscanf를 이용하여 구분지어 입력을 확인하는 방법을 사용하였다. 
		input[strlen(input)-1]=0;
		sscanf(input,"%s",instruction);									// instruction : dump 4, 36에서 dump 처럼 처음 나오는 명령어를 담기 위한 변수
		op = check_input(instruction);									
		check[0] =0;									//각 case문에서 정상적인 명령어(공백 포함)인지 판단하기 위해 사용하는 변수
		com1 = com2 = 0;
		add1 = add2 = value = -1;

		switch (op) {
		case 0: {	// h[elp] 인 경우
			sscanf(input,"%s %s",instruction,check);
			if(check[0] != 0){ printf("Input Error!\n"); break;}	//help이외의 다른 명령어 추가	입력시 에러 처리
			help();
			history(instruction);
			break;
		}
		case 1: {	//d[ir] 인 경우
			sscanf(input,"%s %s",instruction,check);
			if(check[0] != 0){ printf("Input Error!\n"); break;}
			dir();
			history(instruction);
			break;
		}
		case 2: {	//q[uit] 인 경우
			sscanf(input,"%s %s",instruction,check);
			if(check[0] != 0){ printf("Input Error!\n"); break;}
			flg=0;	
			free_memory();	// 동적할당 한 메모리 free해주고 종료 flg 0으로 바꿈
			break;
		}
		case 3: {	//hi[story] 인 경우
			sscanf(input,"%s %s",instruction,check);
			if(check[0] != 0){ printf("Input Error!\n"); break;}
			history(instruction);
			print_history();	
			break;
		}
		case 4:	{	//du[mp] [start,end] 인 경우
			sscanf(input,"%s %s",instruction,check);	// dump나 du만 나오는 형태인지 거르기 위함.
			if(check_error(input)){ printf("Input Error!\n"); break;}	// 0x나 0X의 형태로 입력받느 16진수인지 확인
			if(check[0]==0){										// du나 dump만 입력된 경우
				dump(-1,-1);									//dump나 du만 입력된 경우, start와 end를 -1로 넘김
				strcpy(history_input,instruction);
			}
			else{								//dump나 du가 아닌 경우
				check[0] =0;
				sscanf(input,"%s %X %c%X %s",instruction,&add1,&com1,&add2,check);
				if(com1==0){					//dump start의 경우
					if(isHex(add1,0)){
						dump(add1,-1);					//start에 입력된 값이 정상적인 입력이면 end값에 -1 넣고 dump함수 호출
					}
					else {printf("Input Error!\n"); break;}			
					strcpy(history_input,instruction);				//명령어 정제
					strcat(history_input," ");
					sprintf(add,"%X",add1);
					strcat(history_input,add);						//정제한 명령어는 history_input에 저장
				}
				else{							//dump start, end의 경우
					if(check[0] != 0 || !(isHex(add1,0) && isHex(add2,0))) {
						printf("Input Error!\n"); break;}						// start와 end자리에 입력된 값이 올바른 입력인지 뒤에 추가 입력이 있는지 확인

					if (add1>add2){
						printf("Input Error!\n");					//start가 end보다 큰 경우 에러 처리
						break;
					}
					if(com1 ==','){									// start와 end가 정상적으로 쉼표로 구분된 경우
						dump(add1,add2);
						strcpy(history_input,instruction);
						strcat(history_input," ");
						sprintf(add,"%X",add1);
						strcat(history_input,add);
						sprintf(add,"%X",add2);
						strcat(strcat(history_input,", "),add);
					}
					else{ printf("Input Error!\n"); break;}
				}
			}
			history(history_input);	//정제된 명령어 저장
			break;
		}
		case 5: {
			//edit address, value 인 경우
		
			sscanf(input,"%s %X %c%X %s",instruction,&add1,&com1,&value,check);
			if(check_error(input)){ printf("Input Error!\n"); break;}										//0x나 0X의 형태로 입력받은 16진수인지 확인
			if(check[0] != 0){ printf("Input Error!\n"); break;}

			if(!(isHex(add1,0) && isHex(value,1))){ printf("Input Error!\n"); break;}							// 정상 범위의 16진수인지 확인
			edit(add1,value);
			strcpy(history_input,instruction);
			strcat(history_input," ");
			sprintf(add,"%X",add1);
			strcat(history_input,add);
			sprintf(add,"%X",value);
			strcat(strcat(history_input,", "),add);
			history(history_input);
			break;
		}
		case 6: {// fill start, end, value 인 경우
			sscanf(input,"%s %X %c%X %c%X %s",instruction,&add1,&com1,&add2,&com2,&value,check);					
			if(check_error(input)){ printf("Input Error!\n"); break;}										//0x나 0X의 형태로 입력받은 16진수인지 확인
			if(check[0] != 0 || com1 != ',' || com2 != ','){ printf("Input Error!\n"); break;}					//start,end,value가 정상적으로 쉼표로 구분되어있는지, 뒤에 추가 입력은 없는지 확인	
				
			if(!(isHex(add1,0) && isHex(add2,0) && isHex(value,1))){ printf("Input Error!\n"); break;}			//start,end,value자리에 입력된 값이 정상 입력인지 확인 
		 
			fill(add1,add2,value);
			strcpy(history_input,instruction);						// strcat과 sprintf를 이용하여 명령어 정제
			strcat(history_input," ");
			sprintf(add,"%X",add1);
			strcat(history_input,add);
			sprintf(add,"%X",add2);
			strcat(strcat(history_input,", "),add);
			sprintf(add,"%X",value);
			strcat(strcat(history_input,", "),add);
			history(history_input);
			break;
		}
		case 7: {
			//reset 인 경우
			sscanf(input,"%s %s",instruction,check);
			if(check[0] != 0){ printf("Input Error!\n"); break;}
			reset();
			history(instruction);
			break;
		}
		case 8: {
			//opcode mnemonic 인 경우
			mnemonic[0] = 0;
			sscanf(input,"%s %s %s",instruction,mnemonic,check);
			if(check[0] != 0 || mnemonic[0] == 0){ printf("Input Error!\n"); break;}					//opcode mnemonic의 입력 형식에서 opcode만 입력된 경우와 뒤에 추가 입력이 있는지 확인
			if(!check_opcode(mnemonic)){ printf("mnemonic should be alphabet uppercase!\n"); break;}		// mnemonic이 대문자로 이루어져있는지 확인
			opcode_mnemonic(mnemonic);
			strcpy(history_input,instruction);
			strcat(strcat(history_input," "),mnemonic);
			history(history_input);
			break;
		}
		case 9:{
			//opcodelist 인 경우
			sscanf(input,"%s %s",instruction,check);
			if(check[0] != 0){ printf("Input Error!\n"); break;}
			opcodelist();
			history(instruction);
			break;
		}
		default:
			printf("Input Error!\n");
			break;
		}
		if(flg==0) break;
		instruction[0] = 0;
	}
	return 0;

}
int check_error(char* input){				//0x나 0X 형태의 입력이 들어오는 경우를 걸러내기 위함.


	int i,input_len = strlen(input);				
	for(i=0;i<input_len;i++){
		if(input[i] == 'X' || input[i] == 'x') return 1;
	}
	return 0;
}
void free_memory(){
	int i;
	Hash* h_cur,*h_prev;
	Node* n_cur = head,*n_prev;
	for(i=0;i<20;i++){					// 해시 테이블 free해주는 코드
		h_cur = table[i];
		while(h_cur!=NULL){
			h_prev = h_cur;
			h_cur = h_cur->link;
			free(h_prev);
		}
	}

	if(n_cur == NULL) return;
	while(n_cur!=NULL){					//명령어를 저장한 linkedlist free해주는 코드
		n_prev = n_cur;
		n_cur = n_cur->link;
		free(n_cur);				
	}
	return;
}
int hash_function(char* s){

	int len = strlen(s);
	int key=0,i;
	for(i=0;i<len;i++)	
		key += ((s[i]-'A'+1)*(KEY+1));				//KEY는 20, 알파벳 순서와 21을 곱한 값을 더한 후, 20으로 나머지 연산 처리 함.
	key %= KEY;
	
	return key;
}
void opcodelist(){
	
	int i;
	Hash* cur = NULL,*next = NULL;
	for(i=0;i<20;i++){
		cur = table[i];
		printf("%d : ",i);
		if(cur != NULL){
			next = cur->link;
			while(next!=NULL){							// -> 출력 처리를 위해 마지막 노드 전까지만 출력하게 함
				printf("[%s,%s]",cur->instruction,cur->opcode);
				printf(" -> ");
				cur = next;
				next = next->link;
			}
			printf("[%s,%s]",cur->instruction,cur->opcode);			//해시 테이블 각 행에 마지막 노드 출력
		}
		printf("\n");
	}
	return;
}
void opcode_mnemonic(char* target){
	
	int len = strlen(target);
	Hash* cur = NULL;
		
	cur = table[hash_function(target)];			//검색할 문자열을 target으로, 해시 함수에 넣어 저장되어있다면 어디에 저장되어있을 지 바로 접근.
	if(cur==NULL){								//검색할 문자열이 없는 경우
		printf("There are no %s in opcode.txt!\n",target);
		return;
	}
	while(cur!=NULL){
		if(strlen(cur->instruction) == len){		//문자 길이가 같은지 비교 후, 일치하는 지 확인
			if(!strcmp(cur->instruction,target)){
				printf("opcode is %s\n",cur->opcode);
				return;
			}
		}
		cur = cur->link;
	}
	
	printf("There are no %s in opcode.txt!\n",target);		// 끝까지 찾았는데 없는 경우
	
	return;
}
int check_opcode(char* target){
	
	int i,len = strlen(target);
	for(i=0;i<len;i++)
		if(target[i] <'A' || target[i]>'Z') return 0;	//대문자 알파벳으로 이루어져있는지 확인
	return 1;
}
void make_hash(){

	FILE* fp = fopen("opcode.txt","r");
	char str[102];
	char* opcode,*instruction,*format;
	int hash;
	if(fp == NULL){
		printf("file open error!\n");
		return;
	}
	while(fgets(str,100,fp)!=NULL){						//str에 한 줄씩 입력을 받는다.
		opcode = strtok(str," \t");						//공백을 구분자로, str을 opcode,instruction,format으로 나눈다.
		instruction = strtok(NULL," \t");
		format = strtok(NULL," \t");
		Hash* new = (Hash*)malloc(sizeof(Hash));		//새로운 opcode를 저장하기 위함
		strcpy(new->opcode,opcode);
		strcpy(new->instruction,instruction);
		strcpy(new->format,format);
		new->link = NULL;
		hash = hash_function(instruction);				//instruction을 넣은 해시함수 값을 hash에 저장
		Hash* cur = table[hash];						//cur은 해당 명령어가 저장될 해시 테이블을 가리킨다.
		if(cur == NULL){
			table[hash]= new;							//해시 테이블이 아직 가리키는 값이 없는 경우 가리키게 함.
		}
		else{
			while(cur->link !=NULL){					//가리키는 값이 있는 경우, 끝을 찾아 연결
				cur = cur->link;
			}
			cur->link = new;
		}
	}
	fclose(fp);
	return;
}
int isHex(int num,int x){							//문자열이 자리 크기에 맞는 16진수인지 확인해주는 함수. 조건에 부합하면 1 리턴,아니면 0 리턴.
	if(x==0 && (num >0xFFFFF || num < 0)){				//x가 0이면, 매개변수의 문자열은 주소 인덱싱에 쓰일 값인데, 크기가 초과되면 0리턴
		return 0;
	}
	else if(x == 1 && (num >0xFF || num < 0)){			//x가 1이면, 매개변수의 문자열은 메모리에 저장될 값인데, 크기가 초과되면 0리턴
		return 0;
	}
	return 1;
}
void reset(){										//memory 전체를 0으로 할당해주는 함수
	memset(memory,0,sizeof(memory));
	return;
}
void fill(int start,int end,int value){				//start부터 end까지의 메모리를 value로 할당하는 함수
	if(start>end) printf("Input Error!\n");
	else memset(memory+start,value,end-start+1);
	return;
}
void edit(int address,int value){							//ad번지의 메모리 공간에 val을 저장해주는 함수
	memory[address] = value;
	return;
}
void dump(int start,int end){
	int idx,i;
	int cnt1,cnt2;
	if(start==-1) {									// dump나 du만 입력이 들어온 경우
		if(address==0xFFFFF ) start =0;				// 현재 address가 FFFFF를 가리키고 있는 경우 다시 0으로 돌아옴.
		else start = address+1;						//아니면 address+1을 둠.
	}
	if (end>0xFFFFF){ printf("Bound Error!\n") ; return;}			// end에 FFFFF보다 큰 값이 들어오면 bound 에러 처리
	else if(end == -1) end = start+159;								// end에 입력된 값이 없으면 160개 출력
	if(end >0xFFFFF) end = 0xFFFFF;									// 160개 출력하는데, 메모리를 초과하는 경우 메모리 끝까지만 출력하게끔 함
	idx = start/16;	
	cnt1 = cnt2 = start- start%16;						// 가운테 칼럼과 오른쪽 칼럼을 출력할 때 사용할 인덱스 처리. 
	while(1){
		printf("%04X0 ",idx++);
		for(i=0;i<16;i++){
			if(cnt1>= start && cnt1<=end) printf("%02X ",memory[cnt1]);				//인덱스가 정상 범위이면 출력, 아닐 시 공백 출력
			else printf("   ");
			cnt1++;
		}
		printf("; ");
		for(i=0;i<16;i++){
			if((cnt2 >= start && cnt2<=end) &&(memory[cnt2]>=ASCII_MIN && memory[cnt2]<=ASCII_MAX)) printf("%c",memory[cnt2]);			//인덱스가 정상 범위이고, 아스키 코드값이 20~7E 사이이면 출력
			else printf(".");
			cnt2++;
		}
		printf("\n");
		if(cnt1>end){ 
			address = end;					// end까지 다 출력한 경우, address에 end를 저장 후 총료
			break;
		}
	
	}
	return;
}
void help(){										//수행가능한 shell 명령어의 종류를 나열해주는 함수
	printf("h[elp]\n");
	printf("d[ir]\n");
	printf("q[uit]\n");
	printf("hi[story]\n");
	printf("du[mp] [start, end]\n");
	printf("e[dit] address, value\n");
	printf("f[ill] start, end, value\n");
	printf("reset\n");
	printf("opcode mnemonic\n");
	printf("opcodelist\n");
	return;
}
void print_history(){								//콘솔 창에서 정상적으로 입력된 명령어들의 기록을 출력해주는 함수
	Node* cur = head;
	while(cur!=NULL){
		printf("         %-5d",cur->num);
		fputs(cur->mnemonic,stdout);
		printf("\n");
		cur = cur->link;
	}
	return;

}
void history(char* input){							// 정상적으로 입력된 명령어들을 저장하는 함수	
	Node* new = (Node*)malloc(sizeof(Node));
	new->link = NULL;
	strcpy(new->mnemonic,input);
	if(head==NULL){									//아직 저장된 명령어가 없으면 head는 new를 가리킴
		head = new;
		new->num = ++history_num;
	}
	else{
		Node* cur = head;
		while(cur->link != NULL) cur = cur->link;			//저장된 명령어의 끝을 탐색 후 연결
		cur->link = new;
		new->num = ++history_num;
	}
}
void dir(){
	DIR* dir;
	struct dirent *ent;
	struct stat buf;
	dir = opendir(".");
	int cnt=0;
	char* file_path = (char*)calloc(1, 1+MAXLINE);			//같은 디렉토리에 있는 파일의 이름을 저장할 변수
	if(dir == NULL){	
		printf("opendir() failed!\n");
		return;
	}
	while((ent = readdir(dir))!= NULL){
		memset(file_path, 0, 1+MAXLINE);
		lstat(ent->d_name,&buf);
		strcat(file_path, ent->d_name);
		if(!strncmp(file_path,".",1)|| !strncmp(file_path,"..",2)) continue;	//. 이나 ..은 출력하지 않음
		if (ent->d_type == DT_DIR) *(file_path + strlen(file_path)) = '/';		//디렉토리일 때
		else if (buf.st_mode & 01001001) *(file_path + strlen(file_path)) = '*';		//실행파일 일 때
		cnt++;
		if(cnt==1){
			printf("\t  ");
		}
		printf("%-18s", file_path);
		if(cnt==3){				//3개씩 끊어서 출력
			printf("\n");
			cnt=0;
		}
	}

	free(file_path);
	closedir(dir);
	if(cnt!=0) printf("\n");
	return ;

}
int check_input(char* inputstr) {							//입력된 문자열이 제공하는 shell 명령어에 부합하는지 확인
	if (!strcmp(inputstr, "h") || !strcmp(inputstr, "help")) {
		return 0;
	}
	else if (!strcmp(inputstr, "d") || !strcmp(inputstr, "dir")) {
		return 1;
	}
	else if (!strcmp(inputstr, "q") || !strcmp(inputstr, "quit")) {
		return 2;
	}
	else if(!strcmp(inputstr,"hi") || !strcmp(inputstr,"history")){
		return 3;
	}
	else if (!strcmp(inputstr, "du") || !strcmp(inputstr, "dump")) {
		return 4;
	}
	else if (!strcmp(inputstr, "e") || !strcmp(inputstr, "edit")) {
		return 5;
	}
	else if (!strcmp(inputstr, "f") || !strcmp(inputstr, "fill")) {
		return 6;
	}
	else if (!strcmp(inputstr, "reset")) {
		return 7;
	}
	else if (!strcmp(inputstr, "opcode")) {
		return 8;
	}
	else if (!strcmp(inputstr, "opcodelist")) {
		return 9;
	}
	else return -1;
}
