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

	int i,j;
	ext4_extent_header header;
	ext4_extent leaf[4];
	ext4_extent_idx tree[4];
	ext4_dir_entry_2 dir_entry_2;
	char file_name [256];

	memset(file_name,0,256);
	ext4_get_structure(fd);
	
	uint16_t read_16 = 0;
	uint8_t read_8 = 0;
	uint32_t read_32 = 0;
	uint64_t read_64 = 0;

	read_with_offset(fd, ext4.block.size + ext4.inode.table_loc * ext4.block.size + ext4.inode.size , &read_16, sizeof(read_16));
	printf("i_mode: 0x%X\n",read_16);
	read(fd,&read_16, sizeof(read_16));
	read(fd,&read_16, sizeof(read_16));
	printf("i_size_lo: %d\n",read_16);

	lseek(fd,0x22,SEEK_CUR);
	read(fd,&header, sizeof(header));
	printf("Magic number: 0x%X\n",header.eh_magic);
	printf("Entries: 0x%X\n",header.eh_entries);
	printf("Depth: 0x%X\n",header.eh_depth);
	printf("Max: 0x%X\n\n",header.eh_max);

	read(fd,leaf, sizeof(ext4_extent) * header.eh_entries);
	for(i = 0; i < header.eh_entries;i++){
		printf("Block: 0x%X\n",leaf[i].ee_block);
		printf("Length: 0x%X\n",leaf[i].ee_len);
		printf("StartHi: 0x%X\n",leaf[i].ee_start_hi);
		printf("StartLo: 0x%X\n",leaf[i].ee_start_lo);
	}

	printf("\n");

	read_64 = ((read_64 | leaf[0].ee_start_hi) << 32) | leaf[0].ee_start_lo;
	printf("Size: %lu\n",ext4.block.size * (read_64));

	lseek(fd,1024 + 0x42e,SEEK_SET);
	read(fd,&dir_entry_2, sizeof(dir_entry_2));
	read(fd,file_name, sizeof(char) * dir_entry_2.name_len+3);

	printf("\n---- Points to1 -----\nInode: 0x%X\n",dir_entry_2.inode );
	printf("Rec_len: 0x%X\n",dir_entry_2.rec_len);
	printf("name_len: 0x%X\n",dir_entry_2.name_len);
	printf("file_type: 0x%X\n",dir_entry_2.file_type);
	printf("name: %s\n",file_name);

	memset(file_name,0,256);

	/*read(fd,&dir_entry_2, sizeof(dir_entry_2));
	read(fd,file_name, sizeof(char) * dir_entry_2.name_len+2);

	printf("\n---- Points to2 -----\nInode: 0x%X\n",dir_entry_2.inode,32);
	printf("Rec_len: 0x%X\n",dir_entry_2.rec_len);
	printf("name_len: 0x%X\n",dir_entry_2.name_len);
	printf("file_type: 0x%X\n",dir_entry_2.file_type);
	printf("name: %s\n",file_name);

	memset(file_name,0,256);

	read(fd,&dir_entry_2, sizeof(dir_entry_2));
	read(fd,file_name, sizeof(char) * dir_entry_2.name_len+2);

	printf("\n---- Points to2 -----\nInode: 0x%X\n",dir_entry_2.inode,32);
	printf("Rec_len: 0x%X\n",dir_entry_2.rec_len);
	printf("name_len: 0x%X\n",dir_entry_2.name_len);
	printf("file_type: 0x%X\n",dir_entry_2.file_type);
	printf("name: %s\n",file_name);*/

}

void _search_fat32(int fd, char *name) {

}