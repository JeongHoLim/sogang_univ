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
uint16_t binary_to_two_byte(char* buf,int len){

	int i,diff = 1-'1';
	uint16_t sum = 0,mul = 1<<(len-1);

	for(i=0;i<len;i++,mul/=2){
		sum += (buf[i]+diff)*mul;
	}
	return sum;

}
void get_input(int argc,char* argv[],FILE** ifp,FILE** ofp1,FILE** ofp2,uint8_t* generator, int* dataword_size,int *gen_size){

    if(argc != 6){
        printf("uage: ./crc_decoder input_file output_file result_file generator dataword_size\n");
        exit(-1);
    }
    *ifp = fopen(argv[1],"r");
    if(*ifp == NULL){
        printf("input file open error.\n");
        exit(-1);
    }
    *ofp1 = fopen(argv[2],"w");
    if(*ofp1 == NULL){
        printf("output file open error.\n");
        exit(-1);
    }
	*ofp2 = fopen(argv[3],"w");
	if(*ofp2 == NULL){
		printf("result file open error.\n");
		exit(-1);
	}
    if(strcmp(argv[5],"4") !=0 && strcmp(argv[5],"8") != 0){
        printf("dataword size must be 4 or 8\n");
        exit(-1);
    }

    *dataword_size = atoi(argv[5]);
    *gen_size = strlen(argv[4]);

    *generator = binary_to_byte(argv[4],*gen_size);

    return;
}
int save(char* buffer,int result_buffer_index,uint8_t to_be_restored,int howmany){
	
	int* arr = (int*)malloc(sizeof(int)*(howmany+1));
	memset(arr,0,sizeof(int)*(howmany+1));
	int i=0,j,diff = '1'-1;
	
	while(to_be_restored){
		arr[i++] = to_be_restored % 2;
		to_be_restored /= 2;
	}
	
	for(j=0;j<howmany;j++){
		buffer[result_buffer_index + j] = arr[howmany-1-j] + diff;
	}
	free(arr);
	return howmany;

}
void restore(char* result_buffer,int result_buffer_size,uint8_t* buffer,int buf_size,uint8_t padding_size){
	
	int i,result_buffer_index=0;
	int8_t mask = 0x80;
	mask = ~(mask >> (padding_size == 0 ? 0 : padding_size-1));

	result_buffer_index += save(result_buffer,0,buffer[1]&mask,8-padding_size);

	for(i=2;i<buf_size;i++){
		result_buffer_index += save(result_buffer,result_buffer_index,buffer[i],8);
	}

}
uint16_t modul_2_div(uint16_t x,uint8_t div,int div_size,int dataword_size){

    uint16_t check;
	uint16_t divisor = div;
	uint16_t dividend,temp;

	int shift = dataword_size == 4 ? 9 : 5;

	x = x << shift;
    divisor = divisor << 12;
    for(int i=0;i<dataword_size;i++){
        check = x & (0xF000);
        if(check >= 1<<15){
            dividend = x & (0x0FFF);
            temp = divisor ^ check;
            x = temp + dividend;
        }
        if(i==dataword_size-1)
            break;
        x = x << 1;
    }

    return (x>>12)&0x000F;
}
void decode(FILE* ofp1,FILE* ofp2,uint8_t* buffer,uint8_t generator,int dataword_size,int gen_size,int buf_size){

	int i;
	uint8_t padding_size = buffer[0];
	int result_buffer_size = 8*(buf_size-1)-padding_size;
	
	char* result_buffer = (char*)malloc(sizeof(char)*result_buffer_size);
	
	restore(result_buffer,result_buffer_size,buffer,buf_size,padding_size);
	
	int codeword_length = (dataword_size == 4) ? 7 : 11;
	int codeword_size = result_buffer_size / codeword_length;

	char* output_buffer = (char*)malloc(sizeof(char)*(dataword_size*codeword_size)); 
	int output_buffer_index = 0;

	uint8_t target;
	int error_count = 0;
	int iter = (dataword_size == 4)? 2:1;
	
	uint16_t codeword; 
	uint8_t dataword;
	uint8_t mask = 0x80;
	for(i=0;i<codeword_size;i++){
		codeword = binary_to_two_byte(result_buffer + (i*codeword_length),codeword_length);
		target = modul_2_div(codeword,generator,gen_size,dataword_size);	
		if(target != 0) error_count += 1;
		
		dataword = (uint8_t)(codeword >> 3);

		if(dataword_size == 4) 
			dataword = dataword & (0x1F);

		output_buffer_index += save(output_buffer,output_buffer_index,dataword ,dataword_size);
	}

	uint8_t temp;

	for(i=0;i<codeword_size/iter;i++){
		temp = binary_to_byte(output_buffer+(i*8),8);
		fwrite(&temp,sizeof(uint8_t),1,ofp1);
	}
	fprintf(ofp2,"%d %d",codeword_size,error_count);
	
	free(output_buffer);
	free(result_buffer);
}
int main(int argc,char* argv[]){

    FILE* ifp,*ofp1,*ofp2;
    int dataword_size,gen_size;
    uint8_t generator = 0;

    get_input(argc,argv,&ifp,&ofp1,&ofp2,&generator,&dataword_size,&gen_size);


    fseek(ifp,0,SEEK_END);
    int buf_size = ftell(ifp);
    fseek(ifp,0,SEEK_SET);

    uint8_t* buffer = (uint8_t*)malloc(sizeof(uint8_t)*buf_size);
    fread(buffer,sizeof(uint8_t),buf_size,ifp);

	decode(ofp1,ofp2,buffer,generator,dataword_size,gen_size,buf_size);

    free(buffer);
    fclose(ifp);
    fclose(ofp1);
	fclose(ofp2);

    return 0;
}

