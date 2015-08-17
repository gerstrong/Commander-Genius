#ifndef	__BE_ST_SDL__
#define __BE_ST_SDL__

// WARNING: This struct is used by BE_ST_SDL ONLY

typedef enum { VSYNC_AUTO, VSYNC_OFF, VSYNC_ON } VSyncSettingType;
typedef enum { SCALE_ASPECT, SCALE_FILL } ScaleTypeSettingType;

typedef struct
{
	bool isFullscreen;
	int fullWidth, fullHeight;
	int winWidth, winHeight;
	int displayNum;
	int sdlRendererDriver;
	VSyncSettingType vSync;
	bool isBilinear;
	ScaleTypeSettingType scaleType;
	int scaleFactor;
	bool autolockCursor;
	int sndSampleRate;
	bool disableSoundSubSystem;
	struct
	{
		bool isEnabled;
		int actionMappings[8]; // Buttons/triggers
		int movementMapping; // D-pad, left stick or right stick (or none)
		int menuMouseMapping; // One of the two sticks (or none)
	} altControlScheme;
	unsigned int farPtrSegOffset; // Actually used just in The Catacomb Armageddon/Apocalypse
} RefKeenConfig;

extern RefKeenConfig g_refKeenCfg;

#endif
