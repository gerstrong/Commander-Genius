/*
 * COrderingInfo.h
 *
 *  Created on: 04.08.2009
 *      Author: gerstrong
 */

#ifndef CORDERINGINFO_H_
#define CORDERINGINFO_H_

#include <vector>
#include <string>

class COrderingInfo {
public:
	COrderingInfo(SDL_Surface *Surface, int episode, std::string& datadirectory);
	virtual ~COrderingInfo();

	void Render(stCloneKeenPlus *pCKP);

private:
	std::vector<int> m_Text_Coordinate;
	std::vector<std::string> m_Textline;

	int m_starty;			// start of y-coordinate in textheights
	int m_numberoflines;	// number of lines to print
	SDL_Surface *m_Surface;
};

#endif /* CORDERINGINFO_H_ */
