//
// Created by Eric Fry on 1/11/2017.
//

#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include "vol.h"
#include "file.h"
#include "config.h"

#define MAX_FILES 200
#define FILENAME_LEN 12

unsigned char *extract(File *vol_file, uint16 index, unsigned char *buf, uint32 buf_size, uint32 *bytesRead)
{
    file_seek(vol_file, index * 20 + FILENAME_LEN);
    uint32 offset = file_read4(vol_file);
    uint32 size = file_read4(vol_file);
    file_seek(vol_file, offset);

    if (buf == NULL)
    {
        buf = (unsigned char *)malloc(size);
    } else
    {
        if (buf_size < size) {
            size = buf_size;
        }
    }

    file_read_to_buffer(vol_file, buf, size);

    *bytesRead = size;

    return buf;
}

uint16 get_index_of_file(File *vol_file, const char *filename)
{
    char volFilenameBuf[FILENAME_LEN+1];

    uint16 i=0;
    for(;i<MAX_FILES;i++) {
        file_seek(vol_file, i*20);
        file_read_to_buffer(vol_file, (unsigned char *)volFilenameBuf, FILENAME_LEN);
        volFilenameBuf[FILENAME_LEN] = 0;
        if(strcmp(filename, volFilenameBuf) == 0) {
            return i;
        }
    }

    return MAX_FILES;
}

bool open_vol_file(const char *vol_filename, File *vol_file)
{
    char *fullpath = get_game_dir_full_path(vol_filename);
    if(!file_open(fullpath, "rb", vol_file))
    {
        SDL_free(fullpath);
        printf("Error: opening %s\n", vol_filename);
        return false;
    }
    SDL_free(fullpath);
    return true;
}

unsigned char *vol_file_extract_by_name(const char *vol_filename, const char *filename, uint32 *bytesRead)
{
    File vol_file;
    assert(open_vol_file(vol_filename, &vol_file));

    uint16 index = get_index_of_file(&vol_file, filename);

    if(index == MAX_FILES)
    {
        return NULL;
    }

    unsigned char *data = extract(&vol_file, index, NULL, 0, bytesRead);

    file_close(&vol_file);

    return data;
}

unsigned char *vol_file_load(const char *vol_filename, const char *filename, unsigned char *buffer, uint32 buffer_size, uint32 *bytesRead)
{
    File vol_file;
    assert(open_vol_file(vol_filename, &vol_file));

    uint16 index = get_index_of_file(&vol_file, filename);

    if(index == MAX_FILES)
    {
        file_close(&vol_file);
        return NULL;
    }

    unsigned char *data = extract(&vol_file, index, buffer, buffer_size, bytesRead);

    file_close(&vol_file);

    return data;
}

bool vol_file_open(const char *vol_filename, const char *filename, File *file)
{
    File vol_file;
    assert(open_vol_file(vol_filename, &vol_file));

    uint16 index = get_index_of_file(&vol_file, filename);

    if(index == MAX_FILES)
    {
        file_close(&vol_file);
        return false;
    }

    file_seek(&vol_file, index * 20 + FILENAME_LEN);
    uint32 offset = file_read4(&vol_file);
    uint32 size = file_read4(&vol_file);
    file_close(&vol_file);

    char *fullpath = get_game_dir_full_path(vol_filename);
    if(!file_open_at_offset(fullpath, "rb", file, offset, size))
    {
        SDL_free(fullpath);
        return false;
    }
    SDL_free(fullpath);
    return true;
}
