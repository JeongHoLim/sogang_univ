#include "filesys/directory.h"
#include <stdio.h>
#include <string.h>
#include <list.h>
#include "filesys/filesys.h"
#include "filesys/inode.h"
#include "threads/malloc.h"

#include "threads/thread.h"

/* A directory. */
struct dir 
  {
    struct inode *inode;                /* Backing store. */
    off_t pos;                          /* Current position. */
  };

/* A single directory entry. */
struct dir_entry 
  {
    block_sector_t inode_sector;        /* Sector number of header. */
    char name[NAME_MAX + 1];            /* Null terminated file name. */
    bool in_use;                        /* In use or free? */
  };

/* Creates a directory with space for ENTRY_CNT entries in the
   given SECTOR.  Returns true if successful, false on failure. */
bool
dir_create (block_sector_t sector, size_t entry_cnt)
{
	bool ret = true;
	struct dir_entry e;

	if(!inode_create (sector, entry_cnt * sizeof (struct dir_entry),true))
		ret = false;
	else{
		struct dir* dir = dir_open(inode_open(sector));
		e.inode_sector = sector;

		if(inode_write_at(dir->inode,&e,sizeof(struct dir_entry),0) != sizeof(struct dir_entry))
			ret = false;
		dir_close(dir);
	}
	return ret;

}

/* Opens and returns the directory for the given INODE, of which
   it takes ownership.  Returns a null pointer on failure. */
struct dir *
dir_open (struct inode *inode) 
{
  struct dir *dir = calloc (1, sizeof *dir);
  if (inode != NULL && dir != NULL)
    {
      dir->inode = inode;
      //dir->pos = 0;
      dir->pos = sizeof(struct dir_entry);
	  return dir;
    }
  else
    {
      inode_close (inode);
      free (dir);
      return NULL; 
    }
}

/* Opens the root directory and returns a directory for it.
   Return true if successful, false on failure. */
struct dir *
dir_open_root (void)
{
  return dir_open (inode_open (ROOT_DIR_SECTOR));
}
struct dir*
dir_open_path(const char* path){


	int length = strlen(path)+1;
	char str[length];

	strlcpy(str,path,length);

	struct dir* ret;
	
	struct thread* t = thread_current();
	if(path[0] == '/' || t->cwd == NULL)			//?????? ????????? ??????
		ret = dir_open_root();
	else{						//?????? ????????? ??????
		ret = dir_reopen(t->cwd);
	}

	char* token, *ptr;
	struct inode* inode = NULL;
	struct dir* next_dir;
	token = strtok_r(str,"/",&ptr);
	while(token != NULL){
		if(dir_lookup(ret,token,&inode) == NULL){		// ????????? ??????
			break;
		}
		else{
			next_dir = dir_open(inode);
			if(next_dir == NULL){
				break;
			}
		}
		dir_close(ret);
		ret = next_dir;
		token = strtok_r(NULL,"/",&ptr);

	}
	if(token || inode_is_removed(dir_get_inode(ret))){
		dir_close(ret);
		ret = NULL;
	}

	return ret;
}

void
split_path_filename(const char *path,
    char *directory, char *filename)
{
  int length = strlen(path)+1;
  char *str = (char*) malloc( sizeof(char) * length );
  memcpy (str, path, sizeof(char) * length);

  //memcpy?

  char *dir = directory;
  if(length > 1 && path[0] == '/') {
		if(dir) *dir++ = '/';
  }


  char *token, *ptr,*prev ="";

  token = strtok_r(str, "/", &ptr);
  int token_length;
  while (token != NULL) {
	  token_length = strlen(prev);
	  if (dir && token_length > 0) {
		  memcpy(dir, prev, sizeof(char) * token_length);
		  dir[token_length] = '/';
		  dir += token_length + 1;
	  }
	  prev = token;

	  token = strtok_r(NULL, "/", &ptr);
  }

  if(dir) *dir = '\0';
  memcpy (filename, prev, sizeof(char) * (strlen(prev) + 1));
  free (str);

}

/* Opens and returns a new directory for the same inode as DIR.
   Returns a null pointer on failure. */
struct dir *
dir_reopen (struct dir *dir) 
{
  return dir_open (inode_reopen (dir->inode));
}

/* Destroys DIR and frees associated resources. */
void
dir_close (struct dir *dir) 
{
  if (dir != NULL)
    {
      inode_close (dir->inode);
      free (dir);
    }
}

/* Returns the inode encapsulated by DIR. */
struct inode *
dir_get_inode (struct dir *dir) 
{
  return dir->inode;
}

/* Searches DIR for a file with the given NAME.
   If successful, returns true, sets *EP to the directory entry
   if EP is non-null, and sets *OFSP to the byte offset of the
   directory entry if OFSP is non-null.
   otherwise, returns false and ignores EP and OFSP. */
static bool
lookup (const struct dir *dir, const char *name,
        struct dir_entry *ep, off_t *ofsp) 
{
  struct dir_entry e;
  size_t ofs;
  
