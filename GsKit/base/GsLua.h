#ifndef GSLUA_H
#define GSLUA_H

#include <string>

class GsLua
{
public:
    GsLua();

    virtual ~GsLua();

    bool loadFile(const std::string &fname);

    void runFunctionRetOneInt(const std::string &fun, int &ret);

    void runFunctionRetOneBool(const std::string &fun, bool &ret);

private:

    void *mLuaStatePtr = nullptr;
};

#endif // GSLUA_H
