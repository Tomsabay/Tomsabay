#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/poll.h>
#include <sys/epoll.h>

#define BUFFER_LENGTH 1024

struct conn_item
{
	int fd;
	char buffer[BUFFER_LENGTH];
	int idx
};

struct conn_item connlist[1024] = {0};

int main()
{
	int sockefd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(struct sockaddr_in));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(2048);
	if (-1 == bind(sockefd, (struct sockaddr *)&serveraddr, sizeof(struct sockaddr)))
	{
		perror("bind");
		return -1;
	}
	listen(sockefd, 10);
	int epfd = epoll_create(1);
	struct epoll_event(ev);
	ev.events = EPOLLIN;
	ev.data.fd = sockefd;

	epoll_ctl(epfd, EPOLL_CTL_ADD, epfd, &ev);
	struct epoll_event events[1024] = {0};
	while (1)
	{
		int nready = epoll_wait(epfd, events, 1024, -1);
		int i = 0;
		for (int i = 0; i < nready; i++)
		{
			int connfd = events[i].data.fd;
			if (sockefd == connfd)
			{
				struct sockaddr_in clientaddr;
				socklen_t len = sizeof(clientaddr);
				int clientfd = accept(sockefd, (struct sockaddr *)&clientaddr, &len);
				ev.events = EPOLLIN;
				ev.data.fd = clientfd;
				epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, &ev);

				connlist[clientfd].fd = clientfd;
				memset(connlist[clientfd].buffer, 0, BUFFER_LENGTH);
				connlist[clientfd].idx = 0;
				printf("clientfd: %d\n", clientfd);
			}
			else if (events[i].events & EPOLLIN)
			{
				char *buffer = connlist[connfd].buffer;
				int idx = connlist[connfd].idx;

				int count = recv(connfd, buffer + idx, BUFFER_LENGTH - idx, 0);
				if (count == 0)
				{
					printf("disconnect");
					epoll_ctl(epfd, EPOLL_CTL_DEL, connfd, NULL);
					close(i);
					continue;
				}
				connlist[connfd].idx += count;
				send(connfd, buffer, count, 0);
				printf("client: %d, count:%d, buffer:%s\n", connfd, count, buffer);
			}
		}
	}
}
