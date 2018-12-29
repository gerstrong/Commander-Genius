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

    /**
     * @brief CMessages NOTE: The parameters are not using when
     *                  extracting with python
     * @param p_exebuf
     * @param episode
     * @param demo
     * @param version
     */
    CMessages(unsigned char *p_exebuf, int episode, bool demo, int version);
	
    bool extractGlobalStringsUsingExe();

    bool extractGlobalStringsUsingPython();
	
	void setDecodeOffset(const unsigned int off);
	
private:
	std::pair<std::string, std::string>
	extractNextString( const std::string matchingstring );
    
	std::pair<std::string, std::string>
    extractString(const std::string matchingstring,
                  const unsigned long start,
                  const unsigned long end,
                  const long offset=0 );

	std::pair<std::string, std::string>
	extractStringOff( const std::string matchingstring, unsigned long start );
	
	bool extractEp4Strings(std::map<std::string, std::string>& StringMap);
    bool extractEp5Strings(std::map<std::string, std::string>& stringMap);
	bool extractEp6Strings(std::map<std::string, std::string>& StringMap);
	bool extractEp6DemoStrings(std::map<std::string, std::string>& StringMap);

	unsigned char *mp_exe;
    int m_episode;
	bool m_demo;
	int m_version;
	
	unsigned int mOffset;
};

#endif /* CMESSAGES_H_ */
