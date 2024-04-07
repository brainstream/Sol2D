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

#include <Sol2D/SDL.h>
#include <Sol2D/Def.h>
#include <vector>
#include <optional>
#include <filesystem>

namespace Sol2D {

struct SpriteSheetOptions
{
    uint32_t sprite_width;
    uint32_t sprite_height;
    uint32_t row_count;
    uint32_t col_count;
    uint32_t margin_top;
    uint32_t margin_left;
    uint32_t horizontal_spacing;
    uint32_t vertical_spacing;
    std::optional<SDL_Color> color_to_alpha;
};

class SpriteSheet final
{
public:
    S2_DISABLE_COPY(SpriteSheet)
    S2_DEFAULT_MOVE(SpriteSheet)

    explicit SpriteSheet(SDL_Renderer & _renderer);
    bool loadFromFile(const std::filesystem::path & _path, const SpriteSheetOptions & _options);
    bool isValid() const;
    size_t getSpriteCount() const;
    const std::vector<SDL_FRect> & getRects() const;
    SDL_TexturePtr getTexture() const;

private:
    SDL_Renderer * mp_renderer;
    SDL_TexturePtr m_texture_ptr;
    std::vector<SDL_FRect> m_rects;
};

inline bool SpriteSheet::isValid() const
{
    return m_texture_ptr && !m_rects.empty();
}

inline size_t SpriteSheet::getSpriteCount() const
{
    return m_rects.size();
}

inline const std::vector<SDL_FRect> & SpriteSheet::getRects() const
{
    return m_rects;
}

inline SDL_TexturePtr SpriteSheet::getTexture() const
{
    return m_texture_ptr;
}

} // namespace Sol2D