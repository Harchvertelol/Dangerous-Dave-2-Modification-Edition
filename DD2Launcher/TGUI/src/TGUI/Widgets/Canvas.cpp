/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2019 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Widgets/Canvas.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Canvas::Canvas(const Layout2d& size)
    {
        m_type = "Canvas";

        setSize(size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Canvas::Canvas(const Canvas& other) :
        ClickableWidget{other}
    {
        setSize(other.getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Canvas::Canvas(Canvas&& other) :
        ClickableWidget{std::move(other)}
    {
        // sf::RenderTexture does not support move yet
        setSize(other.getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Canvas& Canvas::operator= (const Canvas& right)
    {
        if (this != &right)
        {
            ClickableWidget::operator=(right);
            setSize(right.getSize());
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Canvas& Canvas::operator= (Canvas&& right)
    {
        if (this != &right)
        {
            ClickableWidget::operator=(std::move(right));

            // sf::RenderTexture does not support move yet
            setSize(right.getSize());
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Canvas::Ptr Canvas::create(Layout2d size)
    {
        return std::make_shared<Canvas>(size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Canvas::Ptr Canvas::copy(Canvas::ConstPtr canvas)
    {
        if (canvas)
            return std::static_pointer_cast<Canvas>(canvas->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Canvas::setSize(const Layout2d& size)
    {
        Vector2f newSize = size.getValue();

        if ((newSize.x > 0) && (newSize.y > 0))
        {
            if ((m_renderTexture.getSize().x < static_cast<unsigned int>(newSize.x)) || (m_renderTexture.getSize().y < static_cast<unsigned int>(newSize.y)))
                m_renderTexture.create(static_cast<unsigned int>(newSize.x), static_cast<unsigned int>(newSize.y));

            m_sprite.setTexture(m_renderTexture.getTexture());
            m_sprite.setTextureRect(sf::IntRect{0, 0, static_cast<int>(newSize.x), static_cast<int>(newSize.y)});

            m_renderTexture.clear();
            m_renderTexture.display();
        }

        Widget::setSize(size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Canvas::setView(const sf::View& view)
    {
        m_renderTexture.setView(view);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::View& Canvas::getView() const
    {
        return m_renderTexture.getView();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::View& Canvas::getDefaultView() const
    {
        return m_renderTexture.getDefaultView();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::IntRect Canvas::getViewport() const
    {
        return m_renderTexture.getViewport(m_renderTexture.getView());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Canvas::clear(Color color)
    {
        m_renderTexture.clear(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Canvas::draw(const sf::Drawable& drawable, const sf::RenderStates& states)
    {
        m_renderTexture.draw(drawable, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Canvas::draw(const sf::Vertex* vertices, std::size_t vertexCount, sf::PrimitiveType type, const sf::RenderStates& states)
    {
        m_renderTexture.draw(vertices, vertexCount, type, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Canvas::draw(const tgui::Sprite& sprite, const sf::RenderStates& states)
    {
        sprite.draw(m_renderTexture, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Canvas::display()
    {
        m_renderTexture.display();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Canvas::rendererChanged(const std::string& property)
    {
        Widget::rendererChanged(property);

        if ((property == "opacity") || (property == "opacitydisabled"))
            m_sprite.setColor(Color::calcColorOpacity(Color::White, m_opacityCached));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Canvas::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if ((getSize().x <= 0) || (getSize().y <= 0))
            return;

        states.transform.translate(getPosition());
        target.draw(m_sprite, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Canvas::canGainFocus() const
    {
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
