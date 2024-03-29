//
// Created by gabriel on 17/04/18.
//

#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>
#include "../include/search.h"
#include "../include/info.h"

//#define SHOW_DEBUG
//#define SEARCH_DEBUG

#ifdef SEARCH_DEBUG
int depth;
#endif

uint32_t _ext4(char show, char *name, uint32_t inode);

uint32_t deepsearch_leaf_ext4(char *name, uint16_t eh_entries);

uint32_t deepsearch_tree_ext4(char *name, uint16_t eh_entries);

void deepshow_tree_ext4(uint16_t eh_entries);

void deepshow_leaf_ext4(uint16_t eh_entries);

off_t _fat32(char show, char *name);

off_t deepsearch_fat32(char *name, uint32_t position);

void deepshow_fat32(off_t off);

char *convert_UCS2_ASCII(uint16_t *in, char *out, int size);

void listFile(char *name);




void search(char show, char *name){


#ifdef SEARCH_DEBUG
	depth = 0;
#endif

	switch (detecta_tipo()) {
		case EXT4:

			ext4_get_structure();

			uint32_t inode = _ext4(show, name, 2);

			if (!show) {

				ext4_inode_info(inode);

			} else {

				if (inode == NOT_FOUND) {

					printf("\nError: File not found\n");

				}
			}
			break;

		case FAT32:

			fat32_get_structure();

			_fat32(show, name);

			break;
		case EXT2:
			printf("\nFile System not recognized. (EXT2)\n");
			break;
		case FAT16:
			printf("\nFile System not recognized. (FAT16)\n");
			break;
		case FAT12:
			printf("\nFile System not recognized. (FAT12)\n");
			break;
		case EXT3:
			printf("\nFile System not recognized. (EXT3)\n");
			break;
		default:
			printf("\nFile System not recognized. (UNKNOWN)\n");
			break;
	}

}

void change_attr(int option, char *name, char *new_date) {

	int type = detecta_tipo();

	if (type == EXT4) {

		ext4_get_structure();
		uint32_t inode = _ext4(0, name, 2);

		if (inode == NOT_FOUND) {

			printf("Error: File not found.\n");

		} else {
			uint16_t mode;
			uint64_t inode_loc = ext4.inode.table_loc * ext4.block.size + (ext4.inode.size * (inode - 1));
			lseek(fd, inode_loc, SEEK_SET);

			switch (option) {
				case O_EN_READ_ONLY:
					read(fd, &mode, sizeof mode);
					printf("MODE 0x%X\n",mode);
					mode &= ~((unsigned short) (0x02 | 0x10 | 0x80));
					lseek(fd, -sizeof mode, SEEK_CUR);
					write(fd, &mode, sizeof mode);
                    printf("MODE 0x%X\n",mode);
					printf("The permissions of %s have been edited.\n", name);
					break;
				case O_DIS_READ_ONLY:
					read(fd, &mode, sizeof mode);
					mode |= (unsigned short) (0x02 | 0x10 | 0x80);
					lseek(fd, -sizeof mode, SEEK_CUR);
					write(fd, &mode, sizeof mode);
					printf("The permissions of %s have been edited.\n", name);
					break;
				case O_NEW_DATE:

					if (strlen(new_date) == 8) {

						struct tm tm;
						time_t t = 0;
						memset(&tm, 0, sizeof(struct tm));

						if (strptime(new_date, "%d%m%Y", &tm)) {

							t = (uint32_t) mktime(&tm);
                            lseek(fd, 0x90, SEEK_CUR);
							write(fd, &t, sizeof(uint32_t));

						}else{

                            printf("Error in strptime\n");

						}

                        printf("The creation date of %s has been modified\n", name);

					}else{

                        printf("Incorrect date format\n");

					}
					break;
				default:
					printf("Operation not supported for the current filesystem.\n");
					break;
			}
		}
	} else if (type == FAT32) {

		fat32_get_structure();
		off_t off = deepsearch_fat32(name, 0);

		if (off != NOT_FOUND) { //retrocompatibilidad

			uint8_t attribute;
			lseek(fd, off + 0x0B, SEEK_SET);
			read(fd, &attribute, sizeof attribute);
			lseek(fd, -sizeof(attribute), SEEK_CUR);

			switch (option) {
				case O_EN_READ_ONLY:
					attribute |= 0x01;
					write(fd, &attribute, sizeof attribute);
					printf("The permissions of %s have been edited.\n", name);
					break;
				case O_DIS_READ_ONLY:
					attribute &= ~(0x01);
					write(fd, &attribute, sizeof attribute);
					printf("The permissions of %s have been edited.\n", name);
					break;
				case O_ENABLE_HIDE:
					attribute |= 0x02;
					write(fd, &attribute, sizeof attribute);
					printf("The visibility status of %s has been modified.\n", name);
					break;
				case O_DISABLE_HIDE:
					attribute &= ~(0x02);
					write(fd, &attribute, sizeof attribute);
					printf("The visibility status of %s has been modified.\n", name);
					break;
				case O_NEW_DATE:

					if (strlen(new_date) == 8) {

						struct tm tm;
						memset(&tm, 0, sizeof(struct tm));

						if (strptime(new_date, "%d%m%Y", &tm)) {

							lseek(fd, off + 0x10, SEEK_SET);
							uint16_t date = 0;

							date = (uint16_t) ((tm.tm_mday & 0b11111) | (((tm.tm_mon + 1) & 0b1111) << 5) |
											   (((tm.tm_year + 1900 - 1980) & 0b1111111) << 9));
							write(fd, &date, sizeof date);

						}else{

                            printf("Error in strptime\n");

						}

                        printf("The creation date of %s has been modified\n", name);

					}else{

                        printf("Incorrect date format\n");

					}

					break;
			}
		}else{

			printf("Error: File not found.\n");

		}
	} else {

		switch (type){
			case EXT2:
				printf("\nFile System not recognized. (EXT2)\n");
				break;
			case FAT16:
				printf("\nFile System not recognized. (FAT16)\n");
				break;
			case FAT12:
				printf("\nFile System not recognized. (FAT12)\n");
				break;
			case EXT3:
				printf("\nFile System not recognized. (EXT3)\n");
				break;
			default:
				printf("\nFile System not recognized. (UNKNOWN)\n");
				break;
		}

	}
}

