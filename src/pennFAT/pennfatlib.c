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
int* file_d;
int* file_pos;
int file_d_size;

int parse_pennfat_command(char ***commands, int commandCount){
    char* cmd = commands[0][0];

    if(strcmp(cmd, "mkfs") == 0) {
        if (commands[0][1] == NULL || commands[0][2] == NULL || commands[0][3] == NULL) {
            printf("insuffcient arguement\n");
            return FAILURE;
        }
        return pennfat_mkfs(commands[0][1], (char) atoi(commands[0][2]), (char) atoi(commands[0][3]), &curr_fat);
    } else if (strcmp(cmd, "mount") == 0) {
        printf("here in mount fract\n");

        if(curr_fat != NULL) {
            printf("A filesystem already mounted, please unmount first\n");
            return FAILURE;
        }
        printf("here, before calling pennfat_mount\n");

        curr_fat = pennfat_mount(commands[0][1]);
        if (curr_fat == NULL) return FAILURE;
        return SUCCESS;
    } else if (curr_fat == NULL) {
        printf("No filesystem mounted yet\n");
        return FAILURE;
    } else if (strcmp(cmd, "umount") == 0) {
        free_fat(curr_fat);
        curr_fat = NULL;
        return SUCCESS;
    } else if (strcmp(cmd, "touch") == 0) {
        return pennfat_touch(commands[0], curr_fat);
    } else if (strcmp(cmd, "mv") == 0) {
        return pennfat_mv(commands[0][1], commands[0][2], curr_fat);
    } else if (strcmp(cmd, "rm") == 0) {
        return pennfat_remove(commands[0], curr_fat);
    }  else if (strcmp(cmd, "cat") == 0) {
        return pennfat_cat(commands[0], curr_fat);
    } else if (strcmp(cmd, "cp") == 0) {
        return pennfat_cp(commands[0], curr_fat);
    } else if (strcmp(cmd, "ls") == 0) {
        return pennfat_ls(curr_fat);
    } else if (strcmp(cmd, "chmod") == 0) {
        return pennfat_chmod(commands[0], curr_fat);
    } else if (strcmp(cmd, "describe") == 0) {
        printf("File system name : %s\n", (curr_fat)->f_name);
        printf("Number of block in the filesystem : %d\n", (curr_fat)->block_num);
        printf("Block size : %d\n", (curr_fat)->block_size);
        printf("Number of entries: %d\n", (curr_fat)->entry_size);
        printf("number of files : %d\n", (curr_fat)->file_num);
        printf("available blocks: %d\n", (curr_fat)->free_entries);
        if((curr_fat)->first_dir_node != NULL) {
            printf("first file is : %s\n", (curr_fat)->first_dir_node->dir_entry->name);
        }
        printf("available blocks: %d\n", (curr_fat)->free_entries);

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
    file_d = (int*) malloc(sizeof(int) * (curr_fat->block_size / 64));
    file_pos = (int*) malloc(sizeof(int) * (curr_fat->block_size / 64));
    // printf("curr_fat->block_size: %d\n", curr_fat->block_size);
    file_d_size = curr_fat->block_size / 64;
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
        if(fd == FAILURE) {
            return FAILURE;
        }
        // testing code for f_read and f_write
        //
        // if(fd == -1) {
        //     printf("open error\n");
        // } else {
        //     printf("open successful\n");
        // }
        // char arr1[6] = "abcdef";
        // int byte_write = f_write(fd, arr1, 6);
        // printf("byte write: %d\n", byte_write);
        // char arr2[5];
        // int byte_read = f_read(fd, 5, arr2);
        // printf("byte read: %d, value: %s\n", byte_read, arr2);
        f_close(fd);
        // char arr2[6] = "ghijkl";
        // byte_write = f_write(fd, arr2, 6);
        // printf("byte write: %d\n", byte_write);
        index += 1;
        file_name = files[index];
    }

    return SUCCESS;
}

