#ifndef PENNFATLIB_H
#define PENNFATLIB_H

#include "FAT.h"

int parse_pennfat_command(char ***commands, int commandCount);

int pennfat_mkfs(char *f_name, uint8_t numBlocks, uint8_t blockSizeIndicator, FAT **fat);

FAT* pennfat_mount(char *f_name);

int pennfat_unmount(FAT *fat);

int pennfat_touch(char **f_name, FAT *fat);

int pennfat_mv(char *old_name, char *new_name, FAT *fat);

int pennfat_remove(char **files, FAT *fat);

int pennfat_cat(char **commands, FAT *fat);

int pennfat_cp(char **commands, FAT *fat);

int pennfat_ls(FAT *fat);

int pennfat_chmod(char **commands, FAT *fat);

dir_node* search_file(char* file_name, FAT* fat, dir_node** prev);

int find_entry_block(char* f_name);





int f_open(const char *f_name, int mode);

int f_read(int fd, int n, char *buf);

int f_write(int fd, const char *str, int n);

int f_close(int fd);

int f_lseek(int fd, int offset, int whence);

int find_entry_block(char* f_name);

#endif