uint32_t _ext4(char show, char *name, uint32_t inode) {
	uint32_t return_value = 0;
	ext4_extent_header header;

	off_t offset = lseek(fd, 0, SEEK_CUR); //Guardar posicion puntero antes de modificarlo

	lseek(fd, (ext4.inode.table_loc * ext4.block.size) + (ext4.inode.size * (inode - 1)) + 0x28, SEEK_SET);

	read(fd, &header, sizeof(header));


	if (show < 2) {

		if (header.eh_depth == 0) {

			return_value = deepsearch_leaf_ext4(name, header.eh_entries);

		} else {

			return_value = deepsearch_tree_ext4(name, header.eh_entries);
		}

		if (return_value > 0 && show == 1) {

			printf("\nFile found! Showing content...\n\n");

			_ext4(2, NULL, return_value);

		}

	} else {

#ifdef SHOW_DEBUG
		printf("\nStart show depth %d\n",header.eh_depth);
#endif
		if (header.eh_depth == 0) {

			deepshow_leaf_ext4(header.eh_entries);

		} else {

			deepshow_tree_ext4(header.eh_entries);

		}

	}

	lseek(fd, offset, SEEK_SET); //Volver a poner el puntero en la posicion inicial para la llamada recursiva

	return return_value;

}

uint32_t deepsearch_tree_ext4(char *name, uint16_t eh_entries) {
	int i;
	ext4_extent_idx *leaf;
	uint32_t return_value = 0;

	leaf = (ext4_extent_idx *) malloc(sizeof(ext4_extent_idx) * eh_entries);

	read(fd, leaf, sizeof(ext4_extent_idx) * eh_entries);

	for (i = 0; i < eh_entries; i++) {

		uint64_t read_64 = 0;
		ext4_extent_header header;

		read_64 = ((read_64 | leaf[i].ei_leaf_hi) << 32) | leaf[i].ei_leaf_lo;

		lseek(fd, read_64 * ext4.block.size, SEEK_SET);

		read(fd, &header, sizeof(header));

		if (header.eh_depth == 0) {

			return_value = deepsearch_leaf_ext4(name, header.eh_entries);

		} else {

			return_value = deepsearch_tree_ext4(name, header.eh_entries);

		}

	}

	return return_value;

}

