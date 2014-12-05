#ifndef SINGLETON_BUFF_HPP
#define SINGLETON_BUFF_HPP

#include <stdint.h>
#include <boost/pool/singleton_pool.hpp>
#include <sys/types.h>

#include "airapi.h"

#define PAGE_SIZE 2048

#define DATA_BLOCK    0x01                                                                                                      
#define CRASH_BLOCK   0x02
#define FIN_BLOCK     0x04
#define CLEAR_BLOCK   0x08

struct Page_t {
};

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

typedef boost::singleton_pool<BufBlock_t, sizeof(BufBlock_t)> BufBlockPool;
typedef boost::singleton_pool<Page_t, PAGE_SIZE> BufPagePool;

inline uint32_t len2page_size(uint32_t length); 

BufBlock_t* alloc_block(uint32_t length);

void free_block(BufBlock_t* block);
#endif 
