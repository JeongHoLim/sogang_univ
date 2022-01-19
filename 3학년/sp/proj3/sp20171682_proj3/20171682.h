#include<stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#define MAXLINE 50
#define ASCII_MIN 32
#define ASCII_MAX 126
#define KEY 20

/*입력된 명령어를 linked list에 저장하기 위한 구조체*/
typedef struct node{
	struct node* link;
	char mnemonic[30];
	int num;
}Node;

/*opcode를 hash table에 저장하기 위한 구조체*/
typedef struct hash{
	struct hash* link;
	char opcode[4];
	char instruction[10];
	int format;
}Hash;
/* symbol들을 저장하기 위한 구조체 */
typedef struct symtab{
	struct symtab* link;	
	char label[15];
	int loc;

}SYMTAB;
/*external symbol의 정보를 저장하기 위한 구조체*/
typedef struct estab{
	
	char name[10];
	int flag;						//flag가1이면 control section,0이면 symbol
	int addr;
	int length;
	struct estab* link;

}ESTAB;

/*break point을 저장하기 위한 구조체*/
typedef struct bp{
	int bp;
	struct bp* link;

}BP;


/* 전역 변수 */
ESTAB *estab[3];		// 최대 3개까지 control  section의 estab을 가리킨다.
BP* bps = NULL;			// sicsim에 설정된 breakpoint를 저장
BP* bp_ptr = NULL;		// 프로그램이 실행될 breakpoint를 가리킨다.
int BP_num;						// break point의 총 개수
int A,X,L,PC,B,S,T,F,SW;		//레지스터 
char reglist[10][3] ={"A","X","L","B","S","T","F","__","PC","SW"};		//레지스터 이름 가지고 있는 배열
int CC;					//조건문의 결과를 저장
int history_num;		// 현재 저장되어 있는 명령어 개수
int address = -1;		// 초기 상태의 address
Node* head = NULL;		// 명령어 저장할 linked list를 가리키는 변수
Hash* table[20];		// opcode.txt.에서 읽은 opcode를 저장하는 변수
unsigned char memory[16*65536] = {0,};	// 할당한 1MB의 메모리 공간
SYMTAB* symtabP[20];			//assemble 성공한 파일의 symtab을 가리키는 변수
SYMTAB* symtabN[20];			//현재 assemble 중인 파일의 symtab을 가리키는 변수
int* LOC;						// 모든 라인의 location counter를 저장하는 배열을 가리키는 포인터 변수
int isSYM;						//현재 assemble 성공한 파일의 symtab이 있는지 알려는 변수
char program_name[15];			// asm 파일의 이름을 저장하는 변수
int program_length;		       // assemble 성공한 파일의 프로그램 길이
int program_start;               // assemble 성공한 파일의 프로그램 시작 주소
int line_length;				// lst파일의 길이
int sp_cnt;                     //symtabP에 저장되어 있는 symbol의 개수
int sn_cnt;                     // symtabN에 저장되어 있는 symbol의 개수
int base;						//base relative 연산에 쓰일 base 값
int progaddr;					//loader 또는 run 명령어를 수행할 때 시작하는 주소 값 저장
int csaddr;						//현재 loader에 의해 scan되고 있는 control section의 주소 저장
int bp_start;					//run이 시작될 pc

/*
 * 함수 : check_input
 * 목적 : 입력된 문자열이 shell 명령어인지 확인하기 위함.
 * 인자 : char*형 하나, shell에서 입력한 문자열
 * 리턴값 : shell 명령어이면 각각의 경우 맞게 0~9까지의 정수 출력, 아닐 시 -1
 * */
int check_input(char* );

/*
 * 함수 : dir
 * 목적 : 같은 디렉토리에 있는 파일 출력
 * 인자 : 없음
 * 리턴값 : 없음
 * */
void dir();

/*
 * 함수 : help
 * 목적 : 제공하는 shell 명령어 출력
 * 인자 : 없음
 * 리턴값 : 없음
 * */
void help();

/*
 * 함수 : history
 * 목적 : 정상적으로 입력된 명령어를 linked list로 저장하기 위함.
 * 인자 : char*형 하나, 정상 입력된 명령어
 * 리턴값 : 없음
 * */
void history(char*);

/*
 * 함수 : print_history
 * 목적 : 콘솔 창에서 입력된 명령어들을 출력하기 위함.
 * 인자 : 없음
 * 리턴값 : 없음
 * */
void print_history();

/*
 * 함수 : dump
 * 목적 : 사용자가 입력한 범위에 맞게 메모리 출력
 * 인자 : int형 2개, start와 end
 * 리턴값 : 없음
 * */
