#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>

uint8_t binary_to_byte(char* buf,int len){
	int i, diff=1-'1';	
	uint8_t sum=0,mul = 1<<(len-1);
	
	for(i=0;i<len;i++,mul/=2){
		sum += (buf[i] + diff)*mul;
	}
	return sum;
}
void get_input(int argc,char* argv[],FILE** ifp,FILE** ofp,uint8_t* generator, int* uint8_tword_size,int *gen_size){

	if(argc != 5){
		printf("uage: ./crc_encoder input_file output_file generator uint8_tword_size\n");
		exit(-1);
	}
	*ifp = fopen(argv[1],"r");
	if(*ifp == NULL){
		printf("input file open error.\n");
		exit(-1);
	}
	*ofp = fopen(argv[2],"w");
	if(*ofp == NULL){
		printf("output file open error.\n");
		exit(-1);
	}
	if(strcmp(argv[4],"4") !=0 && strcmp(argv[4],"8") != 0){
		printf("uint8_tword size must be 4 or 8\n");
		exit(-1);
	}

	*uint8_tword_size = atoi(argv[4]);
	*gen_size = strlen(argv[3]);

	*generator = binary_to_byte(argv[3],*gen_size);

	return;
}
uint16_t modul_2_div(uint16_t x,uint8_t div,int div_size,int uint8_tword_size){
	
	
	uint16_t check;
	uint16_t dividend,temp;
	x = x << (16-uint8_tword_size);
	
	uint16_t divisor = div;
	divisor = divisor << 12;
	
	for(int i=0;i<uint8_tword_size;i++){
		check = x & (0xF000);
		if(check >= 1<<15){
			dividend = x & (0x0FFF);
			temp = divisor ^ check;
			x = temp + dividend;
		}
		if(i==uint8_tword_size-1)
			break;
		x = x << 1;
	}
	
	return (x>>12)&0x000F;
}
int save(char* result_buffer,int result_buffer_index,uint16_t to_be_saved, int howmany){


	int* arr = (int*)malloc(sizeof(int)*(howmany+2));
	int i = 0,j;
	int diff = '1'-1;
	
	while(to_be_saved){
		arr[i++] = to_be_saved % 2;
		to_be_saved /= 2;
	}
	
	for(j=0;j<howmany;j++){
		result_buffer[result_buffer_index + j] = j<howmany-i ? '0' : arr[howmany-1-j] + diff;
	}	

	free(arr);
	return howmany;

}
void encode(FILE* ofp,uint8_t* buffer,uint8_t generator, int uint8_tword_size,int gen_size, int buf_size){

	int i,j;
	int pading_unit = uint8_tword_size == 4 ? 2 : 1;
	int result_buffer_index = 0;
	int diff = 1-'1';
	
	uint8_t padding = ((16-((uint8_tword_size+3)* pading_unit))*buf_size) %8;
	uint8_t sum;
	
	uint16_t codeword,remainder;
	uint16_t front,rear,remainder_f,remainder_r;
	
	int result_buffer_size = 8 + padding +  ((uint8_tword_size+3) * pading_unit )*buf_size + 1;

	char* result_buffer = (char*)malloc(sizeof(char)*(result_buffer_size));

	result_buffer_index += save(result_buffer,result_buffer_index,padding,8);
	result_buffer_index += save(result_buffer,result_buffer_index,0,padding);


	if(uint8_tword_size == 4){	
		for(i=0;i<buf_size;i++){
			front = (((buffer[i]&0xF0)>>4)&0x0F);
			rear = (buffer[i]&0x0F);
			
			remainder_f = modul_2_div(front,generator,gen_size,uint8_tword_size);
			remainder_r = modul_2_div(rear,generator,gen_size,uint8_tword_size);
			
			front = ((front << 3) + remainder_f)<<7;
			rear = (rear << 3)+remainder_r;
			
			codeword = front + rear;
			
			save(result_buffer,result_buffer_index,codeword,14);
			result_buffer_index += 14;
		}

	}
	else{
		for(i=0;i<buf_size;i++){
			remainder = modul_2_div((uint16_t)buffer[i],generator,gen_size,uint8_tword_size);
			
			codeword = ((uint16_t) buffer[i] << 3) + remainder;

			save(result_buffer,result_buffer_index,codeword,11);
			result_buffer_index += 11;

		}
	}
	for(i=0;i<result_buffer_size / 8;i++){
		sum = binary_to_byte(result_buffer+(i*8),8);	
		fwrite(&sum,sizeof(uint8_t),1,ofp);	
		
	}
	free(result_buffer);

}
int main(int argc,char* argv[]){
		
	FILE* ifp,*ofp;
	int uint8_tword_size,gen_size;
	uint8_t generator = 0;
	
	get_input(argc,argv,&ifp,&ofp,&generator,&uint8_tword_size,&gen_size);
		
	fseek(ifp,0,SEEK_END);
	int buf_size = ftell(ifp);	
	fseek(ifp,0,SEEK_SET);

	uint8_t* buffer = (uint8_t*)malloc(sizeof(uint8_t)*buf_size);
	fread(buffer,sizeof(uint8_t),buf_size,ifp);	

	encode(ofp,buffer,generator,uint8_tword_size,gen_size,buf_size);

	free(buffer);
	fclose(ifp);
	fclose(ofp);

	return 0;
}

