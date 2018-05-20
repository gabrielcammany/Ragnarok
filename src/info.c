//
// Created by gabriel on 10/04/18.
//


#include "../include/info.h"
#include "../include/search.h"

const char *const NAMES[] = {"EXT2", "EXT3", "EXT4", "FAT12", "FAT16", "FAT32", "UNKNOWN"};

void info() {
    int type = detecta_tipo();

    switch (type) {
        case EXT4:
            ext4_info();
            break;
        case FAT32:
            fat32_info();
            break;
        default:
            printf(ERR_FILESYSTEM, NAMES[type]);
            break;
    }

}

int detecta_tipo() {

    lseek(fd, EXT4_OFFSET + 0x38, SEEK_SET);
    uint16_t magic_number;
    read(fd, &magic_number, sizeof magic_number);

    if (magic_number == 0xEF53) {

        lseek(fd, EXT4_OFFSET + 0x60, SEEK_SET);
        uint32_t feature_compat;
        read(fd, &feature_compat, sizeof feature_compat);

        if (feature_compat & 0x40) {

            return EXT4;

        } else {

            lseek(fd, EXT4_OFFSET + 0x5C, SEEK_SET);
            read(fd, &feature_compat, sizeof feature_compat);

            if (feature_compat & 0x04) {

                return EXT3;

            } else {

                return EXT2;

            }
        }
    } else {
        //could possibly be a FAT filesystem
        uint16_t read_16;

        lseek(fd, 0x11, SEEK_SET);
        read(fd, &read_16, sizeof(uint16_t));

        if (!read_16) {

            return FAT32;

        } else {

            return FAT16;

        }

        /*
        char fat_type[8];

        if (strcmp(read_at(0x36, fat_type), FAT12_NAME) == 0) {

            return FAT12;

        } else if (strcmp(fat_type, FAT16_NAME) == 0) {

            return FAT16;

        } else if (strcmp(read_at(0x52, fat_type), FAT32_NAME) == 0) {

            return FAT32;

        }*/
    }
    return UNKNOWN;
}

void ext4_info() {

    uint32_t read = 0;
    uint32_t second_read = 0;

    printf("---- Filesystem Information ----\n\n");
    printf("FileSystem: EXT4\n\n");

    printf("INODE INFO\n");

    read_with_offset(0x58, &read, sizeof(read) / 2);
    printf("Inode Size: %d\n", read);

    read_with_offset(0x0, &read, sizeof read);
    printf("Number of inodes: %d\n", read);

    read_with_offset(0x54, &read, sizeof read);
    printf("First inode: %d\n", read);

    read_with_offset(0x28, &read, sizeof read);
    printf("Inodes group: %d\n", read);

    read_with_offset(0x10, &read, sizeof read);
    printf("Free inodes: %d\n\n", read);

    printf("BLOCK INFO\n");
    read_with_offset(0x18, &read, sizeof(read));
    printf("Block Size: %0.f\n", pow(2, 10 + read));

    read_with_offset(0x0154, &read, sizeof read); //high
    read_with_offset(0x08, &second_read, sizeof second_read); //low
    printf("Reserved blocks: %lu\n", (unsigned long) ((((uint64_t) read) << 32) | second_read));

    read_with_offset(0x0C, &read, sizeof read); //low
    read_with_offset(0x158, &second_read, sizeof second_read); //high
    printf("Free Blocks: %lu\n", (unsigned long) ((((uint64_t) second_read) << 32) | read));

    read_with_offset(0x04, &read, sizeof read);
    printf("Total Blocks: %d\n", read);

    read_with_offset(0x20, &read, sizeof read);
    printf("Block Group: %d\n\n", read);

    read_with_offset(0x24, &read, sizeof read);
    printf("Frags Group: %d\n\n", read);

    char lectura[17];
    read_with_offset(0x78, lectura, sizeof(char) * 16);
    lectura[16] = 0;

    printf("VOLUME INFO\n");
    printf("Volume name: %s\n", lectura); //0x78 - 16 bytes

    read_with_offset(0x40, &read, sizeof read);
    time_t t = read;

    printf("Last check: %s", ctime(&t));//0x40 tiempo desde epoch 32 bits

    read_with_offset(0x2C, &read, sizeof read);
    t = read;
    printf("Last mount: %s", ctime(&t)); //0x2C tiempo epoch 32bits

    read_with_offset(0x30, &read, sizeof read);
    t = read;
    printf("Last written: %s", ctime(&t)); //0x30 tiempo lo mismo


}

void read_with_offset(unsigned long offset, void *out, size_t size) {
    lseek(fd, EXT4_OFFSET + offset, SEEK_SET);
    read(fd, out, size);
}

void fat32_read(unsigned int offset, void *out, size_t size) {
    lseek(fd, offset, SEEK_SET);
    read(fd, out, size);
}

