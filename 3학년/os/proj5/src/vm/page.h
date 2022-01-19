#define VM_BIN 0
#define VM_FILE 1
#define VM_ANON 2

/* virual page */

#include "filesys/file.h"
#include <hash.h>

struct page{

	uint8_t type;
	void* addr;
	bool writable;

	bool is_load;

	struct file* file;

	struct list_elem mmap_elem;

	size_t offset;
	size_t read_bytes;
	size_t zero_bytes;

	size_t swap;

	struct hash_elem elem;

};
static unsigned page_hash_func(const struct hash_elem*,void*);
static bool page_less_func(const struct hash_elem*, const struct hash_elem*);
void page_init(struct hash*);
bool insert_page(struct hash*,struct page*);
void delete_page(struct hash*,struct page*);
struct page* find_page(void*);
void page_destroy_func(struct hash_elem*,void*);
void page_destroy(struct hash*);





