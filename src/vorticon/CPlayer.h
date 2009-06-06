/*
 * CPlayer.h
 *
 *  Created on: 31.03.2009
 *      Author: gerstrong
 */

#ifndef CPLAYER_H_
#define CPLAYER_H_

class CPlayer {
public:
	CPlayer();
	virtual ~CPlayer();

	void handlePlayer(void);
	void showStatusBox(void);
	unsigned short getPlayerframe(void);
	bool isPlaying(void);
	void enablePlayer(bool value);
	void useObject(unsigned short value);
	unsigned short getObject(void);
	unsigned long getCoordX(void);
	unsigned long getCoordY(void);
	void setCoord(unsigned long in_x, unsigned long in_y);
	void setpScrCoord(int *px, int *py);

private:
	short episode;

	bool active;
	bool isGameover;
	bool isFalling;
	bool isDying;
	bool isFiring;
	bool isWalking;
	bool isPushed;
	bool isStanding;
	bool cannotWalk;
	bool cannotfall;
	bool usePogostick;

	unsigned long m_x;
	unsigned long m_y;
	int *mp_scrx;
	int *mp_scry;


	unsigned short playframe;
	unsigned short Jumpmode;
    unsigned short dieframe;
    unsigned short frozenframe;
    unsigned short jumpframe;
    unsigned short headingDirection;
    unsigned short frozentime;
    unsigned short walkframe;
    unsigned short object;
    unsigned short walktimer;
    unsigned short frictiontimer;
    short m_vx;
    short m_vy;
    short m_ax;
    short m_ay;

    bool bisBlockedUp;
    bool bisBlockedDown;
    bool bisBlockedLeft;
    bool bisBlockedRight;

	void performDieAnimation(void);
    void processInput(void);
    void processDirection(void);
    void checkCollisions(void);
    void processGoodies(void);
    void processInvincibility(void);
    void processRaygun(void);
    void processStunned(void);
	void processWalking(void);
    void processInertia(void);
    void processFriction(void);
    void processSwitches(void);
    void processPogo(void);
    void processJump(void);
    void processFalling(void);
    void processMovements(void);

    void applyFrame(void);

};

#endif /* CPLAYER_H_ */
