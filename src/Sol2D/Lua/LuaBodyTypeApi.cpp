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

#include <Sol2D/Lua/Aux/LuaMetatable.h>
#include <Sol2D/Lua/Aux/LuaTableApi.h>
#include <Sol2D/Lua/LuaBodyTypeApi.h>
#include <Sol2D/Lua/Aux/LuaStrings.h>
#include <Sol2D/World/BodyType.h>

using namespace Sol2D::World;
using namespace Sol2D::Lua;

void Sol2D::Lua::pushBodyTypeEnum(lua_State * _lua)
{
    lua_newuserdata(_lua, 1);
    if(pushMetatable(_lua, LuaTypeName::body_type) == MetatablePushResult::Created)
    {
        LuaTableApi table(_lua);
        table.setIntegerValue("DYNAMIC", static_cast<lua_Integer>(BodyType::Dynamic));
        table.setIntegerValue("STATIC", static_cast<lua_Integer>(BodyType::Static));
        table.setIntegerValue("KINEMATIC", static_cast<lua_Integer>(BodyType::Kinematic));
    }
    lua_setmetatable(_lua, -2);
}
