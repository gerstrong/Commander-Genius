/**
 * GsKit
 *
 * property tree utilities -> XML Parser
 *
 * code under LGPL
 * created 30-05-2020
 * by Gerhard Stein
 */
#ifndef GSKIT_PROPERTY_TREE_XML
#define GSKIT_PROPERTY_TREE_XML

#include "property_tree.h"

#include <string>
#include <fstream>

namespace GsKit
{

    void read_xml(const std::string &filename, GsKit::ptree &pt);

    void read_xml(std::ifstream &file, GsKit::ptree &pt);

    void write_xml(std::ofstream &file, GsKit::ptree &pt);

};


#endif // GSKIT_PROPERTY_TREE_XML
