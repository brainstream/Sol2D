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

#include <Sol2D/StepState.h>
#include <Sol2D/Object.h>
#include <Sol2D/Def.h>

namespace Sol2D {

class Canvas : public Object
{
    S2_DISABLE_COPY_AND_MOVE(Canvas)

public:
    Canvas() :
        m_rect {.0f, .0f, .0f, .0f},
        m_clear_color {.0f, .0f, .0f, 1.0f}
    {
    }

    void setClearColor(const SDL_FColor & _color);
    const SDL_FColor & getClearColor() const;
    void reconfigure(const SDL_FRect & _rect);
    float getWidth() const;
    float getHeight() const;
    virtual void step(const StepState & _state) = 0;
    SDL_FPoint getTranslatedPoint(float _x, float _y) const;
    void translatePoint(float * _x, float * _y) const;

private:
    SDL_FRect m_rect;
    SDL_FColor m_clear_color;
};

inline void Canvas::setClearColor(const SDL_FColor & _color)
{
    m_clear_color = _color;
}

inline const SDL_FColor & Canvas::getClearColor() const
{
    return m_clear_color;
}

inline void Canvas::reconfigure(const SDL_FRect & _rect)
{
    m_rect = _rect;
}

inline float Canvas::getWidth() const
{
    return m_rect.w;
}

inline float Canvas::getHeight() const
{
    return m_rect.h;
}

inline SDL_FPoint Canvas::getTranslatedPoint(float _x, float _y) const
{
    return {.x = _x - m_rect.x, .y = _y - m_rect.y};
}

inline void Canvas::translatePoint(float * _x, float * _y) const
{
    *_x -= m_rect.x;
    *_y -= m_rect.y;
}

} // namespace Sol2D
