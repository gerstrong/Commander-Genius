
#ifndef RESOURCEMGMT_H_
#define RESOURCEMGMT_H_

#include <string>
#include <set>

std::string getResourceFilename(const std::string& filename, const std::string& gamepath = "",bool essential = true, bool can_be_global = false);

void FilterFilelist(std::set<std::string>& filelist, const std::string& namefilter);

#endif 
