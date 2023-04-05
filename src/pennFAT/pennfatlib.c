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
#include "FAT.h"
// #include "file.h"

int curr_fd = -1;
FAT* curr_fat;
int parse_pennfat_command(char ***commands, int commandCount, FAT **fat){
    char* cmd = commands[0][0];

    if(strcmp(cmd, "mkfs") == 0) {
        if (commands[0][1] == NULL || commands[0][2] == NULL || commands[0][3] == NULL) {
            printf("insuffcient arguement\n");
            return FAILURE;
        }
        return pennfat_mkfs(commands[0][1], (char) atoi(commands[0][2]), (char) atoi(commands[0][3]), fat);
    } else if (strcmp(cmd, "mount") == 0) {
        *fat = pennfat_mount(commands[0][1]);
        if (*fat == NULL) return FAILURE;
        return SUCCESS;
    } else if (*fat == NULL) {
        printf("No filesystem mounted yet\n");
        return FAILURE;
    } else if (strcmp(cmd, "umount") == 0) {
        free(*fat);
        *fat = NULL;
        curr_fat = NULL;
        return SUCCESS;
    } else if (strcmp(cmd, "touch") == 0) {
        return pennfat_touch(commands[0], *fat);
    } else if (strcmp(cmd, "mv") == 0) {
        return pennfat_mv(commands[0][1], commands[0][2], *fat);
    } else if (strcmp(cmd, "rm") == 0) {
        return pennfat_remove(commands[0], *fat);
    }  else if (strcmp(cmd, "cat") == 0) {
        return pennfat_cat(commands[0], *fat);
    } else if (strcmp(cmd, "cp") == 0) {
        return pennfat_cp(commands[0], *fat);
    } else if (strcmp(cmd, "ls") == 0) {
        return pennfat_ls(*fat);
    } else if (strcmp(cmd, "chmod") == 0) {
        return pennfat_chmod(commands[0], *fat);
    } else if (strcmp(cmd, "describe") == 0) {
        printf("File system name : %s\n", (*fat)->f_name);
        printf("Number of block in the filesystem : %d\n", (*fat)->block_num);
        printf("Block size : %d\n", (*fat)->block_size);
        printf("Number of entries: %d\n", (*fat)->entry_size);
        printf("number of files : %d\n", (*fat)->file_num);
        printf("available blocks: %d\n", (*fat)->free_entries);
    }
    
     else {
        printf("unknown command\n");
    }

    return FAILURE;
}

int pennfat_mkfs(char *f_name, uint8_t block_num, uint8_t block_size, FAT **fat){
    printf("making a new file system, file name is  %s, %d number of blocks, with %d block size\n", f_name, block_num, block_size);
    // if FAT exist, we need to overwrite the origianl FAT
    if (fat != NULL) {
        free_fat(*fat);
    }
    *fat = make_fat(f_name, block_num, block_size);
    curr_fat = *fat;
    return SUCCESS;
}

FAT* pennfat_mount(char *f_name) {
    if (f_name == NULL) {
        printf("no filename, please enter a filename\n");
        return NULL;
    }
    FAT* fat = mount_fat(f_name);
    curr_fat = fat;

    return fat;    
}

int pennfat_touch(char **files, FAT *fat){
    if (files[1] == NULL) {
        printf("insuffcient arguement\n");
        return FAILURE;
    }
    int index = 1;

    char *file_name = files[index];
    while (file_name != NULL) {
        dir_node* file_node = search_file(file_name, fat, NULL);
        if (file_node != NULL){    // the file already exists, updates timestamp
                file_node->dir_entry->mtime = time(0);
                index += 1;
                file_name = files[index];
                continue;
            }
        int fd = f_open(file_name, F_WRITE);
        if(fd == -1) {
            printf("open error\n");
        } else {
            printf("open successful\n");
        }
        char arr1[5] = "abcde";
        int byte_write = f_write(fd, arr1, 5);
        printf("byte write: %d\n", byte_write);
        char arr2[5];
        int byte_read = f_read(fd, 5, arr2);
        printf("byte read: %d, value: %s\n", byte_read, arr2);
        f_close(fd);
        index += 1;
        file_name = files[index];
    }

    return SUCCESS;
}

int pennfat_mv(char *oldFileName, char *newFileName, FAT *fat){
    printf("this is mv\n");
    return 1;
}

int pennfat_remove(char **commands, FAT *fat){
    int index = 1;
    int count = 0;
    while (commands[count] != NULL) {
        count++;
    }
    if (count < 2) {
        printf("insuffcient arguemtn\n");
        return FAILURE;
    }
    index = 1;
    char *file_name = commands[index];
    while (file_name != NULL) {
        dir_node* prev_node;
        dir_node* filenode = search_file(file_name, fat, &prev_node);
        if (filenode != NULL){    
            fat->file_num -= 1;
            index += 1;
            file_name = commands[index];
            if (filenode == fat->first_dir_node) {
                fat->first_dir_node = filenode->next;
            }
            else{
                prev_node->next = filenode->next;
            }

            //TODO: REMOVE FILE FROM DIRECTORY BLOCK
            delete_directory_from_block(*filenode->dir_entry, fat);
            // set last node pointer to the prev entry if this entry is the last entry
            if (filenode == fat->last_dir_node)
                fat->last_dir_node = prev_node;
        }else {
            printf("%s file not found", file_name);
            index += 1;
        }
    }
    return SUCCESS;
    
}

