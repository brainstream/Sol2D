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

#include <Sol2D/Forms/Label.h>

using namespace Sol2D::Forms;

Label::Label(const Canvas & _parent, const std::string & _text, Renderer & _renderer) :
    Widget(_parent, _renderer),
    m_text(_text),
    m_texture_width(.0f),
    m_texture_height(.0f),
    horizontal_text_alignment(HorizontalTextAlignment::None),
    vertical_text_alignment(VerticalTextAlignment::None)
{
    m_horizontal_text_alignment_side_effect = std::make_unique<SideEffect<HorizontalTextAlignment>>(this);
    m_vertical_text_alignment_side_effect = std::make_unique<SideEffect<VerticalTextAlignment>>(this);
    horizontal_text_alignment.addObserver(*m_horizontal_text_alignment_side_effect);
    vertical_text_alignment.addObserver(*m_vertical_text_alignment_side_effect);
}

bool Label::setText(const std::string & _text)
{
    if(_text != m_text)
    {
        m_text = _text;
        m_texture.reset();
        return true;
    }
    return false;
}

bool Label::setState(WidgetState _state)
{
    if(Widget::setState(_state))
    {
        m_texture.reset();
        return true;
    }
    return false;
}

void Label::step(const StepState & _state)
{
    std::shared_ptr<TTF_Font> font = this->font[m_state];
    if(!font)
        return;
    if(!m_texture)
        createTexture(font.get());

    const float canvas_width = getParent().getWidth();
    const float canvas_height = getParent().getHeight();
    const float border_width = this->border_width[m_state];
    const float border_width_x_2 = border_width * 2;
    const WidgetPadding & padding = this->padding[m_state];
    const float padding_left = padding.left.getPixels(canvas_width);
    const float padding_right = padding.right.getPixels(canvas_width);
    const float padding_top = padding.top.getPixels(canvas_height);
    const float padding_bottom = padding.bottom.getPixels(canvas_height);
    const SDL_FRect control_rect {
        .x = m_x.getPixels(getParent().getWidth()),
        .y = m_y.getPixels(getParent().getHeight()),
        .w = m_width.getPixels(canvas_width),
        .h = m_height.getPixels(canvas_height)
    };
    const float draw_area_x = control_rect.x + padding_left + border_width;
    const float draw_area_y = control_rect.y + padding_top + border_width;
    const float draw_area_width = control_rect.w - border_width_x_2 - padding_left - padding_right;
    const float draw_area_height = control_rect.h - border_width_x_2 - padding_top - padding_bottom;
    const HorizontalTextAlignment htext_alignment = this->horizontal_text_alignment[m_state];
    const VerticalTextAlignment vtext_alignment = this->vertical_text_alignment[m_state];

    SDL_FRect dest_rect;
    dest_rect.w = draw_area_width > m_texture_width ? m_texture_width : draw_area_width;
    dest_rect.h = draw_area_height > m_texture_height ? m_texture_height : draw_area_height;
    SDL_FRect src_rect {.x = .0f, .y = .0f, .w = dest_rect.w, .h = dest_rect.h};

    // TODO: RTL
    switch(htext_alignment)
    {
    case HorizontalTextAlignment::None:
    case HorizontalTextAlignment::Begin:
        dest_rect.x = draw_area_x;
        break;
    case HorizontalTextAlignment::End:
        dest_rect.x = draw_area_x + draw_area_width - m_texture_width;
        if(src_rect.w < m_texture_width)
            src_rect.x = m_texture_width - src_rect.w;
        break;
    case HorizontalTextAlignment::Center:
        dest_rect.x = draw_area_x + (draw_area_width - m_texture_width) / 2;
        if(src_rect.w < m_texture_width)
            src_rect.x = (m_texture_width - src_rect.w) / 2;
        break;
    }

    switch(vtext_alignment)
    {
    case VerticalTextAlignment::None:
    case VerticalTextAlignment::Top:
        dest_rect.y = draw_area_y;
        break;
    case VerticalTextAlignment::Bottom:
        dest_rect.y = draw_area_y + draw_area_height - m_texture_height;
        if(src_rect.h < m_texture_height)
            src_rect.y = m_texture_height - src_rect.h;
        break;
    case VerticalTextAlignment::Center:
        dest_rect.y = draw_area_y + (draw_area_height - m_texture_height) / 2;
        if(src_rect.h < m_texture_height)
            src_rect.y = (m_texture_height - src_rect.h) / 2;
        break;
    }

    if(dest_rect.x < draw_area_x)
        dest_rect.x = draw_area_x;
    if(dest_rect.y < draw_area_y)
        dest_rect.y = draw_area_y;

    SDL_FColor bg_color = background_color[m_state];
    if(bg_color.a != 0)
        m_renderer.renderRect(SolidRectRenderingData(control_rect, bg_color));

    m_renderer.renderTexture(TextureRenderingData(dest_rect, m_texture, src_rect));
    Widget::step(_state);
}

void Label::createTexture(TTF_Font * _font)
{
    SDL_Surface * surface = TTF_RenderText_Shaded(
        _font,
        m_text.c_str(),
        m_text.size(),
        toR8G8B8A8_UINT(foreground_color[m_state]),
        toR8G8B8A8_UINT(background_color[m_state])
    );
    m_texture = m_renderer.createTexture(*surface, "Label Text");
    m_texture_width = static_cast<float>(surface->w);
    m_texture_height = static_cast<float>(surface->h);
    SDL_DestroySurface(surface);
}
