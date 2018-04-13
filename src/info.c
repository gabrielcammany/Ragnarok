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

const char *const NAMES[] = {"EXT2", "EXT3", "EXT4", "FAT12", "FAT16", "FAT32", "UNKNOWN"};

void info(char *name) {
    int fd = open(name, 0644, O_RDONLY);
    int type = 0;

    if (fd < 0) {
        fprintf(stderr, "Error al abrir el fichero.\n");
        return;
    }

    type = detecta_tipo(fd);

    switch (type) {
        case EXT4:
            ext4_info(fd);
            break;
        case FAT32:
            //FAT32_info(fd);
            break;
        default:
            printf(ERR_FILESYSTEM, NAMES[type]);
            break;
    }

}

int detecta_tipo(int fd) {
    lseek(fd, 1024 + 0x38, SEEK_SET);
    uint16_t magic_number;
    read(fd, &magic_number, sizeof magic_number);


    if (magic_number == 0xEF53) {
        lseek(fd, 1024 + 0x60, SEEK_SET);
        uint32_t feature_compat;
        read(fd, &feature_compat, sizeof feature_compat);
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
        char fat_type[8];
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


void ext4_info(int fd) {

    printf("---- Filesystem Information ----\n\n");
    printf("FileSystem: EXT4\n\n");
    uint32_t read = 0;
    uint32_t second_read = 0;

    printf("INODE INFO\n");

    read_with_offset(fd, 0x58, &read, sizeof(read) / 2);
    printf("Inode Size: %d\n", read);

    read_with_offset(fd, 0x0, &read, sizeof read);
    printf("Number of inodes: %d\n", read);

    read_with_offset(fd, 0x54, &read, sizeof read);
    printf("First inode: %d\n", read);

    read_with_offset(fd, 0x28, &read, sizeof read);
    printf("Inodes group: %d\n", read);

    read_with_offset(fd, 0x10, &read, sizeof read);
    printf("Free inodes: %d\n\n", read);

    printf("BLOCK INFO\n");
    read_with_offset(fd, 0x18, &read, sizeof(read));
    printf("Block Size: %0.f\n", pow(2, 10 + read));


    read_with_offset(fd, 0x0154, &read, sizeof read); //high
    read_with_offset(fd, 0x08, &second_read, sizeof second_read); //low
    printf("Reserved blocks: %lu\n", (unsigned long) ((((uint64_t) read) << 32) | second_read));

    read_with_offset(fd, 0x0C, &read, sizeof read); //low
    read_with_offset(fd, 0x158, &second_read, sizeof second_read); //high
    printf("Free Blocks: %lu\n", (unsigned long) ((((uint64_t) second_read) << 32) | read));

    read_with_offset(fd, 0x04, &read, sizeof read);
    printf("Total Blocks: %d\n", read);
    read_with_offset(fd, 0x10, &read, sizeof read);
    printf("Free inodes: %d\n\n", read);

    char lectura[17];
    read_with_offset(fd, 0x78, lectura, sizeof(char) * 16);
    lectura[16] = 0;
    printf("VOLUME INFO\n");
    printf("Volume name: %s\n", lectura); //0x78 - 16 bytes

    read_with_offset(fd, 0x40, read, sizeof read);
    time_t t = read;
    printf("Last check: %X\n", t); //0x40 tiempo desde epoch 32 bits
    printf("Last mount: %s\n"); //0x2C tiempo epoch 32bits
    printf("Last written: %s\n"); //0x30 tiempo lo mismo


}

void read_with_offset(int fd, unsigned int offset, void *out, size_t size) {
    lseek(fd, EXT4_OFFSET + offset, SEEK_SET);
    read(fd, out, size);
}

void FAT32_info(int fd) {

    printf("---- Filesystem Information ----\n\n");
    printf("FileSystem: FAT32\n\n");

    printf("System name: %s\n");
    printf("Sector Size: %d\n");
    printf("Sectors per Cluster: %d\n");
    printf("Reserved Sectors: %d\n");
    printf("Number of FATs: %d\n");
    printf("Maximum Root Entries: %d\n");
    printf("Sectors per FAT: %d\n");
    printf("Label: %s\n\n");
}

//Pre: out es mínimo un array de 6 caracteres
char *read_at(int fd, unsigned int offset, char *out) {
    lseek(fd, offset, SEEK_SET);
    read(fd, out, sizeof out);
    out[5] = 0;
    return out;
}