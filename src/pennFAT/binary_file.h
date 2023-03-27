#ifndef FILE_H
#define FILE_H

#include <stdbool.h>
#include "FAT.h"

typedef struct binary_file {
    uint8_t* byte_sequence;
    int length;
    uint8_t perm;
    uint8_t type;
} b_file;

int fat_write_file(char* f_name);