  ASSERT (dir != NULL);
  ASSERT (name != NULL);

  for (ofs = sizeof(e); inode_read_at (dir->inode, &e, sizeof e, ofs) == sizeof e;
       ofs += sizeof e) 
    if (e.in_use && !strcmp (name, e.name)) 
      {
        if (ep != NULL)
          *ep = e;
        if (ofsp != NULL)
          *ofsp = ofs;
        return true;
      }
  return false;
}

/* Searches DIR for a file with the given NAME
   and returns true if one exists, false otherwise.
   On success, sets *INODE to an inode for the file, otherwise to
   a null pointer.  The caller must close *INODE. */
bool
dir_lookup (const struct dir *dir, const char *name,
            struct inode **inode) 
{
  struct dir_entry e;
	//static int count = 0;
  ASSERT (dir != NULL);
  ASSERT (name != NULL);
  *inode = NULL;
  if(!strcmp(name,"."))
	  *inode = inode_reopen(dir->inode);
  else if(!strcmp(name,"..")){
		inode_read_at(dir->inode,&e,sizeof e,0);
		*inode = inode_open(e.inode_sector);
   }
  else if (lookup (dir, name, &e, NULL)){
    *inode = inode_open (e.inode_sector);
  }
  else
	  *inode = NULL;
  return *inode != NULL;
}

/* Adds a file named NAME to DIR, which must not already contain a
   file by that name.  The file's inode is in sector
   INODE_SECTOR.
   Returns true if successful, false on failure.
   Fails if NAME is invalid (i.e. too long) or a disk or memory
   error occurs. */
bool
dir_add (struct dir *dir, const char *name, block_sector_t inode_sector,bool is_dir)
{
  struct dir_entry e;
  off_t ofs;
  bool success = false;

  ASSERT (dir != NULL);
  ASSERT (name != NULL);

  /* Check NAME for validity. */
  if (*name == '\0' || strlen (name) > NAME_MAX)
    return false;

  /* Check that NAME is not in use. */
  if (lookup (dir, name, NULL, NULL))
    goto done;

   if(is_dir){

		struct dir* child = dir_open(inode_open(inode_sector));
		if(child == NULL)
			goto done;
		e.inode_sector = inode_get_inumber(dir_get_inode(dir));
		if(inode_write_at(child->inode,&e,sizeof e, 0) != sizeof e){
			dir_close(child);
			goto done;
		}
		else dir_close(child);
   }

  /* Set OFS to offset of free slot.
     If there are no free slots, then it will be set to the
     current end-of-file.
     
     inode_read_at() will only return a short read at end of file.
     Otherwise, we'd need to verify that we didn't get a short
     read due to something intermittent such as low memory. */
  for (ofs = 0; inode_read_at (dir->inode, &e, sizeof e, ofs) == sizeof e;
       ofs += sizeof e) 
    if (!e.in_use)
      break;

  /* Write slot. */
  e.in_use = true;
  strlcpy (e.name, name, sizeof e.name);
  e.inode_sector = inode_sector;
  success = inode_write_at (dir->inode, &e, sizeof e, ofs) == sizeof e;

 done:
  return success;
}

/* Removes any entry for NAME in DIR.
   Returns true if successful, false on failure,
   which occurs only if there is no file with the given NAME. */
bool
dir_remove (struct dir *dir, const char *name) 
{
  struct dir_entry e;
  struct inode *inode = NULL;
  bool success = false;
  off_t ofs;

  ASSERT (dir != NULL);
  ASSERT (name != NULL);

  /* Find directory entry. */
  if (!lookup (dir, name, &e, &ofs))
    goto done;

  /* Open inode. */
  inode = inode_open (e.inode_sector);
  if (inode == NULL)
    goto done;

  if(inode_is_directory(inode)){
	  struct dir* target = dir_open(inode);
	  bool flg = true;
	  struct dir_entry e;
	  for(off_t ofs = sizeof e; inode_read_at(target->inode,&e,sizeof e,ofs) == sizeof e; ofs += sizeof e){
		  if(e.in_use){ 
			  flg = false;
			  break;
		  }
	  }
	  dir_close(target);
	  if(!flg) goto done;
  }



  /* Erase directory entry. */
  e.in_use = false;
  if (inode_write_at (dir->inode, &e, sizeof e, ofs) != sizeof e) 
    goto done;

  /* Remove inode. */
  inode_remove (inode);
  success = true;

 done:
  inode_close (inode);
  return success;
}

/* Reads the next directory entry in DIR and stores the name in
   NAME.  Returns true if successful, false if the directory
   contains no more entries. */
bool
dir_readdir (struct dir *dir, char name[NAME_MAX + 1])
{
  struct dir_entry e;

  while (inode_read_at (dir->inode, &e, sizeof e, dir->pos) == sizeof e) 
    {
      dir->pos += sizeof e;
      if (e.in_use)
        {
          strlcpy (name, e.name, NAME_MAX + 1);
          return true;
        } 
    }
  return false;
}
