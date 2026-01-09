#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(
    char* argv[]
) {
    printf("Usage:\n\t%s\n", argv[0]);
    printf("\t-n: (optional) create new db file\n");
    printf("\t-f: (mandatory) path to the db file\n");
}

int main(
    int argc,
    char* argv[]
) {
    int c;
    bool new_file = false;
    char* filepath = NULL;
    struct dbheader_t* headerOut = NULL;

    if (argc < 2) {
        print_usage(argv);
        return -1;
    }

    while ((c = getopt(argc, argv, "nf:")) != -1) {
        switch (c) {
        case 'n':
            new_file = true;
            break;
        case 'f':
            filepath = optarg;
            break;
        case '?':
            printf("Unknown option -%c\n", c);
        default:
            print_usage(argv);
            return -1;
        }
    }

    if (filepath == NULL) {
        printf("-f argument is mandatory.\n");
        return -1;
    }

    if (new_file == true) {
        int fd = create_db_file(filepath);
        if (fd == STATUS_ERROR) {
            printf("Unable to create db file.\n");
            return STATUS_ERROR;
        }
        printf("Created new file with fd %d.\n", fd);
        if (create_db_header(&headerOut) == STATUS_ERROR) {
            printf("[x] Unable to create db header.\n");
            return STATUS_ERROR;
        }

        struct employee_t* employee = NULL;
        output_file(fd, headerOut, employee);

        close(fd);

    } else {
        int fd = open_db_file(filepath);
        if (fd == STATUS_ERROR) {
            printf("[x] Unable to open db file.\n");
            return STATUS_ERROR;
        }
        printf("Opened new file with fd %d.\n", fd);
        if (validate_db_header(fd, &headerOut) == STATUS_ERROR) {
            printf("[x] Unable to validate db header.\n");
            return STATUS_ERROR;
        }
        close(fd);
    }

    return 0;
}
