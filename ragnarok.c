#include <stdio.h>
#include <string.h>
#include "include/info.h"
#include "include/search.h"

int check_operation(int argc, char **argv);

int main(int argc, char **argv) {
    int type;

    if ((type = check_operation(argc, argv)) < 0) {
        printf("Error!\n");
        return -1;
    }

    fd = open(argv[argc == 5 ? 3 : 2], 0666, O_RDWR);

    if (fd < 0) {

        fprintf(stderr, "Error al abrir el fichero.\n");

        return -1;
    }

    switch (type) {
        case O_INFO:
            info();
            break;
        case O_SEARCH:
            search(0, argv[3]);
            break;
        case O_SHOW:
            search(1, argv[3]);
            break;
        case O_EN_READ_ONLY: //TODO change_attr(type, argv[3], NULL) y juntar los casos del switch
            change_attr(O_EN_READ_ONLY, argv[3], NULL);
            break;
        case O_DIS_READ_ONLY:
            change_attr(O_DIS_READ_ONLY, argv[3], NULL);
            break;
        case O_ENABLE_HIDE:
            change_attr(O_ENABLE_HIDE, argv[3], NULL);
            break;
        case O_DISABLE_HIDE:
            change_attr(O_DISABLE_HIDE, argv[3], NULL);
            break;
        case O_NEW_DATE:
            change_attr(O_NEW_DATE, argv[4], argv[2]);
            break;
    }

    close(fd);
    return 0;
}


int check_operation(int argc, char **argv) {
    if (argc < 2) return -1;

    if (!strcmp(argv[1], INFO)) {
        if (argc != 3) return -1;
        return O_INFO;
    } else if (!strcmp(argv[1], SEARCH)) {
        if (argc != 4) return -1;
        return O_SEARCH;
    } else if (!strcmp(argv[1], SHOW)) {
        if (argc != 4) return -1;
        return O_SHOW;
    } else if (!strcmp(argv[1], EN_READ_ONLY)) {
        if (argc != 4) return -1;
        return O_EN_READ_ONLY;
    } else if (!strcmp(argv[1], DIS_READ_ONLY)) {
        if (argc != 4) return -1;
        return O_DIS_READ_ONLY;
    } else if (!strcmp(argv[1], ENABLE_HIDE)) {
        if (argc != 4) return -1;
        return O_ENABLE_HIDE;
    } else if (!strcmp(argv[1], DISABLE_HIDE)) {
        if (argc != 4) return -1;
        return O_DISABLE_HIDE;
    } else if (!strcmp(argv[1], NEW_DATE)) {
        if (argc != 5) return -1;
        return O_NEW_DATE;
    }
    return 0;
}

//https://www.easeus.com/resource/fat32-disk-structure.htm
//https://ext4.wiki.kernel.org/index.php/Ext4_Disk_Layout
//http://www.nongnu.org/ext2-doc/ext2.html#SUPERBLOCK