// Only bother if python3 is enabled
#if USE_PYTHON3

#include "GsPython.h"
#include "utils/FindFile.h"

#include "GsLogging.h"



void fixNewLine(std::string& str)
{
    // Check for empty string.
    if(str.size() <= 2)
    {
       str.clear();
       return;
    }

    // This removes the extra \"s we get through the Python interface
    str = str.substr(1,str.size()-2);

    for(size_t i=0 ; i<str.size()-1 ; i++)
    {
        if(str[i] == '\\' && str[i+1] == 'n')
        {
            str[i] = ' ';
            str[i+1] = '\n';
        }
    }
}


void replaceSlashes(std::string &text)
{
#ifdef WIN32
    for (std::string::iterator it = text.begin(); it != text.end(); it++)
        if (*it == '/') *it = '\\';
#endif
}


// Windows only. In order to get python work correctly you might need to have doubled amount of backslashes
void duplicateBackslashes(std::string &text)
{
#ifdef WIN32
    auto npos = text.find("\\");
    if(npos != std::string::npos)
    {
        auto subStr = text.substr(npos+1);
        duplicateBackslashes(subStr);
        text = text.substr(0, npos) + "\\\\" + subStr;
    }
#endif
}

PyObject *GsPython::loadModule(const std::string &scriptBaseName, const std::string &baseDir)
{
    // Extra Python script for this AI defined?
    std::string aiscriptPath = baseDir;
    aiscriptPath = JoinPaths(aiscriptPath,scriptBaseName);
    aiscriptPath += ".py";
    aiscriptPath = GetFullFileName(aiscriptPath);

    // Ensure the path is correctly formatted even for Windows
    std::string aidir = ExtractDirectory(aiscriptPath);

    replaceSlashes(aidir);
    duplicateBackslashes(aidir);

    Py_Initialize();

#if PYTHON_VERBOSE
    gLogging.ftextOut("Loading \"%s\" python script file.\n", aiscriptPath.c_str() );
#endif

    PyObject* programName = PyUnicode_FromString(scriptBaseName.c_str());

    PyRun_SimpleString("import sys");   

    const std::string sysPathCommand = "sys.path.append(\"" + aidir + "\")";

    PyRun_SimpleString(sysPathCommand.c_str());

    auto pModule = PyImport_Import(programName);
    Py_DECREF(programName);

    if(!pModule)
    {
#if PYTHON_VERBOSE
        PyErr_Print();
        gLogging.ftextOut("Failed to load \"%s\"\n", aiscriptPath.c_str() );
#endif
    }

    return pModule;
}


bool loadStrFunction(PyObject * pModule,
                     const std::string &pyMethodStr,
                     std::string &value,
                     const int param1)
{
    // pFunc is a new reference
    PyObject *pFunc = PyObject_GetAttrString(pModule, pyMethodStr.c_str());

    // TODO: ... Coding ...
    PyObject *pArgs = PyTuple_New(1);
    PyTuple_SetItem(pArgs, 0, PyLong_FromLong(param1));

    if (pFunc && PyCallable_Check(pFunc))
    {
        PyObject *pValue = PyObject_CallObject(pFunc, pArgs);

        if (pValue != nullptr)
        {
            PyObject *objectsRepresentation = PyObject_Repr(pValue);

            char *str = PyUnicode_AsUTF8(objectsRepresentation) ;

            if(str)
            {
                std::string message = str;

                // Because line breaks are not formatted correctly
                fixNewLine(message);

                if(!message.empty())
                {
                    value = message;
                }
            }

            Py_DECREF(pValue);
            Py_DECREF(objectsRepresentation);
        }
        else
        {
            Py_DECREF(pFunc);
            PyErr_Print();
            gLogging.ftextOut("Call failed\n");
            return false;
        }
    }
    else
    {
        if (PyErr_Occurred())
        {
            PyErr_Print();
        }

        gLogging.ftextOut("Cannot find function \"%s\"\n", pyMethodStr.c_str());
        return false;
    }

    Py_XDECREF(pArgs);
    Py_XDECREF(pFunc);


    return true;
}


bool loadIntegerFunc(PyObject * pModule, const std::string &pyMethodStr, int &value)
{
    // pFunc is a new reference
    PyObject *pFunc = PyObject_GetAttrString(pModule, pyMethodStr.c_str());

    if (pFunc && PyCallable_Check(pFunc))
    {
        PyObject *pValue = PyObject_CallObject(pFunc, nullptr);

        if (pValue != nullptr)
        {
            value = PyLong_AsLong(pValue);
            Py_DECREF(pValue);
        }
        else
        {
            Py_DECREF(pFunc);
            PyErr_Print();
            gLogging.ftextOut("Call failed\n");
            return false;
        }
    }
    else
    {
        if (PyErr_Occurred())
        {
            PyErr_Print();
        }

        gLogging.ftextOut("Cannot find function \"init\"\n");
        return false;
    }

    Py_XDECREF(pFunc);


    return true;
}


bool loadIntegerFunc(PyObject * pModule,
                     const std::string &pyMethodStr,
                     int &value,
                     const int param1)
{
    // pFunc is a new reference
    PyObject *pFunc = PyObject_GetAttrString(pModule, pyMethodStr.c_str());

    PyObject *pArgs = PyTuple_New(1);
    PyTuple_SetItem(pArgs, 0, PyLong_FromLong(param1));

    if (pFunc && PyCallable_Check(pFunc))
    {
        PyObject *pValue = PyObject_CallObject(pFunc, pArgs);

        if (pValue != nullptr)
        {
            value = PyLong_AsLong(pValue);
            Py_DECREF(pValue);
        }
        else
        {
            Py_DECREF(pFunc);
            PyErr_Print();
            gLogging.ftextOut("Call failed\n");
            return false;
        }
    }
    else
    {
        if (PyErr_Occurred())
        {
            PyErr_Print();
        }

        gLogging.ftextOut("Cannot find function \"%s\"\n", pyMethodStr.c_str());
        return false;
    }

    Py_XDECREF(pArgs);
    Py_XDECREF(pFunc);


    return true;
}


#endif
