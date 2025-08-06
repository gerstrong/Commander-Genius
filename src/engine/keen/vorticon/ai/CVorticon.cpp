#include <base/interface/misc.h>

#include <base/audio/Audio.h>
#include <graphics/GsGraphics.h>
#include <graphics/CFlash.h>
#include "CVorticon.h"

// Vorticon (all Episodes, albeit the behavior changes slightly
// depending on levelcontrol.Episode).
CVorticon::CVorticon(std::shared_ptr<CMap> p_map, Uint32 x, Uint32 y, char hp, object_t objtype) :
    CVorticonSpriteObject(p_map, x, y, objtype, 0)
{
    frame = 0;
    animtimer = 0;
    state = VORT_LOOK;
    timer = 0;
    dist_traveled = VORT_TRAPPED_DIST + 1;
    mHealthPoints = hp;
    canbezapped = true;

    const auto diff = gBehaviorEngine.mDifficulty;

    if(diff > NORMAL)
        mHealthPoints++;
    else if( diff < NORMAL && mHealthPoints > 1 )
        mHealthPoints--;

    short Episode = gBehaviorEngine.getEpisode();
    // copy in animation frame indexes for the current ep
    if (Episode == 1)
    {
        WalkLeftFrame = VORT1_WALK_LEFT_FRAME;
        WalkRightFrame = VORT1_WALK_RIGHT_FRAME;
        LookFrame = VORT1_LOOK_FRAME;
        JumpRightFrame = VORT1_JUMP_RIGHT_FRAME;
        JumpLeftFrame = VORT1_JUMP_LEFT_FRAME;
        DyingFrame = VORT1_DYING_FRAME;
    }
    else if (Episode == 2)
    {
        WalkLeftFrame = VORT2_WALK_LEFT_FRAME;
        WalkRightFrame = VORT2_WALK_RIGHT_FRAME;
        LookFrame = VORT2_LOOK_FRAME;
        JumpRightFrame = VORT2_JUMP_RIGHT_FRAME;
        JumpLeftFrame = VORT2_JUMP_LEFT_FRAME;
        DyingFrame = VORT2_DYING_FRAME;
        DeadFrame = VORT2_DEAD_FRAME;
    }
    else if (Episode == 3)
    {
        WalkLeftFrame = VORT3_WALK_LEFT_FRAME;
        WalkRightFrame = VORT3_WALK_RIGHT_FRAME;
        LookFrame = VORT3_LOOK_FRAME;
        JumpRightFrame = VORT3_JUMP_RIGHT_FRAME;
        JumpLeftFrame = VORT3_JUMP_LEFT_FRAME;
        DyingFrame = VORT3_DYING_FRAME;
        DeadFrame = VORT3_DEAD_FRAME;
    }

    if(diff == EXPERT)
    {
        mSprVar = 1;
        mHealthPoints++;
    }
    else if(diff == NINJA)
    {
        mSprVar = 2;
        mHealthPoints+=2;
    }
    else if(diff == ELITE)
    {
        mSprVar = 3;
        mHealthPoints+=3;
    }

}


const int VORTICON_PLAYER_MIN_DIST = 8<<CSF;

bool CVorticon::isNearby(CSpriteObject &theObject)
{
    if( CPlayer *player = dynamic_cast<CPlayer*>(&theObject) )
    {
        if(state == VORT_LOOK)
        {
            mSpriteIdx = LookFrame + frame;

            if (animtimer > VORT_LOOK_ANIM_TIME)
            {
                if (frame > 0)
                {
                    bool isleft = player->getXPosition() < getXPosition();
                    int absdist = isleft ?
                                 getXPosition() - player->getXPosition()
                                 : player->getXPosition() - getXPosition();

                    if(absdist <= VORTICON_PLAYER_MIN_DIST)
                    {
                        if (isleft && !blockedl)
                            movedir = LEFT;
                        else if( !blockedr )
                            movedir = RIGHT;
                    }

                    timer = 0;
                    frame = 0;
                    state = VORT_WALK;
                }
                else
                {
                    frame++;
                }
                animtimer = 0;
            }
            else
            {
                animtimer++;
            }
        }
    }

    return true;
}


void CVorticon::getTouchedBy(CSpriteObject &theObject)
{
    if( CPlayer *player = dynamic_cast<CPlayer*>(&theObject) )
    {
    if (state != VORT_DYING and !mIsDead and state != VORT2_DYING)
        player->kill();
    }
}

