#include "GsGametext.h"

#include <fstream>
#include <sstream>

void GsGameText::dumpLines(const std::string &fname)
{
    std::ofstream gametext("gametext.txt");

    for(const auto &line : mTextBlocks)
    {
        gametext << line << std::endl;
    }
}


std::vector<std::string> GsGameText::readPage(const int blockIdx, const int pageIdx)
{
    std::string block = mTextBlocks.at(blockIdx);

    std::stringstream ss(block);

    std::vector< std::string > pageLines;

    bool skipMode = true;

    int breaksLeft = pageIdx+1;

    while(!ss.eof())
    {
        std::string line;

        std::getline(ss, line);

        // Found the pagebreak
        if(breaksLeft > 0)
        {
            if( line.find("^P") != std::string::npos )
            {
                breaksLeft--;

                if(breaksLeft == 0)
                {
                    skipMode = false;
                }
                continue;
            }
        }

        if(!skipMode)
        {
            // If we get another break, we can quit everything
            if( line.find("^P") != std::string::npos )
            {
                break;
            }

            pageLines.push_back(line);
            continue;
        }
    }

    return pageLines;
}
