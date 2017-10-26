/*
 * CGamePlayMode.h
 *
 *  Created on: 26.03.2011
 *      Author: gerstrong
 */

#ifndef CGAMEPLAYMODE_H_
#define CGAMEPLAYMODE_H_

#include "CGameMode.h"
#include "CPlayGame.h"
#include "fileio/CSaveGameController.h"
#include <memory>
#include <string>

class CGamePlayMode : public CGameMode
{
public:
    CGamePlayMode( const int Episode,
			const std::string& DataDirectory,
			const int startLevel = -1);

	CGamePlayMode( GMSwitchToPlayGameMode &gpmode );

	

    void pumpEvent(const CEvent *evPtr) override;
    void ponder(const float deltaT) override;
    void render() override;

private:
	int m_startLevel;
	int m_Episode;
	std::string m_DataDirectory;
	CSaveGameController m_SavedGame;
	std::unique_ptr<CPlayGame> mp_PlayGame;
};

#endif /* CGAMEPLAYMODE_H_ */
