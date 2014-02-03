/**
 * @date Feb 3rd 2014
 * @brief The GsArguments class as Singleton captures all the passed command line
 *        arguments and can be called accesed by the application at any time.
 *        That you don't have to pass all those parameters, you get nice c++
 *        strings and the handling of order is managed as well.
 *
 * @description The parser will try to find parameter by the following form:
 *              "<flag>=<value>"
 *              Only those parameters are processed. Anything other than that will
 *              be ignored.
 */

#ifndef GSARGUMENTS_H
#define GSARGUMENTS_H

#include <base/Singleton.h>
#include <map>
#include <string>

#define gArgs   GsArguments::get()

class GsArguments : public GsSingleton<GsArguments>
{
public:
    /**
     * @brief passArgs is called by GsApp and passes all the arguments to this singleton class
     * @param argc number of agruments
     * @param argv array of all the parameter passed
     */
    void passArgs(int argc, char *argv[]);

    /**
     * @brief Use this method to get an entry of the given parameters.
     *        if the parameter doesn't exist, the returned string is empty
     * @param text that might be contained in the parameter
     * @return if entry found it will return the whole passed parameter containing your text parameter
     */
    std::string getValue(const std::string &flag);

    /**
     * @brief removeTag will delete the given tag. This can be useful when you launch something with a parameter,
     *        and you want to function that flag only once.
     * @param flag
     */
    void removeTag(const std::string &flag);

private:
    std::map<std::string, std::string> mArgMap;

};


#endif // GSARGUMENTS_H
