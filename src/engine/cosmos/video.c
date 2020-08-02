//
// Created by efry on 3/11/2017.
//

#include <SDL_system.h>
#include "video.h"
#include "palette.h"
#include "tile.h"
#include "input.h"
#include "b800_font.h"

typedef struct VideoSurface {
    SDL_Texture *texture;
    SDL_Surface *surface;
    SDL_Surface *windowSurface;
} VideoSurface;

SDL_Window *window;
SDL_Renderer *renderer;

VideoSurface game_surface;
VideoSurface text_surface;

bool is_game_mode = true;
bool is_fullscreen = false;
bool video_has_initialised = false;
int video_scale_factor = DEFAULT_SCALE_FACTOR;

void video_fill_surface_with_black(SDL_Surface *surface);

void fade_to_black_speed_3()
{
    fade_to_black(3);
}

bool init_surface(VideoSurface *surface, int width, int height)
{
    surface->texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
    if(surface->texture == NULL)
    {
        printf("Error: creating texture. %s\n", SDL_GetError());
        return false;
    }

    surface->surface = SDL_CreateRGBSurface(0, width, height, 8, 0, 0, 0, 0);
    if(surface->surface == NULL)
    {
        printf("Error: creating indexed surface. %s\n", SDL_GetError());
        return false;
    }

    surface->windowSurface = SDL_CreateRGBSurface(0,
                                         surface->surface->w, surface->surface->h,
                                         32, 0, 0, 0, 0);
    if(surface->windowSurface == NULL)
    {
        printf("Error: creating window surface. %s\n", SDL_GetError());
        return false;
    }

    return true;
}

