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
    uint16_t* block_arr;
    dir_node* first_dir_node;
    dir_node* last_dir_node;
    // directory block starting index
    uint32_t directory_starting_index;
    // data region starting index
    uint32_t dblock_starting_index;


}FAT;

typedef struct file {
    int block_arr_start;
    uint8_t* file_bytes;
} file;

dir_node * new_directory_node(char* f_name, uint32_t size, uint16_t firstBlock, uint8_t type, uint8_t perm, time_t time);

void free_directory_node(dir_node *node);


FAT* make_fat(char* f_name, uint8_t num_blocks, uint8_t block_size);

// void free_fat(FAT* fat);

FAT* mount_fat(char* f_name);

void free_fat(FAT* fat);

int delete_directory_from_block(directory_entry en, FAT* fat);

int write_directory_to_block(directory_entry en, FAT* fat);

file* read_file_from_fat(dir_node* f_node, FAT* fat);

// void write_file_to_fat()

uint8_t *read_file_bytes(uint16_t start_index, uint32_t length, FAT* fat); 

void delete_file_bytes(uint16_t startIndex, uint32_t length, FAT *fat);

int free_file(file* file);

dir_node* search_file(char* file_name, FAT* fat, dir_node** prev);


// FAT *mount_fat(char *f_name);
#endif