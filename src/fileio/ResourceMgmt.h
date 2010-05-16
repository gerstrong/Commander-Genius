/*
 * ResourceMgmt.h
 *
 *  Created on: 16.05.2010
 *      Author: gerstrong
 */

#ifndef RESOURCEMGMT_H_
#define RESOURCEMGMT_H_

#include <string>

std::string getResourceFilename(const std::string& filename, const std::string& gamepath = "", bool essential = true);

#endif /* RESOURCEMGMT_H_ */
