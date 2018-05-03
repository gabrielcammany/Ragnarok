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
	uint32_t size;
	uint32_t count;
	uint32_t free_count;
} Block;

typedef struct __attribute__((packed)) {
	uint64_t table_loc;
	uint16_t size;
	uint32_t count;
	uint32_t free_count;
} Inode;


typedef struct __attribute__((packed)) {
    Block block;
    Inode inode;

    uint16_t group_descriptor_size;
} EXT4_info;

typedef struct __attribute__((packed)) {
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    //no cal numbers of fats
    uint32_t sectors_per_fat;
    uint32_t root_first_cluster;

    uint32_t fat_location;
    uint32_t first_cluster;
} FAT32_info;

EXT4_info ext4;
FAT32_info fat32;
int fd;

void info();

void ext4_info();

void fat32_info();

void read_with_offset(unsigned long offset, void *out, size_t size);

int detecta_tipo();

char *read_at(unsigned int offset, char out[6]);

void ext4_get_structure();

void ext4_inode_info(uint32_t inode);

void fat32_file_info(off_t off);

void fat32_get_structure();

#endif //RAGNAROK_OPERATION_H
