#include "buff.hpp"

BufPool BufPool::m_singleton;

BufPool::BufPool()
    :m_head_pool(sizeof(BufBlock_t)), m_page_pool(PAGE_SIZE)
{
}

BufPool::~BufPool()
{
}

BufBlock_t* BufPool::alloc(uint32_t length) {

    BufBlock_t* block = (BufBlock_t*)m_singleton.m_head_pool.malloc();
    if (NULL == block) {
        return NULL;
    }

    uint32_t n = m_singleton.len2page_size(length);
    if (0 == n) {
        block->page_base = NULL;
    }
    else {
        block->page_base = (uint8_t*) m_singleton.m_page_pool.ordered_malloc(n);
        if (NULL == block->page_base) {
            m_singleton.m_head_pool.free(block);
        }
    }

    block->page_num = n;

    return block;
}

void BufPool::free(BufBlock_t* block) {

    if (0 == block->page_num) {
        m_singleton.m_head_pool.free(block);
    }
    else {
        m_singleton.m_page_pool.ordered_free(block->page_base, block->page_num);
        m_singleton.m_head_pool.free(block);
    }
}
