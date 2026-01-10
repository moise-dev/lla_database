#include "parse.h"
#include "common.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

void list_employees(
    int fd,
    struct dbheader_t* headerOut,
    struct employee_t* employees
) {
    if (read_employees(fd, headerOut, &employees) == STATUS_ERROR) {
        printf("[x] Error in reading employees.\n");
        return;
    }
    printf("\nList of employees:\n");
    for (int i = 0; i < headerOut->count; i++) {
        printf(
            "\tName: %s, Address: %s, Hours: %d\n",
            employees[i].name,
            employees[i].address,
            employees[i].hours
        );
    }
}

int add_employee(
    struct dbheader_t* dbhdr,
    struct employee_t** employees,
    char* addstring
) {
    if (dbhdr == NULL || employees == NULL || addstring == NULL) {
        printf("[x] One of the input args is empty\n");
        return STATUS_ERROR;
    }
    char* name = strtok(addstring, ",");
    if (name == NULL) {
        printf("[x] Failed to extract name\n");
        return STATUS_ERROR;
    }
    char* address = strtok(NULL, ",");
    if (name == NULL) {
        printf("[x] Failed to extract address\n");
        return STATUS_ERROR;
    }

    char* hours = strtok(NULL, ",");
    if (name == NULL) {
        printf("[x] Failed to extract hours\n");
        return STATUS_ERROR;
    }

    struct employee_t* e = *employees;
    e = realloc(e, sizeof(struct employee_t) * dbhdr->count + 1);
    if (e == NULL) {
        perror("realloc");
        return STATUS_ERROR;
    }

    dbhdr->count++;
    int i = dbhdr->count - 1;
    strncpy(e[i].name, name, sizeof(e[i].name) - 1);
    strncpy(e[i].address, address, sizeof(e[i].address) - 1);

    e[i].hours = atoi(hours);

    *employees = e;
    return STATUS_SUCCESS;
}

int read_employees(
    int fd,
    struct dbheader_t* dbheadr,
    struct employee_t** employeesOut
) {

    int count = dbheadr->count;
    struct employee_t* employees = calloc(count, sizeof(struct employee_t));
    if (employees == NULL) {
        perror("calloc");
        return STATUS_ERROR;
    }

    read(fd, employees, count * sizeof(struct employee_t));

    for (int i = 0; i < count; i++) {
        employees[i].hours = ntohl(employees[i].hours);
    }

    *employeesOut = employees;
    return STATUS_SUCCESS;
}

int output_file(
    int fd,
    struct dbheader_t* dbheader,
    struct employee_t* employee
) {
    int realcount = dbheader->count;
    dbheader->magic = htonl(dbheader->magic);
    dbheader->filesize = htonl(
        sizeof(struct dbheader_t) + realcount * sizeof(struct employee_t)
    );
    dbheader->version = htons(dbheader->version);
    dbheader->count = htons(dbheader->count);

    lseek(fd, 0, SEEK_SET);

    if (write(fd, dbheader, sizeof(struct dbheader_t)) <= 0) {
        printf("[x] Error writing headers.\n");
        perror("write");
        close(fd);
        return STATUS_ERROR;
    }

    for (int i = 0; i < realcount; i++) {
        employee[i].hours = htonl(employee[i].hours);
        if (write(fd, &employee[i], sizeof(struct employee_t)) ==
            STATUS_ERROR) {
            printf("[x] Error writing employee.\n");
            perror("write");
            close(fd);
            return STATUS_ERROR;
        }
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