int pennfat_mv(char *oldFileName, char *newFileName, FAT *fat){
    dir_node* old_f = search_file(oldFileName, curr_fat, NULL);
    if(old_f == NULL) {
        printf("error mv: the original file does not exist\n");
        return FAILURE;
    }
    if (delete_directory_from_block(*old_f->dir_entry,curr_fat) ==FAILURE) {
        return FAILURE;
    }
    memset(old_f->dir_entry->name, 0, strlen(old_f->dir_entry->name));
    memcpy(old_f->dir_entry->name, newFileName,strlen(newFileName));
    if (write_directory_to_block(*old_f->dir_entry,curr_fat)== -1 ) {
        printf("error: failed to write directory entry to block\n");
        return FAILURE;
    }   
    return SUCCESS;
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
    int count = 0;
    while (commands[count] != NULL) {
        count++;
    }

    if (count < 2) {
        printf("Insufficient argument for cat\n");
        return FAILURE;
    }
    for (int i = 0; i < count; i++) {
        if ((strcmp(commands[i], "-w") == 0 || strcmp(commands[i], "-a") == 0 )&& i != count - 2) {
            printf("Wrong flag position\n");
            return FAILURE;
        }
    bool writing = strcmp(commands[count - 2], "-w") == 0;
    bool appending = strcmp(commands[count - 2], "-a") == 0;

    //handle read from terminal cases:
    // cat -a file
    //cat -w file
    if ((count == 3) && (writing || appending)) {
        char *line = NULL;
        size_t len = 0;
        printf("Please enter here:\n");
        if (getline(&line, &len, stdin) == - 1) {
            perror("getline");
            return FAILURE;
        }
        // writing line to the destination file. 

            char* f_name = commands[count-1];
            if(writing) {
                printf("debugging: writing mode......");
                int fd = f_open(f_name, F_WRITE);
                printf("current fd: %d\n", fd);
                if(f_write(fd, line, len) == -1) {
                    f_close(fd);
                    return FAILURE;
                }
                f_close(fd);
                return SUCCESS;
            } else if (appending)
            {
                printf("debugging: appending mode......");
                int fd = f_open(f_name, F_APPEND);
                printf("current fd: %d\n", fd);
                printf("current line: %s\n", line);
                printf("current len: %zu\n", len);
                printf("file_d[0]: %d\n", file_d[0]);
                printf("file_d[1]: %d\n", file_d[1]);
                printf("file_d[2]: %d\n", file_d[2]);
                printf("file_d[3]: %d\n", file_d[3]);
                printf("file_pos[0]: %d\n", file_pos[0]);
                printf("file_pos[1]: %d\n", file_pos[1]);
                printf("file_pos[2]: %d\n", file_pos[2]);
                printf("file_pos[3]: %d\n", file_pos[3]);
                printf("file_d_size: %d\n", file_d_size);
                int status = f_write(fd, line, len);
                printf("current status: %d\n", status);
                if(status == -1) {
                    f_close(fd);
                    return FAILURE;
                }
                f_close(fd);
                return SUCCESS;
            }
        }

    //handle redirection cases:
    // cat file1 -w file2
    // cat file1 -a file2
    if(count == 4 && (writing || appending)) {
        char* f1_name = commands[1];
        char* f2_name = commands[3];

        int f1_fd = f_open(f1_name, F_READ);
        int f2_fd;
        if(writing) {
        f2_fd = f_open(f2_name, F_WRITE);
        } else {
        f2_fd = f_open(f2_name, F_APPEND);
        }
        dir_node* f1_node = search_file(f1_name,curr_fat,NULL);
        char buff[f1_node->dir_entry->size];
        int status = f_read(f1_fd, f1_node->dir_entry->size, buff);
        // printf("status: %d, buff: %s\n", status, buff);
        if(status >= 0 || status == EOF) {
            if(f_write(f2_fd,buff, sizeof(buff))==SUCCESS) {
                f_close(f1_fd);
                f_close(f2_fd);
                return SUCCESS;
            } else {
                f_close(f1_fd);
                f_close(f2_fd);
                return FAILURE;
            }
        } else {
            printf("error: f_read\n");
            f_close(f1_fd);
            f_close(f2_fd);
            return FAILURE;
        }

    }
    }
    return SUCCESS;
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
    if (commands[1] == NULL) {
        printf("error: No file name entered\n");
        return FAILURE;
    }

    if (commands[2] == NULL) {
        printf("error: No permission specified\n");
        return FAILURE;
    }

    int perm = 0;

    if (strcmp(commands[2], "-w") == 0) {
        perm = WRITE_PERMS;
    } else if (strcmp(commands[2], "r-") == 0) {
        perm = READ_PERMS;
    } else if (strcmp(commands[2], "rw") == 0) {
        perm = READ_WRITE_PERMS;
    } else if (strcmp(commands[2], "--") == 0) {
        perm = NO_PERMS;
    } else {
        printf("Permission type must be one of -w, r-, rw, and --\n");
    }

    dir_node* file_node = search_file(commands[1], curr_fat, NULL);
    file_node->dir_entry->perm = perm;
    if(delete_directory_from_block(*file_node->dir_entry, curr_fat) == FAILURE) {
        printf("error: delte entry from block");
        return FAILURE;
    }
    if(write_directory_to_block(*file_node->dir_entry, curr_fat)) {
        printf("error: write entry to block");
        return FAILURE;
    }
    return SUCCESS;
}

