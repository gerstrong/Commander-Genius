/*
 GsKit

 property tree utilities -> XML Parser

 code under LGPL
 created 30-05-2020
 by Gerhard Stein
 */
#ifndef GSKIT_PROPERTY_TREE_XML
#define GSKIT_PROPERTY_TREE_XML

#include <boost/property_tree/xml_parser.hpp>

#include "property_tree_fwd.h"
#include <string>

namespace GsKit
{
#if BOOST_VERSION >= 105600
    typedef boost::property_tree::xml_writer_settings<std::string> xml_writer_settings;
#else
    typedef boost::property_tree::xml_writer_settings<char> xml_writer_settings;
#endif
/*
    void read_xml(const std::string &filename, GsKit::ptree &pt)
    {
        return boost::property_tree::xml_parser::read_xml(filename, pt.mPtree);
    }
*/
};


#endif // GSKIT_PROPERTY_TREE_XML
