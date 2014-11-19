#ifndef AIRAPI_H
#define AIRAPI_H

#include <stdlib.h>
#include <sys/types.h>
#include <stdint.h>

struct Skinfo_t {
    int socket;
    time_t recvtm;
    time_t sendtm;
    uint32_t local_ip;
    uint16_t local_port;
    uint32_t remote_ip;
    uint16_t remote_port;
};


typedef struct dll_func_struct {
    void  *handle;
    int (*handle_input) (const char*, int, const Skinfo_t*);
    int (*handle_process) (const char*, int, char**, int*, const Skinfo_t*);
} dll_func_t;


#endif
