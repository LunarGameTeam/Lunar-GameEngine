#include "lua_env.h"
#include "world/entity.h"
#include "world/component.h"
#include "world/scene.h"
#include <sol/types.hpp>
namespace luna
{

Scene* CreateScene(const LString& va)
{
	return new Scene();
}

void luna::LuaEnv::Init()
{
	m_lua_state.open_libraries(sol::lib::base, sol::lib::package);
	m_luna_namespace = m_lua_state["luna"].get_or_create<sol::table>();
	auto entity = m_luna_namespace.new_usertype<Entity>("Entity");
	m_luna_namespace.new_usertype<Component>("Component");
	m_luna_namespace.new_usertype<Scene>("Scene");
	m_luna_namespace.new_usertype<LString>("string");
	m_luna_namespace.set_function("create_scene", &CreateScene);
}

void luna::LuaEnv::RunScript(const LString &str)
{
}

sol::state &luna::LuaEnv::GetLuaState()
{
	return m_lua_state;
}


}
