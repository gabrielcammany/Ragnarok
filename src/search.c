//
// Created by gabriel on 17/04/18.
//

#include <stdlib.h>
#include "../include/search.h"

uint32_t _deep_ext4(char show, char *name, uint32_t inode);

uint32_t _deepsearch_leaf_ext4(char *name, uint16_t eh_entries);

uint32_t _deepsearch_tree_ext4(char *name, uint16_t eh_entries);

uint64_t _deepshow_tree_ext4(uint64_t size, uint16_t eh_entries);

uint64_t _deepshow_leaf_ext4(uint64_t size, uint16_t eh_entries);

void _search_fat32(char *name);







void search(char show, char *name) {

	switch (detecta_tipo()) {
		case EXT4:

			ext4_get_structure();

			(show == 0 ? ext4_inode_info(_deep_ext4(show, name, 2)) : _deep_ext4(show, name, 2));

			break;
		case FAT32:
			_search_fat32(name);
			break;
		default:
			break;
	}
}

uint32_t _deep_ext4(char show, char *name, uint32_t inode) {
	uint32_t return_value = 0;
	uint32_t read_32 = 0;
	uint32_t read_64 = 0;
	ext4_extent_header header;
	ext4_extent_idx *tree;

	off_t offset = lseek(fd, 0, SEEK_CUR); //Guardar posicion puntero antes de modificarlo

	lseek(fd, ext4.inode.table_loc * ext4.block.size + (ext4.inode.size * (inode - 1)) + 0x28, SEEK_SET);

	read(fd, &header, sizeof(header));


	if (show < 2) {

		if (header.eh_depth == 0) {

			return_value = _deepsearch_leaf_ext4(name, header.eh_entries);

		} else {

			return_value = _deepsearch_tree_ext4(name, header.eh_entries);
		}

		if (return_value > 0 && show == 1) {

			printf("\nFile found! Showing content...\n\n");

			_deep_ext4(2, NULL, return_value);

		}

	} else {

		lseek(fd, -(0x28 + sizeof(header)), SEEK_CUR);

		lseek(fd, 0x6C, SEEK_CUR);
		read(fd, &read_32, sizeof(read_32));
		read_64 = ((read_64 | read_32 << 32));

		lseek(fd, -0x6C, SEEK_CUR);
		read(fd, &read_32, sizeof(read_32));
		read_64 = ((read_64 | read_32));

		lseek(fd, (0x20 + sizeof(header)), SEEK_CUR);

		if (header.eh_depth == 0) {

			_deepshow_leaf_ext4(read_64, header.eh_entries);

		} else {

			_deepshow_tree_ext4(read_64, header.eh_entries);

		}

	}

	lseek(fd, offset, SEEK_SET); //Volver a poner el puntero en la posicion inicial para la llamada recursiva

	return return_value;

}

uint32_t _deepsearch_tree_ext4(char *name, uint16_t eh_entries) {
	int i;
	ext4_extent_header header;
	ext4_extent_idx *leaf;
	uint64_t read_64 = 0;
	uint32_t return_value = 0;

	leaf = (ext4_extent_idx *) malloc(sizeof(ext4_extent_idx) * eh_entries);

	read(fd, leaf, sizeof(ext4_extent_idx) * eh_entries);

	for (i = 0; i < eh_entries; i++) {

		read_64 = ((read_64 | leaf[i].ei_leaf_hi) << 32) | leaf[i].ei_leaf_lo;

		lseek(fd, read_64 * ext4.block.size, SEEK_SET);

		read(fd, &header, sizeof(header));

		if (header.eh_depth == 0) {

			return_value = _deepsearch_leaf_ext4(name, header.eh_entries);

		} else {

			return_value = _deepsearch_tree_ext4(name, header.eh_entries);

		}

	}

	return return_value;

}

uint32_t _deepsearch_leaf_ext4(char *name, uint16_t eh_entries) {

	int i;
	size_t size;

	ext4_extent *leaf;
	uint64_t read_64 = 0;
	char *file_name;
	ext4_dir_entry_2 dir_entry_2;

	leaf = (ext4_extent *) malloc(sizeof(ext4_extent) * eh_entries);

	read(fd, leaf, sizeof(ext4_extent) * eh_entries);

	for (i = 0; i < eh_entries; i++) {

		read_64 = ((read_64 | leaf[i].ee_start_hi) << 32) | leaf[i].ee_start_lo;

		lseek(fd, read_64 * ext4.block.size, SEEK_SET);

		read(fd, &dir_entry_2, sizeof(dir_entry_2));

		do {

			size = sizeof(char) * ((dir_entry_2.rec_len > (256 + 8) ? dir_entry_2.name_len : dir_entry_2.rec_len - 8));

			file_name = (char *) calloc(size, size);
			read(fd, file_name, size);

			if (!strcmp(file_name, name) && dir_entry_2.file_type == 1) {

				return dir_entry_2.inode;

			}

			if (strcmp(file_name, ".") != 0
				&& strcmp(file_name, "..") != 0
				&& dir_entry_2.file_type == 2) {

				if (dir_entry_2.file_type == 2) {

					if ((read_64 = _deep_ext4(0, name, dir_entry_2.inode))) {

						free(file_name);
						free(leaf);

						return (uint32_t) read_64;

					}

				}

			}

			free(file_name);
			read(fd, &dir_entry_2, sizeof(dir_entry_2));

		} while (dir_entry_2.inode != 0);

	}

	free(leaf);

	return NOT_FOUND;

}


uint64_t _deepshow_tree_ext4(uint64_t size, uint16_t eh_entries) {

	int i;
	char buff;
	uint64_t read_64 = 0;
	uint32_t return_value = 0;
	ext4_extent_header header;
	ext4_extent_idx *leaf;

	leaf = (ext4_extent_idx *) malloc(sizeof(ext4_extent_idx) * eh_entries);

	read(fd, leaf, sizeof(ext4_extent) * eh_entries);

	for (i = 0; i < eh_entries; i++) {

		read_64 = ((read_64 | leaf[i].ei_leaf_hi) << 32) | leaf[i].ei_leaf_lo;

		lseek(fd, read_64 * ext4.block.size, SEEK_SET);

		read(fd, &header, sizeof(header));

		if (header.eh_depth == 0) {

			size = _deepshow_leaf_ext4(size, header.eh_entries);

		} else {

			size = _deepshow_tree_ext4(size, header.eh_entries);

		}


	}

}

uint64_t _deepshow_leaf_ext4(uint64_t size, uint16_t eh_entries) {

	int i;
	char buff;
	uint64_t read_64 = 0;
	uint64_t aux_size = 0;
	ext4_extent *leaf;

	leaf = (ext4_extent *) malloc(sizeof(ext4_extent) * eh_entries);

	read(fd, leaf, sizeof(ext4_extent) * eh_entries);

	for (i = 0; i < eh_entries; i++) {

		read_64 = ((read_64 | leaf[i].ee_start_hi) << 32) | leaf[i].ee_start_lo;

		lseek(fd, read_64 * ext4.block.size, SEEK_SET);

		aux_size = (size > (leaf[i].ee_len * 1024) ? (uint64_t) (leaf[i].ee_len * 1024) : size);

		for (read_64 = 0; read_64 < size; read_64++) {

			read(fd, &buff, sizeof(char));
			if (buff == 0)break;
			printf("%c", buff);

		}

		size -= aux_size;

	}

	return size;

}


void _search_fat32(char *name) {

}