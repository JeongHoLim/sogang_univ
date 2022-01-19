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
	char format[5];
}Hash;

/* 전역 변수 */
int history_num;		// 현재 저장되어 있는 명령어 개수
int address = -1;		// 초기 상태의 address
Node* head = NULL;		// 명령어 저장할 linked list를 가리키는 변수
Hash* table[20];		// opcode.txt.에서 읽은 opcode를 저장하는 변수
unsigned char memory[16*65536] = {0,};	// 할당한 1MB의 메모리 공간


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
 * 인자 : char*형 1개, int형 1개, 검사할 문자열과 값의 용도 플래그
 * 리턴값 : 정상 범위이면 1, 아니면 0
 * */
int isHex(int,int);

/*
 * 함수 : check_opcode
 * 목적 : 입력된 문자열이 대문자로 되어있는지 확인
 * 인자 : char*형 하나, 검사할 문자열
 * 리턴값 : 대문자로 되어있으면 1리턴, 아닐 시 0
 * */
int check_opcode(char*);

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
