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

#include <Sol2D/Lua/LuaRectApi.h>
#include <Sol2D/Lua/Aux/LuaTableApi.h>

using namespace Sol2D::Lua;

namespace {

static constexpr char g_key_x[] = "x";
static constexpr char g_key_y[] = "y";
static constexpr char g_key_w[] = "w";
static constexpr char g_key_h[] = "h";

} // namespace

bool Sol2D::Lua::tryGetRect(lua_State * _lua, int _idx, SDL_FRect & _rect)
{
    LuaTableApi table(_lua, _idx);
    if(!table.isValid())
        return false;

    float x, y, w, h;
    if(table.tryGetNumber(g_key_w, &w) && table.tryGetNumber(g_key_h, &h) && table.tryGetNumber(g_key_x, &x) &&
       table.tryGetNumber(g_key_y, &y))
    {
        _rect.x = x;
        _rect.y = y;
        _rect.w = w;
        _rect.h = h;
        return true;
    }
    return false;
}

void Sol2D::Lua::pushRect(lua_State * _lua, const SDL_FRect & _rect)
{
    LuaTableApi table = LuaTableApi::pushNew(_lua);
    table.setNumberValue(g_key_x, _rect.x);
    table.setNumberValue(g_key_y, _rect.y);
    table.setNumberValue(g_key_w, _rect.w);
    table.setNumberValue(g_key_h, _rect.h);
}
