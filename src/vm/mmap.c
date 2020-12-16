#include "vm/mmap.h"
#include "threads/malloc.h"
#include "vm/sup_page_table.h"

struct mmap *
get_mmap_by_mapid(struct thread *t, int mapid)
{
  struct list *mmap_list = &t->mmap_list;
  for (struct list_elem *e = list_begin(mmap_list); e != list_end(mmap_list); e = list_next(e))
    {
      struct mmap *mmap = list_entry(e, struct mmap, elem);
      if (mmap->mapid == mapid)
        return mmap;
    }
  return NULL;
}

int
mmap_open (struct thread *t, struct file *file, off_t file_size, void *upage)
{
  if (spt_get_page_filesys (&t->spt, upage, file, file_size, true) == false)
    return -1;
  struct mmap *mmap = malloc (sizeof (struct mmap));
  mmap->mapid = t->mapid++;
  mmap->file = file;
  mmap->file_size = file_size;
  mmap->upage = upage;
  list_push_back (&t->mmap_list, &mmap->elem);
  return mmap->mapid;
}

void
mmap_close (struct thread *t, struct mmap *mmap)
{
  spt_free_page_filesys (&t->spt, mmap->upage, mmap->file_size);                   
  list_remove (&mmap->elem);
  free(mmap->file);
  free (mmap); 
}

void
close_all_mmap (struct thread *t)
{
  struct list *mmaps = &t->mmap_list;
  while (!list_empty (mmaps))
    {
      struct list_elem *e = list_pop_front (mmaps);
      struct mmap *mmap = list_entry(e, struct mmap, elem);
      mmap_close (t, mmap);
    }
}

