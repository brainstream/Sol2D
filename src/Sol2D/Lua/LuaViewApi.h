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

#include <Sol2D/Lua/Aux/LuaForward.h>
#include <Sol2D/View.h>
#include <memory>

namespace Sol2D::Lua {

void pushViewApi(lua_State * _lua, std::shared_ptr<View> _view);
std::shared_ptr<View> tryGetView(lua_State * _lua, int _idx);

} // namespace Sol2D::Lua
