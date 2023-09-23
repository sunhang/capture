//
// Created by sunhang on 2023/9/21.
//

#include <stdio.h>


static int index = 0;

FILE *open_save_file(char *parent_path) {
    char str[128];
    sprintf(str, "%s/%d", parent_path, index);
    FILE *fp = fopen(str, "ab+");

    return fp;
}

void close_save_file(FILE *file) {
    fclose(file);
}

void write_yuv_data(const char *parent_path, void *buffer, int len) {
    FILE *file = open_save_file(parent_path);
    fwrite(buffer, 1, len, file);
    close_save_file(file);
    index++;
}

