/**
    \author Gerhard Stein
    \date 08.06.2013
    \description since a new physical model which fits better to the original DOS Keen we decided to split
                 routines into a ponder and render routine.
                 ponder performs code that is based on the input devices and the AI that is performed.
                 Also the management of the game is performed
                 render just is for draw routines. Everything that needs to be put on the screen is done here
*/

#ifndef GAMEOBJECT_H_
#define GAMEOBJECT_H_

class GameObject
{
  public:
    virtual void ponder() = 0;
    virtual void render() = 0;
};

#endif /* GAMEOBJECT_H_ */
