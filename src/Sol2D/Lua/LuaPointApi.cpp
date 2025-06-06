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

#include <Sol2D/Lua/LuaPointApi.h>
#include <Sol2D/Lua/Aux/LuaTableApi.h>

using namespace Sol2D::Lua;

namespace {

static constexpr char g_key_x[] = "x";
static constexpr char g_key_y[] = "y";

} // namespace

bool Sol2D::Lua::tryGetPoint(lua_State * _lua, int _idx, SDL_FPoint & _point)
{
    LuaTableApi table(_lua, _idx);
    if(!table.isValid())
        return false;

    float x, y;
    if(table.tryGetNumber(g_key_x, &x) && table.tryGetNumber(g_key_y, &y))
    {
        _point.x = x;
        _point.y = y;
        return true;
    }
    return false;
}

void Sol2D::Lua::pushPoint(lua_State * _lua, float _x, float _y)
{
    LuaTableApi table = LuaTableApi::pushNew(_lua);
    table.setNumberValue(g_key_x, _x);
    table.setNumberValue(g_key_y, _y);
}
