/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2020 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Text.hpp>
#include <TGUI/Global.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if (SFML_VERSION_MAJOR == 2) && (SFML_VERSION_MINOR < 4)
    #pragma message("Text class is being compiled without support for outlines (requires SFML >= 2.4)")
#endif

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Text::getSize() const
    {
        return m_size;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Text::setString(const sf::String& string)
    {
        m_text.setString(string);
        recalculateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::String& Text::getString() const
    {
        return m_text.getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Text::setCharacterSize(unsigned int size)
    {
        m_text.setCharacterSize(size);
        recalculateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Text::getCharacterSize() const
    {
        return m_text.getCharacterSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Text::setColor(Color color)
    {
        m_color = color;

#if SFML_VERSION_MAJOR > 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR >= 4)
        m_text.setFillColor(Color::calcColorOpacity(color, m_opacity));
#else
        m_text.setColor(Color::calcColorOpacity(color, m_opacity));
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Color Text::getColor() const
    {
        return m_color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Text::setOpacity(float opacity)
    {
        m_opacity = opacity;

#if SFML_VERSION_MAJOR > 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR >= 4)
        m_text.setFillColor(Color::calcColorOpacity(m_color, opacity));
        m_text.setOutlineColor(Color::calcColorOpacity(m_outlineColor, opacity));
#else
        m_text.setColor(Color::calcColorOpacity(m_color, opacity));
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getOpacity() const
    {
        return m_opacity;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Text::setFont(Font font)
    {
        m_font = font;

        if (font)
            m_text.setFont(*font.getFont());
        else
        {
            // We can't keep using a pointer to the old font (it might be destroyed), but sf::Text has no function to pass an empty font
            if (m_text.getFont())
            {
                m_text = sf::Text();
                m_text.setString(getString());
                m_text.setCharacterSize(getCharacterSize());
                m_text.setStyle(getStyle());

#if SFML_VERSION_MAJOR > 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR >= 4)
                m_text.setFillColor(Color::calcColorOpacity(getColor(), getOpacity()));
                m_text.setOutlineColor(Color::calcColorOpacity(getOutlineColor(), getOpacity()));
                m_text.setOutlineThickness(getOutlineThickness());
#else
                m_text.setColor(Color::calcColorOpacity(getColor(), getOpacity()));
#endif
            }
        }

        recalculateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Font Text::getFont() const
    {
        return m_font;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Text::setStyle(TextStyle style)
    {
        if (style != m_text.getStyle())
        {
            m_text.setStyle(style);
            recalculateSize();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextStyle Text::getStyle() const
    {
        return m_text.getStyle();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Text::setOutlineColor(Color color)
    {
        m_outlineColor = color;

#if SFML_VERSION_MAJOR > 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR >= 4)
        m_text.setOutlineColor(Color::calcColorOpacity(m_outlineColor, m_opacity));
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Color Text::getOutlineColor() const
    {
        return m_outlineColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Text::setOutlineThickness(float thickness)
    {
#if SFML_VERSION_MAJOR > 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR >= 4)
        m_text.setOutlineThickness(thickness);
#else
        (void)thickness;
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getOutlineThickness() const
    {
#if SFML_VERSION_MAJOR > 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR >= 4)
        return m_text.getOutlineThickness();
#else
        return 0;
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Text::findCharacterPos(std::size_t index) const
    {
        return m_text.findCharacterPos(index);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Text::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform *= getTransform();

        // Round the position to avoid blurry text
        const float* matrix = states.transform.getMatrix();
        states.transform = sf::Transform{matrix[0], matrix[4], std::round(matrix[12]),
                                         matrix[1], matrix[5], std::floor(matrix[13]),
                                         matrix[3], matrix[7], matrix[15]};

        target.draw(m_text, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Text::recalculateSize()
    {
        if (m_font == nullptr)
        {
            m_size = {0, 0};
            return;
        }

        float width = 0;
        float maxWidth = 0;
        unsigned int lines = 1;
        std::uint32_t prevChar = 0;
        const sf::String& string = m_text.getString();
        const bool bold = (m_text.getStyle() & sf::Text::Bold) != 0;
        const unsigned int textSize = m_text.getCharacterSize();
        for (std::size_t i = 0; i < string.getSize(); ++i)
        {
            const float kerning = m_font.getKerning(prevChar, string[i], textSize);
            if (string[i] == '\n')
            {
                maxWidth = std::max(maxWidth, width);
                width = 0;
                lines++;
            }
            else if (string[i] == '\t')
                width += (static_cast<float>(m_font.getGlyph(' ', textSize, bold).advance) * 4) + kerning;
            else
                width += static_cast<float>(m_font.getGlyph(string[i], textSize, bold).advance) + kerning;

            prevChar = string[i];
        }

        const float extraVerticalSpace = Text::calculateExtraVerticalSpace(m_font, m_text.getCharacterSize(), m_text.getStyle());
        const float height = lines * m_font.getLineSpacing(m_text.getCharacterSize()) + extraVerticalSpace;
        m_size = {std::max(maxWidth, width), height};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TGUI_REMOVE_DEPRECATED_CODE
    float Text::getExtraHorizontalPadding(const Text& text)
    {
        return getExtraHorizontalPadding(text.getFont(), text.getCharacterSize(), text.getStyle());
    }
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getExtraHorizontalPadding() const
    {
        return getExtraHorizontalPadding(getFont(), getCharacterSize(), getStyle());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getExtraHorizontalPadding(Font font, unsigned int characterSize, TextStyle textStyle)
    {
        return getLineHeight(font, characterSize, textStyle) / 10.f;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getExtraHorizontalOffset() const
    {
        return getExtraHorizontalOffset(getFont(), getCharacterSize(), getStyle());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getExtraHorizontalOffset(Font font, unsigned int characterSize, TextStyle textStyle)
    {
        return getLineHeight(font, characterSize, textStyle) / 6.f;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getExtraVerticalPadding(unsigned int characterSize)
    {
        return std::max(1.f, std::round(characterSize / 8.f));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TGUI_REMOVE_DEPRECATED_CODE
    float Text::getLineHeight(const Text& text)
    {
        return getLineHeight(text.getFont(), text.getCharacterSize(), text.getStyle());
    }
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getLineHeight() const
    {
        return getLineHeight(getFont(), getCharacterSize(), getStyle());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getLineHeight(Font font, unsigned int characterSize, TextStyle textStyle)
    {
        const float extraVerticalSpace = Text::calculateExtraVerticalSpace(font, characterSize, textStyle);
        return font.getLineSpacing(characterSize) + extraVerticalSpace;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TGUI_REMOVE_DEPRECATED_CODE
    float Text::getLineWidth(const Text& text)
    {
        return getLineWidth(text.getString(), text.getFont(), text.getCharacterSize(), text.getStyle());
    }
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getLineWidth() const
    {
        return getLineWidth(getString(), getFont(), getCharacterSize(), getStyle());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getLineWidth(const sf::String &text, Font font, unsigned int characterSize, TextStyle textStyle)
    {
        if (font == nullptr)
            return 0.0f;

        bool bold = (textStyle & sf::Text::Bold) != 0;

        float width = 0.0f;
        std::uint32_t prevChar = 0;
        for (std::size_t i = 0; i < text.getSize(); ++i)
        {
            float charWidth;
            const std::uint32_t curChar = text[i];
            if (curChar == '\n')
                break;
            else if (curChar == '\t')
                charWidth = font.getGlyph(' ', characterSize, bold).advance * 4.0f;
            else
                charWidth = font.getGlyph(curChar, characterSize, bold).advance;

            const float kerning = font.getKerning(prevChar, curChar, characterSize);

            width = width + charWidth + kerning;
            prevChar = curChar;
        }

        return width;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Text::findBestTextSize(Font font, float height, int fit)
    {
        if (!font)
            return 0;

        if (height < 2)
            return 1;

        std::vector<unsigned int> textSizes(static_cast<std::size_t>(height));
        for (unsigned int i = 0; i < static_cast<unsigned int>(height); ++i)
            textSizes[i] = i + 1;

        const auto high = std::lower_bound(textSizes.begin(), textSizes.end(), height,
                                           [&](unsigned int charSize, float h) { return font.getLineSpacing(charSize) + Text::calculateExtraVerticalSpace(font, charSize) < h; });
        if (high == textSizes.end())
            return static_cast<unsigned int>(height);

        const float highLineSpacing = font.getLineSpacing(*high);
        if (highLineSpacing == height)
            return *high;

        const auto low = high - 1;
        const float lowLineSpacing = font.getLineSpacing(*low);

        if (fit < 0)
            return *low;
        else if (fit > 0)
            return *high;
        else
        {
            if (std::abs(height - lowLineSpacing) < std::abs(height - highLineSpacing))
                return *low;
            else
                return *high;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::calculateExtraVerticalSpace(Font font, unsigned int characterSize, TextStyle style)
    {
        if (font == nullptr)
            return 0;

        const bool bold = (style & sf::Text::Bold) != 0;

        // Calculate the height of the first line (char size = everything above baseline, height + top = part below baseline)
        const float lineHeight = characterSize
                                 + font.getGlyph('g', characterSize, bold).bounds.height
                                 + font.getGlyph('g', characterSize, bold).bounds.top;

        // Get the line spacing sfml returns
        const float lineSpacing = font.getLineSpacing(characterSize);

        // Calculate the offset of the text
        return lineHeight - lineSpacing;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String Text::wordWrap(float maxWidth, const sf::String& text, Font font, unsigned int textSize, bool bold, bool dropLeadingSpace)
    {
        if (font == nullptr)
            return "";

        sf::String result;
        std::size_t index = 0;
        while (index < text.getSize())
        {
            const std::size_t oldIndex = index;

            // Find out how many characters we can get on this line
            float width = 0;
            std::uint32_t prevChar = 0;
            for (std::size_t i = index; i < text.getSize(); ++i)
            {
                float charWidth;
                const std::uint32_t curChar = text[i];
                if (curChar == '\n')
                {
                    index++;
                    break;
                }
                else if (curChar == '\t')
                    charWidth = font.getGlyph(' ', textSize, bold).advance * 4;
                else
                    charWidth = font.getGlyph(curChar, textSize, bold).advance;

                const float kerning = font.getKerning(prevChar, curChar, textSize);
                if ((maxWidth == 0) || (width + charWidth + kerning <= maxWidth))
                {
                    width += kerning + charWidth;
                    index++;
                }
                else
                    break;

                prevChar = curChar;
            }

            // Every line contains at least one character
            if (index == oldIndex)
                index++;

            // Implement the word-wrap by removing the last few characters from the line
            if (text[index-1] != '\n')
            {
                const std::size_t indexWithoutWordWrap = index;
                if ((index < text.getSize()) && (!isWhitespace(text[index])))
                {
                    std::size_t wordWrapCorrection = 0;
                    while ((index > oldIndex) && (!isWhitespace(text[index - 1])))
                    {
                        wordWrapCorrection++;
                        index--;
                    }

                    // The word can't be split but there is no other choice, it does not fit on the line
                    if ((index - oldIndex) <= wordWrapCorrection)
                        index = indexWithoutWordWrap;
                }
            }

            // If the next line starts with just a space, then the space need not be visible
            if (dropLeadingSpace)
            {
                if ((index < text.getSize()) && (text[index] == ' '))
                {
                    if ((index == 0) || (!isWhitespace(text[index-1])))
                    {
                        // But two or more spaces indicate that it is not a normal text and the spaces should not be ignored
                        if (((index + 1 < text.getSize()) && (!isWhitespace(text[index + 1]))) || (index + 1 == text.getSize()))
                            index++;
                    }
                }
            }

            result += text.substring(oldIndex, index - oldIndex);
            if ((index < text.getSize()) && (text[index-1] != '\n'))
                result += "\n";
        }

        return result;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2020 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Renderers/MessageBoxRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_COLOR(MessageBoxRenderer, TextColor, Color::Black)

    TGUI_RENDERER_PROPERTY_RENDERER(MessageBoxRenderer, Button, "button")
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2020 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Container.hpp>
#include <TGUI/Widgets/ComboBox.hpp>
#include <TGUI/Clipping.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <TGUI/SignalImpl.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox::ComboBox()
    {
        m_type = "ComboBox";
        m_draggableWidget = true;
        m_text.setFont(m_fontCached);
        m_defaultText.setFont(m_fontCached);

        initListBox();

        m_renderer = aurora::makeCopied<ComboBoxRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setTextSize(getGlobalTextSize());
        setSize({m_text.getLineHeight() * 10,
                 m_text.getLineHeight() * 1.25f + m_paddingCached.getTop() + m_paddingCached.getBottom() + m_bordersCached.getTop() + m_bordersCached.getBottom()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox::ComboBox(const ComboBox& other) :
        Widget                           {other},
        onItemSelect                     {other.onItemSelect},
        m_nrOfItemsToDisplay             {other.m_nrOfItemsToDisplay},
        m_listBox                        {ListBox::copy(other.m_listBox)},
        m_text                           {other.m_text},
        m_defaultText                    {other.m_defaultText},
        m_changeItemOnScroll             {other.m_changeItemOnScroll},
        m_expandDirection                {other.m_expandDirection},
        m_spriteBackground               {other.m_spriteBackground},
        m_spriteArrow                    {other.m_spriteArrow},
        m_spriteArrowHover               {other.m_spriteArrowHover},
        m_bordersCached                  {other.m_bordersCached},
        m_paddingCached                  {other.m_paddingCached},
        m_borderColorCached              {other.m_borderColorCached},
        m_backgroundColorCached          {other.m_backgroundColorCached},
        m_arrowColorCached               {other.m_arrowColorCached},
        m_arrowColorHoverCached          {other.m_arrowColorHoverCached},
        m_arrowBackgroundColorCached     {other.m_arrowBackgroundColorCached},
        m_arrowBackgroundColorHoverCached{other.m_arrowBackgroundColorHoverCached}
    {
        initListBox();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox::ComboBox(ComboBox&& other) :
        Widget                           {std::move(other)},
        onItemSelect                     {std::move(other.onItemSelect)},
        m_nrOfItemsToDisplay             {std::move(other.m_nrOfItemsToDisplay)},
        m_listBox                        {std::move(other.m_listBox)},
        m_text                           {std::move(other.m_text)},
        m_defaultText                    {std::move(other.m_defaultText)},
        m_changeItemOnScroll             {std::move(other.m_changeItemOnScroll)},
        m_expandDirection                {std::move(other.m_expandDirection)},
        m_spriteBackground               {std::move(other.m_spriteBackground)},
        m_spriteArrow                    {std::move(other.m_spriteArrow)},
        m_spriteArrowHover               {std::move(other.m_spriteArrowHover)},
        m_bordersCached                  {std::move(other.m_bordersCached)},
        m_paddingCached                  {std::move(other.m_paddingCached)},
        m_borderColorCached              {std::move(other.m_borderColorCached)},
        m_backgroundColorCached          {std::move(other.m_backgroundColorCached)},
        m_arrowColorCached               {std::move(other.m_arrowColorCached)},
        m_arrowColorHoverCached          {std::move(other.m_arrowColorHoverCached)},
        m_arrowBackgroundColorCached     {std::move(other.m_arrowBackgroundColorCached)},
        m_arrowBackgroundColorHoverCached{std::move(other.m_arrowBackgroundColorHoverCached)}
    {
        initListBox();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox& ComboBox::operator= (const ComboBox& other)
    {
        if (this != &other)
        {
            ComboBox temp{other};
            Widget::operator=(other);

            std::swap(onItemSelect,                      temp.onItemSelect);
            std::swap(m_nrOfItemsToDisplay,              temp.m_nrOfItemsToDisplay);
            std::swap(m_listBox,                         temp.m_listBox);
            std::swap(m_text,                            temp.m_text);
            std::swap(m_defaultText,                     temp.m_defaultText);
            std::swap(m_changeItemOnScroll,              temp.m_changeItemOnScroll);
            std::swap(m_expandDirection,                 temp.m_expandDirection);
            std::swap(m_spriteBackground,                temp.m_spriteBackground);
            std::swap(m_spriteArrow,                     temp.m_spriteArrow);
            std::swap(m_spriteArrowHover,                temp.m_spriteArrowHover);
            std::swap(m_bordersCached,                   temp.m_bordersCached);
            std::swap(m_paddingCached,                   temp.m_paddingCached);
            std::swap(m_borderColorCached,               temp.m_borderColorCached);
            std::swap(m_backgroundColorCached,           temp.m_backgroundColorCached);
            std::swap(m_arrowColorCached,                temp.m_arrowColorCached);
            std::swap(m_arrowColorHoverCached,           temp.m_arrowColorHoverCached);
            std::swap(m_arrowBackgroundColorCached,      temp.m_arrowBackgroundColorCached);
            std::swap(m_arrowBackgroundColorHoverCached, temp.m_arrowBackgroundColorHoverCached);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox& ComboBox::operator= (ComboBox&& other)
    {
        if (this != &other)
        {
            Widget::operator=(std::move(other));
            onItemSelect                      = std::move(other.onItemSelect);
            m_nrOfItemsToDisplay              = std::move(other.m_nrOfItemsToDisplay);
            m_listBox                         = std::move(other.m_listBox);
            m_text                            = std::move(other.m_text);
            m_defaultText                     = std::move(other.m_defaultText);
            m_changeItemOnScroll              = std::move(other.m_changeItemOnScroll);
            m_expandDirection                 = std::move(other.m_expandDirection);
            m_spriteBackground                = std::move(other.m_spriteBackground);
            m_spriteArrow                     = std::move(other.m_spriteArrow);
            m_spriteArrowHover                = std::move(other.m_spriteArrowHover);
            m_bordersCached                   = std::move(other.m_bordersCached);
            m_paddingCached                   = std::move(other.m_paddingCached);
            m_borderColorCached               = std::move(other.m_borderColorCached);
            m_backgroundColorCached           = std::move(other.m_backgroundColorCached);
            m_arrowColorCached                = std::move(other.m_arrowColorCached);
            m_arrowColorHoverCached           = std::move(other.m_arrowColorHoverCached);
            m_arrowBackgroundColorCached      = std::move(other.m_arrowBackgroundColorCached);
            m_arrowBackgroundColorHoverCached = std::move(other.m_arrowBackgroundColorHoverCached);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox::Ptr ComboBox::create()
    {
        return std::make_shared<ComboBox>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox::Ptr ComboBox::copy(ComboBox::ConstPtr comboBox)
    {
        if (comboBox)
            return std::static_pointer_cast<ComboBox>(comboBox->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBoxRenderer* ComboBox::getSharedRenderer()
    {
        return aurora::downcast<ComboBoxRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ComboBoxRenderer* ComboBox::getSharedRenderer() const
    {
        return aurora::downcast<const ComboBoxRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBoxRenderer* ComboBox::getRenderer()
    {
        return aurora::downcast<ComboBoxRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ComboBoxRenderer* ComboBox::getRenderer() const
    {
        return aurora::downcast<const ComboBoxRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_bordersCached.updateParentSize(getSize());
        m_paddingCached.updateParentSize(getSize());

        m_spriteBackground.setSize(getInnerSize());
        m_spriteBackgroundDisabled.setSize(getInnerSize());

        const float height = getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom();

        if (height > 0)
        {
            m_listBox->setItemHeight(static_cast<unsigned int>(height));
            updateListBoxHeight();
        }

        if (m_spriteArrow.isSet())
        {
            m_spriteArrow.setSize({m_spriteArrow.getTexture().getImageSize().x * (height / m_spriteArrow.getTexture().getImageSize().y), height});
            m_spriteArrowHover.setSize(m_spriteArrow.getSize());
            m_spriteArrowDisabled.setSize(m_spriteArrow.getSize());
        }

        m_text.setCharacterSize(m_listBox->getTextSize());
        m_defaultText.setCharacterSize(m_listBox->getTextSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setEnabled(bool enabled)
    {
        Widget::setEnabled(enabled);
        if (m_enabled || !m_textColorDisabledCached.isSet())
            m_text.setColor(m_textColorCached);
        else
            m_text.setColor(m_textColorDisabledCached);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setItemsToDisplay(std::size_t nrOfItemsInList)
    {
        m_nrOfItemsToDisplay = nrOfItemsInList;

        if (m_nrOfItemsToDisplay < m_listBox->getItemCount())
            updateListBoxHeight();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t ComboBox::getItemCount() const
    {
        return m_listBox->getItemCount();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t ComboBox::getItemsToDisplay() const
    {
        return m_nrOfItemsToDisplay;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::addItem(const sf::String& item, const sf::String& id)
    {
        const bool ret = m_listBox->addItem(item, id);
        updateListBoxHeight();
        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::setSelectedItem(const sf::String& itemName)
    {
        const int previousSelectedItemIndex = m_listBox->getSelectedItemIndex();

        const bool ret = m_listBox->setSelectedItem(itemName);
        m_text.setString(m_listBox->getSelectedItem());

        if (previousSelectedItemIndex != m_listBox->getSelectedItemIndex())
            onItemSelect.emit(this, m_listBox->getSelectedItem(), m_listBox->getSelectedItemId(), m_listBox->getSelectedItemIndex());

        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::setSelectedItemById(const sf::String& id)
    {
        const int previousSelectedItemIndex = m_listBox->getSelectedItemIndex();

        const bool ret = m_listBox->setSelectedItemById(id);
        m_text.setString(m_listBox->getSelectedItem());

        if (previousSelectedItemIndex != m_listBox->getSelectedItemIndex())
            onItemSelect.emit(this, m_listBox->getSelectedItem(), m_listBox->getSelectedItemId(), m_listBox->getSelectedItemIndex());

        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::setSelectedItemByIndex(std::size_t index)
    {
        const int previousSelectedItemIndex = m_listBox->getSelectedItemIndex();

        const bool ret = m_listBox->setSelectedItemByIndex(index);
        m_text.setString(m_listBox->getSelectedItem());

        if (previousSelectedItemIndex != m_listBox->getSelectedItemIndex())
            onItemSelect.emit(this, m_listBox->getSelectedItem(), m_listBox->getSelectedItemId(), m_listBox->getSelectedItemIndex());

        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::deselectItem()
    {
        m_text.setString("");
        m_listBox->deselectItem();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::removeItem(const sf::String& itemName)
    {
        // Implemented like removeItemByIndex instead of like removeItemById because m_listBox->getItems() may be slow

        const bool ret = m_listBox->removeItem(itemName);

        m_text.setString(m_listBox->getSelectedItem());

        // Shrink the list size
        if ((m_nrOfItemsToDisplay == 0) || (m_listBox->getItemCount() < m_nrOfItemsToDisplay))
            updateListBoxHeight();

        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::removeItemById(const sf::String& id)
    {
        const auto& ids = m_listBox->getItemIds();
        for (std::size_t i = 0; i < ids.size(); ++i)
        {
            if (ids[i] == id)
                return removeItemByIndex(i);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::removeItemByIndex(std::size_t index)
    {
        const bool ret = m_listBox->removeItemByIndex(index);

        m_text.setString(m_listBox->getSelectedItem());

        // Shrink the list size
        if ((m_nrOfItemsToDisplay == 0) || (m_listBox->getItemCount() < m_nrOfItemsToDisplay))
            updateListBoxHeight();

        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::removeAllItems()
    {
        m_text.setString("");
        m_listBox->removeAllItems();

        updateListBoxHeight();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String ComboBox::getItemById(const sf::String& id) const
    {
        return m_listBox->getItemById(id);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String ComboBox::getSelectedItem() const
    {
        return m_listBox->getSelectedItem();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String ComboBox::getSelectedItemId() const
    {
        return m_listBox->getSelectedItemId();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int ComboBox::getSelectedItemIndex() const
    {
        return m_listBox->getSelectedItemIndex();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::changeItem(const sf::String& originalValue, const sf::String& newValue)
    {
        const bool ret = m_listBox->changeItem(originalValue, newValue);
        m_text.setString(m_listBox->getSelectedItem());
        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::changeItemById(const sf::String& id, const sf::String& newValue)
    {
        const bool ret = m_listBox->changeItemById(id, newValue);
        m_text.setString(m_listBox->getSelectedItem());
        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::changeItemByIndex(std::size_t index, const sf::String& newValue)
    {
        const bool ret = m_listBox->changeItemByIndex(index, newValue);
        m_text.setString(m_listBox->getSelectedItem());
        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<sf::String> ComboBox::getItems() const
    {
        return m_listBox->getItems();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::vector<sf::String>& ComboBox::getItemIds() const
    {
        return m_listBox->getItemIds();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setMaximumItems(std::size_t maximumItems)
    {
        m_listBox->setMaximumItems(maximumItems);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t ComboBox::getMaximumItems() const
    {
        return m_listBox->getMaximumItems();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setTextSize(unsigned int textSize)
    {
        m_listBox->setTextSize(textSize);
        m_text.setCharacterSize(m_listBox->getTextSize());
        m_defaultText.setCharacterSize(m_listBox->getTextSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ComboBox::getTextSize() const
    {
        return m_listBox->getTextSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setDefaultText(const sf::String& defaultText)
    {
        m_defaultText.setString(defaultText);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::String& ComboBox::getDefaultText() const
    {
        return m_defaultText.getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setExpandDirection(ExpandDirection direction)
    {
        m_expandDirection = direction;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox::ExpandDirection ComboBox::getExpandDirection() const
    {
        return m_expandDirection;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::contains(const sf::String& item) const
    {
        const auto& items = getItems();
        return std::find(items.begin(), items.end(), item) != items.end();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::containsId(const sf::String& id) const
    {
        const auto& ids = getItemIds();
        return std::find(ids.begin(), ids.end(), id) != ids.end();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setChangeItemOnScroll(bool changeOnScroll)
    {
        m_changeItemOnScroll = changeOnScroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::getChangeItemOnScroll() const
    {
        return m_changeItemOnScroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setParent(Container* parent)
    {
        hideListBox();
        Widget::setParent(parent);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::mouseOnWidget(Vector2f pos) const
    {
        if (FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(pos))
        {
            if (!m_transparentTextureCached || !m_spriteBackground.isTransparentPixel(pos - getPosition() - m_bordersCached.getOffset()))
                return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::leftMousePressed(Vector2f)
    {
        m_mouseDown = true;

        // If the list wasn't visible then open it
        if (!m_listBox->isVisible())
        {
            // Show the list
            showListBox();

            // Reselect the selected item to make sure it is always among the visible items when the list opens
            if (m_listBox->getSelectedItemIndex() >= 0)
                m_listBox->setSelectedItemByIndex(m_listBox->getSelectedItemIndex());
        }
        else // This list was already open, so close it now
            hideListBox();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::mouseWheelScrolled(float delta, Vector2f)
    {
        // Don't do anything when changing item on scroll is disabled
        if (!m_changeItemOnScroll)
            return false;

        // Only act to scrolling when the list is not being shown
        if (m_listBox->isVisible())
            return false;

        // Check if you are scrolling down
        if (delta < 0)
        {
            // Select the next item
            if (static_cast<std::size_t>(m_listBox->getSelectedItemIndex() + 1) < m_listBox->getItemCount())
            {
                m_listBox->setSelectedItemByIndex(static_cast<std::size_t>(m_listBox->getSelectedItemIndex() + 1));
                m_text.setString(m_listBox->getSelectedItem());
                onItemSelect.emit(this, m_listBox->getSelectedItem(), m_listBox->getSelectedItemId(), m_listBox->getSelectedItemIndex());
            }
        }
        else // You are scrolling up
        {
            // Select the previous item
            if (m_listBox->getSelectedItemIndex() > 0)
            {
                m_listBox->setSelectedItemByIndex(static_cast<std::size_t>(m_listBox->getSelectedItemIndex() - 1));
                m_text.setString(m_listBox->getSelectedItem());
                onItemSelect.emit(this, m_listBox->getSelectedItem(), m_listBox->getSelectedItemId(), m_listBox->getSelectedItemIndex());
            }
        }

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& ComboBox::getSignal(std::string signalName)
    {
        if (signalName == toLower(onItemSelect.getName()))
            return onItemSelect;
        else
            return Widget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            setSize(m_size);
        }
        else if (property == "padding")
        {
            m_paddingCached = getSharedRenderer()->getPadding();
            setSize(m_size);
        }
        else if (property == "textcolor")
        {
            m_textColorCached = getSharedRenderer()->getTextColor();
            if (m_enabled || !m_textColorDisabledCached.isSet())
                m_text.setColor(m_textColorCached);
            if (!getSharedRenderer()->getDefaultTextColor().isSet())
                m_defaultText.setColor(getSharedRenderer()->getTextColor());
        }
        else if (property == "textcolordisabled")
        {
            m_textColorDisabledCached = getSharedRenderer()->getTextColorDisabled();
            if (!m_enabled && m_textColorDisabledCached.isSet())
                m_text.setColor(m_textColorDisabledCached);
        }
        else if (property == "textstyle")
        {
            m_text.setStyle(getSharedRenderer()->getTextStyle());
            if (!getSharedRenderer()->getDefaultTextStyle().isSet())
                m_defaultText.setStyle(getSharedRenderer()->getTextStyle());
        }
        else if (property == "defaulttextcolor")
        {
            if (getSharedRenderer()->getDefaultTextColor().isSet())
                m_defaultText.setColor(getSharedRenderer()->getDefaultTextColor());
            else
                m_defaultText.setColor(getSharedRenderer()->getTextColor());
        }
        else if (property == "defaulttextstyle")
        {
            if (getSharedRenderer()->getDefaultTextStyle().isSet())
                m_defaultText.setStyle(getSharedRenderer()->getDefaultTextStyle());
            else
                m_defaultText.setStyle(getSharedRenderer()->getTextStyle());
        }
        else if (property == "texturebackground")
        {
            m_spriteBackground.setTexture(getSharedRenderer()->getTextureBackground());
        }
        else if (property == "texturebackgrounddisabled")
        {
            m_spriteBackgroundDisabled.setTexture(getSharedRenderer()->getTextureBackgroundDisabled());
        }
        else if (property == "texturearrow")
        {
            m_spriteArrow.setTexture(getSharedRenderer()->getTextureArrow());
            setSize(m_size);
        }
        else if (property == "texturearrowhover")
        {
            m_spriteArrowHover.setTexture(getSharedRenderer()->getTextureArrowHover());
        }
        else if (property == "texturearrowdisabled")
        {
            m_spriteArrowDisabled.setTexture(getSharedRenderer()->getTextureArrowDisabled());
        }
        else if (property == "listbox")
        {
            m_listBox->setRenderer(getSharedRenderer()->getListBox());
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == "backgroundcolordisabled")
        {
            m_backgroundColorDisabledCached = getSharedRenderer()->getBackgroundColorDisabled();
        }
        else if (property == "arrowbackgroundcolor")
        {
            m_arrowBackgroundColorCached = getSharedRenderer()->getArrowBackgroundColor();
        }
        else if (property == "arrowbackgroundcolorhover")
        {
            m_arrowBackgroundColorHoverCached = getSharedRenderer()->getArrowBackgroundColorHover();
        }
        else if (property == "arrowbackgroundcolordisabled")
        {
            m_arrowBackgroundColorDisabledCached = getSharedRenderer()->getArrowBackgroundColorDisabled();
        }
        else if (property == "arrowcolor")
        {
            m_arrowColorCached = getSharedRenderer()->getArrowColor();
        }
        else if (property == "arrowcolorhover")
        {
            m_arrowColorHoverCached = getSharedRenderer()->getArrowColorHover();
        }
        else if (property == "arrowcolordisabled")
        {
            m_arrowColorDisabledCached = getSharedRenderer()->getArrowColorDisabled();
        }
        else if ((property == "opacity") || (property == "opacitydisabled"))
        {
            Widget::rendererChanged(property);

            m_spriteBackground.setOpacity(m_opacityCached);
            m_spriteBackgroundDisabled.setOpacity(m_opacityCached);
            m_spriteArrow.setOpacity(m_opacityCached);
            m_spriteArrowHover.setOpacity(m_opacityCached);
            m_spriteArrowDisabled.setOpacity(m_opacityCached);

            m_text.setOpacity(m_opacityCached);
            m_defaultText.setOpacity(m_opacityCached);
        }
        else if (property == "font")
        {
            Widget::rendererChanged(property);

            m_text.setFont(m_fontCached);
            m_defaultText.setFont(m_fontCached);
            m_listBox->setInheritedFont(m_fontCached);

            setSize(m_size);
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> ComboBox::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        if (getItemCount() > 0)
        {
            const auto& items = getItems();
            const auto& ids = getItemIds();

            bool itemIdsUsed = false;
            std::string itemList = "[" + Serializer::serialize(items[0]);
            std::string itemIdList = "[" + Serializer::serialize(ids[0]);
            for (std::size_t i = 1; i < items.size(); ++i)
            {
                itemList += ", " + Serializer::serialize(items[i]);
                itemIdList += ", " + Serializer::serialize(ids[i]);

                if (!ids[i].isEmpty())
                    itemIdsUsed = true;
            }
            itemList += "]";
            itemIdList += "]";

            node->propertyValuePairs["Items"] = std::make_unique<DataIO::ValueNode>(itemList);
            if (itemIdsUsed)
                node->propertyValuePairs["ItemIds"] = std::make_unique<DataIO::ValueNode>(itemIdList);
        }

        node->propertyValuePairs["ItemsToDisplay"] = std::make_unique<DataIO::ValueNode>(to_string(getItemsToDisplay()));
        node->propertyValuePairs["TextSize"] = std::make_unique<DataIO::ValueNode>(to_string(getTextSize()));
        node->propertyValuePairs["MaximumItems"] = std::make_unique<DataIO::ValueNode>(to_string(getMaximumItems()));
        node->propertyValuePairs["ChangeItemOnScroll"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_changeItemOnScroll));

        if (getExpandDirection() == ComboBox::ExpandDirection::Down)
            node->propertyValuePairs["ExpandDirection"] = std::make_unique<DataIO::ValueNode>("Down");
        else if (getExpandDirection() == ComboBox::ExpandDirection::Up)
            node->propertyValuePairs["ExpandDirection"] = std::make_unique<DataIO::ValueNode>("Up");
#ifndef TGUI_NEXT
        else if (getExpandDirection() == ComboBox::ExpandDirection::Automatic)
            node->propertyValuePairs["ExpandDirection"] = std::make_unique<DataIO::ValueNode>("Automatic");
#endif
        if (m_listBox->getSelectedItemIndex() >= 0)
            node->propertyValuePairs["SelectedItemIndex"] = std::make_unique<DataIO::ValueNode>(to_string(m_listBox->getSelectedItemIndex()));

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs["items"])
        {
            if (!node->propertyValuePairs["items"]->listNode)
                throw Exception{"Failed to parse 'Items' property, expected a list as value"};

            if (node->propertyValuePairs["itemids"])
            {
                if (!node->propertyValuePairs["itemids"]->listNode)
                    throw Exception{"Failed to parse 'ItemIds' property, expected a list as value"};

                if (node->propertyValuePairs["items"]->valueList.size() != node->propertyValuePairs["itemids"]->valueList.size())
                    throw Exception{"Amounts of values for 'Items' differs from the amount in 'ItemIds'"};

                for (std::size_t i = 0; i < node->propertyValuePairs["items"]->valueList.size(); ++i)
                {
                    addItem(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["items"]->valueList[i]).getString(),
                            Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["itemids"]->valueList[i]).getString());
                }
            }
            else // There are no item ids
            {
                for (const auto& item : node->propertyValuePairs["items"]->valueList)
                    addItem(Deserializer::deserialize(ObjectConverter::Type::String, item).getString());
            }
        }
        else // If there are no items, there should be no item ids
        {
            if (node->propertyValuePairs["itemids"])
                throw Exception{"Found 'ItemIds' property while there is no 'Items' property"};
        }

        if (node->propertyValuePairs["itemstodisplay"])
            setItemsToDisplay(strToInt(node->propertyValuePairs["itemstodisplay"]->value));
        if (node->propertyValuePairs["textsize"])
            setTextSize(strToInt(node->propertyValuePairs["textsize"]->value));
        if (node->propertyValuePairs["maximumitems"])
            setMaximumItems(strToInt(node->propertyValuePairs["maximumitems"]->value));
        if (node->propertyValuePairs["selecteditemindex"])
            setSelectedItemByIndex(strToInt(node->propertyValuePairs["selecteditemindex"]->value));
        if (node->propertyValuePairs["changeitemonscroll"])
            m_changeItemOnScroll = Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["changeitemonscroll"]->value).getBool();

        if (node->propertyValuePairs["expanddirection"])
        {
            if (toLower(node->propertyValuePairs["expanddirection"]->value) == "up")
                setExpandDirection(ComboBox::ExpandDirection::Up);
            else if (toLower(node->propertyValuePairs["expanddirection"]->value) == "down")
                setExpandDirection(ComboBox::ExpandDirection::Down);
            else if (toLower(node->propertyValuePairs["expanddirection"]->value) == "automatic")
                setExpandDirection(ComboBox::ExpandDirection::Automatic);
            else
                throw Exception{"Failed to parse ExpandDirection property. Only the values Up, Down and Automatic are correct."};
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ComboBox::getInnerSize() const
    {
        return {std::max(0.f, getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight()),
                std::max(0.f, getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom())};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::updateListBoxHeight()
    {
        const Borders borders = m_listBox->getSharedRenderer()->getBorders();
        const Padding padding = m_listBox->getSharedRenderer()->getPadding();

        if (m_nrOfItemsToDisplay > 0)
            m_listBox->setSize({getSize().x, (m_listBox->getItemHeight() * (std::min<std::size_t>(m_nrOfItemsToDisplay, std::max<std::size_t>(m_listBox->getItemCount(), 1))))
                                             + borders.getTop() + borders.getBottom() + padding.getTop() + padding.getBottom()});
        else
            m_listBox->setSize({getSize().x, (m_listBox->getItemHeight() * std::max<std::size_t>(m_listBox->getItemCount(), 1))
                                             + borders.getTop() + borders.getBottom() + padding.getTop() + padding.getBottom()});

    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::showListBox()
    {
        if (m_listBox->isVisible() || !getParent())
            return;

        m_previousSelectedItemIndex = m_listBox->getSelectedItemIndex();

        m_listBox->setVisible(true);

        // Find the GuiContainer that contains the combo box
        Container* container = getParent();
        while (container->getParent() != nullptr)
            container = container->getParent();

        ExpandDirection direction = m_expandDirection;
        if (direction == ExpandDirection::Automatic)
        {
            if ((getAbsolutePosition().y + getSize().y + m_listBox->getSize().y - m_bordersCached.getBottom() > container->getSize().y)
             && (getAbsolutePosition().y - m_listBox->getSize().y + m_bordersCached.getTop() > 0))
                direction = ExpandDirection::Up;
            else
                direction = ExpandDirection::Down;
        }

        if (direction == ExpandDirection::Down)
            m_listBox->setPosition({getAbsolutePosition().x, getAbsolutePosition().y + getSize().y - m_bordersCached.getBottom()});
        else if (direction == ExpandDirection::Up)
            m_listBox->setPosition({getAbsolutePosition().x, getAbsolutePosition().y - m_listBox->getSize().y + m_bordersCached.getTop()});

        container->add(m_listBox, "#TGUI_INTERNAL$ComboBoxListBox#");
        m_listBox->setFocused(true);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::hideListBox()
    {
        if (!m_listBox->isVisible())
            return;

        m_listBox->setVisible(false);
        m_listBox->mouseNoLongerOnWidget();

        // Find the GuiContainer in order to remove the ListBox from it
        Widget* container = this;
        while (container->getParent() != nullptr)
            container = container->getParent();

        if (container != this)
            static_cast<Container*>(container)->remove(m_listBox);

        const int selectedItemIndex = m_listBox->getSelectedItemIndex();
        if (selectedItemIndex != m_previousSelectedItemIndex)
        {
            m_text.setString(m_listBox->getSelectedItem());
            onItemSelect.emit(this, m_listBox->getSelectedItem(), m_listBox->getSelectedItemId(), m_listBox->getSelectedItemIndex());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::initListBox()
    {
        m_listBox->setVisible(false);

        m_listBox->connect("Unfocused", [this](){
                                                    if (!m_mouseHover)
                                                        hideListBox();
                                                });

        m_listBox->connect("MouseReleased", [this](){ hideListBox(); });
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            drawBorders(target, states, m_bordersCached, getSize(), m_borderColorCached);
            states.transform.translate(m_bordersCached.getOffset());
        }

        sf::RenderStates statesForText = states;

        // Draw the background
        if (m_spriteBackground.isSet())
        {
            if (!m_enabled && m_spriteBackgroundDisabled.isSet())
                m_spriteBackgroundDisabled.draw(target, states);
            else
                m_spriteBackground.draw(target, states);
        }
        else
        {
            if (!m_enabled && m_backgroundColorDisabledCached.isSet())
                drawRectangleShape(target, states, getInnerSize(), m_backgroundColorDisabledCached);
            else
                drawRectangleShape(target, states, getInnerSize(), m_backgroundColorCached);
        }

        // Check if we have a texture for the arrow
        float arrowSize;
        if (m_spriteArrow.isSet())
        {
            arrowSize = m_spriteArrow.getSize().x;
            states.transform.translate({getInnerSize().x - m_paddingCached.getRight() - arrowSize, m_paddingCached.getTop()});

            if (!m_enabled && m_spriteArrowDisabled.isSet())
                m_spriteArrowDisabled.draw(target, states);
            else if (m_mouseHover && m_spriteArrowHover.isSet())
                m_spriteArrowHover.draw(target, states);
            else
                m_spriteArrow.draw(target, states);
        }
        else // There are no textures for the arrow
        {
            arrowSize = getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom();
            states.transform.translate({getInnerSize().x - m_paddingCached.getRight() - arrowSize, m_paddingCached.getTop()});

            if (!m_enabled && m_arrowBackgroundColorDisabledCached.isSet())
                drawRectangleShape(target, states, {arrowSize, arrowSize}, m_arrowBackgroundColorDisabledCached);
            else if (m_mouseHover && m_arrowBackgroundColorHoverCached.isSet())
                drawRectangleShape(target, states, {arrowSize, arrowSize}, m_arrowBackgroundColorHoverCached);
            else
                drawRectangleShape(target, states, {arrowSize, arrowSize}, m_arrowBackgroundColorCached);

            sf::ConvexShape arrow{3};
            arrow.setPoint(0, {arrowSize / 5, arrowSize / 5});
            arrow.setPoint(1, {arrowSize / 2, arrowSize * 4/5});
            arrow.setPoint(2, {arrowSize * 4/5, arrowSize / 5});

            if (!m_enabled && m_arrowColorDisabledCached.isSet())
                arrow.setFillColor(m_arrowColorDisabledCached);
            else if (m_mouseHover && m_arrowColorHoverCached.isSet())
                arrow.setFillColor(m_arrowColorHoverCached);
            else
                arrow.setFillColor(m_arrowColorCached);

            target.draw(arrow, states);
        }

        // Draw the selected item
        const int selectedItemIndex = getSelectedItemIndex();
        if (((selectedItemIndex >= 0) && !m_text.getString().isEmpty()) || ((selectedItemIndex == -1) && !m_defaultText.getString().isEmpty()))
        {
            const Clipping clipping{target, statesForText, {m_paddingCached.getLeft(), m_paddingCached.getTop()}, {getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight() - arrowSize, getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}};

            statesForText.transform.translate(m_paddingCached.getLeft() + m_text.getExtraHorizontalPadding(),
                                              m_paddingCached.getTop() + (((getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()) - m_text.getSize().y) / 2.0f));

            if (selectedItemIndex >= 0)
                m_text.draw(target, statesForText);
            else
                m_defaultText.draw(target, statesForText);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2020 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Widgets/TreeView.hpp>
#include <TGUI/Keyboard.hpp>
#include <TGUI/Clipping.hpp>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace
    {
        void setTextSizeImpl(std::vector<std::shared_ptr<TreeView::Node>>& nodes, unsigned int textSize)
        {
            for (auto& node : nodes)
            {
                node->text.setCharacterSize(textSize);
                if (!node->nodes.empty())
                    setTextSizeImpl(node->nodes, textSize);
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void setTextOpacityImpl(std::vector<std::shared_ptr<TreeView::Node>>& nodes, float opacity)
        {
            for (auto& node : nodes)
            {
                node->text.setOpacity(opacity);
                if (!node->nodes.empty())
                    setTextOpacityImpl(node->nodes, opacity);
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void setTextFontImpl(std::vector<std::shared_ptr<TreeView::Node>>& nodes, const Font& font)
        {
            for (auto& node : nodes)
            {
                node->text.setFont(font);
                if (!node->nodes.empty())
                    setTextFontImpl(node->nodes, font);
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::shared_ptr<TreeView::Node> cloneNode(const std::shared_ptr<TreeView::Node>& oldNode, TreeView::Node* parent)
        {
            auto newNode = std::make_shared<TreeView::Node>();
            newNode->text = oldNode->text;
            newNode->depth = oldNode->depth;
            newNode->expanded = oldNode->expanded;
            newNode->parent = parent;

            for (const auto& oldChild : oldNode->nodes)
                newNode->nodes.push_back(cloneNode(oldChild, newNode.get()));

            return newNode;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void cloneVisibleNodeList(const std::vector<std::shared_ptr<TreeView::Node>>& oldNodes,
                                  std::vector<std::shared_ptr<TreeView::Node>>& newNodes,
                                  const std::vector<std::shared_ptr<TreeView::Node>>& oldList,
                                  std::vector<std::shared_ptr<TreeView::Node>>& newList)
        {
            assert(oldNodes.size() == newNodes.size());
            assert(oldList.size() == newList.size());

            for (unsigned int i = 0; i < oldNodes.size(); ++i)
            {
                for (unsigned int j = 0; j < oldList.size(); ++j)
                {
                    if (oldNodes[i] == oldList[j])
                        newList[j] = newNodes[i];
                }

                cloneVisibleNodeList(oldNodes[i]->nodes, newNodes[i]->nodes, oldList, newList);
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void expandOrCollapseAll(std::vector<std::shared_ptr<TreeView::Node>>& nodes, bool expandNode)
        {
            for (auto& node : nodes)
            {
                if (!node->nodes.empty())
                {
                    node->expanded = expandNode;
                    expandOrCollapseAll(node->nodes, expandNode);
                }
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool removeItemImpl(const std::vector<sf::String>& hierarchy, bool removeParentsWhenEmpty, unsigned int parentIndex, std::vector<std::shared_ptr<TreeView::Node>>& nodes)
        {
            for (auto it = nodes.begin(); it != nodes.end(); ++it)
            {
                if ((*it)->text.getString() != hierarchy[parentIndex])
                    continue;

                if (parentIndex + 1 == hierarchy.size())
                {
                    nodes.erase(it);
                    return true;
                }
                else
                {
                    // Return false if some menu in the hierarchy couldn't be found
                    if (!removeItemImpl(hierarchy, removeParentsWhenEmpty, parentIndex + 1, (*it)->nodes))
                        return false;

                    // If parents don't have to be removed as well then we are done
                    if (!removeParentsWhenEmpty)
                        return true;

                    // Also delete the parent if empty
                    if ((*it)->nodes.empty())
                        nodes.erase(it);

                    return true;
                }
            }

            // The hierarchy doesn't exist
            return false;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::vector<TreeView::ConstNode> convertNodesToConstNodes(const std::vector<std::shared_ptr<TreeView::Node>>& nodes)
        {
            std::vector<TreeView::ConstNode> constNodes;
            for (const auto& node : nodes)
            {
                TreeView::ConstNode constNode;
                constNode.expanded = node->expanded;
                constNode.text = node->text.getString();
                constNode.nodes = convertNodesToConstNodes(node->nodes);
                constNodes.push_back(std::move(constNode));
            }
            return constNodes;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        TreeView::Node* findNode(const std::vector<std::shared_ptr<TreeView::Node>>& nodes, const std::vector<sf::String>& hierarchy, unsigned int parentIndex)
        {
            for (auto& node : nodes)
            {
                if (node->text.getString() != hierarchy[parentIndex])
                    continue;
                else if (parentIndex + 1 == hierarchy.size())
                    return node.get();
                else
                    return findNode(node->nodes, hierarchy, parentIndex + 1);
            }

            return nullptr;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void saveItems(std::unique_ptr<DataIO::Node>& parentNode, const std::vector<std::shared_ptr<TreeView::Node>>& items)
        {
            for (const auto& item : items)
            {
                auto itemNode = std::make_unique<DataIO::Node>();
                itemNode->name = "Item";

                itemNode->propertyValuePairs["Text"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(item->text.getString()));

                if (!item->nodes.empty())
                {
                    // Save as nested 'Item' sections only when needed, use the more compact string list when just storing the leaf items
                    bool recursionNeeded = false;
                    for (const auto& childItem : item->nodes)
                    {
                        if (!childItem->nodes.empty())
                        {
                            recursionNeeded = true;
                            break;
                        }
                    }

                    if (recursionNeeded)
                        saveItems(itemNode, item->nodes);
                    else
                    {
                        std::string itemList = "[" + Serializer::serialize(item->nodes[0]->text.getString());
                        for (std::size_t i = 1; i < item->nodes.size(); ++i)
                            itemList += ", " + Serializer::serialize(item->nodes[i]->text.getString());
                        itemList += "]";

                        itemNode->propertyValuePairs["Items"] = std::make_unique<DataIO::ValueNode>(itemList);
                    }
                }

                parentNode->children.push_back(std::move(itemNode));
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TreeView::TreeView()
    {
        m_type = "TreeView";
        m_draggableWidget = true;

        // Rotate the horizontal scrollbar
        m_horizontalScrollbar->setSize(m_horizontalScrollbar->getSize().y, m_horizontalScrollbar->getSize().x);

        m_renderer = aurora::makeCopied<TreeViewRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setTextSize(getGlobalTextSize());
        setItemHeight(static_cast<unsigned int>(Text::getLineHeight(m_fontCached, m_textSize, m_textStyleCached) * 1.25f));
        setSize({Text::getLineHeight(m_fontCached, m_textSize, m_textStyleCached) * 10,
                 (m_itemHeight * 7) + m_paddingCached.getTop() + m_paddingCached.getBottom() + m_bordersCached.getTop() + m_bordersCached.getBottom()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TreeView::TreeView(const TreeView& other) :
        Widget                              {other},
        onItemSelect                        {other.onItemSelect},
        onDoubleClick                       {other.onDoubleClick},
        onExpand                            {other.onExpand},
        onCollapse                          {other.onCollapse},
        onRightClick                        {other.onRightClick},
        m_selectedItem                      {other.m_selectedItem},
        m_hoveredItem                       {other.m_hoveredItem},
        m_itemHeight                        {other.m_itemHeight},
        m_requestedTextSize                 {other.m_requestedTextSize},
        m_maxRight                          {other.m_maxRight},
        m_iconBounds                        {other.m_iconBounds},
        m_verticalScrollbar                 {other.m_verticalScrollbar},
        m_horizontalScrollbar               {other.m_horizontalScrollbar},
        m_possibleDoubleClick               {other.m_possibleDoubleClick},
        m_doubleClickNodeIndex              {other.m_doubleClickNodeIndex},
        m_spriteBranchExpanded              {other.m_spriteBranchExpanded},
        m_spriteBranchCollapsed             {other.m_spriteBranchCollapsed},
        m_spriteLeaf                        {other.m_spriteLeaf},
        m_bordersCached                     {other.m_bordersCached},
        m_paddingCached                     {other.m_paddingCached},
        m_borderColorCached                 {other.m_borderColorCached},
        m_backgroundColorCached             {other.m_backgroundColorCached},
        m_textColorCached                   {other.m_textColorCached},
        m_textColorHoverCached              {other.m_textColorHoverCached},
        m_selectedTextColorCached           {other.m_selectedTextColorCached},
        m_selectedTextColorHoverCached      {other.m_selectedTextColorHoverCached},
        m_selectedBackgroundColorCached     {other.m_selectedBackgroundColorCached},
        m_selectedBackgroundColorHoverCached{other.m_selectedBackgroundColorHoverCached},
        m_backgroundColorHoverCached        {other.m_backgroundColorHoverCached},
        m_textStyleCached                   {other.m_textStyleCached}
    {
        for (const auto& node : other.m_nodes)
            m_nodes.push_back(cloneNode(node, nullptr));

        m_visibleNodes.resize(other.m_visibleNodes.size(), nullptr);
        cloneVisibleNodeList(other.m_nodes, m_nodes, other.m_visibleNodes, m_visibleNodes);

        assert(std::count(m_visibleNodes.begin(), m_visibleNodes.end(), nullptr) == 0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TreeView& TreeView::operator= (const TreeView& other)
    {
        if (this != &other)
        {
            TreeView temp(other);
            Widget::operator=(temp);

            std::swap(onItemSelect,                         temp.onItemSelect);
            std::swap(onDoubleClick,                        temp.onDoubleClick);
            std::swap(onExpand,                             temp.onExpand);
            std::swap(onCollapse,                           temp.onCollapse);
            std::swap(onRightClick,                         temp.onRightClick);
            std::swap(m_nodes,                              temp.m_nodes);
            std::swap(m_visibleNodes,                       temp.m_visibleNodes);
            std::swap(m_selectedItem,                       temp.m_selectedItem);
            std::swap(m_hoveredItem,                        temp.m_hoveredItem);
            std::swap(m_itemHeight,                         temp.m_itemHeight);
            std::swap(m_requestedTextSize,                  temp.m_requestedTextSize);
            std::swap(m_maxRight,                           temp.m_maxRight);
            std::swap(m_iconBounds,                         temp.m_iconBounds);
            std::swap(m_verticalScrollbar,                  temp.m_verticalScrollbar);
            std::swap(m_horizontalScrollbar,                temp.m_horizontalScrollbar);
            std::swap(m_possibleDoubleClick,                temp.m_possibleDoubleClick);
            std::swap(m_doubleClickNodeIndex,               temp.m_doubleClickNodeIndex);
            std::swap(m_spriteBranchExpanded,               temp.m_spriteBranchExpanded);
            std::swap(m_spriteBranchCollapsed,              temp.m_spriteBranchCollapsed);
            std::swap(m_spriteLeaf,                         temp.m_spriteLeaf);
            std::swap(m_bordersCached,                      temp.m_bordersCached);
            std::swap(m_paddingCached,                      temp.m_paddingCached);
            std::swap(m_borderColorCached,                  temp.m_borderColorCached);
            std::swap(m_backgroundColorCached,              temp.m_backgroundColorCached);
            std::swap(m_textColorCached,                    temp.m_textColorCached);
            std::swap(m_textColorHoverCached,               temp.m_textColorHoverCached);
            std::swap(m_selectedTextColorCached,            temp.m_selectedTextColorCached);
            std::swap(m_selectedTextColorHoverCached,       temp.m_selectedTextColorHoverCached);
            std::swap(m_selectedBackgroundColorCached,      temp.m_selectedBackgroundColorCached);
            std::swap(m_selectedBackgroundColorHoverCached, temp.m_selectedBackgroundColorHoverCached);
            std::swap(m_backgroundColorHoverCached,         temp.m_backgroundColorHoverCached);
            std::swap(m_textStyleCached,                    temp.m_textStyleCached);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TreeView::Ptr TreeView::create()
    {
        return std::make_shared<TreeView>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TreeView::Ptr TreeView::copy(TreeView::ConstPtr treeView)
    {
        if (treeView)
            return std::static_pointer_cast<TreeView>(treeView->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TreeViewRenderer* TreeView::getSharedRenderer()
    {
        return aurora::downcast<TreeViewRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const TreeViewRenderer* TreeView::getSharedRenderer() const
    {
        return aurora::downcast<const TreeViewRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TreeViewRenderer* TreeView::getRenderer()
    {
        return aurora::downcast<TreeViewRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const TreeViewRenderer* TreeView::getRenderer() const
    {
        return aurora::downcast<const TreeViewRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        updateIconBounds();
        m_bordersCached.updateParentSize(getSize());
        m_paddingCached.updateParentSize(getSize());

        markNodesDirty();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TreeView::addItem(const std::vector<sf::String>& hierarchy, bool createParents)
    {
        if (hierarchy.empty())
            return false;

        if (hierarchy.size() >= 2)
        {
            auto* node = findParentNode(hierarchy, 0, m_nodes, nullptr, createParents);
            if (!node)
                return false;

            createNode(node->nodes, node, hierarchy.back());
        }
        else // Root node
            createNode(m_nodes, nullptr, hierarchy.back());

        markNodesDirty();
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::expand(const std::vector<sf::String>& hierarchy)
    {
        expandOrCollapse(hierarchy, true);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::expandAll()
    {
        expandOrCollapseAll(m_nodes, true);
        markNodesDirty();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::collapse(const std::vector<sf::String>& hierarchy)
    {
        expandOrCollapse(hierarchy, false);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::collapseAll()
    {
        expandOrCollapseAll(m_nodes, false);
        markNodesDirty();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TreeView::selectItem(const std::vector<sf::String>& hierarchy)
    {
        // Make sure the parent of the item we are selecting is expanded
        if (hierarchy.size() >= 2)
        {
            if (!expandOrCollapse(std::vector<sf::String>(hierarchy.begin(), hierarchy.end()-1), true))
                return false;
        }

        auto* node = findNode(m_nodes, hierarchy, 0);
        if (!node)
            return false;

        for (unsigned int i = 0; i < m_visibleNodes.size(); ++i)
        {
            if (m_visibleNodes[i].get() == node)
            {
                updateSelectedItem(i);
                return true;
            }
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::deselectItem()
    {
        updateSelectedItem(-1);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TreeView::removeItem(const std::vector<sf::String>& hierarchy, bool removeParentsWhenEmpty)
    {
        const bool ret = removeItemImpl(hierarchy, removeParentsWhenEmpty, 0, m_nodes);
        markNodesDirty();
        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::removeAllItems()
    {
        m_nodes.clear();
        markNodesDirty();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<sf::String> TreeView::getSelectedItem() const
    {
        std::vector<sf::String> hierarchy;

        if (m_selectedItem == -1)
            return hierarchy;

        const auto* node = m_visibleNodes[m_selectedItem].get();
        while (node)
        {
            hierarchy.insert(hierarchy.begin(), node->text.getString());
            node = node->parent;
        }

        return hierarchy;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::toggleNodeInternal(std::size_t index)
    {
        if (index >= m_visibleNodes.size())
            return;

        if (m_visibleNodes[index]->nodes.empty())
            return;

        std::vector<sf::String> hierarchy;
        auto* node = m_visibleNodes[index].get();
        while (node)
        {
            hierarchy.insert(hierarchy.begin(), node->text.getString());
            node = node->parent;
        }

        m_visibleNodes[index]->expanded = !m_visibleNodes[index]->expanded;
        if (m_visibleNodes[index]->expanded)
            onExpand.emit(this, hierarchy.back(), hierarchy);
        else
            onCollapse.emit(this, hierarchy.back(), hierarchy);

        markNodesDirty();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f TreeView::getInnerSize() const
    {
        return {std::max(0.f, getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight()),
                std::max(0.f, getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom())};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::updateIconBounds()
    {
        if (m_spriteBranchCollapsed.isSet() || m_spriteBranchExpanded.isSet() || m_spriteLeaf.isSet())
        {
            m_iconBounds =
                {
                    std::max(std::max(m_spriteBranchCollapsed.getSize().x, m_spriteBranchExpanded.getSize().x), m_spriteLeaf.getSize().x),
                    std::max(std::max(m_spriteBranchCollapsed.getSize().y, m_spriteBranchExpanded.getSize().y), m_spriteLeaf.getSize().y)
                };
        }
        else
        {
            const float wantedIconSize = std::round(m_itemHeight / 2.f);
            const float lineThickness = std::round(m_itemHeight / 10.f);
            const float iconSize = (std::floor((wantedIconSize - lineThickness) / 2.f) * 2.f) + lineThickness; // "+" sign should be symmetric
            m_iconBounds = {iconSize, iconSize};
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<TreeView::ConstNode> TreeView::getNodes() const
    {
        return convertNodesToConstNodes(m_nodes);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::setItemHeight(unsigned int itemHeight)
    {
        m_itemHeight = itemHeight;
        if (m_requestedTextSize == 0)
            setTextSize(0);

        m_verticalScrollbar->setScrollAmount(m_itemHeight);
        m_horizontalScrollbar->setScrollAmount(m_itemHeight);
        markNodesDirty();
        updateIconBounds();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int TreeView::getItemHeight() const
    {
        return m_itemHeight;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::setTextSize(unsigned int textSize)
    {
        m_requestedTextSize = textSize;

        if (textSize)
            m_textSize = textSize;
        else
            m_textSize = Text::findBestTextSize(m_fontCached, m_itemHeight * 0.8f);

        setTextSizeImpl(m_nodes, textSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::setVerticalScrollbarValue(unsigned int value)
    {
        m_verticalScrollbar->setValue(value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int TreeView::getVerticalScrollbarValue() const
    {
        return m_verticalScrollbar->getValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::setHorizontalScrollbarValue(unsigned int value)
    {
        m_horizontalScrollbar->setValue(value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int TreeView::getHorizontalScrollbarValue() const
    {
        return m_horizontalScrollbar->getValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TreeView::mouseOnWidget(Vector2f pos) const
    {
        return FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::leftMousePressed(Vector2f pos)
    {
        pos -= getPosition();

        m_mouseDown = true;

        if (m_verticalScrollbar->mouseOnWidget(pos))
            m_verticalScrollbar->leftMousePressed(pos);
        else if (m_horizontalScrollbar->mouseOnWidget(pos))
            m_horizontalScrollbar->leftMousePressed(pos);
        else
        {
            float maxItemWidth = getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight();
            if (m_verticalScrollbar->isShown())
                maxItemWidth -= m_verticalScrollbar->getSize().x;

            if (FloatRect{m_bordersCached.getLeft() + m_paddingCached.getLeft(), m_bordersCached.getTop() + m_paddingCached.getTop(),
                          maxItemWidth, getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}.contains(pos))
            {
                pos.y -= m_bordersCached.getTop() + m_paddingCached.getTop();
                int selectedItem = static_cast<int>(((pos.y - (m_itemHeight - (m_verticalScrollbar->getValue() % m_itemHeight))) / m_itemHeight) + (m_verticalScrollbar->getValue() / m_itemHeight) + 1);
                if (selectedItem >= static_cast<int>(m_visibleNodes.size()))
                    selectedItem = -1;

                updateSelectedItem(selectedItem);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::leftMouseReleased(Vector2f pos)
    {
        pos -= getPosition();
        auto childPos = pos;
        if (m_mouseDown && !m_verticalScrollbar->isMouseDown() && !m_horizontalScrollbar->isMouseDown())
        {
            m_mouseDown = false;

            float maxItemWidth = getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight();
            if (m_verticalScrollbar->isShown())
                maxItemWidth -= m_verticalScrollbar->getSize().x;

            bool iconPressed = false;
            int selectedIndex = -1;
            if (FloatRect{m_bordersCached.getLeft() + m_paddingCached.getLeft(), m_bordersCached.getTop() + m_paddingCached.getTop(),
                          maxItemWidth, getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}.contains(pos))
            {
                pos.y -= m_bordersCached.getTop() + m_paddingCached.getTop();

                selectedIndex = static_cast<int>(((pos.y - (m_itemHeight - (m_verticalScrollbar->getValue() % m_itemHeight))) / m_itemHeight) + (m_verticalScrollbar->getValue() / m_itemHeight) + 1);
                if ((selectedIndex >= 0) && (selectedIndex == m_selectedItem))
                {
                    // Expand or colapse the node when clicking the icon
                    const float iconPaddingX = (m_iconBounds.x / 4.f);
                    const float iconOffsetX = iconPaddingX + ((m_iconBounds.x + iconPaddingX) * m_visibleNodes[selectedIndex]->depth);
                    const float iconOffsetY = (m_itemHeight - m_iconBounds.y) / 2.f;
                    if (FloatRect{iconOffsetX + m_bordersCached.getLeft() + m_paddingCached.getLeft() - m_horizontalScrollbar->getValue(),
                                  iconOffsetY + (selectedIndex * m_itemHeight) + m_bordersCached.getTop() + m_paddingCached.getTop() - m_verticalScrollbar->getValue(),
                                  m_iconBounds.x,
                                  m_iconBounds.y}.contains(pos))
                    {
                        toggleNodeInternal(selectedIndex);
                        m_possibleDoubleClick = false;
                        iconPressed = true;
                    }
                }
            }

            if (m_possibleDoubleClick)
            {
                m_possibleDoubleClick = false;

                if ((selectedIndex >= 0) && (selectedIndex == m_doubleClickNodeIndex) && (selectedIndex < static_cast<int>(m_visibleNodes.size())))
                {
                    toggleNodeInternal(selectedIndex);

                    // Send double click if this was a leaf node
                    if (m_visibleNodes[selectedIndex]->nodes.empty())
                    {
                        std::vector<sf::String> hierarchy;
                        auto* node = m_visibleNodes[selectedIndex].get();
                        while (node)
                        {
                            hierarchy.insert(hierarchy.begin(), node->text.getString());
                            node = node->parent;
                        }

                        onDoubleClick.emit(this, hierarchy.back(), hierarchy);
                    }
                }
            }
            else if (!iconPressed)
            {
                m_animationTimeElapsed = {};
                m_possibleDoubleClick = true;
                m_doubleClickNodeIndex = selectedIndex;
            }
        }

        m_verticalScrollbar->leftMouseReleased(childPos);
        m_horizontalScrollbar->leftMouseReleased(childPos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::rightMousePressed(Vector2f pos)
    {
        pos -= getPosition();

        m_mouseDown = true;

        float maxItemWidth = getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight();
        if (m_verticalScrollbar->isShown())
            maxItemWidth -= m_verticalScrollbar->getSize().x;

        if (FloatRect{m_bordersCached.getLeft() + m_paddingCached.getLeft(), m_bordersCached.getTop() + m_paddingCached.getTop(),
                      maxItemWidth, getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}.contains(pos))
        {
            pos.y -= m_bordersCached.getTop() + m_paddingCached.getTop();
            int selectedItem = static_cast<int>(((pos.y - (m_itemHeight - (m_verticalScrollbar->getValue() % m_itemHeight))) / m_itemHeight) + (m_verticalScrollbar->getValue() / m_itemHeight) + 1);
            if (selectedItem < static_cast<int>(m_visibleNodes.size()))
            {
                updateSelectedItem(selectedItem);

                std::vector<sf::String> hierarchy;
                auto* node = m_visibleNodes[selectedItem].get();
                while (node)
                {
                    hierarchy.insert(hierarchy.begin(), node->text.getString());
                    node = node->parent;
                }

                onRightClick.emit(this, hierarchy.back(), hierarchy);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::mouseMoved(Vector2f pos)
    {
        pos -= getPosition();

        if (!m_mouseHover)
            mouseEnteredWidget();

        if ((m_verticalScrollbar->isMouseDown() && m_verticalScrollbar->isMouseDownOnThumb()) || m_verticalScrollbar->mouseOnWidget(pos))
            m_verticalScrollbar->mouseMoved(pos);
        else if ((m_horizontalScrollbar->isMouseDown() && m_horizontalScrollbar->isMouseDownOnThumb()) || m_horizontalScrollbar->mouseOnWidget(pos))
            m_horizontalScrollbar->mouseMoved(pos);
        else
        {
            m_verticalScrollbar->mouseNoLongerOnWidget();
            m_horizontalScrollbar->mouseNoLongerOnWidget();

            float maxItemWidth = getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight();
            if (m_verticalScrollbar->isShown())
                maxItemWidth -= m_verticalScrollbar->getSize().x;

            if (FloatRect{m_bordersCached.getLeft() + m_paddingCached.getLeft(), m_bordersCached.getTop() + m_paddingCached.getTop(),
                          maxItemWidth, getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}.contains(pos))
            {
                pos.y -= m_bordersCached.getTop() + m_paddingCached.getTop();

                int hoveredItem = static_cast<int>(((pos.y - (m_itemHeight - (m_verticalScrollbar->getValue() % m_itemHeight))) / m_itemHeight) + (m_verticalScrollbar->getValue() / m_itemHeight) + 1);
                if (hoveredItem >= static_cast<int>(m_visibleNodes.size()))
                    hoveredItem = -1;

                updateHoveredItem(hoveredItem);
            }
            else // Mouse is on top of padding or borders
                updateHoveredItem(-1);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TreeView::mouseWheelScrolled(float delta, Vector2f pos)
    {
        if (m_horizontalScrollbar->isShown()
            && (!m_verticalScrollbar->isShown()
                || m_horizontalScrollbar->mouseOnWidget(pos - getPosition())
                || keyboard::isShiftPressed()))
        {
            m_horizontalScrollbar->mouseWheelScrolled(delta, pos - getPosition());
            mouseMoved(pos);
            return true;
        }
        else if (m_verticalScrollbar->isShown())
        {
            m_verticalScrollbar->mouseWheelScrolled(delta, pos - getPosition());
            mouseMoved(pos);
            return true;
        }
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::mouseNoLongerOnWidget()
    {
        Widget::mouseNoLongerOnWidget();
        m_verticalScrollbar->mouseNoLongerOnWidget();
        m_horizontalScrollbar->mouseNoLongerOnWidget();

        updateHoveredItem(-1);
        m_possibleDoubleClick = false;
        updateSelectedAndHoveringItemColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::leftMouseButtonNoLongerDown()
    {
        Widget::leftMouseButtonNoLongerDown();
        m_verticalScrollbar->leftMouseButtonNoLongerDown();
        m_horizontalScrollbar->leftMouseButtonNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& TreeView::getSignal(std::string signalName)
    {
        if (signalName == toLower(onItemSelect.getName()))
            return onItemSelect;
        else if (signalName == toLower(onDoubleClick.getName()))
            return onDoubleClick;
        else if (signalName == toLower(onExpand.getName()))
            return onExpand;
        else if (signalName == toLower(onCollapse.getName()))
            return onCollapse;
        else if (signalName == toLower(onRightClick.getName()))
            return onRightClick;
        else
            return Widget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            setSize(m_size);
        }
        else if (property == "padding")
        {
            m_paddingCached = getSharedRenderer()->getPadding();
            setSize(m_size);
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == "selectedbackgroundcolor")
        {
            m_selectedBackgroundColorCached = getSharedRenderer()->getSelectedBackgroundColor();
        }
        else if (property == "backgroundcolorhover")
        {
            m_backgroundColorHoverCached = getSharedRenderer()->getBackgroundColorHover();
        }
        else if (property == "selectedbackgroundcolorhover")
        {
            m_selectedBackgroundColorHoverCached = getSharedRenderer()->getSelectedBackgroundColorHover();
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == "texturebranchexpanded")
        {
            m_spriteBranchExpanded.setTexture(getSharedRenderer()->getTextureBranchExpanded());
            updateIconBounds();
            markNodesDirty();
        }
        else if (property == "texturebranchcollapsed")
        {
            m_spriteBranchCollapsed.setTexture(getSharedRenderer()->getTextureBranchCollapsed());
            updateIconBounds();
            markNodesDirty();
        }
        else if (property == "textureleaf")
        {
            m_spriteLeaf.setTexture(getSharedRenderer()->getTextureLeaf());
            updateIconBounds();
            markNodesDirty();
        }
        else if (property == "textcolor")
        {
            m_textColorCached = getSharedRenderer()->getTextColor();
            updateTextColors(m_nodes);
            updateSelectedAndHoveringItemColors();
        }
        else if (property == "textcolorhover")
        {
            m_textColorHoverCached = getSharedRenderer()->getTextColorHover();
            updateTextColors(m_nodes);
            updateSelectedAndHoveringItemColors();
        }
        else if (property == "selectedtextcolor")
        {
            m_selectedTextColorCached = getSharedRenderer()->getSelectedTextColor();
            updateTextColors(m_nodes);
            updateSelectedAndHoveringItemColors();
        }
        else if (property == "selectedtextcolorhover")
        {
            m_selectedTextColorHoverCached = getSharedRenderer()->getSelectedTextColorHover();
            updateTextColors(m_nodes);
            updateSelectedAndHoveringItemColors();
        }
        else if (property == "scrollbar")
        {
            m_verticalScrollbar->setRenderer(getSharedRenderer()->getScrollbar());
            m_horizontalScrollbar->setRenderer(getSharedRenderer()->getScrollbar());

            // If no scrollbar width was set then we may need to use the one from the texture
            if (!getSharedRenderer()->getScrollbarWidth())
            {
                const float width = m_verticalScrollbar->getDefaultWidth();
                m_verticalScrollbar->setSize({width, m_verticalScrollbar->getSize().y});
                m_horizontalScrollbar->setSize({m_horizontalScrollbar->getSize().x, width});
                markNodesDirty();
            }
        }
        else if (property == "scrollbarwidth")
        {
            const float width = getSharedRenderer()->getScrollbarWidth() ? getSharedRenderer()->getScrollbarWidth() : m_verticalScrollbar->getDefaultWidth();
            m_verticalScrollbar->setSize({width, m_verticalScrollbar->getSize().y});
            m_horizontalScrollbar->setSize({m_horizontalScrollbar->getSize().x, width});
            markNodesDirty();
        }
        else if ((property == "opacity") || (property == "opacitydisabled"))
        {
            Widget::rendererChanged(property);

            setTextOpacityImpl(m_nodes, m_opacityCached);

            m_spriteBranchExpanded.setOpacity(m_opacityCached);
            m_spriteBranchCollapsed.setOpacity(m_opacityCached);
            m_spriteLeaf.setOpacity(m_opacityCached);

            m_verticalScrollbar->setInheritedOpacity(m_opacityCached);
            m_horizontalScrollbar->setInheritedOpacity(m_opacityCached);
        }
        else if (property == "font")
        {
            Widget::rendererChanged(property);
            setTextFontImpl(m_nodes, m_fontCached);
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> TreeView::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);
        node->propertyValuePairs["ItemHeight"] = std::make_unique<DataIO::ValueNode>(to_string(m_itemHeight));
        node->propertyValuePairs["TextSize"] = std::make_unique<DataIO::ValueNode>(to_string(m_textSize));
        saveItems(node, m_nodes);
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs["itemheight"])
            setItemHeight(strToInt(node->propertyValuePairs["itemheight"]->value));
        if (node->propertyValuePairs["textsize"])
            setTextSize(strToInt(node->propertyValuePairs["textsize"]->value));

        loadItems(node, m_nodes, nullptr);

        // Remove the 'Item' nodes as they have been processed
        node->children.erase(std::remove_if(node->children.begin(), node->children.end(),
            [](const std::unique_ptr<DataIO::Node>& child){ return toLower(child->name) == "item"; }), node->children.end());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TreeView::update(sf::Time elapsedTime)
    {
        const bool screenRefreshRequired = Widget::update(elapsedTime);

        if (m_animationTimeElapsed >= sf::milliseconds(getDoubleClickTime()))
        {
            m_animationTimeElapsed = {};
            m_possibleDoubleClick = false;
        }

        return screenRefreshRequired;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::loadItems(const std::unique_ptr<DataIO::Node>& node, std::vector<std::shared_ptr<Node>>& items, Node* parent)
    {
        for (const auto& childNode : node->children)
        {
            if (toLower(childNode->name) != "item")
                continue;

            if (!childNode->propertyValuePairs["text"])
                throw Exception{"Failed to parse 'Item' property, expected a nested 'Text' propery"};

            const sf::String itemText = Deserializer::deserialize(ObjectConverter::Type::String, childNode->propertyValuePairs["text"]->value).getString();
            createNode(items, parent, itemText);

            // Recursively handle the menu nodes
            if (!childNode->children.empty())
                loadItems(childNode, items.back()->nodes, items.back().get());

            // Menu items can also be stored in an string array in the 'Items' property instead of as a nested Menu section
            if (childNode->propertyValuePairs["items"])
            {
                if (!childNode->propertyValuePairs["items"]->listNode)
                    throw Exception{"Failed to parse 'Items' property inside 'Item' property, expected a list as value"};

                for (std::size_t i = 0; i < childNode->propertyValuePairs["items"]->valueList.size(); ++i)
                {
                    const sf::String subItemText = Deserializer::deserialize(ObjectConverter::Type::String, childNode->propertyValuePairs["items"]->valueList[i]).getString();
                    createNode(items.back()->nodes, items.back().get(), subItemText);
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int TreeView::updateVisibleNodes(std::vector<std::shared_ptr<Node>>& nodes, Node* selectedNode, float textPadding, unsigned int pos)
    {
        for (auto& node : nodes)
        {
            m_visibleNodes.push_back(node);
            if (selectedNode == node.get())
                m_selectedItem = pos;

            const float iconPadding = (m_iconBounds.x / 4.f);
            const float iconOffset = iconPadding + ((m_iconBounds.x + iconPadding) * node->depth);
            node->text.setPosition(iconOffset + m_iconBounds.x + iconPadding + textPadding,
                                   (pos * m_itemHeight) + ((m_itemHeight - node->text.getSize().y) / 2.f));

            const float right = node->text.getPosition().x + node->text.getSize().x + m_paddingCached.getRight();
            if (right > m_maxRight)
                m_maxRight = right;

            pos++;
            if (node->expanded && !node->nodes.empty())
                pos = updateVisibleNodes(node->nodes, selectedNode, textPadding, pos);
        }

        return pos;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::markNodesDirty()
    {
        Node* selectedNode = nullptr;
        if (m_selectedItem >= 0 && static_cast<std::size_t>(m_selectedItem) < m_visibleNodes.size())
            selectedNode = m_visibleNodes[m_selectedItem].get();

        int oldHoveredItem = m_hoveredItem;

        m_maxRight = 0;
        m_hoveredItem = -1;
        m_selectedItem = -1;
        m_visibleNodes.clear();
        updateVisibleNodes(m_nodes, selectedNode, Text::getExtraHorizontalPadding(m_fontCached, m_textSize), 0);

        if (oldHoveredItem >= 0)
        {
            if (static_cast<std::size_t>(oldHoveredItem) < m_visibleNodes.size())
                m_hoveredItem = oldHoveredItem;

            updateTextColors(m_nodes);
            updateSelectedAndHoveringItemColors();
        }

        m_verticalScrollbar->setMaximum(static_cast<unsigned int>(m_itemHeight * m_visibleNodes.size()));
        m_horizontalScrollbar->setMaximum(static_cast<unsigned int>(m_maxRight));

        if ((m_maxRight + m_verticalScrollbar->getSize().x) > (getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight()))
        {
            m_verticalScrollbar->setSize({m_verticalScrollbar->getSize().x, std::max(0.f, getInnerSize().y - m_horizontalScrollbar->getSize().y)});
            m_verticalScrollbar->setViewportSize(static_cast<unsigned int>(getInnerSize().y - m_horizontalScrollbar->getSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()));
        }
        else
        {
            m_verticalScrollbar->setSize({m_verticalScrollbar->getSize().x, std::max(0.f, getInnerSize().y)});
            m_verticalScrollbar->setViewportSize(static_cast<unsigned int>(getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()));
        }

        if (m_verticalScrollbar->isShown())
        {
            m_horizontalScrollbar->setSize({std::max(0.f, getInnerSize().x - m_verticalScrollbar->getSize().x), m_horizontalScrollbar->getSize().y});
            m_horizontalScrollbar->setViewportSize(static_cast<unsigned int>(getInnerSize().x - m_verticalScrollbar->getSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight()));
        }
        else
        {
            m_horizontalScrollbar->setSize({std::max(0.f, getInnerSize().x), m_horizontalScrollbar->getSize().y});
            m_horizontalScrollbar->setViewportSize(static_cast<unsigned int>(getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight()));
        }

        // If the horizontal scrollbar is shown then add a little padding at the end of the longest line
        if (m_horizontalScrollbar->isShown())
            m_horizontalScrollbar->setMaximum(static_cast<unsigned int>(m_horizontalScrollbar->getMaximum() + (m_iconBounds.x / 4.f)));

        m_verticalScrollbar->setPosition(getSize().x - m_bordersCached.getRight() - m_verticalScrollbar->getSize().x, m_bordersCached.getTop());
        m_horizontalScrollbar->setPosition(m_bordersCached.getLeft(), getSize().y - m_bordersCached.getBottom() - m_horizontalScrollbar->getSize().y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());
        sf::RenderStates statesForScrollbars = states;

        if (m_bordersCached != Borders{0})
        {
            drawBorders(target, states, m_bordersCached, getSize(), m_borderColorCached);
            states.transform.translate(m_bordersCached.getOffset());
        }

        drawRectangleShape(target, states, getInnerSize(), m_backgroundColorCached);

        {
            float maxItemWidth = getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight();
            if (m_verticalScrollbar->isShown())
                maxItemWidth -= m_verticalScrollbar->getSize().x;

            const Clipping clipping
            {
                target, states,
                {m_paddingCached.getLeft(), m_paddingCached.getTop()},
                {maxItemWidth, getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}
            };

            int firstNode = 0;
            int lastNode = static_cast<int>(m_visibleNodes.size());
            if (m_verticalScrollbar->getViewportSize() < m_verticalScrollbar->getMaximum())
            {
                firstNode = static_cast<int>(m_verticalScrollbar->getValue() / m_itemHeight);
                lastNode = static_cast<int>((m_verticalScrollbar->getValue() + m_verticalScrollbar->getViewportSize()) / m_itemHeight);

                // Show another item when the scrollbar is standing between two items
                if ((m_verticalScrollbar->getValue() + m_verticalScrollbar->getViewportSize()) % m_itemHeight != 0)
                    ++lastNode;
            }

            states.transform.translate({m_paddingCached.getLeft() - m_horizontalScrollbar->getValue(), m_paddingCached.getTop() - m_verticalScrollbar->getValue()});

            // Draw the background of the selected item
            if ((m_selectedItem >= firstNode) && (m_selectedItem < lastNode))
            {
                states.transform.translate({static_cast<float>(m_horizontalScrollbar->getValue()), m_selectedItem * static_cast<float>(m_itemHeight)});

                const Vector2f size = {getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(), static_cast<float>(m_itemHeight)};
                if ((m_selectedItem == m_hoveredItem) && m_selectedBackgroundColorHoverCached.isSet())
                    drawRectangleShape(target, states, size, m_selectedBackgroundColorHoverCached);
                else
                    drawRectangleShape(target, states, size, m_selectedBackgroundColorCached);

                states.transform.translate({-static_cast<float>(m_horizontalScrollbar->getValue()), -m_selectedItem * static_cast<float>(m_itemHeight)});
            }

            // Draw the background of the item on which the mouse is standing
            if ((m_hoveredItem >= firstNode) && (m_hoveredItem < lastNode) && (m_hoveredItem != m_selectedItem) && m_backgroundColorHoverCached.isSet())
            {
                states.transform.translate({static_cast<float>(m_horizontalScrollbar->getValue()), m_hoveredItem * static_cast<float>(m_itemHeight)});
                drawRectangleShape(target, states, {getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(), static_cast<float>(m_itemHeight)}, m_backgroundColorHoverCached);
                states.transform.translate({-static_cast<float>(m_horizontalScrollbar->getValue()), -m_hoveredItem * static_cast<float>(m_itemHeight)});
            }

            // Draw the icons
            for (int i = firstNode; i < lastNode; ++i)
            {
                auto statesForIcon = states;
                const float iconPadding = (m_iconBounds.x / 4.f);
                const float iconOffset = iconPadding + ((m_iconBounds.x + iconPadding) * m_visibleNodes[i]->depth);
                statesForIcon.transform.translate(std::round(iconOffset), std::round((i * m_itemHeight) + ((m_itemHeight - m_iconBounds.y) / 2.f)));

                // Draw an icon for the leaf node if a texture is set
                if (m_visibleNodes[i]->nodes.empty())
                {
                    if (m_spriteLeaf.isSet())
                        m_spriteLeaf.draw(target, statesForIcon);
                }
                else // Branch node
                {
                    if (m_spriteLeaf.isSet() || m_spriteBranchExpanded.isSet() || m_spriteBranchCollapsed.isSet())
                    {
                        const Sprite* iconSprite = nullptr;
                        if (m_visibleNodes[i]->expanded)
                        {
                            if (m_spriteBranchExpanded.isSet())
                                iconSprite = &m_spriteBranchExpanded;
                            else if (m_spriteBranchCollapsed.isSet())
                                iconSprite = &m_spriteBranchCollapsed;
                            else
                                iconSprite = &m_spriteLeaf;
                        }
                        else // Collapsed node
                        {
                            if (m_spriteBranchCollapsed.isSet())
                                iconSprite = &m_spriteBranchCollapsed;
                            else if (m_spriteBranchExpanded.isSet())
                                iconSprite = &m_spriteBranchExpanded;
                            else
                                iconSprite = &m_spriteLeaf;
                        }

                        iconSprite->draw(target, statesForIcon);
                    }
                    else // No textures are used
                    {
                        Color iconColor = m_textColorCached;
                        if (i == m_selectedItem)
                        {
                            if ((m_selectedItem == m_hoveredItem) && m_selectedTextColorHoverCached.isSet())
                                iconColor = m_selectedTextColorHoverCached;
                            else if (m_selectedTextColorCached.isSet())
                                iconColor = m_selectedTextColorCached;
                        }
                        if ((i == m_hoveredItem) && (m_selectedItem != m_hoveredItem))
                        {
                            if (m_textColorHoverCached.isSet())
                                iconColor = m_textColorHoverCached;
                        }

                        const float thickness = std::max(1.f, std::round(m_itemHeight / 10.f));
                        if (m_visibleNodes[i]->expanded)
                        {
                            // Draw "-"
                            statesForIcon.transform.translate(0, (m_iconBounds.y - thickness) / 2.f);
                            drawRectangleShape(target, statesForIcon, {m_iconBounds.x, thickness}, iconColor);
                        }
                        else // Collapsed node
                        {
                            // Draw "+"
                            statesForIcon.transform.translate(0, (m_iconBounds.y - thickness) / 2.f);
                            drawRectangleShape(target, statesForIcon, {m_iconBounds.x, thickness}, iconColor);
                            statesForIcon.transform.translate((m_iconBounds.x - thickness) / 2.f, -(m_iconBounds.y - thickness) / 2.f);
                            drawRectangleShape(target, statesForIcon, {thickness, m_iconBounds.y}, iconColor);
                        }
                    }
                }
            }

            // Draw the texts
            for (int i = firstNode; i < lastNode; ++i)
                m_visibleNodes[i]->text.draw(target, states);
        }

        m_horizontalScrollbar->draw(target, statesForScrollbars);
        m_verticalScrollbar->draw(target, statesForScrollbars);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::createNode(std::vector<std::shared_ptr<Node>>& nodes, Node* parent, const sf::String& text)
    {
        auto newNode = std::make_shared<Node>();
        newNode->text.setFont(m_fontCached);
        newNode->text.setColor(m_textColorCached);
        newNode->text.setOpacity(m_opacityCached);
        newNode->text.setCharacterSize(m_textSize);
        newNode->text.setString(text);
        newNode->expanded = true;
        newNode->parent = parent;

        if (parent)
            newNode->depth = parent->depth + 1;
        else
            newNode->depth = 0;

        nodes.push_back(std::move(newNode));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TreeView::expandOrCollapse(const std::vector<sf::String>& hierarchy, bool expandNode)
    {
        if (hierarchy.empty())
            return false;

        if (hierarchy.size() >= 2)
        {
            auto* node = findNode(m_nodes, hierarchy, 0);
            if (!node)
                return false;

            bool nodeChanged = false;
            if (expandNode)
            {
                // When expanding, also expand all parents
                auto* nodeToExpand = node;
                while (nodeToExpand)
                {
                    if (nodeToExpand->expanded != expandNode)
                    {
                        nodeToExpand->expanded = expandNode;
                        nodeChanged = true;
                    }

                    nodeToExpand = nodeToExpand->parent;
                }
            }
            else // Collapsing
            {
                if (node->expanded != expandNode)
                {
                    node->expanded = expandNode;
                    nodeChanged = true;
                }
            }


            if (nodeChanged)
                markNodesDirty();

            return true;
        }
        else // Root node
        {
            for (auto& node : m_nodes)
            {
                if (node->text.getString() != hierarchy.back())
                    continue;

                if (node->expanded != expandNode)
                {
                    node->expanded = expandNode;
                    markNodesDirty();
                }

                return true;
            }
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::updateTextColors(std::vector<std::shared_ptr<Node>>& nodes)
    {
        for (auto& node : nodes)
        {
            node->text.setColor(m_textColorCached);
            updateTextColors(node->nodes);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::updateSelectedAndHoveringItemColors()
    {
        if (m_selectedItem >= 0)
        {
            if ((m_selectedItem == m_hoveredItem) && m_selectedTextColorHoverCached.isSet())
                m_visibleNodes[m_selectedItem]->text.setColor(m_selectedTextColorHoverCached);
            else if (m_selectedTextColorCached.isSet())
                m_visibleNodes[m_selectedItem]->text.setColor(m_selectedTextColorCached);
        }

        if ((m_hoveredItem >= 0) && (m_selectedItem != m_hoveredItem))
        {
            if (m_textColorHoverCached.isSet())
                m_visibleNodes[m_hoveredItem]->text.setColor(m_textColorHoverCached);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::updateHoveredItem(int item)
    {
        if (m_hoveredItem == item)
            return;

        if (m_hoveredItem >= 0)
        {
            if ((m_selectedItem == m_hoveredItem) && m_selectedTextColorCached.isSet())
                m_visibleNodes[m_hoveredItem]->text.setColor(m_selectedTextColorCached);
            else
                m_visibleNodes[m_hoveredItem]->text.setColor(m_textColorCached);
        }

        m_hoveredItem = item;
        updateSelectedAndHoveringItemColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::updateSelectedItem(int item)
    {
        if (m_selectedItem == item)
            return;

        if (m_selectedItem >= 0)
        {
            if ((m_selectedItem == m_hoveredItem) && m_textColorHoverCached.isSet())
                m_visibleNodes[m_selectedItem]->text.setColor(m_textColorHoverCached);
            else
                m_visibleNodes[m_selectedItem]->text.setColor(m_textColorCached);
        }

        m_selectedItem = item;
        if (m_selectedItem >= 0)
        {
            std::vector<sf::String> hierarchy;
            auto* node = m_visibleNodes[m_selectedItem].get();
            while (node)
            {
                hierarchy.insert(hierarchy.begin(), node->text.getString());
                node = node->parent;
            }

            onItemSelect.emit(this, hierarchy.back(), hierarchy);
        }
        else
            onItemSelect.emit(this, "", {});

        updateSelectedAndHoveringItemColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TreeView::Node* TreeView::findParentNode(const std::vector<sf::String>& hierarchy, unsigned int parentIndex, std::vector<std::shared_ptr<Node>>& nodes, Node* parent, bool createParents)
    {
        for (auto& node : nodes)
        {
            if (node->text.getString() != hierarchy[parentIndex])
                continue;
            else if (parentIndex + 2 == hierarchy.size())
                return node.get();
            else
                return findParentNode(hierarchy, parentIndex + 1, node->nodes, node.get(), createParents);
        }

        if (createParents)
        {
            createNode(nodes, parent, hierarchy[parentIndex]);
            if (parentIndex + 2 == hierarchy.size())
                return nodes.back().get();
            else
                return findParentNode(hierarchy, parentIndex + 1, nodes.back()->nodes, nodes.back().get(), createParents);
        }

        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

