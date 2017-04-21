#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <pthread.h>

#define MAX_EVENT_NUMBER 1024
#define BUFFER_SIZE 10

int setnonblock(int fd) {
    int old_flags = fcntl(fd, F_GETFL);
    int new_flags = old_flags | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_flags);
    return old_flags;
}

int addfd(int epfd, int fd, bool is_et) {
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;
    if (is_et) {
        event.events |= EPOLLET;
    }
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
    setnonblock(fd);
}

int lt(epoll_event* events, int num, int listenfd, int epfd) {
    char buff[BUFFER_SIZE] = {0};

    for(int i = 0; i < num; ++i) {
        int sockfd = events[i].data.fd;
        if (sockfd == listenfd) {
            struct sockaddr_in addr;
            socklen_t len = sizeof(addr);
            int connfd = accept(listenfd, (struct sockaddr*)&addr, &len);
            addfd(epfd, connfd, false);
        } else if (events[i].events & EPOLLIN){
            memset(buff, 0, BUFFER_SIZE);
            printf("epoll triger once\n");
            int ret = recv(sockfd, buff, BUFFER_SIZE-1, 0);
            if (ret <= 0) {
                close(sockfd);
                continue;
            }
            printf("get %d bytes from socket: %s\n", ret, buff);
        } else {
            printf("something else happen\n");
        }
    }
}

int et(epoll_event* events, int num, int listenfd, int epfd) {
    char buff[BUFFER_SIZE] = {0};
    
    for(int i = 0; i < num; ++i) {
        int sockfd = events[i].data.fd;
        if (sockfd == listenfd) {
            sockaddr_in addr;
            socklen_t len = sizeof(addr);
            int connfd = accept(listenfd, (struct sockaddr*)&addr, &len);
            addfd(epfd, connfd, true);
        } else if (events[i].events & EPOLLIN) {
            printf("epoll triger once\n");
            while(1) {
                memset(buff, 0, BUFFER_SIZE);
                int ret = recv(sockfd, buff, BUFFER_SIZE-1, 0);
                if (ret < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        printf("read later\n");
                        break;
                    }
                    close(sockfd);
                    break;
                } else if (ret == 0) {
                    close(sockfd);
                } else {
                    printf("get %d bytes from socket: %s\n", ret, buff);
                    continue;
                }
            }
        } else {
            printf("something else happen\n");
        }
    }
}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("%s ip port\n", basename(argv[0]));
        return -1;
    }

    const char* ip = argv[1];
    int port = atoi(argv[2]);

    int ret = 0;
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &addr.sin_addr);

    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(listenfd > 0);

    ret = bind(listenfd, (struct sockaddr*)&addr, sizeof(addr));
    assert(ret == 0);

    ret = listen(listenfd, 5);
    assert(ret == 0);
    
    int epfd = epoll_create(1);
    assert(epfd > 0);

    addfd(epfd, listenfd, true);

    epoll_event events[MAX_EVENT_NUMBER];
    while(1) {
        ret = epoll_wait(epfd, events, MAX_EVENT_NUMBER, -1);
        if (ret < 0) {
            printf("epoll_wait failed.\n");
            break;
        }

        //lt(events, ret, listenfd, epfd);
        et(events, ret, listenfd, epfd);
    }

    close(epfd);
    return 0;
}


