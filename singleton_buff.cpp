#include "singleton_buff.hpp"

inline uint32_t len2page_size(uint32_t length) {
    uint32_t n = (length/PAGE_SIZE) + (length%PAGE_SIZE?1:0);
    return n;
}

BufBlock_t* alloc_block(uint32_t length) {

    BufBlock_t* block = (BufBlock_t*)BufBlockPool::malloc();
    if (NULL == block) {
        return NULL;
    }

    uint32_t n = len2page_size(length);
    if (0 == n) {
        block->page_base = NULL;
    }
    else {
        block->page_base = (uint8_t*) BufPagePool::ordered_malloc(n);
        if (NULL == block->page_base) {
            BufBlockPool::free(block);
            return NULL;
        }
    }

    block->page_num = n;

    return block;
}

void free_block(BufBlock_t* block) {

    if (0 == block->page_num) {
        BufBlockPool::free(block);
    }
    else {
        BufPagePool::ordered_free(block->page_base, block->page_num);
        BufBlockPool::free(block);
    }
}


