#ifndef __GSEFFECTCONTROLLER_H__
#define __GSEFFECTCONTROLLER_H__

#include <base/Singleton.h>
#include <base/video/CEffects.h>
#include <base/CInput.h>
#include <memory>

class GsEffectController : public GsSingleton<GsEffectController>
{

public:
    CEffects *Effect() { return mpEffects.get(); }

    void run(const float deltaT)
    {
        if(runningEffect())
        {
            if( gInput.getPressedAnyCommand() || gInput.mouseClicked() )
            {
                setFXfinished(true);
            }
        }

        if(mpEffects)
        {
            mpEffects->ponder(deltaT);
        }
    }

    void render()
    {
        if(mpEffects)
        {
            mpEffects->render();

            if( !runningEffect() )
            {
               mpEffects.release();
            }
        }
    }

    bool runningEffect()
    {
        return ( mpEffects && !mpEffects->finished() );
    }

    bool applyingEffects()
    {
        if(mpEffects)
            return true;
        return false;
    }

    // This will store the effect pointer the developer created in one function
    // You need this call this to make the effect work!
    void setupEffect(CEffects *pEffect)
    {
        mpEffects.reset(pEffect);
    }


    void setFXfinished(const bool value)
    {
        mpEffects->setFinished(value);
    }


private:
    std::unique_ptr<CEffects> mpEffects;

};

#define gEffectController GsEffectController::get()

#endif //__GSEFFECTCONTROLLER_H__
