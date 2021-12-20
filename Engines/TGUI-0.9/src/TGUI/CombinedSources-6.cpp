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


#include <TGUI/Filesystem.hpp>
#include <cstdlib> // getenv
#if !defined(TGUI_SYSTEM_WINDOWS)
    #include <unistd.h> // getuid
    #include <pwd.h> // getpwuid
#endif

#if !defined(TGUI_USE_STD_FILESYSTEM)
    #if defined(TGUI_SYSTEM_WINDOWS)
        #ifndef NOMINMAX // MinGW already defines this which causes a warning without this check
            #define NOMINMAX
        #endif
        #define NOMB
        #define VC_EXTRALEAN
        #define WIN32_LEAN_AND_MEAN
        #include <windows.h>
    #else
        #include <sys/types.h>
        #include <sys/stat.h>
        #include <errno.h>
    #endif
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Filesystem::Path::Path(const String& path)
#ifdef TGUI_USE_STD_FILESYSTEM
        : m_path(std::u32string(path))
#endif
    {
#if !defined(TGUI_USE_STD_FILESYSTEM)
        String part;
        for (const char32_t c : path)
        {
    #if defined(TGUI_SYSTEM_WINDOWS)
            if ((c == '/') || (c == '\\'))
    #else
            if (c == '/')
    #endif
            {
                // If this is the first part then check if we are parsing an absolute path
                if (m_parts.empty() && !m_absolute && m_root.empty())
                {
    #if defined(TGUI_SYSTEM_WINDOWS)
                    if ((part.empty()) || ((part.length() == 2) && (part[1] == U':')))
                    {
                        m_root = part;
                        m_absolute = true;
                    }
    #else
                    if (part.empty())
                        m_absolute = true;
    #endif
                    else // The path is relative, there is no root part
                        m_parts.push_back(part);
                }
                else // There have already been other parts, so just add this one
                {
                    if (!part.empty())
                        m_parts.push_back(part);
                }

                part.clear();
            }
            else
                part.push_back(c);
        }

        if (!part.empty())
            m_parts.push_back(part);
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Filesystem::Path::isEmpty() const
    {
#ifdef TGUI_USE_STD_FILESYSTEM
        return m_path.empty();
#else
        return !m_absolute && m_root.empty() && m_parts.empty();
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String Filesystem::Path::asString() const
    {
#ifdef TGUI_USE_STD_FILESYSTEM
        return m_path.generic_u32string();
#else
        String str = m_root;
        if (m_absolute)
            str += U'/';

        if (!m_parts.empty())
            str += m_parts[0];

        for (unsigned int i = 1; i < m_parts.size(); ++i)
            str += U'/' + m_parts[i];

        return str;
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Filesystem::Path Filesystem::Path::getParentPath() const
    {
#ifdef TGUI_USE_STD_FILESYSTEM
        Path parentPath;
        parentPath.m_path = m_path.parent_path();
#else
        Path parentPath(*this);
        if (!parentPath.m_parts.empty())
            parentPath.m_parts.pop_back();
#endif
        return parentPath;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef TGUI_SYSTEM_WINDOWS
    std::wstring Filesystem::Path::asNativeString() const
    {
    #ifdef TGUI_USE_STD_FILESYSTEM
        return m_path.wstring();
    #else
        String str = m_root;
        if (m_absolute)
            str += U'\\';

        if (!m_parts.empty())
            str += m_parts[0];

        for (unsigned int i = 1; i < m_parts.size(); ++i)
            str += U'\\' + m_parts[i];

        return std::wstring(str);

    #endif
    }
#else
    std::string Filesystem::Path::asNativeString() const
    {
    #ifdef TGUI_USE_STD_FILESYSTEM
        return m_path.generic_string();
    #else
        return std::string(asString());
    #endif
    }
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Filesystem::Path Filesystem::Path::operator/(const Path& path) const
    {
        return Path(*this) /= path;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Filesystem::Path& Filesystem::Path::operator/=(const Path& path)
    {
#ifdef TGUI_USE_STD_FILESYSTEM
        m_path /= path.m_path;
#else
        if (!path.m_absolute)
            m_parts.insert(m_parts.end(), path.m_parts.begin(), path.m_parts.end());
        else // The path to append is absolute, so just replace the current path
            *this = path;
#endif
        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Filesystem::directoryExists(const Path& path)
    {
#ifdef TGUI_USE_STD_FILESYSTEM
        return std::filesystem::is_directory(path);
#elif defined(TGUI_SYSTEM_WINDOWS)
        const DWORD attrib = GetFileAttributesW(path.asNativeString().c_str());
        return ((attrib != INVALID_FILE_ATTRIBUTES) && (attrib & FILE_ATTRIBUTE_DIRECTORY));
#else
        struct stat fileInfo;
        return (stat(path.asNativeString().c_str(), &fileInfo) == 0) && (fileInfo.st_mode & S_IFDIR);
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Filesystem::fileExists(const Path& path)
    {
#ifdef TGUI_USE_STD_FILESYSTEM
        return std::filesystem::exists(path);
#elif defined(TGUI_SYSTEM_WINDOWS)
        const DWORD attrib = GetFileAttributesW(path.asNativeString().c_str());
        return attrib != INVALID_FILE_ATTRIBUTES;
#else
        struct stat fileInfo;
        return stat(path.asNativeString().c_str(), &fileInfo) == 0;
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Filesystem::createDirectory(const Path& path)
    {
#ifdef TGUI_USE_STD_FILESYSTEM
        return std::filesystem::create_directory(path);
#elif defined(TGUI_SYSTEM_WINDOWS)
        const DWORD status = CreateDirectoryW(path.asNativeString().c_str(), NULL);
        return (status != 0) || (GetLastError() == ERROR_ALREADY_EXISTS);
#else
        const int status = mkdir(path.asNativeString().c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
        return (status == 0) || (errno == EEXIST);
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Filesystem::Path Filesystem::getLocalDataDirectory()
    {
        tgui::Filesystem::Path localDataDir;
#ifdef TGUI_SYSTEM_WINDOWS
    #if defined (_MSC_VER)
        wchar_t* appDataDir;
        size_t len;
        if ((_wdupenv_s(&appDataDir, &len, L"LOCALAPPDATA") == 0) && appDataDir)
        {
            localDataDir = tgui::Filesystem::Path(std::wstring(appDataDir));
            free(appDataDir);
        }
    #else
        const char* appDataDir = std::getenv("LOCALAPPDATA");
        if (appDataDir)
            localDataDir = tgui::Filesystem::Path(appDataDir);
    #endif
#else
        const char* homeDir = std::getenv("HOME");
        if (!homeDir)
            homeDir = getpwuid(getuid())->pw_dir;
        if (homeDir)
        {
    #ifdef TGUI_SYSTEM_MACOS
            localDataDir = tgui::Filesystem::Path(homeDir) / U"Library" / U"Application Support";
    #else
            localDataDir = tgui::Filesystem::Path(homeDir) / U".local" / U"share";
    #endif
        }
#endif

        return localDataDir;
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


#include <TGUI/Renderers/BoxLayoutRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BoxLayoutRenderer::setSpaceBetweenWidgets(float distance)
    {
        setProperty("spacebetweenwidgets", ObjectConverter{distance});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BoxLayoutRenderer::getSpaceBetweenWidgets() const
    {
        auto it = m_data->propertyValuePairs.find("spacebetweenwidgets");
        if (it != m_data->propertyValuePairs.end())
            return it->second.getNumber();
        else
        {
            it = m_data->propertyValuePairs.find("padding");
            if (it != m_data->propertyValuePairs.end())
            {
                const Padding padding = it->second.getOutline();
                return std::max(std::min(padding.getLeft(), padding.getRight()), std::min(padding.getTop(), padding.getBottom()));
            }
            else
                return 0;
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


#include <TGUI/Renderers/TextBoxRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_OUTLINE(TextBoxRenderer, Borders)
    TGUI_RENDERER_PROPERTY_OUTLINE(TextBoxRenderer, Padding)

    TGUI_RENDERER_PROPERTY_COLOR(TextBoxRenderer, BackgroundColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(TextBoxRenderer, TextColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(TextBoxRenderer, DefaultTextColor, Color(160, 160, 160))
    TGUI_RENDERER_PROPERTY_COLOR(TextBoxRenderer, SelectedTextColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(TextBoxRenderer, SelectedTextBackgroundColor, Color(0, 110, 255))
    TGUI_RENDERER_PROPERTY_COLOR(TextBoxRenderer, BorderColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(TextBoxRenderer, CaretColor, Color::Black)

    TGUI_RENDERER_PROPERTY_TEXTURE(TextBoxRenderer, TextureBackground)

    TGUI_RENDERER_PROPERTY_NUMBER(TextBoxRenderer, CaretWidth, 1)

    TGUI_RENDERER_PROPERTY_RENDERER(TextBoxRenderer, Scrollbar, "scrollbar")
    TGUI_RENDERER_PROPERTY_NUMBER(TextBoxRenderer, ScrollbarWidth, 0)
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


#include <TGUI/Widgets/MessageBox.hpp>
#include <TGUI/SignalImpl.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox::MessageBox()
    {
        m_type = "MessageBox";

        m_renderer = aurora::makeCopied<MessageBoxRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setTitleButtons(ChildWindow::TitleButton::None);
        setTextSize(getGlobalTextSize());

        add(m_label, "#TGUI_INTERNAL$MessageBoxText#");
        m_label->setTextSize(m_textSize);

        setSize({400, 150});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox::MessageBox(const MessageBox& other) :
        ChildWindow      {other},
        onButtonPress    {other.onButtonPress},
        m_loadedThemeFile{other.m_loadedThemeFile},
        m_buttonClassName{other.m_buttonClassName}
    {
        identifyLabelAndButtons();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox::MessageBox(MessageBox&& other) :
        ChildWindow      {std::move(other)},
        onButtonPress    {std::move(other.onButtonPress)},
        m_loadedThemeFile{std::move(other.m_loadedThemeFile)},
        m_buttonClassName{std::move(other.m_buttonClassName)},
        m_buttons        {std::move(other.m_buttons)},
        m_label          {std::move(other.m_label)}
    {
        for (auto& button : m_buttons)
        {
            button->disconnectAll("Pressed");
            button->connect("Pressed", TGUI_LAMBDA_CAPTURE_EQ_THIS{ onButtonPress.emit(this, button->getText()); });
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox& MessageBox::operator= (const MessageBox& other)
    {
        if (this != &other)
        {
            MessageBox temp(other);
            ChildWindow::operator=(temp);

            std::swap(onButtonPress,     temp.onButtonPress);
            std::swap(m_loadedThemeFile, temp.m_loadedThemeFile);
            std::swap(m_buttonClassName, temp.m_buttonClassName);
            std::swap(m_buttons,         temp.m_buttons);
            std::swap(m_label,           temp.m_label);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox& MessageBox::operator= (MessageBox&& other)
    {
        if (this != &other)
        {
            ChildWindow::operator=(std::move(other));

            onButtonPress     = std::move(other.onButtonPress);
            m_loadedThemeFile = std::move(other.m_loadedThemeFile);
            m_buttonClassName = std::move(other.m_buttonClassName);
            m_buttons         = std::move(other.m_buttons);
            m_label           = std::move(other.m_label);

            for (auto& button : m_buttons)
            {
                button->disconnectAll("Pressed");
                button->connect("Pressed", TGUI_LAMBDA_CAPTURE_EQ_THIS{ onButtonPress.emit(this, button->getText()); });
            }
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox::Ptr MessageBox::create(sf::String title, sf::String text, std::vector<sf::String> buttons)
    {
        auto messageBox = std::make_shared<MessageBox>();
        messageBox->setTitle(title);
        messageBox->setText(text);
        for (auto& buttonText : buttons)
            messageBox->addButton(std::move(buttonText));

        return messageBox;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox::Ptr MessageBox::copy(MessageBox::ConstPtr messageBox)
    {
        if (messageBox)
            return std::static_pointer_cast<MessageBox>(messageBox->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBoxRenderer* MessageBox::getSharedRenderer()
    {
        return aurora::downcast<MessageBoxRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const MessageBoxRenderer* MessageBox::getSharedRenderer() const
    {
        return aurora::downcast<const MessageBoxRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBoxRenderer* MessageBox::getRenderer()
    {
        return aurora::downcast<MessageBoxRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const MessageBoxRenderer* MessageBox::getRenderer() const
    {
        return aurora::downcast<const MessageBoxRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::setText(const sf::String& text)
    {
        m_label->setText(text);

        rearrange();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::String& MessageBox::getText() const
    {
        return m_label->getText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::setTextSize(unsigned int size)
    {
        m_textSize = size;

        m_label->setTextSize(size);

        for (auto& button : m_buttons)
            button->setTextSize(m_textSize);

        rearrange();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::addButton(const sf::String& caption)
    {
        auto button = Button::create(caption);
        button->setRenderer(getSharedRenderer()->getButton());
        button->setTextSize(m_textSize);
        button->connect("Pressed", TGUI_LAMBDA_CAPTURE_EQ_THIS{ onButtonPress.emit(this, caption); });

        add(button, "#TGUI_INTERNAL$MessageBoxButton:" + caption + "#");
        m_buttons.push_back(button);

        rearrange();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<sf::String> MessageBox::getButtons() const
    {
        std::vector<sf::String> buttonTexts;
        for (auto& button : m_buttons)
            buttonTexts.emplace_back(button->getText());

        return buttonTexts;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::rearrange()
    {
        float buttonWidth = 120;
        float buttonHeight = 24;

        // Calculate the button size
        if (m_fontCached)
        {
            buttonWidth = 4.0f * Text::getLineHeight(m_fontCached, m_textSize);
            buttonHeight = Text::getLineHeight(m_fontCached, m_textSize) * 1.25f;

            for (const auto& button : m_buttons)
            {
                const float width = sf::Text(button->getText(), *m_fontCached.getFont(), m_textSize).getLocalBounds().width;
                if (buttonWidth < width * 10.0f / 9.0f)
                    buttonWidth = width * 10.0f / 9.0f;
            }
        }

        // Calculate the space needed for the buttons
        const float distance = buttonHeight * 2.0f / 3.0f;
        float buttonsAreaWidth = distance;
        for (auto& button : m_buttons)
        {
            button->setSize({buttonWidth, buttonHeight});
            buttonsAreaWidth += button->getSize().x + distance;
        }

        // Calculate the suggested size of the window
        Vector2f size = {2*distance + m_label->getSize().x, 3*distance + m_label->getSize().y + buttonHeight};

        // Make sure the buttons fit inside the message box
        if (buttonsAreaWidth > size.x)
            size.x = buttonsAreaWidth;

        // Set the size of the window
        setSize(size);

        // Set the text on the correct position
        m_label->setPosition({distance, distance});

        // Set the buttons on the correct position
        float leftPosition = 0;
        const float topPosition = 2*distance + m_label->getSize().y;
        for (auto& button : m_buttons)
        {
            leftPosition += distance + ((size.x - buttonsAreaWidth) / (m_buttons.size()+1));
            button->setPosition({leftPosition, topPosition});
            leftPosition += button->getSize().x;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& MessageBox::getSignal(std::string signalName)
    {
        if (signalName == toLower(onButtonPress.getName()))
            return onButtonPress;
        else
            return ChildWindow::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::rendererChanged(const std::string& property)
    {
        if (property == "textcolor")
        {
            m_label->getRenderer()->setTextColor(getSharedRenderer()->getTextColor());
        }
        else if (property == "button")
        {
            const auto& renderer = getSharedRenderer()->getButton();
            for (auto& button : m_buttons)
                button->setRenderer(renderer);
        }
        else if (property == "font")
        {
            ChildWindow::rendererChanged(property);

            m_label->setInheritedFont(m_fontCached);

            for (auto& button : m_buttons)
                button->setInheritedFont(m_fontCached);

            rearrange();
        }
        else
            ChildWindow::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> MessageBox::save(SavingRenderersMap& renderers) const
    {
        auto node = ChildWindow::save(renderers);
        node->propertyValuePairs["TextSize"] = std::make_unique<DataIO::ValueNode>(to_string(m_textSize));
        // Label and buttons are saved indirectly by saving the child window
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        // Remove the label that the MessageBox constructor creates because it will be created when loading the child window
        removeAllWidgets();

        ChildWindow::load(node, renderers);

        if (node->propertyValuePairs["textsize"])
            setTextSize(strToInt(node->propertyValuePairs["textsize"]->value));

        identifyLabelAndButtons();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::identifyLabelAndButtons()
    {
        m_label = get<Label>("#TGUI_INTERNAL$MessageBoxText#");

        for (unsigned int i = 0; i < m_widgets.size(); ++i)
        {
            if ((m_widgets[i]->getWidgetName().getSize() >= 32) && (m_widgets[i]->getWidgetName().substring(0, 32) == "#TGUI_INTERNAL$MessageBoxButton:"))
            {
                auto button = std::dynamic_pointer_cast<Button>(m_widgets[i]);

                button->disconnectAll("Pressed");
                button->connect("Pressed", TGUI_LAMBDA_CAPTURE_EQ_THIS{ onButtonPress.emit(this, button->getText()); });
                m_buttons.push_back(button);
            }
        }

        rearrange();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
