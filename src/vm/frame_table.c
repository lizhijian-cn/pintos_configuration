#include "vm/frame_table.h"
#include "threads/malloc.h"
#include "threads/palloc.h"
#include "userprog/pagedir.h"
#include "vm/sup_page_table.h"
#include "vm/swap.h"

static struct list frame_table;

struct frame_table_entry
  {
    void *frame; //
    void *upage;
    struct thread *owner;
    struct list_elem elem;
  };

static struct list_elem *clock_ptr;

static struct frame_table_entry *
clock_algo_next (void)
{
  if (clock_ptr == NULL || clock_ptr == list_end (&frame_table))
    clock_ptr = list_begin (&frame_table);
  else
    clock_ptr = list_next (clock_ptr);
  return list_entry (clock_ptr, struct frame_table_entry, elem);
}

static struct frame_table_entry *
evict (void)
{
  size_t n = list_size (&frame_table);
  uint32_t *pagedir = thread_current ()->pagedir;
  for (size_t i = 0; i < n + n; i++)
    {
      struct frame_table_entry *e = clock_algo_next ();
      if (pagedir_is_accessed (pagedir, e->upage))
        {
          pagedir_set_accessed (pagedir, e->upage, false);
          continue;
        }
      return e;
    }
  PANIC ("no frame to evict");
}

void
ft_init (void)
{
  list_init (&frame_table);
}

void *
ft_get_frame (void)
{
  void *frame = palloc_get_page (PAL_USER | PAL_ZERO);
  if (frame == NULL)
    {
      struct frame_table_entry *fte = evict ();
      struct thread *t = fte->owner;
      void *upage = fte->upage;
      struct sup_page_table_entry *spte = spt_lookup (&t->spt, upage);
      
      spte->status = FROM_SWAP_SLOTS;
      spte->swap_index = swap_to_block (fte->frame);

      pagedir_clear_page (t->pagedir, upage);
      frame = fte->frame;
    }
  return frame;
}

void
ft_free_frame (void *frame)
{
  palloc_free_page (frame);
}
// struct frame_table_entry *
// get_frame ()
// {
//   struct frame_table_entry *fte = malloc (sizeof (struct frame_table_entry));
//   void *kpage = palloc_get_page (PAL_USER | PAL_ZERO);
//   fte->frame = kpage;
//   return fte;
// }

// void
// free_frame (struct frame_table_entry *fte)
// {
//   palloc_free_page (fte->frame);
//   free (fte);
// }