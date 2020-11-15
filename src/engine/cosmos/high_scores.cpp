//
// Created by Eric Fry on 2/01/2018.
//

#include <string.h>
#include <assert.h>
#include "sound/sfx.h"
#include "high_scores.h"
#include "defines.h"
#include "game.h"
#include "dialog.h"
#include "video.h"



HighScore high_scores[NUM_HIGH_SCORE_ENTRIES];

bool is_high_score(uint32 new_score)
{
    if(new_score != 0)
    {
        for(int i = 0; i < NUM_HIGH_SCORE_ENTRIES; i++) {
            if (new_score >= high_scores[i].score) {
                return true;
            }
        }
    }
    return false;
}

HighScore get_high_score(uint8 position)
{
    assert(position < NUM_HIGH_SCORE_ENTRIES);
    return high_scores[position];
}

void add_high_score_with_name(uint32 new_score, const char *name)
{
    if (new_score == 0)
    {
        return;
    }

    for(int i = 0; i < NUM_HIGH_SCORE_ENTRIES; i++)
    {
        if(new_score >= high_scores[i].score)
        {
            for(int j=NUM_HIGH_SCORE_ENTRIES-1;j > i; j--)
            {
                high_scores[j].score = high_scores[j-1].score;
                strcpy(high_scores[j].name, high_scores[j-1].name);
            }
            high_scores[i].score = new_score;
            strcpy(high_scores[i].name, name);
            return;
        }
    }
}

void clear_high_score_table()
{
    for(int i=0; i < NUM_HIGH_SCORE_ENTRIES; i++)
    {
        high_scores[i].name[0] = '\0';
        high_scores[i].score = 0;
    }
}

void show_high_scores() {
    fade_to_black_speed_3();
    video_fill_screen_with_black();
    if(is_high_score(score))
    {
        char name[HIGH_SCORE_MAX_NAME_LENGTH+1];
        enter_high_score_name_dialog(name, HIGH_SCORE_MAX_NAME_LENGTH+1);
        add_high_score_with_name(score, name);
    }

    play_sfx(0x33);
    display_high_score_dialog(true);
}