dir_node* search_file(char* file_name, FAT* fat, dir_node** prev){
    if (fat->file_num == 0){
        return NULL;
    }
    dir_node* curr = fat->first_dir_node;
    while (curr != NULL){
        if (strcmp(curr->dir_entry->name, file_name) != 0){
            if(prev != NULL) {
                *prev = curr;
            }
            curr = curr->next;

        } else{
            return curr;
        }
    }
    return NULL;
}

// mode 0: search file_d, find fd location
// mode 1: search file_d, find empty spot
// mode 2: search file_d, delete fd information
int file_d_search(int fd, int mode) {
    int i = 0;
    int target = 0;

    if(mode == 0 || mode == 2) {
        target = fd;
    }
    // printf("i: %d, file_d_size: %d, target: %d\n", i, file_d_size, target);
    while(i < file_d_size) {
        // printf("file_d value: %d, i: %d\n", file_d[i], i);
        if(file_d[i] == target) {
            if(mode == 2) {
                file_d[i] = 0;
                file_pos[i] = 0;
            }
            return i;
        }
        i++;
    }

    return -1;
}



int f_open(const char *f_name, int mode){
    printf("now in f_open......");
    //search for file with f_name:
    dir_node* file_node = search_file((char*)f_name, curr_fat, NULL);
    if(mode == F_READ) {
        // file has to exist for mode F_READ
        if(file_node == NULL || file_node->dir_entry->perm == 2) {
            return FAILURE;
        }
        int fd = (int) file_node->dir_entry->firstBlock;
        int index = file_d_search(fd, 1);
        file_d[index] = fd;
        file_pos[index] = 0;
        return fd;
    } else if(mode == F_WRITE || mode == F_APPEND) {
        printf("mode is writing/appending");
        //no file found, then create file. 
        if(mode == F_WRITE && curr_fd != -1) {
            printf("debugging: already a file with writing mode is opened! invalid");
            return FAILURE;
        }
        //create new file
        if (file_node == NULL) {
            printf("creating a new file in f_open");
            uint16_t firstBlock = 0;
            for (uint32_t i = 2; i < curr_fat->entry_size; i++){
                if (curr_fat->block_arr[i] == ZERO){
                    firstBlock = (uint16_t) i;
                    curr_fat->block_arr[i] = 0xffff;
                    break;
                }
            }
            if(firstBlock == 0) return FAILURE;
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
        } else if(file_node->dir_entry->perm == 4 || file_node->dir_entry->perm == 5) {
            return FAILURE;
        }
        if(mode == F_WRITE) {
            printf("f_open: F_WRITE");
            curr_fd = (int) file_node->dir_entry->firstBlock;
        }
        // printf("here1\n");
        printf("f_open: append mode will be here......");
        int fd = (int) file_node->dir_entry->firstBlock;
        int index = file_d_search(fd, 1);
        // printf("file index: %d\n", index);
        file_d[index] = fd;
        file_pos[index] = 0;
        return fd;
    }

    return FAILURE;
}