void dump(int,int);

/*
 * 함수 : edit
 * 목적 : address에 value를 할당
 * 인자 : int형 2개, address와 value
 * 리턴값 : 없음
 * */
void edit(int,int);

/*
 * 함수 : fill
 * 목적 : 사용자가 원하는 범위에 value를 할당
 * 인자 : int형 3개, start,end,value
 * 리턴값 : 없음
 * */
void fill(int,int,int);

/*
 * 함수 : reset
 * 목적 : 모든 메모리의 값을 0으로 할당
 * 인자 : 없음
 * 리턴값 : 없음
 * */
void reset();

/*
 * 함수 : make_hash
 * 목적 : opcode.txt에서 opcode와 mnemonic을 해시 함수를 이용하여 저장
 * 인자 : 없음
 * 리턴값 : 없음
 * */
void make_hash();

/*
 * 함수 : isHex
 * 목적 : 값의 용도에 따라 정상 범위의 16진수인지 확인
 * 인자 : int형 2개, 검사할 값과 그 값의 용도 플래그
 * 리턴값 : 정상 범위이면 1, 아니면 0
 * */
int isHex(int,int);

/*
 * 함수 : isUpper
 * 목적 : 입력된 문자열이 대문자로 되어있는지 확인
 * 인자 : char*형 하나, 검사할 문자열
 * 리턴값 : 대문자로 되어있으면 1리턴, 아닐 시 0
 * */
int isUpper(char*);

/*
 * 함수 : opcode_mnemonic
 * 목적 : 입력한 문자열이 저장되어 있는지 확인
 * 인자 : char*형 하나, 확인할 문자열
 * 리턴값 : 없음
 * */
void opcode_mnemonic(char*);

/*
 * 함수 : opcodelist
 * 목적 : opcode.txt에서 읽은 opcode들을 출력
 * 인자 : 없음
 * 리턴값 : 없음
 * */
void opcodelist();

/*
 * 함수 : hash_function
 * 목적 : mnemonic의 hash value를 계산하여 hash table에서의 위치를 찾기 쉽게 위함.
 * 인자 : char*형 하나, mnemonic이 담긴 문자열
 * 리턴값 : 해당 mnemonic의 hash value
 * */
int hash_function(char*);

/*
 * 함수 : free_memory
 * 목적 : 동적할당한 메모리를 free하기 위함.
 * 인자 : 없음
 * 리턴값 : 없음
 * */
void free_memory();

/*
 * 함수 : check_error
 * 목적 : dump,edit,fill등의 함수에서 0x나 0X의 형태의 16진수 입력을 예외처리한다.
 * 인자 : char*형 1개, 입력된 문자열
 * 리턴값 : 0x나 0X의 형식의 16진수이면 1리턴, 아닐 시 0리턴
 * */
int check_error(char*);
/*
 * 함수 : type_filename 
 * 목적 : 현재 디렉토리에서 입력받은 이름의 파일을 찾아 출력하기 위함.
 * 인자 : char*형 1개, 검색할 파일이 담긴 문자열
 * 리턴값 : 검색할 파일이 없으면 0, 있으면 1 리턴
 * */
int type_filename(char*);

/*
 * 함수 : assemble_filename
 * 목적 : asm파일을 읽어서 lst파일과obj파일을 생성한다.
 * 인자 : char*형 1개, assemble할 파일의 이름이 담긴 문자열
 * 리턴값 : assemble 실패하면 0, 성공하면 1 리턴
 * */
int assemble_filename(char*);

/*
 * 함수 : pass1
 * 목적 : asm 파일을 읽어서 pass2에서 사용할 symtab과 immediate file을 생성한다.
 * 인자 : char*형 1개, assemble할 파일의 이름이 담긴 문자열
 * 리턴값 : symtab을 생성하면서 에러가 발생하면 0, 성공하면 1리턴
 * */
int pass1(char*);

/*
 * 함수 : pass2
 * 목적 : pass1에서 생성한 symtab과 immediate file을 읽어서 lst파일을 생성한다.
 * 인자 : char*형 1개, assemble할 파일의 이름이 담긴 문자열
 * 리턴값 : lst파일 생성에 성공하면 1, 실패하면 0리턴
 * */
int pass2(char*);

/*
 * 함수 : checkR
 * 목적 : 입력받은 문자열이 어떤 레지스터인지 알려준다.
 * 인자 : char*형 1개, 어떤 레지스터인지 확인할 문자열
 * 리턴값 : 각 레지스터에 맞는 번호를 리턴, 해당되는 레지스터가 없으면 -1리턴
 * */
int checkR(char*);

