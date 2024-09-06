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

void* client_thread(void* arg){
	int clientfd = *(int *)arg;
	while(1){
		char buffer[128] = {0};
		int count = recv(clientfd, buffer, 128, 0);
		if(count == 0){
			break;
		}
		send(clientfd, buffer, count, 0);
		printf("clientfd: %d\n, count: %d\n", clientfd,count);
	}
	close(clientfd);
}

int main(){
	
	int sockefd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(struct sockaddr_in));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(2048);
	if(-1 == bind(sockefd, (struct sockaddr*)&serveraddr, sizeof(struct sockaddr))){
		perror("bind");
		return -1;
	}
	listen(sockefd, 10);
	printf("xxxxx");
	#if 0
	while(1){	
		struct sockaddr_in clientaddr;
		socklen_t len = sizeof(clientaddr);
		int clientfd = accept(sockefd, (struct sockaddr*)&clientaddr, &len);
		
		pthread_t thid;
		pthread_create(&thid, NULL, client_thread, &clientfd);
	}
	
	

	getchar();
	#elif 0 //select(maxfd, rset, wset, timeout);
	fd_set rfds, rset;
	FD_ZERO(&rfds);
	FD_SET(sockefd, &rfds);
	

	int maxfd = sockefd;
	printf("loop\n");
	while(1){
		rset = rfds;
		int nready = select(maxfd+1, &rset, NULL, NULL, NULL);
		if(FD_ISSET(sockefd, &rset)){
			struct sockaddr_in clientaddr;
			socklen_t len = sizeof(clientaddr);
			int clientfd = accept(sockefd, (struct sockaddr*)&clientaddr, &len);
			printf("sockfd, clientfd:%d\n",clientfd);
			FD_SET(clientfd, &rfds);
			maxfd = clientfd;
		}
		int i = 0;
		for(i = sockefd+1; i <= maxfd; i++){
			if(FD_ISSET(i, &rset)){
				char buffer[128] = {0};
				int count = recv(i, buffer, 128, 0);
				if(count == 0){
					printf("disconnect\n");
					FD_CLR(i, &rset);
					close(i);
					break;
				}
				send(i, buffer, count, 0);
				printf("clientfd: %d\n, count: %d\n", i, count);
			}
		}
	}
	#elif 0//poll
		struct pollfd fds[1024] = {0};
		fds[sockefd].fd = sockefd;
		fds[sockefd].fd = POLLIN;

		int maxfd = sockefd;
		while(1){
			printf("xxxxx");
			int nready = poll(fds, maxfd+1, -1);
			if(fds[sockefd].revents & POLLIN){
				struct sockaddr_in clientaddr;
				socklen_t len = sizeof(clientaddr);
				int clientfd = accept(sockefd, (struct sockaddr*)&clientaddr, &len);
				printf("sockfd, clientfd:%d\n",clientfd);
				fds[sockefd].fd = clientfd;
				fds[sockefd].events = POLLIN;

				maxfd = clientfd;
			}
			printf("xxxxx");
			int i = 0;
			for(i = sockefd+1; i <= maxfd; i++){
				if(fds[i].revents & POLLIN){
					char buffer[128] = {0};
					int count = recv(i, buffer, 128, 0);
					if(count == 0){
						printf("disconnect\n");
						fds[i].fd = -1;
						fds[i].events = 0;
						close(i);
						continue;
					}
					send(i, buffer, count, 0);
					printf("clientfd: %d\n, count: %d\n", i, count);
				}
			}
		}
	#else
		int epfd = epoll_create(1);
		struct epoll_event(ev);
		ev.events = EPOLLIN;
		ev.data.fd = sockefd;

		epoll_ctl(epfd, EPOLL_CTL_ADD, epfd, &ev);
		struct epoll_event events[1024] = {0};
		while(1){
			int nready = epoll_wait(epfd, events, 1024, -1);
			int i = 0;
			for(int i = 0; i < nready; i++){
				int connfd = events[i].data.fd;
				if(sockefd == connfd){
					struct sockaddr_in clientaddr;
					socklen_t len = sizeof(clientaddr);
					int clientfd = accept(sockefd, (struct sockaddr*)&clientaddr, &len);
					ev.events = EPOLLIN | EPOLLET;
					ev.data.fd = clientfd;
					epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, &ev);
				}else if(events[i].events & EPOLLIN){
					char buffer[10] = {0};
					int count = recv(connfd, buffer, 10, 0);
					if(count == 0){
						printf("disconnect");
						epoll_ctl(epfd, EPOLL_CTL_DEL, connfd, NULL);
						close(i);
						continue;
					}
					send(connfd, buffer, count, 0);
					printf("client: %d, count:%d, buffer:%s\n", connfd, count, buffer);
				}
			}
		}

	#endif
}
