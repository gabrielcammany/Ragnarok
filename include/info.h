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

#define FAT12_NAME "FAT12"
#define FAT16_NAME "FAT16"
#define FAT32_NAME "FAT32"

void info(char *name);

int detecta_tipo(int fd);

char* read_at(int fd, unsigned int offset, char out[6]);

#endif //RAGNAROK_OPERATION_H
