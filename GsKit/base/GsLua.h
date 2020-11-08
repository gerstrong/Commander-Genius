#ifndef GSLUA_H
#define GSLUA_H

#include <string>
#include <initializer_list>
#include <vector>

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

    bool runFunction(const std::string &fun,
                     std::initializer_list<const int> list,
                     std::vector<int> &multiRet);

    bool runFunction(const std::string &fun,
                     std::initializer_list<const int> list);



    void runFunctionRetOneBool(const std::string &fun, bool &ret);

    operator bool() const;

private:

    void *mLuaStatePtr = nullptr;
};

#endif // GSLUA_H