int f_read(int fd, int n, char *buf){

    uint32_t byte_read = 0;
    int curr_block = fd;
    int i = file_d_search(fd, 0);
    int pos = file_pos[i];

    uint16_t start_index = curr_fat->dblock_starting_index + (curr_block - 2) * curr_fat->block_size / 2;
    uint16_t index = start_index;
    int curr_pos = pos;

    // printf("i: %d, curr_pos: %d, start_index: %d\n", i, curr_pos, start_index);
    while(curr_pos / 2 >= curr_fat->block_size / 2) {
        if(curr_fat->block_arr[curr_block] != 0xFFFF) {
            curr_block = curr_fat->block_arr[curr_block];
            start_index = curr_fat->dblock_starting_index + (curr_block - 2) * curr_fat->block_size / 2;
            curr_pos = curr_pos / 2 - curr_fat->block_size / 2;
            index = start_index + curr_pos / 2;
            
        } else {
            return 0;
        }
    }
    // printf("curr_block: %d, curr_pos: %d, start_index: %d\n", curr_block, curr_pos, start_index);

    while(curr_pos > 1) {
        curr_pos -= 2;
        index++;
    }

    if(curr_pos % 2 == 1) {
        char ch = (char) (curr_fat->block_arr[index] >> 8); // & 0x00FF);
        if(ch == '\0') return EOF;
        buf[byte_read] = ch;
        byte_read++;
        pos++;
        index++;
    }
    
    
    // printf("here in read systemcall\n");
    // read data into buf
    while(byte_read < n) {

        char ch = (char) curr_fat->block_arr[index] & 0x00FF;//(char) (curr_fat->block_arr[index] >> 8);
        // EOF reached
        // printf("reading ch1: %c, index: %d, byte_read: %d, pos: %d, n: %d\n", ch, index, byte_read, pos, n);
        if(ch == '\0') return EOF;
        buf[byte_read] = ch;
        byte_read++;
        pos++;
        
        
        // read another char at the same index
        if(byte_read < n) {
            ch = (char) (curr_fat->block_arr[index] >> 8); // (char) curr_fat->block_arr[index] & 0x00FF;
            // printf("reading ch2: %c, index: %d, byte_read: %d, pos: %d, n: %d\n", ch, index, byte_read, pos, n);
            if(ch == '\0') return EOF;
            buf[byte_read] = ch;
            byte_read++;
            pos++;
            
        }

        index++;
        // find next data block to read
        if(index == start_index + 32) {
            if(curr_fat->block_arr[curr_block] != 0xFFFF) {
                curr_block = curr_fat->block_arr[curr_block];
                start_index = curr_fat->dblock_starting_index + (curr_block - 2) * curr_fat->block_size / 2;
                index = start_index;
            } else {
                return EOF;
            }
        }
    }
    file_pos[i] = pos;
    return byte_read;
}

