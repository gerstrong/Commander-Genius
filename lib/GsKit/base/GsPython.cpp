#include "GsPython.h"
#include "utils/FindFile.h"

#include "GsLogging.h"


PyObject *GsPython::loadModule(const std::string &scriptBaseName, const std::string &baseDir)
{
    // Extra Python script for this AI defined?
    std::string aiscriptPath = baseDir;
    aiscriptPath = JoinPaths(aiscriptPath,scriptBaseName);
    aiscriptPath += ".py";
    aiscriptPath = GetFullFileName(aiscriptPath);

    std::string aidir = ExtractDirectory(aiscriptPath);

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
