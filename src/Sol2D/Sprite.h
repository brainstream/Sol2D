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

#include <Sol2D/Def.h>
#include <Sol2D/Color.h>
#include <Sol2D/Rect.h>
#include <Sol2D/Utils/Math.h>
#include <SDL3/SDL.h>
#include <filesystem>
#include <optional>

namespace Sol2D {

struct SpriteOptions
{
    SpriteOptions() :
        autodetect_rect(false)
    {
    }

    bool autodetect_rect;
    std::optional<Rect> rect;
    std::optional<Color> color_to_alpha;
};

class Sprite final
{
public:
    S2_DEFAULT_COPY_AND_MOVE(Sprite)

    explicit Sprite(SDL_Renderer & _renderer);
    Sprite(SDL_Renderer & _renderer, std::shared_ptr<SDL_Texture> _texture, const Rect & _rect);
    bool loadFromFile(const std::filesystem::path & _path, const SpriteOptions & _options = SpriteOptions());
    bool isValid() const;
    std::shared_ptr<SDL_Texture> getTexture() const;
    const Rect & getSourceRect() const;
    const Size & getDestinationSize() const;
    void setDesinationSize(const Size & _size);
    void scale(float _scale_factor);
    void scale(float _scale_factor_x, float _scale_factor_y);
    void render(const Point & _point, const Utils::Rotation & _rotation, SDL_FlipMode _flip_mode, const Point & _center);

private:
    SDL_Renderer * mp_renderer;
    std::shared_ptr<SDL_Texture> m_texture_ptr;
    Rect m_source_rect;
    Size m_desination_size;
};

inline Sprite::Sprite(SDL_Renderer & _renderer) :
    mp_renderer(&_renderer),
    m_source_rect({ .0f, .0f, .0f, .0f }),
    m_desination_size({ .0f, .0f })
{
}

inline Sprite::Sprite(SDL_Renderer & _renderer, std::shared_ptr<SDL_Texture> _texture, const Rect & _rect) :
    mp_renderer(&_renderer),
    m_texture_ptr(_texture),
    m_source_rect(_rect),
    m_desination_size(_rect.getSize())
{
}

inline bool Sprite::isValid() const
{
    return m_texture_ptr != nullptr;
}

inline std::shared_ptr<SDL_Texture> Sprite::getTexture() const
{
    return m_texture_ptr;
}

inline const Rect & Sprite::getSourceRect() const
{
    return m_source_rect;
}

inline const Size & Sprite::getDestinationSize() const
{
    return m_desination_size;
}

inline void Sprite::setDesinationSize(const Size & _size)
{
    m_desination_size = _size;
}

inline void Sprite::scale(float _scale_factor)
{
    if(_scale_factor != .0f)
    {
        m_desination_size.w = m_source_rect.w * _scale_factor;
        m_desination_size.h = m_source_rect.h * _scale_factor;
    }
}

inline void Sprite::scale(float _scale_factor_x, float _scale_factor_y)
{
    if(_scale_factor_x != .0 && _scale_factor_y != .0f)
    {
        m_desination_size.w = m_source_rect.w * _scale_factor_x;
        m_desination_size.h = m_source_rect.h * _scale_factor_y;
    }
}

} // namespace Sol2D
