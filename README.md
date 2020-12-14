# VM
pintos实验三的文档。

```c

void *kpage = palloc_get_page(PAL_USER); // get a frame
pagedir_set_page(cur->pagedir, upage, kpage); // map user page to frame
```