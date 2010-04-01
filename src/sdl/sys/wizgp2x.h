#ifndef _GP2X_H_
#define _GP2X_H_

#include "SDL.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>

#define GP2X_BUTTON_UP              0
#define GP2X_BUTTON_DOWN            4
#define GP2X_BUTTON_LEFT            2
#define GP2X_BUTTON_RIGHT           6
#define GP2X_BUTTON_UPLEFT          1
#define GP2X_BUTTON_UPRIGHT         7
#define GP2X_BUTTON_DOWNLEFT        3
#define GP2X_BUTTON_DOWNRIGHT       5

#define GP2X_BUTTON_A               12
#define GP2X_BUTTON_B               13
#define GP2X_BUTTON_X               14
#define GP2X_BUTTON_Y               15
#define GP2X_BUTTON_L               10
#define GP2X_BUTTON_R               11
#define GP2X_BUTTON_CLICK           18
#define GP2X_BUTTON_START           8
#define GP2X_BUTTON_SELECT          9
#define GP2X_BUTTON_VOLUP           16
#define GP2X_BUTTON_VOLDOWN         17

#define VOLUME_MIN          0
#define VOLUME_MAX          100
#define VOLUME_CHANGE_RATE  2
#define VOLUME_NOCHG        0
#define VOLUME_DOWN         1
#define VOLUME_UP           2

extern int volume;
extern int volume_direction;
extern int memfd;
extern volatile uint32_t* memregs32;

void WIZ_EmuKeyboard( int button, int value );
void WIZ_AdjustVolume( int direction );
void WIZ_ptimer_init(void);
unsigned int WIZ_ptimer_get_ticks_ms(void);
void WIZ_ptimer_delay_ms( unsigned int delay );
void WIZ_ptimer_cleanup(void);

#endif
