
void gamedo_RenderScreen(stCloneKeenPlus *pCKP);
void gamedo_frameskipping(stCloneKeenPlus *pCKP);
void gamedo_HandleFKeys(stCloneKeenPlus *pCKP);
void gamedo_frameskipping_blitonly(void);
void gamedo_getInput(stCloneKeenPlus *pCKP);
void gamedo_render_drawobjects(stCloneKeenPlus *pCKP);

// Enemies AI Functions used in gamepdo.cpp
void baby_ai(int o, int episode, bool hard);