/*
 * 함수 : checkNI
 * 목적 : addressing 방법을 구분하여 n,i 비트의 값을 구하기 위함.
 * 인자 : char*형 1개, n,i값을 구할 문자열
 * 리턴값 : immediate이면 1, indirect이면 2, 그 외의 경우 3리턴
 * */
int checkNI(char*);

/*
 * 함수 : toHex
 * 목적 : 문자열을 16진수의 수로 리턴
 * 인자 : char*형 1개, 16진수로 값을 확인할 문자열
 * 리턴값 : 16진수로 읽었을 때의 값 리턴
 * */
int toHex(char*);

/*
 * 함수 : check_opcode
 * 목적 : opcode가 어떤 directive인지 확인하기 위함.
 * 인자 : char*형 1개, 확인할 opcode가 담긴 문자열
 * 리턴값 : 각 directive에 맞는 수 리턴, directive가 아닐 시 -1 리턴
 * */
int check_opcode(char*);

/*
 * 함수 : find_opcode
 * 목적 : mnemonic의 opcode 값을 구하기 위함
 * 인자 : char*형 1개, opcode 값을 구할 mnemonic이 담긴 문자열
 * 리턴값 : 검색할 문자열의 opcode룰 구하면 opcode를 리턴, 검색에 실패할 시 NULL 리턴
 * */
char* find_opcode(char*);

/*
 * 함수 : count_token
 * 목적 : 공백 기준 몇 개의 코드로 이루어져 있는지 확인하기 위함
 * 인자 : char* 형 1개, asm파일에서 읽은 line이 담긴 문자열
 * 리턴값 : 공백 기준 몇 개인지 개수 리턴
 * */
int count_token(char*);

/*
 * 함수 : getFormat
 * 목적 : mnemonic의 format을 확인하기 위함.
 * 인자 : char*형 1개, format을 확인할 mnemonic이 담긴 문자열
 * 리턴값 : opcode 저장하고 있는 table에서 검색에 성공하면 해당 format 리턴, 실패하면-1 리턴 
 * */
int getFormat(char*);

/*
 * 함수 : error
 * 목적 : assemble 과정 중 에러가 발생했을 때 라인 넘버를 출력하기 위함.
 * 인자 : int형1개, char*형 1개. lineNum, 에러가 발생한 라인이나 발생한 이유가 담긴 문자열
 * 리턴값 : 없음
 * */
void error(int,char*);

/*
 * 함수 : setLoc
 * 목적 : 해당line의 코드가 얼만큼의 메모리공간을 할당하고 있어야 하는지 알려준다.
 * 인자 : int형 1개, char*형 3개, int*형 1개.  opcode의 종류를 알 수 있는 op, asm파일에서 읽은 label ,opcode,operand 그리고 그 라인의 location counter의 주소 값
 * 리턴값 : 호출한 line의 코드가 할당받아야 할 메모리 값 리턴, 실패 시 -1 리턴
 * */
int setLoc(int,char*,char*,char*,int*);

/*
 * 함수 :  getLoc
 * 목적 : operand에 해당하는 주소를 구하기 위함
 * 인자 : char*형 1개, operand가 담긴 문자열
 * 리턴값 : symtab에서 검색할 label을 찾으면 해당 주소 리턴, immediate일 때는 숫자 그대로 리턴, 아닐 시 -1 리턴
 * */
int getLoc(char*);

/*
 * 함수 : addS
 * 목적 : pass1에서 operand에 label이름이 나오면 symtab에 저장하기 위함.
 * 인자 : char*형 1개, int형 1개. 검색할 문자열, 호출한 라인의 location counter
 * 리턴값 : symtab에 저장하면 1, 실패하면 0리턴
 * */
int addS(char*,int);

/*
 * 함수 : symbol
 * 목적 : 가장 최근에assemble 성공한 파일의 label들이 저장되어 있는 symtab을내림차순 출력
 * 인자 : 없음
 * 리턴값 : 없음
 * */
void symbol();

/*
 * 함수 : nullCheck
 * 목적 : 문자열이 비어있는지 검사하기 위함.
 * 인자 : char*형 1개, 검사할 문자열
 * 리턴값 : 비어있으면 1, 아니면 0리턴
 * */
int nullCheck(char*);

/*
 * 함수 : convertSymtab
 * 목적 : 현재 assemble한 파일의 symtab과 가장 최근에 assemble 성공한 파일의 symtab을 교체하기 위함
 * 인자 : 없음
 * 리턴값 : 없음
 * */
void convertSymtab(); 

