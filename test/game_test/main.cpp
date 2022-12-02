#include "core/core_library.h"
#include "core/file/file_subsystem.h"
#include "core/asset/asset_subsystem.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include "core/config/config.h"

using namespace luna;

#include <gtest/gtest.h>
#include "lua/lua.h"
#include <lua/lauxlib.h>
#include "lua/lualib.h"


int printMsg(lua_State* L)
{
	return 0;
}
class NativeClass
{
public:
	int Print(int i)
	{
		std::cout << i;
		return 0;
	}
};

template<typename TCls, typename TRet, typename ...ARGS>
lua_CFunction MemberFunctionCaller(TRet(TCls::* func)(ARGS...))
{
	typedef decltype(func) FTYPE; (void)(func);
	struct HelperClass
	{
		static int Invoke(lua_State* state)
		{
			void* calleePtr = lua_touserdata(state, lua_upvalueindex(1));
			luaL_argcheck(state, calleePtr, 1, "cpp closure function not found.");
			if (calleePtr)
			{
				volatile int idx = sizeof...(ARGS) + 1; (void)(idx);
				LuaStackReturn<TRet>(state, (LuaStack<TCls>::get(state, 1).* * (FTYPE*)(calleePtr))(LuaStack<ARGS>::get(state, idx--)...));
				return 1;
			}
			return 0;
		}
	};
	return HelperClass::Invoke;
}


lua_State* L;
int main(int argc, const char *argv[])
{
	L = luaL_newstate();
	luaL_openlibs(L); /* opens the basic library */
	// reg func
	lua_pushcfunction(L, printMsg);
	lua_setglobal(L, "my_printMsg");
	return 1;
}