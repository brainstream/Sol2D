// Sol2D Game Engine
// Copyright (C) 2023-2025 Sergey Smolyannikov aka brainstream
//
// This program is free software: you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option) any
// later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Lesser Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include <Sol2D/Lua/Aux/LuaTableApi.h>
#include <Sol2D/Lua/Aux/LuaCallbackStorage.h>
#include <Sol2D/Lua/Aux/LuaMetatable.h>

//
// Lua Global Registry
// └── Callbacks Table
//     ├── Owner 1
//     │   ├── Event 1
//     │   │   ├── Callback 1
//     │   │   ├── …
//     │   │   └── Callback N
//     │   ├── …
//     │   └── Event N
//     │       ├── Callback 1
//     │       ├── …
//     │       └── Callback N
//     ├── …
//     └── Owner N
//         ├── Event 1
//         │   ├── Callback 1
//         │   ├── …
//         │   └── Callback N
//         ├── …
//         └── Event N
//             ├── Callback 1
//             ├── …
//             └── Callback N
//
// Callbacks are records: (<Subscription ID>, <Lua Function>)
//

using namespace Sol2D;
using namespace Sol2D::Lua;

namespace {

inline std::string makeOwnerKey(const void * _owner)
{
    return std::to_string(reinterpret_cast<size_t>(_owner));
}

inline std::string makeEventKey(uint16_t _event_id)
{
    return std::to_string(_event_id);
}

} // namespace

const char LuaCallbackStorage::sc_callback_registry_key = '\0';
bool LuaCallbackStorage::s_is_disposed = false;
Utils::SequentialId<uint32_t> LuaCallbackStorage::m_sequential_id;

int LuaCallbackStorage::luaGC(lua_State *)
{
    s_is_disposed = true;
    return 0;
}

uint32_t LuaCallbackStorage::addCallback(const void * _owner, uint16_t _event_id, int _callback_idx)
{
    if(s_is_disposed)
        return 0;

    const int callback_abs_idx = lua_absindex(m_lua, _callback_idx);
    const uint32_t id = m_sequential_id.getNext();

    getCallbackRegisty();
    ensureEventsTable(_owner, _event_id);

    lua_pushinteger(m_lua, id);
    lua_pushvalue(m_lua, callback_abs_idx);
    lua_settable(m_lua, -3);

    lua_pop(m_lua, 2);
    return id;
}

void LuaCallbackStorage::getCallbackRegisty()
{
    if(lua_rawgetp(m_lua, LUA_REGISTRYINDEX, &sc_callback_registry_key) != LUA_TTABLE)
    {
        lua_pop(m_lua, 1);
        createCallbackRegisty();
    }
}

void LuaCallbackStorage::createCallbackRegisty()
{
    lua_newtable(m_lua);
    pushMetatable(m_lua, "sol.Internal.CallbackStorage");
    luaL_Reg funcs[] = {
        {"__gc",  LuaCallbackStorage::luaGC}, // Lua is destroying, application is terminating
        {nullptr, nullptr                  }
    };
    luaL_setfuncs(m_lua, funcs, 0);
    lua_setmetatable(m_lua, -2);
    lua_pushvalue(m_lua, -1);
    lua_rawsetp(m_lua, LUA_REGISTRYINDEX, &sc_callback_registry_key);
}

// The callback registry must be on the top of the stack
bool LuaCallbackStorage::tryGetEventsTable(const void * _owner, uint16_t _event_id)
{
    if(lua_getfield(m_lua, -1, makeOwnerKey(_owner).c_str()) != LUA_TTABLE)
    {
        lua_pop(m_lua, 1);
        return false;
    }

    if(lua_getfield(m_lua, -1, makeEventKey(_event_id).c_str()) != LUA_TTABLE)
    {
        lua_pop(m_lua, 2);
        return false;
    }

    lua_remove(m_lua, -2);

    return true;
}

// The callback registry must be on the top of the stack
void LuaCallbackStorage::ensureEventsTable(const void * _owner, uint16_t _event_id)
{
    luaL_getsubtable(m_lua, -1, makeOwnerKey(_owner).c_str());
    luaL_getsubtable(m_lua, -1, makeEventKey(_event_id).c_str());
    lua_remove(m_lua, -2);
}

size_t LuaCallbackStorage::removeCallback(const void * _owner, uint16_t _event_id, uint32_t _subscription_id)
{
    if(s_is_disposed)
        return 0;

    getCallbackRegisty();
    if(!tryGetEventsTable(_owner, _event_id))
    {
        lua_pop(m_lua, 1);
        return 0;
    }

    lua_pushinteger(m_lua, static_cast<lua_Integer>(_subscription_id));
    lua_pushnil(m_lua);
    lua_settable(m_lua, -3);

    size_t callback_count = lua_rawlen(m_lua, -1);

    lua_pop(m_lua, 2);

    return callback_count;
}

void LuaCallbackStorage::execute(
    const Workspace & _workspace,
    const void * _owner,
    uint16_t _event_id,
    uint16_t _args_count,
    uint16_t _return_count /*= 0*/,
    std::optional<std::function<bool()>> _callback /*= std::nullopt*/
)
{
    if(s_is_disposed)
        return;

    const int args_top = lua_gettop(m_lua);
    getCallbackRegisty();
    if(!tryGetEventsTable(_owner, _event_id))
    {
        lua_pop(m_lua, 1);
        return;
    }
    const int callbacks_table_idx = lua_gettop(m_lua);
    lua_pushnil(m_lua);
    for(bool exit = false; !exit && lua_next(m_lua, callbacks_table_idx);)
    {
        if(lua_type(m_lua, -2) != LUA_TNUMBER)
        {
            lua_pop(m_lua, 1);
            continue;
        }
        for(uint32_t i = 1; i <= _args_count; ++i)
            lua_pushvalue(m_lua, args_top - _args_count + i);
        if(lua_pcall(m_lua, _args_count, _return_count, 0) != LUA_OK)
        {
            _workspace.getMainLogger().error(lua_tostring(m_lua, -1));
            lua_pop(m_lua, 1);
        }
        if(_callback.has_value() && !_callback.value()())
            exit = true;
        if(_return_count)
            lua_pop(m_lua, _return_count);
    }
    lua_pop(m_lua, _args_count + 2);
}

void LuaCallbackStorage::destroyCallbacks(const void * _owner)
{
    if(s_is_disposed)
        return;

    getCallbackRegisty();
    lua_pushnil(m_lua);
    lua_setfield(m_lua, -2, makeOwnerKey(_owner).c_str());
}
