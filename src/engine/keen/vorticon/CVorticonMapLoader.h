/*
 * CVorticonMapLoader.h
 *
 *  Created on: 09.10.2009
 *      Author: gerstrong
 */

#ifndef CVorticonMapLoader_H_
#define CVorticonMapLoader_H_

#include "engine/core/CMap.h"
#include "ai/CPlayer.h"
#include "engine/core/CSpriteObject.h"
#include "engine/core/options.h"
#include "sdl/audio/music/CMusic.h"
#include "engine/core/CBehaviorEngine.h"
#include <string>
#include <vector>
#include <memory>

class CVorticonMapLoaderBase
{
public:
	CVorticonMapLoaderBase(std::shared_ptr<CMap> &map);
	
	bool loadBase(  Uint8 episode, 
			Uint8 level, 
			const std::string& path, 
			bool loadNewMusic );
	
	bool load( Uint8 episode, Uint8 level, const std::string& path, bool loadNewMusic=true );
		
	
	//void fixLevelTiles(int &currentTile, const Uint16 curmapX, const Uint16 curmapY, const int episode, const int level);
	
protected:
  
	void blitPlaneToMap(std::vector<Uint16> &planeitems, const Uint16 planesize, const Uint16 planeID, const Uint16 tilemapID);
  
	std::shared_ptr<CMap> &mpMap;
};


class CVorticonMapLoader : public CVorticonMapLoaderBase
{
public:
	CVorticonMapLoader(std::shared_ptr<CMap> &map,
			    std::vector<std::unique_ptr<CVorticonSpriteObject> > &spriteObjectContainer);
		
	bool m_NessieAlreadySpawned;
	
	std::vector< std::unique_ptr<CVorticonSpriteObject> > &mpSpriteObjectContainer;
};



class CVorticonMapLoaderWithPlayer : public CVorticonMapLoader
{
public:
	CVorticonMapLoaderWithPlayer(std::shared_ptr<CMap> &map,
			    std::vector<CPlayer> &playerContainer,
			    std::vector<std::unique_ptr<CVorticonSpriteObject> > &spriteObjectContainer);    
	
	bool load( Uint8 episode, 
		Uint8 level, 
		const std::string& path, 
		bool loadNewMusic = true, 
		bool stategame = false );

	void loadSprites( Uint8 episode, 
			  Uint8 level );		

	void addWorldMapObject(unsigned int t, Uint16 x, Uint16 y, int episode);
	void addSpriteObject(unsigned int t, Uint16 x, Uint16 y, int episode, int level);
	
	
	bool m_checkpointset;
	
private:
	std::vector<CPlayer> &mPlayerContainer;
};


#endif /* CVorticonMapLoader_H_ */
