#include "cosmogameplay.h"

#include <SDL_events.h>
#include <base/GsEventContainer.h>
#include <base/GsLogging.h>
#include <graphics/GsGraphics.h>


typedef unsigned short uint16;
typedef unsigned char  uint8;

extern "C"
{
    int run_gameplay();
    void load_current_level();
    void load_level(int level_number);
    bool executeLogics();
    void video_update();

    bool set_backdrop(uint16 backdrop_index);

    void set_backdropEv(int backdrop_index);

    void loadLevelEv(int level_number);

    uint8 get_episode_number();

    const char *get_level_filename(int level_number);

#include "files/file.h"
#include "tile.h"

    bool open_file(const char *filename, File *file);


    Tile *getBGTilesPtr();
    uint16 getNumBGTiles();
}

struct SetBackdropEvent : public CEvent
{

    SetBackdropEvent(const int index) :
        mIndex(index) {}
    const int mIndex = 0;
};

struct LoadLevelEvent : public CEvent
{
    LoadLevelEvent(const int level) :
        mLevel(level) {}
    const int mLevel = 0;
};


void loadLevelEv(int level_number)
{
    gEventManager.add( new LoadLevelEvent(level_number) );
}

void set_backdropEv(int backdrop_index)
{
    gEventManager.add( new SetBackdropEvent(backdrop_index) );
}

CosmoGameplay::CosmoGameplay()
{

}


bool CosmoGameplay::start()
{
    load_current_level();
    return true;
}


bool CosmoGameplay::load_level_data(int level_number)
{

    //byte_28BE3 = 0;

    File map_file;
    if(!open_file(get_level_filename(level_number), &map_file))
    {
        gLogging.ftextOut("Error: loading level data. %s\n<br>", get_level_filename(level_number));
        return false;
    }

    file_seek(&map_file, 2);
    const auto map_width_in_tiles = file_read2(&map_file);
    const auto map_height_in_tiles = 32768 / map_width_in_tiles - 1;

    mMap.setupEmptyDataPlanes(3, map_width_in_tiles, map_height_in_tiles);

    gLogging.ftextOut("map width (in tiles): %d\n", map_width_in_tiles);


    /*
    switch(map_width_in_tiles)
    {
        case 32: map_stride_bit_shift_amt = 5; break;
        case 64: map_stride_bit_shift_amt = 6; break;
        case 128: map_stride_bit_shift_amt = 7; break;
        case 256: map_stride_bit_shift_amt = 8; break;
        case 512: map_stride_bit_shift_amt = 9; break;
        case 1024: map_stride_bit_shift_amt = 10; break;
        case 2048: map_stride_bit_shift_amt = 11; break;
        default: break;
    }
*/


    uint16 actor_data_size_in_words = file_read2(&map_file);

    int total_num_actors = 0;
    uint16 num_moving_platforms = 0;
    uint16 num_mud_fountains = 0;
    //clear_brightness_objs();
    //brightness_effect_enabled_flag = 1;
    //obj_switch_151_flag = 0;

    const int MAX_ACTORS = 410;

    for(int i=0;i< actor_data_size_in_words/3;i++)
    {
        uint16 actor_type = file_read2(&map_file);
        uint16 x = file_read2(&map_file);
        uint16 y = file_read2(&map_file);
        if(total_num_actors < MAX_ACTORS)
        {
            //load_actor(total_num_actors, actor_type, x, y);
        }
    }


    const int MAX_MAP_TILES = 32768;
    uint16 map_data[MAX_MAP_TILES];


    for(int i=0;i < MAX_MAP_TILES - 4; i++)
    {
        map_data[i] = file_read2(&map_file);
    }

    // Background stuff
    /*
    word *ptr = mMap.getData(0);

    for(int i=0;i<num_moving_platforms;i++)
    {
        for(int j = 2;j < 7;j++)
        {
            moving_platform_tbl[i].map_tiles[j-2] = map_data[moving_platform_tbl[i].x + moving_platform_tbl[i].y * map_width_in_tiles + j - 4];
        }

    }

    current_level = (uint16)level_number;
    map_max_y_offset = 0x10000 / (map_width_in_tiles * 2) - 19;
    */

    return true;
}


