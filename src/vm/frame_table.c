#include "vm/frame_table.h"
#include "threads/malloc.h"
#include "threads/palloc.h"
#include "userprog/pagedir.h"

// static struct list frame_table;

struct frame_table_entry
  {
    void *frame; //
    void *upage;
    struct thread *owner;
    struct list_elem elem;
  };

// static struct list_elem *clock_ptr;

// static struct frame_table_entry *
// clock_algo_next ()
// {
//   if (clock_ptr == NULL || clock_ptr == list_end (&frame_table))
//     clock_ptr = list_begin (&frame_table);
//   else
//     clock_ptr = list_next (clock_ptr);
//   return list_entry (clock_ptr, struct frame_table_entry, elem);
// }

// static struct frame_table_entry *
// evict ()
// {
//   size_t n = list_size (&frame_table);
//   uint32_t *pagedir = thread_current ()->pagedir;
//   for (size_t i = 0; i < n + n; i++)
//     {
//       struct frame_table_entry *e = clock_algo_next ();
//       if (pagedir_is_accessed (pagedir, e->upage))
//         {
//           pagedir_set_accessed (pagedir, e->upage, false);
//           continue;
//         }
//     }
// }

void *
ft_get_frame (void)
{
  void *frame = palloc_get_page (PAL_USER | PAL_ZERO);
  if (frame == NULL)
    {
      // struct frame_table_entry *fte = evict ();
      PANIC ("TODO: swap slots");
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