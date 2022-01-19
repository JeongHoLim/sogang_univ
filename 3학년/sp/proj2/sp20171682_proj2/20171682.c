#include "20171682.h"

int main() {

	char input[102];
	int op;
	int flg=1;						//0이면 종료
	int add1,add2,value;			// 16진수 형태의 주소를 입력받기 위한 변수들			
	char com1, com2;				//쉼표를 입력 받기 위한 변수
	char instruction[102],check[102],mnemonic[15];				// 명령어를 구분하기 위한 변수들
	char history_input[102],add[10];									//정제된 명령어 저장용 변수
	char filename[15];
	sp_cnt=0;
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
			if(!isUpper(mnemonic)){ printf("mnemonic should be alphabet uppercase!\n"); break;}		// mnemonic이 대문자로 이루어져있는지 확인
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
		case 10:{
			//assemble filename인 경우	
			filename[0] = 0;
			sscanf(input,"%s %s %s",instruction,filename,check);
			if(check[0] != 0 || filename[0] == 0){ printf("Input Error!\n"); break;}

			if(assemble_filename(filename) == 1){ 
				strcpy(history_input,instruction);
				strcat(strcat(history_input," "),filename);
				history(history_input);
			}
			sn_cnt = 0;	
			break;
		}
		case 11:{
			//type filename인 경우
			filename[0] = 0;
			sscanf(input,"%s %s %s",instruction,filename,check);
			if(check[0] != 0 || filename[0] == 0){ printf("Input Error!\n"); break;}
		
			if(type_filename(filename)){
				strcpy(history_input,instruction);
				strcat(strcat(history_input," "),filename);
				history(history_input);
			}
			break;
		}
		case 12:{
			//symbol인 경우
			sscanf(input,"%s %s",instruction,check);
			if(check[0] != 0){ printf("Input Error!\n"); break;}
			if(isSYM ==0){ 
				printf("Error! Symbol table doesn't exist\n");
			} 
			else symbol();	
			
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
void symbol(){					
	int i,j;
	SYMTAB* ptr = NULL;
	int tmp;
	int* loc = (int*)malloc(sizeof(int)*sp_cnt);			// symtabP 에 있는 label 복사하기 위해 사용하는 임시 변수들
	char** sym = (char**)malloc(sizeof(char*)*sp_cnt);
	int* idx = (int*)malloc(sizeof(int)*sp_cnt);

	for(i=0;i<sp_cnt;i++){
		sym[i] = (char*)malloc(sizeof(char)*30);				
		idx[i] = i;		
	}
	for(i=0,j=0;i<20;i++){								//symtabP에 있는 symbol들을 한 줄의 linkedlist로 복사한다
		ptr = symtabP[i];
		while(ptr!=NULL){
			strcpy(sym[j],ptr->label);
			loc[j] = ptr->loc;
			j++;
			ptr = ptr->link;
		}

	}
	for(i=0;i<sp_cnt-1;i++){							// 알파벳 순으로 출력하기 위한 index sorting
		for(j=i+1;j<sp_cnt;j++){
			if(strcmp(sym[idx[i]],sym[idx[j]])>0){
				tmp = idx[i]; idx[i] = idx[j]; idx[j] = tmp;
			}
		}
	}
	for(i=0;i<sp_cnt;i++){
		printf("\t%s\t%04X\n",sym[idx[i]],loc[idx[i]]);
	}

	for(i=0;i<sp_cnt;i++) free(sym[i]);			// 사용이 끝난 변수들 메모리 해제
	free(sym);
	free(loc);
	free(idx);
	return;
}
int check_opcode(char* target){						//opcode가 어떤 directive인지 확인하기 위함
	if(strcmp(target,"START")==0) return 0;
	else if(strcmp(target,"END")==0) return 1;
	else if(strcmp(target,"WORD")==0) return 2;
	else if(strcmp(target,"RESW")==0) return 3;
	else if(strcmp(target,"BYTE")==0) return 4;
	else if(strcmp(target,"RESB")==0) return 5;
	else if(strcmp(target,"BASE")==0) return 6;
	else return -1;
}
void error(int lineNum,char* opcode){			//assemble하는 과정 중 에러가 발생한 경우 해당 lineNum과 라인 출력
	printf("Error in line %d : %s\n",lineNum,opcode);
	freeFail();									//addS()에서 symtabN에 할당하던 symtab을 메모리 해제하기 위함
	return;
}
int count_token(char* line){
	
	int cnt=0;
	char* tok = strtok(line," \t");
	if(tok != NULL){
		cnt += 1;
		while((tok = strtok(NULL," \t"))!=NULL) cnt++;			// 라인이 공백 기준으로 몇 개인지 카운트해서 리턴
	}
	return cnt;
}
int getFormat(char* target){
	char temp[30];
	if(target[0]=='+') strcpy(temp,target+1);			//format 4인 경우 +가 앞에 있으므로 +제외하고 복사
	else strcpy(temp,target);
	unsigned int len = strlen(temp);
	Hash* cur = NULL;
	cur = table[hash_function(temp)];			//검색할 문자열을 target으로, 해시 함수에 넣어 저장되어있다면 어디에 저장되어있을 지 바로 접근.
	while(cur!=NULL){
		if(strlen(cur->instruction) == len){		//문자 길이가 같은지 비교 후, 일치하는 지 확인
			if(!strcmp(cur->instruction,temp)){
				return cur->format;
			}
		}
		cur = cur->link;
	}
	return -1;
}
int setLoc(int op,char* label,char* opcode,char* operand,int* loc){				//operand에 맞는 location counter를 리턴
	int flg=0,z,len;
	int opcodeNum;
	char* tok,temp[30],temp2[30];
	if(op == 0){				//start의 경우 
		strcpy(program_name,label);				
		program_start = toHex(operand);
		*loc = program_start;									
		return 0;
	}
	else if(op==1){			//end의 경우
		program_length = (*loc)-program_start;		//프로그램 길이 저장
		return 0;
	}
	else if(op >= 2 && op <= 5){		//word,resw,byte,resb
		if(op==2){					//word의 경우
			return 3;
		}
		else if(op==3){				//resw의 경우
			return 3*(atoi(operand));
		}
		else if(op==4){				//byte의 경우
			strcpy(temp,operand);
			tok = strtok(temp,"'");
			tok = strtok(NULL,"'");
			if(operand[0]=='C'){
				return strlen(tok);
			}
			else if(operand[0]=='X'){
				strcpy(temp2,tok);
				len = strlen(temp2);
				for(z=0;z<len;z++){								// 16진수로 표현될 수 있는 입력인지 확인
					if(!((temp2[z] >='0' && temp2[z]<='9') || (temp2[z]>='A'&&temp2[z]<='F')))
						return -1;
				}
				return (strlen(tok)+1)/2;
			}
		}
		else{				//resb의 경우
			return atoi(operand);				
		}
	}
	else if(op==6){				//base의 경우
		return 0;
	}
	else{
		if(opcode[0] == '+') flg=1;
		opcodeNum = getFormat(opcode+flg);
		if(opcodeNum == -1) return -1;				//opcode가 opcode.txt.에 없는 경우
		return opcodeNum+flg;				//4형식이면 4, 3형식이면 3리턴
	}
	return -1;

}
void freeFail(){					//assemble 과정에서 에러를 발견한 경우 저장하던 symtabN의 메모리를 해제한다

	int i;
	SYMTAB* ptr,*prev=NULL;
	for(i=0;i<20;i++){
		ptr = symtabN[i];
		while(ptr!=NULL){
			prev = ptr;
			ptr = ptr->link;
			free(prev);
		}
		symtabN[i] = NULL;
	}	
	sn_cnt = 0;							// sn_cnt는 symtabN에 저장된 symbol들의 개수, 0으로 초기화
	return;
}
int addS(char* target,int loc){
	
	int hash = hash_function(target);
	unsigned int len = strlen(target);
	SYMTAB* cur = symtabN[hash];
	SYMTAB* prev = symtabN[hash];
	Hash* find = table[hash];	
	if(find != NULL){
		while(find != NULL){
			if(strlen(find->instruction)==len){
				if(strcmp(find->instruction,target)==0){			// label 이름이 opcode.txt에 저장된 opcode와 이름이 같은 경우
					return 0;
				}
			}
			find = find->link;
		}
	}
	if(check_opcode(target)!=-1) return 0;						//label 이름이 예약된 directive와 같은 경우

	if(cur != NULL){
		while(cur != NULL){
			if(strlen(cur->label)==len){
				if(strcmp(cur->label,target)==0){			// symtabN에 같은 이름의 label이 있는 경우
					return 0;
				}
			}
			prev = cur;
			cur = cur->link;
		}
	}
	SYMTAB* new = (SYMTAB*)malloc(sizeof(SYMTAB));				// symtabN에 새로운 symbol을 저장한다.
	new->link = NULL;
	new->loc = loc;
	strcpy(new->label,target);
	sn_cnt += 1;												// symtabN에 저장된 symbol 개수 1 증가
	if(symtabN[hash] == NULL) symtabN[hash] = new;
	else prev->link = new;

	return 1;
}
int getLoc(char* target){							//operand에 해당하는 주소를 리턴
	
	char temp[30];
	int ni = checkNI(target);
	if(ni!=3) strcpy(temp,target+1);				// operand 앞에 @이나 #이 붙어 있는 경우 
	else strcpy(temp,target);
	int hash=hash_function(temp);			
	unsigned int len = strlen(temp);
	SYMTAB* cur = symtabN[hash];
	if(cur != NULL){
		while(cur != NULL){
			if(strlen(cur->label)==len){
				if(strcmp(cur->label,temp)==0){			// symtabP에서 해당되는 symbol 찾는다.
					return cur->loc;					// 그 symbol의 주소 리턴
				}
			}
			cur = cur->link;
		}
	}
	if(ni==1){				// immediate addressing인 경우 숫자 그대로 리턴
		return atoi(temp);
	}
	return -1;			//target의 주소가 없는 경우 
}	
int nullCheck(char* str){					// immediate_file읽을 때 사용되는 함수, 문자열이 비어있는지 확인
	
	if(strcmp(str,"NULL")==0 || strcmp(str,"(null)")==0) return 1;	
	return 0;

}
char* find_opcode(char* target){					//pass2에서 사용되는 함수로, opcode 값을 리턴	

	char temp[30];
	if(target[0]=='+') strcpy(temp,target+1);
	else strcpy(temp,target);
	unsigned int len = strlen(temp);
	Hash* cur = NULL;
	cur = table[hash_function(temp)];			//검색할 문자열을 target으로, 해시 함수에 넣어 저장되어있다면 어디에 저장되어있을 지 바로 접근.
	while(cur!=NULL){
		if(strlen(cur->instruction) == len){		//문자 길이가 같은지 비교 후, 일치하는 지 확인
			if(!strcmp(cur->instruction,temp)){
				return cur->opcode;
			}
		}
		cur = cur->link;
	}
	return NULL;						//없을 시 NULL 리턴
}
int checkR(char* target){					//문자열이 어떤 레지스터인지 리턴, 없을 시 -1리턴	

	if(strcmp(target,"A")==0) return 0;
	else if(strcmp(target,"X")==0) return 1;
	else if(strcmp(target,"L")==0) return 2;
	else if(strcmp(target,"B")==0) return 3;
	else if(strcmp(target,"S")==0) return 4;
	else if(strcmp(target,"T")==0) return 5;
	else if(strcmp(target,"F")==0) return 6;
	else if(strcmp(target,"PC")==0) return 8;
	else if(strcmp(target,"SW")==0) return 9;
	return -1;
}
int checkNI(char* str){						//비트 n,i의 값을 더한 후 리턴
	if(str[0]=='#') return 1;
	else if(str[0] == '@') return 2;
	else return 3;
}
int toHex(char* str){				//문자열을 16진수의 수로 리턴
	int i,len = strlen(str);
	int sum=0,mul=1;
	for(i=len-1;i>=0;i--,mul*=16){
		if(str[i]>='A' && str[i]<='F') sum += (str[i]-'A'+10)*mul;
		else sum += (str[i]-'0')*mul;
	}
	return sum;
}
void convertSymtab(){						//pass1에서 에러 없이 끝난 경우, 원래 저장되어 있던 symtabP를 symtabN으로 교체해주는 함수
	
	int i;
	SYMTAB* ptr=NULL,*prev=NULL;				
	for(i=0;i<20;i++){
		ptr = symtabP[i];					//symtabP을 가리키는 ptr
		symtabP[i] = symtabN[i];			//symtabP는 이제 symtabN을 가리킨다.
		while(ptr!=NULL){
			prev = ptr;
			ptr = ptr->link;
			free(prev);						// symtabP의 symtab들 하나씩 메모리 해제
		}
		symtabN[i] = NULL;
	}	
	isSYM =1;							// symbol함수를 사용할 수 있는지 알려주는 전역변수
	sp_cnt = sn_cnt;					// 새롭게 바뀐 symtabP에 있는 symbol 개수 수정한다
	return;
}
int pass2(char* filename){
	FILE* im = NULL;
	FILE* fp = NULL;
	FILE* obj = NULL;
	char* tok,*temp2,*opmode;
	char line[100],fname[34],temp[100],temp3[30];
	int lineNum,loc,len,bflg;
	int op,format;
	int r1,r2;
	int ni,ta,cnt=0,flg=1;
	int e=0,x=0;
	char label[30],opcode[30],operand1[30],operand2[30];
	im = fopen("immediate_file","r");
	obj = fopen("temp.txt","w");				//obj파일 만들 때 참고할 임시 파일 생성
	strcpy(fname,filename);
	len = strlen(fname);
	fname[len-3]='l';fname[len-2]='s';fname[len-1]='t';				
	fp = fopen(fname,"w");
	while(fgets(line,100,im)!=NULL){
		label[0] = opcode[0] = operand1[0] =operand2[0] = 0;
		sscanf(line,"%d|%d|%d|%[^|]|%[^|]|%[^|]|%s",&bflg,&lineNum,&loc,label,opcode,operand1,operand2);		// pass1에서 작성한 immediate_file을 읽어온다. 
		fprintf(fp,"%-10d",lineNum);		//lineNum 출력
		fprintf(obj,"%d ",loc);				//temp.txt에 location counter 저장
		cnt++;
		e=0;x=0;
		if(bflg==2){					//주석 처리
			strcpy(temp,line);
			temp[strlen(temp)-1]=0;
			tok = strtok(temp,".");
			fprintf(fp,".              ");
			fprintf(obj,".\n");
			while(1){
				tok = strtok(NULL," \t");
				if(tok==NULL) break;
				fprintf(fp," %s",tok);
			}
			fprintf(fp,"\n");
			continue;
		}
		if(bflg!=1) fprintf(fp,"%04X    ",loc);			//base나 end를 구분
		else fprintf(fp,"        ");					//base와 end는 loc 주소 출력 안 함
		if(nullCheck(label)){					//label 없는 경우 처리
			fprintf(fp,"        ");
		}
		else{									//label 있는 경우 출력
			fprintf(fp,"%-7s ",label);
		}
		fprintf(fp,"%-7s",opcode);				//opcode 출력
		if(bflg==3){							//operand2까지 있는 경우
			strcpy(temp,operand1);
			temp2 = strcat(strcat(temp,", "),operand2);
			fprintf(fp,"%-15s",temp2);
		}
		if(nullCheck(operand1)){				//operand1도 없는 경우
			fprintf(fp,"               ");
		}
		else if(bflg!=3){						// operand1만 있는 경우 출력
			fprintf(fp,"%-15s",operand1);
		}
		op = check_opcode(opcode);					//object code 생성하기 위한  opcode 확인
		
		
		if(op==6 || op == 3 || op == 5){			// base, resw, resb 경우 temp.txt에서 구분하기 위함
			if(op==6){
				base = getLoc(operand1);			//base는 temp.txt에서 구분해서 처리하기 때문
				fprintf(obj,".");
			}
			else fprintf(obj,",");
		}
		else if(op==4){								//byte의 경우
			strcpy(temp,operand1);
			tok = strtok(temp,"'");
			if(strcmp(tok,"C")==0){						// C 처리
				tok = strtok(NULL,"'");
				strcpy(temp3,tok);
				len = strlen(temp3);
				for(int z=0;z<len;z++){
					fprintf(fp,"%X",temp3[z]);
					fprintf(obj,"%X",temp3[z]);
				}
			}
			else{										// X 처리
				tok = strtok(NULL,"'");
				len = strlen(tok);
				strcpy(temp3,tok);
				if(len %2!=0){							// 바이트 단위로 할당해야 하기에 짝수 맞춰주기 위해 0 출력
					fprintf(fp,"0");
					fprintf(obj,"0");
				}
				fprintf(fp,"%s",tok);
				fprintf(obj,"%s",tok);
			}
		}
		else if(op == 2){						//WORD의 경우 
			
			fprintf(fp,"%06X",atoi(operand1));
			fprintf(obj,"%06X",atoi(operand1));

		}
		else if(op == -1){						// directive가 아닌 opcode의 경우
			opmode = find_opcode(opcode);					//opcode의 값을 불러온다 
			format = getFormat(opcode);					//opcode의 format을 불러온다 
			if(format == 1){				// format 1인 경우
				fprintf(fp,"%s",opmode);
				fprintf(obj,"%s",opmode);
			}
			else if(format == 2){			// format 2인 경우
				r1 = checkR(operand1);			//checkR은 operand가 무슨 레지스터인지 확인하기 위한 함수
				if(r1==-1){			//operand1이 레지스터가 아닌 경우
					error(lineNum,"Error! operand is not register!");
					flg=0;
					break;
				}
				else{

					if(bflg==3){			//operand2 있음
						r2 = checkR(operand2); 
						if(r2==-1){ error(lineNum,"Error! operand is not register!"); flg=0; break;}	//operand2가 레지스터가 아닌 경우
						fprintf(fp,"%s%X%X",opmode,r1,r2);
						fprintf(obj,"%s%X%X",opmode,r1,r2);
					}
					else{					//operand1만 있음
						fprintf(fp,"%s%X0",opmode,r1);
						fprintf(obj,"%s%X0",opmode,r1);
					}
				}
			}
			else if(format==3){						//format 3이나 4인 경우,  opcode.txt에서 읽을 때 3/4의 형식도 3으로 저장하였기 때문
				ni = checkNI(operand1);				//checkNI() 호출하여 ni 값 계산
				ta = getLoc(operand1);				//getLoc() 호출하여  operand1의 주소 값 계싼
				if(ta==-1){							//operand가 없는 경우 	
					if(nullCheck(operand1)){				//RSUB과 같은 경우 처리
						fprintf(fp,"%02X0000" ,toHex(opmode)+ni);
						fprintf(obj,"%02X0000" ,toHex(opmode)+ni);
					}
					else{
						error(lineNum,"operand doesn't exist in SYMTAB!");
						flg =0;
						break;
					}
				}
				else if(opcode[0] == '+'){				// format 4인 경우
					e = 1;
					fprintf(fp,"%02X%X%05X",toHex(opmode)+ni,e,ta);		
					fprintf(obj,"%02X%X%05X",toHex(opmode)+ni,e,ta);	
				}
				else{
					if(bflg==3){
						if(checkR(operand1)== 1 || checkR(operand2)==1){			//index bit가 있는 경우
							x=8;
						}
					}
					if(operand1[1]>='0' && operand1[1]<='9'){
						fprintf(fp,"%02X0%03X",toHex(opmode)+ni,ta);					//format 3 & immediate address
						fprintf(obj,"%02X0%03X",toHex(opmode)+ni,ta);
					}
					else if(ta-LOC[cnt]>=-2048 && ta-LOC[cnt]<=2047){					// pc relative mode
						if((ta-LOC[cnt])<0){							//음수인 경우 4095와 and 연산 해준다.
							fprintf(fp,"%02X%X%03X",toHex(opmode)+ni,x+2,(ta-LOC[cnt])&4095);
							fprintf(obj,"%02X%X%03X",toHex(opmode)+ni,x+2,(ta-LOC[cnt])&4095);
						
						}
						else{								//0보다 같거나 큰 경우
							fprintf(fp,"%02X%X%03X",toHex(opmode)+ni,x+2,ta-LOC[cnt]);	 
							fprintf(obj,"%02X%X%03X",toHex(opmode)+ni,x+2,ta-LOC[cnt]);	
						}
					}
					else if(ta-base>= 0 && ta-base<=4095){							//		base relative mode
						fprintf(fp,"%02X%X%03X" ,toHex(opmode)+ni,x+4,ta-base);
						fprintf(obj,"%02X%X%03X" ,toHex(opmode)+ni,x+4,ta-base);
					}
					else{
						error(lineNum,"Memory indexing error!");		// format이 3형식인데, pc relative도 아니고, base relative도 아닌 경우 에러 처리
						flg=0;
						break;

					}
				}
			}
		}
		else fprintf(obj,".");
		fprintf(obj," %d\n",ni);
		fprintf(fp,"\n");
	}
	line_length = cnt;					// lst 파일이 몇 줄의 코드인지 저장
	fclose(im);
	remove("immediate_file");				//immediate_file 삭제
	fclose(fp);
	fclose(obj);
	return flg;			
}
int pass1(char* filename){
	
	FILE* fp = NULL;
	FILE *im = NULL;
	char line[100];
	char* label,*opcode,*operand1,*operand2;
	char* tok,temp[100],temp2[100];
	int op,tokenCount,lineNum=5,loc,cnt=0,i=0;
	int flg=1,chk,chk2=0,bflg=0;
	fp = fopen(filename,"r");									//.asm 파일을 읽는다.
	im = fopen("immediate_file","w");							//immediate_file에 저장한다.
	while(fgets(line,100,fp)!=NULL) i++;

	LOC = (int*)malloc(sizeof(int)*i);
	fclose(fp);
	fp = fopen(filename,"r");
	while(fgets(line,100,fp)!= NULL ){
		line[strlen(line)-1]=0;
		strcpy(temp,line);
		tokenCount = count_token(temp);
		strcpy(temp,line);
		label = strtok(temp," \t");
		opcode = strtok(NULL," \t");
		operand1 = strtok(NULL," \t");
		operand2 = "NULL";
		if(tokenCount != 0){						// 빈 줄이 아닌 경우만 처리
			if(line[0] == '.'){						//주석인 경우
				fprintf(im,"%d|%d ",2,lineNum);
				strcpy(temp,line);
				tok = strtok(temp," \t");
				while(tok!= NULL){
					fprintf(im," %s",tok);
					tok = strtok(NULL," \t");
				}
				fprintf(im,"\n");
				lineNum+= 5; cnt+=1;
				continue;
			}
			if(line[0] == ' ' || line[0] == '\t'){			//label 없는 경우
				op = check_opcode(label);
				if(op==1){
					bflg=1;
				}
				if(op==6) bflg=1;
				chk = setLoc(op,"NULL",label,opcode,&loc);
				if(tokenCount==3 && strcmp(opcode+strlen(opcode)-1,",")==0){				//   opcode a, b인 경우
					chk2=1;	
					strcpy(temp,line);
					label = "NULL";
					opcode = strtok(temp," \t");
					operand1 = strtok(NULL,", \t");
					operand2 = strtok(NULL," \t");
					bflg=3;
				}
			}
			else{						//label 있는 경우
				if(tokenCount==4){							//label opcode a, b인 경우
					operand2 = strtok(NULL," \t");
					strcpy(temp2,operand1);
					operand1 = strtok(temp2,",");
					bflg=3;
				}
				op = check_opcode(opcode);
				if(op != 0)
					if(addS(label,loc)==0){				// label 관련 에러 처리
						error(lineNum,line); flg=0; break;}
				chk = setLoc(op,label,opcode,operand1,&loc); 
			}
			
			if(chk ==-1){ error(lineNum,line);flg=0; break;}
			
			if((line[0] == ' ' || line[0] == '\t')&&chk2==0){						//label 없고, opcode a,b의 형태가 아닌 경우
				 fprintf(im,"%d|%d|%d|%s|%s|%s|%s\n",bflg,lineNum,loc,"NULL",label,opcode,operand1);				// label이 없으면 label자리에 NULL을 넣었다
			}
			else fprintf(im,"%d|%d|%d|%s|%s|%s|%s\n",bflg,lineNum,loc,label,opcode,operand1,operand2);			//label 있거나, operand2가 있는 경우
			LOC[cnt] = loc;				//전역 변수 LOC에 현재 라인의 loc 저장
			cnt++;
			loc += chk;					// 현재 라인의 코드가 할당할 만큼의 메모리를 loc에 더한다.
		}
		lineNum += 5;
		chk2 =0; bflg=0;
	}

	fclose(fp);
	fclose(im);
	return flg;
}
void makeObj(char* filename){

	FILE* fp = NULL;
	FILE* ls = NULL;
	char obj[10];
	int len,i,cnt=0,im,temp;
	int prevLoc,curLoc,startLoc;
	int start,last,flg=0;
	int spflg=0;
	fp = fopen(filename,"w");
	ls = fopen("temp.txt","r");
	int* arr1 = (int*)malloc(sizeof(int)*line_length);
	char** arr2 = (char**)malloc(sizeof(char*)*line_length);
	for(i=0;i<line_length;i++)
		arr2[i] = (char*)malloc(sizeof(char)*100);					//T라인, M라인 두 번 읽기 때문에 arr2 배열에 1번 읽을 때 저장하였다.

	fprintf(fp,"H%-6s%06X%06X\n",program_name,program_start,program_length);
	for(i=0;i<line_length;i++){
		fgets(arr2[i],100,ls);
	}
	start=1;last=0;
	
	while(1){
		cnt=0;
		start = last+1;
		flg=0;						// end에 도달하면 1
		i = start;
		startLoc = -1;					//startLoc -1로 초기화하여 처음 T라인에 출력할 부분의 loc을 저장한다.
		spflg=0;
		while(1){
			sscanf(arr2[i],"%d %s",&curLoc,obj);
			if(obj[0] == '.'){			//주석인 경우
				i++;
			}
			else{
				sscanf(arr2[i],"%d %s %d",&curLoc,obj,&im);
				arr1[i] = im;						//immediate address mode가 아닌 format 4 확인하기 위함
				if(startLoc==-1) {					// T라인시작 주소 초기화
					startLoc = curLoc;
					prevLoc = curLoc;
				}
				if(obj[0]==','){					//RESW,RESB
					spflg=1;					
				}
				else{
					if(spflg==1){					// 그 전 라인의 object code가 없었으면 멈추고 T라인을 출력한다.
						if(cnt==0){					// 처음 등장한 라인이 resw, resb와 같은 경우 시작을 현재 라인으로 한다.
							start = i;
							spflg=0;
							startLoc = curLoc;
						}
						else{
							spflg=0;				//멈추고 T라인 출력
							break;

						}
					}
					else if(((curLoc-prevLoc) +cnt)>30 || (cnt + strlen(obj)/2)>30){				//길이가 1E를 넘어가거나, 메모리 차이가 1E를 넘어가는 경우 멈춘다.
						spflg=0;
						break;
					}
					
					cnt += strlen(obj)/2;	
					
					last = i;					// T라인 출력할 때 현재 라인까지 출력하게끔 하였다.
				}
				i++;
				prevLoc = curLoc;				//prevLoc 수정
			}
			if(i==line_length){					//end 도달한 경우
				flg=1;
				last = i-1;
				break;
			}
		}
		fprintf(fp,"T%06X%02X",startLoc,cnt);				//T 라인 출력
		for(i=start;i<=last;i++){
			sscanf(arr2[i],"%d %s",&temp,obj);
			if(obj[0]=='.') continue;						//주석이나 base는 출력 x
			else if(obj[0]!=',')fprintf(fp,"%s",obj);		//object 코드 출력
		}
		fprintf(fp,"\n");
		if(flg==1) break;
	}
	for(i=0;i<line_length;i++){								// M라인을 작성하기 위해 다시 읽는다.
		sscanf(arr2[i],"%d %s",&curLoc,obj);
		len =0;
		if(obj[0]=='.')continue;
		else if(obj[0]!=','){
			len = strlen(obj);
		}
		if(len==8 && arr1[i]!=1){						//format4이고 immediate addressing이 아니면 M 라인에 출력
			fprintf(fp,"M%06X%02X\n",curLoc+1,5);
		}
	}
	fprintf(fp,"E%06X\n",program_start);				// E라인 출력
	free(arr1);
	for(i=0;i<line_length;i++)
		free(arr2[i]);
	free(arr2);
	fclose(ls);
	fclose(fp);
	remove("temp.txt");
	return;
}
int assemble_filename(char* target){				//assemble기능을 하는 함수, pass1과 pass2, makeObj를 호출한다.	

	int len = strlen(target),flag=0;
	FILE* fp  = NULL;
	char instr[31];
	if(strcmp(target+len-4,".asm")!=0){ printf("Error! File should be .asm file\n"); return 0;}			//asm파일이 아니면 에러 처리
	
	fp = fopen(target,"r");
	if(fp == NULL){ printf("Error! File doesn't exist.\n"); return 0;}						//디렉토리에 asm파일이 없는 경우 에러 처리
	
	flag = pass1(target);							//pass1 호출하여 0리턴 하면 에러 발생한 것이므로 종료
	if(flag ==0) { return 0;}
	flag = pass2(target);							//0리턴시 에러 발생한 것이므로 종료
	if(flag==0) { return 0;}				
	convertSymtab();								// 에러 없이 lst 파일이 만들어졌으므로, SYMTAB을 새롭게 교체한다.
	strcpy(instr,target);
	instr[len-3]='l'; instr[len-2]='s'; instr[len-1]='t';
	printf("[%s], ",instr);
	instr[len-3]='o'; instr[len-2]='b'; instr[len-1]='j';
	printf("[%s]\n",instr);
	makeObj(instr);				// .obj 파일을 만들기 위해 makeObj 호출

	fclose(fp);
	return 1;
}
int type_filename(char* target){			//현재 디렉토리에서 입력받은 이름의 파일을 찾아 출력하는 함수
	
	FILE* fp = NULL;
	char c;
	
	fp = fopen(target,"r");
	if(fp == NULL){ printf("Error! file doesn't exist!\n"); return 0;}			//없으면 0리턴
	while(fscanf(fp,"%c",&c)!= EOF){
		printf("%c",c);
	}
	fclose(fp);

	return 1;
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
	SYMTAB* s_cur,*s_prev;
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
		free(n_prev);				
	}
	for(i=0;i<20;i++){					// label을 저장하고 있는 symtabP free해주는 코드
		s_cur = symtabP[i];
		while(s_cur!=NULL){
			s_prev = s_cur;
			s_cur = s_cur->link;
			free(s_prev);
		}
	}
	free(LOC);
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
	
	unsigned int len = strlen(target);
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
int isUpper(char* target){
	
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
		format = strtok(NULL," \t/");
		Hash* new = (Hash*)malloc(sizeof(Hash));		//새로운 opcode를 저장하기 위함
		strcpy(new->opcode,opcode);
		strcpy(new->instruction,instruction);
		new->format = atoi(format);
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
	printf("assemble filename\n");
	printf("type filename\n");
	printf("symbol\n");
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
	else if(!strcmp(inputstr,"assemble")){
		return 10;
	}
	else if(!strcmp(inputstr,"type")){
		return 11;
	}
	else if(!strcmp(inputstr,"symbol")){
		return 12;
	}
	else return -1;
}
