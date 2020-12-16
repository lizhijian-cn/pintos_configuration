#include <bitmap.h>
#include "vm/swap.h"
#include "devices/block.h"
#include "threads/vaddr.h"

#define BLOCK_CNT_PER_PAGE (PGSIZE / BLOCK_SECTOR_SIZE)

static struct block *swap_block;
static struct bitmap *swap_available;

void
swap_init (void)
{
  swap_block = block_get_role(BLOCK_SWAP);
}

void
read_from_block (void *frame, uint32_t index)
{
  for (int i = 0; i < BLOCK_CNT_PER_PAGE; i++)
    block_read (swap_block, 
                index * BLOCK_CNT_PER_PAGE + i, 
                frame + (i * BLOCK_SECTOR_SIZE));
}

void
write_from_block (void *frame, uint32_t index)
{
  for (int i = 0; i < BLOCK_CNT_PER_PAGE; i++)
    block_write (swap_block, 
                 index * BLOCK_CNT_PER_PAGE + i, 
                 frame + (i * BLOCK_SECTOR_SIZE));
}

uint32_t
swap_write (void *frame)
{

}