bool video_init()
{
    window = SDL_CreateWindow("Cosmo Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH*video_scale_factor, SCREEN_HEIGHT*video_scale_factor, 0);
    if(window == NULL)
    {
        printf("Error: creating SDL Window. %s\n", SDL_GetError());
        return false;
    }

    if(is_fullscreen)
    {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }

    renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
    if(renderer == NULL)
    {
        printf("Error: creating render. %s\n", SDL_GetError());
        return false;
    }

    init_surface(&game_surface, SCREEN_WIDTH, SCREEN_HEIGHT);
    set_palette_on_surface(game_surface.surface);

    init_surface(&text_surface, SCREEN_WIDTH*2, SCREEN_HEIGHT*2);
    set_palette_on_surface(text_surface.surface);
    video_fill_surface_with_black(text_surface.surface);

    set_game_mode();

    SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
    SDL_RenderClear( renderer );
    SDL_RenderPresent( renderer);

    video_has_initialised = true;
    return true;
}

void video_shutdown()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

void set_text_mode()
{
    if(!is_game_mode)
        return;
    SDL_RenderSetLogicalSize( renderer, text_surface.surface->w, text_surface.surface->h );
    is_game_mode = false;
}

void set_game_mode()
{
    if(is_game_mode)
        return;
    SDL_RenderSetLogicalSize( renderer, game_surface.surface->w, game_surface.surface->h );
    is_game_mode = true;
}

void video_update()
{
    VideoSurface *s = is_game_mode ? &game_surface : &text_surface;
    SDL_BlitSurface(s->surface, NULL, s->windowSurface, NULL);

    void *pixels;
    int pitch;
    SDL_LockTexture(s->texture, NULL, &pixels, &pitch);
    SDL_ConvertPixels(s->windowSurface->w, s->windowSurface->h,
                      s->windowSurface->format->format,
                      s->windowSurface->pixels, s->windowSurface->pitch,
                      SDL_PIXELFORMAT_RGBA8888,
                      pixels, pitch);
    SDL_UnlockTexture(s->texture);

    /* Make the modified texture visible by rendering it */
    SDL_RenderCopy(renderer, s->texture, NULL, NULL);

    /*
     * Update the screen with any rendering performed since the
     * previous call
     */
    SDL_RenderPresent(renderer);
}

void video_draw_tile(Tile *tile, uint16 x, uint16 y)
{
    uint8 *pixel = (uint8 *)game_surface.surface->pixels + x + y * SCREEN_WIDTH;
    uint8 *tile_pixel = tile->pixels;
    if(tile->type == SOLID)
    {
        for(int i=0;i<TILE_HEIGHT;i++)
        {
            memcpy(pixel, tile_pixel, TILE_WIDTH);
            pixel += SCREEN_WIDTH;
            tile_pixel += TILE_WIDTH;
        }
    }
    else
    {
        for(int i=0;i<TILE_HEIGHT;i++)
        {
            for(int j=0;j<TILE_WIDTH;j++)
            {
                if(*tile_pixel != TRANSPARENT_COLOR)
                {
                    *pixel = *tile_pixel;
                }
                pixel++;
                tile_pixel++;
            }
            pixel += SCREEN_WIDTH - TILE_WIDTH;
        }
    }
}

void video_draw_font_tile(Tile *tile, uint16 x, uint16 y, uint8 font_color)
{
    uint8 *pixel = (uint8 *)game_surface.surface->pixels + x + y * SCREEN_WIDTH;
    uint8 *tile_pixel = tile->pixels;

    for(int i=0;i<TILE_HEIGHT;i++)
    {
        for(int j=0;j<TILE_WIDTH;j++)
        {
            if(*tile_pixel != TRANSPARENT_COLOR)
            {
                *pixel = *tile_pixel == 0xf ? font_color : *tile_pixel;
            }
            pixel++;
            tile_pixel++;
        }
        pixel += SCREEN_WIDTH - TILE_WIDTH;
    }
}

void video_draw_tile_solid_white(Tile *tile, uint16 x, uint16 y)
{
    uint8 *pixel = (uint8 *)game_surface.surface->pixels + x + y * SCREEN_WIDTH;
    uint8 *tile_pixel = tile->pixels;
    if(tile->type == SOLID)
    {
        for(int i=0;i<TILE_HEIGHT;i++)
        {
            memcpy(pixel, tile_pixel, TILE_WIDTH);
            pixel += SCREEN_WIDTH;
            tile_pixel += TILE_WIDTH;
        }
    }
    else
    {
        for(int i=0;i<TILE_HEIGHT;i++)
        {
            for(int j=0;j<TILE_WIDTH;j++)
            {
                if(*tile_pixel != TRANSPARENT_COLOR)
                {
                    *pixel = 0xf;
                }
                pixel++;
                tile_pixel++;
            }
            pixel += SCREEN_WIDTH - TILE_WIDTH;
        }
    }
}

void video_draw_tile_mode3(Tile *tile, uint16 x, uint16 y)
{
    uint8 *pixel = (uint8 *)game_surface.surface->pixels + x + y * SCREEN_WIDTH;
    uint8 *tile_pixel = tile->pixels;
    if(tile->type == TRANSPARENT)
    {
        for(int i=0;i<TILE_HEIGHT;i++)
        {
            for(int j=0;j<TILE_WIDTH;j++)
            {
                if(*tile_pixel != TRANSPARENT_COLOR)
                {
                    *pixel |= 8;
                }
                pixel++;
                tile_pixel++;
            }
            pixel += SCREEN_WIDTH - TILE_WIDTH;
        }
    }
}

void video_draw_highlight_effect(uint16 x, uint16 y, uint8 type)
{
    uint8 *pixel = (uint8 *)game_surface.surface->pixels + x + y * SCREEN_WIDTH;
    for(int i=0;i<TILE_HEIGHT;i++)
    {
        for(int j=0;j<TILE_WIDTH;j++)
        {
            if((type == 0 && i + j >= 7) ||
               type == 1 ||
               (type == 2 && i >= j))
            {
                *pixel |= 8; //set intensity bit
            }
            pixel++;
        }
        pixel += SCREEN_WIDTH - TILE_WIDTH;
    }
}

void video_draw_tile_with_clip_rect(Tile *tile, uint16 x, uint16 y, uint16 clip_x, uint16 clip_y, uint16 clip_w, uint16 clip_h)
{
    uint16 tx = 0;
    uint16 ty = 0;
    uint16 w = TILE_WIDTH;
    uint16 h = TILE_HEIGHT;

    if (x + w < clip_x ||
        y + h < clip_y ||
        x > clip_x + clip_w ||
        y > clip_y + clip_h)
    {
        return;
    }

    if (x < clip_x)
    {
        tx = (clip_x - x);
        w = TILE_WIDTH - tx;
        x = clip_x;
    }

    if (x + w > clip_x + clip_w)
    {
        w -= ((x + w) - (clip_x + clip_w));
    }

    if (y < clip_y)
    {
        ty = (clip_y - y);
        h = TILE_HEIGHT - ty;
        y = clip_y;
    }

    if (y + h > clip_y + clip_h)
    {
        h -= ((y + h) - (clip_y + clip_h));
    }

    uint8 *pixel = (uint8 *)game_surface.surface->pixels + x + y * SCREEN_WIDTH;
    uint8 *tile_pixel = &tile->pixels[tx + ty * TILE_WIDTH];
        for(int i=0;i<h;i++)
        {
            for(int j=0; j < w; j++)
            {
                if(tile_pixel[j] != TRANSPARENT_COLOR)
                {
                    pixel[j] = tile_pixel[j];
                }
            }
            pixel += SCREEN_WIDTH;
            tile_pixel += TILE_WIDTH;
        }
}

void video_draw_tile_flipped(Tile *tile, uint16 x, uint16 y)
{
    uint8 *pixel = (uint8 *)game_surface.surface->pixels + x + (y+TILE_HEIGHT-1) * SCREEN_WIDTH;
    uint8 *tile_pixel = tile->pixels;
    for(int i=0;i<TILE_HEIGHT;i++)
    {
        for(int j=0;j<TILE_WIDTH;j++)
        {
            if(*tile_pixel != TRANSPARENT_COLOR)
            {
                *pixel = *tile_pixel;
            }
            pixel++;
            tile_pixel++;
        }
        pixel -= (SCREEN_WIDTH + TILE_WIDTH);
    }
}

void video_update_palette(int palette_index, SDL_Color new_color)
{
    SDL_SetPaletteColors(game_surface.surface->format->palette, &new_color, palette_index, 1);
}

void fade_to_black(uint16 wait_time)
{
    for(int i=0;i < 16; i++)
    {
        cosmo_wait(wait_time);
        set_palette_color(i, 0);
        video_update();
    }
}

void fade_to_white(uint16 wait_time)
{
    for(int i=0;i < 16; i++)
    {
        cosmo_wait(wait_time);
        set_palette_color(i, 23);
        video_update();
    }
}

void fade_in_from_black(uint16 wait_time)
{
    int j=0;
    for(int i=0;i < 16; i++)
    {
        if(i == 8)
        {
            j = 8;
        }
        set_palette_color(i, i + j);
        video_update();
        cosmo_wait(wait_time);
    }
}

void fade_in_from_black_with_delay_3()
{
    fade_in_from_black(3);
}

void video_fill_surface_with_black(SDL_Surface *surface)
{
    SDL_Rect rect;
    rect.x=0;
    rect.y=0;
    rect.w=surface->w;
    rect.h=surface->h;
    SDL_FillRect(surface, &rect, 0);
}

void video_fill_screen_with_black() {
    video_fill_surface_with_black(game_surface.surface);
}

void video_draw_fullscreen_image(uint8 *pixels) {
    memcpy(game_surface.surface->pixels, pixels, 320 * 200);
}

void video_draw_text(uint8 character, int fg, int bg, int x, int y)
{
    uint8 *pixel = (uint8 *)text_surface.surface->pixels + x*B800_FONT_WIDTH + y*B800_FONT_HEIGHT*text_surface.surface->w;

    int count=0;
    for(int i=0;i<B800_FONT_HEIGHT;i++)
    {
        for (int j = 0; j < B800_FONT_WIDTH; j++)
        {
            int p = (int10_font_16[character * B800_FONT_HEIGHT + (count / 8)] >> (7 - (count % 8))) & 1;
            pixel[j] = p ? (uint8) fg : (uint8) bg;
            count++;
        }
        pixel += text_surface.surface->w;
    }
}

void video_set_fullscreen(bool new_state)
{
    if(new_state == true && !is_fullscreen)
    {
        is_fullscreen = true;
        if(video_has_initialised)
        {
            SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
        }
    }
    else if(new_state == false && is_fullscreen)
    {
        is_fullscreen = false;
        if(video_has_initialised)
        {
            SDL_SetWindowFullscreen(window, 0);
        }
    }

}

void video_set_scale_factor(int scale_factor)
{
    video_scale_factor = scale_factor;
}
