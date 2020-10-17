#ifndef GSLUA_H
#define GSLUA_H

#include <string>

class GsLua
{
public:
    GsLua();

    virtual ~GsLua();

    bool loadFile(const std::string &fname);

    bool runFunctionRetOneStr(const std::string &fun,
                              std::string &ret);

    bool runFunctionRetOneStr(const std::string &fun,
                              const int param,
                              std::string &ret);

    bool runFunctionRetOneInt(const std::string &fun, int &ret);

    bool runFunctionRetOneInt(const std::string &fun,
                              const int param,
                              int &ret);

    void runFunctionRetOneBool(const std::string &fun, bool &ret);

    operator bool() const;

private:

    void *mLuaStatePtr = nullptr;
};

#endif // GSLUA_H
