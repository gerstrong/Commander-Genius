#include "GsLua.h"

extern "C"
{
    #include "lualib.h"
    #include "lauxlib.h"
    #include "lua.h"
}

#include <base/GsLogging.h>
#include <base/utils/FindFile.h>


GsLua::GsLua()
{
}

GsLua::~GsLua()
{
    if(mLuaStatePtr)
    {
        lua_close((lua_State*)mLuaStatePtr);
    }
}

bool GsLua::loadFile(const std::string &fname)
{
    lua_State *L = (lua_State *)mLuaStatePtr;

    if(L)
        return true;

    gLogging.ftextOut("Looking for %s ...", fname.c_str());
    const auto fullfname = GetFullFileName(fname);

    if(!IsFileAvailable(fullfname, true))
    {
        gLogging.ftextOut("File %s not found.", fname.c_str());
        return false;
    }

    gLogging.ftextOut("Found a Lua file: %s. Trying to load it...", fullfname.c_str());

    L = luaL_newstate();
    if (L == nullptr)
    {
        return false;
    }

    mLuaStatePtr = (void*)L;

    /* Load the standard Lua libraries. */
    luaL_openlibs(L);    

    if (luaL_dofile(L, fullfname.c_str()))
    {
        gLogging.ftextOut("Could not load %s", fname.c_str());
        lua_close(L);
        L = nullptr;
        mLuaStatePtr = nullptr;
        return false;
    }

    /* Pull new out of the counter module. */
    lua_getfield(L, -1, "new");

    /* Verify that new is a function. */
      if (!lua_isfunction(L, -1)) {
          gLogging.ftextOut("New not a valid function");
          lua_close(L);
          return false;
      }

      /* Move the counter module to be the first argument of new. */
      lua_insert(L, -2);

      /* Put our actual argument (start) onto the stack. */
      lua_pushinteger(L, 0);

      /* Call new(M, start). 2 arguments. 2 return values. */
      if (lua_pcall(L, 2, 2, 0) != 0) {
          gLogging.ftextOut( "%s", lua_tostring(L, -1));
          lua_close(L);
          return false;
      }

      /* Right now we will either have nil and an error string
       * (nil will be below the string on the stack because it
       * would be returned first and put onto the stack first),
       * or the counter object returned by new. */
      if (lua_type(L, -2) == LUA_TNIL) {
          gLogging.ftextOut("%s", lua_tostring(L, -1));
          lua_close(L);
          return false;
      }

      /* Remove the empty filler nil from the top of the stack. The
       * lua_pcall stated 2 return values but on success we only
       * get one so we have nil filler after. */
      lua_pop(L, 1);

      if (lua_type(L, -1) != LUA_TTABLE) {
          gLogging.ftextOut("Invalid type (%d) returned by new", lua_type(L, -1));
          lua_close(L);
          return false;
      }

      /* We're sure we have a table returned by new.
       * This is the only item on the stack right now. */

      return true;
}

void GsLua::runFunctionRetOneInt(const std::string &fun,
                                 const int param,
                                 int &ret)
{
    if(!mLuaStatePtr)
    {
        gLogging.ftextOut("Lua State not running for %s", fun.c_str());
        return;
    }


    lua_State *L = (lua_State *)mLuaStatePtr;
    lua_getfield(L, -1, fun.c_str());
    lua_pushvalue(L, -2);
    lua_pushnumber(L, param);
    if (lua_pcall(L, 2, 1, 0) != 0)
    {
        gLogging.ftextOut("Error calling %s with %d", fun.c_str(), param);
        return;
    }

    int isnum;
    int n = lua_tointegerx(L, -1, &isnum);
    if (!isnum) {
        gLogging.ftextOut("Error getval didn't return a number");
        return;
    }
    ret = n;
    gLogging.ftextOut("ret=%d\n", n);

    /* Remove the return value from the stack. */
    lua_remove(L, -1);
}

