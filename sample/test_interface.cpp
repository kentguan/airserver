#include "test_interface.h"

extern "C" int handle_init() {
    return 0;
}

extern "C" int handle_input(const char* recv_buf, int recv_len, const Skinfo_t* skinfo) {

    if (recv_len < int(sizeof(header_t))) { //继续接受
        return 0;
    }
    else {
        header_t* header = (header_t *)recv_buf;
        if (header->len < int(sizeof(header_t))) {
            return -1;
        }

        if (header->len > 65536) {
            return -1;
        }

        return header->len;
    }
}

extern "C" int handle_process(const char* recv_buf, int recv_len, char** send_buf, int* send_len, const Skinfo_t* skinfo) {

    header_t* req_header = (header_t*)recv_buf;
    if (recv_len != (int)req_header->len) {
        return -1;
    }

    const char* req_body = (char*)recv_buf + sizeof(*req_header);
    int req_body_len = req_header->len - sizeof(*req_header);

    char* m_send_buf = (char*)malloc(1024);
    *send_buf = m_send_buf;

    header_t* ack_header = (header_t*)m_send_buf;

    ack_header->len = sizeof(*ack_header);
    ack_header->seq = req_header->seq;
    ack_header->cmd = req_header->cmd;
    ack_header->ret = 0;
    ack_header->uid = req_header->uid;

    std::string send_body = "hello world";

    ack_header->len = sizeof(*req_header) + strlen(send_body.c_str()) + 1;
    memcpy(m_send_buf + sizeof(*req_header), send_body.c_str(),strlen(send_body.c_str())+1);

    *send_len = ack_header->len;

    return 0;
}

extern "C" int handle_fini() {
    return 0;
}
