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
    {
        if (*it == '/') *it = '\\';
    }
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

bool
GsPythonModule::
load(const std::string &modName,
     const std::string &dirPath)
{
    mpModule = gPython.loadModule( modName, dirPath );

    return (mpModule != nullptr) ? true : false;
}


bool
GsPythonFunc::
load(GsPythonModule &module,
     const std::string &funcName)
{
    auto pModPtr = module.rawPtr();

    assert(pModPtr);

    mpFunction = PyObject_GetAttrString(pModPtr,
                                        funcName.c_str());

    if (mpFunction && PyCallable_Check(mpFunction))
    {
        return true;
    }
    else
    {
        if (PyErr_Occurred())
        {
            PyErr_Print();
        }

        gLogging.ftextOut("Cannot find function \"%s\"\n", funcName.c_str());
        return false;
    }
}


void
GsPythonFunc::call()
{
    /*PyObject *pValue = */PyObject_CallObject(mpFunction, nullptr);

    /*if (pValue != nullptr)
    {
        if( PyObject_IsTrue(pValue) )
        {
            value = true;
        }
        else
        {
            value = false;
        }
        Py_DECREF(pValue);
    }
    else
    {
        Py_DECREF(pFunc);
        PyErr_Print();
        gLogging.ftextOut("Call failed\n");
        return false;
    }*/
}


PyObject *GsPython::loadModule(const std::string &scriptBaseName,
                               const std::string &baseDir)
{
    // Extra Python script for this AI defined?
    std::string aiscriptPath = baseDir;
    aiscriptPath = JoinPaths(aiscriptPath,scriptBaseName);
    aiscriptPath += ".py";
    aiscriptPath = GetFullFileName(aiscriptPath);

    if( !IsFileAvailable(aiscriptPath) )
    {
        return nullptr;
    }

    // Ensure the path is correctly formatted even for Windows
    std::string aidir = ExtractDirectory(aiscriptPath);

    replaceSlashes(aidir);
    duplicateBackslashes(aidir);

    gLogging.ftextOut("calling Py_Initialize().\n", aiscriptPath.c_str() );


#ifdef ANDROID
    // Because Android does not provide any search for the python assets we have to set them here!
    std::string searchPath;
    GetExactFileName(GetFirstSearchPath(), searchPath);
    const std::string pythonHome = JoinPaths(searchPath,"python3.5");
    setenv("PYTHONHOME", pythonHome.c_str(), 1);
    setenv("PYTHONPATH", pythonHome.c_str(), 1);
#endif


    Py_Initialize();


    PyObject* programName = PyUnicode_FromString(scriptBaseName.c_str());

    PyRun_SimpleString("import sys");   

    const std::string sysPathCommand = "sys.path.append(\"" + aidir + "\")";

    PyRun_SimpleString(sysPathCommand.c_str());

    gLogging.ftextOut("Loading \"%s\" python script file.\n", aiscriptPath.c_str() );

    auto pModule = PyImport_Import(programName);
    Py_DECREF(programName);

    if(!pModule)
    {
        PyErr_Print();
        gLogging.ftextOut("Failed to load \"%s\"\n", aiscriptPath.c_str() );
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


bool loadStrFunction(PyObject * pModule,
                     const std::string &pyMethodStr,
                     std::string &value)
{
    // pFunc is a new reference
    PyObject *pFunc = PyObject_GetAttrString(pModule, pyMethodStr.c_str());

    // TODO: ... Coding ...
    //PyObject *pArgs = PyTuple_New(1);
    //PyTuple_SetItem(pArgs, 0, PyLong_FromLong(param1));

    if (pFunc && PyCallable_Check(pFunc))
    {
        PyObject *pValue = PyObject_CallObject(pFunc, nullptr);

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


bool loadBooleanFunc(PyObject * pModule,
                     const std::string &pyMethodStr,
                     bool &value)
{
    // pFunc is a new reference
    PyObject *pFunc = PyObject_GetAttrString(pModule, pyMethodStr.c_str());

    if (pFunc && PyCallable_Check(pFunc))
    {
        PyObject *pValue = PyObject_CallObject(pFunc, nullptr);

        if (pValue != nullptr)
        {
            auto isBool = PyBool_Check(pValue);

            if(isBool)
            {
                if(pValue == Py_False)
                {
                    value = false;
                }
                else if(pValue == Py_True)
                {
                    value = true;
                }
            }

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
