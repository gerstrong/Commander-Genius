//
// Created by Eric Fry on 1/11/2017.
//

#ifndef COSMO_ENGINE_FILE_H
#define COSMO_ENGINE_FILE_H

#include <stdio.h>
#include <stdbool.h>
#include "defines.h"

typedef struct {
    FILE *fp;
    uint32 size;
    uint32 pos;
    uint32 initial_offset;
} File;

bool file_open(const char *filename, const char *mode, File *file); //FIXME is bool c99?
bool file_open_at_offset(const char *filename, const char *mode, File *file, uint32 offset, uint32 size); //FIXME is bool c99?
uint32 file_get_filesize(File *file);
uint32 file_get_current_position(File *file);

void file_seek(File *file, uint32 new_position);
uint8 file_read1(File *file);
uint16 file_read2(File *file);
uint32 file_read4(File *file);
uint32 file_read_to_buffer(File *file, unsigned char *buf, uint32 num_bytes);

void file_write1(uint8 byte, File *file);
void file_write2(uint16 word, File *file);
void file_write4(uint32 double_word, File *file);

void file_close(File *file);

#endif //COSMO_ENGINE_FILE_H