int f_write(int fd, const char *str, int n){
    printf("\n now in f_write...");
    uint32_t byte_write = 0;
    int curr_block = fd;
    uint16_t start_index = curr_fat->dblock_starting_index + (curr_block - 2) * curr_fat->block_size / 2;
    uint16_t index = start_index;
    printf("f_write start_index: %d, curr_block: %d\n", start_index, curr_block);
    printf("f_write directory_starting_index: %d\n", curr_fat->directory_starting_index);
    printf("f_write dblock_starting_index: %d\n", curr_fat->dblock_starting_index);

    //write mode
    if(curr_fd == fd) {
        printf("f_write mode");
        while(byte_write < n && str[byte_write] != '\0') {
            // curr_fat->block_arr[index] = str[byte_write] << 8 | '\0';
            curr_fat->block_arr[index] = str[byte_write];
            byte_write++;
            printf("byte_write: %d\n", byte_write);
            
            if(byte_write < n && str[byte_write] != '\0') {
                // curr_fat->block_arr[index] = curr_fat->block_arr[index] | str[byte_write];
                curr_fat->block_arr[index] = curr_fat->block_arr[index] | (str[byte_write] << 8);
                byte_write++;
            }
            index++;
            printf("index: %d\n", index);

            //check if current data block is full
            if(index == start_index + curr_fat->block_size / 2) {
                printf("index now is 384");
                //find next available data block
                if(curr_fat->block_arr[curr_block] != 0xFFFF) {
                    curr_block = curr_fat->block_arr[curr_block];
                    start_index = curr_fat->dblock_starting_index + (curr_block - 2) * curr_fat->block_size / 2;
                    index = start_index;
                } else {
                    printf("the current entry is ffff\n");
                    int free_entry_index = 2;
                    // find unused data block
                    while(free_entry_index < curr_fat->directory_starting_index && curr_fat->block_arr[free_entry_index] != 0x0000) {
                        free_entry_index++;
                    }
                    printf("free_entry_index: %d\n", free_entry_index);
                    printf("curr_fat->block_num: %d\n", curr_fat->block_num);
                    
                    if(free_entry_index < curr_fat->directory_starting_index) {
                        curr_fat->block_arr[curr_block] = free_entry_index;
                        curr_block = free_entry_index;
                        start_index = curr_fat->dblock_starting_index + (curr_block - 2) * curr_fat->block_size / 2;
                        index = start_index;
                        printf("curr_fat->block_arr[curr_block]: %d\n", curr_fat->block_arr[curr_block]);
                        printf("curr_block: %d\n", curr_block);
                        printf("start_index: %d\n", start_index);
                        printf("index: %d\n", index);
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
        printf("write mode: curr_dir->size: %d\n", curr_dir->size);
        printf("write mode: entry_ptr size: %d\n", entry_ptr->size);
        printf("write mode: entry_ptr firstBlock: %d\n", entry_ptr->firstBlock);
    } else {
        // append mode
        printf("debugging: currently in f_write append mode");
        dir_node* curr_node = curr_fat->first_dir_node;
        while(curr_node->dir_entry->firstBlock != fd) {
            curr_node = curr_node->next;
        }
        directory_entry* curr_dir = curr_node->dir_entry;
        int curr_size = curr_dir->size;
        // find the end of the file
        if(curr_size != 0) {
            printf("position 1");
            while(curr_size >= 64) {
                printf("position 2");
                curr_block = curr_fat->block_arr[curr_block];
                curr_size -= 64;
            }
            printf("position 3");
            start_index = curr_fat->dblock_starting_index + (curr_block - 2) * curr_fat->block_size / 2;
            index = start_index;
            while(curr_fat->block_arr[index] >> 8 != '\0' && (curr_fat->block_arr[index] & 0x00FF) != '\0') {
                printf("position 4");
                index++;
            }
            // if a free space available at the current index, write one char
            if((curr_fat->block_arr[index] & 0x00FF) != '\0' && (curr_fat->block_arr[index] >> 8) == '\0' && byte_write < n) {
                printf("position 5");
                curr_fat->block_arr[index] = curr_fat->block_arr[index] | (str[byte_write] << 8);
                byte_write++;
                index++;
            }
        }
        printf("position 6");
        while(byte_write < n && str[byte_write] != '\0') {
            printf("position 7");
            //check if current data block is full
            if(index == start_index + curr_fat->block_size / 2) {
                printf("position 8");
                //find next available data block
                if(curr_fat->block_arr[curr_block] != 0xFFFF) {
                    printf("position 9");
                    curr_block = curr_fat->block_arr[curr_block];
                    start_index = curr_fat->dblock_starting_index + (curr_block - 2) * curr_fat->block_size / 2;
                    index = start_index;

                } else {
                    printf("position 10");
                    int free_entry_index = 2;
                    // find unused data block
                    while(free_entry_index < curr_fat->directory_starting_index && curr_fat->block_arr[free_entry_index] != 0x0000) {
                        printf("position 11");
                        free_entry_index++;
                    }

                    if(free_entry_index < curr_fat->directory_starting_index) {
                        printf("position 12");
                        curr_fat->block_arr[curr_block] = free_entry_index;
                        curr_block = free_entry_index;
                        start_index = curr_fat->dblock_starting_index + (curr_block - 2) * curr_fat->block_size / 2;
                        index = start_index;
                    } else {
                        printf("position 13");
                        return FAILURE;
                    }
                    
                }
            }
            printf("position 14");
            curr_fat->block_arr[index] = str[byte_write]; // << 8 | '\0';
            byte_write++;
            if(byte_write < n && str[byte_write] != '\0') {
                printf("position 15");
                curr_fat->block_arr[index] = curr_fat->block_arr[index] | (str[byte_write] << 8);
                byte_write++;
            }
            index++;
        }
        printf("position 16");
        // find file node and update file size
        curr_dir->size = curr_dir->size + byte_write;
        directory_entry* entry_ptr = (directory_entry*) &curr_fat->block_arr[curr_fat->directory_starting_index + (curr_dir->firstBlock - 2) * 32];
        *entry_ptr = *curr_dir;
        printf("after position 16: curr_dir->size: %d\n", curr_dir->size);
        printf("after position 16: entry_ptr size: %d\n", entry_ptr->size);
        printf("after position 16: entry_ptr firstBlock: %d\n", entry_ptr->firstBlock);
    }
    
    return byte_write;
}

int f_close(int fd) {
    if(curr_fd == fd) {
        curr_fd = -1;
    }

    file_d_search(fd, 2); // delete file from file_d
    
    return 0;
}

int f_lseek(int fd, int offset, int whence){
    int index = file_d_search(fd, 0);
    if(index == -1) {
        return FAILURE;
    }

    if(whence == SEEK_SET) {
        file_pos[index] = offset;
    } else if(whence == SEEK_CUR) {
        file_pos[index] += offset;
    } else if(whence == SEEK_END) {
        dir_node* node= curr_fat->first_dir_node;
        while(node != NULL && node->dir_entry != NULL && node->dir_entry->firstBlock != fd) {
            node = node->next;
        }
        int size = node->dir_entry->size;

        file_pos[index] += size + offset;
    } else {
        return FAILURE;
    }

    return file_pos[index];
}