uint32_t deepsearch_leaf_ext4(char *name, uint16_t eh_entries) {

	unsigned int i, bytes = 0;
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
		bytes += sizeof(dir_entry_2);

		do {

		    if(dir_entry_2.inode > ext4.inode.count){
		        break;
		    }

            size = sizeof(char) * ((dir_entry_2.rec_len > (256 + 8) ? dir_entry_2.name_len : dir_entry_2.rec_len - 8));

			if(dir_entry_2.rec_len < 8){
                break;
			}

			file_name = (char *) calloc(size, size);
			read(fd, file_name, size);
			bytes += size;

#ifdef SEARCH_DEBUG
			listFile(file_name);
#endif

			if (!strcmp(file_name, name) && dir_entry_2.file_type == 1) {

				return dir_entry_2.inode;

			}

			if (strcmp(file_name, ".") != 0
				&& strcmp(file_name, "..") != 0
				&& dir_entry_2.file_type == 2) {

#ifdef SEARCH_DEBUG
				depth++;
#endif

				if ((read_64 = _ext4(0, name, dir_entry_2.inode))) {

					free(file_name);
					free(leaf);

					return (uint32_t) read_64;

				}

#ifdef SEARCH_DEBUG
				depth--;
#endif


			}

			free(file_name);
			read(fd, &dir_entry_2, sizeof(dir_entry_2));
			bytes += sizeof(dir_entry_2);

		} while (dir_entry_2.inode != 0 && (leaf[i].ee_len * ext4.block.size) > bytes);

	}

	free(leaf);

	return NOT_FOUND;

}

void deepshow_tree_ext4(uint16_t eh_entries) {

	int i;
	uint64_t read_64 = 0;
	ext4_extent_header header;
	ext4_extent_idx *leaf;

	leaf = (ext4_extent_idx *) malloc(sizeof(ext4_extent_idx) * eh_entries);

	read(fd, leaf, sizeof(ext4_extent) * eh_entries);

#ifdef SHOW_DEBUG
	printf("\nTree Num Entries %d\n",eh_entries);
#endif

	for (i = 0; i < eh_entries; i++) {

		read_64 = 0;

		read_64 = ((read_64 | leaf[i].ei_leaf_hi) << 32) | leaf[i].ei_leaf_lo;

#ifdef SHOW_DEBUG
		printf("Tree Entry %d Start read position 0x%X\n\n",i,
			   (unsigned int)lseek(fd, read_64 * ext4.block.size, SEEK_SET));
#else
		lseek(fd, read_64 * ext4.block.size, SEEK_SET);
#endif

		read(fd, &header, sizeof(header));

		if (header.eh_depth == 0) {

			deepshow_leaf_ext4(header.eh_entries);

		} else {

			deepshow_tree_ext4(header.eh_entries);

		}


	}

}

void deepshow_leaf_ext4(uint16_t eh_entries) {

	int i;
	char buff = 0;
	uint64_t read_64 = 0;
	uint64_t aux_size = 0;
	ext4_extent *leaf;

	leaf = (ext4_extent *) calloc(eh_entries, sizeof(ext4_extent) * eh_entries);

	read(fd, leaf, sizeof(ext4_extent) * eh_entries);

#ifdef SHOW_DEBUG
	printf("\nLeaf Num Entries %d\n",eh_entries);
#endif

	for (i = 0; i < eh_entries; i++) {

		read_64 = 0;

		if (leaf[i].ee_len > 32768) {
			continue;
		}

		read_64 = ((read_64 | leaf[i].ee_start_hi) << 32) | leaf[i].ee_start_lo;

#ifdef SHOW_DEBUG
		printf("- Leaf entry %d \n- Start read position 0x%X\n- Length %d bytes\n\n",i,
			   (unsigned int)lseek(fd, read_64 * ext4.block.size, SEEK_SET),leaf[i].ee_len * ext4.block.size);
#else
		lseek(fd, read_64 * ext4.block.size, SEEK_SET);
#endif

		aux_size = (leaf[i].ee_len * ext4.block.size);

		for (read_64 = 0; read_64 < aux_size; read_64++) {

			read(fd, &buff, sizeof(char));
			if (buff == 0)break;
			printf("%c", buff);

		}

	}

	free(leaf);

}





