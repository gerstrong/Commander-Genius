// Reference from game.cpp
void initgame(stLevelControl *p_levelcontrol);

void cleanup(stCloneKeenPlus *CKP);
void showinventory(int p, stCloneKeenPlus *pCKP);
void YourShipNeedsTheseParts(stCloneKeenPlus *pCKP);
void ShipEp3(stCloneKeenPlus *pCKP);
void youseeinyourmind(int mpx, int mpy, stCloneKeenPlus *pCKP);
void VorticonElder(int mpx, int mpy, stCloneKeenPlus *pCKP);
char save_slot_box(int issave, stCloneKeenPlus *pCKP);
void game_save_interface(stCloneKeenPlus *pCKP);
int VerifyQuit();
char gameiswon(stCloneKeenPlus *pCKP);
void game_save(char *fname, stCloneKeenPlus *pCKP);
int game_load(char *fname, stCloneKeenPlus *pCKP);
void showTextMB(const std::string& Text);
void showF1HelpText(int episode, std::string DataDirectory);
void showGameHint(int mpx, int mpy, int episode, int level);

unsigned int rnd(void);

void endsequence_start(int episode);
