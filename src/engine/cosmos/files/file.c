//
// Created by Eric Fry on 1/11/2017.
//

#include <SDL_filesystem.h>
#include "file.h"

uint32 calculate_filesize(FILE *fp) {
    uint32 file_length;

    fseek(fp, 0, SEEK_END);
    file_length = (uint32)ftell(fp);
    fseek(fp, 0, SEEK_SET);

    return file_length;
}

uint32 file_get_current_position(File *file)
{
    return file ? file->pos : 0;
}

const char *get_full_SDL_path(const char *filename) {
    char *path = SDL_GetBasePath();
    if (!path) {
        return filename;
    } else {
        const size_t maxlen = SDL_strlen(path) + SDL_strlen(filename) + 1;
        path = (char *) SDL_realloc(path, maxlen);
        SDL_strlcat(path, filename, maxlen);
        return path;
    }
}

bool file_open(const char *filename, const char *mode, File *file) {
    file->fp = fopen(filename, mode);

    if(file->fp == NULL)
    {
        printf("Failed opening '%s'\n",filename);
        return false;
    }

    file->size = calculate_filesize(file->fp);
    file->pos = 0;
    file->initial_offset = 0;

    return true;
}

bool file_open_at_offset(const char *filename, const char *mode, File *file, uint32 offset, uint32 size) {
    file->fp = fopen(filename, mode);

    if(file->fp == NULL)
    {
        printf("Failed opening '%s'\n",filename);
        return false;
    }

    uint32 actual_size = calculate_filesize(file->fp);

    if(offset + size > actual_size)
    {
        printf("Failed opening '%s' offset + size > actual_size\n",filename);
        fclose(file->fp);
        return false;
    }

    fseek(file->fp, offset, SEEK_SET);
    file->initial_offset = offset;

    file->pos = 0;
    file->size = size;

    return true;
}

uint32 file_get_filesize(File *file) {
    return file->fp != NULL ? file->size : 0;
}

uint8 file_read1(File *file) {
    if(!file->fp || file->pos >= file->size)
        return 0;

    file->pos++;

    return(uint8)fgetc(file->fp);
}

uint16 file_read2(File *file) {
    uint8 b0, b1;

    if(!file->fp || file->pos > file->size-2)
        return 0;

    file->pos += 2;

    b0 = (uint8)fgetc(file->fp);
    b1 = (uint8)fgetc(file->fp);

    return (b0 + (b1<<8));
}

uint32 file_read4(File *file) {
    uint8 b0, b1, b2, b3;
    if(!file->fp || file->pos > file->size-4)
        return 0;

    file->pos += 4;

    b0 = (uint8)fgetc(file->fp);
    b1 = (uint8)fgetc(file->fp);
    b2 = (uint8)fgetc(file->fp);
    b3 = (uint8)fgetc(file->fp);

    return (b0 + (b1<<8) + (b2<<16) + (b3<<24));
}

uint32 file_read_to_buffer(File *file, unsigned char *buf, uint32 num_bytes) {
    if(file->pos + num_bytes > file->size)
        num_bytes = file->size - file->pos;

    fread(buf,1,num_bytes,file->fp); // FIX for partial read.

    file->pos += num_bytes;

    return num_bytes;
}

void file_write1(uint8 byte, File *file)
{
    fputc(byte,file->fp);

    file->pos++;

    if(file->pos > file->size)
        file->size = file->pos;
}

void file_write2(uint16 word, File *file)
{
    fputc((uint8)(word & 0xff), file->fp);
    fputc((uint8)((word >> 8) & 0xff), file->fp);

    file->pos += 2;

    if(file->pos > file->size)
        file->size = file->pos;
}

void file_write4(uint32 double_word, File *file)
{
    fputc((uint8)(double_word & 0xff), file->fp);
    fputc((uint8)((double_word >> 8) & 0xff), file->fp);
    fputc((uint8)((double_word >> 16) & 0xff), file->fp);
    fputc((uint8)((double_word >> 24) & 0xff), file->fp);

    file->pos += 4;

    if(file->pos > file->size)
        file->size = file->pos;
}

void file_close(File *file) {
    if(file->fp)
    {
        fclose(file->fp);
        file->fp = NULL;
    }
}

void file_seek(File *file, uint32 new_position) {
    if(file->fp && new_position <= file->size)
    {
        fseek(file->fp,file->initial_offset + new_position,SEEK_SET);
        file->pos = new_position;
    }

    return;
}


