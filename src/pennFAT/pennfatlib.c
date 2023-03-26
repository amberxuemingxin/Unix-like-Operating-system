#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <limits.h>

#include "pennfatlib.h"
#include "macro.h"

int parse_pennfat_command(char ***commands, int commandCount, FAT **FAT){
    char* cmd = commands[0][0];

    if(strcmp(cmd, "mkfs") == 0) {
        if (commands[0][1] == NULL || commands[0][2] == NULL || commands[0][3] == NULL) {
            printf("insuffcient arguement\n");
            return FAILURE;
        }
        return pennfat_mkfs(commands[0][1], (char) atoi(commands[0][2]), (char) atoi(commands[0][3]), FAT);
    } else if (strcmp(cmd, "mount") == 0) {
        return pennfat_mount(commands[0][1], FAT);
    } else if (*FAT == NULL) {
        printf("No filesystem mounted\n");
        return FAILURE;
    } else if (strcmp(cmd, "umount") == 0) {
        return pennfat_unmount(FAT);
    } else if (strcmp(cmd, "touch") == 0) {
        return pennfat_touch(commands[0], *FAT);
    } else if (strcmp(cmd, "mv") == 0) {
        return pennfat_mv(commands[0][1], commands[0][2], *FAT);
    } else if (strcmp(cmd, "rm") == 0) {
        return pennfat_remove(commands[0], *FAT);
    }  else if (strcmp(cmd, "cat") == 0) {
        return pennfat_cat(commands[0], *FAT);
    } else if (strcmp(cmd, "cp") == 0) {
        return pennfat_cp(commands[0], *FAT);
    } else if (strcmp(cmd, "ls") == 0) {
        return pennfat_ls(*FAT);
    } else if (strcmp(cmd, "chmod") == 0) {
        return pennfat_chmod(commands[0], *FAT);
    } else if (strcmp(cmd, "describe") == 0) {
        printf("File system name : %s\n", (*FAT)->f_name);
        printf("Number of block in the filesystem : %d\n", (*FAT)->block_num);
        printf("Block size : %d\n", (*FAT)->block_size);
        printf("Number of entries: %d\n", (*FAT)->entry_num);
        printf("number of files : %d\n", (*FAT)->file_num);
        printf("available blocks: %d\n", (*FAT)->free_blocks);
    }
    
     else {
        printf("unknown command");
    }

    return FAILURE;
}

int pennfat_mkfs(char *f_name, uint8_t block_num, uint8_t block_size, FAT **FAT){
    printf("making a new file system, file name is  %s, %d number of blocks, with %d block size\n", f_name, block_num, block_size);
    // if FAT exist, we need to overwrite the origianl FAT
    if (FAT != NULL) {
        free_fat(FAT);
    }
    *FAT = make_fat(f_name, block_num, block_size);

    return SUCCESS;
}

int pennfat_mount(char *fileName, FAT **fat) {
    printf("this is mount\n");
    return 1;
}

int pennfat_unmount(FAT **fat){
    printf("this is unmount\n");
    return 1;
}

int pennfat_touch(char **files, FAT *fat){
    printf("this is touch\n");
    return 1;
}

int pennfat_mv(char *oldFileName, char *newFileName, FAT *fat){
    printf("this is mv\n");
    return 1;
}

int pennfat_remove(char **files, FAT *fat){
    printf("this is rm\n");
    return 1;
}

int pennfat_cat(char **commands, FAT *fat){
    printf("this is cat\n");
    return 1;
}

int pennfat_cp(char **commands, FAT *fat){
    printf("this is cp\n");
    return 1;
}

int pennfat_ls(FAT *fat){
    return 1;
}

int pennfat_chmod(char **commands, FAT *fat){
    return 1;
}