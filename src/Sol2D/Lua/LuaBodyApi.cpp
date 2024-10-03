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

#include <Sol2D/Lua/LuaBodyApi.h>
#include <Sol2D/Lua/LuaBodyShapeApi.h>
#include <Sol2D/Lua/Aux/LuaUserData.h>
#include <Sol2D/Lua/LuaPointApi.h>
#include <Sol2D/Lua/LuaStrings.h>

using namespace Sol2D;
using namespace Sol2D::Utils;
using namespace Sol2D::Lua;
using namespace Sol2D::Lua::Aux;

namespace {

struct Self : LuaSelfBase
{
    Self(std::shared_ptr<Scene> & _scene, uint64_t _body_id) :
        body_id(_body_id),
        m_scene(_scene)
    {
    }

    std::shared_ptr<Scene> getScene(lua_State * _lua) const
    {
        std::shared_ptr<Scene> ptr =  m_scene.lock();
        if(!ptr)
            luaL_error(_lua, "the scene is destroyed");
        return ptr;
    }

    uint64_t body_id;

private:
    std::weak_ptr<Scene> m_scene;
};

using UserData = LuaUserData<Self, LuaTypeName::body>;

template<typename T>
inline void ensureResult(lua_State * _lua, const T & _result)
{
    if(!_result)
        luaL_error(_lua, "the body is invalid or has been destroyed");
}

// 1 self
int luaApi_IsValid(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    lua_pushboolean(_lua, self->getScene(_lua)->doesBodyExist(self->body_id));
    return 1;
}

// 1 self
int luaApi_GetId(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    lua_pushinteger(_lua, self->body_id);
    return 1;
}

// 1 self
// 2 shape key
int luaApi_GetShape(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    const char * shape_key = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, shape_key != nullptr, 2, "shape key expected");
    PreHashedKey<std::string> shape_pre_hashed_key = makePreHashedKey(std::string(shape_key));
    std::shared_ptr<Scene> scene = self->getScene(_lua);
    if(scene->doesBodyShapeExist(self->body_id, shape_pre_hashed_key))
        pushBodyShapeApi(_lua, scene, self->body_id, shape_pre_hashed_key);
    else
        lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 layer
int luaApi_SetLayer(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    const char * layer = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, layer != nullptr, 2, "layer name expected");
    ensureResult(_lua, self->getScene(_lua)->setBodyLayer(self->body_id, layer));
    return 0;
}

// 1 self
int luaApi_GetPosition(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    std::optional<Point> position = self->getScene(_lua)->getBodyPosition(self->body_id);
    ensureResult(_lua, position);
    pushPoint(_lua, position.value().x, position.value().y);
    return 1;
}

// 1 self
// 2 position
int luaApi_SetPosition(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    Point position;
    luaL_argcheck(_lua, tryGetPoint(_lua, 2, position), 2, "position expected");
    self->getScene(_lua)->setBodyPosition(self->body_id, position);
    return 0;
}

// 1 self
// 2 force vector (point)
int luaApi_ApplyForceToCenter(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    Point force;
    luaL_argcheck(_lua, tryGetPoint(_lua, 2, force), 2, "force vector expected");
    self->getScene(_lua)->applyForceToBodyCenter(self->body_id, force);
    return 0;
}

// 1 self
// 2 force vector (point)
int luaApi_ApplyImpulseToCenter(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    Point impulse;
    luaL_argcheck(_lua, tryGetPoint(_lua, 2, impulse), 2, "impulse vector expected");
    self->getScene(_lua)->applyImpulseToBodyCenter(self->body_id, impulse);
    return 0;
}

// 1 self
int luaApi_GetLinearVelocity(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    std::optional<Point> velocity = self->getScene(_lua)->getBodyLinearVelocity(self->body_id);
    ensureResult(_lua, velocity);
    pushPoint(_lua, velocity.value().x, velocity.value().y);
    return 1;
}

// 1 self
// 2 velocity vector
int luaApi_SetLinearVelocity(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    Point velocity;
    luaL_argcheck(_lua, tryGetPoint(_lua, 2, velocity), 2, "velocity vector expected");
    lua_pushboolean(_lua, self->getScene(_lua)->setBodyLinearVelocity(self->body_id, velocity));
    return 1;
}

// 1 self
int luaApi_GetBodyMass(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    std::optional<float> mass = self->getScene(_lua)->getBodyMass(self->body_id);
    ensureResult(_lua, mass);
    lua_pushnumber(_lua, mass.value());
    return 1;
}

} // namespace

void Lua::pushBodyApi(lua_State * _lua, std::shared_ptr<Scene> _scene, uint64_t _body_id)
{
    UserData::pushUserData(_lua, _scene, _body_id);
    if(UserData::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        luaL_Reg funcs[] =
        {
            { "__gc", UserData::luaGC },
            { "isValid", luaApi_IsValid },
            { "getId", luaApi_GetId },
            { "getShape", luaApi_GetShape },
            { "setLayer", luaApi_SetLayer },
            { "getPosition", luaApi_GetPosition },
            { "setPosition", luaApi_SetPosition },
            { "applyForceToCenter", luaApi_ApplyForceToCenter },
            { "applyImpulseToCenter", luaApi_ApplyImpulseToCenter },
            { "getLinearVelocity", luaApi_GetLinearVelocity },
            { "setLinearVelocity", luaApi_SetLinearVelocity },
            { "getMass", luaApi_GetBodyMass },
            { nullptr, nullptr }
        };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);
}

bool Lua::tryGetBodyId(lua_State * _lua, int _idx, uint64_t * _id)
{
    Self * self = UserData::tryGetUserData(_lua, _idx);
    if(self)
    {
        *_id = self->body_id;
        return true;
    }
    return false;
}