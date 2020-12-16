#include "vm/sup_page_table.h"
#include "threads/vaddr.h"
#include "threads/malloc.h"
#include "userprog/pagedir.h"

struct hash sup_page_table;


static unsigned
spte_hash (const struct hash_elem *e, void *aux UNUSED)
{
  struct sup_page_table_entry *spte = hash_entry (e, struct sup_page_table_entry, hash_elem);
  return hash_bytes (&spte->upage, sizeof (spte->upage));
}

static bool
spte_less (const struct hash_elem *a_, const struct hash_elem *b_, void *aux UNUSED)
{
  struct sup_page_table_entry *a = hash_entry (a_, struct sup_page_table_entry, hash_elem);
  struct sup_page_table_entry *b = hash_entry (b_, struct sup_page_table_entry, hash_elem);

  return a->upage < b->upage;
}

void
spt_init (struct hash *spt)
{
  hash_init (spt, spte_hash, spte_less, NULL);
}

struct sup_page_table_entry *
spt_lookup (struct hash *spt, void *upage)
{
  struct sup_page_table_entry spte;
  spte.upage = upage;
  struct hash_elem *e = hash_find (spt, &spte.hash_elem);
  return e != NULL ? hash_entry (e, struct sup_page_table_entry, hash_elem) : NULL;
}

void
spt_free_page (struct hash *spt, void *upage)
{
  struct sup_page_table_entry x;
  x.upage = upage;
  struct hash_elem *e = hash_delete (spt, &x.hash_elem);
  if (e == NULL)
  PANIC ("spt_free_page(): no spte for upage");
  
  struct sup_page_table_entry *spte = hash_entry (e, struct sup_page_table_entry, hash_elem);
  if (spte->frame == NULL)
    {
      free (spte);
      return;
    }
  switch (spte->status)
    {
      case FROM_FILESYS:
        if (pagedir_is_dirty (thread_current ()->pagedir, upage))
          file_write_at (spte->file, spte->frame, spte->read_bytes, spte->file_offset);
        break;
      default:
        break;
    }
    ft_free_frame (spte->frame);
    pagedir_clear_page (thread_current ()->pagedir, upage);
    free (spte);
}

void
spt_free_page_filesys (struct hash *spt, void *upage, off_t file_size)
{
  for (off_t offset = 0; offset < file_size; offset += PGSIZE)
    spt_free_page (spt, upage + offset);
}

void
spt_get_page_empty (struct hash *spt, void *upage)
{
  struct sup_page_table_entry *spte = malloc (sizeof (struct sup_page_table_entry));

  spte->status = EMPTY;

  spte->upage = upage;
  spte->frame = NULL;

  if (hash_insert (spt, &spte->hash_elem) != NULL)
    PANIC ("1");
}

bool
spt_get_page_filesys (struct hash *spt, void *upage, struct file *file, 
                      off_t file_size, bool writable)
{
  for (off_t offset = 0; offset < file_size; offset += PGSIZE)
    {
      size_t page_read_bytes = offset + PGSIZE < file_size ? PGSIZE : file_size - offset;
      size_t page_zero_bytes = PGSIZE - page_read_bytes;

      struct sup_page_table_entry *spte = malloc (sizeof (struct sup_page_table_entry));

      spte->status = FROM_FILESYS;

      spte->upage = upage + offset;
      spte->frame = NULL; // lazy load

      spte->file = file;
      spte->file_offset = offset;
      spte->read_bytes = page_read_bytes;
      spte->zero_bytes = page_zero_bytes;
      spte->writable = writable;

      if (hash_insert (spt, &spte->hash_elem) != NULL)
        return false;
    }
  return true;
}

// void
// spt_free_page_filesys (struct hash *spt, uint32_t *pagedir, void *upage, 
//                        struct file *file, off_t file_size)
// {

// }                    
// , off_t file_offset, size_t read_bytes, size_t zero_bytes, bool writable