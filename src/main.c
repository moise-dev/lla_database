#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(
    char* argv[]
) {
    printf("Usage:\n\t%s\n", argv[0]);
    printf("\t-n: (optional) create new db file\n");
    printf("\t-l: (optional) list db file\n");
    printf("\t-a: (optional) add a csv like string in db\n");
    printf("\t-f: (mandatory) path to the db file\n");
}

int main(
    int argc,
    char* argv[]
) {
    int c;
    int fd = -1;
    bool new_file = false;
    bool listdb = false;
    char* filepath = NULL;
    char* addstring = NULL;
    struct dbheader_t* headerOut = NULL;
    struct employee_t* employees = NULL;

    if (argc < 2) {
        print_usage(argv);
        return STATUS_ERROR;
    }

    while ((c = getopt(argc, argv, "nf:a:l")) != -1) {
        switch (c) {
        case 'n':
            new_file = true;
            break;
        case 'f':
            filepath = optarg;
            break;
        case 'a':
            addstring = optarg;
            break;
        case 'l':
            listdb = true;
            break;
        case '?':
            printf("[x] Unknown option -%c\n", c);
        default:
            print_usage(argv);
            return STATUS_ERROR;
        }
    }

    if (filepath == NULL) {
        printf("[x] -f argument is mandatory.\n");
        return STATUS_ERROR;
    }

    if (new_file) {
        fd = create_db_file(filepath);
        if (fd == STATUS_ERROR) {
            printf("[x] Unable to create db file.\n");
            return STATUS_ERROR;
        }
        if (create_db_header(&headerOut) == STATUS_ERROR) {
            printf("[x] Unable to create db header.\n");
            return STATUS_ERROR;
        }

    } else {
        fd = open_db_file(filepath);
        if (fd == STATUS_ERROR) {
            printf("[x] Unable to open db file.\n");
            return STATUS_ERROR;
        }
        if (validate_db_header(fd, &headerOut) == STATUS_ERROR) {
            printf("[x] Unable to validate db header.\n");
            return STATUS_ERROR;
        }
    }

    if (read_employees(fd, headerOut, &employees) == STATUS_ERROR) {
        printf("Failed to read employees");
        return STATUS_ERROR;
    }

    if (addstring) {
        if (add_employee(headerOut, &employees, addstring) == STATUS_ERROR) {
            printf("[x] Error adding employee.\n");
            return STATUS_ERROR;
        }
    }

    if (listdb) {
        list_employees(fd, headerOut, employees);
    }

    output_file(fd, headerOut, employees);

    return STATUS_SUCCESS;
}
