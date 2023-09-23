//
// Created by sunhang on 2023/9/22.
//

#include <stdio.h>
#include <stdint.h>
#include <malloc.h>

void read(uint8_t **bytes, uint32_t *len) {
    FILE *p = fopen("/home/sunhang/Documents/mycode/car/workspace/out/0", "rb");
    uint8_t *buffer = malloc(460800 * sizeof(uint8_t));
    fread(buffer, 1, 460800, p);
    fclose(p);
}