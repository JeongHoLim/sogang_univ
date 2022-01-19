#include "devices/block.h"
#include "filesys.h"

void buffer_cache_init(void);
void buffer_cache_terminate(void);
void buffer_cache_read(block_sector_t,void*);

void buffer_cache_write(block_sector_t,void*);
struct buffer_cache_entry* buffer_cache_lookup(block_sector_t);

struct buffer_cache_entry* buffer_cache_select_victim(void);

void buffer_cache_flush_entry(struct buffer_cache_entry*);

