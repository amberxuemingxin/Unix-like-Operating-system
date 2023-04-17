#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include "FAT.h"
#include "macro.h"
// #include "file.h"
dir_node * new_directory_node(char* f_name, uint32_t size, uint16_t firstBlock, uint8_t type, uint8_t perm, time_t time) {
    dir_node* res = (dir_node*)malloc(sizeof(dir_node));
    res->next = NULL;
    directory_entry* res_directory_entry = (directory_entry*)malloc(sizeof(directory_entry));
    res->dir_entry = res_directory_entry;
    res_directory_entry->firstBlock=firstBlock;
    res_directory_entry->mtime=time;
    res_directory_entry->perm=perm;
    res_directory_entry->size=size;

    for (size_t i = 0; i<32;i++) {
        res_directory_entry->name[i] = '\0';
    }
    strcpy(res_directory_entry->name,f_name);

    return res;
}

void free_directory_node(dir_node *node) {
    free(node->dir_entry);
    free(node);
}

FAT* make_fat(char* f_name, uint8_t block_num, uint8_t block_size) {
    if (block_num > 32 || block_num < 1) {
        perror("number of blocks needs to be within 1-32");
        return NULL;
    }
    if (block_size > 4 || block_size < 0) {
        perror("block size needs to be within 0-4");
        return NULL;
    }
    FAT* res = (FAT*) malloc(sizeof(FAT));

    res->file_num = 0;

    // res->first_dir_node =NULL;
    // res->last_dir_node =NULL;
    
    res->f_name = f_name;
    int len = strlen(f_name);

    res->f_name = (char*) malloc(len * sizeof(char) + 1);

    strcpy(res->f_name, f_name);
    res->f_name[len] =  '\0';
    res->block_num = block_num;

    if (block_size == 0) {
        res->block_size = 256;
    }
    else if (block_size == 1) {
        res->block_size = 512;
    } else if(block_size == 2) {
        res->block_size = 1024;
    } else if(block_size == 3) {
        res->block_size = 2048;
    } else if(block_size == 4) {
        res->block_size = 4096;
    }
    uint32_t fat_size = 0;
    // # of FAT entries = block size * number of blocks in FAT / 2
    res->entry_size = res->block_size * res->block_num;
    fat_size += res->entry_size;
    res->entry_size /= 2;
    res->free_entries = res->entry_size -2;
    res->data_size = (res->free_entries + 1) * (res->block_size);

    fat_size += res->data_size;

    int fs_fd = open(f_name, O_RDWR | O_TRUNC | O_CREAT, 0644);
    if(fs_fd == -1) {
        perror("open file");
        return NULL;
    }

    // use ftruncate to make the fatsize == block_num * block size
    if(ftruncate(fs_fd, (fat_size)) == -1) {
        perror("file truncate");
        return NULL;
    }
    

    res->block_arr = (uint16_t*) mmap(NULL, (fat_size), PROT_READ | PROT_WRITE, MAP_SHARED, fs_fd, 0);
    res->directory_starting_index = (uint32_t) res->entry_size;
    res->dblock_starting_index = (uint32_t) (res->directory_starting_index); 
    res->dblock_starting_index += (uint32_t) (res->block_size)/2; 


    // res->block_arr[res->directory_starting_index] = (uint16_t) 's';
    //visualizeation
    // res->block_arr[(res->directory_starting_index)] =  0XDDDD;
    // res->block_arr[(res->dblock_starting_index)] =  0XBBBB;


    //first block stored FS information by LSB and MSB
                                    //LSB               MSB
    res->block_arr[0] = (uint16_t) block_num << 8 | block_size;

    //second block is the root directory
    res->block_arr[1] = ROOT;
    close(fs_fd);
    return res;
}   

