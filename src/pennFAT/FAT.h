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
} directoryEntry;

typedef struct dir_node {
    directoryEntry *entry;
    struct dir_node* next;
} dir_node;

typedef struct FAT {
    char* f_name;
    uint8_t block_num;
    uint32_t block_size;
    uint32_t entry_num;
    uint32_t file_num;
    uint32_t free_blocks;
    dir_node* first_dir_node;
    dir_node* last_dir_node;
}FAT;

dir_node * NewDirNode(
    char* f_name,
    uint32_t size,
    uint16_t firstBlock,
    uint8_t type,
    uint8_t perm,
    time_t time
);

void free_dir_node(dir_node *node);


FAT* make_fat(char* f_name, uint8_t num_blocks, uint8_t block_size, bool new);

void free_fat(FAT** fat);
#endif