#pragma once

#define SOL_ALL_SAFETIES_ON 1

#include <sol/sol.hpp>


#include "core/misc/string.h"

namespace sol {
namespace stack {

template <>
struct unqualified_pusher<luna::LString > {
	static int push(lua_State *L, const luna::LString &str) {
		return stack::push(L, str.c_str());
	}
};

template <>
struct unqualified_getter<luna::LString> {
	static luna::LString get(lua_State *L, int index, record &tracking) {
		tracking.use(1); // THIS IS THE ONLY BIT THAT CHANGES
		const char *luastr = stack::get<const char *>(L, index);
		// same as before
		return luna::LString(luastr);
	}
};

}
template <>
struct lua_type_of< luna::LString > : std::integral_constant<type, type::string> {};
} // stack