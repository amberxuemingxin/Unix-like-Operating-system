#ifndef FAT_H
#define FAT_H

#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>



typedef struct directory {
    char name[32];
    uint32_t size;
    uint16_t firstBlock;
    uint8_t type;
    uint8_t perm;
    time_t mtime; 
} directory_entry;

typedef struct dir_node {
    directory_entry *dir_entry;
    struct dir_node* next;
} dir_node;

typedef struct FAT {
    char* f_name;
    uint8_t block_num;
    uint32_t block_size;
    uint32_t entry_size;
    uint32_t data_size;
    uint32_t file_num;
    uint32_t free_entries;
    // dir_node* first_dir_node;
    // dir_node* last_dir_node;
    uint16_t* entry_arr;
    uint16_t* data_arr;
}FAT;

dir_node * new_directory_node(char* f_name, uint32_t size, uint16_t firstBlock, uint8_t type, uint8_t perm, time_t time);

void free_directory_node(dir_node *node);


FAT* make_fat(char* f_name, uint8_t num_blocks, uint8_t block_size);

FAT* mount_fat(char* f_name);

void free_fat(FAT* fat);

// int saveFat(FAT *fat);

// FAT *mount_fat(char *f_name);
#endif