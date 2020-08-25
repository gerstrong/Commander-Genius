#include "cosmogameplay.h"

#include <SDL_events.h>
#include <base/GsEventContainer.h>
#include <base/GsLogging.h>
#include <base/video/CVideoDriver.h>
#include <engine/core/CBehaviorEngine.h>
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


    Tile *getBackdropTilesPtr();
    uint16 getNumBackdropTiles();

    extern int mapwindow_x_offset;

    int getMapWidth();
    int getMapHeight();

    uint16 map_get_tile_cell(int x, int y);

    uint16 getNumBgTiles();
    uint16 getNumFgTiles();

    Tile *map_get_bg_tile(uint16 tile_num);
    Tile *map_get_fg_tile(uint16 tile_num);

    uint16 *map_data_ptr();
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

/*
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
*/
/*
    const int MAX_MAP_TILES = 32768;
    uint16 map_data[MAX_MAP_TILES];


    for(int i=0;i < MAX_MAP_TILES - 4; i++)
    {
        map_data[i] = file_read2(&map_file);
    }
*/
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


auto extractTilemap(GsTilemap &tilemap,
                    const Tile *tiles,
                    const int num_tiles) -> bool
{
    SDL_Surface *sfc = tilemap.getSDLSurface();
    const auto col = tilemap.getNumColumn();

    SDL_FillRect(sfc, nullptr, 0);
    if(SDL_MUSTLOCK(sfc))   SDL_LockSurface(sfc);

    for(int t=0 ; t<num_tiles ; t++)
    {
        const Tile *tile = &(tiles[t]);

        const uint16 tx = (t%col);
        const uint16 ty = (t/col);

        const int x = tx*8;
        const int y = ty*8;

        uint8 *pixel = (uint8 *)sfc->pixels + x + y * 320;
        const uint8 *tile_pixel = tile->pixels;
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
/*
    if(SDL_SaveBMP(sfc, "/tmp/backdrop.bmp") != 0)
    {
        // Error saving bitmap
        gLogging.ftextOut("SDL_SaveBMP failed: %s\n", SDL_GetError());
        return false;
    }*/

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

    GsTilemap &backdroptilemap = gGraphics.getTileMap(0);
    const auto num_backdrop_Tiles = getNumBackdropTiles();
    backdroptilemap.CreateSurface( gGraphics.Palette.m_Palette, SDL_SWSURFACE,
                           num_backdrop_Tiles, 3, 40 );


    extractTilemap(backdroptilemap,
                   getBackdropTilesPtr(),
                   num_backdrop_Tiles);


    return true;
}

bool CosmoGameplay::loadLevel(const int level_number)
{
    gGraphics.freeTilemap();
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



    GsTilemap &tilemap1 = gGraphics.getTileMap(1);
    const auto num_bg_Tiles = getNumBgTiles();
    tilemap1.CreateSurface( gGraphics.Palette.m_Palette, SDL_SWSURFACE,
                           num_bg_Tiles, 3, 40 );

    auto *tiles1 = map_get_bg_tile(0);
    extractTilemap(tilemap1, tiles1, num_bg_Tiles);


    GsTilemap &tilemap2 = gGraphics.getTileMap(2);
    const auto num_fg_Tiles = getNumFgTiles();
    tilemap2.CreateSurface( gGraphics.Palette.m_Palette, SDL_SWSURFACE,
                           num_fg_Tiles, 3, 40 );

    auto *tiles2 = map_get_fg_tile(0);
    extractTilemap(tilemap2, tiles2, num_fg_Tiles);

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


        //////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////
        /// //////////////////////////////////////////////////////////////////
        /// //////////////////////////////////////////////////////////////////
        /// //////////////////////////////////////////////////////////////////v



    /* Here we pass all the from loaded stuff to the Map Object   */

    // TODO: Maybe pass all loaded data to the mMap Object
    //bool loadMap(CMap &Map, Uint8 level);

    //////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////
    /// //////////////////////////////////////////////////////////////////
    /// //////////////////////////////////////////////////////////////////
    ///


    //bool ok = true;

    // Get the MAPHEAD Location from within the Exe File or an external file
    std::vector<char> mapHeadContainer;
    //const std::string &path = gKeenFiles.gameDir;

    // Set Map position and some flags for the freshly loaded level
    mMap.gotoPos(0,0);
    mMap.setLevel(level_number);
    mMap.isSecret = false;
    mMap.mNumFuses = 0;

    /*
    // In case no external file was read, let's use data from the embedded data
    byte *Maphead = gKeenFiles.exeFile.getRawData() + getMapheadOffset();

    // In case there is an external file read it into the container and replace the pointer
    const std::string mapHeadFilename = gKeenFiles.mapheadFilename;
    std::ifstream MapHeadFile;

    if( OpenGameFileR(MapHeadFile, getResourceFilename(mapHeadFilename,path,false,false), std::ios::binary) )
    {
        // get length of file:
        MapHeadFile.seekg (0, std::ios::end);
        unsigned int length = MapHeadFile.tellg();
        MapHeadFile.seekg (0, std::ios::beg);
        mapHeadContainer.resize(length);
        MapHeadFile.read(&mapHeadContainer[0],length*sizeof(char));
        Maphead = reinterpret_cast<byte*>(&mapHeadContainer[0]);
    }

    word magic_word;
    longword level_offset;

    // Get the magic number of the level data from MAPHEAD Located in the EXE-File.
    // This is used for the decompression.
    magic_word = READWORD(Maphead);

    // Get location of the level data from MAPHEAD Located in the EXE-File.
    Maphead += level*sizeof(longword);
    level_offset = READLONGWORD(Maphead);

    // Open the Gamemaps file
    std::string gamemapfile = gKeenFiles.gamemapsFilename;

    std::ifstream MapFile;
    if(OpenGameFileR(MapFile,
                     getResourceFilename(gamemapfile,path,true,false), std::ios::binary))
    {
        if(level_offset == 0 && mapHeadContainer.empty())
        {
            MapFile.close();
            gLogging.textOut("This Level doesn't exist in GameMaps");
            return false;
        }

        // Then jump to that location and read the level map data
        MapFile.seekg (level_offset, std::ios::beg);

        int headbegin;

        // Get the level plane header
        if(gotoNextSignature(MapFile))
        {

            int jumpback = 3*sizeof(longword) + 3*sizeof(word) +
                    2*sizeof(word) + 16*sizeof(byte) + 4*sizeof(byte);

            headbegin = static_cast<int>(MapFile.tellg()) - jumpback;
        }
        else
        {
            MapFile.clear();
            headbegin =  level_offset;
        }

        MapFile.seekg( headbegin, std::ios_base::beg);

        // Get the header of level data
        longword Plane_Offset[3];
        longword Plane_Length[3];
        word Width, Height;
        char name[17];

        // Get the plane offsets
        Plane_Offset[0] = fgetl(MapFile);
        Plane_Offset[1] = fgetl(MapFile);
        Plane_Offset[2] = fgetl(MapFile);

        // Get the dimensions of the level
        Plane_Length[0] = fgetw(MapFile);
        Plane_Length[1] = fgetw(MapFile);
        Plane_Length[2] = fgetw(MapFile);

        Width = fgetw(MapFile);
        Height = fgetw(MapFile);


        if(Width>1024 || Height>1024)
        {
            gLogging.textOut("Sorry, but I cannot uncompress this map and must give up."
                             "Please report this to the developers and send that version to them in order to fix it.<br>" );
            return false;
        }


        for(int c=0 ; c<16 ; c++)
        {
            name[c] = MapFile.get();
        }
        name[16] = '\0';

        // Get and check the signature
        gLogging.textOut("Loading the Level \"" + static_cast<std::string>(name) + "\" (Level No. "+ itoa(level) + ")<br>" );
        Map.setLevelName(name);

        mLevelName = name;

        // Then decompress the level data using rlew and carmack decompression
        gLogging.textOut("Allocating memory for the level planes ...<br>" );
*/
        // Start with the Background

    CTileProperties emptyTileProperties;

    std::vector<CTileProperties> &bgTileProperties = gBehaviorEngine.getTileProperties(0);
    bgTileProperties.assign(getNumBgTiles(), emptyTileProperties);
    std::vector<CTileProperties> &fgTileProperties = gBehaviorEngine.getTileProperties(1);
    fgTileProperties.assign(getNumFgTiles(), emptyTileProperties);

    const auto width = getMapWidth();
    const auto height = getMapHeight();
        mMap.setupEmptyDataPlanes(3, width, height);

        gLogging.textOut("Reading plane 0 (Background)<br>" );
        //ok &= unpackPlaneData(MapFile, Map, 0, Plane_Offset[0], Plane_Length[0], magic_word);


        auto *map0_data = mMap.getData(0);
        auto *map1_data = mMap.getData(1);

        auto *cosmo_map_data = map_data_ptr();
        for(int i = 0 ; i<height*width ; i++)
        {
            const auto map_cell = cosmo_map_data[i];

            if(map_cell < bgTileProperties.size())
            {
                const uint16 tile = map_cell/8;
                map0_data[i] = tile;
            }


            if(map_cell < fgTileProperties.size())
            {
                const uint16 tile =  ((map_cell/8) - 2000) / 5;
                map1_data[i] = tile;
            }
        }

        /*
        auto *map_data = mMap.getData(0);
        memcpy(map_data, map_data_ptr(), width*height*sizeof(uint16));
        */

        //gLogging.textOut("Reading plane 1 (Foreground)<br>" );
        //ok &= unpackPlaneData(MapFile, Map, 1, Plane_Offset[1], Plane_Length[1], magic_word);

        mMap.collectBlockersCoordiantes();
        mMap.setupAnimationTimer();

        // Now that we have all the 3 planes (Background, Foreground, Foes) unpacked...
        // We only will show the first two of them in the screen, because the Foes one
        // is the one which will be used for spawning the foes (Keen, platforms, enemies, etc.)
        /*gLogging.textOut("Loading the foes ...<br>" );
        spawnFoes(Map);

        if(!ok)
        {
            gLogging.textOut("Something went wrong while loading the map!" );
            return false;
        }
    }
    else
    {
        gLogging.ftextOut("Error while trying to open the \"%s\" file!", gamemapfile.c_str() );
        return false;
    }
*/

    // Set Scrollbuffer
    mMap.drawAll();
    gVideoDriver.updateScrollBuffer(mMap.m_scrollx, mMap.m_scrolly);

    gLogging.textOut("Map got loaded successfully!");

    return true;




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

    mMap.animateAllTiles();
}

void CosmoGameplay::render()
{
    mMap.drawAll();

    gVideoDriver.blitScrollSurface();

    // Render the backdrop
    if(!gGraphics.getTileMaps().empty())
    {
        GsTilemap &tilemap = gGraphics.getTileMap(0);
        GsWeakSurface weakSfc(tilemap.getSDLSurface());
        GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

        GsRect<Uint16> srcRect(weakSfc.width(), weakSfc.height());
        GsRect<Uint16> dstRect(weakSfc.width(), weakSfc.height());

        // TODO: Need a xwrap and maybe later a ywrap algorithm (Like Scrollsurface methods we already use?)

        const int scroll_offset_x = (mapwindow_x_offset/2) % dstRect.dim.x;

        srcRect.pos.x = scroll_offset_x;
        dstRect.dim.x -= scroll_offset_x;

        weakSfc.blitTo(blitsfc, srcRect.SDLRect(), dstRect.SDLRect());

        srcRect.pos.x = 0;
        dstRect.pos.x = dstRect.dim.x;

        dstRect.dim.x = scroll_offset_x;

        weakSfc.blitTo(blitsfc, srcRect.SDLRect(), dstRect.SDLRect());


        //video_update();
    }

}
