//
// Created by Eric Fry on 31/10/2017.
//

#include "files/file.h"
#include <stdlib.h>
#include "game.h"
#include "tile.h"

uint8 tileattr_mni_data[7000];

void tile_attr_load()
{
    load_file("TILEATTR.MNI", (unsigned char *)&tileattr_mni_data, 7000);
}
uint8 get_tile_size(TileType type)
{
    return type == SOLID ? (uint8)32 : (uint8)40;
}
uint16 get_number_of_tiles(File *file, TileType type)
{
    uint8 tile_size = get_tile_size(type);

    uint16 num_tiles = 0;
    uint32 filesize = file->size;
    while(filesize > 0xffff)
    {
        num_tiles += 0xffff / tile_size;
        filesize -= 0xffff;
    }

    if(filesize > 0)
    {
        num_tiles += filesize / tile_size;
    }

    return num_tiles;
}

uint8 getPixelAtColumnPosition(uint8 *rowPtr, uint8 column, TileType tileType)
{
    uint8 color = 0;

    if (tileType == TRANSPARENT) {
        if (*rowPtr & (1 << (7 - column))) {
            return TRANSPARENT_COLOR;
        }
        rowPtr++;
    }

    if (tileType == FONT) //Font tiles have an inverted mask layer.
    {
        if ((*rowPtr & (1 << (7 - column))) == 0) {
            return TRANSPARENT_COLOR;
        }
        rowPtr++;
    }

    for (int i = 0; i < 4; i++, rowPtr++) {

        color |= (((*rowPtr >> (7 - column)) & 1) << (i));
    }

    return color;
}

void load_tile(uint8 *planarData, TileType type, Tile *tile)
{
    tile->type = type;
    uint8 *pixelPtr = tile->pixels;

    for(uint8 y=0;y<TILE_HEIGHT;y++) {
        for(uint8 x=0;x<TILE_WIDTH;x++) {
            *pixelPtr = getPixelAtColumnPosition(planarData, x, type);
            pixelPtr++;
        }
        planarData += type == SOLID ? (uint8)4 : (uint8)5;
    }
}

Tile *load_tiles(const char *filename, TileType type, uint16 *num_tiles_loaded) {
    uint8 planarData[40];

    File file;
    if(!open_file(filename, &file))
    {
        printf("Error: opening %s\n", filename);
        return NULL;
    }

    uint8 tile_size = get_tile_size(type);

    uint16 num_tiles = get_number_of_tiles(&file, type);
    Tile *tiles = (Tile *)malloc(sizeof(Tile) * num_tiles);


    for(int i=0;i<num_tiles;i++)
    {
        if(i != 0 && i % (0xffff / tile_size) == 0)
        {
            file_seek(&file, file.pos + (0xffff % tile_size)); //skip the last (unused) bytes from the block.
        }
        file_read_to_buffer(&file, planarData, tile_size);
        load_tile(planarData, type, &tiles[i]);
    }

    *num_tiles_loaded = num_tiles;
    file_close(&file);
    return tiles;
}

uint16 get_number_of_sprite_frames(uint16 current_frame_num, uint16 offset, uint16 num_total_sprites, File *file)
{
    uint16 next_offset = (uint16)(file->size / 2);
    if(current_frame_num < num_total_sprites - 1)
    {
        next_offset = file_read2(file);
    }

    return (uint16)((next_offset - offset) / 4);
}

Sprite *load_tile_info(const char *filename, uint16 *num_records_loaded)
{
    File file;
    if(!open_file(filename, &file))
    {
        printf("Error: opening tileinfo file %s\n", filename);
        return NULL;
    }

    *num_records_loaded = (uint16)file_read2(&file);
    Sprite *sprites = (Sprite *)malloc((size_t)*num_records_loaded * sizeof(Sprite));

    for(uint16 sprite_index=0;sprite_index < *num_records_loaded; sprite_index++)
    {
        sprites[sprite_index].num_frames = 0;
        for(int j=sprite_index; sprites[sprite_index].num_frames == 0 && j < *num_records_loaded; j++) //advance through the sprite info offsets until we find some frames
        {
            file_seek(&file, (uint32) j * 2);
            sprites[sprite_index].num_frames = 0;

            uint16 offset = file_read2(&file);
            sprites[sprite_index].num_frames = get_number_of_sprite_frames(j, offset, *num_records_loaded, &file);

            if (sprites[sprite_index].num_frames != 0)
            {
                file_seek(&file, (uint32) offset * 2);

                sprites[sprite_index].frames = (TileInfo *) malloc((size_t) sprites[sprite_index].num_frames * sizeof(TileInfo));

                for (int frame_num = 0; frame_num < sprites[sprite_index].num_frames; frame_num++)
                {
                    sprites[sprite_index].frames[frame_num].height = file_read2(&file);
                    sprites[sprite_index].frames[frame_num].width = file_read2(&file);
                    uint32 frameOffset = file_read4(&file);
                    sprites[sprite_index].frames[frame_num].tile_num = (uint16)((frameOffset >> 16) * 1638 + (frameOffset & 0xffff) / 40);
                }
            }
        }
    }

    file_close(&file);

    return sprites;
}

