#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <string.h>

#define MAXEVENTS 64

static int create_and_bind(char *port) {
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int s, sfd;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	s = getaddrinfo(NULL, port, &hints, &result);
	if (s != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		return -1;
	}

	for(rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sfd == -1)
			continue;

		s = bind(sfd, rp->ai_addr, rp->ai_addrlen);
		if (s == 0) {
			break;
		}

		close(sfd);
	}

	if (rp == NULL) {
		fprintf(stderr, "Could not bind\n");
		return -1;
	}

	freeaddrinfo(result);

	return sfd;
}

static int make_socket_non_blocking(int fd) {
	int flags, s;

	flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1) {
		fprintf(stderr, "fcntl get failed.");
		return -1;
	}

	flags |= O_NONBLOCK;
	s = fcntl(fd, F_SETFL, flags);
	if (s == -1) {
		fprintf(stderr, "fcntl set failed.");
		return -1;
	}

	return 0;
}

int main(int argc, char **argv) {
	int sfd, s;
	int efd;
	struct epoll_event event;
	struct epoll_event *events;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s [port]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	sfd = create_and_bind(argv[1]);
	if (sfd == -1)
		abort();

	s = make_socket_non_blocking(sfd);
	if (s == -1)
		abort();

	s = listen(sfd, SOMAXCONN);
	if (s == -1) {
		fprintf(stderr, "listen failed.\n");
		return -1;
	}

	efd = epoll_create(1);
	if (efd == -1) {
		fprintf(stderr, "epoll_create failed.\n");
		return -1;
	}

	event.data.fd = sfd;
	event.events = EPOLLIN | EPOLLET;
	s = epoll_ctl(efd, EPOLL_CTL_ADD, sfd, &event);
	if (s == -1) {
		fprintf(stderr, "epoll_ctl failed\n");
		return -1;
	}

	events = (epoll_event*)calloc(MAXEVENTS, sizeof(event));

	while(1) {
		int n, i;

		n = epoll_wait(efd, events, MAXEVENTS, -1);
		for(i = 0; i < n; ++i) {
			if ((events[i].events & EPOLLERR) ||
				(events[i].events & EPOLLHUP) ||
				(!(events[i].events & EPOLLIN))) {
				fprintf(stderr, "epoll error\n");
				close(events[i].data.fd);
				continue;
			} else if (sfd == events[i].data.fd) {
				while(1) {
					struct sockaddr in_addr;
					socklen_t in_len;
					int infd;

					in_len = sizeof(in_addr);
					infd = accept(sfd, &in_addr, &in_len);
					if (infd == -1) {
						if ((errno == EAGAIN) ||
							(errno == EWOULDBLOCK)) {
							break;
						} else {
							fprintf(stderr, "accept failed\n");
							break;
						}
					}
					
					s = make_socket_non_blocking(infd);
					if (s == -1) {
						fprintf(stderr, "make accept sockfd non block failed, fd: %d\n", infd);
					}

					event.data.fd = infd;
					event.events = EPOLLIN | EPOLLET;
					s = epoll_ctl(efd, EPOLL_CTL_ADD, infd, &event);
					if (s == -1) {
						fprintf(stderr, "epoll_ctl\n");
						return -1;
					}
				}
				continue;
			} else {
				int done = 0;
				while(1) {
					ssize_t count;
					char buf[512];

					count = read(events[i].data.fd, buf, sizeof(buf));
					if (count == -1) {
						if (errno != EAGAIN) {
							fprintf(stderr, "read\n");
							done = 1;
						}
						break;
					} else if (count == 0) {
						done = 1;
						break;
					}

					s = write(1, buf, count);
					if (s == -1) {
						fprintf(stderr, "write\n");
						return -1;
					}
				}
				
				if (done) {
					printf("close connection on descriptor %d\n", events[i].data.fd);
					close(events[i].data.fd);
				}
			}
		}
	}

	free(events);
	close(sfd);

	return 0;
}



