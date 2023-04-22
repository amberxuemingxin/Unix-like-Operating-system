/**
 * @file pennfatlib.h
 * @brief the PennFAT header file, includes all PennFAT functions and methods
 */

#ifndef PENNFATLIB_H
#define PENNFATLIB_H
#include <stdarg.h>

#include "FAT.h"
/**
 * @brief parse given commands and see if it matches any library calls
 * @param commands the commands that are passed in
 * @param commandCount the number of commands
 * @return an int to indicate whether the parse is successful or not
 */
int parse_pennfat_command(char ***commands, int commandCount);

/**
 * @brief the method to make a fat system
 * @param f_name that name of the file system
 * @param numBlocks number of intended blocks
 * @param blockSizeIndicator the size of the block
 * @param fat the FAT the user wishes to use
 * @return an int to indicate whether successful or not
 */
int pennfat_mkfs(char *f_name, uint8_t numBlocks, uint8_t blockSizeIndicator, FAT **fat);

/**
 * @brief mount the PennFAT to the name provided
 * @param f_name the file name to be mounted
 * @return a FAT structure
 */
FAT *pennfat_mount(char *f_name);

/**
 * @brief unmount the provided FAT
 * @param fat the FAT user wishes to unmount
 * @return an int to indicate whether successful or not
 */
int pennfat_unmount(FAT *fat);

/**
 * @brief touch a new file
 * @param f_name the file name the user would like to touch
 * @return an int to indicate whether successful or not
 */
int pennfat_touch(char **f_name);

/**
 * @brief rename the file
 * @param old_name the old name of the file
 * @param new_name the new name of the file
 * @return an int to indicate whether successful or not
 */
int pennfat_mv(char *old_name, char *new_name);

/**
 * @brief remove the file
 * @param files the files user wishes to remove
 * @return an int to indicate whether successful or not
 */
int pennfat_remove(char **files);

/**
 * @brief reads from the terminal and output
 * @param commands the commands to be executed
 * @param fd0 the file descriptor to be read from
 * @param fd1 the file descriptor to write to
 * @return an int to indicate whether successful or not
 */
int pennfat_cat(char **commands, int *fd0, int *fd1);

/**
 * @brief copy from source to destination
 * @param commands include the source and destination arguments
 * @return an int to indicate whether successful or not
 */
int pennfat_cp(char **commands);

/**
 * @brief list all files in the directory
 * @return an int to indicate whether successful or not
 */
int pennfat_ls();

/**
 * @brief flag to change mode
 * @param commands the arguments for mode change
 * @return an int to indicate whether successful or not
 */
int pennfat_chmod(char **commands);

/**
 * @brief update the file descriptor
 */
void pennfat_update_fd();

/**
 * @brief search for the exact file in the FAT
 * @param file_name the file name to be searched for
 * @param fat the FAT the user wish to search in
 * @param prev the node to start
 * @return a directory node
 */
dir_node *search_file(char *file_name, FAT *fat, dir_node **prev);

/**
 * @brief given a file name, it traverses all directory and return the
 * number of block the entry with the same name resides in
 * @param f_name the file name
 * @return an int to indicate whether successful or not
 */
int find_entry_block(char *f_name);

/**
 * @brief save the file descriptor
 * @param f_name the file name
 * @param file_d_size the size of the file descriptor
 * @param file_d the file descriptor
 * @param file_pos the position of the fiel
 */
void save_fds(char *f_name, int file_d_size, int *file_d, int *file_pos);

/**
 * @brief load the file descriptor
 * @param file_path the path to the file
 */
void load_fds(const char *file_path);

/**
 * @brief if the file is executable
 * @param f_name the file name
 * @return a boolean whether the file is executable
 */
bool is_file_executable(char *f_name);

/**
 * @brief open the file
 * @param f_name the file name
 * @param mode mode of read/write
 * @return an int to indicate whether successful or not
 */
int f_open(const char *f_name, int mode);

/**
 * @brief read from the file
 * @param fd the file descriptor
 * @param n the number of bits to read
 * @param buf the buffer to read into
 * @return the number of bits read
 */
int f_read(int fd, int n, char *buf);

/**
 * @brief wrtie to the file
 * @param fd the file descriptor
 * @param content the content to write
 * @para"""""KMm n the number of bits to write
 */
int f_write(int fd, const char *content, int n, ...);

/**
 * @brief close the file
 * @param fd the file descriptor to be closed
 * @return an int to indicate whether successful or not
 */
int f_close(int fd);

/**
 * @brief seek to the offset
 * @param fd the fiel descriptor
 * @param offset the offset amount to be sought
 * @param whence
 * @return an int to indicate whether successful or not
 */
int f_lseek(int fd, int offset, int whence);

/**
 * @brief find the entry block
 * @param f_name file name
 * @return an int to indicate whether successful or not
 */
int find_entry_block(char *f_name);

/**
 * @brief save the fds
 */
void os_savefds();

/**
 * @brief get the file content from the fd
 * @param fd the file descriptor
 * @return the chars read
 */
char *get_file_content(int fd);

#endif