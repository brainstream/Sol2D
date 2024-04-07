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

#include <Sol2D/BodyPrototype.h>
#include <Sol2D/BodyOptions.h>
#include <Sol2D/BodyShapeOptions.h>
#include <Sol2D/Contact.h>
#include <Sol2D/Workspace.h>
#include <Sol2D/Tiles/TileMap.h>
#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <boost/container/slist.hpp>
#include <filesystem>
#include <unordered_set>
#include <functional>

namespace Sol2D {

namespace Private {

class SceneContactListener;

} // namespace Private

class Scene final // TODO: rename to Surface
{
public:
    S2_DISABLE_COPY_AND_MOVE(Scene)

    Scene(const Workspace & _workspace, SDL_Renderer & _renderer);
    ~Scene();
    uint64_t createBody(const SDL_FPoint & _position, const BodyPrototype & _prototype);
    void createBodiesFromMapObjects(
        const std::string & _class,
        const BodyOptions & _body_options,
        const BodyShapeOptions & _shape_options);
    bool destroyBody(uint64_t _body_id);
    bool setFollowedBody(uint64_t _body_id);
    void resetFollowedBody();
    bool setBodyLayer(uint64_t _body_id, const std::string & _layer);
    bool setBodyShapeCurrentGraphic(
        uint64_t _body_id,
        const std::string & _shape_key,
        const std::string & _graphic_key);
    bool flipBodyShapeGraphic(
        uint64_t _body_id,
        const std::string & _shape_key,
        const std::string & _graphic_key,
        bool _flip_horizontally,
        bool _flip_vertically);
    bool loadTileMap(const std::filesystem::path & _file_path);
    const Tiles::TileMapObject * getTileMapObjectById(uint32_t _id) const;
    const Tiles::TileMapObject * getTileMapObjectByName(std::string _name) const;
    void render(const SDL_FRect & _viewport, std::chrono::milliseconds _time_passed);
    void applyForce(uint64_t _body_id, const SDL_FPoint & _force);
    void setBodyPosition(uint64_t _body_id, const SDL_FPoint & _position);
    std::optional<SDL_FPoint> getBodyPosition(uint64_t _body_id) const;
    void addContactObserver(ContactObserver & _observer);
    void removeContactObserver(ContactObserver & _observer);
    std::optional<std::vector<SDL_FPoint>> findPath(
        uint64_t _body_id,
        const SDL_FPoint & _destination,
        bool _allow_diagonal_steps,
        bool _avoid_sensors) const;

private:
    void deinitialize();
    void destroyBody(b2Body * _body);
    static b2BodyType mapBodyType(BodyType _type);
    void executeDefers();
    void syncWorldWithFollowedBody(const SDL_FRect & _viewport);
    void drawLayersAndBodies(const Tiles::TileMapLayerContainer & _container,
                             const SDL_FRect & _viewport,
                             std::unordered_set<uint64_t> & _bodies_to_render,
                             std::chrono::milliseconds _time_passed);
    b2Body * findBody(uint64_t _body_id) const;
    void drawBody(b2Body & _body, std::chrono::milliseconds _time_passed);
    void drawObjectLayer(const Tiles::TileMapObjectLayer & _layer);
    void drawPolyXObject(const Tiles::TileMapPolyX & _poly, bool _close);
    void drawCircle(const Tiles::TileMapCircle & _circle);
    void drawTileLayer(const SDL_FRect & _viewport, const Tiles::TileMapTileLayer & _layer);
    void drawImageLayer(const Tiles::TileMapImageLayer & _layer);
    void drawBox2D();
    SDL_FPoint toAbsoluteCoords(float _world_x, float _world_y) const;

private:
    const Workspace & mr_workspace;
    SDL_Renderer & mr_renderer;
    SDL_FPoint m_world_offset;
    b2World * mp_b2_world;
    float m_scale_factor;
    std::unordered_map<uint64_t, b2Body *> m_bodies;
    b2Body * mp_followed_body;
    Private::SceneContactListener * mp_contact_listener;
    std::unique_ptr<Tiles::TileHeap> m_tile_heap_ptr;
    std::unique_ptr<Tiles::ObjectHeap> m_object_heap_ptr;
    std::unique_ptr<Tiles::TileMap> m_tile_map_ptr;
    boost::container::slist<std::function<void()>> m_defers;
};

inline const Tiles::TileMapObject * Scene::getTileMapObjectById(uint32_t _id) const
{
    return m_object_heap_ptr->findObject<Tiles::TileMapObject>(_id);
}

inline const Tiles::TileMapObject * Scene::getTileMapObjectByName(std::string _name) const
{
    return m_object_heap_ptr->findObject<Tiles::TileMapObject>(_name);
}

} // namespace Sol2D