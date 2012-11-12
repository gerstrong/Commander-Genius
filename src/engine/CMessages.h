/*
 * CMessages.h
 *
 *  Created on: 16.07.2009
 *      Author: gerstrong
 */

#ifndef CMESSAGES_H_
#define CMESSAGES_H_

#include <string>
#include <map>

class CMessages {
public:	
	CMessages(unsigned char *p_exebuf, char episode, int version);
	
	bool extractGlobalStrings();
	
	void setDecodeOffset(const unsigned int off);
	
private:
	std::pair<std::string, std::string>
	extractNextString( const std::string matchingstring );
    
	std::pair<std::string, std::string>
	extractString( const std::string matchingstring, unsigned long start, unsigned long end, long offset=0 );

	bool extractEp4Strings(std::map<std::string, std::string>& StringMap);

	unsigned char *mp_exe;
	char m_episode;
	int m_version;
	
	unsigned int mOffset;
};

#endif /* CMESSAGES_H_ */
