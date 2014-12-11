
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

struct header_t {
    uint32_t len;
    uint32_t seq;                                                                                                               
    uint32_t cmd;
    uint32_t ret;
    uint32_t uid;
} __attribute__((packed));


static const char* request = "GET http://localhost";

int setnonblocking(int fd) {
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);

    return old_option;
}

void addfd(int epoll_fd, int fd) {
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLOUT | EPOLLET | EPOLLERR;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);

    setnonblocking(fd);
}


bool write_nbytes(int sockfd, const char* buffer, int len) {
    
    printf("write out %d bytes to socket %d\n",len, sockfd);
    int bytes_write = 0;

    while (1) {

        bytes_write = send(sockfd, buffer, len , 0);
        if (bytes_write == -1) {
            return false;
        }
        else if (bytes_write == 0) {
            return false;
        }

        len -= bytes_write;

        if (len <= 0) {
            return true;
        }

    }
}

bool read_once(int socketfd, char* buffer, int len) {

    memset(buffer, '\0', len);
    int bytes_read = recv(socketfd, buffer, len, 0);
    

    if (bytes_read <= 0) {
        return false;
    }

    printf("read in %d bytes from socket %d with content:%s\n", bytes_read, socketfd, buffer);
    return true;
}

void start_conn(int epoll_fd, int num, const char*ip, int port) {

    int ret = 0;
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    for (int i = 0; i < num; i++) {
        sleep(1);
        int sockfd = socket(PF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            continue;
        }

        if (connect(sockfd, (struct sockaddr*)&address, sizeof(address)) == 0) {
            printf("build connection %d\n", i);
            addfd(epoll_fd, sockfd);
        }
    }
}

void close_conn(int epoll_fd, int sockfd) {
    printf("close fd %d \n", sockfd);
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, sockfd, 0);
    close(sockfd);
}



int main (int argc, char* argv[]) {
    assert(argc == 4);

    int epoll_fd = epoll_create(10000);
    start_conn(epoll_fd, atoi(argv[3]), argv[1], atoi(argv[2]));
    epoll_event events[10000];

    char buffer[2048];
    char send_buffer[2048];

    while (1) {

        int fds = epoll_wait(epoll_fd, events, 10000, 2000);
        for (int i = 0; i < fds; i++) {

            int sockfd = events[i].data.fd;

            if (events[i].events & EPOLLIN) {

                if (!read_once(sockfd, buffer, 2048)) {
                    close_conn(epoll_fd, sockfd);
                }

                struct epoll_event event;
                event.events = EPOLLOUT|EPOLLET|EPOLLERR;
                event.data.fd = sockfd;

                epoll_ctl(epoll_fd, EPOLL_CTL_MOD, sockfd, &event);
            }
            else if (events[i].events & EPOLLOUT) {

                header_t* req_header = (header_t*)send_buffer;

                req_header->len = sizeof(*req_header);
                req_header->seq = 1 ;
                req_header->cmd = 1;
                req_header->ret = 0;
                req_header->uid = 111;
                req_header->len = sizeof(*req_header) + strlen(request) + 1;


                if (!write_nbytes(sockfd, send_buffer, req_header->len)) {
                    close_conn(epoll_fd, sockfd);
                }

                struct epoll_event event;
                event.events = EPOLLIN|EPOLLET|EPOLLERR;
                event.data.fd = sockfd;

                epoll_ctl(epoll_fd, EPOLL_CTL_MOD, sockfd, &event);
            }
            else if (events[i].events & EPOLLOUT) {
                close_conn(epoll_fd, sockfd);
            }
        }

    }

    return 0;
}