void fat32_info() {
    char name[12];
    uint32_t big_value;
    uint16_t value;
    uint8_t small_value;

    printf("---- Filesystem Information ----\n\n");
    printf("FileSystem: FAT32\n\n");

    fat32_read(0x03, name, sizeof(char) * 8);
    name[8] = 0;
    printf("System name: %s\n", name);

    fat32_read(0x0B, &value, sizeof(value));
    printf("Sector Size: %d\n", value);

    read(fd, &small_value, sizeof(small_value));
    printf("Sectors per Cluster: %d\n", small_value);

    read(fd, &value, sizeof(value));
    printf("Reserved Sectors: %d\n", value);

    read(fd, &small_value, sizeof(small_value));
    printf("Number of FATs: %d\n", small_value);

    fat32_read(0x24, &big_value, sizeof(big_value));
    printf("Maximum Root Entries: %d\n", big_value);

    fat32_read(0x16, &value, sizeof(value));
    printf("Sectors per FAT: %d\n", value);

    fat32_read(0x47, name, sizeof(char) * 11);
    name[11] = 0;
    printf("Label: %s\n\n", name);
}

//Pre: out es mínimo un array de 6 caracteres
char *read_at(unsigned int offset, char *out) {
    lseek(fd, offset, SEEK_SET);
    read(fd, out, sizeof out);
    out[5] = 0;
    return out;
}

void ext4_get_structure() {


    read_with_offset(0x18, &(ext4.block.size), sizeof(uint32_t));
    ext4.block.size = (uint32_t) pow(2, 10 + ext4.block.size);

    uint32_t low;
    uint32_t high;

    if (ext4.block.size > EXT4_OFFSET) {

        //low
        lseek(fd, ext4.block.size + 0x08, SEEK_SET);
        read(fd, &(low), sizeof(low));


        //high
        lseek(fd, ext4.block.size + 0x28, SEEK_SET);
        read(fd, &(high), sizeof(high));

    } else {

        //low
        read_with_offset(ext4.block.size + 0x08, &low, sizeof low);


        //high
        read_with_offset(ext4.block.size + 0x28, &high, sizeof high);

    }

    ext4.inode.table_loc = (((uint64_t) (high)) << 32) | low;

    read_with_offset(0x58, &(ext4.inode.size), sizeof(uint16_t));

    read_with_offset(0xFE, &(ext4.group_descriptor_size), sizeof(uint16_t));

    read_with_offset(0x00, &(ext4.inode.count), sizeof(uint32_t));

    read_with_offset(0x10, &(ext4.inode.free_count), sizeof(uint32_t));

    read_with_offset(0xC, &(ext4.block.free_count), sizeof(uint32_t));


}

void fat32_get_structure() {
    memset(&fat32, 0, sizeof fat32);
    fat32_read(0x0B, &(fat32.bytes_per_sector), sizeof fat32.bytes_per_sector);
    fat32_read(0x0D, &(fat32.sectors_per_cluster), sizeof fat32.sectors_per_cluster);
    fat32_read(0x0E, &(fat32.reserved_sectors), sizeof fat32.reserved_sectors);
    fat32_read(0x24, &(fat32.sectors_per_fat), sizeof fat32.sectors_per_fat);
    fat32_read(0x2C, &(fat32.root_first_cluster), sizeof fat32.root_first_cluster);

    fat32.fat_location = (uint32_t) (fat32.bytes_per_sector * (fat32.reserved_sectors));

    fat32.first_cluster = fat32.fat_location + fat32.sectors_per_fat * fat32.bytes_per_sector * 2;
}


void fat32_file_info(off_t off) {

    fat32_directory fat32_directory;
    uint32_t year, month, day;

    lseek(fd, off, SEEK_SET);
    read(fd, &fat32_directory, sizeof(fat32_directory));

    if (fat32_directory.attribute & 0x2) {

        printf("\nError: File not found\n");

    } else {

        printf("\nFile Found! Size: %d bytes.\t", fat32_directory.size);

        year = (uint32_t) 1980 + ((fat32_directory.created_date & 0xFE00) >> 9);
        month = (uint32_t) (fat32_directory.created_date & 0x1E0) >> 5;
        day = (uint32_t) (fat32_directory.created_date & 0x1F);
        printf("Created on: %.2d/%.2d/%d\n", day, month, year);

    }

}


void ext4_inode_info(uint32_t inode) {

    if (!inode) {
        printf("\nError: File not found\n");
        return;
    }

    off_t offset = lseek(fd, 0, SEEK_CUR);

    uint32_t read_32 = 0;
    uint32_t read_64 = 0;

    lseek(fd, ext4.inode.table_loc * ext4.block.size + (ext4.inode.size * (inode - 1)), SEEK_SET);

    lseek(fd, 0x6C, SEEK_CUR);
    read(fd, &read_32, sizeof(read_32));
    read_64 = ((read_64 | read_32 << 32));

    lseek(fd, -0x6C, SEEK_CUR);
    read(fd, &read_32, sizeof(read_32));
    read_64 = ((read_64 | read_32));

    printf("\nFile Found! Size: %d bytes.\t", read_64);

    lseek(fd, 0x88, SEEK_CUR);
    read(fd, &read_32, sizeof(read_32));

    time_t t = read_32;
    struct tm *tm_info = localtime(&t);
    printf("Created on: %.2d/%.2d/%d\n", tm_info->tm_mday, tm_info->tm_mon, 1900 + tm_info->tm_year);


    lseek(fd, offset, SEEK_SET);

}