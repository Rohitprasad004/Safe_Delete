// main.c
#include "safedelete.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("SafeDelete - Secure File Manager\n");
        printf("Usage:\n");
        printf("  Delete:  %s -d <file>\n", argv[0]);
        printf("  List:    %s -l\n", argv[0]);
        printf("  Recover: %s -r <index>\n", argv[0]);
        printf("  Cleanup: %s -c <days>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "-d") == 0 && argc == 3) {
        secure_delete(argv[2]);
    } 
    else if (strcmp(argv[1], "-l") == 0) {
        list_trash();
    }
    else if (strcmp(argv[1], "-r") == 0 && argc == 3) {
        recover_file(atoi(argv[2]));
    }
    else if (strcmp(argv[1], "-c") == 0 && argc == 3) {
        empty_trash(atoi(argv[2]));
    }
    else {
        printf("Invalid command!\n");
    }

    return 0;
}
