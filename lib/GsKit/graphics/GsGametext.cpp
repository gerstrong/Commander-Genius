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


int GsGameText::getNumPages(const int blockIdx)
{
    int numPages = 0;

    std::string block = mTextBlocks.at(blockIdx);

    std::stringstream ss(block);

    while(!ss.eof())
    {
        std::string line;

        std::getline(ss, line);

        if( line.find("^P") != std::string::npos || line.find("^p") != std::string::npos  )
        {
            numPages++;
        }
    }

    numPages--;

    return numPages;
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
            if( line.find("^P") != std::string::npos || line.find("^p") != std::string::npos )
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
            if( line.find("^P") != std::string::npos || line.find("^p") != std::string::npos )
            {
                break;
            }

            pageLines.push_back(line);
            continue;
        }
    }

    return pageLines;
}
