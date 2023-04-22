/**
 * @file FAT.h
 * @brief the FAT main file that contains the data structure of the FAT
 */
#ifndef FAT_H
#define FAT_H

#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>

/**
 * @brief the directory data strucutre, which contains the size, block, type other fields
 */
typedef struct directory
{
    char name[32];
    uint32_t size;
    uint16_t firstBlock;
    uint8_t type;
    uint8_t perm;
    time_t mtime;
} directory_entry;

/**
 * @brief the directory node, which acts like a linkedlist node of directories
 */
typedef struct dir_node
{
    directory_entry *dir_entry;
    struct dir_node *next;
} dir_node;

/**
 * @brief the FAT data structure that contains various fields for data usage
 */
typedef struct FAT
{
    char *f_name;
    uint8_t block_num;
    uint32_t block_size;
    uint32_t entry_size;
    uint32_t data_size;
    uint32_t file_num;
    uint32_t free_entries;
    uint16_t *block_arr;
    dir_node *first_dir_node;
    dir_node *last_dir_node;
    // directory block starting index
    uint32_t directory_starting_index;
    // data region starting index
    uint32_t dblock_starting_index;

} FAT;

/**
 * @brief the file data structure that contains fields like file bytes and size
 */
typedef struct file
{
    int block_arr_start;
    uint8_t *file_bytes;
    uint32_t size;
} file;

/**
 * @brief initiate a new directory node
 * @param f_name the file name
 * @param size the size of the file
 * @param firstBlock the first block of the directory
 * @param type the type of the file
 * @param perm the permission level of the file
 * @param time the time the file is created
 * @return a directory node
 */
dir_node *new_directory_node(char *f_name, uint32_t size, uint16_t firstBlock, uint8_t type, uint8_t perm, time_t time);

/**
 * @brief free a certain directory node
 * @param node the node to be freed
 */
void free_directory_node(dir_node *node);

/**
 * @brief make a FAT from the file
 * @param f_name the file name
 * @param block_num  the number of blocks to be created
 * @param block_size the size of the block
 * @return a FAT object
 */
FAT *make_fat(char *f_name, uint8_t block_num, uint8_t block_size);

// void free_fat(FAT* fat);
/**
 * @brief the function to mount the FAT for a file
 * @param f_name the file name
 * @return the FAT mounted
 */
FAT *mount_fat(char *f_name);

/**
 * @brief free a certain FAT object
 * @param fat the FAT to be freed
 */
void free_fat(FAT *fat);

/**
 * @brief delete a directory from a block
 * @param en the directory to be removed
 * @param fat the FAT to remove from
 * @return an int to indicate whether successful
 */
int delete_directory_from_block(directory_entry en, FAT *fat);

/**
 * @brief write a directory to a block
 * @param en the directory node
 * @param fat the FAT to write
 * @param reside_block the block the directory would reside on
 * @return an int to indicate whether successful
 */
int write_directory_to_block(directory_entry *en, FAT *fat, int *reside_block);

/**
 * @brief read file from FAT
 * @param f_node the file node to read
 * @param fat the FAT to read from
 * @return the file
 */
file *read_file_from_fat(dir_node *f_node, FAT *fat);

// void write_file_to_fat()

/**
 * @brief read file bytes from FAT
 * @param start_index the start index of the file
 * @param length the length to read
 * @param fat the FAT to read from
 * @return the file bytes
 */
uint8_t *read_file_bytes(uint32_t start_index, uint32_t length, FAT *fat);

/**
 * @brief delete file bytes from FAT
 * @param start_index the start index of the file
 * @param length the length to delete
 * @param fat the FAT to delete from
 * @return the number of file bytes deleted
 */
int delete_file_bytes(uint32_t startIndex, uint32_t length, FAT *fat);

/**
 * @brief free the file
 * @param file the file to be freed
 * @return an int to indicate successful or not
 */
int free_file(file *file);

/**
 * @brief search for the file in the FAT
 * @param file_name the file name
 * @param fat FAT to search in
 * @param prev the previous node
 * @return the directory node found
 */
dir_node *search_file(char *file_name, FAT *fat, dir_node **prev);

// FAT *mount_fat(char *f_name);
#endif