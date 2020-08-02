//
// Created by Eric Fry on 31/10/2017.
//

#ifndef COSMO_ENGINE_DEFINES_H
#define COSMO_ENGINE_DEFINES_H

#include <stdbool.h>

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

typedef signed char sint8;
typedef signed short sint16;
typedef signed int sint32;

typedef enum {
    NOT_BLOCKED = 0,
    BLOCKED = 1,
    SLOPE = 2
} BlockingType;

typedef enum {
    UP    = 0,
    DOWN  = 1,
    LEFT  = 2,
    RIGHT = 3
} Direction;

#endif //COSMO_ENGINE_DEFINES_H
