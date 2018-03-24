/*
 * This small header just holds the information about Cheat codes that can be used.
 * So far it's Galaxy only, but in future it might also be used in vorticon.
 * This structure is shared normally be the Gameplay engine where the Cheat codes
 * can be triggered and by the player where the AI will know what do depending on what is done
 */

#ifndef __CHEAT_STRUCT__
#define __CHEAT_STRUCT__

struct stCheat
{
    bool jump = false;
    bool god = false;
    bool noclipping = false;
    bool items = false;
};

#endif // __CHEAT_STRUCT__
