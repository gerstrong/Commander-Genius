#ifndef GSPYTHON_H
#define GSPYTHON_H

#if USE_PYTHON3

#include <Python.h>
#include <base/Singleton.h>
#include <string>

#define PYTHON_VERBOSE 0

#define gPython	GsPython::get()

class GsPython : public GsSingleton<GsPython>
{

public:
    /**
     * @brief loadModule  Load the module and return as python object
     * @param scriptBaseName    name of script to load
     * @param baseDir           Base directory of the game where to look at
     * @return pointer to the module
     */
    PyObject *loadModule(const std::string &scriptBaseName,
                         const std::string &baseDir);


};


#endif

#endif // GSPYTHON_H
