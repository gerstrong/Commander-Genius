//
// Created by Eric Fry on 2/01/2018.
//

#ifndef COSMO_ENGINE_HIGH_SCORES_H
#define COSMO_ENGINE_HIGH_SCORES_H

#include "defines.h"

#define NUM_HIGH_SCORE_ENTRIES 10

#define HIGH_SCORE_MAX_NAME_LENGTH 14

typedef struct HighScore {
    char name[HIGH_SCORE_MAX_NAME_LENGTH + 1];
    uint32 score;
} HighScore;

void clear_high_score_table();
void add_high_score_with_name(uint32 new_score, const char *name);

void show_high_scores();
HighScore get_high_score(uint8 position);

#endif //COSMO_ENGINE_HIGH_SCORES_H
