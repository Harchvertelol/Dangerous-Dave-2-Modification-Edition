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


#include <TGUI/Global.hpp>
#include <TGUI/Texture.hpp>
#include <TGUI/Exception.hpp>
#include <TGUI/TextureManager.hpp>

#include <cassert>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    Texture::TextureLoaderFunc Texture::m_textureLoader = &TextureManager::getTexture;
    Texture::ImageLoaderFunc Texture::m_imageLoader = [](const sf::String& filename) -> std::unique_ptr<sf::Image>
        {
#ifdef TGUI_SYSTEM_WINDOWS
            const std::string filenameAnsiString(filename.toAnsiString());
#else
            const std::basic_string<sf::Uint8>& filenameUtf8 = filename.toUtf8();
            const std::string filenameAnsiString(filenameUtf8.begin(), filenameUtf8.end());
#endif

            auto image = std::make_unique<sf::Image>();
            if (image->loadFromFile(filenameAnsiString))
                return image;
            else
                return nullptr;
        };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture::Texture(const sf::String& id, const sf::IntRect& partRect, const sf::IntRect& middlePart, bool smooth)
    {
        load(id, partRect, middlePart, smooth);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture::Texture(const sf::Texture& texture, const sf::IntRect& partRect, const sf::IntRect& middlePart)
    {
        load(texture, partRect, middlePart);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture::Texture(const Texture& other) :
        m_data            {other.m_data},
        m_color           {other.m_color},
        m_shader          {other.m_shader},
        m_middleRect      {other.m_middleRect},
        m_id              {other.m_id},
        m_copyCallback    {other.m_copyCallback},
        m_destructCallback{other.m_destructCallback}
    {
        if (getData() && (m_copyCallback != nullptr))
            m_copyCallback(getData());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture::Texture(Texture&& other) noexcept :
        m_data            {std::move(other.m_data)},
        m_color           {std::move(other.m_color)},
        m_shader          {std::move(other.m_shader)},
        m_middleRect      {std::move(other.m_middleRect)},
        m_id              {std::move(other.m_id)},
        m_copyCallback    {std::move(other.m_copyCallback)},
        m_destructCallback{std::move(other.m_destructCallback)}
    {
        other.m_data = nullptr;
        other.m_copyCallback = nullptr;
        other.m_destructCallback = nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture::~Texture()
    {
        if (getData() && (m_destructCallback != nullptr))
            m_destructCallback(getData());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture& Texture::operator=(const Texture& other)
    {
        if (this != &other)
        {
            Texture temp{other};

            std::swap(m_data,             temp.m_data);
            std::swap(m_color,            temp.m_color);
            std::swap(m_shader,           temp.m_shader);
            std::swap(m_middleRect,       temp.m_middleRect);
            std::swap(m_id,               temp.m_id);
            std::swap(m_copyCallback,     temp.m_copyCallback);
            std::swap(m_destructCallback, temp.m_destructCallback);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture& Texture::operator=(Texture&& other) noexcept
    {
        if (this != &other)
        {
            m_data             = std::move(other.m_data);
            m_color            = std::move(other.m_color);
            m_shader           = std::move(other.m_shader);
            m_middleRect       = std::move(other.m_middleRect);
            m_id               = std::move(other.m_id);
            m_copyCallback     = std::move(other.m_copyCallback);
            m_destructCallback = std::move(other.m_destructCallback);

            other.m_data = nullptr;
            other.m_copyCallback = nullptr;
            other.m_destructCallback = nullptr;
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::load(const sf::String& id, const sf::IntRect& partRect, const sf::IntRect& middleRect, bool smooth)
    {
        if (id.isEmpty())
        {
            *this = Texture{};
            return;
        }

        if (getData() && (m_destructCallback != nullptr))
            m_destructCallback(getData());

        m_data = nullptr;

        std::shared_ptr<TextureData> data;
#ifdef TGUI_SYSTEM_WINDOWS
        if ((id[0] != '/') && (id[0] != '\\') && ((id.getSize() <= 1) || (id[1] != ':')))
#else
        if (id[0] != '/')
#endif
        {
            data = m_textureLoader(*this, getResourcePath() + id, partRect);
            if (!data)
                throw Exception{"Failed to load '" + getResourcePath() + id + "'"};
        }
        else
        {
            data = m_textureLoader(*this, id, partRect);
            if (!data)
                throw Exception{"Failed to load '" + id + "'"};
        }

        m_id = id;
        setTextureData(data, middleRect);

        if (smooth)
            setSmooth(true);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::load(const sf::Texture& texture, const sf::IntRect& partRect, const sf::IntRect& middleRect)
    {
        if (getData() && (m_destructCallback != nullptr))
            m_destructCallback(getData());

        m_data = nullptr;
        auto data = std::make_shared<TextureData>();
        if (partRect == sf::IntRect{})
            data->texture = texture;
        else
            data->texture.loadFromImage(texture.copyToImage(), partRect);

        m_id = "";
        setTextureData(data, middleRect);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::String& Texture::getId() const
    {
        return m_id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<TextureData> Texture::getData() const
    {
        return m_data;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Texture::getImageSize() const
    {
        if (!m_data)
            return {0,0};

        if (m_data->svgImage)
            return m_data->svgImage->getSize();
        else
            return {sf::Vector2f{m_data->texture.getSize()}};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::setSmooth(bool smooth)
    {
        if (m_data)
            m_data->texture.setSmooth(smooth);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Texture::isSmooth() const
    {
        if (m_data)
            return m_data->texture.isSmooth();
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::setColor(const Color& color)
    {
        m_color = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Color& Texture::getColor() const
    {
        return m_color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::setShader(sf::Shader* shader)
    {
#ifdef TGUI_NEXT
        m_shader = shader;
#else
        if (m_data)
            m_data->shader = shader;
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Shader* Texture::getShader() const
    {
#ifdef TGUI_NEXT
        return m_shader;
#else
        if (m_data)
            return m_data->shader;
        else
            return nullptr;
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::IntRect Texture::getMiddleRect() const
    {
        return m_middleRect;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Texture::isTransparentPixel(sf::Vector2u pixel) const
    {
        if (!m_data || !m_data->image)
            return false;

        assert(pixel.x < m_data->texture.getSize().x && pixel.y < m_data->texture.getSize().y);

        if (m_data->image->getPixel(pixel.x + m_data->rect.left, pixel.y + m_data->rect.top).a == 0)
            return true;
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::setCopyCallback(const CallbackFunc& func)
    {
        m_copyCallback = func;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::setDestructCallback(const CallbackFunc& func)
    {
        m_destructCallback = func;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Texture::operator==(const Texture& right) const
    {
        return (m_id == right.m_id)
            && (!m_id.isEmpty() || (m_data == right.m_data))
            && (m_middleRect == right.m_middleRect)
            && (m_shader == right.m_shader)
            && (m_color == right.m_color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Texture::operator!=(const Texture& right) const
    {
        return !(*this == right);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::setImageLoader(const ImageLoaderFunc& func)
    {
        assert(func != nullptr);
        m_imageLoader = func;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Texture::ImageLoaderFunc& Texture::getImageLoader()
    {
        return m_imageLoader;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::setTextureLoader(const TextureLoaderFunc& func)
    {
        assert(func != nullptr);
        m_textureLoader = func;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Texture::TextureLoaderFunc& Texture::getTextureLoader()
    {
        return m_textureLoader;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::setTextureData(std::shared_ptr<TextureData> data, const sf::IntRect& middleRect)
    {
        if (getData() && (m_destructCallback != nullptr))
            m_destructCallback(getData());

        m_data = data;

        if (middleRect == sf::IntRect{})
        {
            if (m_data->svgImage)
                m_middleRect = {0, 0, static_cast<int>(m_data->svgImage->getSize().x), static_cast<int>(m_data->svgImage->getSize().y)};
            else
                m_middleRect = {0, 0, static_cast<int>(m_data->texture.getSize().x), static_cast<int>(m_data->texture.getSize().y)};
        }
        else
            m_middleRect = middleRect;
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


#include <TGUI/Renderers/PanelRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_OUTLINE(PanelRenderer, Borders)

    TGUI_RENDERER_PROPERTY_COLOR(PanelRenderer, BorderColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(PanelRenderer, BackgroundColor, Color::White)

    TGUI_RENDERER_PROPERTY_TEXTURE(PanelRenderer, TextureBackground)
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
#include <TGUI/Widgets/EditBox.hpp>
#include <TGUI/Clipboard.hpp>
#include <TGUI/Keyboard.hpp>
#include <TGUI/Clipping.hpp>

/// TODO: Where m_selStart and m_selEnd are compared, use std::min and std::max and merge the if and else bodies

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if TGUI_COMPILED_WITH_CPP_VER < 17
    const std::string EditBox::Validator::All   = ".*";
    const std::string EditBox::Validator::Int   = "[+-]?[0-9]*";
    const std::string EditBox::Validator::UInt  = "[0-9]*";
    const std::string EditBox::Validator::Float = "[+-]?[0-9]*\\.?[0-9]*";
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBox::EditBox()
    {
        m_type = "EditBox";
        m_textBeforeSelection.setFont(m_fontCached);
        m_textSelection.setFont(m_fontCached);
        m_textAfterSelection.setFont(m_fontCached);
        m_textFull.setFont(m_fontCached);
        m_textSuffix.setFont(m_fontCached);
        m_defaultText.setFont(m_fontCached);

        m_draggableWidget = true;

        m_renderer = aurora::makeCopied<EditBoxRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setTextSize(getGlobalTextSize());
        setSize({m_textFull.getLineHeight() * 10,
                 m_textFull.getLineHeight() * 1.25f + m_paddingCached.getTop() + m_paddingCached.getBottom() + m_bordersCached.getTop() + m_bordersCached.getBottom()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBox::Ptr EditBox::create()
    {
        return std::make_shared<EditBox>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBox::Ptr EditBox::copy(EditBox::ConstPtr editBox)
    {
        if (editBox)
            return std::static_pointer_cast<EditBox>(editBox->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBoxRenderer* EditBox::getSharedRenderer()
    {
        return aurora::downcast<EditBoxRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const EditBoxRenderer* EditBox::getSharedRenderer() const
    {
        return aurora::downcast<const EditBoxRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBoxRenderer* EditBox::getRenderer()
    {
        return aurora::downcast<EditBoxRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const EditBoxRenderer* EditBox::getRenderer() const
    {
        return aurora::downcast<const EditBoxRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_bordersCached.updateParentSize(getSize());
        m_paddingCached.updateParentSize(getSize());

        // Recalculate the text size when auto scaling
        if (m_textSize == 0)
            setText(m_text);

        m_sprite.setSize(getInnerSize());
        m_spriteHover.setSize(getInnerSize());
        m_spriteDisabled.setSize(getInnerSize());
        m_spriteFocused.setSize(getInnerSize());

        // Set the size of the caret
        m_caret.setSize({m_caret.getSize().x, getInnerSize().y - m_paddingCached.getBottom() - m_paddingCached.getTop()});

        // Recalculate the position of the texts
        recalculateTextPositions();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setEnabled(bool enabled)
    {
        Widget::setEnabled(enabled);
        updateTextColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setText(const sf::String& text)
    {
        // Check if the text is auto sized
        if (m_textSize == 0)
        {
            m_textFull.setCharacterSize(Text::findBestTextSize(m_fontCached, (getInnerSize().y - m_paddingCached.getBottom() - m_paddingCached.getTop()) * 0.8f));
            m_textSuffix.setCharacterSize(m_textFull.getCharacterSize());
            m_textBeforeSelection.setCharacterSize(m_textFull.getCharacterSize());
            m_textSelection.setCharacterSize(m_textFull.getCharacterSize());
            m_textAfterSelection.setCharacterSize(m_textFull.getCharacterSize());
            m_defaultText.setCharacterSize(m_textFull.getCharacterSize());
        }
        else // When the text has a fixed size
        {
            m_textFull.setCharacterSize(m_textSize);
            m_textSuffix.setCharacterSize(m_textSize);
            m_textBeforeSelection.setCharacterSize(m_textSize);
            m_textSelection.setCharacterSize(m_textSize);
            m_textAfterSelection.setCharacterSize(m_textSize);
            m_defaultText.setCharacterSize(m_textSize);
        }

        // Change the text if allowed
        if (m_regexString == ".*")
            m_text = text;
        else if (std::regex_match(text.toAnsiString(), m_regex))
            m_text = text.toAnsiString(); // Unicode is not supported when using regex because it can't be checked
        else // Clear the text
            m_text = "";

        // Remove all the excess characters if there is a character limit
        if ((m_maxChars > 0) && (m_text.getSize() > m_maxChars))
            m_text.erase(m_maxChars, sf::String::InvalidPos);

        // Set the displayed text
        if (m_passwordChar != '\0')
        {
            sf::String displayedText = m_text;
            std::fill(displayedText.begin(), displayedText.end(), m_passwordChar);

            m_textFull.setString(displayedText);
        }
        else
            m_textFull.setString(m_text);

        // Set the texts
        m_textBeforeSelection.setString(m_textFull.getString());
        m_textSelection.setString("");
        m_textAfterSelection.setString("");

        if (!m_fontCached)
            return;

        // Check if there is a text width limit
        const float width = getVisibleEditBoxWidth();
        if (m_limitTextWidth)
        {
            // Now check if the text fits into the EditBox
            while (!m_textFull.getString().isEmpty() && (getFullTextWidth() > width))
            {
                // The text doesn't fit inside the EditBox, so the last character must be deleted.
                sf::String displayedString = m_textFull.getString();
                displayedString.erase(displayedString.getSize()-1);
                m_textFull.setString(displayedString);
                m_text.erase(m_text.getSize()-1);
            }

            m_textBeforeSelection.setString(m_textFull.getString());
        }
        else // There is no text cropping
        {
            // If the text can be moved to the right then do so
            const float textWidth = getFullTextWidth();
            if (textWidth > width)
            {
                if (textWidth - m_textCropPosition < width)
                    m_textCropPosition = static_cast<unsigned int>(textWidth - width);
            }
            else
                m_textCropPosition = 0;
        }

        // Set the caret behind the last character
        setCaretPosition(m_textFull.getString().getSize());

        onTextChange.emit(this, m_text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::String& EditBox::getText() const
    {
        return m_text;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setDefaultText(const sf::String& text)
    {
        m_defaultText.setString(text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::String& EditBox::getDefaultText() const
    {
        return m_defaultText.getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::selectText(std::size_t start, std::size_t length)
    {
        m_selStart = start;
        m_selEnd = std::min(m_text.getSize(), start + length);
        updateSelection();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String EditBox::getSelectedText() const
    {
        return m_text.substring(std::min(m_selStart, m_selEnd), std::max(m_selStart, m_selEnd));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setTextSize(unsigned int size)
    {
        // Change the text size
        m_textSize = size;

        // Call setText to re-position the text
        setText(m_text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int EditBox::getTextSize() const
    {
        return m_textFull.getCharacterSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setPasswordCharacter(char passwordChar)
    {
        // Change the password character
        m_passwordChar = passwordChar;

        // Recalculate the text position
        setText(m_text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    char EditBox::getPasswordCharacter() const
    {
        return m_passwordChar;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setMaximumCharacters(unsigned int maxChars)
    {
        // Set the new character limit ( 0 to disable the limit )
        m_maxChars = maxChars;

        // If there is a character limit then check if it is exceeded
        if ((m_maxChars > 0) && (m_textFull.getString().getSize() > m_maxChars))
        {
            sf::String displayedText = m_textFull.getString();

            // Remove all the excess characters
            m_text.erase(m_maxChars, sf::String::InvalidPos);
            displayedText.erase(m_maxChars, sf::String::InvalidPos);

            // If we passed here then the text has changed.
            m_textBeforeSelection.setString(displayedText);
            m_textSelection.setString("");
            m_textAfterSelection.setString("");
            m_textFull.setString(displayedText);

            // Set the caret behind the last character
            setCaretPosition(displayedText.getSize());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int EditBox::getMaximumCharacters() const
    {
        return m_maxChars;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setAlignment(Alignment alignment)
    {
        m_textAlignment = alignment;
        setText(m_text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBox::Alignment EditBox::getAlignment() const
    {
        return m_textAlignment;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::limitTextWidth(bool limitWidth)
    {
        m_limitTextWidth = limitWidth;

        if (!m_fontCached)
            return;

        // Check if the width is being limited
        if (m_limitTextWidth)
        {
            // Delete the last characters when the text no longer fits inside the edit box
            const float width = getVisibleEditBoxWidth();
            while (!m_textFull.getString().isEmpty() && (getFullTextWidth() > width))
            {
                sf::String displayedString = m_textFull.getString();
                displayedString.erase(displayedString.getSize()-1);
                m_textFull.setString(displayedString);
                m_text.erase(m_text.getSize()-1);
            }

            m_textBeforeSelection.setString(m_textFull.getString());

            // There is no clipping
            m_textCropPosition = 0;

            // If the caret was behind the limit, then set it at the end
            if (m_selEnd > m_textFull.getString().getSize())
                setCaretPosition(m_selEnd);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool EditBox::isTextWidthLimited() const
    {
        return m_limitTextWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setReadOnly(bool readOnly)
    {
        m_readOnly = readOnly;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool EditBox::isReadOnly() const
    {
        return m_readOnly;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setCaretPosition(std::size_t charactersBeforeCaret)
    {
        // The caret position has to stay inside the string
        if (charactersBeforeCaret > m_text.getSize())
            charactersBeforeCaret = m_text.getSize();

        // Set the caret to the correct position
        m_selStart = charactersBeforeCaret;
        m_selEnd = charactersBeforeCaret;
        updateSelection();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t EditBox::getCaretPosition() const
    {
        return m_selEnd;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool EditBox::setInputValidator(const std::string& regex)
    {
        try
        {
            m_regex = regex;
        }
        catch (const std::regex_error&)
        {
            return false;
        }

        m_regexString = regex;
        setText(m_text);
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& EditBox::getInputValidator() const
    {
        return m_regexString;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setSuffix(const sf::String& suffix)
    {
        m_textSuffix.setString(suffix);
        recalculateTextPositions();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::String& EditBox::getSuffix() const
    {
        return m_textSuffix.getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setFocused(bool focused)
    {
        if (focused)
        {
            m_caretVisible = true;
            m_animationTimeElapsed = {};
        }
        else // Unfocusing
        {
            // If there is a selection then undo it now
            if (m_selChars)
                setCaretPosition(m_selEnd);
        }

    #if defined (TGUI_SYSTEM_ANDROID) || defined (TGUI_SYSTEM_IOS)
        sf::Keyboard::setVirtualKeyboardVisible(focused);
    #endif

        Widget::setFocused(focused);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool EditBox::mouseOnWidget(Vector2f pos) const
    {
        if (ClickableWidget::mouseOnWidget(pos))
        {
            if (!m_transparentTextureCached || !m_sprite.isTransparentPixel(pos - getPosition() - m_bordersCached.getOffset()))
                return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::leftMousePressed(Vector2f pos)
    {
        pos -= getPosition();

        // Find the caret position
        const float positionX = pos.x - m_bordersCached.getLeft() - m_paddingCached.getLeft();

        std::size_t caretPosition = findCaretPosition(positionX);
        const float textOffset = m_textFull.getExtraHorizontalPadding();

        // When clicking on the left of the first character, move the caret to the left
        if ((positionX < textOffset) && (caretPosition > 0))
            --caretPosition;

        // When clicking on the right of the right character, move the caret to the right
        else if ((positionX > getVisibleEditBoxWidth() - textOffset) && (caretPosition < m_textFull.getString().getSize()))
            ++caretPosition;

        // Check if this is a double click
        if ((m_possibleDoubleClick) && (m_selChars == 0) && (caretPosition == m_selEnd))
        {
            // The next click is going to be a normal one again
            m_possibleDoubleClick = false;

            // Set the caret at the end of the text
            setCaretPosition(m_textFull.getString().getSize());

            // Select the whole text
            m_selStart = 0;
            m_selEnd = m_text.getSize();
            updateSelection();
        }
        else // No double clicking
        {
            // Set the new caret
            setCaretPosition(caretPosition);

            // If the next click comes soon enough then it will be a double click
            m_possibleDoubleClick = true;
        }

        // Set the mouse down flag
        m_mouseDown = true;
        onMousePress.emit(this, pos);

        // The caret should be visible
        m_caretVisible = true;
        m_animationTimeElapsed = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::mouseMoved(Vector2f pos)
    {
        pos -= getPosition();

        if (!m_mouseHover)
            mouseEnteredWidget();

        // The mouse has moved so a double click is no longer possible
        m_possibleDoubleClick = false;

        // Check if the mouse is hold down (we are selecting multiple characters)
        if (m_mouseDown)
        {
            const auto oldSelEnd = m_selEnd;

            // Check if there is a text width limit
            if (m_limitTextWidth)
            {
                // Find out between which characters the mouse is standing
                m_selEnd = findCaretPosition(pos.x - m_bordersCached.getLeft() - m_paddingCached.getLeft());
            }
            else // Scrolling is enabled
            {
                const float width = getVisibleEditBoxWidth();
                const float textOffset = m_textFull.getExtraHorizontalPadding();

                // Check if the mouse is on the left of the text
                if (pos.x < m_bordersCached.getLeft() + m_paddingCached.getLeft() + textOffset)
                {
                    // Move the text by a few pixels
                    if (m_textFull.getCharacterSize() > 10)
                    {
                        if (m_textCropPosition > m_textFull.getCharacterSize() / 10)
                            m_textCropPosition -= static_cast<unsigned int>(m_textFull.getCharacterSize() / 10.f);
                        else
                            m_textCropPosition = 0;
                    }
                    else
                    {
                        if (m_textCropPosition)
                            m_textCropPosition -= 1;
                    }
                }
                // Check if the mouse is on the right of the text AND there is a possibility to scroll
                else if ((pos.x > m_bordersCached.getLeft() + m_paddingCached.getLeft() + width - textOffset) && (getFullTextWidth() > width))
                {
                    // Move the text by a few pixels
                    if (m_textFull.getCharacterSize() > 10)
                    {
                        const float pixelsToMove = m_textFull.getCharacterSize() / 10.f;
                        if (m_textCropPosition + width + pixelsToMove < getFullTextWidth())
                            m_textCropPosition += static_cast<unsigned int>(pixelsToMove);
                        else
                            m_textCropPosition = static_cast<unsigned int>(getFullTextWidth() - width);
                    }
                    else
                    {
                        if (m_textCropPosition + width < getFullTextWidth())
                            m_textCropPosition += 1;
                    }
                }

                // Find out between which characters the mouse is standing
                m_selEnd = findCaretPosition(pos.x - m_bordersCached.getLeft() - m_paddingCached.getLeft());
            }

            if (m_selEnd != oldSelEnd)
                updateSelection();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::keyPressed(const sf::Event::KeyEvent& event)
    {
        if (event.code == sf::Keyboard::Return)
            onReturnKeyPress.emit(this, m_text);
        else if (event.code == sf::Keyboard::BackSpace)
            backspaceKeyPressed();
        else if (event.code == sf::Keyboard::Delete)
            deleteKeyPressed();
        else if (keyboard::isKeyPressCopy(event))
            copySelectedTextToClipboard();
        else if (keyboard::isKeyPressCut(event))
            cutSelectedTextToClipboard();
        else if (keyboard::isKeyPressPaste(event))
            pasteTextFromClipboard();
        else if (keyboard::isKeyPressSelectAll(event))
            selectText();
        else
        {
            bool caretMoved = true;
            if (keyboard::isKeyPressMoveCaretLeft(event))
                moveCaretLeft(event.shift);
            else if (keyboard::isKeyPressMoveCaretRight(event))
                moveCaretRight(event.shift);
            else if (keyboard::isKeyPressMoveCaretWordBegin(event))
                moveCaretWordBegin();
            else if (keyboard::isKeyPressMoveCaretWordEnd(event))
                moveCaretWordEnd();
            else if (keyboard::isKeyPressMoveCaretLineStart(event) || keyboard::isKeyPressMoveCaretUp(event)
                  || keyboard::isKeyPressMoveCaretDocumentBegin(event) || (event.code == sf::Keyboard::PageUp))
                m_selEnd = 0;
            else if (keyboard::isKeyPressMoveCaretLineEnd(event) || keyboard::isKeyPressMoveCaretDown(event)
                  || keyboard::isKeyPressMoveCaretDocumentEnd(event) || (event.code == sf::Keyboard::PageDown))
                m_selEnd = m_text.getSize();
            else
                caretMoved = false;

            if (caretMoved)
            {
                if (!event.shift)
                    m_selStart = m_selEnd;

                updateSelection();
            }
        }

        // The caret should be visible again
        m_caretVisible = true;
        m_animationTimeElapsed = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::textEntered(std::uint32_t key)
    {
        if (m_readOnly)
            return;

        // Only add the character when the regex matches
        if (m_regexString != ".*")
        {
            sf::String text = m_text;

            if (m_selChars == 0)
                text.insert(m_selEnd, key);
            else
            {
                const std::size_t pos = std::min(m_selStart, m_selEnd);
                text.erase(pos, m_selChars);
                text.insert(pos, key);
            }

            // The character has to match the regex
            if (!std::regex_match(text.toAnsiString(), m_regex))
                return;
        }

        // If there are selected characters then delete them first
        if (m_selChars > 0)
            deleteSelectedCharacters();

        // Make sure we don't exceed our maximum characters limit
        if ((m_maxChars > 0) && (m_text.getSize() + 1 > m_maxChars))
            return;

        // Insert our character
        m_text.insert(m_selEnd, key);

        // Change the displayed text
        sf::String displayedText = m_textFull.getString();
        if (m_passwordChar != '\0')
            displayedText.insert(m_selEnd, m_passwordChar);
        else
            displayedText.insert(m_selEnd, key);

        m_textFull.setString(displayedText);

        // When there is a text width limit then reverse what we just did
        if (m_limitTextWidth)
        {
            // Now check if the text fits into the EditBox
            if (getFullTextWidth() > getVisibleEditBoxWidth())
            {
                // If the text does not fit in the EditBox then delete the added character
                m_text.erase(m_selEnd, 1);
                displayedText.erase(m_selEnd, 1);
                m_textFull.setString(displayedText);
                return;
            }
        }

        // Move our caret forward
        setCaretPosition(m_selEnd + 1);

        // The caret should be visible again
        m_caretVisible = true;
        m_animationTimeElapsed = {};

        onTextChange.emit(this, m_text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& EditBox::getSignal(std::string signalName)
    {
        if (signalName == toLower(onTextChange.getName()))
            return onTextChange;
        else if (signalName == toLower(onReturnKeyPress.getName()))
            return onReturnKeyPress;
        else
            return ClickableWidget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            setSize(m_size);
        }
        else if (property == "padding")
        {
            m_paddingCached = getSharedRenderer()->getPadding();
            m_paddingCached.updateParentSize(getSize());

            setText(m_text);

            m_caret.setSize({m_caret.getSize().x, getInnerSize().y - m_paddingCached.getBottom() - m_paddingCached.getTop()});
        }
        else if (property == "caretwidth")
        {
            m_caret.setPosition({m_caret.getPosition().x + ((m_caret.getSize().x - getSharedRenderer()->getCaretWidth()) / 2.0f), m_caret.getPosition().y});
            m_caret.setSize({getSharedRenderer()->getCaretWidth(), getInnerSize().y - m_paddingCached.getBottom() - m_paddingCached.getTop()});
        }
        else if ((property == "textcolor") || (property == "textcolordisabled") || (property == "textcolorfocused"))
        {
            updateTextColor();
        }
        else if (property == "selectedtextcolor")
        {
            m_textSelection.setColor(getSharedRenderer()->getSelectedTextColor());
        }
        else if (property == "defaulttextcolor")
        {
            m_defaultText.setColor(getSharedRenderer()->getDefaultTextColor());
        }
        else if (property == "texture")
        {
            m_sprite.setTexture(getSharedRenderer()->getTexture());
        }
        else if (property == "texturehover")
        {
            m_spriteHover.setTexture(getSharedRenderer()->getTextureHover());
        }
        else if (property == "texturedisabled")
        {
            m_spriteDisabled.setTexture(getSharedRenderer()->getTextureDisabled());
        }
        else if (property == "texturefocused")
        {
            m_spriteFocused.setTexture(getSharedRenderer()->getTextureFocused());
        }
        else if (property == "textstyle")
        {
            const TextStyle style = getSharedRenderer()->getTextStyle();
            m_textBeforeSelection.setStyle(style);
            m_textAfterSelection.setStyle(style);
            m_textSelection.setStyle(style);
            m_textSuffix.setStyle(style);
            m_textFull.setStyle(style);
        }
        else if (property == "defaulttextstyle")
        {
            m_defaultText.setStyle(getSharedRenderer()->getDefaultTextStyle());
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == "bordercolorhover")
        {
            m_borderColorHoverCached = getSharedRenderer()->getBorderColorHover();
        }
        else if (property == "bordercolordisabled")
        {
            m_borderColorDisabledCached = getSharedRenderer()->getBorderColorDisabled();
        }
        else if (property == "bordercolorfocused")
        {
            m_borderColorFocusedCached = getSharedRenderer()->getBorderColorFocused();
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == "backgroundcolorhover")
        {
            m_backgroundColorHoverCached = getSharedRenderer()->getBackgroundColorHover();
        }
        else if (property == "backgroundcolordisabled")
        {
            m_backgroundColorDisabledCached = getSharedRenderer()->getBackgroundColorDisabled();
        }
        else if (property == "backgroundcolorfocused")
        {
            m_backgroundColorFocusedCached = getSharedRenderer()->getBackgroundColorFocused();
        }
        else if (property == "caretcolor")
        {
            m_caretColorCached = getSharedRenderer()->getCaretColor();
        }
        else if (property == "caretcolorhover")
        {
            m_caretColorHoverCached = getSharedRenderer()->getCaretColorHover();
        }
        else if (property == "caretcolorfocused")
        {
            m_caretColorFocusedCached = getSharedRenderer()->getCaretColorFocused();
        }
        else if (property == "selectedtextbackgroundcolor")
        {
            m_selectedTextBackgroundColorCached = getSharedRenderer()->getSelectedTextBackgroundColor();
        }
        else if ((property == "opacity") || (property == "opacitydisabled"))
        {
            Widget::rendererChanged(property);

            m_textBeforeSelection.setOpacity(m_opacityCached);
            m_textAfterSelection.setOpacity(m_opacityCached);
            m_textSelection.setOpacity(m_opacityCached);
            m_defaultText.setOpacity(m_opacityCached);
            m_textSuffix.setOpacity(m_opacityCached);

            m_sprite.setOpacity(m_opacityCached);
            m_spriteHover.setOpacity(m_opacityCached);
            m_spriteDisabled.setOpacity(m_opacityCached);
            m_spriteFocused.setOpacity(m_opacityCached);
        }
        else if (property == "font")
        {
            Widget::rendererChanged(property);

            m_textBeforeSelection.setFont(m_fontCached);
            m_textSelection.setFont(m_fontCached);
            m_textAfterSelection.setFont(m_fontCached);
            m_textSuffix.setFont(m_fontCached);
            m_textFull.setFont(m_fontCached);
            m_defaultText.setFont(m_fontCached);

            // Recalculate the text size and position
            setText(m_text);
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> EditBox::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        if (getAlignment() != EditBox::Alignment::Left)
        {
            if (getAlignment() == EditBox::Alignment::Center)
                node->propertyValuePairs["Alignment"] = std::make_unique<DataIO::ValueNode>("Center");
            else
                node->propertyValuePairs["Alignment"] = std::make_unique<DataIO::ValueNode>("Right");
        }

        if (getInputValidator() != ".*")
        {
            if (getInputValidator() == EditBox::Validator::Int)
                node->propertyValuePairs["InputValidator"] = std::make_unique<DataIO::ValueNode>("Int");
            else if (getInputValidator() == EditBox::Validator::UInt)
                node->propertyValuePairs["InputValidator"] = std::make_unique<DataIO::ValueNode>("UInt");
            else if (getInputValidator() == EditBox::Validator::Float)
                node->propertyValuePairs["InputValidator"] = std::make_unique<DataIO::ValueNode>("Float");
            else
                node->propertyValuePairs["InputValidator"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(sf::String{getInputValidator()}));
        }

        if (!m_text.isEmpty())
            node->propertyValuePairs["Text"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_text));
        if (!getDefaultText().isEmpty())
            node->propertyValuePairs["DefaultText"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(getDefaultText()));
        if (getPasswordCharacter() != '\0')
            node->propertyValuePairs["PasswordCharacter"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(sf::String(getPasswordCharacter())));
        if (getMaximumCharacters() != 0)
            node->propertyValuePairs["MaximumCharacters"] = std::make_unique<DataIO::ValueNode>(to_string(getMaximumCharacters()));
        if (isTextWidthLimited())
            node->propertyValuePairs["TextWidthLimited"] = std::make_unique<DataIO::ValueNode>("true");
        if (isReadOnly())
            node->propertyValuePairs["ReadOnly"] = std::make_unique<DataIO::ValueNode>("true");
        if (!getSuffix().isEmpty())
            node->propertyValuePairs["Suffix"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(getSuffix()));

        node->propertyValuePairs["TextSize"] = std::make_unique<DataIO::ValueNode>(to_string(m_textSize));

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs["text"])
            setText(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["text"]->value).getString());
        if (node->propertyValuePairs["defaulttext"])
            setDefaultText(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["defaulttext"]->value).getString());
        if (node->propertyValuePairs["textsize"])
            setTextSize(strToInt(node->propertyValuePairs["textsize"]->value));
        if (node->propertyValuePairs["maximumcharacters"])
            setMaximumCharacters(strToInt(node->propertyValuePairs["maximumcharacters"]->value));
        if (node->propertyValuePairs["textwidthlimited"])
            limitTextWidth(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["textwidthlimited"]->value).getBool());
        if (node->propertyValuePairs["readonly"])
            setReadOnly(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["readonly"]->value).getBool());
        if (node->propertyValuePairs["suffix"])
            setSuffix(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["suffix"]->value).getString());
        if (node->propertyValuePairs["passwordcharacter"])
        {
            const std::string pass = Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["passwordcharacter"]->value).getString();
            if (!pass.empty())
                setPasswordCharacter(pass[0]);
        }
        if (node->propertyValuePairs["alignment"])
        {
            if (toLower(node->propertyValuePairs["alignment"]->value) == "left")
                setAlignment(EditBox::Alignment::Left);
            else if (toLower(node->propertyValuePairs["alignment"]->value) == "center")
                setAlignment(EditBox::Alignment::Center);
            else if (toLower(node->propertyValuePairs["alignment"]->value) == "right")
                setAlignment(EditBox::Alignment::Right);
            else
                throw Exception{"Failed to parse Alignment property. Only the values Left, Center and Right are correct."};
        }
        if (node->propertyValuePairs["inputvalidator"])
        {
            if (toLower(node->propertyValuePairs["inputvalidator"]->value) == "int")
                setInputValidator(EditBox::Validator::Int);
            else if (toLower(node->propertyValuePairs["inputvalidator"]->value) == "uint")
                setInputValidator(EditBox::Validator::UInt);
            else if (toLower(node->propertyValuePairs["inputvalidator"]->value) == "float")
                setInputValidator(EditBox::Validator::Float);
            else
                setInputValidator(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["inputvalidator"]->value).getString());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float EditBox::getFullTextWidth() const
    {
        return m_textFull.getSize().x + (2 * m_textFull.getExtraHorizontalPadding());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f EditBox::getInnerSize() const
    {
        return {std::max(0.f, getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight()),
                std::max(0.f, getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom())};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float EditBox::getVisibleEditBoxWidth() const
    {
        float extraSuffixWidth = 0;
        if (!m_textSuffix.getString().isEmpty())
        {
            const float textOffset = m_textFull.getExtraHorizontalPadding();
            extraSuffixWidth = m_textSuffix.getSize().x + textOffset;
        }

        return std::max(0.f, getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight() - m_paddingCached.getLeft() - m_paddingCached.getRight() - extraSuffixWidth);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t EditBox::findCaretPosition(float posX)
    {
        // Take the part outside the edit box into account when the text does not fit inside it
        posX += m_textCropPosition;

        if (m_textAlignment == Alignment::Left)
            posX -= m_textFull.getExtraHorizontalPadding();
        else
        {
            // If the text is centered or aligned to the right then the position has to be corrected when the edit box is not entirely full
            const float editBoxWidth = getVisibleEditBoxWidth();
            const float textWidth = getFullTextWidth();
            if (textWidth < editBoxWidth)
            {
                // Set the number of pixels to move
                if (m_textAlignment == Alignment::Center)
                    posX -= (editBoxWidth - textWidth) / 2.f;
                else // if (textAlignment == Alignment::Right)
                    posX -= editBoxWidth - textWidth;
            }
        }

        float width = 0;
        std::uint32_t prevChar = 0;
        const unsigned int textSize = getTextSize();
        const bool bold = (m_textFull.getStyle() & sf::Text::Bold) != 0;

        std::size_t index;
        for (index = 0; index < m_text.getSize(); ++index)
        {
            float charWidth;
            std::uint32_t curChar = m_text[index];
            if (curChar == '\n')
            {
                // This should not happen as edit box is for single line text, but lets try the next line anyway since we haven't found the position yet
                width = 0;
                prevChar = 0;
                continue;
            }
            else if (curChar == '\t')
                charWidth = static_cast<float>(m_fontCached.getGlyph(' ', textSize, bold).advance) * 4;
            else
                charWidth = static_cast<float>(m_fontCached.getGlyph(curChar, textSize, bold).advance);

            const float kerning = m_fontCached.getKerning(prevChar, curChar, textSize);
            if (width + charWidth < posX)
                width += charWidth + kerning;
            else
            {
                // If the mouse is on the second halve of the character then the caret should be on the right of it
                if (width + charWidth - posX < charWidth / 2.f)
                    index++;

                break;
            }

            prevChar = curChar;
        }

        return index;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::deleteSelectedCharacters()
    {
        // Nothing to delete when no text was selected
        if (m_selChars == 0)
            return;

        const std::size_t pos = std::min(m_selStart, m_selEnd);

        // Erase the characters
        sf::String displayedString = m_textFull.getString();
        displayedString.erase(pos, m_selChars);
        m_textFull.setString(displayedString);
        m_text.erase(pos, m_selChars);

        // Set the caret back on the correct position
        setCaretPosition(pos);

        // If the text can be moved to the right then do so
        const float width = getVisibleEditBoxWidth();
        const float textWidth = getFullTextWidth();
        if (textWidth > width)
        {
            if (textWidth - m_textCropPosition < width)
                m_textCropPosition = static_cast<unsigned int>(textWidth - width);
        }
        else
            m_textCropPosition = 0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::recalculateTextPositions()
    {
        // If the edit box is resized then it might happen that text which previously didn't fit will now fit inside it
        if (m_textCropPosition != 0)
        {
            const float textWidth = getFullTextWidth();
            const float maxTextCropPosition = textWidth - getVisibleEditBoxWidth();
            m_textCropPosition = std::min(m_textCropPosition, static_cast<unsigned int>(std::max(0.f, maxTextCropPosition)));
        }

        const float textOffset = m_textFull.getExtraHorizontalPadding();
        float textX = m_paddingCached.getLeft() - m_textCropPosition + textOffset;
        const float textY = m_paddingCached.getTop() + (((getInnerSize().y - m_paddingCached.getBottom() - m_paddingCached.getTop()) - m_textFull.getSize().y) / 2.f);

        // Check if the layout wasn't left
        if (m_textAlignment != Alignment::Left)
        {
            // Calculate the text width
            const float textWidth = m_textFull.getString().isEmpty() ? (m_defaultText.getSize().x + 2 * textOffset) : getFullTextWidth();

            // Check if a layout would make sense
            if (textWidth < getVisibleEditBoxWidth())
            {
                // Put the text on the correct position
                if (m_textAlignment == Alignment::Center)
                    textX += (getVisibleEditBoxWidth() - textWidth) / 2.f;
                else // if (textAlignment == Alignment::Right)
                    textX += getVisibleEditBoxWidth() - textWidth;
            }
        }

        float caretLeft = textX;

        // Set the text before the selection on the correct position
        m_textBeforeSelection.setPosition(textX, textY);
        m_defaultText.setPosition(textX, textY);

        // Check if there is a selection
        if (m_selChars != 0)
        {
            // Watch out for the kerning
            if (m_textBeforeSelection.getString().getSize() > 0)
                textX += m_fontCached.getKerning(m_textFull.getString()[m_textBeforeSelection.getString().getSize() - 1], m_textFull.getString()[m_textBeforeSelection.getString().getSize()], m_textBeforeSelection.getCharacterSize());

            textX += m_textBeforeSelection.findCharacterPos(m_textBeforeSelection.getString().getSize()).x;

            // Set the position and size of the rectangle that gets drawn behind the selected text
            m_selectedTextBackground.setSize({m_textSelection.findCharacterPos(m_textSelection.getString().getSize()).x,
                                              getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()});
            m_selectedTextBackground.setPosition({textX, m_paddingCached.getTop()});

            // Set the text selected text on the correct position
            m_textSelection.setPosition(textX, textY);

            // Watch out for kerning
            if (m_textFull.getString().getSize() > m_textBeforeSelection.getString().getSize() + m_textSelection.getString().getSize())
                textX += m_fontCached.getKerning(m_textFull.getString()[m_textBeforeSelection.getString().getSize() + m_textSelection.getString().getSize() - 1], m_textFull.getString()[m_textBeforeSelection.getString().getSize() + m_textSelection.getString().getSize()], m_textBeforeSelection.getCharacterSize());

            // Set the text selected text on the correct position
            textX += m_textSelection.findCharacterPos(m_textSelection.getString().getSize()).x;
            m_textAfterSelection.setPosition(textX, textY);
        }

        // Set the position of the caret
        caretLeft += m_textFull.findCharacterPos(m_selEnd).x - (m_caret.getSize().x * 0.5f);
        m_caret.setPosition({caretLeft, m_paddingCached.getTop()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::updateSelection()
    {
        // Check if we are selecting text from left to right
        if (m_selEnd > m_selStart)
        {
            // Update the number of characters that are selected
            m_selChars = m_selEnd - m_selStart;

            // Change our three texts
            m_textBeforeSelection.setString(m_textFull.getString().substring(0, m_selStart));
            m_textSelection.setString(m_textFull.getString().substring(m_selStart, m_selChars));
            m_textAfterSelection.setString(m_textFull.getString().substring(m_selEnd));
        }
        else if (m_selEnd < m_selStart)
        {
            // Update the number of characters that are selected
            m_selChars = m_selStart - m_selEnd;

            // Change our three texts
            m_textBeforeSelection.setString(m_textFull.getString().substring(0, m_selEnd));
            m_textSelection.setString(m_textFull.getString().substring(m_selEnd, m_selChars));
            m_textAfterSelection.setString(m_textFull.getString().substring(m_selStart));
        }
        else
        {
            // Update the number of characters that are selected
            m_selChars = 0;

            // Change our three texts
            m_textBeforeSelection.setString(m_textFull.getString());
            m_textSelection.setString("");
            m_textAfterSelection.setString("");
        }

        if (!m_fontCached)
            return;

        // Check if scrolling is enabled
        if (!m_limitTextWidth)
        {
            // Find out the position of the caret
            const float caretPosition = m_textFull.findCharacterPos(m_selEnd).x;

            // If the caret is too far on the right then adjust the cropping
            if (m_textCropPosition + getVisibleEditBoxWidth() - (2 * m_textFull.getExtraHorizontalPadding()) < caretPosition)
                m_textCropPosition = static_cast<unsigned int>(caretPosition - getVisibleEditBoxWidth() + (2 * m_textFull.getExtraHorizontalPadding()));

            // If the caret is too far on the left then adjust the cropping
            if (m_textCropPosition > caretPosition)
                m_textCropPosition = static_cast<unsigned int>(caretPosition);
        }

        recalculateTextPositions();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::updateTextColor()
    {
        if (!m_enabled && getSharedRenderer()->getTextColorDisabled().isSet())
        {
            m_textBeforeSelection.setColor(getSharedRenderer()->getTextColorDisabled());
            m_textAfterSelection.setColor(getSharedRenderer()->getTextColorDisabled());
            m_textSuffix.setColor(getSharedRenderer()->getTextColorDisabled());
        }
        else if (m_focused && getSharedRenderer()->getTextColorFocused().isSet())
        {
            m_textBeforeSelection.setColor(getSharedRenderer()->getTextColorFocused());
            m_textAfterSelection.setColor(getSharedRenderer()->getTextColorFocused());
            m_textSuffix.setColor(getSharedRenderer()->getTextColorFocused());
        }
        else
        {
            m_textBeforeSelection.setColor(getSharedRenderer()->getTextColor());
            m_textAfterSelection.setColor(getSharedRenderer()->getTextColor());
            m_textSuffix.setColor(getSharedRenderer()->getTextColor());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool EditBox::update(sf::Time elapsedTime)
    {
        bool screenRefreshRequired = Widget::update(elapsedTime);

        // Only show/hide the caret every half second
        if (m_animationTimeElapsed >= sf::milliseconds(getEditCursorBlinkRate()))
        {
            // Reset the elapsed time
            m_animationTimeElapsed = {};

            // Switch the value of the visible flag
            m_caretVisible = !m_caretVisible;

            // Too slow for double clicking
            m_possibleDoubleClick = false;

            screenRefreshRequired = true;
        }

        return screenRefreshRequired;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::backspaceKeyPressed()
    {
        if (m_readOnly)
            return;

        // Make sure that we did not select any characters
        if (m_selChars == 0)
        {
            // We can't delete any characters when you are at the beginning of the string
            if (m_selEnd == 0)
                return;

            // Erase the character
            sf::String displayedString = m_textFull.getString();
            displayedString.erase(m_selEnd-1, 1);
            m_textFull.setString(displayedString);
            m_text.erase(m_selEnd-1, 1);

            // Set the caret back on the correct position
            setCaretPosition(m_selEnd - 1);

            const float width = getVisibleEditBoxWidth();

            // If the text can be moved to the right then do so
            const float textWidth = getFullTextWidth();
            if (textWidth > width)
            {
                if (textWidth - m_textCropPosition < width)
                    m_textCropPosition = static_cast<unsigned int>(textWidth - width);
            }
            else
                m_textCropPosition = 0;
        }
        else // When you did select some characters, delete them
            deleteSelectedCharacters();

        onTextChange.emit(this, m_text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::deleteKeyPressed()
    {
        if (m_readOnly)
            return;

        // Make sure that no text is selected
        if (m_selChars == 0)
        {
            // When the caret is at the end of the line then you can't delete anything
            if (m_selEnd == m_text.getSize())
                return;

            // Erase the character
            sf::String displayedString = m_textFull.getString();
            displayedString.erase(m_selEnd, 1);
            m_textFull.setString(displayedString);
            m_text.erase(m_selEnd, 1);

            // Set the caret back on the correct position
            setCaretPosition(m_selEnd);

            // If the text can be moved to the right then do so
            const float width = getVisibleEditBoxWidth();
            const float textWidth = getFullTextWidth();
            if (textWidth > width)
            {
                if (textWidth - m_textCropPosition < width)
                    m_textCropPosition = static_cast<unsigned int>(textWidth - width);
            }
            else
                m_textCropPosition = 0;
        }
        else // You did select some characters, delete them
            deleteSelectedCharacters();

        onTextChange.emit(this, m_text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::copySelectedTextToClipboard()
    {
        Clipboard::set(m_textSelection.getString());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::cutSelectedTextToClipboard()
    {
        Clipboard::set(m_textSelection.getString());

        if (m_readOnly)
            return;

        deleteSelectedCharacters();

        onTextChange.emit(this, m_text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::pasteTextFromClipboard()
    {
        if (m_readOnly)
            return;

        // Only continue pasting if you actually have to do something
        const auto clipboardContents = Clipboard::get();
        if ((m_selChars > 0) || (clipboardContents.getSize() > 0))
        {
            deleteSelectedCharacters();

            const std::size_t oldCaretPos = m_selEnd;

            if (m_text.getSize() > m_selEnd)
                setText(m_text.toWideString().substr(0, m_selEnd) + Clipboard::get() + m_text.toWideString().substr(m_selEnd, m_text.getSize() - m_selEnd));
            else
                setText(m_text + clipboardContents);

            setCaretPosition(oldCaretPos + clipboardContents.getSize());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::moveCaretLeft(bool shiftPressed)
    {
        // If text is selected then move to the cursor to the left side of the selected text
        if ((m_selChars > 0) && !shiftPressed)
            m_selEnd = std::min(m_selStart, m_selEnd);
        else if (m_selEnd > 0)
            m_selEnd--;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::moveCaretRight(bool shiftPressed)
    {
        // If text is selected then move to the cursor to the right side of the selected text
        if ((m_selChars > 0) && !shiftPressed)
            m_selEnd = std::max(m_selStart, m_selEnd);
        else if (m_selEnd < m_text.getSize())
            m_selEnd++;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::moveCaretWordBegin()
    {
        // Move to the beginning of the word (or to the beginning of the previous word when already at the beginning)
        bool done = false;
        bool skippedWhitespace = false;
        for (std::size_t i = m_selEnd; i > 0; --i)
        {
            if (skippedWhitespace)
            {
                if (isWhitespace(m_text[i-1]))
                {
                    m_selEnd = i;
                    done = true;
                    break;
                }
            }
            else
            {
                if (!isWhitespace(m_text[i-1]))
                    skippedWhitespace = true;
            }
        }

        if (!done && skippedWhitespace)
            m_selEnd = 0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::moveCaretWordEnd()
    {
        // Move to the end of the word (or to the end of the next word when already at the end)
        bool done = false;
        bool skippedWhitespace = false;
        for (std::size_t i = m_selEnd; i < m_text.getSize(); ++i)
        {
            if (skippedWhitespace)
            {
                if (isWhitespace(m_text[i]))
                {
                    m_selEnd = i;
                    done = true;
                    break;
                }
            }
            else
            {
                if (!isWhitespace(m_text[i]))
                    skippedWhitespace = true;
            }
        }

        if (!done && skippedWhitespace)
            m_selEnd = m_text.getSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            if (!m_enabled && m_borderColorDisabledCached.isSet())
                drawBorders(target, states, m_bordersCached, getSize(), m_borderColorDisabledCached);
            else if (m_mouseHover && m_borderColorHoverCached.isSet())
                drawBorders(target, states, m_bordersCached, getSize(), m_borderColorHoverCached);
            else if (m_focused && m_borderColorFocusedCached.isSet())
                drawBorders(target, states, m_bordersCached, getSize(), m_borderColorFocusedCached);
            else
                drawBorders(target, states, m_bordersCached, getSize(), m_borderColorCached);

            states.transform.translate(m_bordersCached.getOffset());
        }

        // Draw the background
        if (m_sprite.isSet())
        {
            if (!m_enabled && m_spriteDisabled.isSet())
                m_spriteDisabled.draw(target, states);
            else if (m_mouseHover && m_spriteHover.isSet())
                m_spriteHover.draw(target, states);
            else if (m_focused && m_spriteFocused.isSet())
                m_spriteFocused.draw(target, states);
            else
                m_sprite.draw(target, states);
        }
        else // There is no background texture
        {
            if (!m_enabled && m_backgroundColorDisabledCached.isSet())
                drawRectangleShape(target, states, getInnerSize(), m_backgroundColorDisabledCached);
            else if (m_mouseHover && m_backgroundColorHoverCached.isSet())
                drawRectangleShape(target, states, getInnerSize(), m_backgroundColorHoverCached);
            else if (m_focused && m_backgroundColorFocusedCached.isSet())
                drawRectangleShape(target, states, getInnerSize(), m_backgroundColorFocusedCached);
            else
                drawRectangleShape(target, states, getInnerSize(), m_backgroundColorCached);
        }

        // Draw the suffix
        float suffixSpace = 0;
        if (!m_textSuffix.getString().isEmpty())
        {
            const Clipping clipping{target, states, {m_paddingCached.getLeft(), m_paddingCached.getTop()}, {getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(), getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}};

            const float textOffset = m_textFull.getExtraHorizontalPadding();
            Vector2f offset{getInnerSize().x - m_paddingCached.getRight() - textOffset - m_textSuffix.getSize().x,
                            m_paddingCached.getTop() + ((getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom() - m_textSuffix.getSize().y) / 2.f)};

            states.transform.translate(offset);
            m_textSuffix.draw(target, states);
            states.transform.translate(-offset);

            suffixSpace = m_textSuffix.getSize().x + textOffset;
        }

        // Draw the text
        {
            const Clipping clipping{target, states, {m_paddingCached.getLeft(), m_paddingCached.getTop()}, {getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight() - suffixSpace, getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}};

            if ((m_textBeforeSelection.getString() != "") || (m_textSelection.getString() != ""))
            {
                m_textBeforeSelection.draw(target, states);

                if (m_textSelection.getString() != "")
                {
                    states.transform.translate(m_selectedTextBackground.getPosition());
                    drawRectangleShape(target, states, m_selectedTextBackground.getSize(), m_selectedTextBackgroundColorCached);
                    states.transform.translate(-m_selectedTextBackground.getPosition());

                    m_textSelection.draw(target, states);
                    m_textAfterSelection.draw(target, states);
                }
            }
            else if (m_defaultText.getString() != "")
            {
                m_defaultText.draw(target, states);
            }
        }

        // Draw the caret
        states.transform.translate(m_caret.getPosition());
        if (m_enabled && m_focused && m_caretVisible)
        {
            if (m_mouseHover && m_caretColorHoverCached.isSet())
                drawRectangleShape(target, states, m_caret.getSize(), m_caretColorHoverCached);
            else if (m_focused && m_caretColorFocusedCached.isSet())
                drawRectangleShape(target, states, m_caret.getSize(), m_caretColorFocusedCached);
            else
                drawRectangleShape(target, states, m_caret.getSize(), m_caretColorCached);
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


#include <TGUI/Widgets/VerticalLayout.hpp>
#include <numeric>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    VerticalLayout::VerticalLayout(const Layout2d& size) :
        BoxLayoutRatios{size}
    {
        m_type = "VerticalLayout";

        m_renderer = aurora::makeCopied<BoxLayoutRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    VerticalLayout::Ptr VerticalLayout::create(const Layout2d& size)
    {
        return std::make_shared<VerticalLayout>(size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    VerticalLayout::Ptr VerticalLayout::copy(VerticalLayout::ConstPtr layout)
    {
        if (layout)
            return std::static_pointer_cast<VerticalLayout>(layout->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void VerticalLayout::updateWidgets()
    {
        const float totalSpaceBetweenWidgets = (m_spaceBetweenWidgetsCached * m_widgets.size()) - m_spaceBetweenWidgetsCached;
        const Vector2f contentSize = {getSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(),
                                          getSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()};

        const float totalRatio = std::accumulate(m_ratios.begin(), m_ratios.end(), 0.f);

        float currentOffset = 0;
        for (std::size_t i = 0; i < m_widgets.size(); ++i)
        {
            auto& widget = m_widgets[i];
            const float height = (contentSize.y - totalSpaceBetweenWidgets) * (m_ratios[i] / totalRatio);

            widget->setSize({contentSize.x, height});
            widget->setPosition({0, currentOffset});

            // Correct the size for widgets that are bigger than what you set (e.g. have borders around it or a text next to them)
            if (widget->getFullSize() != widget->getSize())
            {
                const Vector2f newSize = widget->getSize() - (widget->getFullSize() - widget->getSize());
                if (newSize.x > 0 && newSize.y > 0)
                {
                    widget->setSize(newSize);
                    widget->setPosition(widget->getPosition() - widget->getWidgetOffset());
                }
            }

            currentOffset += height + m_spaceBetweenWidgetsCached;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
