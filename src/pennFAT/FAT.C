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


void free_fat(FAT** fat){
    printf("freed!\n");

    // struct FAT *curr_fat = *fat;
    // if (curr_fat == NULL) return;


    // if(curr_fat->f_name != NULL) {
    //     free(curr_fat->f_name);
    // }

}
    