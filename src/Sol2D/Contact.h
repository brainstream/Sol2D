// Sol2D Game Engine
// Copyright (C) 2023-2024 Sergey Smolyannikov aka brainstream
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

#include <cstdint>
#include <string>
#include <optional>

namespace Sol2D {

struct ContactSide
{
    uint64_t body_id;
    std::string shape_key;
    std::optional<uint32_t> tile_map_object_id;
};

struct Contact
{
    ContactSide side_a;
    ContactSide side_b;
};

class ContactObserver
{
public:
    virtual ~ContactObserver() { }
    virtual void beginContact(Contact & _contact) = 0;
    virtual void endContact(Contact & _contact) = 0;
};

} // namespace Sol2D