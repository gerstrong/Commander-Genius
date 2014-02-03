#include "GsArguments.h"
#include <base/GsLogging.h>

void GsArguments::passArgs(int argc, char *argv[])
{
    for(int i=1 ; i<argc ; i++ )
    {
        std::string arg = argv[i];

        size_t pos = arg.find("=");

        if(pos != std::string::npos && pos > 1)
        {
            std::string flag = arg.substr(0,pos);
            std::string value = arg.substr(pos+1);

            mArgMap[flag] = value;
        }
        else
        {
            const std::string warning = "The argument \"" + arg + "\" wasn't recognized. Continuing without this parameter.\n";
            gLogging.textOut(warning);
        }
    }
}

std::string GsArguments::getValue(const std::string &flag)
{
    std::map<std::string, std::string>::iterator it = mArgMap.find(flag);

    if( it != mArgMap.end() )
        return it->second;
    else
        return "";
}

void GsArguments::removeTag(const std::string &flag)
{
    std::map<std::string, std::string>::iterator it = mArgMap.find(flag);

    if( it != mArgMap.end() )
        mArgMap.erase(it);
}
