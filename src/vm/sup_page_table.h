#include <hash.h>
#include "threads/thread.h"
#include "filesys/file.h"
#include "vm/frame_table.h"

enum page_status
  {
    EMPTY,
    FROM_FRAME,
    FROM_SWAP_SLOTS,
    FROM_FILESYS
  };

struct sup_page_table_entry
  {
    enum page_status status;

    void *upage;
    void *frame;

    struct hash_elem hash_elem;

    // used by SWAP_SLOTS
    uint32_t swap_index;  

    // used by FILESYS
    struct file *file;
    off_t file_offset;
    size_t read_bytes;
    size_t zero_bytes;
    bool writable;
  };

void spt_init (struct hash *spt);

struct sup_page_table_entry *spt_lookup (struct hash *spt, void *upage);

void spt_get_page_empty (struct hash *spt, void *upage);

void spt_get_page_filesys (struct hash *supt, void *upage, struct file *file, 
                           off_t file_size, bool writable);

void spt_free_page (struct hash *spt, void *upage);                          
void spt_free_page_filesys (struct hash *spt, void *upage, off_t file_size);