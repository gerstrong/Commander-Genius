void gamedo_RenderScreen();


void gamedo_frameskipping();
void gamedo_HandleFKeys(stCloneKeenPlus *pCKP);
void gamedo_frameskipping_blitonly();
void gamedo_getInput(stLevelControl *p_levelcontrol);

void gamedo_render_drawobjects();

// Enemies AI Functions used in gamepdo.cpp
void baby_ai(int o, int episode, bool hard);
