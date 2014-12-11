#ifndef TEST_INTERFACE
#define TEST_INTERFACE

#include <sys/types.h>
#include <stdint.h>
#include "airapi.h"
#include <ctype.h>
#include <string>
#include <string.h>

struct header_t {
    uint32_t len;
    uint32_t seq;
    uint32_t cmd;
    uint32_t ret;
    uint32_t uid;
} __attribute__((packed));


extern "C" int handle_init(); 
extern "C" int handle_fini(); 
extern "C" int handle_input(const char* recv_buf, int recv_len, const Skinfo_t* skinfo); 
extern "C" int handle_process(const char* recv_buf, int recv_len, char** send_buf, int* send_len, const Skinfo_t* skinfo);

#endif 
