/////////////////////////////////////////
//
//   OpenLieroX
//
//   Auxiliary Software class library
//
//   based on the work of JasonB
//   enhanced by Dark Charlie and Albert Zeyer
//
//   code under LGPL
//
/////////////////////////////////////////


// Config file handler
// Created 30/9/01
// By Jason Boettcher


#ifndef __CONFIGHANDLER_H__
#define __CONFIGHANDLER_H__

#include <base/interface/StringUtils.h>
#include "MathLib.h"
#include <string>
#include <SDL.h>
#include <vector>

#define		MAX_STRING_LENGTH	4096
#define		MAX_MINOR_LENGTH	256
#define		MAX_KEYWORDS		256



// Value reading
bool		ReadString(const std::string& filename, const std::string& section, const std::string& key, std::string& value, std::string defaultv, bool abs_fn = false);
bool		ReadInteger(const std::string& filename, const std::string& section, const std::string& key, int *value, int defaultv);
inline bool ReadInteger(const std::string& filename, const std::string& section, const std::string& key, Uint16 *value, int defaultv);
bool		ReadFloat(const std::string& filename, const std::string& section, const std::string& key, float *value, float defaultv);
bool		ReadIntArray(const std::string& filename, const std::string& section, const std::string& key, int *array, int num_items);


struct GsColor;
bool	ReadColour(const std::string& filename, const std::string& section, const std::string& key, GsColor& value, const GsColor& defaultv);



template<typename T>
bool ReadArray(const std::string& filename, const std::string& section, const std::string& key, T* data, size_t num) {
	std::string string;

	if (!ReadString(filename,section,key,string,""))
		return false;

	std::vector<std::string> arr = explode(string,",");
	for (size_t i=0; i< MIN(num,arr.size()); i++)
		data[i] = from_string<T>(arr[i]);

	return num == arr.size();
}

bool	AddKeyword(const std::string& key, int value);
bool	ReadKeyword(const std::string& filename, const std::string& section, const std::string& key, int *value, int defaultv);
bool	ReadKeyword(const std::string& filename, const std::string& section, const std::string& key, bool *value, bool defaultv);
bool	ReadKeywordList(const std::string& filename, const std::string& section, const std::string& key, int *value, int defaultv);

#endif  //  __CONFIGHANDLER_H__
