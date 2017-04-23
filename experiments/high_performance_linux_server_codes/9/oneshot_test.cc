#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>

#define BUFFER_SIZE 1024
#define MAX_EVENTS_SIZE 1024

struct fds {
    int epfd;
    int connfd;
};

int setnonblock(int fd) {
    int old_flags = fcntl(fd, F_GETFL);
    int new_flags = old_flags | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_flags);
    return old_flags;
}

void addfd(int epfd, int fd, bool is_oneshot) {
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    if (is_oneshot) {
        event.events |= EPOLLONESHOT;
    }
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
    setnonblock(fd);
}

void reset_oneshot(int epfd, int fd) {
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN |EPOLLET |EPOLLONESHOT;
    epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &event);
}

void* worker(void* arg) {
    fds* worker_fds = (fds*)arg;
    int epfd = worker_fds->epfd;
    int connfd = worker_fds->connfd;
    printf("start new thread to receive data from socket\n");

    char buff[BUFFER_SIZE] = {0};
    while(1) {
        int ret = recv(connfd, buff, BUFFER_SIZE, 0);
        if (ret == 0) {
            close(connfd);
            break;
        } else if (ret < 0) {
            if (errno == EAGAIN) {
                reset_oneshot(epfd, connfd);
                printf("read later\n");
                break;
            }
        } else {
            printf("get %d bytes from socket: %s\n", ret, buff);
        }
    }
}

int main(int argc, char** argv) {
    if (argc <= 2) {
        printf("usage: %s ip port\n", basename(argv[0]));
        return -1;
    }

    int ret = 0;
    const char* ip = argv[1];
    int port = atoi(argv[2]);

    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &addr.sin_addr);

    int listenfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(listenfd >= 0);

    ret = bind(listenfd, (struct sockaddr*)&addr, sizeof(addr));
    assert(ret == 0);

    ret = listen(listenfd, 5);
    assert(ret == 0);

    epoll_event events[MAX_EVENTS_SIZE];
    int epfd = epoll_create(1);
    assert(epfd > 0);
    addfd(epfd, listenfd, false);

    while(1) {
        ret = epoll_wait(epfd, events, MAX_EVENTS_SIZE, -1);
        if (ret < 0) {
            printf("epoll_wait failed.\n");
            break;
        }

        for(int i = 0; i < ret; ++i) {
            int sockfd = events[i].data.fd;
            if (sockfd == listenfd) {
                struct sockaddr_in addr_cli;
                socklen_t len = sizeof(addr_cli);
                int connfd = accept(listenfd, (struct sockaddr*)&addr_cli, &len);
                addfd(epfd, connfd, true);
            } else if (events[i].events && EPOLLIN) {
                pthread_t pid;
                fds fds_for_new_worker;
                fds_for_new_worker.epfd = epfd;
                fds_for_new_worker.connfd = events[i].data.fd;
                pthread_create(&pid, NULL, worker, (void*)&fds_for_new_worker);
            } else {
                printf("something else happen.\n");
            }
        }
    }

    close(listenfd);

    return 0;
}





