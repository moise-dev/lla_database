#include <stdio.h>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include "proto.h"

void handle_client(int fd) {
    char buf[4096] = {0};
    int reallen = sizeof(int);
    proto_hdr_t *hdr = (proto_hdr_t*)buf;
    hdr->type = htonl(PROTO_HELLO);
    hdr->len = htons(reallen);
    
    int *data = (int*)&hdr[1];
    *data = htonl(1);
    write(fd, hdr, sizeof(proto_hdr_t) + reallen);

}

int main() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in sock_addr = {0};
    struct sockaddr_in sock_client = {0};

    int sock_client_size = 0;

    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = 0;
    sock_addr.sin_port = htons(5555);

    if (bind(fd, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) == -1) {
        perror("bind");
        close(fd);
        return -1;
    }

    if (listen(fd, 0) == -1) {
        perror("listen");
        close(fd);
        return -1;
    }

    while (1) {

        int client_fd = accept(fd, (struct sockaddr*)&sock_client, &sock_client_size) ;

        if (client_fd < 0) {
            perror("accept");
            close(fd);
            return -1;
        }
        handle_client(client_fd);
        close(client_fd);
    }

    close(fd);
    return 0;
}

