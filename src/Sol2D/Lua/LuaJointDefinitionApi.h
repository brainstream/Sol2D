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

#pragma once

#include <Sol2D/World/JointDefinition.h>
#include <Sol2D/Lua/Aux/LuaForward.h>

namespace Sol2D::Lua {

bool tryGetDistanceJointDefinition(lua_State * _lua, int _idx, World::DistanceJointDefinition & _result);
bool tryGetMotorJointDefinition(lua_State * _lua, int _idx, World::MotorJointDefinition & _result);
bool tryGetMouseJointDefinition(lua_State * _lua, int _idx, World::MouseJointDefinition & _result);
bool tryGetPrismaticJointDefinition(lua_State * _lua, int _idx, World::PrismaticJointDefinition & _result);
bool tryGetWeldJointDefinition(lua_State * _lua, int _idx, World::WeldJointDefinition & _result);
bool tryGetWheelJointDefinition(lua_State * _lua, int _idx, World::WheelJointDefinition & _result);

} // namespace Sol2D::Lua
