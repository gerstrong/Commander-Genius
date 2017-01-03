#ifndef GSPYTHON_H
#define GSPYTHON_H

#if USE_PYTHON3

#include <Python.h>
#include <base/Singleton.h>
#include <string>

#define PYTHON_VERBOSE 1

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

/**
 * @brief fixNewLine Ensures that newline bytes are threated correctly. Python like to put extra backslashes for these.
 * @param str
 */
void fixNewLine(std::string& str);

/**
 * @brief loadStrFunction   Function that takes on integer and returns string
 * @param pModule
 * @param pyMethodStr
 * @param value
 * @param param1
 * @return
 */
bool loadStrFunction(PyObject * pModule,
                     const std::string &pyMethodStr,
                     std::string &value,
                     const int param1);

/**
 * @brief loadIntegerGetter for a python function that has no arguments but returns an integer
 * @param pModule
 * @param pyMethodStr
 * @param value
 * @return
 */
bool loadIntegerFunc(PyObject * pModule, const std::string &pyMethodStr, int &value);

bool loadIntegerFunc(PyObject * pModule,
                     const std::string &pyMethodStr,
                     int &value,
                     const int param1);

#endif

#endif // GSPYTHON_H
