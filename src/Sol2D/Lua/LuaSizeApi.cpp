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

#include <Sol2D/Lua/LuaSizeApi.h>
#include <Sol2D/Lua/Aux/LuaTableApi.h>

using namespace Sol2D::Lua;

namespace {

static constexpr char g_key_w[] = "w";
static constexpr char g_key_h[] = "h";

} // namespace

bool Sol2D::Lua::tryGetSize(lua_State * _lua, int _idx, FSize & _size)
{
    LuaTableApi table(_lua, _idx);
    if(!table.isValid())
        return false;

    float w, h;
    if(table.tryGetNumber(g_key_w, &w) && table.tryGetNumber(g_key_h, &h))
    {
        _size.h = h;
        _size.w = w;
        return true;
    }
    return false;
}

void Sol2D::Lua::pushSize(lua_State * _lua, float _w, float _h)
{
    LuaTableApi table = LuaTableApi::pushNew(_lua);
    table.setNumberValue(g_key_h, _h);
    table.setNumberValue(g_key_w, _w);
}
