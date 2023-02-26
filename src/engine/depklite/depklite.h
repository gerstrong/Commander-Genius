#ifndef DEPKLITE_H
#define DEPKLITE_H

#include <cstdbool>
#include <cstdio>
#include <vector>
#include <string>

bool depklite_unpack(const std::vector<unsigned char> compData, std::vector<unsigned char> &decdata);

#endif