int pennfat_cat(char **commands, FAT *fat){
    return 1;
}

int pennfat_cp(char **commands, FAT *fat){
    printf("this is cp\n");
    return 1;
}

int pennfat_ls(FAT *fat){
    dir_node *node = fat->first_dir_node;

    while (node != NULL) {
        directory_entry *entry = node->dir_entry;
        char *perms;
        switch(entry->perm) {
            case(NO_PERMS)       : perms = "--"; break;
            case(READ_PERMS)     : perms = "r-"; break;
            case(WRITE_PERMS)    : perms = "-w"; break;
            case(READ_WRITE_EXCUTABLE): perms = "rw"; break;
        }

        struct tm *localTime = localtime(&entry->mtime);
        
        char month[4];
        char day[3];
        char time[6];

        strftime(month, 4, "%b", localTime);
        strftime(day, 3, "%d", localTime);
        strftime(time, 6, "%H:%M", localTime);


 
        printf("%2s%6db%4s%3s%6s %s\n", perms, entry->size, month, day, time, entry->name);

        node = node->next;
    }

    return SUCCESS;
}

int pennfat_chmod(char **commands, FAT *fat){
    return 1;
}

dir_node* search_file(char* file_name, FAT* fat, dir_node** prev){
    if (fat->file_num == 0){
        return NULL;
    }
    dir_node* curr = fat->first_dir_node;
    while (curr != NULL){
        printf("%s\n",curr->dir_entry->name);
        printf("%s\n",file_name);

        if (strcmp(curr->dir_entry->name, file_name) != 0){
            curr = curr->next;
            if(prev != NULL) {
                *prev = curr;
            }
        } else{
            return curr;
        }
    }
    return NULL;
} 

int f_open(const char *f_name, int mode){
    //search for file with f_name:
    dir_node* file_node = search_file((char*)f_name, curr_fat, NULL);

    if(mode == F_READ) {
        // file has to exist for mode F_READ
        if(file_node == NULL || file_node->dir_entry->perm == 2) {
            return FAILURE;
        }

        return (int)file_node->dir_entry->firstBlock;
    } else if(mode == F_WRITE || mode == F_APPEND) {
        //no file found, then create file. 
        if(mode == F_WRITE && curr_fd != -1) {
            return FAILURE;
        }
        //create new file
        if (file_node == NULL) {
            uint16_t firstBlock = 12;
            for (uint32_t i = 2; i < curr_fat->entry_size; i++){
                if (curr_fat->block_arr[i] == ZERO){
                    firstBlock = (uint16_t) i;
                    break;
                }
            }
            // new a dir entry with 0 byte (empty file)
            file_node = new_directory_node((char*)f_name, 0, firstBlock, REGULAR_FILETYPE, READ_WRITE_EXCUTABLE, time(0));
            // append this node to the FAT dir information. 
            if (curr_fat->first_dir_node == NULL){
                curr_fat->first_dir_node = file_node;
                curr_fat->last_dir_node = file_node;
            } else {
                curr_fat->last_dir_node->next = file_node;
                curr_fat->last_dir_node = file_node;
            }
            curr_fat->file_num++;
            write_directory_to_block(*file_node->dir_entry, curr_fat);
            if(mode == F_WRITE) {
                curr_fd = (int) file_node->dir_entry->firstBlock;
            }

        } else if(file_node->dir_entry->perm == 4 || file_node->dir_entry->perm == 5) {
            return FAILURE;
        }
        return (int) file_node->dir_entry->firstBlock;
    }

    return FAILURE;
}

int f_read(int fd, int n, char *buf){

    uint32_t byte_read = 0;
    int curr_block = fd;
    uint16_t start_index = curr_fat->dblock_starting_index + (curr_block - 2) * 32;
    uint16_t index = start_index;
    // read data into buf
    while(byte_read < n) {
        char ch = (char) (curr_fat->block_arr[index] >> 8);
        buf[byte_read] = ch;
        byte_read++;
        // EOF reached
        if(ch == '\0') return EOF;
        // read another char at the same index
        if(byte_read < n) {
            ch = (char) curr_fat->block_arr[index] & 0x00FF;
            buf[byte_read] = ch;
            byte_read++;
            if(ch == '\0') return EOF;
        }
        index++;
        // find next data block to read
        if(index == start_index + 32) {
            if(curr_fat->block_arr[curr_block] != 0xFFFF) {
                curr_block = curr_fat->block_arr[curr_block];
                start_index = curr_fat->dblock_starting_index + (curr_block - 2) * 32;
                index = start_index;
            }
        }
    }
    return byte_read;
}