void CVorticon::process()
{
    bool kill = false;
    short Episode = gBehaviorEngine.getEpisode();

    if (mHealthPoints <= 0 && state != VORT_DYING && state != VORT2_DYING)
        kill = true;

    if (kill)
    {
        animtimer = 0;
        frame = 0;
        if (Episode == 1)
        {
            // White Fade and back
            if (gBehaviorEngine.mOptions[GameOption::SPECIALFX].value)
            {
                gEffectController.setupEffect(new CFlash(3000, 8, GsColor(0xFF, 0xFF, 0xFF), 200));
            }
            state = VORT_DYING;
            dying = true;
        }
        else
        {
            state = VORT2_DYING;
            dying = true;
        }

        playSound(GameSound::VORT_DIE);
    }

    vort_reprocess: ;

    switch (state)
    {
    case VORT_JUMP:
        if (movedir == RIGHT && !blockedr)
            xinertia = VORT_WALK_SPEED;
        else if (!blockedl)
            xinertia = -VORT_WALK_SPEED;

        if (yinertia == 0 && blockedd) { // The Vorticon Has landed after the jump!
            state = VORT_LOOK;
            goto vort_reprocess;
        }

        break;
    case VORT_LOOK:
        mSpriteIdx = LookFrame + frame;

        if (animtimer > VORT_LOOK_ANIM_TIME)
        {
            if (frame > 0)
            {
                if (blockedl)
                    movedir = RIGHT;
                else if (blockedr)
                    movedir = LEFT;

                timer = 0;
                frame = 0;
                state = VORT_WALK;
            }
            else
                frame++;
            animtimer = 0;
        } else
            animtimer++;
        break;
    case VORT_WALK:
        dist_traveled++;
        int odds;

        switch (gBehaviorEngine.mDifficulty)
        {
        case EASY:
            odds = getProbability(VORT_JUMP_PROB_EASY);
            break;
        case HARD:
            odds = getProbability(VORT_JUMP_PROB_HARD);
            break;
        default:
            odds = getProbability(VORT_JUMP_PROB);
            break;
        }

        if (odds)
        { // let's jump.
            if (!mpMap->m_Dark && !blockedu)
            {
                initiateJump();
                goto vort_reprocess;
            }
        }
        if (movedir == LEFT) { // move left
            mSpriteIdx = WalkLeftFrame + frame;

            if (!blockedl)
            {
                xinertia = -VORT_WALK_SPEED;
            }
            else
            {
                frame = 0;
                animtimer = 0;
                state = VORT_LOOK;

                if(!mpMap->m_Dark)
                {
                    // if we only traveled a tiny amount before hitting a wall, we've
                    // probably fallen into a small narrow area, and we need to try
                    // to jump out of it
                    if (dist_traveled < VORT_TRAPPED_DIST && !mpMap->m_Dark && blockedd
                            && !blockedu) {
                        initiateJump();
                        if (rnd() & 1)
                            yinertia = -VORT_MAX_JUMP_HEIGHT;
                        else
                            yinertia = -VORT_MIN_JUMP_HEIGHT;

                        goto vort_reprocess;
                    } else
                        dist_traveled = 0;
                }
            }
        }
        else
        { // move right
            mSpriteIdx = WalkRightFrame + frame;

            if (!blockedr)
            {
                xinertia = VORT_WALK_SPEED;
            }
            else
            {
                frame = 0;
                animtimer = 0;
                state = VORT_LOOK;

                if(!mpMap->m_Dark)
                {
                    if (dist_traveled < VORT_TRAPPED_DIST &&
                            blockedd &&
                            !blockedu)
                    {
                        initiateJump();
                        if (rnd() & 1)
                        {
                            yinertia = -VORT_MAX_JUMP_HEIGHT;
                        } else
                        {
                            yinertia = -VORT_MIN_JUMP_HEIGHT;
                        }
                        goto vort_reprocess;
                    } else
                        dist_traveled = 0;
                }
            }
        }
        // walk animation
        if (animtimer > VORT_WALK_ANIM_TIME) {
            if (frame >= 3)
                frame = 0;
            else
                frame++;
            animtimer = 0;
        } else
            animtimer++;
        break;
    case VORT_DYING:
        mSpriteIdx = DyingFrame + frame;

        if (animtimer > VORT_DIE_ANIM_TIME) {
            frame++;
            if (frame >= 6) {
                frame = 5;
                gGraphics.Palette.fadeto(0, FADE_SPEED_VERY_SLOW);
                if (!gGraphics.Palette.in_progress())
                    mIsDead = true;
            }

            animtimer = 0;
        } else
            animtimer++;
        break;
    case VORT2_DYING:
        mSpriteIdx = DyingFrame;
        if (animtimer > VORT2_DIE_ANIM_TIME) {
            mSpriteIdx = DeadFrame;
            mIsDead = true;
        } else {
            animtimer++;
        }
        break;
    default:
        break;
    }
}

void CVorticon::initiateJump()
{
    // must be standing on floor to jump
    if (!blockedd)
        return;

    frame = 0;
    animtimer = 0;
    yinertia = -((rnd() % (VORT_MAX_JUMP_HEIGHT - VORT_MIN_JUMP_HEIGHT))
            + VORT_MIN_JUMP_HEIGHT);

    if (movedir == RIGHT)
        mSpriteIdx = JumpRightFrame;
    else
        mSpriteIdx = JumpLeftFrame;

    state = VORT_JUMP;
}