FAT* mount_fat(char* f_name) {
        //file system file descriptor
    int fs_fd;
    if ((fs_fd = open(f_name, O_RDONLY, 0644)) == -1) {
        perror("open");
        return NULL;
    }

   //read the first bytes as blocksize
    uint8_t block_size = 0;
    uint32_t actual_block_size= 0;
    if (read(fs_fd, &block_size, sizeof(uint8_t)) == -1) {
        perror("read");
        return NULL;
    }

    // read the next byte as number of blocks
    uint8_t numBlocks = 0;
    if (read(fs_fd, &numBlocks, sizeof(uint8_t)) == -1) {
        perror("read");
        return NULL;
    }    
    if (block_size == 0) {
        actual_block_size = 256;
    }
    else if (block_size == 1) {
        actual_block_size = 512;
    } else if(block_size == 2) {
        actual_block_size = 1024;
    } else if(block_size == 3) {
        actual_block_size = 2048;
    } else if(block_size == 4) {
        actual_block_size = 4096;
    }
    // TODO: read direcory entry information:
    uint32_t entry_size = 0;
    // # of FAT entries = block size * number of blocks in FAT / 2
    entry_size = actual_block_size * numBlocks;
    //max filenum denots the maximum number of directory entries in a block
    int max_filenum = entry_size / SIZE_DIRECTORY_ENTRY;
    int count = 0;
    uint16_t directory_block = 0;
    int block = 1;
    while (directory_block != 0XFFFF) {
        if (read(fs_fd, &directory_block, sizeof(uint16_t)) == -1) {
            perror("read");
            return NULL;
        }
        int starting_index = block* (int) entry_size;
        for(int i = 0 ; i < max_filenum; i++) {
            lseek(fs_fd, starting_index + SIZE_DIRECTORY_ENTRY * i, SEEK_SET);
            uint8_t* buf = malloc(sizeof(uint8_t));
            if (read(fs_fd, buf, 1) == -1) {
                perror("read");
                return NULL;
            }
            if((uint8_t)buf[0] == 0){
                free(buf);
                break;
            }
            count++;
            free(buf);
        }
        block = (int) directory_block;
        //set to next fat_entry to read
        lseek(fs_fd, block*sizeof(uint16_t),SEEK_SET);
    }
    //we can use count to determine how many entries are in the directory_entry
    directory_entry **entry_arr = malloc(count * sizeof(directory_entry *));
    for (int i = 0; i < count; i++) {
    entry_arr[i] = malloc(sizeof(directory_entry));
    if (entry_arr[i] == NULL) {
        perror("Failed to allocate memory for entry_arr[i]");
        // Free previously allocated memory and return NULL
        for (int j = 0; j < i; j++) {
            free(entry_arr[j]);
        }
        free(entry_arr);
        return NULL;
        }
    }
    //let's read again
    directory_block = 0;
    block = 1;
    lseek(fs_fd, 1*sizeof(uint16_t), SEEK_SET);
    // j mark the index in 
    int entry_num = 0;
  
    while (directory_block != 0XFFFF && entry_num < count) {
        if (read(fs_fd, &directory_block, sizeof(uint16_t)) == -1) {
            perror("read");
            return NULL;
        }
        int starting_index = block * (int)entry_size;
        printf("strange shit right here, starting index is %d\n",starting_index);
        for (int i = 0; i < max_filenum; i++) {
            lseek(fs_fd, starting_index + SIZE_DIRECTORY_ENTRY * i, SEEK_SET);
            uint8_t buffer[SIZE_DIRECTORY_ENTRY];
            for (int j = 0; j < SIZE_DIRECTORY_ENTRY; j++) {
                buffer[j] = 0x00;
            }
            if (read(fs_fd, buffer, SIZE_DIRECTORY_ENTRY) == -1) {
                perror("read");
                break;
            }
            // If not a directory_entry, exit the loop
            if ((uint8_t) buffer[0] == 0) {
                break;
            }
            // Copy the contents of the buffer into the struct at the appropriate index in the entry_arr array
            memcpy(&entry_arr[entry_num*max_filenum + i]->name, buffer, 32);
            memcpy(&entry_arr[entry_num*max_filenum + i]->size, buffer + 32, sizeof(uint32_t));
            memcpy(&entry_arr[entry_num*max_filenum + i]->firstBlock, buffer + 36, sizeof(uint16_t));
            memcpy(&entry_arr[entry_num*max_filenum + i]->type, buffer + 38, sizeof(uint8_t));
            memcpy(&entry_arr[entry_num*max_filenum + i]->perm, buffer + 39, sizeof(uint8_t));
            memcpy(&entry_arr[entry_num*max_filenum + i]->mtime, buffer + 40, sizeof(time_t));
            // memcpy(entry_arr[entry_num*max_filenum + i], buffer, sizeof(directory_entry));
            printf("debugging: just checking, entry_arr[entry_num*max_filenum + i]'s first block is %d\n", entry_arr[entry_num*max_filenum + i]->firstBlock);
        }
        entry_num += 1;
        block = (int) directory_block;
        lseek(fs_fd, block * sizeof(uint16_t), SEEK_SET);
    }

    dir_node* head = NULL; 
    dir_node* curr_node = NULL; 
    for (size_t i = 0; i < count; i++) {
        if (entry_arr[i] == NULL) {
            break;
        }
        dir_node* new_node = malloc(sizeof(dir_node));
        new_node->dir_entry = entry_arr[i];
        new_node->next = NULL;
        if (head == NULL) { 
            head = new_node; 
            curr_node = head;
        } else {
            curr_node->next = new_node; 
            curr_node = new_node; 
        }
    }

    // TODO: WRITE DATA REGION TO FAT
    // create a temp binary file that copies content from original files

    
    int tmp_fd = open("temp", O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
    if (tmp_fd < -1) {
        perror("error: mount, couldn't open temp file\n");
        return NULL;
    }

    char tmp_buffer[1024];
    int bytes_read, bytes_written;
    lseek(fs_fd, 0, SEEK_SET);
    while ((bytes_read = read(fs_fd, tmp_buffer, sizeof(tmp_buffer))) > 0) {
        bytes_written = write(tmp_fd, tmp_buffer, bytes_read);
        if (bytes_written != bytes_read) {
            printf("error: Could not write to tmp file!\n");
            close(fs_fd);
            close(tmp_fd);
            return NULL;
        }
    }    
    // make fat
    // write the temp to new file
    // delete temp file. 

    FAT *res = make_fat(f_name, numBlocks, block_size);
    if (res == NULL) {
        printf("error: Failed to load FAT\n");
        return NULL;
    }
    res->first_dir_node = head;
    dir_node* curr = res->first_dir_node;
    while(curr != NULL) {
        res->file_num += 1;
        int* reside_index = malloc(sizeof(int));
        if(write_directory_to_block(*curr->dir_entry,res,reside_index)== FAILURE) {
            printf("error: write directory entry to block");
            return res;
        }
        if(curr->next ==NULL) {
            res->last_dir_node = curr;
        }
        curr = curr->next;
    }
    close(fs_fd);
    int fs_wfd = open(f_name, O_WRONLY);
    //write tmp back to f_name
    tmp_fd = open("temp", O_RDONLY);
    if (tmp_fd < -1) {
        perror("error: mount, couldn't open temp file\n");
        return NULL;
    }


    while ((bytes_read = read(tmp_fd, tmp_buffer, sizeof(tmp_buffer))) > 0) {
            bytes_written = write(fs_wfd, tmp_buffer, bytes_read);
            if (bytes_written != bytes_read) {
                printf("error: Could not write to fs file!\n");
                close(fs_wfd);
                close(tmp_fd);
                return NULL;
            }
        }
    close(fs_wfd);
    close(tmp_fd);
    if (unlink("temp") == -1) {
        perror("unlink");
}
    return res;
    }

void free_fat(FAT* fat){
    struct FAT *curr_fat = fat;
    if (curr_fat == NULL)   return;

    while (curr_fat->first_dir_node != NULL) {
        dir_node *curr = curr_fat->first_dir_node;
        curr_fat->first_dir_node = curr->next;
        free_directory_node(curr);
    }

    if (munmap(curr_fat->block_arr, curr_fat->block_size * curr_fat->block_num) == -1) {
        perror("munmap");
        return;
    }

    free(curr_fat);
}

// TODO:
    /*
        - CHECK block[1]
        - if not 0XFFFF, find newblock, update block[1]
        - if 0XFFFF, find empty 64bytes, if full, search another block
    */

int write_directory_to_block(directory_entry en, FAT* fat, int* reside_block) {
    *reside_block = 1;
    //block_len in bytes
    uint16_t block_len = fat->block_size * fat->block_num;
    //block len in 2 bytes
    block_len /= 2;    // 128
    if(fat->block_arr[1] == 0XFFFF) {
        bool dir_full = false; 
        // find a spot in file system
        uint16_t index = 0;
        //increment 32 at a time
        while(fat->block_arr[fat->directory_starting_index + index] != ZERO && index < block_len) {
            // if the index is non-zero, jump to the next directory block
            // each directory entry is 64 bytes, and each array index is 2 bytes as it is uint16_t type
            // thus increment by 32
            index += 32;
        }
        // find an empty index in FAT REGION
        if (index >= block_len) {
            dir_full = true;
            index = 2;
            while(fat->block_arr[index] != 0X0000 && index < block_len) {
                index++;
            } 
            if(index >= fat->directory_starting_index) {
                printf("error: fat entry are all occupied");
                return FAILURE;
            }
            
        }
        // if directory block is available:
        if(!dir_full) {
            // writing the directory entry struct into the directory block
            directory_entry* entry_ptr = (directory_entry*) &fat->block_arr[fat->directory_starting_index+index];
            *entry_ptr = en;
            return SUCCESS;
        }
        // if we are using another block:
        fat->block_arr[1] = (uint16_t) index;
        fat->block_arr[index] = 0XFFFF;
        *reside_block = index;
        index = index*block_len;
        directory_entry* entry_ptr = (directory_entry*) &fat->block_arr[index];
        *entry_ptr = en;
        return SUCCESS;
    }
  
    // if directory block was already extended
    int curr_block = fat->block_arr[1];
    int prev = -1;
    while(curr_block!=0XFFFF) {
        prev = curr_block;
        curr_block = fat->block_arr[(int)curr_block];
    }
    *reside_block = prev;    // prev = 6 = reside block
    uint16_t index = 0;
    bool dir_full = false; 
    // int start_index = fat->directory_starting_index + (prev)*((int)block_len);    // 128 + 768 = 896
    int start_index = (prev)*((int)block_len);
    //increment 32 at a time
    while(fat->block_arr[start_index + index] != ZERO && index < block_len) {
        //if the index is non-zero, jump to the next directory block
        //each directory entry is 64 bytes, and each array index is 2 bytes as it is uint16_t type
        // thus increment by 32
        index += 32;
    }

    if (index >= block_len) {
        dir_full = true;
        index = 2;
        //find the next free block
        while(fat->block_arr[index] != 0X0000 && index < block_len/2) {
            index++;
        } 
        if(index >= fat->directory_starting_index) {
            printf("error: fat entry are all occupied");
            return FAILURE;
        }
            
    }
    if(!dir_full) {
        // writing the directory entry struct into the directory block
        directory_entry* entry_ptr = (directory_entry*) &fat->block_arr[start_index+index];
        *entry_ptr = en;
        return SUCCESS;
    }
    // if we are using another block:
    fat->block_arr[prev] = (uint16_t) index;
    *reside_block = index;
    fat->block_arr[index] = 0XFFFF;

    index = index*block_len;

    directory_entry* entry_ptr = (directory_entry*) &fat->block_arr[index];
    *entry_ptr = en;
    return SUCCESS;
   
}

int delete_directory_from_block(directory_entry en, FAT* fat) {
    // find a spot in file system
    // uint32_t entry_size = 0;
    // // # of FAT entries = block size * number of blocks in FAT / 2
    // entry_size = fat->block_size * fat->block_num;
    // //max filenum denots the maximum number of directory entries in a block
    // int max_filenum = entry_size / SIZE_DIRECTORY_ENTRY;
    uint16_t index = 0;
    // int b_index = 1;
    // while(fat->block_arr[b_index] != 0XFFFF) {
    //     index = block_arr[b_index];
    //     for(int i = 0; i<max_filenum; i++) {

    //     }
    // }
    while(fat->directory_starting_index + index < fat->dblock_starting_index) {
        //if the index is non-zero, jump to the next directory block
        //each directory entry is 64 bytes, and each array index is 2 bytes as it is uint16_t type
        // thus increment by 32
        directory_entry* curr_entry = (directory_entry*) &fat->block_arr[fat->directory_starting_index+index];
        if(strcmp(curr_entry->name, en.name) == 0) {
            for (int i = 0; i<32; i++) {
                fat->block_arr[fat->directory_starting_index + index + i] = ZERO;
            }
            return SUCCESS;
        }
        index += 32;
    }
    printf("file not found\n");
    return FAILURE;
}

file* read_file_from_fat(dir_node *f_node, FAT* fat) {
    if (f_node == NULL) {
        printf("error: %s not found\n", f_node->dir_entry->name);
        return NULL;
    }

    file *res = malloc(sizeof(file));
    res->file_bytes = read_file_bytes(f_node->dir_entry->firstBlock, f_node->dir_entry->size, fat);
    res->size = f_node->dir_entry->size;
    if (res == NULL)
        return NULL;

    return res;
}

uint8_t *read_file_bytes(uint16_t startIndex, uint32_t length, FAT *fat) {
    uint8_t *result = malloc(length * sizeof(uint8_t) + 1);
    if (result == NULL) {
        perror("malloc");
        return NULL;
    }
    result[length] = '\0';

    // read bytes from FAT storage for each block of this file
    // open the file to read from and check for errors
    int fd;
    if ((fd = open(fat->f_name, O_RDONLY, 0644)) == -1) {
        perror("open");
        free(result);
        return NULL;
    }

    // seek to the first empty block
    uint32_t fatSize = fat->block_num * fat->block_size;
    uint16_t currIndex = startIndex;

    if (lseek(fd, fatSize + ((currIndex - 1) * fat->block_size), SEEK_SET) == -1) {
        perror("lseek");
        free(result);
        return NULL;
    }

    // read all (length) bytes, finding a new block every time we read (blockSize) bytes
    for (int i = 0; i < length; i = i + fat->block_size) {
        if (i != 0 && i % fat->block_size == 0) {
            // get next block to start reading from
            currIndex = fat->block_arr[currIndex];
            if (lseek(fd, fatSize + ((currIndex - 1) * fat->block_size), SEEK_SET) == -1) {
                perror("lseek");
                free(result);
                return NULL;
            }
        }

        // If we are at the last block;
        int bytesToRead = fat->block_size;
        if (bytesToRead > length - i) {
            bytesToRead = length - i;
        }

        if (read(fd, &result[i], bytesToRead) == -1) {
            perror("read");
            free(result);
            return NULL;
        }
    }
    // close the file
    if (close(fd) == -1) {
        perror("close");
        free(result);
        return NULL;
        }

    return result;
}

int delete_file_bytes(uint16_t startIndex, uint32_t length, FAT *fat) {
    int fd;
    if ((fd = open(fat->f_name, O_WRONLY, 0644)) == -1) {
        perror("open");
    }
    if(length == 0) {
        printf("nothing in the file\n");
        return FAILURE;
    }
    // seek to the first empty block
    uint32_t fatSize = fat->block_num * fat->block_size;
    uint16_t currIndex = startIndex;

    if (lseek(fd, fatSize + ((currIndex - 1) * fat->block_size), SEEK_SET) == -1) {
        perror("lseek");
        return FAILURE;
    }

    // read all (length) bytes, finding a new block every time we read (blockSize) bytes
    for (int i = 0; i < length; i += fat->block_size) {
        if (i != 0 && i % fat->block_size == 0) {
            // get next block to start reading from
            currIndex = fat->block_arr[currIndex];
            if (lseek(fd, fatSize + ((currIndex - 1) * fat->block_size), SEEK_SET) == -1) {
                perror("lseek");
                return FAILURE;
            }
        }

        // If we are at the last block;
        int bytes_to_write = fat->block_size;
        if (bytes_to_write > length - i) {
            bytes_to_write = length - i;
        }
        
        // delete the content 
        uint8_t wipe = 0X00;
        int index = 0;
        while (index <= bytes_to_write) {
            if (write(fd, &wipe, 1) == -1) {
                perror("write");
                return FAILURE;
            }
            index ++;
        }

    }
    // close the file
    if (close(fd) == -1) {
        perror("close");
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

int free_file(file* file) {
    if(file->file_bytes!=NULL && file != NULL) {
        free(file->file_bytes);
        free(file);
        return SUCCESS;
    } else {
        printf("error: free_file, unable to free a NULL pointer");
        return FAILURE;
    }


}