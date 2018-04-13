#include <stdio.h>
#include <string.h>
#include "include/info.h"

#define INFO "-info"
#define SEARCH "-search"
#define SHOW "-show"
#define READ "-r"
#define WRITE "-w"
#define ENABLE_HIDE "-h"
#define DISABLE_HIDE "-h"
#define NEW_DATE "-d"

int check_operation(int argc, char **argv);

int main(int argc, char **argv) {

	if (check_operation(argc, argv) < 0) {

		printf("Error!\n");

	}
	return 0;
}


int check_operation(int argc, char **argv) {
	if (argc < 2) return -1;

	if (!strcmp(argv[1], INFO)) {

		if (argc != 3) return -1;

		info(argv[2]);

	} else if (!strcmp(argv[1], SEARCH)) {

	} else if (!strcmp(argv[1], SHOW)) {

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