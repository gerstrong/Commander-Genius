#ifndef GSGAMETEXT_H
#define GSGAMETEXT_H

#include <base/Singleton.h>

#include <string>
#include <vector>

#define gGameText GsGameText::get()

/**
 * @brief The GsGameText class  represents most of the game text found in the galaxy.
 *                              Not of the text is loaded though. It is meanly meant for the story pages
 *                              found in the computer wrist menu (Galaxy games).
 *                              Also note that these are more like command where bitmaps with small animation
 *                              are presented on these pages.
 */
class GsGameText : public GsSingleton<GsGameText>
{
public:

    /**
     * @brief dumpLines For debugging purposes only, outputs the all the line to a given file
     */
    void dumpLines(const std::string &fname);

    /**
     * @brief addLine   add a new line to the GameText Instance
     */
    void addLine(const std::string &line)
    {
        mTextBlocks.push_back(line);
    }

    /**
     * @brief clear Jsut clear all the lines
     */
    void clear()
    {
        mTextBlocks.clear();
    }

    /**
     * @brief readPage
     * @param idx   Index of the desired block
     * @param pageIdx   Index of the desired page
     * @return  Vector of strings with the command for the help section
     */
    std::vector<std::string> readPage(const int blockIdx, const int pageIdx);

private:

    std::vector<std::string>    mTextBlocks;
};

#endif // GSGAMETEXT_H
