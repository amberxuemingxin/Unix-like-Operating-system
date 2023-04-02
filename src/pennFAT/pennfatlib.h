#ifndef PENNFATLIB_H
#define PENNFATLIB_H

#include "FAT.h"

int parse_pennfat_command(char ***commands, int commandCount, FAT **fat);

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

#endif