int f_write(int fd, const char *str, int n){
    uint32_t byte_write = 0;
    int curr_block = fd;
    uint16_t start_index = curr_fat->dblock_starting_index + (curr_block - 2) * 32;
    uint16_t index = start_index;

    //write mode
    if(curr_fd == fd) {
        while(byte_write < n) {
            curr_fat->block_arr[index] = str[byte_write] << 8 | '\0';
            byte_write++;
            if(byte_write < n) {
                curr_fat->block_arr[index] = curr_fat->block_arr[index] | str[byte_write];
                byte_write++;
            }
            index++;

            //check if current data block is full
            if(index == start_index + 32) {
                //find next available data block
                if(curr_fat->block_arr[curr_block] != 0xFFFF) {
                    curr_block = curr_fat->block_arr[curr_block];
                    start_index = curr_fat->dblock_starting_index + (curr_block - 2) * 32;
                    index = start_index;
                } else {
                    int free_entry_index = 2;
                    // find unused data block
                    while(free_entry_index < curr_fat->directory_starting_index && curr_fat->block_arr[free_entry_index] != 0x0000) {
                        free_entry_index++;
                    }

                    if(free_entry_index < curr_fat->block_num) {
                        curr_fat->block_arr[curr_block] = free_entry_index;
                        curr_block = free_entry_index;
                        start_index = curr_fat->dblock_starting_index + (curr_block - 2) * 32;
                        index = start_index;
                    } else {
                        return FAILURE;
                    }
                    
                }
            }
        }

        // find file node and update file size
        dir_node* curr_node = curr_fat->first_dir_node;
        while(curr_node->dir_entry->firstBlock != fd) {
            curr_node = curr_node->next;
        }
        directory_entry* curr_dir = curr_node->dir_entry;
        curr_dir->size = byte_write;
        

        directory_entry* entry_ptr = (directory_entry*) &curr_fat->block_arr[curr_fat->directory_starting_index + (curr_dir->firstBlock - 2) * 32];
        *entry_ptr = *curr_dir;
    } else {
        dir_node* curr_node = curr_fat->first_dir_node;
        while(curr_node->dir_entry->firstBlock != fd) {
            curr_node = curr_node->next;
        }
        directory_entry* curr_dir = curr_node->dir_entry;
        int curr_size = curr_dir->size;

        // find the end of the file
        if(curr_size != 0) {
            while(curr_size >= 64) {
                curr_block = curr_fat->block_arr[curr_block];
                curr_size -= 64;
            }
            start_index = curr_fat->dblock_starting_index + (curr_block - 2) * 32;
            index = start_index;
            while(curr_fat->block_arr[index] >> 8 != '\0' || (curr_fat->block_arr[index] & 0x00FF) != '\0') {
                index++;
            }

            // if a free space available at the current index, write one char
            if((curr_fat->block_arr[index] & 0x00FF) == '\0' && byte_write < n) {
                curr_fat->block_arr[index] = curr_fat->block_arr[index] | str[byte_write];
                index++;
            }
            
        }

        while(byte_write < n) {
            //check if current data block is full
            if(index == start_index + 32) {
                //find next available data block
                if(curr_fat->block_arr[curr_block] != 0xFFFF) {
                    curr_block = curr_fat->block_arr[curr_block];
                    start_index = curr_fat->dblock_starting_index + (curr_block - 2) * 32;
                    index = start_index;
                } else {
                    int free_entry_index = 2;
                    // find unused data block
                    while(free_entry_index < curr_fat->directory_starting_index && curr_fat->block_arr[free_entry_index] != 0x0000) {
                        free_entry_index++;
                    }

                    if(free_entry_index < curr_fat->block_num) {
                        curr_fat->block_arr[curr_block] = free_entry_index;
                        curr_block = free_entry_index;
                        start_index = curr_fat->dblock_starting_index + (curr_block - 2) * 32;
                        index = start_index;
                    } else {
                        return FAILURE;
                    }
                    
                }
            }

            curr_fat->block_arr[index] = str[byte_write] << 8 | '\0';
            byte_write++;
            if(byte_write < n) {
                curr_fat->block_arr[index] = curr_fat->block_arr[index] | str[byte_write];
                byte_write++;
            }
            index++;
        }

        // find file node and update file size
        curr_dir->size = curr_dir->size + byte_write;
        directory_entry* entry_ptr = (directory_entry*) &curr_fat->block_arr[curr_fat->directory_starting_index + (curr_dir->firstBlock - 2) * 32];
        *entry_ptr = *curr_dir;
    }
    
    return byte_write;
}

int f_close(int fd) {
    if(curr_fd == fd) {
        curr_fd = -1;
    }
    
    return 0;
}