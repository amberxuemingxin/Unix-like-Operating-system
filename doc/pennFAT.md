# PennFAT API Spec

pennFAT is designed as a custom file system in pennOS. It is based on FAT16, and for the sake of this project, it will only have a root directory.

# PennFAT files:
    - FAT.h
    - FAT.c
    - macro.h
    - pennFAT.c
    - pennfatlib.h
    - pennfatlib.c

    # FAT.h
    - struct: directory, directory_node, FAT
        - diretory:
            A custome datastructure used to store the meta data of each of the file to be stored in FAT.
        - directory_nodeL
            A linkedlist datastructure used as a Wrapper struct to store a directory ptr and a next ptr pointing to the next directory
        - FAT:
            File Allocation Table: stores metadata of a file system, as well as a pointer to the block. 
    - Function APIs:            
        - dir_node * new_directory_node(char* f_name, uint32_t size, uint16_t firstBlock, uint8_t type, uint8_t perm, time_t time):
            Initiate a new directory_node, given the filename, size of the file, first_empty block index, type of file, permission, and modified time

        - void free_directory_node(dir_node *node);
            Free the specified directory_node   

        - FAT* make_fat(char* f_name, uint8_t num_blocks, uint8_t block_size);
            Make a fat table given, fat_name, number of blocks(1-32), and blocksize(0-4)
        - void free_fat(FAT* fat);
            Free the specified fat table.

    # pennfatlib.h
        int parse_pennfat_command(char ***commands, int commandCount, FAT **fat):
            given user input command, figure out what shell-function is, and jump to the specified 
            api. If fat is not yet initialized, print error message

        int pennfat_mkfs(char *f_name, uint8_t block_num, uint8_t block_size, FAT **FAT);
            make a file system, based on given parameter, user specified block_num and block_size
        int pennfat_mount(char *fileName, FAT **fat);

        int pennfat_unmount(FAT **fat);

        int pennfat_touch(char **files, FAT *fat);
            given an array of files, touch command will create a new file if the f_name is not in FAT
            or update the m_time using time(2)
        int pennfat_mv(char *oldFileName, char *newFileName, FAT *fat);

        int pennfat_remove(char **files, FAT *fat);
            handles "rm xxx" commands, this will delete the entry node as well as updates FAT's meta data.
        int pennfat_cat(char **commands, FAT *fat);

        int pennfat_cp(char **commands, FAT *fat);

        int pennfat_ls(FAT *fat);
            prints out all current file in the Filesytem and last time it was modified. 
        int pennfat_chmod(char **commands, FAT *fat);

        dir_node* search_file(char* file_name, FAT* fat, dir_node** prev);
            given a file_name, this function iterates the linkedlist to find the dir_node containing the file 
            with file_name. prev is a return parameter used to contain the previous node of the found file.

    # macro.h:
        created to store macros used in pennFAT such as permission macros, 
