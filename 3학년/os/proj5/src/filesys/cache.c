#include<stdbool.h>
#include <string.h>
#include "threads/synch.h"
#include "cache.h"
struct buffer_cache_entry{

	bool valid_bit;
	bool reference_bit;
	bool dirty_bit;
	block_sector_t disk_sector;
	uint8_t buffer[BLOCK_SECTOR_SIZE];	
	
};
#define NUM_CACHE 64

static struct buffer_cache_entry cache[NUM_CACHE];

static struct lock buffer_cache_lock;


void buffer_cache_init(){
	lock_init(&buffer_cache_lock);
	for(int i=0;i<NUM_CACHE;i++){
		cache[i].valid_bit = false;
		cache[i].reference_bit = false;
		cache[i].dirty_bit = false;
	}

}
void buffer_cache_terminate(){
	lock_acquire(&buffer_cache_lock);

	int i;
	for(i=0;i<NUM_CACHE;i++){
		if(cache[i].valid_bit){
			buffer_cache_flush_entry(&cache[i]);
		}
	}
	lock_release(&buffer_cache_lock);
}
void buffer_cache_read(block_sector_t sector,void* to){

	lock_acquire(&buffer_cache_lock);
	
	struct buffer_cache_entry* find = buffer_cache_lookup(sector);
	if(find == NULL){		//cache miss
		find = buffer_cache_select_victim();
		
		find->valid_bit = true;
		find->disk_sector = sector;
		block_read(fs_device,sector,find->buffer);		//디스크 접근해서 읽음
		find->dirty_bit = false;
		
	}
	
	memcpy(to,find->buffer,BLOCK_SECTOR_SIZE);	 //  cache에있는 메모리 복사
	find->reference_bit = true;			
	lock_release(&buffer_cache_lock);
}

void buffer_cache_write(block_sector_t sector, void* target){


	lock_acquire(&buffer_cache_lock);
	
	struct buffer_cache_entry* find = buffer_cache_lookup(sector);

	if(find == NULL){		// cache miss 난 경우
		
		find = buffer_cache_select_victim();

		find->valid_bit = true;
		find->disk_sector = sector;
		block_read(fs_device,sector,find->buffer);
	}

	memcpy(find->buffer,target,BLOCK_SECTOR_SIZE);		//cache에 write back
	find->dirty_bit = true;
	find->reference_bit = true;
	
	lock_release(&buffer_cache_lock);

}
struct buffer_cache_entry* buffer_cache_lookup(block_sector_t sector){

	int i;
	for(i=0;i<NUM_CACHE;i++){
		if(cache[i].valid_bit){
			if(cache[i].disk_sector == sector)
				return &(cache[i]);
		}
	}
	return NULL;
}

struct buffer_cache_entry* buffer_cache_select_victim(){
	
	int tick=0;
	while(1){
	
		if(cache[tick].valid_bit == false || cache[tick].reference_bit == false){
			break;
		}
		//second chance
		if(cache[tick].reference_bit){
			cache[tick].reference_bit = false;
		}
		tick = (tick+1)%NUM_CACHE;
	}

	if(cache[tick].dirty_bit){
		buffer_cache_flush_entry(&cache[tick]);
	}
	return &cache[tick];
}

void buffer_cache_flush_entry(struct buffer_cache_entry* e){
	
	//lock???
	if(e->dirty_bit){
		 block_write(fs_device,e->disk_sector,e->buffer);
		 e->dirty_bit = false;
		 e->valid_bit = false;
		 e->reference_bit = false;

	}
}




