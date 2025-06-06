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

#include <Sol2D/Lua/LuaBodyDefinitionApi.h>
#include <Sol2D/Lua/LuaRectApi.h>
#include <Sol2D/Lua/LuaPointApi.h>
#include <Sol2D/Lua/LuaGraphicsPackDefinitionApi.h>
#include <Sol2D/Lua/LuaBodyPhysicsDefinitionApi.h>
#include <Sol2D/Lua/LuaBodyShapePhysicsDefinitionApi.h>
#include <Sol2D/Lua/Aux/LuaTableApi.h>
#include <Sol2D/Lua/Aux/LuaUtils.h>

using namespace Sol2D;
using namespace Sol2D::World;
using namespace Sol2D::Lua;

namespace {

template<BodyShapeType shape_type>
void readBasicShape(LuaTableApi & _table, BodyBasicShapeDefinition<shape_type> & _shape);

bool tryGetPoints(lua_State * _lua, int _idx, std::vector<SDL_FPoint> & _points);

void addPolygon(
    LuaTableApi & _table,
    const std::string & _key,
    std::vector<std::pair<std::string, BodyVariantShapeDefinition>> & _shapes
);

void addCircle(
    LuaTableApi & _table,
    const std::string & _key,
    std::vector<std::pair<std::string, BodyVariantShapeDefinition>> & _shapes
);

void addCapsule(
    LuaTableApi & _table,
    const std::string & _key,
    std::vector<std::pair<std::string, BodyVariantShapeDefinition>> & _shapes
);

void addShape(
    lua_State * _lua,
    int _idx,
    const std::string & _key,
    std::vector<std::pair<std::string, BodyVariantShapeDefinition>> & _shapes
);

void getShapes(lua_State * _lua, int _idx, std::vector<std::pair<std::string, BodyVariantShapeDefinition>> & _shapes);

void getShapes(lua_State * _lua, int _idx, std::vector<std::pair<std::string, BodyVariantShapeDefinition>> & _shapes)
{
    int dictionary_index = lua_absindex(_lua, _idx);
    if(!lua_istable(_lua, dictionary_index))
        return;
    lua_pushnil(_lua);
    while(lua_next(_lua, dictionary_index))
    {
        if(const char * key = argToString(_lua, -2))
            addShape(_lua, -1, key, _shapes);
        lua_pop(_lua, 1);
    }
}

void addShape(
    lua_State * _lua,
    int _idx,
    const std::string & _key,
    std::vector<std::pair<std::string, BodyVariantShapeDefinition>> & _shapes
)
{
    LuaTableApi table(_lua, _idx);
    if(!table.isValid())
        return;
    lua_Integer value;
    if(!table.tryGetInteger("type", &value))
        return;
    switch(value)
    {
    case static_cast<lua_Integer>(BodyShapeType::Polygon):
        addPolygon(table, _key, _shapes);
        break;
    case static_cast<lua_Integer>(BodyShapeType::Circle):
        addCircle(table, _key, _shapes);
        break;
    case static_cast<lua_Integer>(BodyShapeType::Capsule):
        addCapsule(table, _key, _shapes);
        break;
    default:
        break;
    }
}

void addPolygon(
    LuaTableApi & _table,
    const std::string & _key,
    std::vector<std::pair<std::string, BodyVariantShapeDefinition>> & _shapes
)
{
    if(_table.tryGetValue("rect"))
    {
        BodyRectDefinition def;
        if(tryGetRect(_table.getLua(), -1, def))
        {
            readBasicShape(_table, def);
            _shapes.emplace_back(_key, def);
        }
        lua_pop(_table.getLua(), 1);
    }
    else if(_table.tryGetValue("points"))
    {
        BodyPolygonDefinition def;
        if(tryGetPoints(_table.getLua(), -1, def.points))
        {
            readBasicShape(_table, def);
            _shapes.emplace_back(_key, def);
        }
        lua_pop(_table.getLua(), 1);
    }
}

template<BodyShapeType shape_type>
void readBasicShape(LuaTableApi & _table, BodyBasicShapeDefinition<shape_type> & _shape)
{
    if(_table.tryGetValue("physics"))
    {
        tryGetBodyShapePhysicsDefinition(_table.getLua(), -1, _shape.physics);
        lua_pop(_table.getLua(), 1);
    }
    if(_table.tryGetValue("graphics"))
    {
        int graphics_table_idx = lua_gettop(_table.getLua());
        lua_pushnil(_table.getLua());
        while(lua_next(_table.getLua(), graphics_table_idx))
        {
            if(const char * key = argToString(_table.getLua(), -2))
            {
                GraphicsPackDefinition graphics;
                if(tryGetGraphicsPackDefinition(_table.getLua(), -1, graphics))
                    _shape.graphics.emplace_back(key, std::move(graphics));
            }
            lua_pop(_table.getLua(), 1);
        }
        lua_pop(_table.getLua(), 1);
    }
}

bool tryGetPoints(lua_State * _lua, int _idx, std::vector<SDL_FPoint> & _points)
{
    if(!lua_istable(_lua, _idx))
        return false;
    SDL_FPoint point;
    lua_Unsigned len = lua_rawlen(_lua, _idx);
    for(lua_Unsigned i = 1; i <= len; ++i)
    {
        if(lua_rawgeti(_lua, _idx, i) == LUA_TTABLE)
        {
            if(Lua::tryGetPoint(_lua, -1, point))
                _points.push_back(point);
        }
        lua_pop(_lua, 1);
    }
    return true;
}

void addCircle(
    LuaTableApi & _table,
    const std::string & _key,
    std::vector<std::pair<std::string, BodyVariantShapeDefinition>> & _shapes
)
{
    BodyCircleDefinition def;
    {
        lua_Number radius;
        if(!_table.tryGetNumber("radius", &radius))
            return;
        def.radius = static_cast<float>(radius);
    }
    if(!_table.tryGetPoint("center", def.center))
        return;
    readBasicShape(_table, def);
    _shapes.emplace_back(_key, def);
}

void addCapsule(
    LuaTableApi & _table,
    const std::string & _key,
    std::vector<std::pair<std::string, BodyVariantShapeDefinition>> & _shapes
)
{
    BodyCapsuleShapeDefinition def;
    if(!_table.tryGetNumber("radius", &def.radius) || !_table.tryGetPoint("center1", def.center1) ||
       !_table.tryGetPoint("center2", def.center2))
    {
        return;
    }
    readBasicShape(_table, def);
    _shapes.emplace_back(_key, def);
}

} // namespace

std::unique_ptr<BodyDefinition> Sol2D::Lua::tryGetBodyDefinition(lua_State * _lua, int _idx)
{
    LuaTableApi table(_lua, _idx);
    if(!table.isValid())
        return nullptr;
    std::unique_ptr<BodyDefinition> def = std::make_unique<BodyDefinition>();
    {
        lua_Integer value;
        if(!table.tryGetInteger("type", &value))
            return nullptr;
        switch(value)
        {
        case static_cast<lua_Integer>(BodyType::Static):
            def->type = BodyType::Static;
            break;
        case static_cast<lua_Integer>(BodyType::Dynamic):
            def->type = BodyType::Dynamic;
            break;
        case static_cast<lua_Integer>(BodyType::Kinematic):
            def->type = BodyType::Kinematic;
            break;
        default:
            return nullptr;
        }
    }
    if(table.tryGetValue("physics"))
    {
        tryGetBodyPhysicsDefinition(_lua, -1, def->physics);
        lua_pop(_lua, 1);
    }
    if(table.tryGetValue("shapes"))
    {
        getShapes(_lua, -1, def->shapes);
        lua_pop(_lua, 1);
    }
    return def;
}
