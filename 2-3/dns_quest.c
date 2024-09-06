#include <stdio.h>
#include <stdlib.h>
#include <ares.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define ASYNC 1

#if ASYNC
// 异步 DNS 请求
void callback(void *arg, int status, int timeouts, struct hostent *host) {
    if (status != ARES_SUCCESS) {
        fprintf(stderr, "查询失败: %s\n", ares_strerror(status));
        return;
    }

    struct in_addr **addr_list = (struct in_addr **)host->h_addr_list;
    for (int i = 0; addr_list[i] != NULL; i++) {
        printf("IP: %s\n", inet_ntoa(*addr_list[i]));
    }

    ares_free_hostent(host);
}

int main() {
    ares_channel channel;
    ares_init(&channel);

    ares_gethostbyname(channel, "www.baidu.com", AF_INET, callback, NULL);

    ares_process_fd(channel, ARES_SOCKET_BAD, ARES_SOCKET_BAD);
    ares_destroy(channel);

    return 0;
}
#else
// 同步 DNS 请求 (使用 gethostbyname)
int main() {
    struct hostent *host;
    host = gethostbyname("www.example.com");

    if (host == NULL) {
        perror("gethostbyname");
        return 1;
    }

    struct in_addr **addr_list = (struct in_addr **)host->h_addr_list;
    for (int i = 0; addr_list[i] != NULL; i++) {
        printf("IP: %s\n", inet_ntoa(*addr_list[i]));
    }

    return 0;
}
#endif
