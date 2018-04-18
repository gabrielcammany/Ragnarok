//
// Created by gabriel on 10/04/18.
//

#ifndef RAGNAROK_OPERATION_H
#define RAGNAROK_OPERATION_H

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include <time.h>

#define FAT12_NAME "FAT12"
#define FAT16_NAME "FAT16"
#define FAT32_NAME "FAT32"
#define EXT4_OFFSET 1024
#define ERR_FILESYSTEM "File System not recognized (%s)\n"

enum {
    EXT2,
    EXT3,
    EXT4,
    FAT12,
    FAT16,
    FAT32,
    UNKNOWN
};

typedef struct __attribute__((packed)) {
    uint32_t block_size;
    uint64_t inode_table_loc;
    uint16_t inode_size;
    uint16_t group_descriptor_size;
} ext4_block;

void info(int fd);

void ext4_info(int fd);

void fat32_info(int fd);

void read_with_offset(int fd, unsigned int offset, void *out, size_t size);

int detecta_tipo(int fd);

char *read_at(int fd, unsigned int offset, char out[6]);

void ext4_get_structure(int fd, ext4_block *out);

#endif //RAGNAROK_OPERATION_H