off_t _fat32(char show, char *name) {

	off_t off;

	if ((off = deepsearch_fat32(name, 0)) == NOT_FOUND) {

		printf("\nError: File not found\n");
		return NOT_FOUND;

	} else {

		if (show == 1) {

			printf("\nFile found! Showing content...\n\n");

			deepshow_fat32(off);

		} else if (!show) {

			fat32_file_info(off);

		}

	}

	return off;
}

void deepshow_fat32(off_t off) {

	fat32_directory fat32_directory;
	unsigned int i, position = 0;
	char buff = 0;
	uint32_t read_32;

	lseek(fd, off, SEEK_SET);

	memset(&fat32_directory,0, sizeof(fat32_directory));

	read(fd, &fat32_directory, sizeof(fat32_directory));

	position = CLUSTER(fat32_directory.cluster_high, fat32_directory.cluster_low);

	lseek(fd, (fat32.first_cluster + ((fat32.bytes_per_sector * fat32.sectors_per_cluster) * position)), SEEK_SET);

	for (i = 0; i < fat32_directory.size; i++) {

		read(fd, &buff, sizeof(char));

        if (buff == '\0')break;

        printf("%c", buff);

		if (!(i % (fat32.bytes_per_sector * fat32.sectors_per_cluster)) && i) {

			lseek(fd, fat32.fat_location + 8 + (position * 4), SEEK_SET);
			read(fd, &read_32, sizeof read_32);

			if (read_32 > 0xFFFFFF7)break;

			read_32 -= 2;

			lseek(fd, fat32.first_cluster + ((read_32) * (fat32.bytes_per_sector * fat32.sectors_per_cluster)),
				  SEEK_SET);

			position = read_32;

			read(fd, &buff, sizeof(char));

		}

	}
}

void _longname_fat32(unsigned int *limit, char *name, uint8_t num) {

	int i = 0;
	fat32_vfat vfat;
	char out[15];

	for (; i < num && (*limit) < ((fat32.bytes_per_sector * fat32.sectors_per_cluster) / sizeof(fat32_directory)); i++) {

		memset(&vfat, 0, sizeof(vfat));
		read(fd, &vfat, sizeof vfat);
		(*limit)++;

		if(vfat.sequence == 0xE5){
			memset(name, 0, 16);
			continue;
		}

		if (vfat.attribute != 0xF) {
			lseek(fd, -sizeof(vfat), SEEK_CUR);
			return;
		}

		char *tmp = strdup(name);
		strcpy(name, convert_UCS2_ASCII(vfat.name, out, 5));
		strcat(name, convert_UCS2_ASCII(vfat.name2, out, 6));
		strcat(name, convert_UCS2_ASCII(vfat.name3, out, 2));
		strcat(name, tmp);

	}

}

void extract_filename(char *name, char const *short_name, char const *extension){
	int i = 0,j = 0;

	for(; i < 8; i++){

		if(short_name[i] == ' ')break;
		name[i] = short_name[i];

	}
	name[i++] = '.';

	for(; j < 3; j++){

		if(extension[j] == ' ')break;
		name[i] = extension[j];
		i++;

	}

}

