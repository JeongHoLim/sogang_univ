#include "page.h"
#include "threads/palloc.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
static unsigned page_hash_func(const struct hash_elem* e,void* aux){

	struct page* p = hash_entry(e,struct page,elem);
	
	return hash_int(p->addr);

}
static bool page_less_func(const struct hash_elem* a,const struct hash_elem* b){

	struct page* p1 = hash_entry(a,struct page,elem);
	struct page* p2 = hash_entry(b,struct page,elem);

	return p1->addr < p2->addr;
	
}
void page_init(struct hash* pg){

	hash_init(pg,page_hash_func,page_less_func,NULL);

}

/* 삽입 성공시 true 반환 */

bool insert_page(struct hash* pg,struct page* new){
	
	return hash_insert(pg,&new->elem) == NULL;

}
void delete_page(struct hash* pg,struct page* del){

	hash_delete(pg,&del->elem);
}
struct page* find_page(void* vaddr){
	
	void* target = pg_round_down(vaddr);
	struct page tmp;
	tmp.addr = target;
	struct hash_elem* e = hash_find(&(thread_current()->supple_pt),&tmp.elem);

	if(e == NULL)
		return NULL;
	else{
		return hash_entry(e,struct page,elem);
	}
}
void page_destroy_func(struct hash_elem* e,void* aux){
	

	struct page* p = hash_entry(e,struct page,elem);
	if(p->load){
		uint8_t* kpage = palloc_get_page(PAL_USER);
		palloc_free_page(kpage);
		pagedir_clear_page(thread_current()->pagedir,p->addr);
	}
	free(p);

}
void page_destroy(struct hash* pg){
	
	hash_destroy(pg,page_destroy_func);

}
