# VM
pintos实验三的文档。

# page fault
* 检测是否是权限错误
    * 如果是，比如写只读页，直接`exit(-1)`

    * 检测`fault_addr`是否合法
        * 如果不合法，交给`kill`，正常报错
        
            不合法情况有这些
            * NULL
            
            * kernel address

            * below user stack (< 0x08048000)

            * 没有沿着栈生长 
            ```c
            static inline bool
            is_valid_user_vaddr (const void *vaddr) {
                return vaddr && is_user_vaddr (vaddr) && vaddr >= (void *) 0x08048000;
            }
            ```
        * 查找virtual memory是否曾经分配过frame
            * 如果是，reclaim it
                ```c
                struct sur_page_table_entry *spte = lookup_frame (cur->spt, fault_addr);
                if (spte != NULL) {
                    // reclamation
                    // can from swap, mmap, ...
                }
                ```
            * 查找有无空闲帧
                * 如果有，get page and install it
                ```c
                struct frame_table_entry *fte = get_frame ();
                if (fte != NULL) {
                    // install frame
                }
                ```
                * 如果没有，evict a frame and re-use it
                ```c
                // eviction
                ```

# second-change page-replacement algorithm