bool CosmoGameplay::setBackdrop(const int index)
{
    /*
    static uint8 cur_background_x_scroll_flag = 0;
    static uint8 cur_background_y_scroll_flag = 0;

    if (new_backdrop_index != backdrop_index ||
            cur_background_x_scroll_flag != background_x_scroll_flag ||
            cur_background_y_scroll_flag != background_y_scroll_flag)
    {
        backdrop_index = new_backdrop_index;
        cur_background_x_scroll_flag = background_x_scroll_flag;
        cur_background_y_scroll_flag = background_y_scroll_flag;

        load_backdrop_image(backdrop_filename_tbl[backdrop_index]);
    }

    */

    //auto dataPtr = mMap.getData(0);

    gGraphics.Palette.setupColorPalettes(nullptr, 0);

    GsTilemap &tilemap = gGraphics.getTileMap(0);
    const auto num_bg_Tiles = getNumBGTiles();
    tilemap.CreateSurface( gGraphics.Palette.m_Palette, SDL_SWSURFACE,
                           num_bg_Tiles, 3, 40 );

    Tile *bg_tiles = getBGTilesPtr();
    SDL_Surface *sfc = tilemap.getSDLSurface();
    SDL_FillRect(sfc, nullptr, 0);
    if(SDL_MUSTLOCK(sfc))   SDL_LockSurface(sfc);

    for(int t=0 ; t<num_bg_Tiles ; t++)
    {
        Tile *tile = &(bg_tiles[t]);

        const uint16 tx = (t%40);
        const uint16 ty = (t/40);

        const int x = tx*8;
        const int y = ty*8;

        uint8 *pixel = (uint8 *)sfc->pixels + x + y * 320;
        uint8 *tile_pixel = tile->pixels;
        for(int i=0 ; i<TILE_HEIGHT ; i++)
        {
            for(int j=0 ; j < TILE_WIDTH ; j++)
            {
                if(tile_pixel[j] != TRANSPARENT_COLOR)
                {
                    pixel[j] = tile_pixel[j];
                }
            }
            pixel += 320;
            tile_pixel += TILE_WIDTH;
        }

        /*
        if(exportGfx)
        {
            exportGfxToFile(sfc, std::string("BACKDROP") + std::string(".bmp"));
        }*/

/*
        video_draw_tile_with_clip_rect(
            &bg_tiles[((x+x_offset) % BACKGROUND_WIDTH) +
                      ((y+y_offset) % BACKGROUND_HEIGHT) * BACKGROUND_WIDTH],
            (x+1)*8 - sub_tile_x, (y+1)*8 - sub_tile_y,
             8, 8,
             8*MAP_WINDOW_WIDTH,
             8*MAP_WINDOW_HEIGHT);
*/

        /*
        std::vector<unsigned char> &data = m_egagraph.at(IndexOfTiles).data;

        const Uint16 size = (1 << pbasetilesize);

        const size_t tileSize = 4 * (size / 8) * size;

        const auto exportGfx = exportArgEnabled();

        if(tileoff)
        {
            size_t expectedSize = NumTiles * rowlength * tileSize;
            if(!data.empty() && data.size() != expectedSize)
            {
                gLogging.ftextOut("bad tile offset data expected size=%u data size=%u", expectedSize, data.size());
            }
        }
        */
    }

    if(SDL_MUSTLOCK(sfc))   SDL_UnlockSurface(sfc);

    if(SDL_SaveBMP(sfc, "/tmp/backdrop.bmp") != 0)
    {
        // Error saving bitmap
        gLogging.ftextOut("SDL_SaveBMP failed: %s\n", SDL_GetError());
        return false;
    }


    return true;
}

bool CosmoGameplay::loadLevel(const int level_number)
{
    gGraphics.createEmptyTilemaps(4);

    File map_file;
    if(!open_file(get_level_filename(level_number), &map_file))
    {
        gLogging.ftextOut("Error: loading level data. %s\n", get_level_filename(level_number));
        return false;
    }
    const uint16 level_flags = file_read2(&map_file);
    file_close(&map_file);

/*
    stop_music();
    rain_flag = (uint8)(level_flags & 0x20);
    */
    const uint16 backdrop_index = (uint16)(level_flags & 0x1f);
    /*
    background_x_scroll_flag = (uint8)(level_flags & 0x40);
    background_y_scroll_flag = (uint8)(level_flags & 0x80);

    palette_anim_type = (uint8)(level_flags >> 8 & 7);
    uint16 music_index = (uint16)((level_flags >> 11) & 0x1f);

    printf("Level %d: rain=%d, backdrop_index=%d, bg_x_scroll=%d, bg_y_scroll=%d, pal_anim_type=%d, music_index=%d\n", current_level, rain_flag, backdrop_index, background_x_scroll_flag, background_y_scroll_flag, palette_anim_type, music_index);
    reset_game_state();

*/


    load_level_data(level_number);

    setBackdrop(backdrop_index);
/*
    if (level_number == 0 && show_one_moment_screen_flag != 0) {
        fade_to_black_speed_3();
        show_one_moment_screen_flag = 0;
    }
    if (game_play_mode == 0) {
        switch (level_number) {
            case 0:
            case 1:
            case 4:
            case 5:
            case 8:
            case 9:
            case 12:
            case 13:
            case 16:
            case 17:
                video_fill_screen_with_black();
                fade_in_from_black_with_delay_3();
                now_entering_level_n_dialog(level_number);
                wait_for_time_or_key(0x96);
                fade_to_black_speed_3();
                break;
            default:
                break;
        }
    }
    explode_effect_clear_sprites();
    struct6_clear_sprites();
    effect_clear_sprites();
    player_reset_push_variables();
    actor_toss_clear_all();
    status_panel_init();

    write_savegame_file('T');
    load_music(music_index);


*/


    return true;
}


void CosmoGameplay::pumpEvent(const CEvent *evPtr)
{
    if( const auto* backdrop = dynamic_cast<const SetBackdropEvent*>(evPtr) )
    {
    }
    else if( const auto* levelEv = dynamic_cast<const LoadLevelEvent*>(evPtr) )
    {
        loadLevel(levelEv->mLevel);
    }
}



void CosmoGameplay::ponder(const float deltaT)
{
    /*
    int input_state = process_ext_input();

    if (input_state == QUIT)
    {
        //return false;
        return;
    }

    if (input_state == PAUSED)
    {
        //return true;
        return;
    }
    */

    executeLogics();
    //run_gameplay();
}

void CosmoGameplay::render()
{
    //video_update();
}
