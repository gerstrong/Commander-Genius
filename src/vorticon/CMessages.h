/*
 * CMessages.h
 *
 *  Created on: 16.07.2009
 *      Author: gerstrong
 */

#ifndef CMESSAGES_H_
#define CMESSAGES_H_

#include <list>
#include <string>
#include <iostream>

// TODO: Make the strings a class, but it must read from the exe-files basing on uncompressed buffer

class CMessages {
public:	
	CMessages();
	virtual ~CMessages();
	
	bool readData(unsigned char *buf, int episode, int version, const std::string& DataDirectory);
	char *getString(const char *IDtext);
	
private:
	std::list<std::string> StringList;
	std::list<int> StringIDList;
};

#endif /* CMESSAGES_H_ */