/*
 * 함수 : freeFail
 * 목적 : assemble 과정 중 에러가 발생하여 종료할 때, 생성하던 symtabN의 메모리를 해제하기 위함
 * 인자 : 없음
 * 리턴값 : 없음
 * */
void freeFail();

/*
 * 함수 : loader 
 * 목적 : 입력 받은 오브젝트 파일을 메모리에 load하기 위함
 * 인자 : char형 2차원 배열 1개, int형 1개, load할 파일이 담긴 배열, 해당 obj 파일의 개수
 * 리턴값 : 없음
 * */
void loader(char(*)[15],int);
/*
 * 함수 : L_pass1
 * 목적 : external symbol들의 주소를 할당한다. 
 * 인자 : char형 2차원 배열 1개, int형 1개, load할 파일이 담긴 배열, 해당 obj 파일의 개수
 * 리턴값 : 없음
 * */
void L_pass1(char(*)[15],int);
/*
 * 함수 : L_pass2
 * 목적 : 실질적 loading, relocating, linking 작업 수행 
 * 인자 : char형 2차원 배열 1개, int형 1개, load할 파일이 담긴 배열, 해당 obj 파일의 개수
 * 리턴값 : 없음
 * */
void L_pass2(char(*)[15],int);
/*
 * 함수 : make_estab 
 * 목적 : obj파일을 읽어 해당 파일의 estab을 생성
 * 인자 : char*형 1개,int형 1개, 파일 이름이 담긴 문자열, 파일의 index
 * 리턴값 : 오류 발생시 0, 성공시 1
 * */
int make_estab(char*,int);
/*
 * 함수 : insert_estab
 * 목적 : control section의 symbol을 estab에삽입한다. 
 * 인자 : char*형 1개, int형 4개, symbol의 이름, cs인지 symbol인지 구분하는 flag, 주소와 길이, index
 * 리턴값 : 오류 발생시 0, 성공시 1
 * */
int insert_estab(char*,int,int,int,int);
/*
 * 함수 : print_loadmap 
 * 목적 : L_pass1에서 생성된 estab의 내용을 출력
 * 인자 : int형 1개, control section의 개수
 * 리턴값 : 없음
 * */
void print_loadmap(int);
/*
 * 함수 : free_L
 * 목적 : estab에 할당된 메모리 공간 해제
 * 인자 : 없음
 * 리턴값 : 없음
 * */
void free_L();
/*
 * 함수 : load
 * 목적 : 메모리에 프로그램을 load한다. 
 * 인자 : char*형 1개, load할 obj파일의 이름이 담긴 변수
 * 리턴값 : 없음
 * */
void load(char*);
/*
 * 함수 : find_estab
 * 목적 : estab에서 symbol의 주소를 찾기 위함 
 * 인자 : char*형 1개, 검색할 symbol의 이름이 담긴 문자열
 * 리턴값 : symbol의 주소, 없을 시 0
 * */
int find_estab(char*);
/*
 * 함수 : char_to_hex
 * 목적 : char형의 문자를 16진수 형태에 맞게 저장
 * 인자 : 16진수의 값으로 바꿀 문자
 * 리턴값 : 16진수 형태에 맞는 값 리턴
 * */
int char_to_hex(char);
/*
 * 함수 : insert_bp
 * 목적 : 새로운 break point를 삽입한다.
 * 인자 : break할loc값이 담긴 문자열
 * 리턴값 : 없음
 * */
void insert_bp(char*);
/*
 * 함수 : print_bp
 * 목적 : 현재 설정된 break point들을 출력
 * 인자 : 없음
 * 리턴값 : 없음
 * */
void print_bp();
/*
 * 함수 : clear_bp
 * 목적 : 설정된 break point를 전부 삭제
 * 인자 : 없음
 * 리턴값 : 없음
 * */
void clear_bp();
/*
 * 함수 : run
 * 목적 : 메모리에 load된 프로그램을 실행한다.
 * 인자 : 없음
 * 리턴값 : 없음
 * */

void run();
/*
 * 함수 : print_reg 
 * 목적 : 레지스티 상태를 화면에 출력
 * 인자 : 없음
 * 리턴값 : 없음
 * */
void print_reg();
/*
 * 함수 : clearReg
 * 목적 : 특정 레지스터를 0으로 reset
 * 인자 : int형 1개, reset할 레지스터의 숫자
 * 리턴값 : 없음
 * */
void clearReg(int);
/*
 * 함수 : getReg
 * 목적 : 번호에 해당하는 레지스터의 값 리턴
 * 인자 : int형 1개, 레지스터 번호를 담은 변수
 * 리턴값 : 레지스터의 값을 리턴, 실패 시 -1
 * */
int getReg(int);