off_t deepsearch_fat32(char *name, uint32_t position) {

	unsigned int i;
	fat32_directory fat32_dir;
	fat32_vfat vfat;
	uint32_t read_32;
	off_t return_value;

	char out[15];
	memset(out, 0, 15);

	char final_name[MAX_NAME];

	off_t offset = lseek(fd, 0, SEEK_CUR); //Guardar posicion puntero antes de modificarlo

	lseek(fd, (fat32.first_cluster + ((fat32.bytes_per_sector * fat32.sectors_per_cluster) * position)), SEEK_SET);

	do {

        if((unsigned char) (*final_name) != 0)memset(final_name, 0, MAX_NAME);

		for (i = 0; i < ((fat32.bytes_per_sector * fat32.sectors_per_cluster) / sizeof(fat32_directory)); i++) {

			memset(&fat32_dir, 0, sizeof(fat32_dir));

			read(fd, &fat32_dir, sizeof fat32_dir);

			if ((unsigned char) (*fat32_dir.short_name) == 0xE5){
				if(*final_name != 0)memset(final_name, 0, MAX_NAME);
				continue;
			}

			if ((unsigned char) (*fat32_dir.short_name) == 0)break;

			if (fat32_dir.attribute == 0xF) {

				lseek(fd, -sizeof(fat32_dir), SEEK_CUR);

				memset(&vfat, 0, sizeof(vfat));

				read(fd, &vfat, sizeof vfat);

				if(*final_name != 0)memset(final_name, 0, MAX_NAME);

				strcpy(final_name, convert_UCS2_ASCII(vfat.name, out, 5));
				strcat(final_name, convert_UCS2_ASCII(vfat.name2, out, 6));
				strcat(final_name, convert_UCS2_ASCII(vfat.name3, out, 2));


				if (vfat.sequence & 0x40) {

					_longname_fat32(&i,final_name, (uint8_t) ((vfat.sequence & 0xBF) - 1));

				}


			} else {

				if (fat32_dir.attribute & 0x10 && !(fat32_dir.attribute & 0x2)) {

					if ((unsigned char) (*fat32_dir.short_name) != '.'
						&& (unsigned char) *(fat32_dir.short_name+1) != '.' ) {

						if((unsigned char) (*final_name) == 0){
							extract_filename(final_name,fat32_dir.short_name,fat32_dir.file_extension);
						}
#ifdef SEARCH_DEBUG
                        if(!(fat32_dir.attribute & 0x02)){
                            listFile(final_name);
                        }
#endif

						if (CLUSTER(fat32_dir.cluster_high,
									fat32_dir.cluster_low) != position) {

#ifdef SEARCH_DEBUG
							depth++;
#endif

							if ((return_value = deepsearch_fat32(name, CLUSTER(fat32_dir.cluster_high,
																				fat32_dir.cluster_low))) != NOT_FOUND) {
								return return_value;
							}

							if((unsigned char) (*final_name) != 0)memset(final_name, 0, MAX_NAME);

#ifdef SEARCH_DEBUG
							depth--;
#endif


						}

					}


				} else if (fat32_dir.attribute & 0x20) {

					if((unsigned char) (*final_name) == 0){
						extract_filename(final_name,fat32_dir.short_name,fat32_dir.file_extension);
					}

#ifdef SEARCH_DEBUG
					if(!(fat32_dir.attribute & 0x02)){
						listFile(final_name);
					}


#endif
					if (!strcmp(name, final_name)) {

						lseek(fd, -sizeof(fat32_dir), SEEK_CUR);
						return lseek(fd, 0, SEEK_CUR);

					}

					if((unsigned char) (*final_name) != 0)memset(final_name, 0, MAX_NAME);

				}

			}

		}

		lseek(fd, fat32.fat_location + 8 + (position * 4), SEEK_SET);
		read(fd, &read_32, sizeof read_32);

		if (read_32 > 0xFFFFFF7)break;

		read_32 -= 2;

		lseek(fd, fat32.first_cluster + ((read_32) * (fat32.bytes_per_sector * fat32.sectors_per_cluster)), SEEK_SET);

		position = read_32;

	} while (read_32);

	lseek(fd, offset, SEEK_SET); //Volver a poner el puntero en la posicion inicial para la llamada recursiva

	return NOT_FOUND;

}

char *convert_UCS2_ASCII(uint16_t *in, char *out, int size) {
	int i;

	for (i = 0; i < size; i++) {

		if (in[i] == 0 || in[i] > 0xFF) {
			break;
		}

		out[i] = (char) in[i];
	}

	out[i] = 0;
	return out;
}


#ifdef SEARCH_DEBUG
void listFile(char *name) {
	int i;

//	if (name[0] == '.') return;

	if (depth)
		printf("|");

	for (i = 0; i < depth; i++) {
		printf("  ");

	}
	printf("|-");
	printf("%s",name);
	printf("\n");
}
#endif