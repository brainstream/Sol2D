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

#include <Sol2D/AStar.h>
#include <cmath>
#include <map>
#include <stack>

using namespace Sol2D;

namespace {

struct MatrixIndex
{
    int32_t x, y;
};

union Index
{
    int64_t map_key;
    MatrixIndex matrix_index;
};

struct Node
{
    int32_t x, y;
    uint32_t cost;
    Index index;
    const Node * prev;
};

class AStar final
{
public:
    AStar(const b2World & _world,
          const b2Body & _body,
          const b2Vec2 & _destination,
          const AStarOptions & _options);
    ~AStar();
    std::optional<std::vector<b2Vec2>> exec();

private:
    static b2Vec2 calculateCellSize(const b2Body & _body);
    void emplaceSuccessors(const Node & _node);
    void emplaceNode(int32_t _x, int32 _y, const Node & _parent_node);
    bool isDestination(const b2Vec2 & _point) const;
    bool isDeadEnd(const b2Vec2 & _point) const;
    std::vector<b2Vec2> translatePath(const Node & _end_node) const;
    static float getDistance(const b2Vec2 & _p1, const b2Vec2 & _p2);
    static bool arePointsInRow(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3);

private:
    static const uint16_t s_step_cost = 1;
    const b2World & mr_world;
    const b2Body & mr_body;
    const b2Vec2 m_start_point;
    const b2Vec2 & mr_dest_point;
    const b2Vec2 m_cell_size;
    const AStarOptions & mr_options;
    std::map<int64_t, Node *> m_known_nodes;
    std::multimap<float, Node *> m_open_nodes;
};

} // namespace

AStar::AStar(
    const b2World & _world,
    const b2Body & _body,
    const b2Vec2 & _destination,
    const AStarOptions & _options
) :
    mr_world(_world),
    mr_body(_body),
    m_start_point(mr_body.GetPosition()),
    mr_dest_point(_destination),
    m_cell_size(calculateCellSize(_body)),
    mr_options(_options)
{
    Node * root = new Node
    {
        .x = 0,
        .y = 0,
        .cost = 0,
        .index = { .map_key = 0 },
        .prev = nullptr
    };
    m_known_nodes.emplace(0, root);
    m_open_nodes.emplace(.0f, root);
}

AStar::~AStar()
{
    for(auto & pair : m_known_nodes)
        delete pair.second;
}

b2Vec2 AStar::calculateCellSize(const b2Body & _body)
{
    const b2Fixture * fixture = _body.GetFixtureList();
    b2AABB body_aabb = fixture->GetAABB(0);
    for(const b2Fixture * f = fixture->GetNext(); f; f = f->GetNext())
    {
        if(!f->IsSensor()) // TODO: from user options
            body_aabb.Combine(f->GetAABB(0));
    }
    return b2Vec2(
        std::abs(body_aabb.upperBound.x - body_aabb.lowerBound.x),
        std::abs(body_aabb.upperBound.y - body_aabb.lowerBound.y)
    );
}

std::optional<std::vector<b2Vec2>> AStar::exec()
{
    b2Vec2 position;
    const Node * node;
    while(!m_open_nodes.empty()) // TODO: a potentially endless loop, must be a limit (?)
    {
        const auto item = m_open_nodes.extract(m_open_nodes.begin());
        node = item.mapped();
        position.x = m_start_point.x + node->x * m_cell_size.x;
        position.y = m_start_point.y + node->y * m_cell_size.y;
        if(isDestination(position))
            return translatePath(*node);
        if(!isDeadEnd(position))
            emplaceSuccessors(*node);
    }
    return std::optional<std::vector<b2Vec2>>();
}

void AStar::emplaceSuccessors(const Node & _node)
{
    emplaceNode(_node.index.matrix_index.x + 1, _node.index.matrix_index.y, _node);
    emplaceNode(_node.index.matrix_index.x - 1, _node.index.matrix_index.y, _node);
    emplaceNode(_node.index.matrix_index.x, _node.index.matrix_index.y + 1, _node);
    emplaceNode(_node.index.matrix_index.x, _node.index.matrix_index.y - 1, _node);
    if(mr_options.allow_diagonal_steps)
    {
        emplaceNode(_node.index.matrix_index.x - 1, _node.index.matrix_index.y - 1, _node);
        emplaceNode(_node.index.matrix_index.x + 1, _node.index.matrix_index.y + 1, _node);
        emplaceNode(_node.index.matrix_index.x - 1, _node.index.matrix_index.y + 1, _node);
        emplaceNode(_node.index.matrix_index.x + 1, _node.index.matrix_index.y - 1, _node);
    }
}

