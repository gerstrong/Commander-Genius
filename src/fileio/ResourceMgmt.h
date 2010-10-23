/*
 * ResourceMgmt.h
 *
 *  Created on: 16.05.2010
 *      Author: gerstrong
 */

#ifndef RESOURCEMGMT_H_
#define RESOURCEMGMT_H_

#include <string>
#include <set>

int getRessourceID(const std::string& filename, const std::string& namefilter);

void FilterFilelist(std::set<std::string>& filelist, const std::string& namefilter);

std::string getResourceFilename(const std::string& filename, const std::string& gamepath = "",
											bool essential = true, bool can_be_global = false);

#endif /* RESOURCEMGMT_H_ */
