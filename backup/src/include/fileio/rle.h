/*
 * rle.h
 *
 *  Created on: 29.01.2009
 *      Author: gerstrong
 */

unsigned long unRLEW(FILE *fp, std::vector<unsigned int>& filebuf);

unsigned long unRLEW(std::ifstream& file, std::vector<Uint16>& filebuf);
