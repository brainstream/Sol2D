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

#include <Sol2D/Lua/LuaFormApi.h>
#include <Sol2D/Lua/LuaFontApi.h>
#include <Sol2D/Lua/LuaWidgetApi.h>
#include <Sol2D/Lua/LuaColorApi.h>
#include <Sol2D/Lua/Aux/LuaStrings.h>
#include <Sol2D/Lua/Aux/LuaUserData.h>

using namespace Sol2D;
using namespace Sol2D::Forms;
using namespace Sol2D::Lua;

namespace {

struct Self : LuaSelfBase
{
    Self(const Workspace & _workspace, std::shared_ptr<Form> & _form) :
        workspace(_workspace),
        form(_form)
    {
    }

    std::shared_ptr<Form> getForm(lua_State * _lua) const
    {
        std::shared_ptr<Form> ptr = form.lock();
        if(!ptr)
            luaL_error(_lua, LuaMessage::form_is_destroyed);
        return ptr;
    }

    const Workspace & workspace;
    std::weak_ptr<Form> form;
};

using UserData = LuaUserData<Self, LuaTypeName::form>;

// 1 self
// 2 color
int luaApi_SetBackgroundColor(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    SDL_FColor color;
    luaL_argexpected(_lua, tryGetColor(_lua, 2, color), 2, LuaTypeName::color);
    self->getForm(_lua)->setClearColor(color);
    return 0;
}

// 1 self
// 2 text
int luaApi_CreateLabel(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    const char * text = lua_tostring(_lua, 2);
    std::shared_ptr<Label> label = self->getForm(_lua)->createLabel(text ? std::string(text) : std::string());
    pushLabelApi(_lua, label);
    return 1;
}

// 1 self
// 2 text
int luaApi_CreateButton(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    const char * text = lua_tostring(_lua, 2);
    std::shared_ptr<Button> button = self->getForm(_lua)->createButton(text ? std::string(text) : std::string());
    pushButtonApi(_lua, button, self->workspace);
    return 1;
}

} // namespace

void Sol2D::Lua::pushFormApi(lua_State * _lua, const Workspace & _workspace, std::shared_ptr<Form> _form)
{
    UserData::pushUserData(_lua, _workspace, _form);
    if(UserData::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        luaL_Reg funcs[] = {
            {"__gc",               UserData::luaGC          },
            {"setBackgroundColor", luaApi_SetBackgroundColor},
            {"createLabel",        luaApi_CreateLabel       },
            {"createButton",       luaApi_CreateButton      },
            {nullptr,              nullptr                  }
        };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);
}

std::shared_ptr<Forms::Form> Sol2D::Lua::tryGetForm(lua_State * _lua, int _idx)
{
    if(const Self * self = UserData::tryGetUserData(_lua, _idx))
        return self->getForm(_lua);
    return nullptr;
}
