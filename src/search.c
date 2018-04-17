//
// Created by gabriel on 17/04/18.
//

#include "../include/search.h"
#include "../include/info.h"

void _search_ext4(int fd, char *name);
void _search_fat32(int fd, char *name);

void search(int fd, char *name) {

    switch (detecta_tipo(fd)) {
        case EXT4:
            _search_ext4(fd, name);
            break;
        case FAT32:
            _search_fat32(fd, name);
            break;
    }
}

void _search_ext4(int fd, char *name) {
    ext4_block block;
    ext4_get_structure(fd, &block);

}

void _search_fat32(int fd, char *name) {

}