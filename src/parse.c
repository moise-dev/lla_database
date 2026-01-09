#include "parse.h"
#include "common.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

int output_file(
    int fd,
    struct dbheader_t* dbheader,
    struct employee_t* employee
) {
    employee = NULL;
    dbheader->magic = htonl(dbheader->magic);
    dbheader->version = htons(dbheader->version);
    dbheader->count = htons(dbheader->count);
    dbheader->filesize = htonl(dbheader->filesize);

    if (write(fd, dbheader, sizeof(struct dbheader_t)) <= 0) {
        perror("write");
        close(fd);
        return STATUS_ERROR;
    }
    return STATUS_SUCCESS;
}

int create_db_header(
    struct dbheader_t** headerOut
) {
    struct dbheader_t* header = calloc(1, sizeof(struct dbheader_t));
    if (header == NULL) {
        perror("calloc");
        return STATUS_ERROR;
    }

    header->magic = HEADER_MAGIC;
    header->version = 0x1;
    header->count = 0;
    header->filesize = sizeof(struct dbheader_t);

    *headerOut = header;
    return STATUS_SUCCESS;
}

int validate_db_header(
    int fd,
    struct dbheader_t** headerOut
) {
    struct dbheader_t* header = calloc(1, sizeof(struct dbheader_t));
    if (header == NULL) {
        perror("calloc");
        return STATUS_ERROR;
    }

    if (read(fd, header, sizeof(struct dbheader_t)) == -1) {
        perror("read");
        return STATUS_ERROR;
    }

    header->magic = ntohl(header->magic);
    header->version = ntohs(header->version);
    header->count = ntohs(header->count);
    header->filesize = ntohl(header->filesize);

    if (header->magic != (int)HEADER_MAGIC) {
        printf(
            "[x] Invalid magic. Received %d, expected %d. Read %d\n",
            header->magic,
            HEADER_MAGIC,
            header->magic
        );
        free(header);
        return STATUS_ERROR;
    }
    if (header->version != 1) {
        printf("[x] Invalid version. Received %d\n", header->version);
        free(header);
        return STATUS_ERROR;
    }

    struct stat size = {0};
    int err = fstat(fd, &size);
    if (err == -1) {
        perror("fstat");
        free(header);
        return STATUS_ERROR;
    }

    if (header->filesize != size.st_size) {
        printf("[x] Invalid size. Received %d\n", header->filesize);
        free(header);
        return STATUS_ERROR;
    }

    *headerOut = header;
    return STATUS_SUCCESS;
}
