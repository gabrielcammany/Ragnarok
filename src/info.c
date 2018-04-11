//
// Created by gabriel on 10/04/18.
//


#include "../include/info.h"

enum {
    EXT2,
    EXT3,
    EXT4,
    FAT12,
    FAT16,
    FAT32,
    UNKNOWN
};

void info(char *name) {
	int fd = open(name, 0644, O_RDONLY);
	if (fd < 0)  {
		fprintf(stderr, "Error al abrir el fichero.\n");
		return;
	}

	detecta_tipo(fd);
}

int detecta_tipo(int fd) {
    lseek(fd, 1024 + 0x38, SEEK_SET);
    uint16_t magic_number;
    read(fd, &magic_number, sizeof magic_number);

    if (magic_number == 0xEF53) {
        lseek(fd, 1024 + 0x60, SEEK_SET);
        uint32_t feature_compat;
        read(fd, &feature_compat, sizeof feature_compat);
        printf("32 bytes %X\n", feature_compat);
        if (feature_compat & 0x40) {
            return EXT4;
        } else {
            lseek(fd, 1024 + 0x5C, SEEK_SET);
            read(fd, &feature_compat, sizeof feature_compat);
            if (feature_compat & 0x04) {
                return EXT3;
            } else {
                return EXT2;
            }
        }
    } else {
        //could possibly be a FAT filesystem
        char fat_type[6];

        if (strcmp(read_at(fd, 0x36, fat_type), FAT12_NAME) == 0) {
            return FAT12;
        } else if (strcmp(fat_type, FAT16_NAME) == 0) {
            return FAT16;
        } else if (strcmp(read_at(fd, 0x52, fat_type), FAT32_NAME) == 0) {
            return FAT32;
        }
    }
    return UNKNOWN;
}

char* read_at(int fd, unsigned int offset, char out[6]) {
    lseek(fd, offset, SEEK_SET);
    read(fd, out, sizeof out);
    out[5] = 0;
    return out;
}