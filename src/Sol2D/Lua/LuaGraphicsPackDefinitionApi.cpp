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

#include <Sol2D/Lua/LuaSpriteApi.h>
#include <Sol2D/Lua/LuaSpriteSheetApi.h>
#include <Sol2D/Lua/LuaPointApi.h>
#include <Sol2D/Lua/LuaGraphicsPackDefinitionApi.h>
#include <Sol2D/Lua/Aux/LuaTable.h>

using namespace Sol2D;
using namespace Sol2D::Lua;
using namespace Sol2D::Lua::Aux;

namespace {

void addSprites(lua_State * _lua, int _idx, GraphicsPackFrameDefinition & _frame_def)
{
    int count = lua_rawlen(_lua, _idx);
    _frame_def.sprites.reserve(count);
    for(int i = 1; i <= count; ++i)
    {
        lua_rawgeti(_lua, _idx, i);
        GraphicsPackSpriteDefinition sprite_definition;
        if(tryGetGraphicsPackSpriteDefinition(_lua, -1, sprite_definition))
            _frame_def.sprites.push_back(std::move(sprite_definition));
        lua_pop(_lua, 1); // lua_rawgeti
    }
}

void addFrames(lua_State * _lua, int _idx, GraphicsPackDefinition & _pack_def)
{
    int count = lua_rawlen(_lua, _idx);
    _pack_def.frames.reserve(count);
    for(int i = 1; i <= count; ++i)
    {
        lua_rawgeti(_lua, _idx, i);
        GraphicsPackFrameDefinition frame;
        if(tryGetGraphicsPackFrameDefinition(_lua, -1, frame))
            _pack_def.frames.push_back(std::move(frame));
        lua_pop(_lua, 1); // lua_rawgeti
    }
}

} // namespace

bool Sol2D::Lua::tryGetGraphicsPackDefinition(
    lua_State * _lua,
    int _idx,
    GraphicsPackDefinition & _result)
{
    if(!lua_istable(_lua, _idx))
        return false;

    LuaTable table(_lua, _idx);
    if(table.tryGetValue("position"))
    {
        tryGetPoint(_lua, -1, _result.position);
        lua_pop(_lua, 1); // position
    }
    if(table.tryGetValue("flipCenter"))
    {
        Point flip_center;
        if(tryGetPoint(_lua, -1, flip_center))
            _result.flip_center = flip_center;
        lua_pop(_lua, 1); // flipCenter
    }
    table.tryGetBoolean("isFlippedHorizontally", &_result.is_flipped_horizontally);
    table.tryGetBoolean("isFlippedVertically", &_result.is_flipped_vertically);
    {
        lua_Integer value;
        if(table.tryGetInteger("animationIterations", &value))
            _result.animation_iterations = static_cast<int32_t>(value);
    }
    if(table.tryGetValue("frames"))
    {
        addFrames(_lua, -1, _result);
        lua_pop(_lua, 1); // frames
    }
    return true;
}

bool Sol2D::Lua::tryGetGraphicsPackFrameDefinition(lua_State * _lua, int _idx, GraphicsPackFrameDefinition & _result)
{
    if(!lua_istable(_lua, _idx))
        return false;

    LuaTable table(_lua, _idx);
    {
        lua_Integer value;
        if(table.tryGetInteger("duration", &value))
            _result.duration = std::chrono::milliseconds(static_cast<std::chrono::milliseconds::rep>(value));
    }
    {
        bool value;
        if(table.tryGetBoolean("isVisible", &value))
            _result.is_visible = value;
    }
    if(table.tryGetValue("sprites"))
    {
        addSprites(_lua, -1, _result);
        lua_pop(_lua, 1); // sprites
    }
    return true;
}

bool Sol2D::Lua::tryGetGraphicsPackSpriteDefinition(lua_State * _lua, int _idx, GraphicsPackSpriteDefinition & _result)
{
    if(!lua_istable(_lua, _idx))
        return false;
    LuaTable table(_lua, _idx);
    if(!table.tryGetValue("sprite"))
        return false;

    bool is_there_sprite = true;
    if(std::shared_ptr<Sprite> sprite = tryGetSprite(_lua, _idx))
    {
        _result.sprite = sprite;
    }
    else if(lua_istable(_lua, -1))
    {
        LuaTable sprite_definition_table(_lua, -1);
        if(sprite_definition_table.tryGetValue("spriteSheet"))
        {
            std::shared_ptr<SpriteSheet> sprite_sheet = tryGetSpriteSheet(_lua, -1);
            lua_pop(_lua, 1); // spriteSheet
            lua_Integer sprite_index;
            if(sprite_sheet && sprite_definition_table.tryGetInteger("spriteIndex", &sprite_index))
                _result.sprite = GraphicsPackSpriteSheetSpriteDefinition(sprite_sheet, static_cast<size_t>(sprite_index));
            else
                is_there_sprite = false;
        }
    }
    else
    {
        is_there_sprite = false;
    }
    if(is_there_sprite)
    {
        table.tryGetBoolean("isVisible", &_result.is_visible);
        if(table.tryGetValue("position"))
        {
            tryGetPoint(_lua, -1, _result.position);
            lua_pop(_lua, 1); // position
        }
    }
    lua_pop(_lua, 1); // sprite
    return true;
}