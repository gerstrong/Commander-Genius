#if defined(WIZ) || defined(GP2X)

#include "wizgp2x.h"

int volume;
int volume_direction;
int joy_but_pressed[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int memfd;
volatile uint32_t* memregs32;

void WIZ_EmuKeyboard( int button, int value )
{
	SDL_Event fakeevent1, fakeevent2;

	//printf( "Button %d Value %d\n", button, value );

    volume_direction = VOLUME_NOCHG;

	if( value == 1 ) {
	    joy_but_pressed[button] = 1;
		fakeevent1.type			    = SDL_KEYDOWN;
		fakeevent1.key.state        = SDL_PRESSED;
		fakeevent1.key.type		    = SDL_KEYDOWN;
		fakeevent1.key.keysym.mod	= KMOD_NONE;

		fakeevent2.type			    = SDL_KEYDOWN;
		fakeevent2.key.state		= SDL_PRESSED;
		fakeevent2.key.type		    = SDL_KEYDOWN;
		fakeevent2.key.keysym.mod	= KMOD_NONE;
	}
	else {
	    joy_but_pressed[button] = 0;
		fakeevent1.type			    = SDL_KEYUP;
		fakeevent1.key.state		= SDL_RELEASED;
		fakeevent1.key.type		    = SDL_KEYUP;
		fakeevent1.key.keysym.mod	= KMOD_NONE;

		fakeevent2.type			    = SDL_KEYUP;
		fakeevent2.key.state		= SDL_RELEASED;
		fakeevent2.key.type		    = SDL_KEYUP;
		fakeevent2.key.keysym.mod	= KMOD_NONE;
	}

	//printf( "Button %d %d\n", button, value );
	fakeevent1.key.keysym.sym = SDLK_UNKNOWN;
	fakeevent2.key.keysym.sym = SDLK_UNKNOWN;

	// Right press
	if( (button == GP2X_BUTTON_RIGHT || button == GP2X_BUTTON_UPRIGHT || button == GP2X_BUTTON_DOWNRIGHT || button == GP2X_BUTTON_R) &&
	    ((value == 1 &&
	    (joy_but_pressed[GP2X_BUTTON_RIGHT] ||  joy_but_pressed[GP2X_BUTTON_UPRIGHT] ||  joy_but_pressed[GP2X_BUTTON_DOWNRIGHT])) ||
	    (value == 0 &&
	    (!joy_but_pressed[GP2X_BUTTON_RIGHT] && !joy_but_pressed[GP2X_BUTTON_UPRIGHT] && !joy_but_pressed[GP2X_BUTTON_DOWNRIGHT])))    )
	{
	    fakeevent1.key.keysym.unicode = fakeevent1.key.keysym.sym = SDLK_RIGHT;
	}

	// Left press
	if( (button == GP2X_BUTTON_LEFT || button == GP2X_BUTTON_UPLEFT || button == GP2X_BUTTON_DOWNLEFT || button == GP2X_BUTTON_R) &&
	    ((value == 1 &&
	    (joy_but_pressed[GP2X_BUTTON_LEFT] ||  joy_but_pressed[GP2X_BUTTON_UPLEFT] ||  joy_but_pressed[GP2X_BUTTON_DOWNLEFT])) ||
	    (value == 0 &&
	    (!joy_but_pressed[GP2X_BUTTON_LEFT] && !joy_but_pressed[GP2X_BUTTON_UPLEFT] &&  !joy_but_pressed[GP2X_BUTTON_DOWNLEFT])))    )
	{
	    fakeevent1.key.keysym.unicode = fakeevent1.key.keysym.sym = SDLK_LEFT;
	}

	// Up press
	if( (button == GP2X_BUTTON_UP || button == GP2X_BUTTON_UPLEFT || button == GP2X_BUTTON_UPRIGHT || button == GP2X_BUTTON_R) &&
	    ((value == 1 &&
	    (joy_but_pressed[GP2X_BUTTON_UP] ||  joy_but_pressed[GP2X_BUTTON_UPLEFT] ||  joy_but_pressed[GP2X_BUTTON_UPRIGHT])) ||
	    (value == 0 &&
	    (!joy_but_pressed[GP2X_BUTTON_UP] && !joy_but_pressed[GP2X_BUTTON_UPLEFT] && !joy_but_pressed[GP2X_BUTTON_UPRIGHT])))    )
	{
	    fakeevent1.key.keysym.unicode = fakeevent1.key.keysym.sym = SDLK_UP;
	}

	// Down press
	if( (button == GP2X_BUTTON_DOWN || button == GP2X_BUTTON_DOWNLEFT || button == GP2X_BUTTON_DOWNRIGHT || button == GP2X_BUTTON_R) &&
	    ((value == 1 &&
	    (joy_but_pressed[GP2X_BUTTON_DOWN] ||  joy_but_pressed[GP2X_BUTTON_DOWNLEFT] ||  joy_but_pressed[GP2X_BUTTON_DOWNRIGHT])) ||
	    (value == 0 &&
	    (!joy_but_pressed[GP2X_BUTTON_DOWN] && !joy_but_pressed[GP2X_BUTTON_DOWNLEFT] && !joy_but_pressed[GP2X_BUTTON_DOWNRIGHT])))    )
	{
	    fakeevent1.key.keysym.unicode = fakeevent1.key.keysym.sym = SDLK_DOWN;
	}


	switch(button)
	{
		case GP2X_BUTTON_SELECT:
			fakeevent1.key.keysym.sym = SDLK_RETURN;
			break;
		case GP2X_BUTTON_START:
			fakeevent1.key.keysym.sym = SDLK_ESCAPE;
			break;
		case GP2X_BUTTON_L:
			fakeevent1.key.keysym.sym = SDLK_q;
			fakeevent2.key.keysym.sym = SDLK_1;
			break;
		case GP2X_BUTTON_R:
			fakeevent1.key.keysym.sym = SDLK_t;
			fakeevent2.key.keysym.sym = SDLK_2;
			break;
		case GP2X_BUTTON_A:
			fakeevent1.key.keysym.sym = SDLK_LCTRL;
			break;
		case GP2X_BUTTON_B:
			fakeevent1.key.keysym.sym = SDLK_LALT;
			break;
		case GP2X_BUTTON_X:
			fakeevent1.key.keysym.sym = SDLK_SPACE;
			break;
		case GP2X_BUTTON_Y:
			fakeevent1.key.keysym.sym = SDLK_y;
			fakeevent2.key.keysym.sym = SDLK_F3;
			break;
		case GP2X_BUTTON_VOLUP:
			if( value == 1)
				volume_direction = VOLUME_UP;
			break;
		case GP2X_BUTTON_VOLDOWN:
			if( value == 1)
				volume_direction = VOLUME_DOWN;
			break;
	}

	if( fakeevent1.key.keysym.sym != SDLK_UNKNOWN )
	{
		SDL_PushEvent (&fakeevent1);
	}

	if( fakeevent2.key.keysym.sym != SDLK_UNKNOWN )
	{
		SDL_PushEvent (&fakeevent2);
	}
}

void WIZ_AdjustVolume( int direction )
{
	if( direction != VOLUME_NOCHG )
	{
		if( volume <= 10 )
		{
			if( direction == VOLUME_UP )   volume += VOLUME_CHANGE_RATE/2;
			if( direction == VOLUME_DOWN ) volume -= VOLUME_CHANGE_RATE/2;
		}
		else
		{
			if( direction == VOLUME_UP )   volume += VOLUME_CHANGE_RATE;
			if( direction == VOLUME_DOWN ) volume -= VOLUME_CHANGE_RATE;
		}

		if( volume < VOLUME_MIN ) volume = VOLUME_MIN;
		if( volume > VOLUME_MAX ) volume = VOLUME_MAX;

		printf( "Volume Change: %i\n", volume );

		unsigned long soundDev = open("/dev/mixer", O_RDWR);
		if(soundDev)
		{
			int vol = ((volume << 8) | volume);
			ioctl(soundDev, SOUND_MIXER_WRITE_PCM, &vol);
			close(soundDev);
		}
	}
}

#endif

#ifdef WIZ

#define TIMER_BASE3 0x1980
#define TIMER_REG(x) memregs32[(TIMER_BASE3 + x) >> 2]

void WIZ_ptimer_init(void)
{
	/* open /dev/mem to access registers */
	memfd = open("/dev/mem", O_RDWR);
	if(memfd < 0) {
		printf( "Could not open /dev/mem!\n" );
		return;
	}

	/* get access to the registers */
	memregs32 = (volatile uint32_t*)mmap(0, 0x20000, PROT_READ|PROT_WRITE, MAP_SHARED, memfd, 0xC0000000);
	if(memregs32 == (volatile uint32_t*)0xFFFFFFFF) {
	    printf( "Could not mmap hardware registers!\n" );
	    return;
	}

    TIMER_REG(0x44) = 0x922;
    TIMER_REG(0x40) = 0x0c;
    TIMER_REG(0x08) = 0x6b;

    printf( "Wiz hardware timer started\n" );
}

unsigned int WIZ_ptimer_get_ticks_ms(void)
{
    unsigned int microsec;

    TIMER_REG(0x08) = 0x4b;  /* run timer, latch value */
    microsec = TIMER_REG(0);
    return (microsec/1000);
}

void WIZ_ptimer_delay_ms( unsigned int delay )
{
    unsigned int start;

    start = WIZ_ptimer_get_ticks_ms();
    while(WIZ_ptimer_get_ticks_ms()-start < delay) {}
}

void WIZ_ptimer_cleanup(void)
{
    TIMER_REG(0x40) = 0x0c;
    TIMER_REG(0x08) = 0x23;
    TIMER_REG(0x00) = 0;
    TIMER_REG(0x40) = 0;
    TIMER_REG(0x44) = 0;

	memregs32 = NULL;
	close(memfd);

	printf( "Wiz hardware timer stoped\n" );
}
#endif
