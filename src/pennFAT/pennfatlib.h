#ifndef PENNFATLIB_H
#define PENNFATLIB_H
#include <stdarg.h>

#include "FAT.h"

int parse_pennfat_command(char ***commands, int commandCount);

int pennfat_mkfs(char *f_name, uint8_t numBlocks, uint8_t blockSizeIndicator, FAT **fat);

FAT* pennfat_mount(char *f_name);

int pennfat_unmount(FAT *fat);

int pennfat_touch(char **f_name);

int pennfat_mv(char *old_name, char *new_name);

int pennfat_remove(char **files);

int pennfat_cat(char **commands);

int pennfat_cp(char **commands);

int pennfat_ls();

int pennfat_chmod(char **commands);

void pennfat_update_fd();

dir_node* search_file(char* file_name, FAT* fat, dir_node** prev);

int find_entry_block(char* f_name);


void save_fds(char *f_name, int file_d_size, int *file_d, int *file_pos);

void load_fds(const char* file_path);

void os_updatefds();

int f_open(const char *f_name, int mode);

int f_read(int fd, int n, char *buf);

int f_write(int fd, const char *content, int n,...);

int f_close(int fd);

int f_lseek(int fd, int offset, int whence);

int find_entry_block(char* f_name);


#endif