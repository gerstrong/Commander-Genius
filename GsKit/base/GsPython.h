#ifndef GSPYTHON_H
#define GSPYTHON_H

#if USE_PYTHON3

#include <Python.h>
#include <base/Singleton.h>
#include <string>

#define gPython	GsPython::get()

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


bool loadStrFunction(PyObject * pModule,
                     const std::string &pyMethodStr,
                     std::string &value);


/**
 * @brief loadIntegerGetter for a python function that has no arguments but returns an integer
 * @param pModule
 * @param pyMethodStr
 * @param value
 * @return
 */
bool loadIntegerFunc(PyObject * pModule,
                     const std::string &pyMethodStr,
                     int &value);

bool loadIntegerFunc(PyObject * pModule,
                     const std::string &pyMethodStr,
                     int &value,
                     const int param1);

bool loadBooleanFunc(PyObject * pModule,
                     const std::string &pyMethodStr,
                     bool &value);

class GsPythonModule
{
public:

      bool load(const std::string &modName,
                const std::string &dirPath);

      virtual ~GsPythonModule();

      PyObject *rawPtr() const
      {
          return mpModule;
      }

      // bool operator
      operator bool() const
      {
          return (mpModule!=nullptr);
      }


      bool loadIntegerFunc(const std::string &pyMethodStr,
                           int &value);

      bool loadIntegerFunc(const std::string &pyMethodStr,
                           int &value,
                           const int param1);

      bool loadBooleanFunc(const std::string &pyMethodStr,
                           bool &value);


private:

      PyObject *mpModule = nullptr;
};

class GsPythonFunc
{

public:

    virtual ~GsPythonFunc();

    bool load(GsPythonModule &module,
              const std::string &funcName);

    // bool operator
    operator bool() const
    {
        return (mpFunction!=nullptr);
    }


    PyObject *call(PyObject *args = nullptr);

private:
    PyObject *mpFunction = nullptr;
};



class GsPython : public GsSingleton<GsPython>
{

public:

    GsPython();
    virtual ~GsPython();

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





#endif

#endif // GSPYTHON_H