bool GsLua::runFunctionRetOneStr(const std::string &fun,
                                 std::string &ret)
{
    if(!mLuaStatePtr)
    {
        gLogging.ftextOut("Lua State not running for %s", fun.c_str());
        return false;
    }


    lua_State *L = (lua_State *)mLuaStatePtr;
    lua_getfield(L, -1, fun.c_str());
    lua_pushvalue(L, -2);
    if (lua_pcall(L, 1, 1, 0) != 0)
    {
        gLogging.ftextOut("Error calling %s", fun.c_str());
        return false;
    }

    const char *retStr = lua_tostring(L, -1);
    if (!retStr) {
        gLogging.ftextOut("Error getval didn't return a valid string");
        return false;
    }
    ret = retStr;
    gLogging.ftextOut("ret=%s\n", retStr);

    /* Remove the return value from the stack. */
    lua_remove(L, -1);

    return true;
}

bool GsLua::runFunctionRetOneStr(const std::string &fun,
                                 const int param,
                                 std::string &ret)
{
    if(!mLuaStatePtr)
    {
        gLogging.ftextOut("Lua State not running for %s", fun.c_str());
        return false;
    }


    lua_State *L = (lua_State *)mLuaStatePtr;
    lua_getfield(L, -1, fun.c_str());
    lua_pushvalue(L, -2);
    lua_pushnumber(L, param);
    if (lua_pcall(L, 2, 1, 0) != 0)
    {
        gLogging.ftextOut("Error calling %s", fun.c_str());
        return false;
    }

    const char *retStr = lua_tostring(L, -1);
    if (!retStr) {
        gLogging.ftextOut("Error getval didn't return a valid string");
        return false;
    }
    ret = retStr;
    gLogging.ftextOut("ret=%s\n", retStr);

    /* Remove the return value from the stack. */
    lua_remove(L, -1);

    return true;
}


void GsLua::runFunctionRetOneInt(const std::string &fun, int &ret)
{
    if(!mLuaStatePtr)
    {
        gLogging.ftextOut("Lua State not running for %s", fun.c_str());
        return;
    }


    lua_State *L = (lua_State *)mLuaStatePtr;
    lua_getfield(L, -1, fun.c_str());
    lua_pushvalue(L, -2);
    if (lua_pcall(L, 1, 1, 0) != 0)
    {
        gLogging.ftextOut("Error calling %s", fun.c_str());
        return;
    }

    int isnum;
    int n = lua_tointegerx(L, -1, &isnum);
    if (!isnum) {
        gLogging.ftextOut("Error getval didn't return a number");
        return;
    }
    ret = n;
    gLogging.ftextOut("ret=%d\n", n);

    /* Remove the return value from the stack. */
    lua_remove(L, -1);
}

void GsLua::runFunctionRetOneBool(const std::string &fun, bool &ret)
{
    if(!mLuaStatePtr)
    {
        gLogging.ftextOut("Lua State not running for %s", fun.c_str());
        return;
    }


    lua_State *L = (lua_State *)mLuaStatePtr;
    lua_getfield(L, -1, fun.c_str());

    if(lua_isfunction(L, -1) != 1)
    {
        lua_remove(L, -1);
        return;
    }

    lua_pushvalue(L, -2);
    if (lua_pcall(L, 1, 1, 0) != 0)
    {
        // NOTE: It can be normal not having such function.
        // Enable this only for debugging purposes
        gLogging.ftextOut("Error calling %s", fun.c_str());
        lua_remove(L, -1);
        return;
    }

    if (lua_isboolean(L, -1) == 0)
    {
        gLogging.ftextOut("Error %s does not return a boolean", fun.c_str());
        lua_remove(L, -1);
        return;
    }

    bool isboolean = (bool) lua_toboolean(L, -1);
    gLogging.ftextOut(isboolean ? "true" : "false");

    ret = isboolean;

    /* Remove the return value from the stack. */
    lua_remove(L, -1);
}

GsLua::operator bool() const
{
    return (mLuaStatePtr != nullptr);
}
