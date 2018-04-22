//
// Created by gabriel on 17/04/18.
//

#ifndef RAGNAROK_SEARCH_H
#define RAGNAROK_SEARCH_H

#include "info.h"

typedef struct __attribute__((packed)) {
	uint16_t eh_magic;
	uint16_t eh_entries;
	uint16_t eh_max;
	uint16_t eh_depth;
	uint32_t eh_generation;
} ext4_extent_header;

typedef struct __attribute__((packed)) {
	uint32_t ei_block;
	uint32_t ei_leaf_lo;
	uint16_t ei_leaf_hi;
	uint16_t ei_unused;
} ext4_extent_idx; //Raiz

typedef struct __attribute__((packed)) {
	uint32_t ee_block;
	uint16_t ee_len;
	uint16_t ee_start_hi;
	uint32_t ee_start_lo;
} ext4_extent; //Leaf

typedef struct __attribute__((packed)) {
	uint32_t inode;
	uint16_t rec_len;
	uint8_t name_len;
	uint8_t file_type;
} ext4_dir_entry_2;


void search(int fd, char *name);

#endif //RAGNAROK_SEARCH_H
