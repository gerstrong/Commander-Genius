//
// Created by Eric Fry on 22/12/2017.
//

#include <assert.h>
#include "files/file.h"
#include "sound/music.h"
#include "fullscreen_image.h"
#include "video.h"
#include "game.h"

static const char filename_ep1_tbl[][13] = {
        "PRETITLE.MNI",
        "TITLE1.MNI",
        "CREDIT.MNI",
        "BONUS.MNI",
        "END1.MNI",
        "ONEMOMNT.MNI"
};
static const char filename_ep2_tbl[][13] = {
        "PRETITLE.MNI",
        "TITLE2.MNI",
        "CREDIT.MNI",
        "BONUS.MNI",
        "END2.MNI",
        "ONEMOMNT.MNI"
};
static const char filename_ep3_tbl[][13] = {
        "PRETITLE.MNI",
        "TITLE3.MNI",
        "CREDIT.MNI",
        "BONUS.MNI",
        "END3.MNI",
        "ONEMOMNT.MNI"
};

#define NUM_PIXELS (320 * 200)

const char * get_filename(uint8 image_number)
{
    switch(get_episode_number())
    {
        case 1 : return filename_ep1_tbl[image_number];
        case 2 : return filename_ep2_tbl[image_number];
        case 3 : return filename_ep3_tbl[image_number];
        default : break;
    }
    return NULL;
}

uint8 *load_image(const char *filename)
{
    uint8 *pixels = (uint8 *)malloc(320*200);

    File image;
    if(!open_file(filename, &image)) {
		printf("failed to open %s\n", filename);
    }

    for(int c=0;c<4;c++) {

        for (int i = 0; i < NUM_PIXELS / 8; i++) {
            uint8 data = file_read1(&image);
            for (int j = 0; j < 8; j++) {
                if(c==0)
                {
                    pixels[i * 8 + j] = (data >> (7 - j)) & 1;
                }
                else
                {
                    pixels[i * 8 + j] |= ((data >> (7 - j)) & 1) << c;
                }
            }
        }
    }
    file_close(&image);

    return pixels;
}

void display_fullscreen_image(uint8 image_number)
{
    assert(image_number < 6);

    if(image_number != 1 && image_number != 2)
    {
        stop_music();
    }

    fade_to_black_speed_3();

    uint8 *image = load_image(get_filename(image_number));
    video_draw_fullscreen_image(image);
    free(image);

    fade_in_from_black_with_delay_3();

}