void AStar::emplaceNode(int32_t _x, int32 _y, const Node & _parent_node)
{
    Index index { .matrix_index { _x, _y } };
    if(m_known_nodes.contains(index.map_key))
        return;
    Node * node = new Node
    {
        .x = _x,
        .y = _y,
        .cost = _parent_node.cost + s_step_cost,
        .index = index,
        .prev = &_parent_node
    };
    m_known_nodes.emplace(index.map_key, node);
    float full_cost = node->cost +
        getDistance(mr_dest_point, { m_start_point.x + _x * m_cell_size.x, m_start_point.y + _y * m_cell_size.y });
    m_open_nodes.emplace(full_cost, node);
}

inline float AStar::getDistance(const b2Vec2 & _p1, const b2Vec2 & _p2)
{
    return std::sqrt(std::pow(_p2.x - _p1.x, 2) + std::pow(_p2.y - _p1.y, 2)); // TODO: can sqrt be removed?
}

inline bool AStar::isDestination(const b2Vec2 & _point) const
{
    return
        std::abs(_point.x - mr_dest_point.x) <= m_cell_size.x / 2 &&
        std::abs(_point.y - mr_dest_point.y) <= m_cell_size.y / 2;
}

bool AStar::isDeadEnd(const b2Vec2 & _point) const
{
    class Callback : public b2QueryCallback
    {
    public:
        explicit Callback(const b2Body & __body, const AStarOptions & __options) :
            mr_body(__body),
            mr_options(__options),
            m_is_dead_end(false)
        {
        }

        bool ReportFixture(b2Fixture * __fixture) override
        {
            if(__fixture->GetBody() == &mr_body || (__fixture->IsSensor() && !mr_options.avoid_sensors))
                return true;
            m_is_dead_end = true;
            return false;
        }

        bool isDeadEnd() const
        {
            return m_is_dead_end;
        }

    private:
        const b2Body & mr_body;
        const AStarOptions & mr_options;
        bool m_is_dead_end;
    } callback(mr_body, mr_options); // TODO: single object for every algorithm run

    float half_width = m_cell_size.x / 2; // TODO: to const
    float half_height = m_cell_size.y / 2; // TODO: to const
    b2AABB aabb {
        .lowerBound = b2Vec2(_point.x - half_width, _point.y - half_height),
        .upperBound = b2Vec2(_point.x + half_width, _point.y + half_height)
    };
    mr_world.QueryAABB(&callback, aabb);
    return callback.isDeadEnd();
}

std::vector<b2Vec2> AStar::translatePath(const Node & _end_node) const
{
    std::stack<std::pair<int32_t, int32_t>> node_points;
    node_points.push(std::make_pair(_end_node.x, _end_node.y));
    for(const Node * node = _end_node.prev; node != nullptr; node = node->prev)
    {
        const std::pair<int32_t, int32_t> & next = node_points.top();
        const Node * prev = node->prev;
        if(!prev || !arePointsInRow(prev->x, prev->y, node->x, node->y, next.first, next.second))
            node_points.push(std::make_pair(node->x, node->y));
    }
    std::vector<b2Vec2> result;
    result.reserve(node_points.size());
    while(!node_points.empty())
    {
        const std::pair<int32_t, int32_t> & point = node_points.top();
        result.push_back(b2Vec2(m_start_point.x + point.first * m_cell_size.x, m_start_point.y + point.second * m_cell_size.y));
        node_points.pop();
    }
    return result;
}

inline bool AStar::arePointsInRow(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3)
{
    return (x1 == x2 && x1 == x3) || (y1 == y2 && y1 == y3); // TODO: diagonal
}

std::optional<std::vector<b2Vec2>> Sol2D::aStarFindPath(const b2World & _world,
    const b2Body & _body,
    const b2Vec2 & _destination,
    const AStarOptions & _options)
{
    AStar a(_world, _body, _destination, _options);
    return a.exec();
}