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


#include <TGUI/ObjectConverter.hpp>
#include <TGUI/Loading/Serializer.hpp>
#include <TGUI/Loading/Deserializer.hpp>
#include <cassert>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::String& ObjectConverter::getString()
    {
        assert(m_type != Type::None);

        if (m_serialized)
            return m_string;

        m_string = Serializer::serialize(ObjectConverter{*this});
        m_serialized = true;
        return m_string;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Font& ObjectConverter::getFont()
    {
        assert(m_type != Type::None);
        assert(m_type == Type::Font || m_type == Type::String);

        if (m_type == Type::String)
        {
            m_value = Font(m_string);
            m_type = Type::Font;
        }

        return m_value.get<Font>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Color& ObjectConverter::getColor()
    {
        assert(m_type != Type::None);
        assert(m_type == Type::Color || m_type == Type::String);

        if (m_type == Type::String)
        {
            m_value = Color(m_string);
            m_type = Type::Color;
        }

        return m_value.get<Color>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ObjectConverter::getBool()
    {
        assert(m_type != Type::None);
        assert(m_type == Type::Bool || m_type == Type::String);

        if (m_type == Type::String)
        {
            m_value = Deserializer::deserialize(ObjectConverter::Type::Bool, m_string).getBool();
            m_type = Type::Bool;
        }

        return m_value.get<bool>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float ObjectConverter::getNumber()
    {
        assert(m_type != Type::None);
        assert(m_type == Type::Number || m_type == Type::String);

        if (m_type == Type::String)
        {
            m_value = Deserializer::deserialize(ObjectConverter::Type::Number, m_string).getNumber();
            m_type = Type::Number;
        }

        return m_value.get<float>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Outline& ObjectConverter::getOutline()
    {
        assert(m_type != Type::None);
        assert(m_type == Type::Outline || m_type == Type::String);

        if (m_type == Type::String)
        {
            m_value = Deserializer::deserialize(ObjectConverter::Type::Outline, m_string).getOutline();
            m_type = Type::Outline;
        }

        return m_value.get<Outline>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture& ObjectConverter::getTexture()
    {
        assert(m_type != Type::None);
        assert(m_type == Type::Texture || m_type == Type::String);

        if (m_type == Type::String)
        {
            m_value = Deserializer::deserialize(ObjectConverter::Type::Texture, m_string).getTexture();
            m_type = Type::Texture;
        }

        return m_value.get<Texture>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const TextStyle& ObjectConverter::getTextStyle()
    {
        assert(m_type != Type::None);
        assert(m_type == Type::TextStyle || m_type == Type::String);

        if (m_type == Type::String)
        {
            m_value = Deserializer::deserialize(ObjectConverter::Type::TextStyle, m_string).getTextStyle();
            m_type = Type::TextStyle;
        }

        return m_value.get<TextStyle>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::shared_ptr<RendererData>& ObjectConverter::getRenderer()
    {
        assert(m_type != Type::None);
        assert(m_type == Type::RendererData || m_type == Type::String);

        if (m_type == Type::String)
        {
            m_value = Deserializer::deserialize(ObjectConverter::Type::RendererData, m_string).getRenderer();
            m_type = Type::RendererData;
        }

        return m_value.get<std::shared_ptr<RendererData>>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ObjectConverter::Type ObjectConverter::getType() const
    {
        return m_type;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ObjectConverter::operator==(const ObjectConverter& right) const
    {
        if (this == &right)
            return true;

        if (m_type != right.m_type)
            return false;

        switch (m_type)
        {
        case Type::None:
            return true;
        case Type::String:
            return m_string == right.m_string;
        case Type::Bool:
            return m_value.get<bool>() == right.m_value.get<bool>();
        case Type::Font:
            return m_value.get<Font>() == right.m_value.get<Font>();
        case Type::Color:
            return m_value.get<Color>() == right.m_value.get<Color>();
        case Type::Number:
            return m_value.get<float>() == right.m_value.get<float>();
        case Type::Outline:
            return m_value.get<Outline>() == right.m_value.get<Outline>();
        case Type::Texture:
            return m_value.get<Texture>() == right.m_value.get<Texture>();
        case Type::TextStyle:
            return m_value.get<TextStyle>() == right.m_value.get<TextStyle>();
        case Type::RendererData:
            return m_value.get<std::shared_ptr<RendererData>>() == right.m_value.get<std::shared_ptr<RendererData>>();
        default: // This case should never occur, but prevents a warning that control reaches end of non-void function
            return false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ObjectConverter::operator!=(const ObjectConverter& right) const
    {
        return !(*this == right);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
