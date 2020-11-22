#ifndef COSMO_ENGINE_STATUS_H
#define COSMO_ENGINE_STATUS_H

#include <base/Vector2D.h>
#include <base/Singleton.h>

#define gStatus cosmos_engine::Status::get()

namespace cosmos_engine
{

class Status : public GsSingleton<Status>
{
public:

    void loadTiles();

    void init();

    void initPanel();

    void display();

    void addToScoreUpdateOnDisplay(const int amount_to_add_low,
                                   const GsVec2D<int> pos);

    void updateHealthBarDisplay();

    void displayNumStarsCollected();

    void displayNumBombsLeft();

    void setNumStartsCollected(const int value);
    int numStartsCollected() const ;

    void addStar();

private:

    int numStarsCollected = 0;

};

};



#endif //COSMO_ENGINE_STATUS_H
