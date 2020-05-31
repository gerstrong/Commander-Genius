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

#include <boost/property_tree/xml_parser.hpp>
#include <boost/version.hpp>

#include "property_tree_fwd.h"

#include <string>
#include <fstream>

namespace GsKit
{
#if BOOST_VERSION >= 105600
    typedef boost::property_tree::xml_writer_settings<std::string> xml_writer_settings;
#else
    typedef boost::property_tree::xml_writer_settings<char> xml_writer_settings;
#endif
/*
    void read_xml(const std::string &filename, GsKit::ptree &pt);

    void read_xml(std::ifstream &file, GsKit::ptree &pt);

    void write_xml(std::ofstream &file, GsKit::ptree &pt);
*/
};


#endif // GSKIT_PROPERTY_TREE_XML
