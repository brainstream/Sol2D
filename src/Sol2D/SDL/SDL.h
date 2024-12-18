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

#include <SDL3/SDL.h>
#include <memory>

namespace Sol2D::SDL {

inline std::shared_ptr<SDL_Texture> wrapTexture(SDL_Texture * _texture)
{
    return std::shared_ptr<SDL_Texture>(_texture, SDL_DestroyTexture);
}

void sdlRenderCircle(SDL_Renderer * _renderer, const SDL_FPoint & _center, float _radius);

void sdlRenderCapsule(
    SDL_Renderer * _renderer,
    const SDL_FPoint & _center1,
    const SDL_FPoint & _center2,
    float _radius);

void sdlDetectContentRect(const SDL_Surface & _surface, SDL_Rect & _rect);

} // namespace Sol2D::SDL

bool operator == (const SDL_Color & _color1, const SDL_Color & _color2);
