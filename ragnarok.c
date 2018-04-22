#include <stdio.h>
#include <string.h>
#include "include/info.h"
#include "include/search.h"

#define INFO "-info"
#define SEARCH "-search"
#define SHOW "-show"
#define READ "-r"
#define WRITE "-w"
#define ENABLE_HIDE "-h"
#define DISABLE_HIDE "-h"
#define NEW_DATE "-d"

enum {
    O_INFO,
    O_SEARCH,
    O_SHOW,
    O_READ,
    O_WRITE,
    O_ENABLE_HIDE,
    O_DISABLE_HIDE,
    O_NEW_DATE
};

int check_operation(int argc, char **argv);

int main(int argc, char **argv) {
    int type;

	if ((type = check_operation(argc, argv)) < 0) {
		printf("Error!\n");
		return -1;
	}

    fd = open(argv[2], 0644, O_RDONLY);

    if (fd < 0) {
        fprintf(stderr, "Error al abrir el fichero.\n");
        return -1;
    }

    switch (type) {
        case O_INFO:
            info();
            break;
        case O_SEARCH:
            search(0,argv[3]);
            break;
        case O_SHOW:
			search(1,argv[3]);
            break;
        case O_READ:
            break;
        case O_WRITE:
            break;
        case O_ENABLE_HIDE:
            break;
        case O_DISABLE_HIDE:
            break;
        case O_NEW_DATE:
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
	} else if (!strcmp(argv[1], READ)) {

	} else if (!strcmp(argv[1], WRITE)) {

	} else if (!strcmp(argv[1], ENABLE_HIDE)) {

	} else if (!strcmp(argv[1], DISABLE_HIDE)) {

	} else if (!strcmp(argv[1], NEW_DATE)) {

	}
	return 0;
}

//https://www.easeus.com/resource/fat32-disk-structure.htm
//https://ext4.wiki.kernel.org/index.php/Ext4_Disk_Layout
//http://www.nongnu.org/ext2-doc/ext2.html#SUPERBLOCK