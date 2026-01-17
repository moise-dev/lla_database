#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>

#include "proto.h"

#define ERROR -1
#define SUCCESS 0

void handle_client(int fd) {
    char buf[4096] = {0};
    read(fd, hdr, sizeof(proto_hdr_t) + reallen);

    proto_hdr_t *hdr = (proto_hdr_t*)buf;
    hdr->type = ntohl(hdr->type);
    hdr->len = ntohs(hdr->len);

    int *data = hdr[1];
    *data = ntohl(*data);

    if (hdr->type != PROTO_HELLO) {
        printf("[x] type mismatch\n");
        return;
    }
    if (hdr->len != 1) {
        printf("[x] len mismatch\n");
        return;
    }

    printf("server connected on v1\n");


}
int main(int argc, char *argv[]) {
    
    if (argc > 2){
        printf("Usage: %s [<ip_addr>]", argv[0]);
        return ERROR;
    }
    char *ip_addr = "127.0.0.1";
    if (argc == 2) {
        ip_addr = argv[1];
    }

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    int return_value = SUCCESS;
    if (fd == -1) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in socket_client = {0};
    socket_client.sin_family = AF_INET;
    socket_client.sin_port = htons(5555);
    socket_client.sin_addr.s_addr = inet_addr(ip_addr);
    if (connect(fd, (struct sockaddr *)&socket_client, sizeof(socket_client)) == -1) {
        perror("connect");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}
