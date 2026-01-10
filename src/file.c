#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

#include "file.h"
#include "common.h"

#define PERMS_644 S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH

int create_db_file(
    char* filename
) {
    int fd = -1;
    if ((fd = open(filename, O_RDONLY)) != -1) {
        printf("File already exists\n");
        close(fd);
        return STATUS_ERROR;
    }

    fd = open(filename, O_RDWR | O_CREAT, PERMS_644);
    if (fd == -1) {
        perror("open");
        return STATUS_ERROR;
    }
    return fd;
}

int open_db_file(
    char* filename
) {
    int fd = open(filename, O_RDWR, PERMS_644);
    if (fd == -1) {
        perror("open");
        return STATUS_ERROR;
    }

    return fd;
}
