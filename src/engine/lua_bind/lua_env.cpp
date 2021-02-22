#include "lua_env.h"
#include "world/entity.h"
#include "world/component.h"
#include "world/scene.h"
#include <sol/types.hpp>
namespace luna
{

int LuaExceptionHandler(lua_State* L, sol::optional<const std::exception&> maybe_exception, sol::string_view description) {
	// L is the lua state, which you can wrap in a state_view if necessary
	// maybe_exception will contain exception, if it exists
	// description will either be the what() of the exception or a description saying that we hit the general-case catch(...)
	std::cout << "An exception occurred in a function, here's what it says ";
	if (maybe_exception) {
		std::cout << "(straight from the exception): ";
		const std::exception& ex = *maybe_exception;
		std::cout << ex.what() << std::endl;
	}
	else {
		std::cout << "(from the description parameter): ";
		std::cout.write(description.data(), static_cast<std::streamsize>(description.size()));
		std::cout << std::endl;
	}

	// you must push 1 element onto the stack to be
	// transported through as the error object in Lua
	// note that Lua -- and 99.5% of all Lua users and libraries -- expects a string
	// so we push a single string (in our case, the description of the error)
	return sol::stack::push(L, description);
}

void luna::LuaEnv::Init()
{
	m_lua_state.open_libraries(sol::lib::base, sol::lib::package);
	m_lua_state.set_exception_handler(&LuaExceptionHandler);

	m_luna_namespace = m_lua_state["luna"].get_or_create<sol::table>();	
	auto component = m_luna_namespace.new_usertype<Component>("Component");
	auto entity = m_luna_namespace.new_usertype<Entity>("Entity");
	auto scene = m_luna_namespace.new_usertype<Scene>("Scene");
	auto scene_manager = m_luna_namespace.new_usertype<SceneManager>("SceneManager");
	scene_manager.set_function("instance", SceneManager::instance);
	scene_manager.set("main_scene", sol::property(&SceneManager::MainScene));
	auto str = m_luna_namespace.new_usertype<LString>("string");
}

void luna::LuaEnv::RunScript(const LString &str)
{
}

sol::state &luna::LuaEnv::GetLuaState()
{
	return m_lua_state;
}


}
