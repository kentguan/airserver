#ifndef BUFF_HPP
#define BUFF_HPP

#include <stdint.h>
#include <boost/pool/pool.hpp>
#include <sys/types.h>

#include "airapi.h"

#define PAGE_SIZE 2048

#define DATA_BLOCK    0x01                                                                                                      
#define CRASH_BLOCK   0x02
#define FIN_BLOCK     0x04
#define CLEAR_BLOCK   0x08

struct BufHead_t {
    int id;
    uint32_t buf_type;
    uint32_t len;
    uint32_t key;
    Skinfo_t sk;
}__attribute__((packed));

struct BufBlock_t {
    BufHead_t buf_head;
    uint8_t *page_base;
    uint32_t page_num;
};


class BufPool {
public:
    BufPool();
    ~BufPool();

public:
    static BufBlock_t *alloc(uint32_t length);
    static void free(BufBlock_t *block);

    inline uint32_t len2page_size(uint32_t length) {
        uint32_t n = (length/PAGE_SIZE) + (length%PAGE_SIZE?1:0);
        return n;
    }

private:
    static BufPool m_singleton;
    boost::pool<boost::default_user_allocator_malloc_free> m_head_pool;
    boost::pool<boost::default_user_allocator_malloc_free> m_page_pool;
};

#endif
