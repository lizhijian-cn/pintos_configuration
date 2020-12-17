#include <bitmap.h>
#include "vm/swap.h"
#include "devices/block.h"
#include "threads/vaddr.h"

#define BLOCK_CNT_PER_PAGE (PGSIZE / BLOCK_SECTOR_SIZE)

static struct block *swap_block;
static struct bitmap *swap_bitmap;

void
swap_init (void)
{
  swap_block = block_get_role(BLOCK_SWAP);
  swap_bitmap = bitmap_create (block_size (swap_block) / BLOCK_CNT_PER_PAGE);
  if (swap_block == NULL || swap_bitmap == NULL)
    PANIC ("swap init failed");
}

static void
read_from_block (void *frame, uint32_t index)
{
  for (int i = 0; i < BLOCK_CNT_PER_PAGE; i++)
    block_read (swap_block, 
                index * BLOCK_CNT_PER_PAGE + i, 
                frame + (i * BLOCK_SECTOR_SIZE));
}

static void
write_to_block (void *frame, uint32_t index)
{
  for (int i = 0; i < BLOCK_CNT_PER_PAGE; i++)
    block_write (swap_block, 
                 index * BLOCK_CNT_PER_PAGE + i, 
                 frame + (i * BLOCK_SECTOR_SIZE));
}

uint32_t
swap_to_block (void *frame)
{
  uint32_t index = bitmap_scan_and_flip (swap_bitmap, 0, 1, false);
  if (index == BITMAP_ERROR)
    PANIC ("no slot");
  
  write_to_block (frame, index);
  return index;
}

void
swap_from_block (void *frame, uint32_t index)
{
  read_from_block (frame, index);
}