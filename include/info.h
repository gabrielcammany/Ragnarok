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

void info(char *name);

void ext4_info(int fd);

void FAT32_info(int fd);

void read_with_offset(int fd, unsigned int offset, void *out, size_t size);

int detecta_tipo(int fd);

char *read_at(int fd, unsigned int offset, char out[6]);

#endif //RAGNAROK_OPERATION_H
