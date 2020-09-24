/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2019 Bruno Van de Velde (vdv_b@tgui.eu)
//                         Koushtav Chakrabarty (koushtav@fleptic.eu)
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


#include <TGUI/Clipboard.hpp>

#if SFML_VERSION_MAJOR > 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR >= 5)
    #include <SFML/Window/Clipboard.hpp>
#else
    #ifdef SFML_SYSTEM_WINDOWS
        #define NOMB
        #define NOMINMAX
        #define VC_EXTRALEAN
        #define WIN32_LEAN_AND_MEAN
        #include <windows.h>
    #endif
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR < 5
    sf::String Clipboard::m_contents;
    sf::WindowHandle Clipboard::m_windowHandle = sf::WindowHandle();
    bool Clipboard::m_isWindowHandleSet = false;
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String Clipboard::get()
    {
#if SFML_VERSION_MAJOR > 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR >= 5)
        return sf::Clipboard::getString();
#else
    #ifdef SFML_SYSTEM_WINDOWS
        if (m_isWindowHandleSet)
        {
            if (IsClipboardFormatAvailable(CF_TEXT) && OpenClipboard(m_windowHandle))
            {
                HGLOBAL hGlobal = GetClipboardData(CF_TEXT);
                if (hGlobal != NULL)
                {
                    const char* lpszData = static_cast<const char*>(GlobalLock(hGlobal));
                    if (lpszData != NULL)
                    {
                        m_contents = lpszData;

                        GlobalUnlock(hGlobal);
                    }
                }

                CloseClipboard();
            }
        }
    #endif

        return m_contents;
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Clipboard::set(const sf::String& contents)
    {
#if SFML_VERSION_MAJOR > 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR >= 5)
        sf::Clipboard::setString(contents);
#else
        m_contents = contents;

    #ifdef SFML_SYSTEM_WINDOWS
        if (m_isWindowHandleSet)
        {
            if (OpenClipboard(m_windowHandle))
            {
                EmptyClipboard();

                HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, m_contents.getSize() + 1);
                if (hGlobal != NULL)
                {
                    char* pchData = static_cast<char*>(GlobalLock(hGlobal));
                    if (pchData != NULL)
                    {
                        memcpy(pchData, m_contents.toAnsiString().c_str(), m_contents.getSize() + 1);
                        SetClipboardData(CF_TEXT, hGlobal);

                        GlobalUnlock(hGlobal);
                    }

                    GlobalFree(hGlobal);
                }

                CloseClipboard();
            }
        }
    #endif
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR < 5
    void Clipboard::setWindowHandle(const sf::WindowHandle& windowHandle)
    {
        m_windowHandle = windowHandle;
        m_isWindowHandleSet = true;
    }
#endif
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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


#include <TGUI/Color.hpp>
#include <TGUI/Loading/Deserializer.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    const Color Color::Black      {  0,   0,   0};
    const Color Color::White      {255, 255, 255};
    const Color Color::Red        {255,   0,   0};
    const Color Color::Green      {  0, 255,   0};
    const Color Color::Blue       {  0,   0, 255};
    const Color Color::Yellow     {255, 255,   0};
    const Color Color::Magenta    {255,   0, 255};
    const Color Color::Cyan       {  0, 255, 255};
    const Color Color::Transparent{  0,   0,   0,   0};

    const std::map<std::string, Color> Color::colorMap =
    {
        {"black", Color::Black},
        {"white", Color::White},
        {"red", Color::Red},
        {"yellow", Color::Yellow},
        {"green", Color::Green},
        {"cyan", Color::Cyan},
        {"blue", Color::Blue},
        {"magenta", Color::Magenta},
        {"transparent", Color::Transparent}
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Color::Color(const sf::String& string) :
        Color{Deserializer::deserialize(ObjectConverter::Type::Color, string).getColor()}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Color::Color(const std::string& string) :
        Color{sf::String{string}}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Color::Color(const char* string) :
        Color{sf::String{string}}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Color::isSet() const
    {
        return m_isSet;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Color::operator sf::Color() const
    {
        return m_color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::uint8_t Color::getRed() const
    {
        return m_color.r;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::uint8_t Color::getGreen() const
    {
        return m_color.g;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::uint8_t Color::getBlue() const
    {
        return m_color.b;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::uint8_t Color::getAlpha() const
    {
        return m_color.a;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Color::operator==(const Color& right) const
    {
        return (m_isSet == right.m_isSet) && (m_color == right.m_color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Color::operator!=(const Color& right) const
    {
        return !(*this == right);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Color::operator==(const sf::Color& right) const
    {
        return m_isSet && (m_color == right);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Color::operator!=(const sf::Color& right) const
    {
        return !(*this == right);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


#include <TGUI/CustomWidgetForBindings.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CustomWidgetForBindings::Ptr CustomWidgetForBindings::create()
    {
        return std::make_shared<CustomWidgetForBindings>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CustomWidgetForBindings::setPosition(const Layout2d& position)
    {
        Widget::setPosition(position);
        implPositionChanged(position.getValue());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CustomWidgetForBindings::setSize(const Layout2d& size)
    {
        Widget::setSize(size);
        implSizeChanged(size.getValue());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f CustomWidgetForBindings::getFullSize() const
    {
        return implGetFullSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f CustomWidgetForBindings::getAbsolutePosition() const
    {
        return implGetAbsolutePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f CustomWidgetForBindings::getWidgetOffset() const
    {
        return implGetWidgetOffset();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CustomWidgetForBindings::setVisible(bool visible)
    {
        Widget::setVisible(visible);
        implVisibleChanged(visible);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CustomWidgetForBindings::setEnabled(bool enabled)
    {
        Widget::setEnabled(enabled);
        implEnableChanged(enabled);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CustomWidgetForBindings::setFocused(bool focused)
    {
        Widget::setFocused(focused);
        implFocusChanged(focused);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool CustomWidgetForBindings::canGainFocus() const
    {
        return implCanGainFocus();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CustomWidgetForBindings::update(sf::Time elapsedTime)
    {
        Widget::update(elapsedTime);
        implUpdateFunction(elapsedTime);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool CustomWidgetForBindings::mouseOnWidget(Vector2f pos) const
    {
        return implMouseOnWidget(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CustomWidgetForBindings::leftMousePressed(Vector2f pos)
    {
        Widget::leftMousePressed(pos);
        implLeftMousePressed(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CustomWidgetForBindings::leftMouseReleased(Vector2f pos)
    {
        Widget::leftMouseReleased(pos);
        implLeftMouseReleased(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CustomWidgetForBindings::rightMousePressed(Vector2f pos)
    {
        Widget::rightMousePressed(pos);
        implRightMousePressed(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CustomWidgetForBindings::rightMouseReleased(Vector2f pos)
    {
        Widget::rightMouseReleased(pos);
        implRightMouseReleased(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CustomWidgetForBindings::mouseMoved(Vector2f pos)
    {
        Widget::mouseMoved(pos);
        implMouseMoved(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CustomWidgetForBindings::keyPressed(const sf::Event::KeyEvent& event)
    {
        Widget::keyPressed(event);
        implKeyPressed(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CustomWidgetForBindings::textEntered(std::uint32_t key)
    {
        Widget::textEntered(key);
        implTextEntered(key);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool CustomWidgetForBindings::mouseWheelScrolled(float delta, Vector2f pos)
    {
        return implMouseWheelScrolled(delta, pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CustomWidgetForBindings::mouseNoLongerOnWidget()
    {
        Widget::mouseNoLongerOnWidget();
        implMouseNoLongerOnWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CustomWidgetForBindings::leftMouseButtonNoLongerDown()
    {
        Widget::leftMouseButtonNoLongerDown();
        implLeftMouseButtonNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CustomWidgetForBindings::rendererChanged(const std::string& property)
    {
        if (!implRendererChanged(property))
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CustomWidgetForBindings::mouseEnteredWidget()
    {
        Widget::mouseEnteredWidget();
        implMouseEnteredWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CustomWidgetForBindings::mouseLeftWidget()
    {
        Widget::mouseLeftWidget();
        implMouseLeftWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CustomWidgetForBindings::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        implDrawFunction(target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


#include <TGUI/Global.hpp>
#include <TGUI/Clipboard.hpp>
#include <TGUI/DefaultFont.hpp>
#include <functional>
#include <sstream>
#include <locale>
#include <cctype> // isspace
#include <cmath> // abs

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace
    {
        unsigned int globalTextSize = 13;
        unsigned int globalDoubleClickTime = 500;
        std::string globalResourcePath = "";
        std::shared_ptr<sf::Font> globalFont = nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void setGlobalTextSize(unsigned int textSize)
    {
        globalTextSize = textSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int getGlobalTextSize()
    {
        return globalTextSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void setGlobalFont(const Font& font)
    {
        globalFont = font.getFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Font getGlobalFont()
    {
        if (!globalFont)
        {
            globalFont = std::make_shared<sf::Font>();
            globalFont->loadFromMemory(defaultFontBytes, sizeof(defaultFontBytes));
        }

        return globalFont;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::shared_ptr<sf::Font>& getInternalGlobalFont()
    {
        return globalFont;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void setDoubleClickTime(unsigned int milliseconds)
    {
        globalDoubleClickTime = milliseconds;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int getDoubleClickTime()
    {
        return globalDoubleClickTime;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void setResourcePath(const std::string& path)
    {
        globalResourcePath = path;

        if (!globalResourcePath.empty())
        {
            if (globalResourcePath[globalResourcePath.length()-1] != '/')
                globalResourcePath.push_back('/');
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& getResourcePath()
    {
        return globalResourcePath;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool compareFloats(float x, float y)
    {
        return (std::abs(x - y) < 0.0000001f);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TGUI_REMOVE_DEPRECATED_CODE
    int stoi(const std::string& value)
    {
        int result = 0;

        std::istringstream iss(value);
        iss.imbue(std::locale::classic());
        iss >> result;

        if (iss.fail())
            result = 0;

        return result;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float stof(const std::string& value)
    {
        float result = 0;

        std::istringstream iss(value);
        iss.imbue(std::locale::classic());
        iss >> result;

        if (iss.fail())
            result = 0;

        return result;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool extractBoolFromString(const std::string& property, const std::string& value)
    {
        if ((value == "true") || (value == "True") || (value == "TRUE") || (value == "1"))
            return true;
        else if ((value == "false") || (value == "False") || (value == "FALSE") || (value == "0"))
            return false;
        else
            throw Exception{"Failed to parse boolean value of property '" + property + "'."};
    }
#endif
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TGUI_NEXT
    bool removeWhitespace(const std::string& line, std::string::const_iterator& c)
    {
        while (c != line.end())
        {
            if ((*c == ' ') || (*c == '\t') || (*c == '\r'))
                ++c;
            else
                return true;
        }

        return false;
    }
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


#include <TGUI/Layout.hpp>
#include <TGUI/Widget.hpp>
#include <TGUI/Gui.hpp>
#include <TGUI/to_string.hpp>
#include <SFML/System/Err.hpp>
#include <cassert>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    static std::pair<std::string, std::string> parseMinMaxExpresssion(const std::string& expression)
    {
        unsigned int bracketCount = 0;
        auto commaOrBracketPos = expression.find_first_of(",()");
        while (commaOrBracketPos != std::string::npos)
        {
            if (expression[commaOrBracketPos] == '(')
                bracketCount++;
            else if (expression[commaOrBracketPos] == ')')
            {
                if (bracketCount == 0)
                    break;

                bracketCount--;
            }
            else // if (expression[commaOrBracketPos] == ',')
            {
                if (bracketCount == 0)
                    return {expression.substr(0, commaOrBracketPos), expression.substr(commaOrBracketPos + 1)};
            }

            commaOrBracketPos = expression.find_first_of(",()", commaOrBracketPos + 1);
        }

        TGUI_PRINT_WARNING("bracket mismatch while parsing min or max in layout string '" << expression << "'.");
        return {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout::Layout(std::string expression)
    {
        // Empty strings have value 0 (although this might indicate a mistake in the expression, it is valid for unary minus)
        expression = trim(expression);
        if (expression.empty())
            return;

        auto searchPos = expression.find_first_of("+-/*()");

        // Extract the value from the string when there are no more operators
        if (searchPos == std::string::npos)
        {
            // Convert percentages to references to the parent widget
            if (expression.back() == '%')
            {
                // We don't know if we have to bind the width or height, so bind "size" and let the connectWidget function figure it out later
                if (expression == "100%")
                {
                    m_boundString = "&.innersize";
                    m_operation = Operation::BindingString;
                }
                else // value is a fraction of parent size
                {
                    *this = Layout{Layout::Operation::Multiplies,
                                   std::make_unique<Layout>(strToFloat(expression.substr(0, expression.length()-1)) / 100.f),
                                   std::make_unique<Layout>("&.innersize")};
                }
            }
            else
            {
                // The expression might reference to a widget instead of being a constant
                expression = toLower(expression);
                if ((expression.substr(expression.size()-1) == "x")
                 || (expression.substr(expression.size()-1) == "y")
                 || (expression.substr(expression.size()-1) == "w") // width
                 || (expression.substr(expression.size()-1) == "h") // height
                 || (expression.size() >= 2 && expression.substr(expression.size()-2) == "iw") // width inside the container
                 || (expression.size() >= 2 && expression.substr(expression.size()-2) == "ih") // height inside the container
                 || (expression.size() >= 4 && expression.substr(expression.size()-4) == "left")
                 || (expression.size() >= 3 && expression.substr(expression.size()-3) == "top")
                 || (expression.size() >= 5 && expression.substr(expression.size()-5) == "width")
                 || (expression.size() >= 6 && expression.substr(expression.size()-6) == "height")
                 || (expression.size() >= 4 && expression.substr(expression.size()-4) == "size")
                 || (expression.size() >= 3 && expression.substr(expression.size()-3) == "pos")
                 || (expression.size() >= 8 && expression.substr(expression.size()-8) == "position")
                 || (expression.size() >= 9 && expression.substr(expression.size()-9) == "innersize")
                 || (expression.size() >= 10 && expression.substr(expression.size()-10) == "innerwidth")
                 || (expression.size() >= 11 && expression.substr(expression.size()-11) == "innerheight"))
                {
                    // We can't search for the referenced widget yet as no widget is connected to the widget yet, so store the string for future parsing
                    m_boundString = expression;
                    m_operation = Operation::BindingString;
                }
                else if (expression.size() >= 5 && expression.substr(expression.size()-5) == "right")
                {
                    *this = Layout{Operation::Plus,
                                   std::make_unique<Layout>(expression.substr(0, expression.size()-5) + "left"),
                                   std::make_unique<Layout>(expression.substr(0, expression.size()-5) + "width")};
                }
                else if (expression.size() >= 6 && expression.substr(expression.size()-6) == "bottom")
                {
                    *this = Layout{Operation::Plus,
                                   std::make_unique<Layout>(expression.substr(0, expression.size()-6) + "top"),
                                   std::make_unique<Layout>(expression.substr(0, expression.size()-6) + "height")};
                }
                else // Constant value
                    m_value = strToFloat(expression);
            }

            return;
        }

        // The string contains an expression, so split it up in multiple layouts
        std::list<Layout> operands;
        std::vector<Operation> operators;
        decltype(searchPos) prevSearchPos = 0;
        while (searchPos != std::string::npos)
        {
            switch (expression[searchPos])
            {
            case '+':
                operators.push_back(Operation::Plus);
                operands.emplace_back(expression.substr(prevSearchPos, searchPos - prevSearchPos));
                break;
            case '-':
                operators.push_back(Operation::Minus);
                operands.emplace_back(expression.substr(prevSearchPos, searchPos - prevSearchPos));
                break;
            case '*':
                operators.push_back(Operation::Multiplies);
                operands.emplace_back(expression.substr(prevSearchPos, searchPos - prevSearchPos));
                break;
            case '/':
                operators.push_back(Operation::Divides);
                operands.emplace_back(expression.substr(prevSearchPos, searchPos - prevSearchPos));
                break;
            case '(':
            {
                // Find corresponding closing bracket
                unsigned int bracketCount = 0;
                auto bracketPos = expression.find_first_of("()", searchPos + 1);
                while (bracketPos != std::string::npos)
                {
                    if (expression[bracketPos] == '(')
                        bracketCount++;
                    else if (bracketCount > 0)
                        bracketCount--;
                    else
                    {
                        // If the entire layout was in brackets then remove these brackets
                        if ((searchPos == 0) && (bracketPos == expression.size()-1))
                        {
                            *this = Layout{expression.substr(1, expression.size()-2)};
                            return;
                        }
                        else if ((searchPos == 3) && (bracketPos == expression.size()-1) && (expression.substr(0, 3) == "min"))
                        {
                            const auto& minSubExpressions = parseMinMaxExpresssion(expression.substr(4, expression.size() - 5));
                            *this = Layout{Operation::Minimum, std::make_unique<Layout>(minSubExpressions.first), std::make_unique<Layout>(minSubExpressions.second)};
                            return;
                        }
                        else if ((searchPos == 3) && (bracketPos == expression.size()-1) && (expression.substr(0, 3) == "max"))
                        {
                            const auto& maxSubExpressions = parseMinMaxExpresssion(expression.substr(4, expression.size() - 5));
                            *this = Layout{Operation::Maximum, std::make_unique<Layout>(maxSubExpressions.first), std::make_unique<Layout>(maxSubExpressions.second)};
                            return;
                        }
                        else // The brackets form a sub-expression
                            searchPos = bracketPos;

                        break;
                    }

                    bracketPos = expression.find_first_of("()", bracketPos + 1);
                }

                if (bracketPos == std::string::npos)
                {
                    TGUI_PRINT_WARNING("bracket mismatch while parsing layout string '" << expression << "'.");
                    return;
                }
                else
                {
                    // Search for the next operator, starting from the closing bracket, but keeping prevSearchPos before the opening bracket
                    searchPos = expression.find_first_of("+-/*()", searchPos + 1);
                    continue;
                }
            }
            case ')':
                TGUI_PRINT_WARNING("bracket mismatch while parsing layout string '" << expression << "'.");
                return;
            };

            prevSearchPos = searchPos + 1;
            searchPos = expression.find_first_of("+-/*()", searchPos + 1);
        }

        if (prevSearchPos == 0)
        {
            // We would get an infinite loop if we don't abort in this condition
            TGUI_PRINT_WARNING("error in expression '" << expression << "'.");
            return;
        }

        operands.emplace_back(expression.substr(prevSearchPos));

        // First perform all * and / operations
        auto operandIt = operands.begin();
        for (std::size_t i = 0; i < operators.size(); ++i)
        {
            if ((operators[i] == Operation::Multiplies) || (operators[i] == Operation::Divides))
            {
                auto nextOperandIt = operandIt;
                std::advance(nextOperandIt, 1);

                (*operandIt) = Layout{operators[i],
                                      std::make_unique<Layout>(*operandIt),
                                      std::make_unique<Layout>(*nextOperandIt)};

                operands.erase(nextOperandIt);
            }
            else
                ++operandIt;
        }

        // Now perform all + and - operations
        operandIt = operands.begin();
        for (std::size_t i = 0; i < operators.size(); ++i)
        {
            if ((operators[i] == Operation::Plus) || (operators[i] == Operation::Minus))
            {
                assert(operandIt != operands.end());

                auto nextOperandIt = operandIt;
                std::advance(nextOperandIt, 1);

                assert(nextOperandIt != operands.end());

                // Handle unary plus or minus
                if ((operandIt->m_operation == Operation::Value) && (nextOperandIt->m_operation == Operation::Value) && (operandIt->m_value == 0))
                {
                    if (operators[i] == Operation::Minus)
                        nextOperandIt->m_value = -nextOperandIt->m_value;

                    *operandIt = *nextOperandIt;
                }
                else // Normal addition or subtraction
                {
                    *operandIt = Layout{operators[i],
                                        std::make_unique<Layout>(*operandIt),
                                        std::make_unique<Layout>(*nextOperandIt)};
                }

                operands.erase(nextOperandIt);
            }
        }

        assert(operands.size() == 1);
        *this = operands.front();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout::Layout(Operation operation, Widget* boundWidget) :
        m_operation  {operation},
        m_boundWidget{boundWidget}
    {
        assert((m_operation == Operation::BindingLeft) || (m_operation == Operation::BindingTop)
               || (m_operation == Operation::BindingWidth) || (m_operation == Operation::BindingHeight)
               || (m_operation == Operation::BindingInnerWidth) || (m_operation == Operation::BindingInnerHeight));
        assert(m_boundWidget != nullptr);

        if (m_operation == Operation::BindingLeft)
            m_value = m_boundWidget->getPosition().x;
        else if (m_operation == Operation::BindingTop)
            m_value = m_boundWidget->getPosition().y;
        else if (m_operation == Operation::BindingWidth)
            m_value = m_boundWidget->getSize().x;
        else if (m_operation == Operation::BindingHeight)
            m_value = m_boundWidget->getSize().y;
        else if (m_operation == Operation::BindingInnerWidth)
        {
            const auto* boundContainer = dynamic_cast<Container*>(boundWidget);
            if (boundContainer)
                m_value = boundContainer->getInnerSize().x;
        }
        else if (m_operation == Operation::BindingInnerHeight)
        {
            const auto* boundContainer = dynamic_cast<Container*>(boundWidget);
            if (boundContainer)
                m_value = boundContainer->getInnerSize().y;
        }

        resetPointers();
        recalculateValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout::Layout(Operation operation, std::unique_ptr<Layout> leftOperand, std::unique_ptr<Layout> rightOperand) :
        m_operation   {operation},
        m_leftOperand {std::move(leftOperand)},
        m_rightOperand{std::move(rightOperand)}
    {
        assert(m_leftOperand != nullptr);
        assert(m_rightOperand != nullptr);
        resetPointers();
        recalculateValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout::Layout(const Layout& other) :
        m_value          {other.m_value},
        m_parent         {other.m_parent},
        m_operation      {other.m_operation},
        m_leftOperand    {other.m_leftOperand ? std::make_unique<Layout>(*other.m_leftOperand) : nullptr},
        m_rightOperand   {other.m_rightOperand ? std::make_unique<Layout>(*other.m_rightOperand) : nullptr},
        m_boundWidget    {other.m_boundWidget},
        m_boundString    {other.m_boundString}
    {
        // Disconnect the bound widget if a string was used, the same name may apply to a different widget now
        if (!m_boundString.empty())
            m_boundWidget = nullptr;

        resetPointers();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout::Layout(Layout&& other) :
        m_value          {std::move(other.m_value)},
        m_parent         {std::move(other.m_parent)},
        m_operation      {other.m_operation},
        m_leftOperand    {std::move(other.m_leftOperand)},
        m_rightOperand   {std::move(other.m_rightOperand)},
        m_boundWidget    {other.m_boundWidget},
        m_boundString    {std::move(other.m_boundString)}
    {
        resetPointers();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout& Layout::operator=(const Layout& other)
    {
        if (this != &other)
        {
            unbindLayout();

            m_value           = other.m_value;
            m_parent          = other.m_parent;
            m_operation       = other.m_operation;
            m_leftOperand     = other.m_leftOperand ? std::make_unique<Layout>(*other.m_leftOperand) : nullptr;
            m_rightOperand    = other.m_rightOperand ? std::make_unique<Layout>(*other.m_rightOperand) : nullptr;
            m_boundWidget     = other.m_boundWidget;
            m_boundString     = other.m_boundString;

            // Disconnect the bound widget if a string was used, the same name may apply to a different widget now
            if (!m_boundString.empty())
                m_boundWidget = nullptr;

            resetPointers();
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout& Layout::operator=(Layout&& other)
    {
        if (this != &other)
        {
            unbindLayout();

            m_value           = std::move(other.m_value);
            m_parent          = std::move(other.m_parent);
            m_operation       = other.m_operation;
            m_leftOperand     = std::move(other.m_leftOperand);
            m_rightOperand    = std::move(other.m_rightOperand);
            m_boundWidget     = other.m_boundWidget;
            m_boundString     = std::move(other.m_boundString);

            resetPointers();
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout::~Layout()
    {
        unbindLayout();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string Layout::toString() const
    {
        if (m_operation == Operation::Value)
        {
            return to_string(m_value);
        }
        else if (m_operation == Operation::Minimum)
        {
            return "min(" + m_leftOperand->toString() + ", " + m_rightOperand->toString() + ")";
        }
        else if (m_operation == Operation::Maximum)
        {
            return "max(" + m_leftOperand->toString() + ", " + m_rightOperand->toString() + ")";
        }
        else if ((m_operation == Operation::Plus) || (m_operation == Operation::Minus) || (m_operation == Operation::Multiplies) || (m_operation == Operation::Divides))
        {
            char operatorChar;
            if (m_operation == Operation::Plus)
                operatorChar = '+';
            else if (m_operation == Operation::Minus)
                operatorChar = '-';
            else if (m_operation == Operation::Multiplies)
                operatorChar = '*';
            else // if (m_operation == Operation::Divides)
                operatorChar = '/';

            auto subExpressionNeedsBrackets = [](const std::unique_ptr<Layout>& operand)
                {
                    if (!operand->m_leftOperand)
                        return false;

                    if ((operand->m_operation == Operation::Minimum) || (operand->m_operation == Operation::Maximum))
                        return false;

                    if ((operand->m_operation == Operation::Multiplies) && (operand->m_leftOperand->m_operation == Operation::Value) && (operand->m_rightOperand->toString() == "100%"))
                        return false;

                    return true;
                };

            if (subExpressionNeedsBrackets(m_leftOperand) && subExpressionNeedsBrackets(m_rightOperand))
                return "(" + m_leftOperand->toString() + ") " + operatorChar + " (" + m_rightOperand->toString() + ")";
            else if (subExpressionNeedsBrackets(m_leftOperand))
                return "(" + m_leftOperand->toString() + ") " + operatorChar + " " + m_rightOperand->toString();
            else if (subExpressionNeedsBrackets(m_rightOperand))
                return m_leftOperand->toString() + " " + operatorChar + " (" + m_rightOperand->toString() + ")";
            else
            {
                if ((m_operation == Operation::Multiplies) && (m_leftOperand->m_operation == Operation::Value) && (m_rightOperand->toString() == "100%"))
                    return to_string(m_leftOperand->getValue() * 100) + '%';
                else
                    return m_leftOperand->toString() + " " + operatorChar + " " + m_rightOperand->toString();
            }
        }
        else
        {
            if (m_boundString == "&.innersize")
                return "100%";

            // Hopefully the expression is stored in the bound string, otherwise (i.e. when bind functions were used) it is infeasible to turn it into a string
            return m_boundString;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Layout::unbindLayout()
    {
        if (m_boundWidget)
        {
            assert((m_operation == Operation::BindingLeft) || (m_operation == Operation::BindingTop)
                   || (m_operation == Operation::BindingWidth) || (m_operation == Operation::BindingHeight)
                   || (m_operation == Operation::BindingInnerWidth) || (m_operation == Operation::BindingInnerHeight));

            if ((m_operation == Operation::BindingLeft) || (m_operation == Operation::BindingTop))
                m_boundWidget->unbindPositionLayout(this);
            else
                m_boundWidget->unbindSizeLayout(this);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Layout::resetPointers()
    {
        if (m_leftOperand != nullptr)
        {
            assert(m_rightOperand != nullptr);

            m_leftOperand->m_parent = this;
            m_rightOperand->m_parent = this;
        }

        if (m_boundWidget)
        {
            assert((m_operation == Operation::BindingLeft) || (m_operation == Operation::BindingTop)
                   || (m_operation == Operation::BindingWidth) || (m_operation == Operation::BindingHeight)
                   || (m_operation == Operation::BindingInnerWidth) || (m_operation == Operation::BindingInnerHeight));

            if ((m_operation == Operation::BindingLeft) || (m_operation == Operation::BindingTop))
                m_boundWidget->bindPositionLayout(this);
            else
                m_boundWidget->bindSizeLayout(this);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Layout::connectWidget(Widget* widget, bool xAxis, std::function<void()> valueChangedCallbackHandler)
    {
        const float oldValue = m_value;

        // No callbacks must be made while parsing, a single callback will be made when done if needed
        m_connectedWidgetCallback = nullptr;

        parseBindingStringRecursive(widget, xAxis);

        // Restore the callback function
        m_connectedWidgetCallback = valueChangedCallbackHandler;

        if (m_value != oldValue)
        {
            if (m_connectedWidgetCallback)
                m_connectedWidgetCallback();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Layout::unbindWidget()
    {
        m_boundWidget = nullptr;

        if (!m_boundString.empty())
            m_operation = Operation::BindingString;
        else
        {
            m_value = 0;
            m_operation = Operation::Value;
        }

        recalculateValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Layout::recalculateValue()
    {
        const float oldValue = m_value;

        switch (m_operation)
        {
            case Operation::Value:
                break;
            case Operation::Plus:
                m_value = m_leftOperand->getValue() + m_rightOperand->getValue();
                break;
            case Operation::Minus:
                m_value = m_leftOperand->getValue() - m_rightOperand->getValue();
                break;
            case Operation::Multiplies:
                m_value = m_leftOperand->getValue() * m_rightOperand->getValue();
                break;
            case Operation::Divides:
                if (m_rightOperand->getValue() != 0)
                    m_value = m_leftOperand->getValue() / m_rightOperand->getValue();
                else
                    m_value = 0;
                break;
            case Operation::Minimum:
                m_value = std::min(m_leftOperand->getValue(), m_rightOperand->getValue());
                break;
            case Operation::Maximum:
                m_value = std::max(m_leftOperand->getValue(), m_rightOperand->getValue());
                break;
            case Operation::BindingLeft:
                m_value = m_boundWidget->getPosition().x;
                break;
            case Operation::BindingTop:
                m_value = m_boundWidget->getPosition().y;
                break;
            case Operation::BindingWidth:
                m_value = m_boundWidget->getSize().x;
                break;
            case Operation::BindingHeight:
                m_value = m_boundWidget->getSize().y;
                break;
            case Operation::BindingInnerWidth:
            {
                const auto* boundContainer = dynamic_cast<Container*>(m_boundWidget);
                if (boundContainer)
                    m_value = boundContainer->getInnerSize().x;
                break;
            }
            case Operation::BindingInnerHeight:
            {
                const auto* boundContainer = dynamic_cast<Container*>(m_boundWidget);
                if (boundContainer)
                    m_value = boundContainer->getInnerSize().y;
                break;
            }
            case Operation::BindingString:
                // The string should have already been parsed by now.
                // Passing here either means something is wrong with the string or the layout was not connected to a widget with a parent yet.
                break;
        };

        if (m_value != oldValue)
        {
            if (m_parent)
                m_parent->recalculateValue();
            else
            {
                // The topmost layout must tell the connected widget about the new value
                if (m_connectedWidgetCallback)
                    m_connectedWidgetCallback();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Layout::parseBindingStringRecursive(Widget* widget, bool xAxis)
    {
        if (m_leftOperand)
        {
            assert(m_rightOperand != nullptr);

            m_leftOperand->parseBindingStringRecursive(widget, xAxis);
            m_rightOperand->parseBindingStringRecursive(widget, xAxis);
        }

        // Parse the string binding even when the referred widget was already found. The widget may be added to a different parent
        if (!m_boundString.empty())
            parseBindingString(m_boundString, widget, xAxis);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Layout::parseBindingString(const std::string& expression, Widget* widget, bool xAxis)
    {
        if (expression == "x" || expression == "left")
        {
            m_operation = Operation::BindingLeft;
            m_boundWidget = widget;
        }
        else if (expression == "y" || expression == "top")
        {
            m_operation = Operation::BindingTop;
            m_boundWidget = widget;
        }
        else if (expression == "w" || expression == "width")
        {
            m_operation = Operation::BindingWidth;
            m_boundWidget = widget;
        }
        else if (expression == "h" || expression == "height")
        {
            m_operation = Operation::BindingHeight;
            m_boundWidget = widget;
        }
        else if (expression == "iw" || expression == "innerwidth")
        {
            m_operation = Operation::BindingInnerWidth;
            m_boundWidget = widget;
        }
        else if (expression == "ih" || expression == "innerheight")
        {
            m_operation = Operation::BindingInnerHeight;
            m_boundWidget = widget;
        }
        else if (expression == "size")
        {
            if (xAxis)
                return parseBindingString("width", widget, xAxis);
            else
                return parseBindingString("height", widget, xAxis);
        }
        else if (expression == "innersize")
        {
            if (xAxis)
                return parseBindingString("innerwidth", widget, xAxis);
            else
                return parseBindingString("innerheight", widget, xAxis);
        }
        else if ((expression == "pos") || (expression == "position"))
        {
            if (xAxis)
                return parseBindingString("x", widget, xAxis);
            else
                return parseBindingString("y", widget, xAxis);
        }
        else
        {
            const auto dotPos = expression.find('.');
            if (dotPos != std::string::npos)
            {
                const std::string widgetName = expression.substr(0, dotPos);
                if (widgetName == "parent" || widgetName == "&")
                {
                    if (widget->getParent())
                        return parseBindingString(expression.substr(dotPos+1), widget->getParent(), xAxis);
                }
                else if (!widgetName.empty())
                {
                    // If the widget is a container, search in its children first
                    Container* container = dynamic_cast<Container*>(widget);
                    if (container != nullptr)
                    {
                        const auto& widgets = container->getWidgets();
                        for (std::size_t i = 0; i < widgets.size(); ++i)
                        {
                            if (toLower(widgets[i]->getWidgetName()) == widgetName)
                                return parseBindingString(expression.substr(dotPos+1), widgets[i].get(), xAxis);
                        }
                    }

                    // If the widget has a parent, look for a sibling
                    if (widget->getParent())
                    {
                        const auto& widgets = widget->getParent()->getWidgets();
                        for (std::size_t i = 0; i < widgets.size(); ++i)
                        {
                            if (toLower(widgets[i]->getWidgetName()) == widgetName)
                                return parseBindingString(expression.substr(dotPos+1), widgets[i].get(), xAxis);
                        }
                    }
                }
            }

            // The referred widget was not found or there was something wrong with the string
            return;
        }

        resetPointers();
        recalculateValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout operator-(Layout right)
    {
        return Layout{Layout::Operation::Minus, std::make_unique<Layout>(), std::make_unique<Layout>(std::move(right))};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout operator+(Layout left, Layout right)
    {
        return Layout{Layout::Operation::Plus, std::make_unique<Layout>(std::move(left)), std::make_unique<Layout>(std::move(right))};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout operator-(Layout left, Layout right)
    {
        return Layout{Layout::Operation::Minus, std::make_unique<Layout>(std::move(left)), std::make_unique<Layout>(std::move(right))};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout operator*(Layout left, Layout right)
    {
        return Layout{Layout::Operation::Multiplies, std::make_unique<Layout>(std::move(left)), std::make_unique<Layout>(std::move(right))};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout operator/(Layout left, Layout right)
    {
        return Layout{Layout::Operation::Divides, std::make_unique<Layout>(std::move(left)), std::make_unique<Layout>(std::move(right))};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d operator-(Layout2d right)
    {
        return Layout2d{-std::move(right.x), -std::move(right.y)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d operator+(Layout2d left, Layout2d right)
    {
        return Layout2d{std::move(left.x) + std::move(right.x), std::move(left.y) + std::move(right.y)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d operator-(Layout2d left, Layout2d right)
    {
        return Layout2d{std::move(left.x) - std::move(right.x), std::move(left.y) - std::move(right.y)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d operator*(Layout2d left, const Layout& right)
    {
        return Layout2d{std::move(left.x) * right, std::move(left.y) * right};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d operator*(const Layout& left, Layout2d right)
    {
        return Layout2d{left * std::move(right.x), left * std::move(right.y)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d operator/(Layout2d left, const Layout& right)
    {
        return Layout2d{std::move(left.x) / right, std::move(left.y) / right};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    inline namespace bind_functions
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout bindLeft(Widget::Ptr widget)
        {
            return Layout{Layout::Operation::BindingLeft, widget.get()};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout bindTop(Widget::Ptr widget)
        {
            return Layout{Layout::Operation::BindingTop, widget.get()};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout bindWidth(Widget::Ptr widget)
        {
            return Layout{Layout::Operation::BindingWidth, widget.get()};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout bindHeight(Widget::Ptr widget)
        {
            return Layout{Layout::Operation::BindingHeight, widget.get()};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout bindRight(Widget::Ptr widget)
        {
            return Layout{Layout::Operation::Plus,
                          std::make_unique<Layout>(Layout::Operation::BindingLeft, widget.get()),
                          std::make_unique<Layout>(Layout::Operation::BindingWidth, widget.get())};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout bindBottom(Widget::Ptr widget)
        {
            return Layout{Layout::Operation::Plus,
                          std::make_unique<Layout>(Layout::Operation::BindingTop, widget.get()),
                          std::make_unique<Layout>(Layout::Operation::BindingHeight, widget.get())};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout2d bindPosition(Widget::Ptr widget)
        {
            return {bindLeft(widget), bindTop(widget)};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout2d bindSize(Widget::Ptr widget)
        {
            return {bindWidth(widget), bindHeight(widget)};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout bindWidth(Gui& gui)
        {
            return bindWidth(gui.getContainer());
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout bindHeight(Gui& gui)
        {
            return bindHeight(gui.getContainer());
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout2d bindSize(Gui& gui)
        {
            return bindSize(gui.getContainer());
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout bindMin(const Layout& value1, const Layout& value2)
        {
            return Layout{Layout::Operation::Minimum, std::make_unique<Layout>(value1), std::make_unique<Layout>(value2)};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout bindMax(const Layout& value1, const Layout& value2)
        {
            return Layout{Layout::Operation::Maximum, std::make_unique<Layout>(value1), std::make_unique<Layout>(value2)};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


#include <TGUI/Sprite.hpp>
#include <TGUI/Color.hpp>
#include <TGUI/Clipping.hpp>

#include <cassert>
#include <cmath>

#if TGUI_COMPILED_WITH_CPP_VER >= 17
    #include <optional>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    Sprite::Sprite(const Texture& texture)
    {
        setTexture(texture);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Sprite::setTexture(const Texture& texture)
    {
        m_texture = texture;
        m_vertexColor = m_texture.getColor();
        m_shader = m_texture.getShader();

        if (isSet())
        {
            if (getSize() == Vector2f{})
                setSize(texture.getImageSize());
            else
                updateVertices();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Texture& Sprite::getTexture() const
    {
        return m_texture;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef TGUI_NEXT
    Texture& Sprite::getTexture()
    {
        return m_texture;
    }
#endif
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Sprite::isSet() const
    {
        return m_texture.getData() != nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Sprite::setSize(const Vector2f& size)
    {
        m_size.x = std::max(size.x, 0.f);
        m_size.y = std::max(size.y, 0.f);

        if (isSet())
            updateVertices();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Sprite::getSize() const
    {
        return m_size;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef TGUI_REMOVE_DEPRECATED_CODE
    void Sprite::setColor(const Color& color)
    {
        m_vertexColor = color;

        const Color& vertexColor = Color::calcColorOpacity(m_vertexColor, m_opacity);
        for (auto& vertex : m_vertices)
            vertex.color = vertexColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Color& Sprite::getColor() const
    {
        return m_vertexColor;
    }
#endif
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Sprite::setOpacity(float opacity)
    {
        m_opacity = opacity;

        const Color& vertexColor = Color::calcColorOpacity(m_vertexColor, m_opacity);
        for (auto& vertex : m_vertices)
            vertex.color = vertexColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Sprite::getOpacity() const
    {
        return m_opacity;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Sprite::setVisibleRect(const FloatRect& visibleRect)
    {
        m_visibleRect = visibleRect;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FloatRect Sprite::getVisibleRect() const
    {
        return m_visibleRect;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Sprite::isTransparentPixel(Vector2f pos) const
    {
        if (!isSet() || !m_texture.getData()->image || (m_size.x == 0) || (m_size.y == 0))
            return true;

        if (getRotation() != 0)
        {
            Vector2f offset = {getTransform().transformRect(FloatRect({}, getSize())).left,
                               getTransform().transformRect(FloatRect({}, getSize())).top};

            pos = getInverseTransform().transformPoint(pos) + getInverseTransform().transformPoint(offset);

            // Watch out for rounding errors
            const float epsilon = 0.00001f;
            if ((pos.x < 0) && (pos.x > -epsilon))
                pos.x = 0;
            if ((pos.y < 0) && (pos.y > -epsilon))
                pos.y = 0;
            if ((pos.x >= getSize().x) && (pos.x < getSize().x + epsilon))
                pos.x = getSize().x;
            if ((pos.y >= getSize().y) && (pos.y < getSize().y + epsilon))
                pos.y = getSize().y;
        }
        else // There is no rotation
            pos -= getPosition();

        if ((pos.x < 0) || (pos.y < 0) || (pos.x >= getSize().x) || (pos.y >= getSize().y))
            return true;

        // Find out on which pixel the mouse is standing
        sf::Vector2u pixel;
        sf::IntRect middleRect = m_texture.getMiddleRect();
        const sf::Texture& texture = m_texture.getData()->texture;
        switch (m_scalingType)
        {
            case ScalingType::Normal:
            {
                pixel.x = static_cast<unsigned int>(pos.x / m_size.x * texture.getSize().x);
                pixel.y = static_cast<unsigned int>(pos.y / m_size.y * texture.getSize().y);
                break;
            }
            case ScalingType::Horizontal:
            {
                if (pos.x >= m_size.x - (texture.getSize().x - middleRect.left - middleRect.width) * (m_size.y / texture.getSize().y))
                {
                    float xDiff = (pos.x - (m_size.x - (texture.getSize().x - middleRect.left - middleRect.width) * (m_size.y / texture.getSize().y)));
                    pixel.x = static_cast<unsigned int>(middleRect.left + middleRect.width + (xDiff / m_size.y * texture.getSize().y));
                }
                else if (pos.x >= middleRect.left * (m_size.y / texture.getSize().y))
                {
                    float xDiff = pos.x - (middleRect.left * (m_size.y / texture.getSize().y));
                    pixel.x = static_cast<unsigned int>(middleRect.left + (xDiff / (m_size.x - ((texture.getSize().x - middleRect.width) * (m_size.y / texture.getSize().y))) * middleRect.width));
                }
                else // Mouse on the left part
                {
                    pixel.x = static_cast<unsigned int>(pos.x / m_size.y * texture.getSize().y);
                }

                pixel.y = static_cast<unsigned int>(pos.y / m_size.y * texture.getSize().y);
                break;
            }
            case ScalingType::Vertical:
            {
                if (pos.y >= m_size.y - (texture.getSize().y - middleRect.top - middleRect.height) * (m_size.x / texture.getSize().x))
                {
                    float yDiff = (pos.y - (m_size.y - (texture.getSize().y - middleRect.top - middleRect.height) * (m_size.x / texture.getSize().x)));
                    pixel.y = static_cast<unsigned int>(middleRect.top + middleRect.height + (yDiff / m_size.x * texture.getSize().x));
                }
                else if (pos.y >= middleRect.top * (m_size.x / texture.getSize().x))
                {
                    float yDiff = pos.y - (middleRect.top * (m_size.x / texture.getSize().x));
                    pixel.y = static_cast<unsigned int>(middleRect.top + (yDiff / (m_size.y - ((texture.getSize().y - middleRect.height) * (m_size.x / texture.getSize().x))) * middleRect.height));
                }
                else // Mouse on the top part
                {
                    pixel.y = static_cast<unsigned int>(pos.y / m_size.x * texture.getSize().x);
                }

                pixel.x = static_cast<unsigned int>(pos.x / m_size.x * texture.getSize().x);
                break;
            }
            case ScalingType::NineSlice:
            {
                if (pos.x < middleRect.left)
                    pixel.x = static_cast<unsigned int>(pos.x);
                else if (pos.x >= m_size.x - (texture.getSize().x - middleRect.width - middleRect.left))
                    pixel.x = static_cast<unsigned int>(pos.x - m_size.x + texture.getSize().x);
                else
                {
                    float xDiff = (pos.x - middleRect.left) / (m_size.x - (texture.getSize().x - middleRect.width)) * middleRect.width;
                    pixel.x = static_cast<unsigned int>(middleRect.left + xDiff);
                }

                if (pos.y < middleRect.top)
                    pixel.y = static_cast<unsigned int>(pos.y);
                else if (pos.y >= m_size.y - (texture.getSize().y - middleRect.height - middleRect.top))
                    pixel.y = static_cast<unsigned int>(pos.y - m_size.y + texture.getSize().y);
                else
                {
                    float yDiff = (pos.y - middleRect.top) / (m_size.y - (texture.getSize().y - middleRect.height)) * middleRect.height;
                    pixel.y = static_cast<unsigned int>(middleRect.top + yDiff);
                }

                break;
            }
        };

        return m_texture.isTransparentPixel(pixel);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Sprite::ScalingType Sprite::getScalingType() const
    {
        return m_scalingType;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Sprite::updateVertices()
    {
        // Figure out how the image is scaled best
        Vector2f textureSize;
        FloatRect middleRect;
        if (m_texture.getData()->svgImage)
        {
            if (!m_svgTexture)
                m_svgTexture = aurora::makeCopied<sf::Texture>();

            const sf::Vector2u svgTextureSize{
                static_cast<unsigned int>(std::round(getSize().x)),
                static_cast<unsigned int>(std::round(getSize().y))};

            m_texture.getData()->svgImage->rasterize(*m_svgTexture, svgTextureSize);

            m_scalingType = ScalingType::Normal;
            textureSize = getSize();
        }
        else
        {
            textureSize = m_texture.getImageSize();
            middleRect = sf::FloatRect{m_texture.getMiddleRect()};
            if (middleRect == FloatRect(0, 0, textureSize.x, textureSize.y))
            {
                m_scalingType = ScalingType::Normal;
            }
            else if (middleRect.height == textureSize.y)
            {
                if (m_size.x >= (textureSize.x - middleRect.width) * (m_size.y / textureSize.y))
                    m_scalingType = ScalingType::Horizontal;
                else
                    m_scalingType = ScalingType::Normal;
            }
            else if (middleRect.width == textureSize.x)
            {
                if (m_size.y >= (textureSize.y - middleRect.height) * (m_size.x / textureSize.x))
                    m_scalingType = ScalingType::Vertical;
                else
                    m_scalingType = ScalingType::Normal;
            }
            else
            {
                if (m_size.x >= textureSize.x - middleRect.width)
                {
                    if (m_size.y >= textureSize.y - middleRect.height)
                        m_scalingType = ScalingType::NineSlice;
                    else
                        m_scalingType = ScalingType::Horizontal;
                }
                else if (m_size.y >= (textureSize.y - middleRect.height) * (m_size.x / textureSize.x))
                    m_scalingType = ScalingType::Vertical;
                else
                    m_scalingType = ScalingType::Normal;
            }
        }

        // Calculate the vertices based on the way we are scaling
        const Color& vertexColor = Color::calcColorOpacity(m_vertexColor, m_opacity);
        switch (m_scalingType)
        {
        case ScalingType::Normal:
            ///////////
            // 0---1 //
            // |   | //
            // 2---3 //
            ///////////
            m_vertices.resize(4);
            m_vertices[0] = {{0, 0}, vertexColor, {0, 0}};
            m_vertices[1] = {{m_size.x, 0}, vertexColor, {textureSize.x, 0}};
            m_vertices[2] = {{0, m_size.y}, vertexColor, {0, textureSize.y}};
            m_vertices[3] = {{m_size.x, m_size.y}, vertexColor, {textureSize.x, textureSize.y}};
            break;

        case ScalingType::Horizontal:
            ///////////////////////
            // 0---2-------4---6 //
            // |   |       |   | //
            // 1---3-------5---7 //
            ///////////////////////
            m_vertices.resize(8);
            m_vertices[0] = {{0, 0}, vertexColor, {0, 0}};
            m_vertices[1] = {{0, m_size.y}, vertexColor, {0, textureSize.y}};
            m_vertices[2] = {{middleRect.left * (m_size.y / textureSize.y), 0}, vertexColor, {middleRect.left, 0}};
            m_vertices[3] = {{middleRect.left * (m_size.y / textureSize.y), m_size.y}, vertexColor, {middleRect.left, textureSize.y}};
            m_vertices[4] = {{m_size.x - (textureSize.x - middleRect.left - middleRect.width) * (m_size.y / textureSize.y), 0}, vertexColor, {middleRect.left + middleRect.width, 0}};
            m_vertices[5] = {{m_size.x - (textureSize.x - middleRect.left - middleRect.width) * (m_size.y / textureSize.y), m_size.y}, vertexColor, {middleRect.left + middleRect.width, textureSize.y}};
            m_vertices[6] = {{m_size.x, 0}, vertexColor, {textureSize.x, 0}};
            m_vertices[7] = {{m_size.x, m_size.y}, vertexColor, {textureSize.x, textureSize.y}};
            break;

        case ScalingType::Vertical:
            ///////////
            // 0---1 //
            // |   | //
            // 2---3 //
            // |   | //
            // |   | //
            // |   | //
            // 4---5 //
            // |   | //
            // 6---7-//
            ///////////
            m_vertices.resize(8);
            m_vertices[0] = {{0, 0}, vertexColor, {0, 0}};
            m_vertices[1] = {{m_size.x, 0}, vertexColor, {textureSize.x, 0}};
            m_vertices[2] = {{0, middleRect.top * (m_size.x / textureSize.x)}, vertexColor, {0, middleRect.top}};
            m_vertices[3] = {{m_size.x, middleRect.top * (m_size.x / textureSize.x)}, vertexColor, {textureSize.x, middleRect.top}};
            m_vertices[4] = {{0, m_size.y - (textureSize.y - middleRect.top - middleRect.height) * (m_size.x / textureSize.x)}, vertexColor, {0, middleRect.top + middleRect.height}};
            m_vertices[5] = {{m_size.x, m_size.y - (textureSize.y - middleRect.top - middleRect.height) * (m_size.x / textureSize.x)}, vertexColor, {textureSize.x, middleRect.top + middleRect.height}};
            m_vertices[6] = {{0, m_size.y}, vertexColor, {0, textureSize.y}};
            m_vertices[7] = {{m_size.x, m_size.y}, vertexColor, {textureSize.x, textureSize.y}};
            break;

        case ScalingType::NineSlice:
            //////////////////////////////////
            // 0---1/13-----------14-----15 //
            // |    |              |     |  //
            // 2---3/11----------12/16---17 //
            // |    |              |     |  //
            // |    |              |     |  //
            // |    |              |     |  //
            // 4---5/9-----------10/18---19 //
            // |    |              |     |  //
            // 6----7-------------8/20---21 //
            //////////////////////////////////
            m_vertices.resize(22);
            m_vertices[0] = {{0, 0}, vertexColor, {0, 0}};
            m_vertices[1] = {{middleRect.left, 0}, vertexColor, {middleRect.left, 0}};
            m_vertices[2] = {{0, middleRect.top}, vertexColor, {0, middleRect.top}};
            m_vertices[3] = {{middleRect.left, middleRect.top}, vertexColor, {middleRect.left, middleRect.top}};
            m_vertices[4] = {{0, m_size.y - (textureSize.y - middleRect.top - middleRect.height)}, vertexColor, {0, middleRect.top + middleRect.height}};
            m_vertices[5] = {{middleRect.left, m_size.y - (textureSize.y - middleRect.top - middleRect.height)}, vertexColor, {middleRect.left, middleRect.top + middleRect.height}};
            m_vertices[6] = {{0, m_size.y}, vertexColor, {0, textureSize.y}};
            m_vertices[7] = {{middleRect.left, m_size.y}, vertexColor, {middleRect.left, textureSize.y}};
            m_vertices[8] = {{m_size.x - (textureSize.x - middleRect.left - middleRect.width), m_size.y}, vertexColor, {middleRect.left + middleRect.width, textureSize.y}};
            m_vertices[9] = m_vertices[5];
            m_vertices[10] = {{m_size.x - (textureSize.x - middleRect.left - middleRect.width), m_size.y - (textureSize.y - middleRect.top - middleRect.height)}, vertexColor, {middleRect.left + middleRect.width, middleRect.top + middleRect.height}};
            m_vertices[11] = m_vertices[3];
            m_vertices[12] = {{m_size.x - (textureSize.x - middleRect.left - middleRect.width), middleRect.top}, vertexColor, {middleRect.left + middleRect.width, middleRect.top}};
            m_vertices[13] = m_vertices[1];
            m_vertices[14] = {{m_size.x - (textureSize.x - middleRect.left - middleRect.width), 0}, vertexColor, {middleRect.left + middleRect.width, 0}};
            m_vertices[15] = {{m_size.x, 0}, vertexColor, {textureSize.x, 0}};
            m_vertices[16] = m_vertices[12];
            m_vertices[17] = {{m_size.x, middleRect.top}, vertexColor, {textureSize.x, middleRect.top}};
            m_vertices[18] = m_vertices[10];
            m_vertices[19] = {{m_size.x, m_size.y - (textureSize.y - middleRect.top - middleRect.height)}, vertexColor, {textureSize.x, middleRect.top + middleRect.height}};
            m_vertices[20] = m_vertices[8];
            m_vertices[21] = {{m_size.x, m_size.y}, vertexColor, {textureSize.x, textureSize.y}};
            break;
        };
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Sprite::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if (!isSet())
            return;

        // A rotation can cause the image to be shifted, so we move it upfront so that it ends at the correct location
        if (getRotation() != 0)
        {
            Vector2f pos = {getTransform().transformRect(FloatRect({}, getSize())).left,
                            getTransform().transformRect(FloatRect({}, getSize())).top};

            states.transform.translate(getPosition() - pos);
        }

        states.transform *= getTransform();

        // Apply clipping when needed
#if TGUI_COMPILED_WITH_CPP_VER >= 17
        std::optional<Clipping> clipping;
        if (m_visibleRect != FloatRect{})
            clipping.emplace(target, states, Vector2f{m_visibleRect.left, m_visibleRect.top}, Vector2f{m_visibleRect.width, m_visibleRect.height});
#else
        std::unique_ptr<Clipping> clipping;
        if (m_visibleRect != FloatRect{0, 0, 0, 0})
            clipping = std::make_unique<Clipping>(target, states, Vector2f{m_visibleRect.left, m_visibleRect.top}, Vector2f{m_visibleRect.width, m_visibleRect.height});
#endif

        if (m_texture.getData()->svgImage)
            states.texture = m_svgTexture.get();
        else
            states.texture = &m_texture.getData()->texture;

        states.shader = m_shader;
        target.draw(m_vertices.data(), m_vertices.size(), sf::PrimitiveType::TrianglesStrip, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


#include <TGUI/String.hpp>

#include <cctype> // tolower, toupper, isspace

#if TGUI_COMPILED_WITH_CPP_VER >= 17
    #include <charconv>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    const decltype(std::u32string::npos) String::npos = std::u32string::npos;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool isWhitespace(char character)
    {
        if (character == ' ' || character == '\t' || character == '\r' || character == '\n')
            return true;
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef TGUI_NEXT
    bool isWhitespace(uint32_t character)
#else
    bool isWhitespace(sf::Uint32 character)
#endif
    {
        if (character == U' ' || character == U'\t' || character == U'\r' || character == U'\n')
            return true;
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int strToInt(const std::string& str, int defaultValue)
    {
#if TGUI_COMPILED_WITH_CPP_VER >= 17
        int value;
        const char* cstr = str.c_str();
        if (std::from_chars(&cstr[0], &cstr[str.length()], value).ec == std::errc{})
            return value;
        else
            return defaultValue;
#else
        try
        {
            return std::stoi(str);
        }
        catch (const std::exception&)
        {
            return defaultValue;
        }
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int strToUInt(const std::string& str, unsigned int defaultValue)
    {
#if TGUI_COMPILED_WITH_CPP_VER >= 17
        unsigned int value;
        const char* cstr = str.c_str();
        if (std::from_chars(&cstr[0], &cstr[str.length()], value).ec == std::errc{})
            return value;
        else
            return defaultValue;
#else
        try
        {
            return static_cast<unsigned int>(std::stoi(str));
        }
        catch (const std::exception&)
        {
            return defaultValue;
        }
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float strToFloat(const std::string& str, float defaultValue)
    {
#if TGUI_COMPILED_WITH_CPP_VER >= 17
        float value;
        const char* cstr = str.c_str();
        if (std::from_chars(&cstr[0], &cstr[str.length()], value).ec == std::errc{})
            return value;
        else
            return defaultValue;
#else
        try
        {
            return std::stof(str);
        }
        catch (const std::exception&)
        {
            return defaultValue;
        }
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string toLower(const std::string& oldString)
    {
        std::string newString;
        newString.reserve(oldString.length() + 1);
        for (const char& c : oldString)
            newString.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));

        return newString;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string toUpper(const std::string& oldString)
    {
        std::string newString;
        newString.reserve(oldString.length() + 1);
        for (const char& c : oldString)
            newString.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(c))));

        return newString;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string trim(const std::string& str)
    {
        const auto firstIt = std::find_if(str.begin(), str.end(), [](unsigned char c) { return !std::isspace(c); });
        if (firstIt == str.end())
            return {};

        const auto lastIt = std::find_if(str.rbegin(), str.rend(), [](unsigned char c) { return !std::isspace(c); }).base();
        return std::string(firstIt, lastIt);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int String::toInt(int defaultValue) const
    {
        std::string ansiString;
        ansiString.reserve(m_string.length() + 1);
        sf::Utf32::toLatin1(m_string.begin(), m_string.end(), std::back_inserter(ansiString), 0);

        return strToInt(ansiString, defaultValue);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float String::toFloat(float defaultValue) const
    {
        std::string ansiString;
        ansiString.reserve(m_string.length() + 1);
        sf::Utf32::toLatin1(m_string.begin(), m_string.end(), std::back_inserter(ansiString), 0);

        return strToFloat(ansiString, defaultValue);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String String::trim() const
    {
        const auto firstIt = std::find_if(m_string.begin(), m_string.end(), [](char32_t c) { return (c >= 256) || !std::isspace(static_cast<unsigned char>(c)); });
        if (firstIt == m_string.end())
            return {};

        const auto lastIt = std::find_if(m_string.rbegin(), m_string.rend(), [](char32_t c) { return (c >= 256) || !std::isspace(static_cast<unsigned char>(c)); }).base();
        return {firstIt, lastIt};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String String::toLower() const
    {
        String str;
        str.reserve(m_string.length() + 1);
        for (const char32_t& c : m_string)
        {
            if (c < 256)
                str.push_back(static_cast<char32_t>(std::tolower(static_cast<unsigned char>(c))));
            else
                str.push_back(c);
        }

        return str;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String String::toUpper() const
    {
        String str;
        str.reserve(m_string.length() + 1);
        for (const char32_t& c : m_string)
        {
            if (c < 256)
                str.push_back(static_cast<char32_t>(std::toupper(static_cast<unsigned char>(c))));
            else
                str.push_back(c);
        }

        return str;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef TGUI_NEXT // Code not working on GCC 4.9
    String& String::replace(const String& searchFor, const String& replaceWith)
    {
        std::size_t step = replaceWith.length();
        std::size_t len = searchFor.length();
        std::size_t pos = find(searchFor);

        // Replace each occurrence of search
        while (pos != npos)
        {
            replace(pos, len, replaceWith);
            pos = find(searchFor, pos + step);
        }

        return *this;
    }
#endif
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String::String(const std::string& str)
    {
        m_string.reserve(str.length()+1);
        sf::Utf8::toUtf32(str.begin(), str.end(), std::back_inserter(m_string));
    }

    String::String(const std::wstring& str)
    {
        m_string.reserve(str.length()+1);
        sf::Utf32::fromWide(str.begin(), str.end(), std::back_inserter(m_string));
    }

    String::String(const std::u16string& str)
    {
        m_string.reserve(str.length()+1);
        sf::Utf16::toUtf32(str.begin(), str.end(), std::back_inserter(m_string));
    }

    String::String(const std::u32string& str)
        : m_string{str}
    {
    }

    String::String(char ansiChar, const std::locale& locale)
        : m_string(1, static_cast<char32_t>(sf::Utf32::decodeAnsi(ansiChar, locale)))
    {
    }

    String::String(wchar_t wideChar)
        : m_string(1, static_cast<char32_t>(wideChar))
    {
    }

    String::String(char16_t utfChar)
        : m_string(1, static_cast<char32_t>(utfChar))
    {
    }

    String::String(char32_t utfChar)
        : m_string(1, utfChar)
    {
    }

    String::String(const char* str)
    {
        const std::size_t len = std::strlen(str);
        m_string.reserve(len+1);
        sf::Utf8::toUtf32(str, str + len, std::back_inserter(m_string));
    }

    String::String(const wchar_t* str)
        : String{std::wstring{str}}
    {
    }

    String::String(const char16_t* str)
        : String{std::u16string{str}}
    {
    }

    String::String(const char32_t* str)
        : m_string{str}
    {
    }

    String::String(std::size_t count, char ch)
        : m_string(count, static_cast<char32_t>(sf::Utf32::decodeAnsi(ch)))
    {
    }

    String::String(std::size_t count, wchar_t ch)
        : m_string(count, static_cast<char32_t>(ch))
    {
    }

    String::String(std::size_t count, char16_t ch)
        : m_string(count, static_cast<char32_t>(ch))
    {
    }

    String::String(std::size_t count, char32_t ch)
        : m_string(count, ch)
    {
    }

    String::String(const std::string& str, std::size_t pos)
        : String{std::string(str, pos)}
    {
    }

    String::String(const std::wstring& str, std::size_t pos)
        : String{std::wstring(str, pos)}
    {
    }

    String::String(const std::u16string& str, std::size_t pos)
        : String{std::u16string(str, pos)}
    {
    }

    String::String(const std::u32string& str, std::size_t pos)
        : m_string(str, pos)
    {
    }

    String::String(const std::string& str, std::size_t pos, std::size_t count)
        : String{std::string(str, pos, count)}
    {
    }

    String::String(const std::wstring& str, std::size_t pos, std::size_t count)
        : String{std::wstring(str, pos, count)}
    {
    }

    String::String(const std::u16string& str, std::size_t pos, std::size_t count)
        : String{std::u16string(str, pos, count)}
    {
    }

    String::String(const std::u32string& str, std::size_t pos, std::size_t count)
        : m_string(str, pos, count)
    {
    }

    String::String(const char* str, std::size_t count)
        : String{std::string{str, count}}
    {
    }

    String::String(const wchar_t* str, std::size_t count)
        : String{std::wstring{str, count}}
    {
    }

    String::String(const char16_t* str, std::size_t count)
        : String{std::u16string{str, count}}
    {
    }

    String::String(const char32_t* str, std::size_t count)
        : m_string{str, count}
    {
    }

    String::String(std::initializer_list<char> chars)
        : String(std::string(chars.begin(), chars.end()))
    {
    }

    String::String(std::initializer_list<wchar_t> chars)
        : String(std::wstring(chars.begin(), chars.end()))
    {
    }

    String::String(std::initializer_list<char16_t> chars)
        : String(std::u16string(chars.begin(), chars.end()))
    {
    }

    String::String(std::initializer_list<char32_t> chars)
        : m_string(chars)
    {
    }

    String::String(std::string::const_iterator first, std::string::const_iterator last)
        : String{std::string(first, last)}
    {
    }

    String::String(std::wstring::const_iterator first, std::wstring::const_iterator last)
        : String{std::wstring(first, last)}
    {
    }

    String::String(std::u16string::const_iterator first, std::u16string::const_iterator last)
        : String{std::u16string(first, last)}
    {
    }

    String::String(std::u32string::const_iterator first, std::u32string::const_iterator last)
        : m_string(first, last)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String::operator std::string() const
    {
        return asAnsiString();
    }

    String::operator std::wstring() const
    {
        return asWideString();
    }

    String::operator std::u16string() const
    {
        return asUtf16();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string String::asAnsiString() const
    {
        std::string output;
        output.reserve(m_string.length() + 1);
        sf::Utf32::toUtf8(m_string.begin(), m_string.end(), std::back_inserter(output));
        return output;
    }

    std::wstring String::asWideString() const
    {
        std::wstring output;
        output.reserve(m_string.length() + 1);
        sf::Utf32::toWide(m_string.begin(), m_string.end(), std::back_inserter(output));
        return output;
    }

    std::u16string String::asUtf16() const
    {
        std::u16string output;
        output.reserve(m_string.length() + 1);
        sf::Utf32::toUtf16(m_string.begin(), m_string.end(), std::back_inserter(output));
        return output;
    }

    const std::u32string& String::asUtf32() const
    {
        return m_string;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String& String::assign(std::size_t count, char ch)
    {
        m_string.assign(count, static_cast<char32_t>(sf::Utf32::decodeAnsi(ch)));
        return *this;
    }

    String& String::assign(std::size_t count, wchar_t ch)
    {
        m_string.assign(count, static_cast<char32_t>(ch));
        return *this;
    }

    String& String::assign(std::size_t count, char16_t ch)
    {
        m_string.assign(count, static_cast<char32_t>(ch));
        return *this;
    }

    String& String::assign(std::size_t count, char32_t ch)
    {
        m_string.assign(count, ch);
        return *this;
    }

    String& String::assign(const std::string& str)
    {
        return *this = str;
    }

    String& String::assign(const std::wstring& str)
    {
        return *this = str;
    }

    String& String::assign(const std::u16string& str)
    {
        return *this = str;
    }

    String& String::assign(const std::u32string& str)
    {
        m_string.assign(str);
        return *this;
    }

    String& String::assign(const String& str)
    {
        m_string.assign(str.m_string);
        return *this;
    }

    String& String::assign(const std::string& str, std::size_t pos, std::size_t count)
    {
        return *this = {str, pos, count};
    }

    String& String::assign(const std::wstring& str, std::size_t pos, std::size_t count)
    {
        return *this = {str, pos, count};
    }

    String& String::assign(const std::u16string& str, std::size_t pos, std::size_t count)
    {
        return *this = {str, pos, count};
    }

    String& String::assign(const std::u32string& str, std::size_t pos, std::size_t count)
    {
        m_string.assign(str, pos, count);
        return *this;
    }

    String& String::assign(const String& str, std::size_t pos, std::size_t count)
    {
        m_string.assign(str.m_string, pos, count);
        return *this;
    }

    String& String::assign(std::string&& str)
    {
        return *this = std::move(str);
    }

    String& String::assign(std::wstring&& str)
    {
        return *this = std::move(str);
    }

    String& String::assign(std::u16string&& str)
    {
        return *this = std::move(str);
    }

    String& String::assign(std::u32string&& str)
    {
        m_string.assign(std::move(str));
        return *this;
    }

    String& String::assign(String&& str)
    {
        m_string.assign(std::move(str.m_string));
        return *this;
    }

    String& String::assign(const char* str, std::size_t count)
    {
        return *this = {str, count};
    }

    String& String::assign(const wchar_t* str, std::size_t count)
    {
        return *this = {str, count};
    }

    String& String::assign(const char16_t* str, std::size_t count)
    {
        return *this = {str, count};
    }

    String& String::assign(const char32_t* str, std::size_t count)
    {
        m_string.assign(str, count);
        return *this;
    }

    String& String::assign(const char* str)
    {
        return *this = str;
    }

    String& String::assign(const wchar_t* str)
    {
        return *this = str;
    }

    String& String::assign(const char16_t* str)
    {
        return *this = str;
    }

    String& String::assign(const char32_t* str)
    {
        m_string.assign(str);
        return *this;
    }

    String& String::assign(std::initializer_list<char> chars)
    {
        return *this = chars;
    }

    String& String::assign(std::initializer_list<wchar_t> chars)
    {
        return *this = chars;
    }

    String& String::assign(std::initializer_list<char16_t> chars)
    {
        return *this = chars;
    }

    String& String::assign(std::initializer_list<char32_t> chars)
    {
        m_string.assign(chars);
        return *this;
    }

    String& String::assign(std::string::const_iterator first, std::string::const_iterator last)
    {
        return *this = {first, last};
    }

    String& String::assign(std::wstring::const_iterator first, std::wstring::const_iterator last)
    {
        return *this = {first, last};
    }

    String& String::assign(std::u16string::const_iterator first, std::u16string::const_iterator last)
    {
        return *this = {first, last};
    }

    String& String::assign(std::u32string::const_iterator first, std::u32string::const_iterator last)
    {
        m_string.assign(first, last);
        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String::reference       String::at(std::size_t pos)
    {
        return m_string.at(pos);
    }

    String::const_reference String::at(std::size_t pos) const
    {
        return m_string.at(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String::const_reference String::operator [](std::size_t index) const
    {
        return m_string[index];
    }

    String::reference String::operator [](std::size_t index)
    {
        return m_string[index];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String::reference String::front()
    {
        return m_string.front();
    }

    String::const_reference String::front() const
    {
        return m_string.front();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String::reference String::back()
    {
        return m_string.back();
    }

    String::const_reference String::back() const
    {
        return m_string.back();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const char32_t* String::data() const noexcept
    {
        return m_string.data();
    }

    const char32_t* String::c_str() const noexcept
    {
        return m_string.c_str();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String::iterator String::begin() noexcept
    {
        return m_string.begin();
    }

    String::const_iterator String::begin() const noexcept
    {
        return m_string.begin();
    }

    String::const_iterator String::cbegin() const noexcept
    {
        return m_string.cbegin();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String::iterator String::end() noexcept
    {
        return m_string.end();
    }

    String::const_iterator String::end() const noexcept
    {
        return m_string.end();
    }

    String::const_iterator String::cend() const noexcept
    {
        return m_string.cend();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String::reverse_iterator String::rbegin() noexcept
    {
        return m_string.rbegin();
    }

    String::const_reverse_iterator String::rbegin() const noexcept
    {
        return m_string.rbegin();
    }

    String::const_reverse_iterator String::crbegin() const noexcept
    {
        return m_string.crbegin();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String::reverse_iterator String::rend() noexcept
    {
        return m_string.rend();
    }

    String::const_reverse_iterator String::rend() const noexcept
    {
        return m_string.rend();
    }

    String::const_reverse_iterator String::crend() const noexcept
    {
        return m_string.crend();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void String::clear() noexcept
    {
        m_string.clear();
    }

    bool String::empty() const noexcept
    {
        return m_string.empty();
    }

    std::size_t String::size() const noexcept
    {
        return m_string.size();
    }

    std::size_t String::length() const noexcept
    {
        return m_string.length();
    }

    std::size_t String::max_size() const noexcept
    {
        return m_string.max_size();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void String::reserve(std::size_t newCap)
    {
        m_string.reserve(newCap);
    }

    std::size_t String::capacity() const noexcept
    {
        return m_string.capacity();
    }

    void String::shrink_to_fit()
    {
        m_string.shrink_to_fit();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef TGUI_NEXT // Code not working on GCC 4.9
    String& String::insert(std::size_t index, std::size_t count, char ch)
    {
        m_string.insert(index, count, static_cast<char32_t>(sf::Utf32::decodeAnsi(ch)));
        return *this;
    }

    String& String::insert(std::size_t index, std::size_t count, wchar_t ch)
    {
        m_string.insert(index, count, static_cast<char32_t>(ch));
        return *this;
    }

    String& String::insert(std::size_t index, std::size_t count, char16_t ch)
    {
        m_string.insert(index, count, static_cast<char32_t>(ch));
        return *this;
    }

    String& String::insert(std::size_t index, std::size_t count, char32_t ch)
    {
        m_string.insert(index, count, ch);
        return *this;
    }

    String& String::insert(std::size_t index, const std::string& str)
    {
        m_string.insert(index, String{str}.m_string);
        return *this;
    }

    String& String::insert(std::size_t index, const std::wstring& str)
    {
        m_string.insert(index, String{str}.m_string);
        return *this;
    }

    String& String::insert(std::size_t index, const std::u16string& str)
    {
        m_string.insert(index, String{str}.m_string);
        return *this;
    }

    String& String::insert(std::size_t index, const std::u32string& str)
    {
        m_string.insert(index, str);
        return *this;
    }

    String& String::insert(std::size_t index, const String& str)
    {
        m_string.insert(index, str.m_string);
        return *this;
    }

    String& String::insert(std::size_t index, const std::string& str, std::size_t pos, std::size_t count)
    {
        m_string.insert(index, String{str, pos, count}.m_string);
        return *this;
    }

    String& String::insert(std::size_t index, const std::wstring& str, std::size_t pos, std::size_t count)
    {
        m_string.insert(index, String{str, pos, count}.m_string);
        return *this;
    }
    String& String::insert(std::size_t index, const std::u16string& str, std::size_t pos, std::size_t count)
    {
        m_string.insert(index, String{str, pos, count}.m_string);
        return *this;
    }

    String& String::insert(std::size_t index, const std::u32string& str, std::size_t pos, std::size_t count)
    {
        m_string.insert(index, str, pos, count);
        return *this;
    }

    String& String::insert(std::size_t index, const String& str, std::size_t pos, std::size_t count)
    {
        m_string.insert(index, str.m_string, pos, count);
        return *this;
    }

    String& String::insert(std::size_t index, const char* str, std::size_t count)
    {
        m_string.insert(index, String{str, count}.m_string);
        return *this;
    }

    String& String::insert(std::size_t index, const wchar_t* str, std::size_t count)
    {
        m_string.insert(index, String{str, count}.m_string);
        return *this;
    }

    String& String::insert(std::size_t index, const char16_t* str, std::size_t count)
    {
        m_string.insert(index, String{str, count}.m_string);
        return *this;
    }

    String& String::insert(std::size_t index, const char32_t* str, std::size_t count)
    {
        m_string.insert(index, str, count);
        return *this;
    }

    String& String::insert(std::size_t index, const char* str)
    {
        m_string.insert(index, String{str}.m_string);
        return *this;
    }

    String& String::insert(std::size_t index, const wchar_t* str)
    {
        m_string.insert(index, String{str}.m_string);
        return *this;
    }

    String& String::insert(std::size_t index, const char16_t* str)
    {
        m_string.insert(index, String{str}.m_string);
        return *this;
    }

    String& String::insert(std::size_t index, const char32_t* str)
    {
        m_string.insert(index, str);
        return *this;
    }

    String::iterator String::insert(const_iterator pos, char ch)
    {
        return m_string.insert(pos, static_cast<char32_t>(sf::Utf32::decodeAnsi(ch)));
    }

    String::iterator String::insert(const_iterator pos, wchar_t ch)
    {
        return m_string.insert(pos, static_cast<char32_t>(ch));
    }

    String::iterator String::insert(const_iterator pos, char16_t ch)
    {
        return m_string.insert(pos, static_cast<char32_t>(ch));
    }

    String::iterator String::insert(const_iterator pos, char32_t ch)
    {
        return m_string.insert(pos, ch);
    }

    String::iterator String::insert(const_iterator pos, std::size_t count, char ch)
    {
        return m_string.insert(pos, count, static_cast<char32_t>(sf::Utf32::decodeAnsi(ch)));
    }

    String::iterator String::insert(const_iterator pos, std::size_t count, wchar_t ch)
    {
        return m_string.insert(pos, count, static_cast<char32_t>(ch));
    }

    String::iterator String::insert(const_iterator pos, std::size_t count, char16_t ch)
    {
        return m_string.insert(pos, count, static_cast<char32_t>(ch));
    }

    String::iterator String::insert(const_iterator pos, std::size_t count, char32_t ch)
    {
        return m_string.insert(pos, count, ch);
    }

    String::iterator String::insert(const_iterator pos, std::initializer_list<char> chars)
    {
        const auto tmpStr = String{chars};
        return m_string.insert(pos, tmpStr.begin(), tmpStr.end());
    }

    String::iterator String::insert(const_iterator pos, std::initializer_list<wchar_t> chars)
    {
        const auto tmpStr = String{chars};
        return m_string.insert(pos, tmpStr.begin(), tmpStr.end());
    }

    String::iterator String::insert(const_iterator pos, std::initializer_list<char16_t> chars)
    {
        const auto tmpStr = String{chars};
        return m_string.insert(pos, tmpStr.begin(), tmpStr.end());
    }

    String::iterator String::insert(const_iterator pos, std::initializer_list<char32_t> chars)
    {
        // This function was missing in libstdc++ with GCC 8.2 on Arch Linux. It is present now with GCC 9.1.
        //return m_string.insert(pos, chars);

        const auto tmpStr = String{chars};
        return m_string.insert(pos, tmpStr.begin(), tmpStr.end());
    }

    String::iterator String::insert(const_iterator pos, std::string::const_iterator first, std::string::const_iterator last)
    {
        const auto tmpStr = String{first, last};
        return m_string.insert(pos, tmpStr.begin(), tmpStr.end());
    }

    String::iterator String::insert(const_iterator pos, std::wstring::const_iterator first, std::wstring::const_iterator last)
    {
        const auto tmpStr = String{first, last};
        return m_string.insert(pos, tmpStr.begin(), tmpStr.end());
    }

    String::iterator String::insert(const_iterator pos, std::u16string::const_iterator first, std::u16string::const_iterator last)
    {
        const auto tmpStr = String{first, last};
        return m_string.insert(pos, tmpStr.begin(), tmpStr.end());
    }

    String::iterator String::insert(const_iterator pos, std::u32string::const_iterator first, std::u32string::const_iterator last)
    {
        return m_string.insert(pos, first, last);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String& String::erase(std::size_t index, std::size_t count)
    {
        m_string.erase(index, count);
        return *this;
    }

    String::iterator String::erase(const_iterator position)
    {
        return m_string.erase(position);
    }

    String::iterator String::erase(const_iterator first, const_iterator last)
    {
        return m_string.erase(first, last);
    }
#endif
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void String::push_back(char ch)
    {
        m_string.push_back(static_cast<char32_t>(sf::Utf32::decodeAnsi(ch)));
    }

    void String::push_back(wchar_t ch)
    {
        m_string.push_back(static_cast<char32_t>(ch));
    }

    void String::push_back(char16_t ch)
    {
        m_string.push_back(static_cast<char32_t>(ch));
    }

    void String::push_back(char32_t ch)
    {
        m_string.push_back(ch);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void String::pop_back()
    {
        m_string.pop_back();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String& String::append(std::size_t count, char ch)
    {
        m_string.append(count, static_cast<char32_t>(sf::Utf32::decodeAnsi(ch)));
        return *this;
    }

    String& String::append(std::size_t count, wchar_t ch)
    {
        m_string.append(count, static_cast<char32_t>(ch));
        return *this;
    }

    String& String::append(std::size_t count, char16_t ch)
    {
        m_string.append(count, static_cast<char32_t>(ch));
        return *this;
    }

    String& String::append(std::size_t count, char32_t ch)
    {
        m_string.append(count, ch);
        return *this;
    }

    String& String::append(const std::string& str)
    {
        return append(String{str});
    }

    String& String::append(const std::wstring& str)
    {
        return append(String{str});
    }

    String& String::append(const std::u16string& str)
    {
        return append(String{str});
    }

    String& String::append(const std::u32string& str)
    {
        m_string.append(str);
        return *this;
    }

    String& String::append(const String& str)
    {
        m_string.append(str.m_string);
        return *this;
    }

    String& String::append(const std::string& str, std::size_t pos, std::size_t count)
    {
        return append(String{str, pos, count});
    }

    String& String::append(const std::wstring& str, std::size_t pos, std::size_t count)
    {
        return append(String{str, pos, count});
    }

    String& String::append(const std::u16string& str, std::size_t pos, std::size_t count)
    {
        return append(String{str, pos, count});
    }

    String& String::append(const std::u32string& str, std::size_t pos, std::size_t count)
    {
        m_string.append(str, pos, count);
        return *this;
    }

    String& String::append(const String& str, std::size_t pos, std::size_t count)
    {
        m_string.append(str.m_string, pos, count);
        return *this;
    }

    String& String::append(const char* str, std::size_t count)
    {
        return append(String{str, count});
    }

    String& String::append(const wchar_t* str, std::size_t count)
    {
        return append(String{str, count});
    }

    String& String::append(const char16_t* str, std::size_t count)
    {
        return append(String{str, count});
    }

    String& String::append(const char32_t* str, std::size_t count)
    {
        m_string.append(str, count);
        return *this;
    }

    String& String::append(const char* str)
    {
        return append(String{str});
    }

    String& String::append(const wchar_t* str)
    {
        return append(String{str});
    }

    String& String::append(const char16_t* str)
    {
        return append(String{str});
    }

    String& String::append(const char32_t* str)
    {
        m_string.append(str);
        return *this;
    }

    String& String::append(std::initializer_list<char> chars)
    {
        return append(String{chars});
    }

    String& String::append(std::initializer_list<wchar_t> chars)
    {
        return append(String{chars});
    }

    String& String::append(std::initializer_list<char16_t> chars)
    {
        return append(String{chars});
    }

    String& String::append(std::initializer_list<char32_t> chars)
    {
        m_string.append(chars);
        return *this;
    }

    String& String::append(std::string::const_iterator first, std::string::const_iterator last)
    {
        return append(String{first, last});
    }

    String& String::append(std::wstring::const_iterator first, std::wstring::const_iterator last)
    {
        return append(String{first, last});
    }

    String& String::append(std::u16string::const_iterator first, std::u16string::const_iterator last)
    {
        return append(String{first, last});
    }

    String& String::append(std::u32string::const_iterator first, std::u32string::const_iterator last)
    {
        m_string.append(first, last);
        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String& String::operator+=(const String& str)
    {
        m_string += str.m_string;
        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int String::compare(const std::string& str) const noexcept
    {
        return m_string.compare(String{str});
    }

    int String::compare(const std::wstring& str) const noexcept
    {
        return m_string.compare(String{str});
    }

    int String::compare(const std::u16string& str) const noexcept
    {
        return m_string.compare(String{str});
    }

    int String::compare(const std::u32string& str) const noexcept
    {
        return m_string.compare(str);
    }

    int String::compare(const String& str) const noexcept
    {
        return m_string.compare(str.m_string);
    }

    int String::compare(std::size_t pos1, std::size_t count1, const std::string& str) const
    {
        return m_string.compare(pos1, count1, String{str});
    }

    int String::compare(std::size_t pos1, std::size_t count1, const std::wstring& str) const
    {
        return m_string.compare(pos1, count1, String{str});
    }

    int String::compare(std::size_t pos1, std::size_t count1, const std::u16string& str) const
    {
        return m_string.compare(pos1, count1, String{str});
    }

    int String::compare(std::size_t pos1, std::size_t count1, const std::u32string& str) const
    {
        return m_string.compare(pos1, count1, str);
    }

    int String::compare(std::size_t pos1, std::size_t count1, const String& str) const
    {
        return m_string.compare(pos1, count1, str.m_string);
    }

    int String::compare(std::size_t pos1, std::size_t count1, const std::string& str, std::size_t pos2, std::size_t count2) const
    {
        return m_string.compare(pos1, count1, String{str, pos2, count2});
    }

    int String::compare(std::size_t pos1, std::size_t count1, const std::wstring& str, std::size_t pos2, std::size_t count2) const
    {
        return m_string.compare(pos1, count1, String{str, pos2, count2});
    }

    int String::compare(std::size_t pos1, std::size_t count1, const std::u16string& str, std::size_t pos2, std::size_t count2) const
    {
        return m_string.compare(pos1, count1, String{str, pos2, count2});
    }

    int String::compare(std::size_t pos1, std::size_t count1, const std::u32string& str, std::size_t pos2, std::size_t count2) const
    {
        return m_string.compare(pos1, count1, str, pos2, count2);
    }

    int String::compare(std::size_t pos1, std::size_t count1, const String& str, std::size_t pos2, std::size_t count2) const
    {
        return m_string.compare(pos1, count1, str.m_string, pos2, count2);
    }

    int String::compare(const char* s) const
    {
        return m_string.compare(String{s});
    }

    int String::compare(const wchar_t* s) const
    {
        return m_string.compare(String{s});
    }

    int String::compare(const char16_t* s) const
    {
        return m_string.compare(String{s});
    }

    int String::compare(const char32_t* s) const
    {
        return m_string.compare(s);
    }

    int String::compare(std::size_t pos1, std::size_t count1, const char* s) const
    {
        return m_string.compare(pos1, count1, String{s});
    }

    int String::compare(std::size_t pos1, std::size_t count1, const wchar_t* s) const
    {
        return m_string.compare(pos1, count1, String{s});
    }

    int String::compare(std::size_t pos1, std::size_t count1, const char16_t* s) const
    {
        return m_string.compare(pos1, count1, String{s});
    }

    int String::compare(std::size_t pos1, std::size_t count1, const char32_t* s) const
    {
        return m_string.compare(pos1, count1, s);
    }

    int String::compare(std::size_t pos1, std::size_t count1, const char* s, std::size_t count2) const
    {
        return m_string.compare(pos1, count1, String{s, count2});
    }

    int String::compare(std::size_t pos1, std::size_t count1, const wchar_t* s, std::size_t count2) const
    {
        return m_string.compare(pos1, count1, String{s, count2});
    }

    int String::compare(std::size_t pos1, std::size_t count1, const char16_t* s, std::size_t count2) const
    {
        return m_string.compare(pos1, count1, String{s, count2});
    }

    int String::compare(std::size_t pos1, std::size_t count1, const char32_t* s, std::size_t count2) const
    {
        return m_string.compare(pos1, count1, s, count2);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef TGUI_NEXT // Code not working on GCC 4.9
    String& String::replace(std::size_t pos, std::size_t count, const std::string& str)
    {
        m_string.replace(pos, count, String{str}.m_string);
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, const std::wstring& str)
    {
        m_string.replace(pos, count, String{str}.m_string);
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, const std::u16string& str)
    {
        m_string.replace(pos, count, String{str}.m_string);
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, const std::u32string& str)
    {
        m_string.replace(pos, count, str);
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, const String& str)
    {
        m_string.replace(pos, count, str.m_string);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, const std::string& str)
    {
        m_string.replace(first, last, String{str}.m_string);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, const std::wstring& str)
    {
        m_string.replace(first, last, String{str}.m_string);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, const std::u16string& str)
    {
        m_string.replace(first, last, String{str}.m_string);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, const std::u32string& str)
    {
        m_string.replace(first, last, str);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, const String& str)
    {
        m_string.replace(first, last, str.m_string);
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, const std::string& str, std::size_t pos2, std::size_t count2)
    {
        m_string.replace(pos, count, String{str, pos2, count2}.m_string);
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, const std::wstring& str, std::size_t pos2, std::size_t count2)
    {
        m_string.replace(pos, count, String{str, pos2, count2}.m_string);
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, const std::u16string& str, std::size_t pos2, std::size_t count2)
    {
        m_string.replace(pos, count, String{str, pos2, count2}.m_string);
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, const std::u32string& str, std::size_t pos2, std::size_t count2)
    {
        m_string.replace(pos, count, str, pos2, count2);
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, const String& str, std::size_t pos2, std::size_t count2)
    {
        m_string.replace(pos, count, str.m_string, pos2, count2);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, std::string::const_iterator first2, std::string::const_iterator last2)
    {
        m_string.replace(first, last, String{first2, last2}.m_string);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, std::wstring::const_iterator first2, std::wstring::const_iterator last2)
    {
        m_string.replace(first, last, String{first2, last2}.m_string);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, std::u16string::const_iterator first2, std::u16string::const_iterator last2)
    {
        m_string.replace(first, last, String{first2, last2}.m_string);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, std::u32string::const_iterator first2, std::u32string::const_iterator last2)
    {
        m_string.replace(first, last, first2, last2);
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, const char* cstr, std::size_t count2)
    {
        m_string.replace(pos, count, String{cstr, count2}.m_string);
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, const wchar_t* cstr, std::size_t count2)
    {
        m_string.replace(pos, count, String{cstr, count2}.m_string);
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, const char16_t* cstr, std::size_t count2)
    {
        m_string.replace(pos, count, String{cstr, count2}.m_string);
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, const char32_t* cstr, std::size_t count2)
    {
        m_string.replace(pos, count, cstr, count2);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, const char* cstr, std::size_t count2)
    {
        m_string.replace(first, last, String{cstr, count2}.m_string);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, const wchar_t* cstr, std::size_t count2)
    {
        m_string.replace(first, last, String{cstr, count2}.m_string);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, const char16_t* cstr, std::size_t count2)
    {
        m_string.replace(first, last, String{cstr, count2}.m_string);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, const char32_t* cstr, std::size_t count2)
    {
        m_string.replace(first, last, cstr, count2);
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, const char* cstr)
    {
        m_string.replace(pos, count, String{cstr}.m_string);
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, const wchar_t* cstr)
    {
        m_string.replace(pos, count, String{cstr}.m_string);
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, const char16_t* cstr)
    {
        m_string.replace(pos, count, String{cstr}.m_string);
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, const char32_t* cstr)
    {
        m_string.replace(pos, count, cstr);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, const char* cstr)
    {
        m_string.replace(first, last, String{cstr}.m_string);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, const wchar_t* cstr)
    {
        m_string.replace(first, last, String{cstr}.m_string);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, const char16_t* cstr)
    {
        m_string.replace(first, last, String{cstr}.m_string);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, const char32_t* cstr)
    {
        m_string.replace(first, last, cstr);
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, std::size_t count2, char ch)
    {
        m_string.replace(pos, count, String(count2, ch).m_string);
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, std::size_t count2, wchar_t ch)
    {
        m_string.replace(pos, count, String(count2, ch).m_string);
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, std::size_t count2, char16_t ch)
    {
        m_string.replace(pos, count, String(count2, ch).m_string);
        return *this;
    }

    String& String::replace(std::size_t pos, std::size_t count, std::size_t count2, char32_t ch)
    {
        m_string.replace(pos, count, count2, ch);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, std::size_t count2, char ch)
    {
        m_string.replace(first, last, String(count2, ch).m_string);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, std::size_t count2, wchar_t ch)
    {
        m_string.replace(first, last, String(count2, ch).m_string);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, std::size_t count2, char16_t ch)
    {
        m_string.replace(first, last, String(count2, ch).m_string);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, std::size_t count2, char32_t ch)
    {
        m_string.replace(first, last, count2, ch);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, std::initializer_list<char> chars)
    {
        m_string.replace(first, last, String{chars}.m_string);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, std::initializer_list<wchar_t> chars)
    {
        m_string.replace(first, last, String{chars}.m_string);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, std::initializer_list<char16_t> chars)
    {
        m_string.replace(first, last, String{chars}.m_string);
        return *this;
    }

    String& String::replace(const_iterator first, const_iterator last, std::initializer_list<char32_t> chars)
    {
        m_string.replace(first, last, chars);
        return *this;
    }
#endif
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String String::substr(std::size_t pos, std::size_t count) const
    {
        return {m_string.substr(pos, count)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t String::copy(char32_t* dest, std::size_t count, std::size_t pos) const
    {
        return m_string.copy(dest, count, pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void String::resize(std::size_t count)
    {
        m_string.resize(count);
    }

    void String::resize(std::size_t count, char ch)
    {
        m_string.resize(count, static_cast<char32_t>(ch));
    }

    void String::resize(std::size_t count, wchar_t ch)
    {
        m_string.resize(count, static_cast<char32_t>(ch));
    }

    void String::resize(std::size_t count, char16_t ch)
    {
        m_string.resize(count, static_cast<char32_t>(ch));
    }

    void String::resize(std::size_t count, char32_t ch)
    {
        m_string.resize(count, ch);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void String::swap(String& other)
    {
        m_string.swap(other.m_string);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t String::find(const std::string& str, std::size_t pos) const noexcept
    {
        return m_string.find(String{str}.m_string, pos);
    }

    std::size_t String::find(const std::wstring& str, std::size_t pos) const noexcept
    {
        return m_string.find(String{str}.m_string, pos);
    }

    std::size_t String::find(const std::u16string& str, std::size_t pos) const noexcept
    {
        return m_string.find(String{str}.m_string, pos);
    }

    std::size_t String::find(const std::u32string& str, std::size_t pos) const noexcept
    {
        return m_string.find(str, pos);
    }

    std::size_t String::find(const String& str, std::size_t pos) const noexcept
    {
        return m_string.find(str.m_string, pos);
    }

    std::size_t String::find(const char* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find(String{s, count}.m_string, pos);
    }

    std::size_t String::find(const wchar_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find(String{s, count}.m_string, pos);
    }

    std::size_t String::find(const char16_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find(String{s, count}.m_string, pos);
    }

    std::size_t String::find(const char32_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find(s, pos, count);
    }

    std::size_t String::find(const char* s, std::size_t pos) const
    {
        return m_string.find(String{s}.m_string, pos);
    }

    std::size_t String::find(const wchar_t* s, std::size_t pos) const
    {
        return m_string.find(String{s}.m_string, pos);
    }

    std::size_t String::find(const char16_t* s, std::size_t pos) const
    {
        return m_string.find(String{s}.m_string, pos);
    }

    std::size_t String::find(const char32_t* s, std::size_t pos) const
    {
        return m_string.find(s, pos);
    }

    std::size_t String::find(char ch, std::size_t pos) const noexcept
    {
        return m_string.find(static_cast<char32_t>(ch), pos);
    }

    std::size_t String::find(wchar_t ch, std::size_t pos) const noexcept
    {
        return m_string.find(static_cast<char32_t>(ch), pos);
    }

    std::size_t String::find(char16_t ch, std::size_t pos) const noexcept
    {
        return m_string.find(static_cast<char32_t>(ch), pos);
    }

    std::size_t String::find(char32_t ch, std::size_t pos) const noexcept
    {
        return m_string.find(ch, pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t String::find_first_of(const std::string& str, std::size_t pos) const noexcept
    {
        return m_string.find_first_of(String{str}.m_string, pos);
    }

    std::size_t String::find_first_of(const std::wstring& str, std::size_t pos) const noexcept
    {
        return m_string.find_first_of(String{str}.m_string, pos);
    }

    std::size_t String::find_first_of(const std::u16string& str, std::size_t pos) const noexcept
    {
        return m_string.find_first_of(String{str}.m_string, pos);
    }

    std::size_t String::find_first_of(const std::u32string& str, std::size_t pos) const noexcept
    {
        return m_string.find_first_of(str, pos);
    }

    std::size_t String::find_first_of(const String& str, std::size_t pos) const noexcept
    {
        return m_string.find_first_of(str.m_string, pos);
    }

    std::size_t String::find_first_of(const char* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find_first_of(String{s, count}.m_string, pos);
    }

    std::size_t String::find_first_of(const wchar_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find_first_of(String{s, count}.m_string, pos);
    }

    std::size_t String::find_first_of(const char16_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find_first_of(String{s, count}.m_string, pos);
    }

    std::size_t String::find_first_of(const char32_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find_first_of(s, pos, count);
    }

    std::size_t String::find_first_of(const char* s, std::size_t pos) const
    {
        return m_string.find_first_of(String{s}.m_string, pos);
    }

    std::size_t String::find_first_of(const wchar_t* s, std::size_t pos) const
    {
        return m_string.find_first_of(String{s}.m_string, pos);
    }

    std::size_t String::find_first_of(const char16_t* s, std::size_t pos) const
    {
        return m_string.find_first_of(String{s}.m_string, pos);
    }

    std::size_t String::find_first_of(const char32_t* s, std::size_t pos) const
    {
        return m_string.find_first_of(s, pos);
    }

    std::size_t String::find_first_of(char ch, std::size_t pos) const noexcept
    {
        return m_string.find_first_of(static_cast<char32_t>(ch), pos);
    }

    std::size_t String::find_first_of(wchar_t ch, std::size_t pos) const noexcept
    {
        return m_string.find_first_of(static_cast<char32_t>(ch), pos);
    }

    std::size_t String::find_first_of(char16_t ch, std::size_t pos) const noexcept
    {
        return m_string.find_first_of(static_cast<char32_t>(ch), pos);
    }

    std::size_t String::find_first_of(char32_t ch, std::size_t pos) const noexcept
    {
        return m_string.find_first_of(ch, pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t String::find_first_not_of(const std::string& str, std::size_t pos) const noexcept
    {
        return m_string.find_first_not_of(String{str}.m_string, pos);
    }

    std::size_t String::find_first_not_of(const std::wstring& str, std::size_t pos) const noexcept
    {
        return m_string.find_first_not_of(String{str}.m_string, pos);
    }

    std::size_t String::find_first_not_of(const std::u16string& str, std::size_t pos) const noexcept
    {
        return m_string.find_first_not_of(String{str}.m_string, pos);
    }

    std::size_t String::find_first_not_of(const std::u32string& str, std::size_t pos) const noexcept
    {
        return m_string.find_first_not_of(str, pos);
    }

    std::size_t String::find_first_not_of(const String& str, std::size_t pos) const noexcept
    {
        return m_string.find_first_not_of(str.m_string, pos);
    }

    std::size_t String::find_first_not_of(const char* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find_first_not_of(String{s, count}.m_string, pos);
    }

    std::size_t String::find_first_not_of(const wchar_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find_first_not_of(String{s, count}.m_string, pos);
    }

    std::size_t String::find_first_not_of(const char16_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find_first_not_of(String{s, count}.m_string, pos);
    }

    std::size_t String::find_first_not_of(const char32_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find_first_not_of(s, pos, count);
    }

    std::size_t String::find_first_not_of(const char* s, std::size_t pos) const
    {
        return m_string.find_first_not_of(String{s}.m_string, pos);
    }

    std::size_t String::find_first_not_of(const wchar_t* s, std::size_t pos) const
    {
        return m_string.find_first_not_of(String{s}.m_string, pos);
    }

    std::size_t String::find_first_not_of(const char16_t* s, std::size_t pos) const
    {
        return m_string.find_first_not_of(String{s}.m_string, pos);
    }

    std::size_t String::find_first_not_of(const char32_t* s, std::size_t pos) const
    {
        return m_string.find_first_not_of(s, pos);
    }

    std::size_t String::find_first_not_of(char ch, std::size_t pos) const noexcept
    {
        return m_string.find_first_not_of(static_cast<char32_t>(ch), pos);
    }

    std::size_t String::find_first_not_of(wchar_t ch, std::size_t pos) const noexcept
    {
        return m_string.find_first_not_of(static_cast<char32_t>(ch), pos);
    }

    std::size_t String::find_first_not_of(char16_t ch, std::size_t pos) const noexcept
    {
        return m_string.find_first_not_of(static_cast<char32_t>(ch), pos);
    }

    std::size_t String::find_first_not_of(char32_t ch, std::size_t pos) const noexcept
    {
        return m_string.find_first_not_of(ch, pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t String::rfind(const std::string& str, std::size_t pos) const noexcept
    {
        return m_string.rfind(String{str}.m_string, pos);
    }

    std::size_t String::rfind(const std::wstring& str, std::size_t pos) const noexcept
    {
        return m_string.rfind(String{str}.m_string, pos);
    }

    std::size_t String::rfind(const std::u16string& str, std::size_t pos) const noexcept
    {
        return m_string.rfind(String{str}.m_string, pos);
    }

    std::size_t String::rfind(const std::u32string& str, std::size_t pos) const noexcept
    {
        return m_string.rfind(str, pos);
    }

    std::size_t String::rfind(const String& str, std::size_t pos) const noexcept
    {
        return m_string.rfind(str.m_string, pos);
    }

    std::size_t String::rfind(const char* s, std::size_t pos, std::size_t count) const
    {
        return m_string.rfind(String{s, count}.m_string, pos);
    }

    std::size_t String::rfind(const wchar_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.rfind(String{s, count}.m_string, pos);
    }

    std::size_t String::rfind(const char16_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.rfind(String{s, count}.m_string, pos);
    }

    std::size_t String::rfind(const char32_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.rfind(s, pos, count);
    }

    std::size_t String::rfind(const char* s, std::size_t pos) const
    {
        return m_string.rfind(String{s}.m_string, pos);
    }

    std::size_t String::rfind(const wchar_t* s, std::size_t pos) const
    {
        return m_string.rfind(String{s}.m_string, pos);
    }

    std::size_t String::rfind(const char16_t* s, std::size_t pos) const
    {
        return m_string.rfind(String{s}.m_string, pos);
    }

    std::size_t String::rfind(const char32_t* s, std::size_t pos) const
    {
        return m_string.rfind(s, pos);
    }

    std::size_t String::rfind(char ch, std::size_t pos) const noexcept
    {
        return m_string.rfind(static_cast<char32_t>(ch), pos);
    }

    std::size_t String::rfind(wchar_t ch, std::size_t pos) const noexcept
    {
        return m_string.rfind(static_cast<char32_t>(ch), pos);
    }

    std::size_t String::rfind(char16_t ch, std::size_t pos) const noexcept
    {
        return m_string.rfind(static_cast<char32_t>(ch), pos);
    }

    std::size_t String::rfind(char32_t ch, std::size_t pos) const noexcept
    {
        return m_string.rfind(ch, pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t String::find_last_of(const std::string& str, std::size_t pos) const noexcept
    {
        return m_string.find_last_of(String{str}.m_string, pos);
    }

    std::size_t String::find_last_of(const std::wstring& str, std::size_t pos) const noexcept
    {
        return m_string.find_last_of(String{str}.m_string, pos);
    }

    std::size_t String::find_last_of(const std::u16string& str, std::size_t pos) const noexcept
    {
        return m_string.find_last_of(String{str}.m_string, pos);
    }

    std::size_t String::find_last_of(const std::u32string& str, std::size_t pos) const noexcept
    {
        return m_string.find_last_of(str, pos);
    }

    std::size_t String::find_last_of(const String& str, std::size_t pos) const noexcept
    {
        return m_string.find_last_of(str.m_string, pos);
    }

    std::size_t String::find_last_of(const char* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find_last_of(String{s, count}.m_string, pos);
    }

    std::size_t String::find_last_of(const wchar_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find_last_of(String{s, count}.m_string, pos);
    }

    std::size_t String::find_last_of(const char16_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find_last_of(String{s, count}.m_string, pos);
    }

    std::size_t String::find_last_of(const char32_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find_last_of(s, pos, count);
    }

    std::size_t String::find_last_of(const char* s, std::size_t pos) const
    {
        return m_string.find_last_of(String{s}.m_string, pos);
    }

    std::size_t String::find_last_of(const wchar_t* s, std::size_t pos) const
    {
        return m_string.find_last_of(String{s}.m_string, pos);
    }

    std::size_t String::find_last_of(const char16_t* s, std::size_t pos) const
    {
        return m_string.find_last_of(String{s}.m_string, pos);
    }

    std::size_t String::find_last_of(const char32_t* s, std::size_t pos) const
    {
        return m_string.find_last_of(s, pos);
    }

    std::size_t String::find_last_of(char ch, std::size_t pos) const noexcept
    {
        return m_string.find_last_of(static_cast<char32_t>(ch), pos);
    }

    std::size_t String::find_last_of(wchar_t ch, std::size_t pos) const noexcept
    {
        return m_string.find_last_of(static_cast<char32_t>(ch), pos);
    }

    std::size_t String::find_last_of(char16_t ch, std::size_t pos) const noexcept
    {
        return m_string.find_last_of(static_cast<char32_t>(ch), pos);
    }

    std::size_t String::find_last_of(char32_t ch, std::size_t pos) const noexcept
    {
        return m_string.find_last_of(ch, pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t String::find_last_not_of(const std::string& str, std::size_t pos) const noexcept
    {
        return m_string.find_last_not_of(String{str}.m_string, pos);
    }

    std::size_t String::find_last_not_of(const std::wstring& str, std::size_t pos) const noexcept
    {
        return m_string.find_last_not_of(String{str}.m_string, pos);
    }

    std::size_t String::find_last_not_of(const std::u16string& str, std::size_t pos) const noexcept
    {
        return m_string.find_last_not_of(String{str}.m_string, pos);
    }

    std::size_t String::find_last_not_of(const std::u32string& str, std::size_t pos) const noexcept
    {
        return m_string.find_last_not_of(str, pos);
    }

    std::size_t String::find_last_not_of(const String& str, std::size_t pos) const noexcept
    {
        return m_string.find_last_not_of(str.m_string, pos);
    }

    std::size_t String::find_last_not_of(const char* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find_last_not_of(String{s, count}.m_string, pos);
    }

    std::size_t String::find_last_not_of(const wchar_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find_last_not_of(String{s, count}.m_string, pos);
    }

    std::size_t String::find_last_not_of(const char16_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find_last_not_of(String{s, count}.m_string, pos);
    }

    std::size_t String::find_last_not_of(const char32_t* s, std::size_t pos, std::size_t count) const
    {
        return m_string.find_last_not_of(s, pos, count);
    }

    std::size_t String::find_last_not_of(const char* s, std::size_t pos) const
    {
        return m_string.find_last_not_of(String{s}.m_string, pos);
    }

    std::size_t String::find_last_not_of(const wchar_t* s, std::size_t pos) const
    {
        return m_string.find_last_not_of(String{s}.m_string, pos);
    }

    std::size_t String::find_last_not_of(const char16_t* s, std::size_t pos) const
    {
        return m_string.find_last_not_of(String{s}.m_string, pos);
    }

    std::size_t String::find_last_not_of(const char32_t* s, std::size_t pos) const
    {
        return m_string.find_last_not_of(s, pos);
    }

    std::size_t String::find_last_not_of(char ch, std::size_t pos) const noexcept
    {
        return m_string.find_last_not_of(static_cast<char32_t>(ch), pos);
    }

    std::size_t String::find_last_not_of(wchar_t ch, std::size_t pos) const noexcept
    {
        return m_string.find_last_not_of(static_cast<char32_t>(ch), pos);
    }

    std::size_t String::find_last_not_of(char16_t ch, std::size_t pos) const noexcept
    {
        return m_string.find_last_not_of(static_cast<char32_t>(ch), pos);
    }

    std::size_t String::find_last_not_of(char32_t ch, std::size_t pos) const noexcept
    {
        return m_string.find_last_not_of(ch, pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const String& str)
    {
        os << std::string(str);
        return os;
    }

    std::basic_ostream<wchar_t>& operator<<(std::basic_ostream<wchar_t>& os, const String& str)
    {
        os << std::wstring(str);
        return os;
    }

// Code didn't compile with AppleClang 9.1 (Xcode 9.4). Android build also failed, so libc++ issue?
/*
    std::basic_ostream<char16_t>& operator<<(std::basic_ostream<char16_t>& os, const String& str)
    {
        os << std::u16string(str);
        return os;
    }

    std::basic_ostream<char32_t>& operator<<(std::basic_ostream<char32_t>& os, const String& str)
    {
        os << std::u32string(str);
        return os;
    }
*/

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


#include <TGUI/SvgImage.hpp>

#define NANOSVG_IMPLEMENTATION
#include "TGUI/nanosvg/nanosvg.h"

#define NANOSVGRAST_IMPLEMENTATION
#include "TGUI/nanosvg/nanosvgrast.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SvgImage::SvgImage(const sf::String& filename)
    {
#ifdef SFML_SYSTEM_WINDOWS
        const std::string filenameAnsiString(filename.toAnsiString());
#else
        const std::basic_string<sf::Uint8>& filenameUtf8 = filename.toUtf8();
        const std::string filenameAnsiString(filenameUtf8.begin(), filenameUtf8.end());
#endif

        m_svg = nsvgParseFromFile(filenameAnsiString.c_str(), "px", 96);
        if (!m_svg)
            TGUI_PRINT_WARNING("Failed to load svg: " << filenameAnsiString);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SvgImage::~SvgImage()
    {
        if (m_rasterizer)
            nsvgDeleteRasterizer(m_rasterizer);
        if (m_svg)
            nsvgDelete(m_svg);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SvgImage::isSet() const
    {
        return (m_svg != nullptr);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f SvgImage::getSize() const
    {
        if (m_svg)
            return {static_cast<float>(m_svg->width), static_cast<float>(m_svg->height)};
        else
            return {0, 0};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SvgImage::rasterize(sf::Texture& texture, sf::Vector2u size)
    {
        if (!m_svg)
            return;

        if (!m_rasterizer)
            m_rasterizer = nsvgCreateRasterizer();

        if (texture.getSize() != size)
        {
            if (!texture.create(size.x, size.y))
                return;
        }

        const float scaleX = size.x / static_cast<float>(m_svg->width);
        const float scaleY = size.y / static_cast<float>(m_svg->height);

        auto pixels = std::make_unique<unsigned char[]>(size.x * size.y * 4);
        nsvgRasterizeFull(m_rasterizer, m_svg, 0, 0, static_cast<double>(scaleX), static_cast<double>(scaleY), pixels.get(), size.x, size.y, size.x * 4);

        texture.update(pixels.get(), size.x, size.y, 0, 0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


#include <TGUI/Texture.hpp>
#include <TGUI/TextureManager.hpp>
#include <TGUI/Exception.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    std::map<sf::String, std::list<TextureDataHolder>> TextureManager::m_imageMap;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<TextureData> TextureManager::getTexture(Texture& texture, const sf::String& filename, const sf::IntRect& partRect)
    {
        // Look if we already had this image
        auto imageIt = m_imageMap.find(filename);
        if (imageIt != m_imageMap.end())
        {
            // Loop all our textures to find the one containing the image
            for (auto dataIt = imageIt->second.begin(); dataIt != imageIt->second.end(); ++dataIt)
            {
                // Only reuse the texture when the exact same part of the image is used
                if (dataIt->data->rect == partRect)
                {
                    // The texture is now used at multiple places
                    ++(dataIt->users);

                    // Let the texture alert the texture manager when it is being copied or destroyed
                    texture.setCopyCallback(&TextureManager::copyTexture);
                    texture.setDestructCallback(&TextureManager::removeTexture);
                    return dataIt->data;
                }
            }
        }
        else // The image doesn't exist yet
        {
            auto it = m_imageMap.insert({filename, {}});
            imageIt = it.first;
        }

        // Add new data to the list
        TextureDataHolder dataHolder;
        dataHolder.filename = filename;
        dataHolder.users = 1;
        dataHolder.data = std::make_shared<TextureData>();
        dataHolder.data->rect = partRect;
        imageIt->second.push_back(std::move(dataHolder));

        // Let the texture alert the texture manager when it is being copied or destroyed
        texture.setCopyCallback(&TextureManager::copyTexture);
        texture.setDestructCallback(&TextureManager::removeTexture);

        // Load the image
        auto data = imageIt->second.back().data;
        if ((filename.getSize() > 4) && (toLower(filename.substring(filename.getSize() - 4, 4)) == ".svg"))
        {
            data->svgImage = std::make_unique<SvgImage>(filename);
            if (data->svgImage->isSet())
                return data;
        }
        else // Not an svg
        {
            // Share the image if it was loaded before
            if (imageIt->second.size() >= 2)
                data->image = imageIt->second.begin()->data->image;

            if (!data->image)
                data->image = texture.getImageLoader()(filename);
            if (data->image)
            {
                // Create a texture from the image
                bool loadFromImageSuccess;
                if (partRect == sf::IntRect{})
                    loadFromImageSuccess = data->texture.loadFromImage(*data->image);
                else
                {
                    if ((partRect.left < static_cast<int>(data->image->getSize().x)) && (partRect.top < static_cast<int>(data->image->getSize().y)))
                        loadFromImageSuccess = data->texture.loadFromImage(*data->image, partRect);
                    else
                        loadFromImageSuccess = false;
                }

                if (loadFromImageSuccess)
                    return data;
            }
        }

        // The image could not be loaded
        if (imageIt->second.size() > 1)
            imageIt->second.pop_back();
        else
            m_imageMap.erase(imageIt);

        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextureManager::copyTexture(std::shared_ptr<TextureData> textureDataToCopy)
    {
        // Loop all our textures to check if we already have this one
        for (auto& dataHolder : m_imageMap)
        {
            for (auto& data : dataHolder.second)
            {
                // Check if the pointer points to our texture
                if (data.data == textureDataToCopy)
                {
                    // The texture is now used at multiple places
                    ++data.users;
                    return;
                }
            }
        }

        throw Exception{"Trying to copy texture data that was not loaded by the TextureManager."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextureManager::removeTexture(std::shared_ptr<TextureData> textureDataToRemove)
    {
        // Loop all our textures to check which one it is
        for (auto imageIt = m_imageMap.begin(); imageIt != m_imageMap.end(); ++imageIt)
        {
            for (auto dataIt = imageIt->second.begin(); dataIt != imageIt->second.end(); ++dataIt)
            {
                // Check if the pointer points to our texture
                if (dataIt->data == textureDataToRemove)
                {
                    // If this was the only place where the texture is used then delete it
                    if (--(dataIt->users) == 0)
                    {
                        imageIt->second.erase(dataIt);
                        if (imageIt->second.empty())
                            m_imageMap.erase(imageIt);
                    }

                    return;
                }
            }
        }

        throw Exception{"Trying to remove a texture that was not loaded by the TextureManager."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


#include <TGUI/ToolTip.hpp>
#include <TGUI/Container.hpp>
#include <TGUI/Animation.hpp>
#include <TGUI/Vector2f.hpp>
#include <TGUI/Loading/WidgetFactory.hpp>
#include <TGUI/SignalManager.hpp>
#include <SFML/System/Err.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    namespace
    {
        void finishExistingConflictingAnimations(std::vector<std::shared_ptr<priv::Animation>>& animations, ShowAnimationType type)
        {
            // Only one animation of each type can be played at the same type. If e.g. a fade animation was already in progress
            // when starting a new one, the old animation is finished immediately.
            // Different types of animations (e.g. fading and moving) can occur at the same time.
            auto animIt = animations.begin();
            while (animIt != animations.end())
            {
                auto& animation = *animIt;
                if (((type == ShowAnimationType::Fade) && (animation->getType() == priv::Animation::Type::Fade))
                 || ((type == ShowAnimationType::Scale) && (animation->getType() == priv::Animation::Type::Resize))
                 || ((type != ShowAnimationType::Fade) && (animation->getType() == priv::Animation::Type::Move)))
                {
                    animation->finish();
                    animIt = animations.erase(animIt);
                }
                else
                    ++animIt;
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        static Layout2d parseLayout(std::string str)
        {
            if (str.empty())
                throw Exception{"Failed to parse layout '" + str + "'. String was empty."};

            // Remove the brackets around the value
            if (((str.front() == '(') && (str.back() == ')')) || ((str.front() == '{') && (str.back() == '}')))
                str = str.substr(1, str.length() - 2);

            if (str.empty())
                return {0, 0};

            // Find the comma that splits the x and y layouts, taking into account that these layouts may contain commas too
            unsigned int bracketCount = 0;
            auto commaOrBracketPos = str.find_first_of(",()");
            decltype(commaOrBracketPos) commaPos = 0;
            while (commaOrBracketPos != std::string::npos)
            {
                if (str[commaOrBracketPos] == '(')
                    bracketCount++;
                else if (str[commaOrBracketPos] == ')')
                {
                    if (bracketCount == 0)
                        throw Exception{"Failed to parse layout '" + str + "'. Brackets didn't match."};

                    bracketCount--;
                }
                else // if (str[commaOrBracketPos] == ',')
                {
                    if (bracketCount == 0)
                        commaPos = commaOrBracketPos;
                }

                commaOrBracketPos = str.find_first_of(",()", commaOrBracketPos + 1);
            }

            // Remove quotes around the values
            std::string x = trim(str.substr(0, commaPos));
            if ((x.size() >= 2) && ((x[0] == '"') && (x[x.length()-1] == '"')))
                x = x.substr(1, x.length()-2);

            std::string y = trim(str.substr(commaPos + 1));
            if ((y.size() >= 2) && ((y[0] == '"') && (y[y.length()-1] == '"')))
                y = y.substr(1, y.length()-2);

            return {x, y};
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Widget()
    {
        m_renderer->subscribe(this, m_rendererChangedCallback);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::~Widget()
    {
        // The renderer will be null when the widget was moved
        if (m_renderer)
            m_renderer->unsubscribe(this);

        for (auto& layout : m_boundPositionLayouts)
            layout->unbindWidget();

        for (auto& layout : m_boundSizeLayouts)
            layout->unbindWidget();

        SignalManager::getSignalManager()->remove(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Widget(const Widget& other) :
        SignalWidgetBase               {other},
        enable_shared_from_this<Widget>{other},
        m_type                         {other.m_type},
        m_name                         {other.m_name},
        m_position                     {other.m_position},
        m_size                         {other.m_size},
        m_textSize                     {other.m_textSize},
        m_boundPositionLayouts         {},
        m_boundSizeLayouts             {},
        m_enabled                      {other.m_enabled},
        m_visible                      {other.m_visible},
        m_parent                       {nullptr},
        m_draggableWidget              {other.m_draggableWidget},
        m_containerWidget              {other.m_containerWidget},
        m_toolTip                      {other.m_toolTip ? other.m_toolTip->clone() : nullptr},
        m_renderer                     {other.m_renderer},
        m_showAnimations               {other.m_showAnimations},
        m_fontCached                   {other.m_fontCached},
        m_opacityCached                {other.m_opacityCached}
    {
        m_position.x.connectWidget(this, true, [this]{ setPosition(getPositionLayout()); });
        m_position.y.connectWidget(this, false, [this]{ setPosition(getPositionLayout()); });
        m_size.x.connectWidget(this, true, [this]{ setSize(getSizeLayout()); });
        m_size.y.connectWidget(this, false, [this]{ setSize(getSizeLayout()); });

        m_renderer->subscribe(this, m_rendererChangedCallback);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Widget(Widget&& other) :
        SignalWidgetBase               {std::move(other)},
        enable_shared_from_this<Widget>{std::move(other)},
        onPositionChange               {std::move(other.onPositionChange)},
        onSizeChange                   {std::move(other.onSizeChange)},
        onFocus                        {std::move(other.onFocus)},
        onUnfocus                      {std::move(other.onUnfocus)},
        onMouseEnter                   {std::move(other.onMouseEnter)},
        onMouseLeave                   {std::move(other.onMouseLeave)},
        m_type                         {std::move(other.m_type)},
        m_name                         {std::move(other.m_name)},
        m_position                     {std::move(other.m_position)},
        m_size                         {std::move(other.m_size)},
        m_textSize                     {std::move(other.m_textSize)},
        m_boundPositionLayouts         {std::move(other.m_boundPositionLayouts)},
        m_boundSizeLayouts             {std::move(other.m_boundSizeLayouts)},
        m_enabled                      {std::move(other.m_enabled)},
        m_visible                      {std::move(other.m_visible)},
        m_parent                       {nullptr},
        m_mouseHover                   {std::move(other.m_mouseHover)},
        m_mouseDown                    {std::move(other.m_mouseDown)},
        m_focused                      {std::move(other.m_focused)},
        m_animationTimeElapsed         {std::move(other.m_animationTimeElapsed)},
        m_draggableWidget              {std::move(other.m_draggableWidget)},
        m_containerWidget              {std::move(other.m_containerWidget)},
        m_toolTip                      {std::move(other.m_toolTip)},
        m_renderer                     {other.m_renderer},
        m_showAnimations               {std::move(other.m_showAnimations)},
        m_fontCached                   {std::move(other.m_fontCached)},
        m_opacityCached                {std::move(other.m_opacityCached)}
    {
        m_position.x.connectWidget(this, true, [this]{ setPosition(getPositionLayout()); });
        m_position.y.connectWidget(this, false, [this]{ setPosition(getPositionLayout()); });
        m_size.x.connectWidget(this, true, [this]{ setSize(getSizeLayout()); });
        m_size.y.connectWidget(this, false, [this]{ setSize(getSizeLayout()); });

        other.m_renderer->unsubscribe(&other);
        m_renderer->subscribe(this, m_rendererChangedCallback);

        other.m_renderer = nullptr;

        if (other.m_parent)
            m_parent->remove(other.shared_from_this());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget& Widget::operator=(const Widget& other)
    {
        if (this != &other)
        {
            m_renderer->unsubscribe(this);

            SignalWidgetBase::operator=(other);
            enable_shared_from_this::operator=(other);

            onPositionChange.disconnectAll();
            onSizeChange.disconnectAll();
            onFocus.disconnectAll();
            onUnfocus.disconnectAll();
            onMouseEnter.disconnectAll();
            onMouseLeave.disconnectAll();

            m_type                 = other.m_type;
            m_name                 = other.m_name;
            m_position             = other.m_position;
            m_size                 = other.m_size;
            m_textSize             = other.m_textSize;
            m_boundPositionLayouts = {};
            m_boundSizeLayouts     = {};
            m_enabled              = other.m_enabled;
            m_visible              = other.m_visible;
            m_mouseHover           = false;
            m_mouseDown            = false;
            m_focused              = false;
            m_animationTimeElapsed = {};
            m_draggableWidget      = other.m_draggableWidget;
            m_containerWidget      = other.m_containerWidget;
            m_toolTip              = other.m_toolTip ? other.m_toolTip->clone() : nullptr;
            m_renderer             = other.m_renderer;
            m_showAnimations       = {};
            m_fontCached           = other.m_fontCached;
            m_opacityCached        = other.m_opacityCached;

            m_position.x.connectWidget(this, true, [this]{ setPosition(getPositionLayout()); });
            m_position.y.connectWidget(this, false, [this]{ setPosition(getPositionLayout()); });
            m_size.x.connectWidget(this, true, [this]{ setSize(getSizeLayout()); });
            m_size.y.connectWidget(this, false, [this]{ setSize(getSizeLayout()); });

            m_renderer->subscribe(this, m_rendererChangedCallback);

            if (m_parent)
            {
                SignalManager::getSignalManager()->remove(this);
                SignalManager::getSignalManager()->add(shared_from_this());
            }
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget& Widget::operator=(Widget&& other)
    {
        if (this != &other)
        {
            m_renderer->unsubscribe(this);
            other.m_renderer->unsubscribe(&other);

            SignalWidgetBase::operator=(std::move(other));
            enable_shared_from_this::operator=(std::move(other));

            onPositionChange       = std::move(other.onPositionChange);
            onSizeChange           = std::move(other.onSizeChange);
            onFocus                = std::move(other.onFocus);
            onUnfocus              = std::move(other.onUnfocus);
            onMouseEnter           = std::move(other.onMouseEnter);
            onMouseLeave           = std::move(other.onMouseLeave);
            m_type                 = std::move(other.m_type);
            m_name                 = std::move(other.m_name);
            m_position             = std::move(other.m_position);
            m_size                 = std::move(other.m_size);
            m_textSize             = std::move(other.m_textSize);
            m_boundPositionLayouts = std::move(other.m_boundPositionLayouts);
            m_boundSizeLayouts     = std::move(other.m_boundSizeLayouts);
            m_enabled              = std::move(other.m_enabled);
            m_visible              = std::move(other.m_visible);
            m_mouseHover           = std::move(other.m_mouseHover);
            m_mouseDown            = std::move(other.m_mouseDown);
            m_focused              = std::move(other.m_focused);
            m_animationTimeElapsed = std::move(other.m_animationTimeElapsed);
            m_draggableWidget      = std::move(other.m_draggableWidget);
            m_containerWidget      = std::move(other.m_containerWidget);
            m_toolTip              = std::move(other.m_toolTip);
            m_renderer             = std::move(other.m_renderer);
            m_showAnimations       = std::move(other.m_showAnimations);
            m_fontCached           = std::move(other.m_fontCached);
            m_opacityCached        = std::move(other.m_opacityCached);

            m_position.x.connectWidget(this, true, [this]{ setPosition(getPositionLayout()); });
            m_position.y.connectWidget(this, false, [this]{ setPosition(getPositionLayout()); });
            m_size.x.connectWidget(this, true, [this]{ setSize(getSizeLayout()); });
            m_size.y.connectWidget(this, false, [this]{ setSize(getSizeLayout()); });

            m_renderer->subscribe(this, m_rendererChangedCallback);

            other.m_renderer = nullptr;

            if (m_parent)
            {
                SignalManager::getSignalManager()->remove(&other);
                SignalManager::getSignalManager()->add(shared_from_this());
            }
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setRenderer(std::shared_ptr<RendererData> rendererData)
    {
        if (rendererData == nullptr)
            rendererData = RendererData::create();

        std::shared_ptr<RendererData> oldData = m_renderer->getData();

        // Update the data
        m_renderer->unsubscribe(this);
        m_renderer->setData(rendererData);
        m_renderer->subscribe(this, m_rendererChangedCallback);
        rendererData->shared = true;

        // Tell the widget about all the updated properties, both new ones and old ones that were now reset to their default value
        auto oldIt = oldData->propertyValuePairs.begin();
        auto newIt = rendererData->propertyValuePairs.begin();
        while (oldIt != oldData->propertyValuePairs.end() && newIt != rendererData->propertyValuePairs.end())
        {
            if (oldIt->first < newIt->first)
            {
                // Update values that no longer exist in the new renderer and are now reset to the default value
                rendererChanged(oldIt->first);
                ++oldIt;
            }
            else
            {
                // Update changed and new properties
                rendererChanged(newIt->first);

                if (newIt->first < oldIt->first)
                    ++newIt;
                else
                {
                    ++oldIt;
                    ++newIt;
                }
            }
        }
        while (oldIt != oldData->propertyValuePairs.end())
        {
            rendererChanged(oldIt->first);
            ++oldIt;
        }
        while (newIt != rendererData->propertyValuePairs.end())
        {
            rendererChanged(newIt->first);
            ++newIt;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const WidgetRenderer* Widget::getSharedRenderer() const
    {
        // You should not be allowed to call setters on the renderer when the widget is const
        return m_renderer.get();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    WidgetRenderer* Widget::getSharedRenderer()
    {
        return m_renderer.get();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const WidgetRenderer* Widget::getRenderer() const
    {
        if (m_renderer->getData()->shared)
        {
            m_renderer->unsubscribe(this);
            m_renderer->setData(m_renderer->clone());
            m_renderer->subscribe(this, m_rendererChangedCallback);
            m_renderer->getData()->shared = false;
        }

        // You should not be allowed to call setters on the renderer when the widget is const
        return m_renderer.get();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    WidgetRenderer* Widget::getRenderer()
    {
        if (m_renderer->getData()->shared)
        {
            m_renderer->unsubscribe(this);
            m_renderer->setData(m_renderer->clone());
            m_renderer->subscribe(this, m_rendererChangedCallback);
            m_renderer->getData()->shared = false;
        }

        return m_renderer.get();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setPosition(const Layout2d& position)
    {
        m_position = position;
        m_position.x.connectWidget(this, true, [this]{ setPosition(getPositionLayout()); });
        m_position.y.connectWidget(this, false, [this]{ setPosition(getPositionLayout()); });

        if (getPosition() != m_prevPosition)
        {
            m_prevPosition = getPosition();
            onPositionChange.emit(this, getPosition());

            for (auto& layout : m_boundPositionLayouts)
                layout->recalculateValue();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setSize(const Layout2d& size)
    {
        m_size = size;
        m_size.x.connectWidget(this, true, [this]{ setSize(getSizeLayout()); });
        m_size.y.connectWidget(this, false, [this]{ setSize(getSizeLayout()); });

        if (getSize() != m_prevSize)
        {
            m_prevSize = getSize();
            onSizeChange.emit(this, getSize());

            for (auto& layout : m_boundSizeLayouts)
                layout->recalculateValue();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Widget::getFullSize() const
    {
        return getSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Widget::getAbsolutePosition() const
    {
        if (m_parent)
            return m_parent->getAbsolutePosition() + m_parent->getChildWidgetsOffset() + getPosition();
        else
            return getPosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Widget::getWidgetOffset() const
    {
        return Vector2f{0, 0};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::showWithEffect(ShowAnimationType type, sf::Time duration)
    {
        setVisible(true);

        // We store the state the widget is currently in. In the event another animation was already playing, we should try to
        // use the current state to start our animation at, but this is not the state that the widget should end at. We must
        // get this state BEFORE finishing the previous animation which is done by finishExistingConflictingAnimations.
        const float startOpacity = getInheritedOpacity();
        //const Vector2f startPosition = getPosition();
        //const Vector2f startSize = getSize();

        finishExistingConflictingAnimations(m_showAnimations, type);

        switch (type)
        {
            case ShowAnimationType::Fade:
            {
                // Start from fully transparent, unless a fade animation was already playing
                float animStartOpacity = startOpacity;
                const float endOpacity = getInheritedOpacity();
                if (startOpacity == endOpacity)
                {
                    setInheritedOpacity(0);
                    animStartOpacity = 0;
                }
                else // If fading was already in progress then adapt the duration to finish the animation sooner
                    duration *= (startOpacity / endOpacity);

                m_showAnimations.push_back(std::make_shared<priv::FadeAnimation>(shared_from_this(), animStartOpacity, endOpacity, duration,
                                                                                 TGUI_LAMBDA_CAPTURE_EQ_THIS{onAnimationFinished.emit(this, type, true); }));
                break;
            }
            case ShowAnimationType::Scale:
            {
                m_showAnimations.push_back(std::make_shared<priv::MoveAnimation>(shared_from_this(), getPosition() + (getSize() / 2.f), getPosition(), duration));
                m_showAnimations.push_back(std::make_shared<priv::ResizeAnimation>(shared_from_this(), Vector2f{0, 0}, getSize(), duration,
                                                                                   TGUI_LAMBDA_CAPTURE_EQ_THIS{onAnimationFinished.emit(this, type, true); }));
                setPosition(getPosition() + (getSize() / 2.f));
                setSize(0, 0);
                break;
            }
            case ShowAnimationType::SlideFromLeft:
            {
                m_showAnimations.push_back(std::make_shared<priv::MoveAnimation>(shared_from_this(), Vector2f{-getFullSize().x, getPosition().y}, getPosition(), duration,
                                                                                 TGUI_LAMBDA_CAPTURE_EQ_THIS{onAnimationFinished.emit(this, type, true); }));
                setPosition({-getFullSize().x, getPosition().y});
                break;
            }
            case ShowAnimationType::SlideFromRight:
            {
                if (getParent())
                {
                    m_showAnimations.push_back(std::make_shared<priv::MoveAnimation>(shared_from_this(), Vector2f{getParent()->getSize().x + getWidgetOffset().x, getPosition().y}, getPosition(), duration,
                                                                                     TGUI_LAMBDA_CAPTURE_EQ_THIS{onAnimationFinished.emit(this, type, true); }));
                    setPosition({getParent()->getSize().x + getWidgetOffset().x, getPosition().y});
                }
                else
                {
                    TGUI_PRINT_WARNING("showWithEffect(SlideFromRight) does not work before widget has a parent.");
                }

                break;
            }
            case ShowAnimationType::SlideFromTop:
            {
                m_showAnimations.push_back(std::make_shared<priv::MoveAnimation>(shared_from_this(), Vector2f{getPosition().x, -getFullSize().y}, getPosition(), duration,
                                                                                 TGUI_LAMBDA_CAPTURE_EQ_THIS{onAnimationFinished.emit(this, type, true); }));
                setPosition({getPosition().x, -getFullSize().y});
                break;
            }
            case ShowAnimationType::SlideFromBottom:
            {
                if (getParent())
                {
                    m_showAnimations.push_back(std::make_shared<priv::MoveAnimation>(shared_from_this(), Vector2f{getPosition().x, getParent()->getSize().y + getWidgetOffset().y}, getPosition(), duration,
                                                                                     TGUI_LAMBDA_CAPTURE_EQ_THIS{onAnimationFinished.emit(this, type, true); }));
                    setPosition({getPosition().x, getParent()->getSize().y + getWidgetOffset().y});
                }
                else
                {
                    TGUI_PRINT_WARNING("showWithEffect(SlideFromBottom) does not work before widget has a parent.");
                }

                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::hideWithEffect(ShowAnimationType type, sf::Time duration)
    {
        // We store the state the widget is currently in. In the event another animation was already playing, we should try to
        // use the current state to start our animation at, but this is not the state that the widget should end at. We must
        // get this state BEFORE finishing the previous animation which is done by finishExistingConflictingAnimations.
        const float startOpacity = getInheritedOpacity();
        //const Vector2f startPosition = getPosition();
        //const Vector2f startSize = getSize();

        finishExistingConflictingAnimations(m_showAnimations, type);

        const auto position = getPosition();

        switch (type)
        {
            case ShowAnimationType::Fade:
            {
                const float endOpacity = getInheritedOpacity(); // Value to reset to after widget is hidden

                // If fading was already in progress then adapt the duration to finish the animation sooner
                if (startOpacity != endOpacity)
                    duration *= (startOpacity / endOpacity);

                m_showAnimations.push_back(std::make_shared<priv::FadeAnimation>(shared_from_this(), startOpacity, 0.f, duration,
                    TGUI_LAMBDA_CAPTURE_EQ_THIS{ setVisible(false); setInheritedOpacity(endOpacity); onAnimationFinished.emit(this, type, false); }));
                break;
            }
            case ShowAnimationType::Scale:
            {
                const auto size = getSize();
                m_showAnimations.push_back(std::make_shared<priv::MoveAnimation>(shared_from_this(), position, position + (size / 2.f), duration));
                m_showAnimations.push_back(std::make_shared<priv::ResizeAnimation>(shared_from_this(), size, Vector2f{0, 0}, duration,
                    TGUI_LAMBDA_CAPTURE_EQ_THIS{ setVisible(false); setPosition(position); setSize(size); onAnimationFinished.emit(this, type, false); }));
                break;
            }
            case ShowAnimationType::SlideToRight:
            {
                if (getParent())
                {
                    m_showAnimations.push_back(std::make_shared<priv::MoveAnimation>(shared_from_this(), position, Vector2f{getParent()->getSize().x + getWidgetOffset().x, position.y}, duration,
                        TGUI_LAMBDA_CAPTURE_EQ_THIS{ setVisible(false); setPosition(position); onAnimationFinished.emit(this, type, false); }));
                }
                else
                {
                    TGUI_PRINT_WARNING("hideWithEffect(SlideToRight) does not work before widget has a parent.");
                }

                break;
            }
            case ShowAnimationType::SlideToLeft:
            {
                m_showAnimations.push_back(std::make_shared<priv::MoveAnimation>(shared_from_this(), position, Vector2f{-getFullSize().x, position.y}, duration,
                    TGUI_LAMBDA_CAPTURE_EQ_THIS{ setVisible(false); setPosition(position); onAnimationFinished.emit(this, type, false); }));
                break;
            }
            case ShowAnimationType::SlideToBottom:
            {
                if (getParent())
                {
                    m_showAnimations.push_back(std::make_shared<priv::MoveAnimation>(shared_from_this(), position, Vector2f{position.x, getParent()->getSize().y + getWidgetOffset().y}, duration,
                        TGUI_LAMBDA_CAPTURE_EQ_THIS{ setVisible(false); setPosition(position); onAnimationFinished.emit(this, type, false); }));
                }
                else
                {
                    TGUI_PRINT_WARNING("hideWithEffect(SlideToBottom) does not work before widget has a parent.");
                }

                break;
            }
            case ShowAnimationType::SlideToTop:
            {
                m_showAnimations.push_back(std::make_shared<priv::MoveAnimation>(shared_from_this(), position, Vector2f{position.x, -getFullSize().y}, duration,
                    TGUI_LAMBDA_CAPTURE_EQ_THIS{ setVisible(false); setPosition(position); onAnimationFinished.emit(this, type, false); }));
                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setVisible(bool visible)
    {
        m_visible = visible;

        // If the widget is hiden while still focused then it must be unfocused
        if (!visible)
            setFocused(false);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setEnabled(bool enabled)
    {
        m_enabled = enabled;

        if (!enabled)
        {
            m_mouseHover = false;
            m_mouseDown = false;
            setFocused(false);
        }

        // Refresh widget opacity if there is a different value set for enabled and disabled widgets
        if (getSharedRenderer()->getOpacityDisabled() != -1)
            rendererChanged("opacitydisabled");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setFocused(bool focused)
    {
        if (m_focused == focused)
            return;

        if (focused)
        {
            if (canGainFocus())
            {
                m_focused = true;

                if (m_parent)
                    m_parent->childWidgetFocused(shared_from_this());

                onFocus.emit(this);
            }
        }
        else // Unfocusing widget
        {
            m_focused = false;
            onUnfocus.emit(this);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& Widget::getWidgetType() const
    {
        return m_type;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Widget::isAnimationPlaying() const
    {
        return !m_showAnimations.empty();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::moveToFront()
    {
        if (m_parent)
            m_parent->moveWidgetToFront(shared_from_this());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::moveToBack()
    {
        if (m_parent)
            m_parent->moveWidgetToBack(shared_from_this());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setInheritedFont(const Font& font)
    {
        m_inheritedFont = font;
        rendererChanged("font");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Font& Widget::getInheritedFont() const
    {
        return m_inheritedFont;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setInheritedOpacity(float opacity)
    {
        m_inheritedOpacity = opacity;
        rendererChanged("opacity");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Widget::getInheritedOpacity() const
    {
        return m_inheritedOpacity;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setTextSize(unsigned int size)
    {
        m_textSize = size;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Widget::getTextSize() const
    {
        return m_textSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setToolTip(Widget::Ptr toolTip)
    {
        m_toolTip = toolTip;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Widget::getToolTip() const
    {
        return m_toolTip;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setWidgetName(const sf::String& name)
    {
        if (m_name != name)
        {
            m_name = name;
            if (m_parent)
            {
                SignalManager::getSignalManager()->remove(this);
                SignalManager::getSignalManager()->add(shared_from_this());
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String Widget::getWidgetName() const
    {
        return m_name;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setFocusable(bool focusable)
    {
        m_focusable = focusable;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Widget::isFocusable() const
    {
        return m_focusable;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Widget::canGainFocus() const
    {
        return m_enabled && m_visible && m_focusable;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Widget::isContainer() const
    {
        return m_containerWidget;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setParent(Container* parent)
    {
        if (!parent)
        {
            SignalManager::getSignalManager()->remove(this);
        }
        else if (!m_parent)
        {
            SignalManager::getSignalManager()->add(shared_from_this());
        }

        m_parent = parent;

        // Give the layouts another chance to find widgets to which it refers
        m_position.x.connectWidget(this, true, [this]{ setPosition(getPositionLayout()); });
        m_position.y.connectWidget(this, false, [this]{ setPosition(getPositionLayout()); });
        m_size.x.connectWidget(this, true, [this]{ setSize(getSizeLayout()); });
        m_size.y.connectWidget(this, false, [this]{ setSize(getSizeLayout()); });
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::update(sf::Time elapsedTime)
    {
        m_animationTimeElapsed += elapsedTime;

        for (unsigned int i = 0; i < m_showAnimations.size();)
        {
            if (m_showAnimations[i]->update(elapsedTime))
                m_showAnimations.erase(m_showAnimations.begin() + i);
            else
                i++;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::leftMousePressed(Vector2f)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::leftMouseReleased(Vector2f)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::rightMousePressed(Vector2f)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::rightMouseReleased(Vector2f)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mousePressed(sf::Mouse::Button button, Vector2f pos)
    {
        if (button == sf::Mouse::Left)
            leftMousePressed(pos);
        else if (button == sf::Mouse::Right)
            rightMousePressed(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseReleased(sf::Mouse::Button button, Vector2f pos)
    {
        if (button == sf::Mouse::Left)
            leftMouseReleased(pos);
        else if (button == sf::Mouse::Right)
            rightMouseReleased(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseMoved(Vector2f)
    {
        if (!m_mouseHover)
            mouseEnteredWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::keyPressed(const sf::Event::KeyEvent&)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::textEntered(std::uint32_t)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Widget::mouseWheelScrolled(float, Vector2f)
    {
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseNoLongerOnWidget()
    {
        if (m_mouseHover)
            mouseLeftWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TGUI_REMOVE_DEPRECATED_CODE
    void Widget::mouseNoLongerDown()
    {
        leftMouseButtonNoLongerDown();
    }
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::leftMouseButtonNoLongerDown()
    {
        m_mouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::rightMouseButtonNoLongerDown()
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Widget::askToolTip(Vector2f mousePos)
    {
        if (m_toolTip && mouseOnWidget(mousePos))
            return getToolTip();
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::bindPositionLayout(Layout* layout)
    {
        m_boundPositionLayouts.insert(layout);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::unbindPositionLayout(Layout* layout)
    {
        m_boundPositionLayouts.erase(layout);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::bindSizeLayout(Layout* layout)
    {
        m_boundSizeLayouts.insert(layout);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::unbindSizeLayout(Layout* layout)
    {
        m_boundSizeLayouts.erase(layout);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& Widget::getSignal(std::string signalName)
    {
        if (signalName == toLower(onPositionChange.getName()))
            return onPositionChange;
        else if (signalName == toLower(onSizeChange.getName()))
            return onSizeChange;
        else if (signalName == toLower(onFocus.getName()))
            return onFocus;
        else if (signalName == toLower(onUnfocus.getName()))
            return onUnfocus;
        else if (signalName == toLower(onMouseEnter.getName()))
            return onMouseEnter;
        else if (signalName == toLower(onMouseLeave.getName()))
            return onMouseLeave;
        else if (signalName == toLower(onAnimationFinished.getName()))
            return onAnimationFinished;

        throw Exception{"No signal exists with name '" + std::move(signalName) + "'."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::rendererChanged(const std::string& property)
    {
        if ((property == "opacity") || (property == "opacitydisabled"))
        {
            if (!m_enabled && (getSharedRenderer()->getOpacityDisabled() != -1))
                m_opacityCached = getSharedRenderer()->getOpacityDisabled() * m_inheritedOpacity;
            else
                m_opacityCached = getSharedRenderer()->getOpacity() * m_inheritedOpacity;
        }
        else if (property == "font")
        {
            if (getSharedRenderer()->getFont())
                m_fontCached = getSharedRenderer()->getFont();
            else if (m_inheritedFont)
                m_fontCached = m_inheritedFont;
            else
                m_fontCached = getGlobalFont();
        }
        else if (property == "transparenttexture")
        {
            m_transparentTextureCached = getSharedRenderer()->getTransparentTexture();
        }
        else
            throw Exception{"Could not set property '" + property + "', widget of type '" + getWidgetType() + "' does not has this property."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> Widget::save(SavingRenderersMap& renderers) const
    {
        sf::String widgetName;
        widgetName = m_name;

        auto node = std::make_unique<DataIO::Node>();
        if (widgetName.isEmpty())
            node->name = getWidgetType();
        else
            node->name = getWidgetType() + "." + Serializer::serialize(widgetName);

        if (!isVisible())
            node->propertyValuePairs["Visible"] = std::make_unique<DataIO::ValueNode>("false");
        if (!isEnabled())
            node->propertyValuePairs["Enabled"] = std::make_unique<DataIO::ValueNode>("false");
        if (getPosition() != Vector2f{})
            node->propertyValuePairs["Position"] = std::make_unique<DataIO::ValueNode>(m_position.toString());
        if (getSize() != Vector2f{})
            node->propertyValuePairs["Size"] = std::make_unique<DataIO::ValueNode>(m_size.toString());
#if TGUI_COMPILED_WITH_CPP_VER >= 17
        if (m_userData.has_value())
        {
            try
            {
                const sf::String string = std::any_cast<sf::String>(m_userData);
                node->propertyValuePairs["UserData"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(string.toAnsiString()));
            }
            catch (const std::bad_any_cast&)
            {
                try
                {
                    const std::string string = std::any_cast<std::string>(m_userData);
                    node->propertyValuePairs["UserData"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(string));
                }
                catch (const std::bad_any_cast&)
                {
                    try
                    {
                        const std::string string = std::any_cast<const char*>(m_userData);
                        node->propertyValuePairs["UserData"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(string));
                    }
                    catch (const std::bad_any_cast&)
                    {
                    }
                }
            }
        }
#else
        if (m_userData.not_null())
        {
            if (m_userData.is<sf::String>())
            {
                node->propertyValuePairs["UserData"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_userData.as<sf::String>().toAnsiString()));
            }
            else if (m_userData.is<std::string>())
            {
                node->propertyValuePairs["UserData"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_userData.as<std::string>()));
            }
            else if (m_userData.is<const char*>())
            {
                node->propertyValuePairs["UserData"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_userData.as<const char*>()));
            }
        }
#endif

        if (getToolTip() != nullptr)
        {
            auto toolTipWidgetNode = getToolTip()->save(renderers);

            auto toolTipNode = std::make_unique<DataIO::Node>();
            toolTipNode->name = "ToolTip";
            toolTipNode->children.emplace_back(std::move(toolTipWidgetNode));

            toolTipNode->propertyValuePairs["InitialDelay"] = std::make_unique<DataIO::ValueNode>(to_string(ToolTip::getInitialDelay().asSeconds()));
            toolTipNode->propertyValuePairs["DistanceToMouse"] = std::make_unique<DataIO::ValueNode>("(" + to_string(ToolTip::getDistanceToMouse().x) + "," + to_string(ToolTip::getDistanceToMouse().y) + ")");

            node->children.emplace_back(std::move(toolTipNode));
        }

        if (renderers.at(this).first)
            node->children.emplace_back(std::move(renderers.at(this).first));
        else
            node->propertyValuePairs["renderer"] = std::make_unique<DataIO::ValueNode>("&" + renderers.at(this).second);

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        if (node->propertyValuePairs["visible"])
            setVisible(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["visible"]->value).getBool());
        if (node->propertyValuePairs["enabled"])
            setEnabled(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["enabled"]->value).getBool());
        if (node->propertyValuePairs["position"])
            setPosition(parseLayout(node->propertyValuePairs["position"]->value));
        if (node->propertyValuePairs["size"])
            setSize(parseLayout(node->propertyValuePairs["size"]->value));
        if (node->propertyValuePairs["userdata"])
        {
#if TGUI_COMPILED_WITH_CPP_VER >= 17
            m_userData = std::make_any<std::string>(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["userdata"]->value).getString().toAnsiString());
#else
            m_userData = tgui::Any(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["userdata"]->value).getString().toAnsiString());
#endif
        }

        if (node->propertyValuePairs["renderer"])
        {
            const sf::String value = node->propertyValuePairs["renderer"]->value;
            if (value.isEmpty() || (value[0] != '&'))
                throw Exception{"Expected reference to renderer, did not find '&' character"};

            const auto it = renderers.find(toLower(value.substring(1)));
            if (it == renderers.end())
                throw Exception{"Widget refers to renderer with name '" + value.substring(1) + "', but no such renderer was found"};

            setRenderer(it->second);
        }

        for (const auto& childNode : node->children)
        {
            if (toLower(childNode->name) == "tooltip")
            {
                for (const auto& pair : childNode->propertyValuePairs)
                {
                    if (pair.first == "initialdelay")
                        ToolTip::setInitialDelay(sf::seconds(strToFloat(pair.second->value)));
#ifndef TGUI_REMOVE_DEPRECATED_CODE
                    else if (pair.first == "timetodisplay")
                        ToolTip::setInitialDelay(sf::seconds(strToFloat(pair.second->value)));
#endif
                    else if (pair.first == "distancetomouse")
                        ToolTip::setDistanceToMouse(Vector2f{pair.second->value});
                }

                if (!childNode->children.empty())
                {
                    // There can only be one child in the tool tip section
                    if (childNode->children.size() > 1)
                        throw Exception{"ToolTip section contained multiple children."};

                    const auto& toolTipWidgetNode = childNode->children[0];
                    const auto& constructor = WidgetFactory::getConstructFunction(toolTipWidgetNode->name);
                    if (constructor)
                    {
                        Widget::Ptr toolTip = constructor();
                        toolTip->load(toolTipWidgetNode, renderers);
                        setToolTip(toolTip);
                    }
                    else
                        throw Exception{"No construct function exists for widget type '" + toolTipWidgetNode->name + "'."};
                }
            }
            else if (toLower(childNode->name) == "renderer")
                setRenderer(RendererData::createFromDataIONode(childNode.get()));

            /// TODO: Signals?
        }
        node->children.erase(std::remove_if(node->children.begin(), node->children.end(), [](const std::unique_ptr<DataIO::Node>& child){
                return (toLower(child->name) == "tooltip") || (toLower(child->name) == "renderer");
            }), node->children.end());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseEnteredWidget()
    {
        m_mouseHover = true;
        onMouseEnter.emit(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseLeftWidget()
    {
        m_mouseHover = false;
        onMouseLeave.emit(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::rendererChangedCallback(const std::string& property)
    {
        rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::drawRectangleShape(sf::RenderTarget& target,
                                    const sf::RenderStates& states,
                                    Vector2f size,
                                    Color color) const
    {
        sf::RectangleShape shape{size};

        if (m_opacityCached < 1)
            shape.setFillColor(Color::calcColorOpacity(color, m_opacityCached));
        else
            shape.setFillColor(color);

        target.draw(shape, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::drawBorders(sf::RenderTarget& target,
                             const sf::RenderStates& states,
                             const Borders& borders,
                             Vector2f size,
                             Color borderColor) const
    {
        const Color color = Color::calcColorOpacity(borderColor, m_opacityCached);

        // If size is too small then draw entire size as border
        if ((size.x <= borders.getLeft() + borders.getRight()) || (size.y <= borders.getTop() + borders.getBottom()))
        {
            sf::RectangleShape border;
            border.setFillColor(color);
            border.setSize({size.x, size.y});
            target.draw(border, states);
        }
        else // Draw borders in the normal way
        {
            //////////////////////
            // 0--1/8---------6 //
            // |              | //
            // |   9------7   | //
            // |   |      |   | //
            // |   |      |   | //
            // |   3------5   | //
            // |              | //
            // 2--------------4 //
            //////////////////////
            const std::vector<sf::Vertex> vertices = {
                {{0, 0}, color},
                {{borders.getLeft(), 0}, color},
                {{0, size.y}, color},
                {{borders.getLeft(), size.y - borders.getBottom()}, color},
                {{size.x, size.y}, color},
                {{size.x - borders.getRight(), size.y - borders.getBottom()}, color},
                {{size.x, 0}, color},
                {{size.x - borders.getRight(), borders.getTop()}, color},
                {{borders.getLeft(), 0}, color},
                {{borders.getLeft(), borders.getTop()}, color}
            };

            target.draw(vertices.data(), vertices.size(), sf::PrimitiveType::TrianglesStrip, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


#include <TGUI/Loading/Deserializer.hpp>
#include <TGUI/Loading/DataIO.hpp>
#include <TGUI/Renderers/WidgetRenderer.hpp>
#include <TGUI/Global.hpp>
#include <cstdint>
#include <cassert>

namespace tgui
{
    namespace
    {
        unsigned char hexToDec(char c)
        {
            assert((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')  || (c >= 'a' && c <= 'f'));

            if (c == 'A' || c == 'a')
                return 10;
            else if (c == 'B' || c == 'b')
                return 11;
            else if (c == 'C' || c == 'c')
                return 12;
            else if (c == 'D' || c == 'd')
                return 13;
            else if (c == 'E' || c == 'e')
                return 14;
            else if (c == 'F' || c == 'f')
                return 15;
            else // if (c >= '0' && c <= '9')
                return c - '0';
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef TGUI_NEXT
        bool removeWhitespace(const std::string& line, std::string::const_iterator& c)
        {
            while (c != line.end())
            {
                if ((*c == ' ') || (*c == '\t') || (*c == '\r'))
                    ++c;
                else
                    return true;
            }

            return false;
        }
#endif

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool readIntRect(std::string value, sf::IntRect& rect)
        {
            if (!value.empty() && (value[0] == '(') && (value[value.length()-1] == ')'))
            {
                std::vector<std::string> tokens = Deserializer::split(value.substr(1, value.size()-2), ',');
                if (tokens.size() == 4)
                {
                    rect = {strToInt(tokens[0]), strToInt(tokens[1]), strToInt(tokens[2]), strToInt(tokens[3])};
                    return true;
                }
            }

            return false;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ObjectConverter deserializeBool(const std::string& value)
        {
            const std::string str = toLower(value);
            if (str == "true" || str == "yes" || str == "on" || str == "1")
                return {true};
            else if (str == "false" || str == "no" || str == "off" || str == "0")
                return {false};
            else
                throw Exception{"Failed to deserialize boolean from '" + str + "'"};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ObjectConverter deserializeFont(const std::string& value)
        {
            if (value == "null" || value == "nullptr")
                return Font{};

            sf::String filename = Deserializer::deserialize(ObjectConverter::Type::String, value).getString();
            if (filename.isEmpty())
                return Font{};

            // Load the font but insert the resource path into the filename unless the filename is an absolute path
            auto font = std::make_shared<sf::Font>();
#ifdef SFML_SYSTEM_WINDOWS
            if ((filename[0] != '/') && (filename[0] != '\\') && ((filename.getSize() <= 1) || (filename[1] != ':')))
#else
            if (filename[0] != '/')
#endif
                font->loadFromFile(getResourcePath() + filename);
            else
                font->loadFromFile(filename);

            // We create the SFML font manually first, as passing the string to the Font constructor would cause
            // an endless recursive call to this function.
            return Font{font};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ObjectConverter deserializeColor(const std::string& value)
        {
            std::string str = toLower(trim(value));
            if (str.empty() || (str == "none"))
                return Color();

            // Check if the color is represented by a string with its name
            const auto it = Color::colorMap.find(toLower(str));
            if (it != Color::colorMap.end())
                return it->second;

            // The color can be represented with a hexadecimal number
            if (str[0] == '#')
            {
                // You can only have hex characters
                for (std::size_t i = 1; i < value.length(); ++i)
                {
                    if (!((value[i] >= '0' && value[i] <= '9') || (value[i] >= 'A' && value[i] <= 'F')  || (value[i] >= 'a' && value[i] <= 'f')))
                        throw Exception{"Failed to deserialize color '" + value + "'. Value started but '#' but contained an invalid character afterwards."};
                }

                // Parse the different types of strings (#123, #1234, #112233 and #11223344)
                if (value.length() == 4)
                {
                    return Color{static_cast<std::uint8_t>(hexToDec(value[1]) * 16 + hexToDec(value[1])),
                                 static_cast<std::uint8_t>(hexToDec(value[2]) * 16 + hexToDec(value[2])),
                                 static_cast<std::uint8_t>(hexToDec(value[3]) * 16 + hexToDec(value[3]))};
                }
                else if (value.length() == 5)
                {
                    return Color{static_cast<std::uint8_t>(hexToDec(value[1]) * 16 + hexToDec(value[1])),
                                 static_cast<std::uint8_t>(hexToDec(value[2]) * 16 + hexToDec(value[2])),
                                 static_cast<std::uint8_t>(hexToDec(value[3]) * 16 + hexToDec(value[3])),
                                 static_cast<std::uint8_t>(hexToDec(value[4]) * 16 + hexToDec(value[4]))};
                }
                else if (value.length() == 7)
                {
                    return Color{static_cast<std::uint8_t>(hexToDec(value[1]) * 16 + hexToDec(value[2])),
                                 static_cast<std::uint8_t>(hexToDec(value[3]) * 16 + hexToDec(value[4])),
                                 static_cast<std::uint8_t>(hexToDec(value[5]) * 16 + hexToDec(value[6]))};
                }
                else if (value.length() == 9)
                {
                    return Color{static_cast<std::uint8_t>(hexToDec(value[1]) * 16 + hexToDec(value[2])),
                                 static_cast<std::uint8_t>(hexToDec(value[3]) * 16 + hexToDec(value[4])),
                                 static_cast<std::uint8_t>(hexToDec(value[5]) * 16 + hexToDec(value[6])),
                                 static_cast<std::uint8_t>(hexToDec(value[7]) * 16 + hexToDec(value[8]))};
                }
                else
                    throw Exception{"Failed to deserialize color '" + value + "'. Value started but '#' but has the wrong length."};
            }

            // The string can optionally start with "rgb" or "rgba", but this is ignored
            if (str.substr(0, 4) == "rgba")
                str.erase(0, 4);
            else if (str.substr(0, 3) == "rgb")
                str.erase(0, 3);

            // Remove the first and last characters when they are brackets
            if ((str[0] == '(') && (str[str.length()-1] == ')'))
                str = str.substr(1, str.length()-2);

            const std::vector<std::string> tokens = Deserializer::split(str, ',');
            if (tokens.size() == 3 || tokens.size() == 4)
            {
                return Color{static_cast<std::uint8_t>(strToInt(tokens[0])),
                             static_cast<std::uint8_t>(strToInt(tokens[1])),
                             static_cast<std::uint8_t>(strToInt(tokens[2])),
                             static_cast<std::uint8_t>((tokens.size() == 4) ? strToInt(tokens[3]) : 255)};
            }

            throw Exception{"Failed to deserialize color '" + value + "'."};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ObjectConverter deserializeString(const std::string& value)
        {
            // Only deserialize the string when it is surrounded with quotes
            if ((value.size() >= 2) && ((value[0] == '"') && (value[value.length()-1] == '"')))
            {
                std::string result = value.substr(1, value.length()-2);

                std::size_t backslashPos = 0;
                while ((backslashPos = result.find('\\', backslashPos)) < result.size()-1)
                {
                    result.erase(backslashPos, 1);

                    if (result[backslashPos] == 'n')
                        result[backslashPos] = '\n';
                    else if (result[backslashPos] == 't')
                        result[backslashPos] = '\t';
                    else if (result[backslashPos] == 'v')
                        result[backslashPos] = '\v';

                    backslashPos++;
                }

                return {sf::String::fromUtf8(result.begin(), result.end())};
            }
            else
                return {sf::String::fromUtf8(value.begin(), value.end())};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ObjectConverter deserializeNumber(const std::string& value)
        {
            return {strToFloat(value)};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ObjectConverter deserializeOutline(const std::string& value)
        {
            std::string str = trim(value);

            if (str.empty())
                throw Exception{"Failed to deserialize outline '" + value + "'. String was empty."};

            // Remove the brackets around the value
            if (((str.front() == '(') && (str.back() == ')')) || ((str.front() == '{') && (str.back() == '}')))
                str = str.substr(1, str.length() - 2);

            if (str.empty())
                return {Outline{0}};

            const std::vector<std::string> tokens = Deserializer::split(str, ',');
            if (tokens.size() == 1)
                return {Outline{tokens[0]}};
            else if (tokens.size() == 2)
                return {Outline{tokens[0], tokens[1]}};
            else if (tokens.size() == 4)
                return {Outline{tokens[0], tokens[1], tokens[2], tokens[3]}};
            else
                throw Exception{"Failed to deserialize outline '" + value + "'. Expected numbers separated with a comma."};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ObjectConverter deserializeTexture(const std::string& value)
        {
            if (value.empty() || (toLower(value) == "none"))
                return Texture{};

            // If there are no quotes then the value just contains a filename
            if (value[0] != '"')
            {
                // Load the texture but insert the resource path into the filename unless the filename is an absolute path
#ifdef SFML_SYSTEM_WINDOWS
                if ((value[0] != '/') && (value[0] != '\\') && ((value.size() <= 1) || (value[1] != ':')))
#else
                if (value[0] != '/')
#endif
                    return Texture{getResourcePath() + value};
                else
                    return Texture{value};
            }

            std::string::const_iterator c = value.begin();
            ++c; // Skip the opening quote

            std::string filename;
            char prev = '\0';

            // Look for the end quote
            bool filenameFound = false;
            while (c != value.end())
            {
                if ((*c != '"') || (prev == '\\'))
                {
                    prev = *c;
                    filename.push_back(*c);
                    ++c;
                }
                else
                {
                    ++c;
                    filenameFound = true;
                    break;
                }
            }

            if (!filenameFound)
                throw Exception{"Failed to deserialize texture '" + value + "'. Failed to find the closing quote of the filename."};

            // There may be optional parameters
            sf::IntRect partRect;
            sf::IntRect middleRect;
            bool smooth = false;

            while (removeWhitespace(value, c))
            {
                std::string word;
                auto openingBracketPos = value.find('(', c - value.begin());
                if (openingBracketPos != std::string::npos)
                    word = value.substr(c - value.begin(), openingBracketPos - (c - value.begin()));
                else
                {
                    if (toLower(trim(value.substr(c - value.begin()))) == "smooth")
                    {
                        smooth = true;
                        break;
                    }
                    else
                        throw Exception{"Failed to deserialize texture '" + value + "'. Invalid text found behind filename."};
                }

                sf::IntRect* rect = nullptr;
                if ((word == "Part") || (word == "part"))
                {
                    rect = &partRect;
                    std::advance(c, 4);
                }
                else if ((word == "Middle") || (word == "middle"))
                {
                    rect = &middleRect;
                    std::advance(c, 6);
                }
                else
                {
                    if (word.empty())
                        throw Exception{"Failed to deserialize texture '" + value + "'. Expected 'Part' or 'Middle' in front of opening bracket."};
                    else
                        throw Exception{"Failed to deserialize texture '" + value + "'. Unexpected word '" + word + "' in front of opening bracket. Expected 'Part' or 'Middle'."};
                }

                auto closeBracketPos = value.find(')', c - value.begin());
                if (closeBracketPos != std::string::npos)
                {
                    if (!readIntRect(value.substr(c - value.begin(), closeBracketPos - (c - value.begin()) + 1), *rect))
                        throw Exception{"Failed to parse " + word + " rectangle while deserializing texture '" + value + "'."};
                }
                else
                    throw Exception{"Failed to deserialize texture '" + value + "'. Failed to find closing bracket for " + word + " rectangle."};

                std::advance(c, closeBracketPos - (c - value.begin()) + 1);
            }

            return Texture{filename, partRect, middleRect, smooth};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ObjectConverter deserializeTextStyle(const std::string& style)
        {
            unsigned int decodedStyle = sf::Text::Regular;
            std::vector<std::string> styles = Deserializer::split(style, '|');
            for (const auto& elem : styles)
            {
                std::string requestedStyle = toLower(elem);
                if (requestedStyle == "bold")
                    decodedStyle |= sf::Text::Bold;
                else if (requestedStyle == "italic")
                    decodedStyle |= sf::Text::Italic;
                else if (requestedStyle == "underlined")
                    decodedStyle |= sf::Text::Underlined;
                else if (requestedStyle == "strikethrough")
                    decodedStyle |= sf::Text::StrikeThrough;
            }

            return TextStyle(decodedStyle);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ObjectConverter deserializeRendererData(const std::string& renderer)
        {
            std::stringstream ss{renderer};
            auto node = DataIO::parse(ss);

            // The root node should contain exactly one child which is the node we need
            if (node->propertyValuePairs.empty() && (node->children.size() == 1))
                node = std::move(node->children[0]);

            auto rendererData = RendererData::create();
            for (const auto& pair : node->propertyValuePairs)
                rendererData->propertyValuePairs[pair.first] = ObjectConverter(pair.second->value); // Did not compile with VS2015 Update 2 when using braces

            for (const auto& child : node->children)
            {
                std::stringstream ss2;
                DataIO::emit(child, ss2);
                rendererData->propertyValuePairs[toLower(child->name)] = {sf::String{"{\n" + ss2.str() + "}"}};
            }

            return rendererData;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<ObjectConverter::Type, Deserializer::DeserializeFunc> Deserializer::m_deserializers =
        {
            {ObjectConverter::Type::Bool, deserializeBool},
            {ObjectConverter::Type::Font, deserializeFont},
            {ObjectConverter::Type::Color, deserializeColor},
            {ObjectConverter::Type::String, deserializeString},
            {ObjectConverter::Type::Number, deserializeNumber},
            {ObjectConverter::Type::Outline, deserializeOutline},
            {ObjectConverter::Type::Texture, deserializeTexture},
            {ObjectConverter::Type::TextStyle, deserializeTextStyle},
            {ObjectConverter::Type::RendererData, deserializeRendererData}
        };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ObjectConverter Deserializer::deserialize(ObjectConverter::Type type, const std::string& serializedString)
    {
        assert(m_deserializers.find(type) != m_deserializers.end());
        return m_deserializers[type](serializedString);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Deserializer::setFunction(ObjectConverter::Type type, const DeserializeFunc& deserializer)
    {
        m_deserializers[type] = deserializer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Deserializer::DeserializeFunc& Deserializer::getFunction(ObjectConverter::Type type)
    {
        return m_deserializers[type];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<std::string> Deserializer::split(const std::string& str, char delim)
    {
        std::vector<std::string> tokens;

        std::size_t start = 0;
        std::size_t end = 0;
        while ((end = str.find(delim, start)) != std::string::npos) {
            tokens.push_back(trim(str.substr(start, end - start)));
            start = end + 1;
        }

        tokens.push_back(trim(str.substr(start)));
        return tokens;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


#include <TGUI/Loading/Theme.hpp>
#include <TGUI/Loading/Serializer.hpp>
#include <TGUI/Global.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace
    {
        struct DefaultTheme : public Theme
        {
            void reset()
            {
                m_renderers = {
                    {"bitmapbutton", RendererData::create({{"borders", Borders{1}},
                                                           {"bordercolor", Color{60, 60, 60}},
                                                           {"bordercolorhover", Color::Black},
                                                           {"bordercolordown", Color::Black},
                                                           {"bordercolorfocused", Color{30, 30, 180}},
                                                           {"bordercolordisabled", Color{125, 125, 125}},
                                                           {"textcolor", Color{60, 60, 60}},
                                                           {"textcolorhover", Color::Black},
                                                           {"textcolordown", Color::Black},
                                                           {"textcolordisabled", Color{125, 125, 125}},
                                                           {"backgroundcolor", Color{245, 245, 245}},
                                                           {"backgroundcolorhover", Color::White},
                                                           {"backgroundcolordown", Color{235, 235, 235}},
                                                           {"backgroundcolordisabled", Color{230, 230, 230}}})},
                    {"button", RendererData::create({{"borders", Borders{1}},
                                                     {"bordercolor", Color{60, 60, 60}},
                                                     {"bordercolorhover", Color::Black},
                                                     {"bordercolordown", Color::Black},
                                                     {"bordercolorfocused", Color{30, 30, 180}},
                                                     {"bordercolordisabled", Color{125, 125, 125}},
                                                     {"textcolor", Color{60, 60, 60}},
                                                     {"textcolorhover", Color::Black},
                                                     {"textcolordown", Color::Black},
                                                     {"textcolordisabled", Color{125, 125, 125}},
                                                     {"backgroundcolor", Color{245, 245, 245}},
                                                     {"backgroundcolorhover", Color::White},
                                                     {"backgroundcolordown", Color{235, 235, 235}},
                                                     {"backgroundcolordisabled", Color{230, 230, 230}}})},
                    {"chatbox", RendererData::create({{"borders", Borders{1}},
                                                      {"padding", Padding{0}},
                                                      {"bordercolor", Color::Black},
                                                      {"backgroundcolor", Color{245, 245, 245}}})},
                    {"checkbox", RendererData::create({{"borders", Borders{1}},
                                                       {"bordercolor", Color{60, 60, 60}},
                                                       {"bordercolorhover", Color::Black},
                                                       {"bordercolorfocused", Color{30, 30, 180}},
                                                       {"bordercolordisabled", Color{125, 125, 125}},
                                                       {"textcolor", Color{60, 60, 60}},
                                                       {"textcolorhover", Color::Black},
                                                       {"textcolordisabled", Color{125, 125, 125}},
                                                       {"backgroundcolor", Color{245, 245, 245}},
                                                       {"backgroundcolorhover", Color::White},
                                                       {"backgroundcolordisabled", Color{230, 230, 230}},
                                                       {"checkcolor", Color{60, 60, 60}},
                                                       {"checkcolorhover", Color::Black},
                                                       {"checkcolordisabled", Color{125, 125, 125}},
                                                       {"textdistanceratio", 0.2f}})},
                    {"childwindow", RendererData::create({{"borders", Borders{1}},
                                                          {"bordercolor", Color::Black},
                                                          {"titlecolor", Color{60, 60, 60}},
                                                          {"titlebarcolor", Color::White},
                                                          {"backgroundcolor", Color{230, 230, 230}},
                                                          {"distancetoside", 3.f},
                                                          {"borderbelowtitlebar", 1.f},
                                                          {"paddingbetweenbuttons", 1.f},
                                                          {"showtextontitlebuttons", true}})},
                    {"childwindowbutton", RendererData::create({{"borders", Borders{1}},
                                                                {"bordercolor", Color{60, 60, 60}},
                                                                {"bordercolorhover", Color::Black},
                                                                {"bordercolordown", Color::Black},
                                                                {"textcolor", Color{60, 60, 60}},
                                                                {"textcolorhover", Color::Black},
                                                                {"textcolordown", Color::Black},
                                                                {"backgroundcolor", Color{245, 245, 245}},
                                                                {"backgroundcolorhover", Color::White},
                                                                {"backgroundcolordown", Color{235, 235, 235}}})},
                    {"combobox", RendererData::create({{"borders", Borders{1}},
                                                       {"padding", Padding{0}},
                                                       {"bordercolor", Color::Black},
                                                       {"textcolor", Color{60, 60, 60}},
                                                       {"backgroundcolor", Color{245, 245, 245}},
                                                       {"arrowcolor", Color{60, 60, 60}},
                                                       {"arrowcolorhover", Color::Black},
                                                       {"arrowbackgroundcolor", Color{245, 245, 245}},
                                                       {"arrowbackgroundcolorhover", Color::White}})},
                    {"editbox", RendererData::create({{"borders", Borders{1}},
                                                      {"padding", Padding{0}},
                                                      {"caretwidth", 1.f},
                                                      {"caretcolor", Color::Black},
                                                      {"bordercolor", Color{60, 60, 60}},
                                                      {"bordercolorhover", Color::Black},
                                                      {"bordercolordisabled", Color{125, 125, 125}},
                                                      {"textcolor", Color{60, 60, 60}},
                                                      {"textcolordisabled", Color{125, 125, 125}},
                                                      {"selectedtextcolor", Color::White},
                                                      {"selectedtextbackgroundcolor", Color{0, 110, 255}},
                                                      {"defaulttextcolor", Color{160, 160, 160}},
                                                      {"backgroundcolor", Color{245, 245, 245}},
                                                      {"backgroundcolorhover", Color::White},
                                                      {"backgroundcolordisabled", Color{230, 230, 230}}})},
                    {"group", RendererData::create()},
                    {"horizontallayout", RendererData::create()},
                    {"horizontalwrap", RendererData::create()},
                    {"knob", RendererData::create({{"borders", Borders{2}},
                                                   {"imagerotation", 0.f},
                                                   {"bordercolor", Color::Black},
                                                   {"thumbcolor", Color::Black},
                                                   {"backgroundcolor", Color::White}})},
                    {"label", RendererData::create({{"borders", Borders{}},
                                                    {"bordercolor", Color{60, 60, 60}},
                                                    {"textcolor", Color{60, 60, 60}},
                                                    {"backgroundcolor", Color::Transparent}})},
                    {"listbox", RendererData::create({{"borders", Borders{1}},
                                                      {"padding", Padding{0}},
                                                      {"bordercolor", Color::Black},
                                                      {"textcolor", Color{60, 60, 60}},
                                                      {"textcolorhover", Color::Black},
                                                      {"selectedtextcolor", Color::White},
                                                      {"backgroundcolor", Color{245, 245, 245}},
                                                      {"backgroundcolorhover", Color::White},
                                                      {"selectedbackgroundcolor", Color{0, 110, 255}},
                                                      {"selectedbackgroundcolorhover", Color{30, 150, 255}}})},
                    {"listview", RendererData::create({{"borders", Borders{1}},
                                                       {"padding", Padding{0}},
                                                       {"bordercolor", Color::Black},
                                                       {"separatorcolor", Color(200, 200, 200)},
                                                       {"headertextcolor", Color::Black},
                                                       {"headerbackgroundcolor", Color(230, 230, 230)},
                                                       {"textcolor", Color{60, 60, 60}},
                                                       {"textcolorhover", Color::Black},
                                                       {"selectedtextcolor", Color::White},
                                                       {"backgroundcolor", Color{245, 245, 245}},
                                                       {"backgroundcolorhover", Color::White},
                                                       {"selectedbackgroundcolor", Color{0, 110, 255}},
                                                       {"selectedbackgroundcolorhover", Color{30, 150, 255}}})},
                    {"menubar", RendererData::create({{"textcolor", Color{60, 60, 60}},
                                                      {"selectedtextcolor", Color::White},
                                                      {"backgroundcolor", Color::White},
                                                      {"selectedbackgroundcolor", Color{0, 110, 255}},
                                                      {"textcolordisabled", Color{125, 125, 125}}})},
                    {"messagebox", RendererData::create({{"borders", Borders{1}},
                                                         {"bordercolor", Color::Black},
                                                         {"titlecolor", Color{60, 60, 60}},
                                                         {"titlebarcolor", Color::White},
                                                         {"backgroundcolor", Color{230, 230, 230}},
                                                         {"distancetoside", 3.f},
                                                         {"paddingbetweenbuttons", 1.f},
                                                         {"textcolor", Color::Black}})},
                    {"panel", RendererData::create({{"bordercolor", Color::Black},
                                                    {"backgroundcolor", Color::White}})},
                    {"picture", RendererData::create()},
                    {"progressbar", RendererData::create({{"borders", Borders{1}},
                                                          {"bordercolor", Color::Black},
                                                          {"textcolor", Color{60, 60, 60}},
                                                          {"textcolorfilled", Color::White},
                                                          {"backgroundcolor", Color{245, 245, 245}},
                                                          {"fillcolor", Color{0, 110, 255}}})},
                    {"radiobutton", RendererData::create({{"borders", Borders{1}},
                                                          {"bordercolor", Color{60, 60, 60}},
                                                          {"bordercolorhover", Color::Black},
                                                          {"bordercolorfocused", Color{30, 30, 180}},
                                                          {"bordercolordisabled", Color{125, 125, 125}},
                                                          {"textcolor", Color{60, 60, 60}},
                                                          {"textcolorhover", Color::Black},
                                                          {"textcolordisabled", Color{125, 125, 125}},
                                                          {"backgroundcolor", Color{245, 245, 245}},
                                                          {"backgroundcolorhover", Color::White},
                                                          {"backgroundcolordisabled", Color{230, 230, 230}},
                                                          {"checkcolor", Color{60, 60, 60}},
                                                          {"checkcolorhover", Color::Black},
                                                          {"checkcolordisabled", Color{125, 125, 125}},
                                                          {"textdistanceratio", 0.2f}})},
                    {"rangeslider", RendererData::create({{"borders", Borders{1}},
                                                          {"bordercolor", Color{60, 60, 60}},
                                                          {"bordercolorhover", Color::Black},
                                                          {"trackcolor", Color{245, 245, 245}},
                                                          {"trackcolorhover", Color{255, 255, 255}},
                                                          {"thumbcolor", Color{245, 245, 245}},
                                                          {"thumbcolorhover", Color{255, 255, 255}}})},
                    {"scrollablepanel", RendererData::create({{"bordercolor", Color::Black},
                                                              {"backgroundcolor", Color::White}})},
                    {"scrollbar", RendererData::create({{"trackcolor", Color{245, 245, 245}},
                                                        {"thumbcolor", Color{220, 220, 220}},
                                                        {"thumbcolorhover", Color{230, 230, 230}},
                                                        {"arrowbackgroundcolor", Color{245, 245, 245}},
                                                        {"arrowbackgroundcolorhover", Color{255, 255, 255}},
                                                        {"arrowcolor", Color{60, 60, 60}},
                                                        {"arrowcolorhover", Color{0, 0, 0}}})},
                    {"slider", RendererData::create({{"borders", Borders{1}},
                                                     {"bordercolor", Color{60, 60, 60}},
                                                     {"bordercolorhover", Color::Black},
                                                     {"trackcolor", Color{245, 245, 245}},
                                                     {"trackcolorhover", Color{255, 255, 255}},
                                                     {"thumbcolor", Color{245, 245, 245}},
                                                     {"thumbcolorhover", Color{255, 255, 255}}})},
                    {"spinbutton", RendererData::create({{"borders", Borders{1}},
                                                         {"bordercolor", Color::Black},
                                                         {"backgroundcolor", Color{245, 245, 245}},
                                                         {"backgroundcolorhover", Color::White},
                                                         {"arrowcolor", Color{60, 60, 60}},
                                                         {"arrowcolorhover", Color::Black},
                                                         {"borderbetweenarrows", 2.f}})},
                    {"tabs", RendererData::create({{"borders", Borders{1}},
                                                   {"bordercolor", Color::Black},
                                                   {"textcolor", Color{60, 60, 60}},
                                                   {"textcolorhover", Color::Black},
                                                   {"textcolordisabled", Color{125, 125, 125}},
                                                   {"selectedtextcolor", Color::White},
                                                   {"backgroundcolor", Color{245, 245, 245}},
                                                   {"backgroundcolorhover", Color::White},
                                                   {"backgroundcolordisabled", Color{230, 230, 230}},
                                                   {"selectedbackgroundcolor", Color{0, 110, 255}},
                                                   {"selectedbackgroundcolorhover", Color{30, 150, 255}}})},
                    {"textbox", RendererData::create({{"borders", Borders{1}},
                                                      {"padding", Padding{0}},
                                                      {"caretwidth", 1.f},
                                                      {"caretcolor", Color::Black},
                                                      {"bordercolor", Color::Black},
                                                      {"textcolor", Color{60, 60, 60}},
                                                      {"defaulttextcolor", Color{160, 160, 160}},
                                                      {"selectedtextcolor", Color::White},
                                                      {"selectedtextbackgroundcolor", Color{0, 110, 255}},
                                                      {"backgroundcolor", Color::White}})},
                    {"treeview", RendererData::create({{"borders", Borders{1}},
                                                       {"padding", Padding{0}},
                                                       {"bordercolor", Color::Black},
                                                       {"textcolor", Color{60, 60, 60}},
                                                       {"textcolorhover", Color::Black},
                                                       {"selectedtextcolor", Color::White},
                                                       {"backgroundcolor", Color{245, 245, 245}},
                                                       {"backgroundcolorhover", Color::White},
                                                       {"selectedbackgroundcolor", Color{0, 110, 255}},
                                                       {"selectedbackgroundcolorhover", Color{30, 150, 255}}})},
                    {"verticallayout", RendererData::create({})}
               };
            }
        };
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Theme* Theme::m_defaultTheme = nullptr;
    std::shared_ptr<BaseThemeLoader> Theme::m_themeLoader = std::make_shared<DefaultThemeLoader>();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Theme::Theme(const std::string& primary) :
        m_primary(primary)
    {
        if (!primary.empty())
            m_themeLoader->preload(primary);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Theme::~Theme()
    {
        if (m_defaultTheme == this)
            setDefault(nullptr);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Theme::load(const std::string& primary)
    {
        m_primary = primary;
        m_themeLoader->preload(primary);

        // Update the existing renderers
        for (auto& pair : m_renderers)
        {
            auto& renderer = pair.second;
            const auto oldData = renderer;

            if (!m_themeLoader->canLoad(m_primary, pair.first))
                continue;

            auto& properties = m_themeLoader->load(m_primary, pair.first);

            // Update the property-value pairs of the renderer
            renderer->propertyValuePairs = std::map<std::string, ObjectConverter>{};
            for (const auto& property : properties)
                renderer->propertyValuePairs[property.first] = ObjectConverter(property.second);

            // Tell the widgets that were using this renderer about all the updated properties, both new ones and old ones that were now reset to their default value
            auto oldIt = oldData->propertyValuePairs.begin();
            auto newIt = renderer->propertyValuePairs.begin();
            while (oldIt != oldData->propertyValuePairs.end() && newIt != renderer->propertyValuePairs.end())
            {
                if (oldIt->first < newIt->first)
                {
                    // Update values that no longer exist in the new renderer and are now reset to the default value
                    for (const auto& observer : renderer->observers)
                        observer.second(oldIt->first);

                    ++oldIt;
                }
                else
                {
                    // Update changed and new properties
                    for (const auto& observer : renderer->observers)
                        observer.second(newIt->first);

                    if (newIt->first < oldIt->first)
                        ++newIt;
                    else
                    {
                        ++oldIt;
                        ++newIt;
                    }
                }
            }
            while (oldIt != oldData->propertyValuePairs.end())
            {
                for (const auto& observer : renderer->observers)
                    observer.second(oldIt->first);

                ++oldIt;
            }
            while (newIt != renderer->propertyValuePairs.end())
            {
                for (const auto& observer : renderer->observers)
                    observer.second(newIt->first);

                ++newIt;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<RendererData> Theme::getRenderer(const std::string& id)
    {
        std::string lowercaseSecondary = toLower(id);

        // If we already have this renderer in cache then just return it
        auto it = m_renderers.find(lowercaseSecondary);
        if (it != m_renderers.end())
            return it->second;

        m_renderers[lowercaseSecondary] = RendererData::create();
        auto& properties = m_themeLoader->load(m_primary, lowercaseSecondary);
        for (const auto& property : properties)
            m_renderers[lowercaseSecondary]->propertyValuePairs[property.first] = ObjectConverter(property.second);

        return m_renderers[lowercaseSecondary];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<RendererData> Theme::getRendererNoThrow(const std::string& id)
    {
        std::string lowercaseSecondary = toLower(id);

        // If we already have this renderer in cache then just return it
        auto it = m_renderers.find(lowercaseSecondary);
        if (it != m_renderers.end())
            return it->second;

        m_renderers[lowercaseSecondary] = RendererData::create();

        if (!m_themeLoader->canLoad(m_primary, lowercaseSecondary))
            return nullptr;

        auto& properties = m_themeLoader->load(m_primary, lowercaseSecondary);
        for (const auto& property : properties)
            m_renderers[lowercaseSecondary]->propertyValuePairs[property.first] = ObjectConverter(property.second);

        return m_renderers[lowercaseSecondary];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Theme::addRenderer(const std::string& id, std::shared_ptr<RendererData> renderer)
    {
        m_renderers[toLower(id)] = renderer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Theme::removeRenderer(const std::string& id)
    {
        auto it = m_renderers.find(toLower(id));
        if (it != m_renderers.end())
        {
            m_renderers.erase(it);
            return true;
        }
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& Theme::getPrimary() const
    {
        return m_primary;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Theme::setThemeLoader(std::shared_ptr<BaseThemeLoader> themeLoader)
    {
        m_themeLoader = themeLoader;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<BaseThemeLoader> Theme::getThemeLoader()
    {
        return m_themeLoader;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Theme::setDefault(Theme* theme)
    {
        m_defaultTheme = theme;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Theme* Theme::getDefault()
    {
        if (!m_defaultTheme)
        {
            static DefaultTheme defaultTheme;
            defaultTheme.reset();
            m_defaultTheme = &defaultTheme;
        }

        return m_defaultTheme;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


#include <TGUI/Loading/WidgetFactory.hpp>
#include <TGUI/Widgets/BitmapButton.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/Canvas.hpp>
#include <TGUI/Widgets/ChatBox.hpp>
#include <TGUI/Widgets/CheckBox.hpp>
#include <TGUI/Widgets/ChildWindow.hpp>
#include <TGUI/Widgets/ComboBox.hpp>
#include <TGUI/Widgets/EditBox.hpp>
#include <TGUI/Widgets/Grid.hpp>
#include <TGUI/Widgets/Group.hpp>
#include <TGUI/Widgets/HorizontalLayout.hpp>
#include <TGUI/Widgets/HorizontalWrap.hpp>
#include <TGUI/Widgets/Knob.hpp>
#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Widgets/ListBox.hpp>
#include <TGUI/Widgets/ListView.hpp>
#include <TGUI/Widgets/MenuBar.hpp>
#include <TGUI/Widgets/MessageBox.hpp>
#include <TGUI/Widgets/Panel.hpp>
#include <TGUI/Widgets/Picture.hpp>
#include <TGUI/Widgets/ProgressBar.hpp>
#include <TGUI/Widgets/RadioButton.hpp>
#include <TGUI/Widgets/RadioButtonGroup.hpp>
#include <TGUI/Widgets/RangeSlider.hpp>
#include <TGUI/Widgets/ScrollablePanel.hpp>
#include <TGUI/Widgets/Scrollbar.hpp>
#include <TGUI/Widgets/Slider.hpp>
#include <TGUI/Widgets/SpinButton.hpp>
#include <TGUI/Widgets/Tabs.hpp>
#include <TGUI/Widgets/TextBox.hpp>
#include <TGUI/Widgets/TreeView.hpp>
#include <TGUI/Widgets/VerticalLayout.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    std::map<std::string, std::function<Widget::Ptr()>> WidgetFactory::m_constructFunctions =
    {
        {"bitmapbutton", std::make_shared<BitmapButton>},
        {"button", std::make_shared<Button>},
        {"canvas", std::make_shared<Canvas>},
        {"chatbox", std::make_shared<ChatBox>},
        {"checkbox", std::make_shared<CheckBox>},
        {"childwindow", std::make_shared<ChildWindow>},
        {"clickablewidget", std::make_shared<ClickableWidget>},
        {"combobox", std::make_shared<ComboBox>},
        {"editbox", std::make_shared<EditBox>},
        {"grid", std::make_shared<Grid>},
        {"group", std::make_shared<Group>},
        {"horizontallayout", std::make_shared<HorizontalLayout>},
        {"horizontalwrap", std::make_shared<HorizontalWrap>},
        {"knob", std::make_shared<Knob>},
        {"label", std::make_shared<Label>},
        {"listbox", std::make_shared<ListBox>},
        {"listview", std::make_shared<ListView>},
        {"menubar", std::make_shared<MenuBar>},
        {"messagebox", std::make_shared<MessageBox>},
        {"panel", std::make_shared<Panel>},
        {"picture", std::make_shared<Picture>},
        {"progressbar", std::make_shared<ProgressBar>},
        {"radiobutton", std::make_shared<RadioButton>},
        {"radiobuttongroup", std::make_shared<RadioButtonGroup>},
        {"rangeslider", std::make_shared<RangeSlider>},
        {"scrollablepanel", std::make_shared<ScrollablePanel>},
        {"scrollbar", std::make_shared<Scrollbar>},
        {"slider", std::make_shared<Slider>},
        {"spinbutton", std::make_shared<SpinButton>},
        {"tabs", std::make_shared<Tabs>},
        {"textbox", std::make_shared<TextBox>},
        {"treeview", std::make_shared<TreeView>},
        {"verticallayout", std::make_shared<VerticalLayout>}
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void WidgetFactory::setConstructFunction(const std::string& type, const std::function<Widget::Ptr()>& constructor)
    {
        m_constructFunctions[toLower(type)] = constructor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::function<Widget::Ptr()>& WidgetFactory::getConstructFunction(const std::string& type)
    {
        return m_constructFunctions[toLower(type)];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


#include <TGUI/Renderers/ButtonRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_OUTLINE(ButtonRenderer, Borders)

    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, TextColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, TextColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, TextColorDown, {})
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, TextColorDisabled, {})
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, TextColorFocused, {})
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, BackgroundColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, BackgroundColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, BackgroundColorDown, {})
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, BackgroundColorDisabled, {})
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, BackgroundColorFocused, {})
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, BorderColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, BorderColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, BorderColorDown, {})
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, BorderColorDisabled, {})
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, BorderColorFocused, {})

    TGUI_RENDERER_PROPERTY_TEXTURE(ButtonRenderer, Texture)
    TGUI_RENDERER_PROPERTY_TEXTURE(ButtonRenderer, TextureHover)
    TGUI_RENDERER_PROPERTY_TEXTURE(ButtonRenderer, TextureDown)
    TGUI_RENDERER_PROPERTY_TEXTURE(ButtonRenderer, TextureDisabled)
    TGUI_RENDERER_PROPERTY_TEXTURE(ButtonRenderer, TextureFocused)

    TGUI_RENDERER_PROPERTY_TEXT_STYLE(ButtonRenderer, TextStyle, sf::Text::Regular)
    TGUI_RENDERER_PROPERTY_TEXT_STYLE(ButtonRenderer, TextStyleHover, {})
    TGUI_RENDERER_PROPERTY_TEXT_STYLE(ButtonRenderer, TextStyleDown, {})
    TGUI_RENDERER_PROPERTY_TEXT_STYLE(ButtonRenderer, TextStyleDisabled, {})
    TGUI_RENDERER_PROPERTY_TEXT_STYLE(ButtonRenderer, TextStyleFocused, {})

    TGUI_RENDERER_PROPERTY_NUMBER(ButtonRenderer, TextOutlineThickness, 0)
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, TextOutlineColor, {})
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


#include <TGUI/Renderers/ChildWindowRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_OUTLINE(ChildWindowRenderer, Borders)

    TGUI_RENDERER_PROPERTY_COLOR(ChildWindowRenderer, TitleBarColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(ChildWindowRenderer, TitleColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(ChildWindowRenderer, BackgroundColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(ChildWindowRenderer, BorderColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(ChildWindowRenderer, BorderColorFocused, {})

    TGUI_RENDERER_PROPERTY_TEXTURE(ChildWindowRenderer, TextureTitleBar)
    TGUI_RENDERER_PROPERTY_TEXTURE(ChildWindowRenderer, TextureBackground)

    TGUI_RENDERER_PROPERTY_RENDERER(ChildWindowRenderer, CloseButton, "childwindowbutton")
    TGUI_RENDERER_PROPERTY_RENDERER(ChildWindowRenderer, MaximizeButton, "childwindowbutton")
    TGUI_RENDERER_PROPERTY_RENDERER(ChildWindowRenderer, MinimizeButton, "childwindowbutton")

    TGUI_RENDERER_PROPERTY_NUMBER(ChildWindowRenderer, BorderBelowTitleBar, 0)
    TGUI_RENDERER_PROPERTY_NUMBER(ChildWindowRenderer, DistanceToSide, 3)
    TGUI_RENDERER_PROPERTY_NUMBER(ChildWindowRenderer, PaddingBetweenButtons, 1)
    TGUI_RENDERER_PROPERTY_NUMBER(ChildWindowRenderer, MinimumResizableBorderWidth, 5)

    TGUI_RENDERER_PROPERTY_BOOL(ChildWindowRenderer, ShowTextOnTitleButtons, false)

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float ChildWindowRenderer::getTitleBarHeight() const
    {
        auto it = m_data->propertyValuePairs.find("titlebarheight");
        if (it != m_data->propertyValuePairs.end())
            return it->second.getNumber();
        else
        {
            it = m_data->propertyValuePairs.find("texturetitlebar");
            if (it != m_data->propertyValuePairs.end() && it->second.getTexture().getData())
                return it->second.getTexture().getImageSize().y;
            else
                return 20;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindowRenderer::setTitleBarHeight(float number)
    {
        setProperty("titlebarheight", ObjectConverter{number});
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


#include <TGUI/Renderers/EditBoxRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_OUTLINE(EditBoxRenderer, Borders)
    TGUI_RENDERER_PROPERTY_OUTLINE(EditBoxRenderer, Padding)

    TGUI_RENDERER_PROPERTY_NUMBER(EditBoxRenderer, CaretWidth, 1)

    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, TextColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, TextColorDisabled, {})
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, TextColorFocused, {})
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, SelectedTextColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, SelectedTextBackgroundColor, Color(0, 110, 255))
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, DefaultTextColor, Color(160, 160, 160))
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, BackgroundColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, BackgroundColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, BackgroundColorDisabled, {})
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, BackgroundColorFocused, {})
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, CaretColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, CaretColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, CaretColorFocused, {})
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, BorderColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, BorderColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, BorderColorDisabled, {})
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, BorderColorFocused, {})

    TGUI_RENDERER_PROPERTY_TEXTURE(EditBoxRenderer, Texture)
    TGUI_RENDERER_PROPERTY_TEXTURE(EditBoxRenderer, TextureHover)
    TGUI_RENDERER_PROPERTY_TEXTURE(EditBoxRenderer, TextureDisabled)
    TGUI_RENDERER_PROPERTY_TEXTURE(EditBoxRenderer, TextureFocused)

    TGUI_RENDERER_PROPERTY_TEXT_STYLE(EditBoxRenderer, TextStyle, sf::Text::Regular)
    TGUI_RENDERER_PROPERTY_TEXT_STYLE(EditBoxRenderer, DefaultTextStyle, sf::Text::Italic)
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


#include <TGUI/Renderers/KnobRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_OUTLINE(KnobRenderer, Borders)

    TGUI_RENDERER_PROPERTY_COLOR(KnobRenderer, BackgroundColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(KnobRenderer, ThumbColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(KnobRenderer, BorderColor, Color::Black)

    TGUI_RENDERER_PROPERTY_TEXTURE(KnobRenderer, TextureBackground)
    TGUI_RENDERER_PROPERTY_TEXTURE(KnobRenderer, TextureForeground)

    TGUI_RENDERER_PROPERTY_NUMBER(KnobRenderer, ImageRotation, 0)
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


#include <TGUI/Renderers/ListBoxRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_OUTLINE(ListBoxRenderer, Borders)
    TGUI_RENDERER_PROPERTY_OUTLINE(ListBoxRenderer, Padding)

    TGUI_RENDERER_PROPERTY_COLOR(ListBoxRenderer, BackgroundColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(ListBoxRenderer, BackgroundColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(ListBoxRenderer, SelectedBackgroundColor, Color(0, 110, 255))
    TGUI_RENDERER_PROPERTY_COLOR(ListBoxRenderer, SelectedBackgroundColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(ListBoxRenderer, TextColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(ListBoxRenderer, TextColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(ListBoxRenderer, SelectedTextColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(ListBoxRenderer, SelectedTextColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(ListBoxRenderer, BorderColor, Color::Black)

    TGUI_RENDERER_PROPERTY_TEXTURE(ListBoxRenderer, TextureBackground)

    TGUI_RENDERER_PROPERTY_TEXT_STYLE(ListBoxRenderer, TextStyle, sf::Text::Regular)
    TGUI_RENDERER_PROPERTY_TEXT_STYLE(ListBoxRenderer, SelectedTextStyle, {})

    TGUI_RENDERER_PROPERTY_RENDERER(ListBoxRenderer, Scrollbar, "scrollbar")
    TGUI_RENDERER_PROPERTY_NUMBER(ListBoxRenderer, ScrollbarWidth, 0)
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


#include <TGUI/Renderers/MenuBarRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_COLOR(MenuBarRenderer, BackgroundColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(MenuBarRenderer, SelectedBackgroundColor, Color(0, 110, 255))
    TGUI_RENDERER_PROPERTY_COLOR(MenuBarRenderer, TextColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(MenuBarRenderer, SelectedTextColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(MenuBarRenderer, TextColorDisabled, {})

    TGUI_RENDERER_PROPERTY_TEXTURE(MenuBarRenderer, TextureBackground)
    TGUI_RENDERER_PROPERTY_TEXTURE(MenuBarRenderer, TextureItemBackground)
    TGUI_RENDERER_PROPERTY_TEXTURE(MenuBarRenderer, TextureSelectedItemBackground)

    TGUI_RENDERER_PROPERTY_NUMBER(MenuBarRenderer, DistanceToSide, 0)
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


#include <TGUI/Renderers/ProgressBarRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_OUTLINE(ProgressBarRenderer, Borders)

    TGUI_RENDERER_PROPERTY_COLOR(ProgressBarRenderer, TextColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(ProgressBarRenderer, TextColorFilled, {})
    TGUI_RENDERER_PROPERTY_COLOR(ProgressBarRenderer, BackgroundColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(ProgressBarRenderer, FillColor, Color(0, 110, 255))
    TGUI_RENDERER_PROPERTY_COLOR(ProgressBarRenderer, BorderColor, Color::Black)

    TGUI_RENDERER_PROPERTY_TEXTURE(ProgressBarRenderer, TextureBackground)
    TGUI_RENDERER_PROPERTY_TEXTURE(ProgressBarRenderer, TextureFill)

    TGUI_RENDERER_PROPERTY_TEXT_STYLE(ProgressBarRenderer, TextStyle, sf::Text::Regular)
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


#include <TGUI/Renderers/RangeSliderRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_COLOR(RangeSliderRenderer, SelectedTrackColor, {})
    TGUI_RENDERER_PROPERTY_COLOR(RangeSliderRenderer, SelectedTrackColorHover, {})
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


#include <TGUI/Renderers/ScrollbarRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_COLOR(ScrollbarRenderer, TrackColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(ScrollbarRenderer, TrackColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(ScrollbarRenderer, ThumbColor, Color(128, 128, 128))
    TGUI_RENDERER_PROPERTY_COLOR(ScrollbarRenderer, ThumbColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(ScrollbarRenderer, ArrowBackgroundColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(ScrollbarRenderer, ArrowBackgroundColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(ScrollbarRenderer, ArrowColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(ScrollbarRenderer, ArrowColorHover, {})

    TGUI_RENDERER_PROPERTY_TEXTURE(ScrollbarRenderer, TextureTrack)
    TGUI_RENDERER_PROPERTY_TEXTURE(ScrollbarRenderer, TextureTrackHover)
    TGUI_RENDERER_PROPERTY_TEXTURE(ScrollbarRenderer, TextureThumb)
    TGUI_RENDERER_PROPERTY_TEXTURE(ScrollbarRenderer, TextureThumbHover)
    TGUI_RENDERER_PROPERTY_TEXTURE(ScrollbarRenderer, TextureArrowUp)
    TGUI_RENDERER_PROPERTY_TEXTURE(ScrollbarRenderer, TextureArrowUpHover)
    TGUI_RENDERER_PROPERTY_TEXTURE(ScrollbarRenderer, TextureArrowDown)
    TGUI_RENDERER_PROPERTY_TEXTURE(ScrollbarRenderer, TextureArrowDownHover)
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


#include <TGUI/Renderers/SpinButtonRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_OUTLINE(SpinButtonRenderer, Borders)

    TGUI_RENDERER_PROPERTY_NUMBER(SpinButtonRenderer, BorderBetweenArrows, 0)

    TGUI_RENDERER_PROPERTY_COLOR(SpinButtonRenderer, BackgroundColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(SpinButtonRenderer, BackgroundColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(SpinButtonRenderer, ArrowColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(SpinButtonRenderer, ArrowColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(SpinButtonRenderer, BorderColor, Color::Black)

    TGUI_RENDERER_PROPERTY_TEXTURE(SpinButtonRenderer, TextureArrowUp)
    TGUI_RENDERER_PROPERTY_TEXTURE(SpinButtonRenderer, TextureArrowUpHover)
    TGUI_RENDERER_PROPERTY_TEXTURE(SpinButtonRenderer, TextureArrowDown)
    TGUI_RENDERER_PROPERTY_TEXTURE(SpinButtonRenderer, TextureArrowDownHover)
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


#include <TGUI/Renderers/WidgetRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_BOOL(WidgetRenderer, TransparentTexture, false)

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_RENDERER_PROPERTY_GET_NUMBER(WidgetRenderer, Opacity, 1)

    void WidgetRenderer::setOpacity(float opacity)
    {
        setProperty("opacity", ObjectConverter{std::max(0.f, std::min(1.f, opacity))});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_RENDERER_PROPERTY_GET_NUMBER(WidgetRenderer, OpacityDisabled, -1)

    void WidgetRenderer::setOpacityDisabled(float opacity)
    {
        if (opacity != -1.f)
            setProperty("opacitydisabled", ObjectConverter{std::max(0.f, std::min(1.f, opacity))});
        else
            setProperty("opacitydisabled", ObjectConverter{-1.f});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void WidgetRenderer::setFont(Font font)
    {
        setProperty("font", font);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Font WidgetRenderer::getFont() const
    {
        auto it = m_data->propertyValuePairs.find("font");
        if (it != m_data->propertyValuePairs.end())
            return it->second.getFont();
        else
            return {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void WidgetRenderer::setProperty(const std::string& property, ObjectConverter&& value)
    {
        std::string lowercaseProperty = toLower(property);

        if (m_data->propertyValuePairs[lowercaseProperty] != value)
        {
            m_data->propertyValuePairs[lowercaseProperty] = value;

            for (const auto& observer : m_data->observers)
                observer.second(lowercaseProperty);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ObjectConverter WidgetRenderer::getProperty(const std::string& property) const
    {
        auto it = m_data->propertyValuePairs.find(toLower(property));
        if (it != m_data->propertyValuePairs.end())
            return it->second;
        else
            return {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::map<std::string, ObjectConverter>& WidgetRenderer::getPropertyValuePairs() const
    {
        return m_data->propertyValuePairs;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void WidgetRenderer::subscribe(const void* id, const std::function<void(const std::string& property)>& function)
    {
        m_data->observers[id] = function;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void WidgetRenderer::unsubscribe(const void* id)
    {
        m_data->observers.erase(id);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void WidgetRenderer::setData(const std::shared_ptr<RendererData>& data)
    {
        m_data = data;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<RendererData> WidgetRenderer::getData() const
    {
        return m_data;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<RendererData> WidgetRenderer::clone() const
    {
        auto data = std::make_shared<RendererData>(*m_data);
        data->observers = {};
        return data;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


#include <TGUI/Widgets/BoxLayout.hpp>
#include <TGUI/Widgets/ClickableWidget.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BoxLayout::BoxLayout(const Layout2d& size) :
        Group{size}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BoxLayoutRenderer* BoxLayout::getSharedRenderer()
    {
        return aurora::downcast<BoxLayoutRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const BoxLayoutRenderer* BoxLayout::getSharedRenderer() const
    {
        return aurora::downcast<const BoxLayoutRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BoxLayoutRenderer* BoxLayout::getRenderer()
    {
        return aurora::downcast<BoxLayoutRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const BoxLayoutRenderer* BoxLayout::getRenderer() const
    {
        return aurora::downcast<const BoxLayoutRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BoxLayout::setSize(const Layout2d& size)
    {
        Container::setSize(size);

        updateWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BoxLayout::add(const Widget::Ptr& widget, const sf::String& widgetName)
    {
        insert(m_widgets.size(), widget, widgetName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BoxLayout::insert(std::size_t index, const Widget::Ptr& widget, const sf::String& widgetName)
    {
        // Move the widget to the right position
        if (index < m_widgets.size())
        {
            Group::add(widget, widgetName);

            m_widgets.pop_back();

            m_widgets.insert(m_widgets.begin() + index, widget);
        }
        else // Just add the widget to the back
            Group::add(widget, widgetName);

        updateWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BoxLayout::remove(const Widget::Ptr& widget)
    {
        for (std::size_t i = 0; i < m_widgets.size(); ++i)
        {
            if (m_widgets[i] == widget)
                return remove(i);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BoxLayout::remove(std::size_t index)
    {
        if (index >= m_widgets.size())
            return false;

        Group::remove(m_widgets[index]);

        updateWidgets();
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr BoxLayout::get(std::size_t index) const
    {
        if (index < m_widgets.size())
            return m_widgets[index];
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BoxLayout::rendererChanged(const std::string& property)
    {
        if (property == "spacebetweenwidgets")
        {
            m_spaceBetweenWidgetsCached = getSharedRenderer()->getSpaceBetweenWidgets();
            updateWidgets();
        }
        else if (property == "padding")
        {
            Group::rendererChanged(property);

            // Update the space between widgets as the padding is used when no space was explicitly set
            m_spaceBetweenWidgetsCached = getSharedRenderer()->getSpaceBetweenWidgets();
            updateWidgets();
        }
        else
            Group::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


#include <TGUI/Widgets/Button.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Button::Button()
    {
        m_type = "Button";
        m_text.setFont(m_fontCached);

        m_renderer = aurora::makeCopied<ButtonRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setTextSize(getGlobalTextSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Button::Ptr Button::create(const sf::String& text)
    {
        auto button = std::make_shared<Button>();

        if (!text.isEmpty())
            button->setText(text);

        return button;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Button::Ptr Button::copy(Button::ConstPtr button)
    {
        if (button)
            return std::static_pointer_cast<Button>(button->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ButtonRenderer* Button::getSharedRenderer()
    {
        return aurora::downcast<ButtonRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ButtonRenderer* Button::getSharedRenderer() const
    {
        return aurora::downcast<const ButtonRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ButtonRenderer* Button::getRenderer()
    {
        return aurora::downcast<ButtonRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ButtonRenderer* Button::getRenderer() const
    {
        return aurora::downcast<const ButtonRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_autoSize = false;
        updateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setEnabled(bool enabled)
    {
        Widget::setEnabled(enabled);
        updateTextColorAndStyle();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setText(const sf::String& text)
    {
        m_string = text;
        m_text.setString(text);

        // Set the text size when the text has a fixed size
        if (m_textSize != 0)
        {
            m_text.setCharacterSize(m_textSize);
            updateSize();
        }

        // Draw the text normally unless the height is more than double of the width
        if (getInnerSize().y <= getInnerSize().x * 2)
        {
            // Auto size the text when necessary
            if (m_textSize == 0)
            {
                const unsigned int textSize = Text::findBestTextSize(m_fontCached, getInnerSize().y * 0.8f);
                m_text.setCharacterSize(textSize);

                // Make the text smaller when it's too width
                if (m_text.getSize().x > (getInnerSize().x * 0.85f))
                    m_text.setCharacterSize(static_cast<unsigned int>(textSize * ((getInnerSize().x * 0.85f) / m_text.getSize().x)));
            }
        }
        else // Place the text vertically
        {
            // The text is vertical
            if (!m_string.isEmpty())
            {
                m_text.setString(m_string[0]);

                for (unsigned int i = 1; i < m_string.getSize(); ++i)
                    m_text.setString(m_text.getString() + "\n" + m_string[i]);
            }

            // Auto size the text when necessary
            if (m_textSize == 0)
            {
                unsigned int textSize = Text::findBestTextSize(m_fontCached, getInnerSize().x * 0.8f);
                m_text.setCharacterSize(textSize);

                // Make the text smaller when it's too high
                if (m_text.getSize().y > (getInnerSize().y * 0.85f))
                    m_text.setCharacterSize(static_cast<unsigned int>(textSize * getInnerSize().y * 0.85f / m_text.getSize().y));
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::String& Button::getText() const
    {
        return m_string;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setTextSize(unsigned int size)
    {
        if (size != m_textSize)
        {
            m_textSize = size;

            // Call setText to reposition the text
            setText(getText());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Button::getTextSize() const
    {
        return m_text.getCharacterSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Button::mouseOnWidget(Vector2f pos) const
    {
        if (ClickableWidget::mouseOnWidget(pos))
        {
            if (!m_transparentTextureCached || !m_sprite.isTransparentPixel(pos - getPosition() - m_bordersCached.getOffset()))
                return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::leftMousePressed(Vector2f pos)
    {
        ClickableWidget::leftMousePressed(pos);

        updateTextColorAndStyle();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::leftMouseReleased(Vector2f pos)
    {
        if (m_mouseDown)
            onPress.emit(this, m_text.getString());

        ClickableWidget::leftMouseReleased(pos);

        updateTextColorAndStyle();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::keyPressed(const sf::Event::KeyEvent& event)
    {
        if ((event.code == sf::Keyboard::Space) || (event.code == sf::Keyboard::Return))
            onPress.emit(this, m_text.getString());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::mouseEnteredWidget()
    {
        Widget::mouseEnteredWidget();
        updateTextColorAndStyle();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::mouseLeftWidget()
    {
        Widget::mouseLeftWidget();
        updateTextColorAndStyle();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& Button::getSignal(std::string signalName)
    {
        if (signalName == toLower(onPress.getName()))
            return onPress;
        else
            return ClickableWidget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            updateSize();
        }
        else if ((property == "textcolor") || (property == "textcolorhover") || (property == "textcolordown") || (property == "textcolordisabled") || (property == "textcolorfocused")
              || (property == "textstyle") || (property == "textstylehover") || (property == "textstyledown") || (property == "textstyledisabled") || (property == "textstylefocused"))
        {
            updateTextColorAndStyle();
        }
        else if (property == "texture")
        {
            m_sprite.setTexture(getSharedRenderer()->getTexture());
        }
        else if (property == "texturehover")
        {
            m_spriteHover.setTexture(getSharedRenderer()->getTextureHover());
        }
        else if (property == "texturedown")
        {
            m_spriteDown.setTexture(getSharedRenderer()->getTextureDown());
        }
        else if (property == "texturedisabled")
        {
            m_spriteDisabled.setTexture(getSharedRenderer()->getTextureDisabled());
        }
        else if (property == "texturefocused")
        {
            m_spriteFocused.setTexture(getSharedRenderer()->getTextureFocused());
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == "bordercolorhover")
        {
            m_borderColorHoverCached = getSharedRenderer()->getBorderColorHover();
        }
        else if (property == "bordercolordown")
        {
            m_borderColorDownCached = getSharedRenderer()->getBorderColorDown();
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
        else if (property == "backgroundcolordown")
        {
            m_backgroundColorDownCached = getSharedRenderer()->getBackgroundColorDown();
        }
        else if (property == "backgroundcolordisabled")
        {
            m_backgroundColorDisabledCached = getSharedRenderer()->getBackgroundColorDisabled();
        }
        else if (property == "backgroundcolorfocused")
        {
            m_backgroundColorFocusedCached = getSharedRenderer()->getBackgroundColorFocused();
        }
        else if (property == "textoutlinethickness")
        {
            m_text.setOutlineThickness(getSharedRenderer()->getTextOutlineThickness());
        }
        else if (property == "textoutlinecolor")
        {
            m_text.setOutlineColor(getSharedRenderer()->getTextOutlineColor());
        }
        else if ((property == "opacity") || (property == "opacitydisabled"))
        {
            Widget::rendererChanged(property);

            m_sprite.setOpacity(m_opacityCached);
            m_spriteHover.setOpacity(m_opacityCached);
            m_spriteDown.setOpacity(m_opacityCached);
            m_spriteDisabled.setOpacity(m_opacityCached);
            m_spriteFocused.setOpacity(m_opacityCached);

            m_text.setOpacity(m_opacityCached);
        }
        else if (property == "font")
        {
            Widget::rendererChanged(property);

            m_text.setFont(m_fontCached);
            setText(getText());
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> Button::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        if (!m_string.isEmpty())
            node->propertyValuePairs["Text"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_string));
        if (m_textSize > 0)
            node->propertyValuePairs["TextSize"] = std::make_unique<DataIO::ValueNode>(to_string(m_textSize));

        // Don't store size when auto-sizing
        if (m_autoSize)
            node->propertyValuePairs.erase("Size");

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs["text"])
            setText(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["text"]->value).getString());
        if (node->propertyValuePairs["textsize"])
            setTextSize(strToInt(node->propertyValuePairs["textsize"]->value));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Button::getInnerSize() const
    {
        return {std::max(0.f, getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight()),
                std::max(0.f, getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom())};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Color& Button::getCurrentBackgroundColor() const
    {
        if (!m_enabled && m_backgroundColorDisabledCached.isSet())
            return m_backgroundColorDisabledCached;
        else if (m_mouseHover && m_mouseDown && m_backgroundColorDownCached.isSet())
            return m_backgroundColorDownCached;
        else if (m_mouseHover && m_backgroundColorHoverCached.isSet())
            return m_backgroundColorHoverCached;
        else if (m_focused && m_backgroundColorFocusedCached.isSet())
            return m_backgroundColorFocusedCached;
        else
            return m_backgroundColorCached;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Color& Button::getCurrentBorderColor() const
    {
        if (!m_enabled && m_borderColorDisabledCached.isSet())
            return m_borderColorDisabledCached;
        else if (m_mouseHover && m_mouseDown && m_borderColorDownCached.isSet())
            return m_borderColorDownCached;
        else if (m_mouseHover && m_borderColorHoverCached.isSet())
            return m_borderColorHoverCached;
        else if (m_focused && m_borderColorFocusedCached.isSet())
            return m_borderColorFocusedCached;
        else
            return m_borderColorCached;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::updateTextColorAndStyle()
    {
        if (!m_enabled && getSharedRenderer()->getTextStyleDisabled().isSet())
            m_text.setStyle(getSharedRenderer()->getTextStyleDisabled());
        else if (m_mouseHover && m_mouseDown && getSharedRenderer()->getTextStyleDown().isSet())
            m_text.setStyle(getSharedRenderer()->getTextStyleDown());
        else if (m_mouseHover && getSharedRenderer()->getTextStyleHover().isSet())
            m_text.setStyle(getSharedRenderer()->getTextStyleHover());
        else if (m_focused && getSharedRenderer()->getTextStyleFocused().isSet())
            m_text.setStyle(getSharedRenderer()->getTextStyleFocused());
        else
            m_text.setStyle(getSharedRenderer()->getTextStyle());

        if (!m_enabled && getSharedRenderer()->getTextColorDisabled().isSet())
            m_text.setColor(getSharedRenderer()->getTextColorDisabled());
        else if (m_mouseHover && m_mouseDown && getSharedRenderer()->getTextColorDown().isSet())
            m_text.setColor(getSharedRenderer()->getTextColorDown());
        else if (m_mouseHover && getSharedRenderer()->getTextColorHover().isSet())
            m_text.setColor(getSharedRenderer()->getTextColorHover());
        else if (m_focused && getSharedRenderer()->getTextColorFocused().isSet())
            m_text.setColor(getSharedRenderer()->getTextColorFocused());
        else
            m_text.setColor(getSharedRenderer()->getTextColor());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::updateSize()
    {
        if (m_autoSize)
        {
            const float spaceAroundText = m_text.getSize().y;
            Widget::setSize({m_text.getSize().x + spaceAroundText + m_bordersCached.getLeft() + m_bordersCached.getRight(),
                             m_text.getSize().y * 1.25f + m_bordersCached.getTop() + m_bordersCached.getBottom()});
        }

        m_bordersCached.updateParentSize(getSize());

        // Reset the texture sizes
        m_sprite.setSize(getInnerSize());
        m_spriteHover.setSize(getInnerSize());
        m_spriteDown.setSize(getInnerSize());
        m_spriteDisabled.setSize(getInnerSize());
        m_spriteFocused.setSize(getInnerSize());

        // Recalculate the text size when auto sizing
        if (m_textSize == 0)
            setText(getText());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            drawBorders(target, states, m_bordersCached, getSize(), getCurrentBorderColor());
            states.transform.translate(m_bordersCached.getOffset());
        }

        // Check if there is a background texture
        if (m_sprite.isSet())
        {
            if (!m_enabled && m_spriteDisabled.isSet())
                m_spriteDisabled.draw(target, states);
            else if (m_mouseHover && m_mouseDown && m_spriteDown.isSet())
                m_spriteDown.draw(target, states);
            else if (m_mouseHover && m_spriteHover.isSet())
                m_spriteHover.draw(target, states);
            else if (m_focused && m_spriteFocused.isSet())
                m_spriteFocused.draw(target, states);
            else
                m_sprite.draw(target, states);
        }
        else // There is no background texture
        {
            drawRectangleShape(target, states, getInnerSize(), getCurrentBackgroundColor());
        }

        // If the button has a text then also draw the text
        states.transform.translate({(getInnerSize().x - m_text.getSize().x) / 2.f, (getInnerSize().y - m_text.getSize().y) / 2.f});
        m_text.draw(target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


#include <TGUI/Widgets/ChatBox.hpp>
#include <TGUI/Clipping.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBox::ChatBox()
    {
        m_type = "ChatBox";
        m_draggableWidget = true;

        m_renderer = aurora::makeCopied<ChatBoxRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setTextSize(getGlobalTextSize());
        setSize({Text::getLineHeight(m_fontCached, m_textSize) * 18,
                 Text::getLineHeight(m_fontCached, m_textSize) * 8
                 + Text::getExtraVerticalPadding(m_textSize)
                 + m_paddingCached.getTop() + m_paddingCached.getBottom()
                 + m_bordersCached.getTop() + m_bordersCached.getBottom()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBox::Ptr ChatBox::create()
    {
        return std::make_shared<ChatBox>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBox::Ptr ChatBox::copy(ChatBox::ConstPtr chatBox)
    {
        if (chatBox)
            return std::static_pointer_cast<ChatBox>(chatBox->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBoxRenderer* ChatBox::getSharedRenderer()
    {
        return aurora::downcast<ChatBoxRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ChatBoxRenderer* ChatBox::getSharedRenderer() const
    {
        return aurora::downcast<const ChatBoxRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBoxRenderer* ChatBox::getRenderer()
    {
        return aurora::downcast<ChatBoxRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ChatBoxRenderer* ChatBox::getRenderer() const
    {
        return aurora::downcast<const ChatBoxRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_spriteBackground.setSize(getInnerSize());

        m_bordersCached.updateParentSize(getSize());
        m_paddingCached.updateParentSize(getSize());

        m_scroll->setPosition(getSize().x - m_bordersCached.getRight() - m_scroll->getSize().x, m_bordersCached.getTop());
        m_scroll->setSize({m_scroll->getSize().x, getInnerSize().y});
        m_scroll->setViewportSize(static_cast<unsigned int>(getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()));

        recalculateAllLines();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::addLine(const sf::String& text)
    {
        addLine(text, m_textColor, m_textStyle);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::addLine(const sf::String& text, Color color)
    {
        addLine(text, color, m_textStyle);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::addLine(const sf::String& text, Color color, TextStyle style)
    {
        // Remove the oldest line if you exceed the maximum
        if ((m_maxLines > 0) && (m_maxLines == m_lines.size()))
        {
            if (m_newLinesBelowOthers)
                removeLine(0);
            else
                removeLine(m_maxLines-1);
        }

        Line line;
        line.string = text;
        line.text.setColor(color);
        line.text.setStyle(style);
        line.text.setOpacity(m_opacityCached);
        line.text.setCharacterSize(m_textSize);
        line.text.setString(text);
        line.text.setFont(m_fontCached);

        recalculateLineText(line);

        if (m_newLinesBelowOthers)
            m_lines.push_back(std::move(line));
        else
            m_lines.push_front(std::move(line));

        recalculateFullTextHeight();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String ChatBox::getLine(std::size_t lineIndex) const
    {
        if (lineIndex < m_lines.size())
        {
            return m_lines[lineIndex].string;
        }
        else // Index too high
            return "";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Color ChatBox::getLineColor(std::size_t lineIndex) const
    {
        if (lineIndex < m_lines.size())
        {
            return m_lines[lineIndex].text.getColor();
        }
        else // Index too high
            return m_textColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextStyle ChatBox::getLineTextStyle(std::size_t lineIndex) const
    {
        if (lineIndex < m_lines.size())
            return m_lines[lineIndex].text.getStyle();
        else // Index too high
            return m_textStyle;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChatBox::removeLine(std::size_t lineIndex)
    {
        if (lineIndex < m_lines.size())
        {
            m_lines.erase(m_lines.begin() + lineIndex);

            recalculateFullTextHeight();
            return true;
        }
        else // Index too high
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::removeAllLines()
    {
        m_lines.clear();

        recalculateFullTextHeight();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t ChatBox::getLineAmount()
    {
        return m_lines.size();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setLineLimit(std::size_t maxLines)
    {
        m_maxLines = maxLines;

        // Remove the oldest lines if there are too many lines
        if ((m_maxLines > 0) && (m_maxLines < m_lines.size()))
        {
            if (m_newLinesBelowOthers)
                m_lines.erase(m_lines.begin(), m_lines.begin() + m_lines.size() - m_maxLines);
            else
                m_lines.erase(m_lines.begin() + m_maxLines, m_lines.end());

            recalculateFullTextHeight();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t ChatBox::getLineLimit()
    {
        return m_maxLines;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setTextSize(unsigned int size)
    {
        m_textSize = size;
        m_scroll->setScrollAmount(size);

        for (auto& line : m_lines)
            line.text.setCharacterSize(size);

        recalculateAllLines();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setTextColor(Color color)
    {
        m_textColor = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Color& ChatBox::getTextColor() const
    {
        return m_textColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setTextStyle(TextStyle style)
    {
        m_textStyle = style;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextStyle ChatBox::getTextStyle() const
    {
        return m_textStyle;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setLinesStartFromTop(bool startFromTop)
    {
        if (m_linesStartFromTop != startFromTop)
            m_linesStartFromTop = startFromTop;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChatBox::getLinesStartFromTop() const
    {
        return m_linesStartFromTop;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setNewLinesBelowOthers(bool newLinesBelowOthers)
    {
        m_newLinesBelowOthers = newLinesBelowOthers;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChatBox::getNewLinesBelowOthers() const
    {
        return m_newLinesBelowOthers;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setScrollbarValue(unsigned int value)
    {
        m_scroll->setValue(value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ChatBox::getScrollbarValue() const
    {
        return m_scroll->getValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChatBox::mouseOnWidget(Vector2f pos) const
    {
        if (FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(pos))
        {
            if (!m_transparentTextureCached || !m_spriteBackground.isTransparentPixel(pos - getPosition() - m_bordersCached.getOffset()))
                return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::leftMousePressed(Vector2f pos)
    {
        // Set the mouse down flag to true
        m_mouseDown = true;

        // Pass the event to the scrollbar
        if (m_scroll->mouseOnWidget(pos - getPosition()))
            m_scroll->leftMousePressed(pos - getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::leftMouseReleased(Vector2f pos)
    {
        if (m_scroll->isMouseDown())
            m_scroll->leftMouseReleased(pos - getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::mouseMoved(Vector2f pos)
    {
        if (!m_mouseHover)
            mouseEnteredWidget();

        // Pass the event to the scrollbar when the mouse is on top of it or when we are dragging its thumb
        if (((m_scroll->isMouseDown()) && (m_scroll->isMouseDownOnThumb())) || m_scroll->mouseOnWidget(pos - getPosition()))
            m_scroll->mouseMoved(pos - getPosition());
        else
            m_scroll->mouseNoLongerOnWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::mouseNoLongerOnWidget()
    {
        Widget::mouseNoLongerOnWidget();
        m_scroll->mouseNoLongerOnWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::leftMouseButtonNoLongerDown()
    {
        Widget::leftMouseButtonNoLongerDown();
        m_scroll->leftMouseButtonNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChatBox::mouseWheelScrolled(float delta, Vector2f pos)
    {
        if (m_scroll->getViewportSize() < m_scroll->getMaximum())
        {
            m_scroll->mouseWheelScrolled(delta, pos - getPosition());
            return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::recalculateLineText(Line& line)
    {
        line.text.setString("");

        // Find the maximum width of one line
        const float maxWidth = getInnerSize().x - m_scroll->getSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight();
        if (maxWidth < 0)
            return;

        line.text.setString(Text::wordWrap(maxWidth, line.string, m_fontCached, line.text.getCharacterSize(), false));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::recalculateAllLines()
    {
        for (auto& line : m_lines)
            recalculateLineText(line);

        recalculateFullTextHeight();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::recalculateFullTextHeight()
    {
        m_fullTextHeight = 0;
        for (const auto& line : m_lines)
            m_fullTextHeight += line.text.getSize().y;

        // Update the maximum of the scrollbar
        const unsigned int oldMaximum = m_scroll->getMaximum();
        m_scroll->setMaximum(static_cast<unsigned int>(m_fullTextHeight + Text::getExtraVerticalPadding(m_textSize)));

        // Scroll down to the last item when there is a scrollbar and it is at the bottom
        if (m_newLinesBelowOthers)
        {
            if (((oldMaximum >= m_scroll->getViewportSize()) && (m_scroll->getValue() == oldMaximum - m_scroll->getViewportSize()))
             || ((oldMaximum <= m_scroll->getViewportSize()) && (m_scroll->getMaximum() > m_scroll->getViewportSize())))
            {
                m_scroll->setValue(m_scroll->getMaximum() - m_scroll->getViewportSize());
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::rendererChanged(const std::string& property)
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
        else if (property == "texturebackground")
        {
            m_spriteBackground.setTexture(getSharedRenderer()->getTextureBackground());
        }
        else if (property == "scrollbar")
        {
            m_scroll->setRenderer(getSharedRenderer()->getScrollbar());

            // If no scrollbar width was set then we may need to use the one from the texture
            if (!getSharedRenderer()->getScrollbarWidth())
            {
                m_scroll->setSize({m_scroll->getDefaultWidth(), m_scroll->getSize().y});
                setSize(m_size);
            }
        }
        else if (property == "scrollbarwidth")
        {
            const float width = getSharedRenderer()->getScrollbarWidth() ? getSharedRenderer()->getScrollbarWidth() : m_scroll->getDefaultWidth();
            m_scroll->setSize({width, m_scroll->getSize().y});
            setSize(m_size);
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if ((property == "opacity") || (property == "opacitydisabled"))
        {
            Widget::rendererChanged(property);

            m_spriteBackground.setOpacity(m_opacityCached);
            m_scroll->setInheritedOpacity(m_opacityCached);

            for (auto& line : m_lines)
                line.text.setOpacity(m_opacityCached);
        }
        else if (property == "font")
        {
            Widget::rendererChanged(property);

            for (auto& line : m_lines)
                line.text.setFont(m_fontCached);

            recalculateAllLines();
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> ChatBox::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        node->propertyValuePairs["TextSize"] = std::make_unique<DataIO::ValueNode>(to_string(m_textSize));
        node->propertyValuePairs["TextColor"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_textColor));

        if (m_textStyle != sf::Text::Style::Regular)
            node->propertyValuePairs["TextStyle"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_textStyle));

        if (m_maxLines > 0)
            node->propertyValuePairs["LineLimit"] = std::make_unique<DataIO::ValueNode>(to_string(m_maxLines));

        if (m_linesStartFromTop)
            node->propertyValuePairs["LinesStartFromTop"] = std::make_unique<DataIO::ValueNode>("true");
        else
            node->propertyValuePairs["LinesStartFromTop"] = std::make_unique<DataIO::ValueNode>("false");

        if (m_newLinesBelowOthers)
            node->propertyValuePairs["NewLinesBelowOthers"] = std::make_unique<DataIO::ValueNode>("true");
        else
            node->propertyValuePairs["NewLinesBelowOthers"] = std::make_unique<DataIO::ValueNode>("false");

        for (std::size_t i = 0; i < m_lines.size(); ++i)
        {
            auto lineNode = std::make_unique<DataIO::Node>();
            lineNode->name = "Line";

            lineNode->propertyValuePairs["Text"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(getLine(i)));

            const Color lineTextColor = getLineColor(i);
            if (lineTextColor != m_textColor)
                lineNode->propertyValuePairs["Color"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(lineTextColor));

            const TextStyle lineTextStyle = getLineTextStyle(i);
            if (lineTextStyle != m_textStyle)
                lineNode->propertyValuePairs["Style"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(lineTextStyle));

            node->children.push_back(std::move(lineNode));
        }

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs["textsize"])
            setTextSize(strToInt(node->propertyValuePairs["textsize"]->value));
        if (node->propertyValuePairs["textcolor"])
            setTextColor(Deserializer::deserialize(ObjectConverter::Type::Color, node->propertyValuePairs["textcolor"]->value).getColor());
        if (node->propertyValuePairs["textstyle"])
            setTextStyle(Deserializer::deserialize(ObjectConverter::Type::TextStyle, node->propertyValuePairs["textstyle"]->value).getTextStyle());
        if (node->propertyValuePairs["linelimit"])
            setLineLimit(strToInt(node->propertyValuePairs["linelimit"]->value));

        for (const auto& childNode : node->children)
        {
            if (toLower(childNode->name) == "line")
            {
                Color lineTextColor = getTextColor();
                if (childNode->propertyValuePairs["color"])
                    lineTextColor = Deserializer::deserialize(ObjectConverter::Type::Color, childNode->propertyValuePairs["color"]->value).getColor();

                TextStyle lineTextStyle = getTextStyle();
                if (childNode->propertyValuePairs["style"])
                    lineTextStyle = Deserializer::deserialize(ObjectConverter::Type::TextStyle, childNode->propertyValuePairs["style"]->value).getTextStyle();

                if (childNode->propertyValuePairs["text"])
                    addLine(Deserializer::deserialize(ObjectConverter::Type::String, childNode->propertyValuePairs["text"]->value).getString(), lineTextColor, lineTextStyle);
            }
        }
        node->children.erase(std::remove_if(node->children.begin(), node->children.end(),
                                        [](const std::unique_ptr<DataIO::Node>& child){ return toLower(child->name) == "line"; }), node->children.end());

        if (node->propertyValuePairs["linesstartfromtop"])
            setLinesStartFromTop(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["linesstartfromtop"]->value).getBool());

        // This has to be parsed after the lines have been added
        if (node->propertyValuePairs["newlinesbelowothers"])
            setNewLinesBelowOthers(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["newlinesbelowothers"]->value).getBool());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ChatBox::getInnerSize() const
    {
        return {std::max(0.f, getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight()),
                std::max(0.f, getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom())};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        const sf::RenderStates scrollbarStates = states;

        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            drawBorders(target, states, m_bordersCached, getSize(), m_borderColorCached);
            states.transform.translate(m_bordersCached.getOffset());
        }

        // Draw the background
        if (m_spriteBackground.isSet())
            m_spriteBackground.draw(target, states);
        else
            drawRectangleShape(target, states, getInnerSize(), m_backgroundColorCached);

        // Draw the scrollbar
        m_scroll->draw(target, scrollbarStates);

        states.transform.translate({m_paddingCached.getLeft(), m_paddingCached.getTop()});

        // Set the clipping for all draw calls that happen until this clipping object goes out of scope
        const Clipping clipping{target, states, {}, {getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight() - m_scroll->getSize().x,
                                                     getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}};

        states.transform.translate({Text::getExtraHorizontalPadding(m_fontCached, m_textSize), -static_cast<float>(m_scroll->getValue())});

        // Put the lines at the bottom of the chat box if needed
        if (!m_linesStartFromTop && (m_fullTextHeight + Text::getExtraVerticalPadding(m_textSize) < getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()))
            states.transform.translate(0, getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom() - m_fullTextHeight - Text::getExtraVerticalPadding(m_textSize));

        for (const auto& line : m_lines)
        {
            line.text.draw(target, states);
            states.transform.translate(0, line.text.getSize().y);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


#include <TGUI/Widgets/ChildWindow.hpp>
#include <TGUI/Vector2f.hpp>
#include <TGUI/Clipping.hpp>
#include <TGUI/SignalImpl.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace
    {
        float clamp(float value, float lower, float upper)
        {
            if (value < lower)
                return lower;

            if (value > upper)
                return upper;

            return value;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindow::ChildWindow(const sf::String& title, unsigned int titleButtons)
    {
        m_type = "ChildWindow";
        m_isolatedFocus = true;
        m_titleText.setFont(m_fontCached);

        setTitleTextSize(getGlobalTextSize());
        m_titleBarHeightCached = m_titleText.getSize().y * 1.25f;

        m_renderer = aurora::makeCopied<ChildWindowRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setTitle(title);
        setTitleButtons(titleButtons);
        setSize({400, 300});

        m_maximizeButton->connect("pressed", [this]{ onMaximize.emit(this); });
        m_minimizeButton->connect("pressed", [this]{ onMinimize.emit(this); });
        m_closeButton->connect("pressed", [this]{ close(); });
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindow::Ptr ChildWindow::create(const sf::String& title, unsigned int titleButtons)
    {
        return std::make_shared<ChildWindow>(title, titleButtons);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindow::Ptr ChildWindow::copy(ChildWindow::ConstPtr childWindow)
    {
        if (childWindow)
            return std::static_pointer_cast<ChildWindow>(childWindow->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindowRenderer* ChildWindow::getSharedRenderer()
    {
        return aurora::downcast<ChildWindowRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ChildWindowRenderer* ChildWindow::getSharedRenderer() const
    {
        return aurora::downcast<const ChildWindowRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindowRenderer* ChildWindow::getRenderer()
    {
        return aurora::downcast<ChildWindowRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ChildWindowRenderer* ChildWindow::getRenderer() const
    {
        return aurora::downcast<const ChildWindowRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setPosition(const Layout2d& position)
    {
        float x = position.getValue().x;
        float y = position.getValue().y;

        if (m_keepInParent && m_parent && (m_parent->getSize().x > 0) && (m_parent->getSize().y > 0)
         && ((y < 0) || (y > m_parent->getSize().y - getFullSize().y) || (x < 0) || (x > m_parent->getSize().x - getFullSize().x)))
        {
            if (y < 0)
                y = 0;
            else if (y > m_parent->getSize().y - getFullSize().y)
                y = std::max(0.f, m_parent->getSize().y - getFullSize().y);

            if (x < 0)
                x = 0;
            else if (x > m_parent->getSize().x - getFullSize().x)
                x = std::max(0.f, m_parent->getSize().x - getFullSize().x);

            Container::setPosition({x, y});
        }
        else
            Container::setPosition(position);

        // Calculate the distance from the right side that the buttons will need
        float buttonOffsetX = 0;
        for (const auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
        {
            if (button->isVisible())
                buttonOffsetX += (buttonOffsetX > 0 ? m_paddingBetweenButtonsCached : 0) + button->getSize().x;
        }

        if (buttonOffsetX > 0)
            buttonOffsetX += m_distanceToSideCached;

        if (m_titleAlignment == TitleAlignment::Left)
        {
            m_titleText.setPosition(m_distanceToSideCached,
                                    (m_titleBarHeightCached - m_titleText.getSize().y) / 2.0f);
        }
        else if (m_titleAlignment == TitleAlignment::Center)
        {
            m_titleText.setPosition(m_distanceToSideCached + ((getSize().x - (2 * m_distanceToSideCached) - buttonOffsetX - m_titleText.getSize().x) / 2.0f),
                                    (m_titleBarHeightCached - m_titleText.getSize().y) / 2.0f);
        }
        else // if (m_titleAlignment == TitleAlignment::Right)
        {
            m_titleText.setPosition(getSize().x - m_distanceToSideCached - buttonOffsetX - m_titleText.getSize().x,
                                    (m_titleBarHeightCached - m_titleText.getSize().y) / 2.0f);
        }

        buttonOffsetX = m_distanceToSideCached;
        for (auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
        {
            if (button->isVisible())
            {
                button->setPosition(m_bordersCached.getLeft() + getSize().x - buttonOffsetX - button->getSize().x,
                                    m_bordersCached.getTop() + (m_titleBarHeightCached - button->getSize().y) / 2.f);

                buttonOffsetX += button->getSize().x + m_paddingBetweenButtonsCached;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setSize(const Layout2d& size)
    {
        Container::setSize(size);

        m_bordersCached.updateParentSize(getSize());

        m_spriteTitleBar.setSize({getSize().x, m_titleBarHeightCached});
        m_spriteBackground.setSize(getSize());

        // Reposition the images and text
        setPosition(m_position);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ChildWindow::getFullSize() const
    {
        return {getSize().x + m_bordersCached.getLeft() + m_bordersCached.getRight(),
                getSize().y + m_bordersCached.getTop() + m_bordersCached.getBottom() + m_titleBarHeightCached + m_borderBelowTitleBarCached};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setMaximumSize(Vector2f size)
    {
        if ((size.x < getSize().x) || (size.y < getSize().y))
        {
            // The window is currently larger than the new maximum size, lets downsize
            setSize(std::min(size.x, getSize().x), std::min(size.y, getSize().y));
        }

        m_maximumSize = size;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ChildWindow::getMaximumSize() const
    {
        return m_maximumSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setMinimumSize(Vector2f size)
    {
        if ((size.x > getSize().x) || (size.y > getSize().y))
        {
            // The window is currently smaller than the new minimum size, lets upsize
            setSize(std::max(size.x, getSize().x), std::max(size.y, getSize().y));
        }

        m_minimumSize = size;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ChildWindow::getMinimumSize() const
    {
        return m_minimumSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setTitle(const sf::String& title)
    {
        m_titleText.setString(title);

        // Reposition the images and text
        setPosition(m_position);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::String& ChildWindow::getTitle() const
    {
        return m_titleText.getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setTitleTextSize(unsigned int size)
    {
        m_titleTextSize = size;

        if (m_titleTextSize)
            m_titleText.setCharacterSize(m_titleTextSize);
        else
            m_titleText.setCharacterSize(Text::findBestTextSize(m_fontCached, m_titleBarHeightCached * 0.8f));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ChildWindow::getTitleTextSize() const
    {
        return m_titleTextSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setTitleAlignment(TitleAlignment alignment)
    {
        m_titleAlignment = alignment;

        // Reposition the images and text
        setPosition(m_position);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindow::TitleAlignment ChildWindow::getTitleAlignment() const
    {
        return m_titleAlignment;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setTitleButtons(unsigned int buttons)
    {
        m_titleButtons = buttons;

        if (m_titleButtons & TitleButton::Close)
        {
            m_closeButton->setVisible(true);
            m_closeButton->setRenderer(getSharedRenderer()->getCloseButton());
            m_closeButton->setInheritedOpacity(m_opacityCached);

            if (m_showTextOnTitleButtonsCached)
                m_closeButton->setText(L"\u2715");
            else
                m_closeButton->setText("");
        }
        else
            m_closeButton->setVisible(false);

        if (m_titleButtons & TitleButton::Maximize)
        {
            m_maximizeButton->setVisible(true);
            m_maximizeButton->setRenderer(getSharedRenderer()->getMaximizeButton());
            m_maximizeButton->setInheritedOpacity(m_opacityCached);

            if (m_showTextOnTitleButtonsCached)
                m_maximizeButton->setText(L"\u2610");
            else
                m_maximizeButton->setText("");
        }
        else
            m_maximizeButton->setVisible(false);

        if (m_titleButtons & TitleButton::Minimize)
        {
            m_minimizeButton->setVisible(true);
            m_minimizeButton->setRenderer(getSharedRenderer()->getMinimizeButton());
            m_minimizeButton->setInheritedOpacity(m_opacityCached);

            if (m_showTextOnTitleButtonsCached)
                m_minimizeButton->setText(L"\u2043");
            else
                m_minimizeButton->setText("");
        }
        else
            m_minimizeButton->setVisible(false);

        updateTitleBarHeight();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ChildWindow::getTitleButtons() const
    {
        return m_titleButtons;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::close()
    {
        if (!onClose.emit(this))
            destroy();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::destroy()
    {
        if (m_parent)
            m_parent->remove(shared_from_this());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setResizable(bool resizable)
    {
        m_resizable = resizable;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChildWindow::isResizable() const
    {
        return m_resizable;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setPositionLocked(bool positionLocked)
    {
        m_positionLocked = positionLocked;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChildWindow::isPositionLocked() const
    {
        return m_positionLocked;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setKeepInParent(bool enabled)
    {
        m_keepInParent = enabled;

        if (enabled)
            setPosition(m_position);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChildWindow::isKeptInParent() const
    {
        return m_keepInParent;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ChildWindow::getChildWidgetsOffset() const
    {
        return {m_bordersCached.getLeft(), m_bordersCached.getTop() + m_titleBarHeightCached + m_borderBelowTitleBarCached};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setParent(Container* parent)
    {
        Container::setParent(parent);
        if (m_keepInParent)
            setPosition(m_position);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChildWindow::mouseOnWidget(Vector2f pos) const
    {
        FloatRect region{getPosition(), getFullSize()};

        // Expand the region if the child window is resizable (to make the borders easier to click on)
        if (m_resizable)
        {
            region.left -= std::max(0.f, m_minimumResizableBorderWidthCached - m_bordersCached.getLeft());
            region.top -= std::max(0.f, m_minimumResizableBorderWidthCached - m_bordersCached.getTop());
            region.width += std::max(0.f, m_minimumResizableBorderWidthCached - m_bordersCached.getLeft())
                            + std::max(0.f, m_minimumResizableBorderWidthCached - m_bordersCached.getRight());
            region.height += std::max(0.f, m_minimumResizableBorderWidthCached - m_bordersCached.getTop())
                             + std::max(0.f, m_minimumResizableBorderWidthCached - m_bordersCached.getBottom());
        }

        if (region.contains(pos))
        {
            // If the mouse enters the border or title bar then then none of the widgets can still be under the mouse
            if (m_widgetBelowMouse && !FloatRect{getPosition() + getChildWidgetsOffset(), getSize()}.contains(pos))
                m_widgetBelowMouse->mouseNoLongerOnWidget();

            return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::leftMousePressed(Vector2f pos)
    {
        pos -= getPosition();

        m_mouseDown = true;

        // Move the child window to the front
        moveToFront();

        onMousePress.emit(this);

        if (FloatRect{getChildWidgetsOffset(), getSize()}.contains(pos))
        {
            // Propagate the event to the child widgets
            Container::leftMousePressed(pos + getPosition());
        }
        else if (!FloatRect{m_bordersCached.getLeft(), m_bordersCached.getTop(), getSize().x, getSize().y + m_titleBarHeightCached + m_borderBelowTitleBarCached}.contains(pos))
        {
            if (!m_focused)
                setFocused(true);

            // Mouse is on top of the borders
            if (m_resizable)
            {
                // Check on which border the mouse is standing
                m_resizeDirection = ResizeNone;
                if (pos.x < m_bordersCached.getLeft())
                    m_resizeDirection |= ResizeLeft;
                if (pos.y < m_bordersCached.getTop())
                    m_resizeDirection |= ResizeTop;
                if (pos.x >= getFullSize().x - m_bordersCached.getRight())
                    m_resizeDirection |= ResizeRight;
                if (pos.y >= getFullSize().y - m_bordersCached.getBottom())
                    m_resizeDirection |= ResizeBottom;
            }

            m_draggingPosition = pos;
        }
        else if (FloatRect{m_bordersCached.getLeft(), m_bordersCached.getTop(), getSize().x, m_titleBarHeightCached}.contains(pos))
        {
            if (!m_focused)
                setFocused(true);

            // Send the mouse press event to the title buttons
            for (auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
            {
                if (button->isVisible() && button->mouseOnWidget(pos))
                {
                    button->leftMousePressed(pos);
                    return;
                }
            }

            // The mouse went down on the title bar
            if (!m_positionLocked)
            {
                m_mouseDownOnTitleBar = true;
                m_draggingPosition = pos;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::leftMouseReleased(Vector2f pos)
    {
        pos -= getPosition();

        m_mouseDownOnTitleBar = false;
        m_resizeDirection = ResizeNone;

        if (FloatRect{getChildWidgetsOffset(), getSize()}.contains(pos))
        {
            // Propagate the event to the child widgets
            Container::leftMouseReleased(pos + getPosition());
        }
        else
        {
            // Tell the widgets that the mouse was released
            for (auto& widget : m_widgets)
                widget->leftMouseButtonNoLongerDown();

            // Check if the mouse is on top of the title bar
            if (FloatRect{m_bordersCached.getLeft(), m_bordersCached.getTop(), getSize().x, m_titleBarHeightCached}.contains(pos))
            {
                // Send the mouse release event to the title buttons
                for (auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
                {
                    if (button->isVisible() && button->mouseOnWidget(pos))
                    {
                        button->leftMouseReleased(pos);
                        break;
                    }
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::rightMousePressed(Vector2f pos)
    {
        if (FloatRect{getChildWidgetsOffset(), getSize()}.contains(pos - getPosition()))
            Container::rightMousePressed(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::rightMouseReleased(Vector2f pos)
    {
        pos -= getPosition();

        if (FloatRect{getChildWidgetsOffset(), getSize()}.contains(pos))
        {
            // Propagate the event to the child widgets
            Container::rightMouseReleased(pos + getPosition());
        }
        else
        {
            // Tell the widgets that the mouse was released
            for (auto& widget : m_widgets)
                widget->rightMouseButtonNoLongerDown();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::mouseMoved(Vector2f pos)
    {
        pos -= getPosition();

        // Check if you are dragging the child window
        if (m_mouseDown && m_mouseDownOnTitleBar)
        {
            // Move the child window, but don't allow the dragging position to leave the screen
            Vector2f newPosition;
            if (getPosition().x + pos.x <= 0)
                newPosition.x = -m_draggingPosition.x + 1;
            else if (m_parent && getPosition().x + pos.x >= m_parent->getSize().x)
                newPosition.x = m_parent->getSize().x - m_draggingPosition.x - 1;
            else
                newPosition.x = getPosition().x + (pos.x - m_draggingPosition.x);

            if (getPosition().y + pos.y <= 0)
                newPosition.y = -m_draggingPosition.y + 1;
            else if (m_parent && getPosition().y + pos.y >= m_parent->getSize().y)
                newPosition.y = m_parent->getSize().y - m_draggingPosition.y - 1;
            else
                newPosition.y = getPosition().y + (pos.y - m_draggingPosition.y);

            setPosition(newPosition);
        }

        // Check if you are resizing the window
        else if (m_mouseDown && m_resizeDirection != ResizeNone)
        {
            float minimumWidth = 0;
            for (const auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
            {
                if (button->isVisible())
                    minimumWidth += (minimumWidth > 0 ? m_paddingBetweenButtonsCached : 0) + button->getSize().x;
            }
            minimumWidth += 2 * m_distanceToSideCached;

            if ((m_resizeDirection & ResizeLeft) != 0)
            {
                const float diff = clamp(m_draggingPosition.x - pos.x, std::max(minimumWidth, m_minimumSize.x) - getSize().x, m_maximumSize.x - getSize().x);
                setPosition(getPosition().x - diff, getPosition().y);
                setSize(getSize().x + diff, getSize().y);
            }
            else if ((m_resizeDirection & ResizeRight) != 0)
            {
                const float diff = clamp(pos.x - m_draggingPosition.x, std::max(minimumWidth, m_minimumSize.x) - getSize().x, m_maximumSize.x - getSize().x);
                setSize(getSize().x + diff, getSize().y);
                m_draggingPosition.x += diff;
            }

            if ((m_resizeDirection & ResizeTop) != 0)
            {
                const float diff = clamp(m_draggingPosition.y - pos.y, m_minimumSize.y - getSize().y, m_maximumSize.y - getSize().y);
                setPosition(getPosition().x, getPosition().y - diff);
                setSize(getSize().x, getSize().y + diff);
            }
            else if ((m_resizeDirection & ResizeBottom) != 0)
            {
                const float diff = clamp(pos.y - m_draggingPosition.y, m_minimumSize.y - getSize().y, m_maximumSize.y - getSize().y);
                setSize(getSize().x, getSize().y + diff);
                m_draggingPosition.y += diff;
            }
        }
        else // Not dragging child window
        {
            if (FloatRect{getChildWidgetsOffset(), getSize()}.contains(pos))
            {
                // Propagate the event to the child widgets
                Container::mouseMoved(pos + getPosition());
            }
            else
            {
                if (!m_mouseHover)
                    mouseEnteredWidget();

                // Check if the mouse is on top of the title bar
                if (FloatRect{m_bordersCached.getLeft(), m_bordersCached.getTop(), getSize().x, m_titleBarHeightCached}.contains(pos))
                {
                    // Send the hover event to the buttons inside the title bar
                    for (auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
                    {
                        if (button->isVisible())
                        {
                            if (button->mouseOnWidget(pos))
                                button->mouseMoved(pos);
                            else
                                button->mouseNoLongerOnWidget();
                        }
                    }
                }
                else // When the mouse is not on the title bar, the mouse can't be on the buttons inside it
                {
                    for (auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
                    {
                        if (button->isVisible())
                            button->mouseNoLongerOnWidget();
                    }
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::keyPressed(const sf::Event::KeyEvent& event)
    {
        if (event.code == sf::Keyboard::Escape)
            onEscapeKeyPressed.emit(this);

        Container::keyPressed(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::mouseNoLongerOnWidget()
    {
        Container::mouseNoLongerOnWidget();

        for (const auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
        {
            if (button->isVisible())
                button->mouseNoLongerOnWidget();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::leftMouseButtonNoLongerDown()
    {
        Container::leftMouseButtonNoLongerDown();

        for (const auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
        {
            if (button->isVisible())
                button->leftMouseButtonNoLongerDown();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::updateTitleBarHeight()
    {
        m_spriteTitleBar.setSize({getSize().x, m_titleBarHeightCached});

        // Set the size of the buttons in the title bar
        for (auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
        {
            if (button->isVisible())
            {
                if (m_spriteTitleBar.isSet() && (button->getSharedRenderer()->getTexture().getData() != nullptr))
                {
                    button->setSize(button->getSharedRenderer()->getTexture().getImageSize().x * (m_titleBarHeightCached / m_spriteTitleBar.getTexture().getImageSize().y),
                                    button->getSharedRenderer()->getTexture().getImageSize().y * (m_titleBarHeightCached / m_spriteTitleBar.getTexture().getImageSize().y));
                }
                else
                    button->setSize({m_titleBarHeightCached * 0.8f, m_titleBarHeightCached * 0.8f});

                const Borders& buttonBorders = button->getSharedRenderer()->getBorders();
                button->setTextSize(Text::findBestTextSize(m_titleText.getFont(), (button->getSize().y - buttonBorders.getTop() - buttonBorders.getBottom()) * 0.8f));
            }
        }

        if (m_titleTextSize == 0)
            m_titleText.setCharacterSize(Text::findBestTextSize(m_fontCached, m_titleBarHeightCached * 0.8f));

        // Reposition the images and text
        setPosition(m_position);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& ChildWindow::getSignal(std::string signalName)
    {
        if (signalName == toLower(onMousePress.getName()))
            return onMousePress;
        else if (signalName == toLower(onClose.getName()))
            return onClose;
        else if (signalName == toLower(onMinimize.getName()))
            return onMinimize;
        else if (signalName == toLower(onMaximize.getName()))
            return onMaximize;
        else if (signalName == toLower(onEscapeKeyPressed.getName()))
            return onEscapeKeyPressed;
        else
            return Container::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            setSize(m_size);
        }
        else if (property == "titlecolor")
        {
            m_titleText.setColor(getSharedRenderer()->getTitleColor());
        }
        else if (property == "texturetitlebar")
        {
            m_spriteTitleBar.setTexture(getSharedRenderer()->getTextureTitleBar());

            // If the title bar height is determined by the texture then update it (note that getTitleBarHeight has a non-trivial implementation)
            m_titleBarHeightCached = getSharedRenderer()->getTitleBarHeight();
            if (m_titleBarHeightCached == m_spriteTitleBar.getTexture().getImageSize().y)
                updateTitleBarHeight();
        }
        else if (property == "texturebackground")
        {
            m_spriteBackground.setTexture(getSharedRenderer()->getTextureBackground());
        }
        else if (property == "titlebarheight")
        {
            m_titleBarHeightCached = getSharedRenderer()->getTitleBarHeight();
            updateTitleBarHeight();
        }
        else if (property == "borderbelowtitlebar")
        {
            m_borderBelowTitleBarCached = getSharedRenderer()->getBorderBelowTitleBar();
        }
        else if (property == "distancetoside")
        {
            m_distanceToSideCached = getSharedRenderer()->getDistanceToSide();
            setPosition(m_position);
        }
        else if (property == "paddingbetweenbuttons")
        {
            m_paddingBetweenButtonsCached = getSharedRenderer()->getPaddingBetweenButtons();
            setPosition(m_position);
        }
        else if (property == "minimumresizableborderwidth")
        {
            m_minimumResizableBorderWidthCached = getSharedRenderer()->getMinimumResizableBorderWidth();
        }
        else if (property == "showtextontitlebuttons")
        {
            m_showTextOnTitleButtonsCached = getSharedRenderer()->getShowTextOnTitleButtons();
            setTitleButtons(m_titleButtons);
        }
        else if (property == "closebutton")
        {
            if (m_closeButton->isVisible())
            {
                m_closeButton->setRenderer(getSharedRenderer()->getCloseButton());
                m_closeButton->setInheritedOpacity(m_opacityCached);
            }

            updateTitleBarHeight();
        }
        else if (property == "maximizebutton")
        {
            if (m_maximizeButton->isVisible())
            {
                m_maximizeButton->setRenderer(getSharedRenderer()->getMaximizeButton());
                m_maximizeButton->setInheritedOpacity(m_opacityCached);
            }

            updateTitleBarHeight();
        }
        else if (property == "minimizebutton")
        {
            if (m_minimizeButton->isVisible())
            {
                m_minimizeButton->setRenderer(getSharedRenderer()->getMinimizeButton());
                m_minimizeButton->setInheritedOpacity(m_opacityCached);
            }

            updateTitleBarHeight();
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == "titlebarcolor")
        {
            m_titleBarColorCached = getSharedRenderer()->getTitleBarColor();
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == "bordercolorfocused")
        {
            m_borderColorFocusedCached = getSharedRenderer()->getBorderColorFocused();
        }
        else if ((property == "opacity") || (property == "opacitydisabled"))
        {
            Container::rendererChanged(property);

            for (auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
            {
                if (button->isVisible())
                    button->setInheritedOpacity(m_opacityCached);
            }

            m_titleText.setOpacity(m_opacityCached);
            m_spriteTitleBar.setOpacity(m_opacityCached);
            m_spriteBackground.setOpacity(m_opacityCached);
        }
        else if (property == "font")
        {
            Container::rendererChanged(property);

            for (auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
            {
                if (button->isVisible())
                    button->setInheritedFont(m_fontCached);
            }

            m_titleText.setFont(m_fontCached);
            if (m_titleTextSize == 0)
                m_titleText.setCharacterSize(Text::findBestTextSize(m_fontCached, getSharedRenderer()->getTitleBarHeight() * 0.8f));

            setPosition(m_position);
        }
        else
            Container::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> ChildWindow::save(SavingRenderersMap& renderers) const
    {
        auto node = Container::save(renderers);

        if (m_titleAlignment == TitleAlignment::Left)
            node->propertyValuePairs["TitleAlignment"] = std::make_unique<DataIO::ValueNode>("Left");
        else if (m_titleAlignment == TitleAlignment::Center)
            node->propertyValuePairs["TitleAlignment"] = std::make_unique<DataIO::ValueNode>("Center");
        else if (m_titleAlignment == TitleAlignment::Right)
            node->propertyValuePairs["TitleAlignment"] = std::make_unique<DataIO::ValueNode>("Right");

        if (getTitle().getSize() > 0)
            node->propertyValuePairs["Title"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(getTitle()));

        if (m_keepInParent)
            node->propertyValuePairs["KeepInParent"] = std::make_unique<DataIO::ValueNode>("true");

        if (m_resizable)
            node->propertyValuePairs["Resizable"] = std::make_unique<DataIO::ValueNode>("true");
        if (m_positionLocked)
            node->propertyValuePairs["PositionLocked"] = std::make_unique<DataIO::ValueNode>("true");

        if (m_minimumSize != Vector2f{})
            node->propertyValuePairs["MinimumSize"] = std::make_unique<DataIO::ValueNode>("(" + to_string(m_minimumSize.x) + ", " + to_string(m_minimumSize.y) + ")");
        if (m_maximumSize != Vector2f{std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()})
            node->propertyValuePairs["MaximumSize"] = std::make_unique<DataIO::ValueNode>("(" + to_string(m_maximumSize.x) + ", " + to_string(m_maximumSize.y) + ")");

        std::string serializedTitleButtons;
        if (m_titleButtons & TitleButton::Minimize)
            serializedTitleButtons += " | Minimize";
        if (m_titleButtons & TitleButton::Maximize)
            serializedTitleButtons += " | Maximize";
        if (m_titleButtons & TitleButton::Close)
            serializedTitleButtons += " | Close";

        if (!serializedTitleButtons.empty())
            serializedTitleButtons.erase(0, 3);
        else
            serializedTitleButtons = "None";

        node->propertyValuePairs["TitleButtons"] = std::make_unique<DataIO::ValueNode>(serializedTitleButtons);

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Container::load(node, renderers);

        if (node->propertyValuePairs["titlealignment"])
        {
            if (toLower(node->propertyValuePairs["titlealignment"]->value) == "left")
                setTitleAlignment(TitleAlignment::Left);
            else if (toLower(node->propertyValuePairs["titlealignment"]->value) == "center")
                setTitleAlignment(TitleAlignment::Center);
            else if (toLower(node->propertyValuePairs["titlealignment"]->value) == "right")
                setTitleAlignment(TitleAlignment::Right);
            else
                throw Exception{"Failed to parse TitleAlignment property. Only the values Left, Center and Right are correct."};
        }

        if (node->propertyValuePairs["titlebuttons"])
        {
            int decodedTitleButtons = TitleButton::None;
            std::vector<std::string> titleButtons = Deserializer::split(node->propertyValuePairs["titlebuttons"]->value, '|');
            for (const auto& elem : titleButtons)
            {
                std::string requestedTitleButton = toLower(trim(elem));
                if (requestedTitleButton == "close")
                    decodedTitleButtons |= TitleButton::Close;
                else if (requestedTitleButton == "maximize")
                    decodedTitleButtons |= TitleButton::Maximize;
                else if (requestedTitleButton == "minimize")
                    decodedTitleButtons |= TitleButton::Minimize;
            }

            setTitleButtons(decodedTitleButtons);
        }

        if (node->propertyValuePairs["title"])
            setTitle(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["title"]->value).getString());

        if (node->propertyValuePairs["keepinparent"])
            setKeepInParent(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["keepinparent"]->value).getBool());

        if (node->propertyValuePairs["resizable"])
            setResizable(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["resizable"]->value).getBool());
        if (node->propertyValuePairs["positionlocked"])
            setPositionLocked(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["positionlocked"]->value).getBool());

        if (node->propertyValuePairs["minimumsize"])
            setMinimumSize(Vector2f{node->propertyValuePairs["minimumsize"]->value});

        if (node->propertyValuePairs["maximumsize"])
            setMaximumSize(Vector2f{node->propertyValuePairs["maximumsize"]->value});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            if (m_focused && m_borderColorFocusedCached.isSet())
                drawBorders(target, states, m_bordersCached, getFullSize(), m_borderColorFocusedCached);
            else
                drawBorders(target, states, m_bordersCached, getFullSize(), m_borderColorCached);

            states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop()});
        }

        // Draw the title bar
        if (m_spriteTitleBar.isSet())
            m_spriteTitleBar.draw(target, states);
        else
            drawRectangleShape(target, states, {getSize().x, m_titleBarHeightCached}, m_titleBarColorCached);

        // Draw the text in the title bar (after setting the clipping area)
        {
            float buttonOffsetX = 0;
            for (const auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
            {
                if (button->isVisible())
                    buttonOffsetX += (buttonOffsetX > 0 ? m_paddingBetweenButtonsCached : 0) + button->getSize().x;
            }

            if (buttonOffsetX > 0)
                buttonOffsetX += m_distanceToSideCached;

            const float clippingLeft = m_distanceToSideCached;
            const float clippingRight = getSize().x - m_distanceToSideCached - buttonOffsetX;

            const Clipping clipping{target, states, {clippingLeft, 0}, {clippingRight - clippingLeft, m_titleBarHeightCached}};

            m_titleText.draw(target, states);
        }

        // Draw the buttons
        states.transform.translate({-m_bordersCached.getLeft(), -m_bordersCached.getTop()});
        for (auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
        {
            if (button->isVisible())
                button->draw(target, states);
        }

        states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop() + m_titleBarHeightCached});

        // Draw the border below the title bar
        if (m_borderBelowTitleBarCached > 0)
        {
            if (m_focused && m_borderColorFocusedCached.isSet())
                drawRectangleShape(target, states, {getSize().x, m_borderBelowTitleBarCached}, m_borderColorFocusedCached);
            else
                drawRectangleShape(target, states, {getSize().x, m_borderBelowTitleBarCached}, m_borderColorCached);

            states.transform.translate({0, m_borderBelowTitleBarCached});
        }

        // Draw the background
        if (m_spriteBackground.isSet())
            m_spriteBackground.draw(target, states);
        else if (m_backgroundColorCached != Color::Transparent)
            drawRectangleShape(target, states, getSize(), m_backgroundColorCached);

        // Draw the widgets in the child window
        const Clipping clipping{target, states, {}, {getSize()}};
        drawWidgetContainer(&target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
        }

        m_text.setCharacterSize(m_listBox->getTextSize());
        m_defaultText.setCharacterSize(m_listBox->getTextSize());
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
            m_text.setColor(getSharedRenderer()->getTextColor());
            if (!getSharedRenderer()->getDefaultTextColor().isSet())
                m_defaultText.setColor(getSharedRenderer()->getTextColor());
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
        else if (property == "texturearrow")
        {
            m_spriteArrow.setTexture(getSharedRenderer()->getTextureArrow());
            setSize(m_size);
        }
        else if (property == "texturearrowhover")
        {
            m_spriteArrowHover.setTexture(getSharedRenderer()->getTextureArrowHover());
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
        else if (property == "arrowbackgroundcolor")
        {
            m_arrowBackgroundColorCached = getSharedRenderer()->getArrowBackgroundColor();
        }
        else if (property == "arrowbackgroundcolorhover")
        {
            m_arrowBackgroundColorHoverCached = getSharedRenderer()->getArrowBackgroundColorHover();
        }
        else if (property == "arrowcolor")
        {
            m_arrowColorCached = getSharedRenderer()->getArrowColor();
        }
        else if (property == "arrowcolorhover")
        {
            m_arrowColorHoverCached = getSharedRenderer()->getArrowColorHover();
        }
        else if ((property == "opacity") || (property == "opacitydisabled"))
        {
            Widget::rendererChanged(property);

            m_spriteBackground.setOpacity(m_opacityCached);
            m_spriteArrow.setOpacity(m_opacityCached);
            m_spriteArrowHover.setOpacity(m_opacityCached);

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
            m_spriteBackground.draw(target, states);
        else
            drawRectangleShape(target, states, getInnerSize(), m_backgroundColorCached);

        // Check if we have a texture for the arrow
        float arrowSize;
        if (m_spriteArrow.isSet())
        {
            arrowSize = m_spriteArrow.getSize().x;
            states.transform.translate({getInnerSize().x - m_paddingCached.getRight() - arrowSize, m_paddingCached.getTop()});

            if (m_mouseHover && m_spriteArrowHover.isSet())
                m_spriteArrowHover.draw(target, states);
            else
                m_spriteArrow.draw(target, states);
        }
        else // There are no textures for the arrow
        {
            arrowSize = getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom();
            states.transform.translate({getInnerSize().x - m_paddingCached.getRight() - arrowSize, m_paddingCached.getTop()});

            if (m_mouseHover && m_arrowBackgroundColorHoverCached.isSet())
                drawRectangleShape(target, states, {arrowSize, arrowSize}, m_arrowBackgroundColorHoverCached);
            else
                drawRectangleShape(target, states, {arrowSize, arrowSize}, m_arrowBackgroundColorCached);

            sf::ConvexShape arrow{3};
            arrow.setPoint(0, {arrowSize / 5, arrowSize / 5});
            arrow.setPoint(1, {arrowSize / 2, arrowSize * 4/5});
            arrow.setPoint(2, {arrowSize * 4/5, arrowSize / 5});

            if (m_mouseHover && m_arrowColorHoverCached.isSet())
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


#include <TGUI/Widgets/Group.hpp>
#include <TGUI/Clipping.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Group::Group(const Layout2d& size)
    {
        m_type = "Group";

        m_renderer = aurora::makeCopied<GroupRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setSize(size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Group::Ptr Group::create(const Layout2d& size)
    {
        return std::make_shared<Group>(size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Group::Ptr Group::copy(Group::ConstPtr group)
    {
        if (group)
            return std::static_pointer_cast<Group>(group->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    GroupRenderer* Group::getSharedRenderer()
    {
        return aurora::downcast<GroupRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const GroupRenderer* Group::getSharedRenderer() const
    {
        return aurora::downcast<const GroupRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    GroupRenderer* Group::getRenderer()
    {
        return aurora::downcast<GroupRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const GroupRenderer* Group::getRenderer() const
    {
        return aurora::downcast<const GroupRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::setSize(const Layout2d& size)
    {
        m_paddingCached.updateParentSize(size.getValue());

        Container::setSize(size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Group::getInnerSize() const
    {
        return {std::max(0.f, getSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight()),
                std::max(0.f, getSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom())};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Group::getChildWidgetsOffset() const
    {
        return {m_paddingCached.getLeft(), m_paddingCached.getTop()};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Group::mouseOnWidget(Vector2f pos) const
    {
        pos -= getPosition();

        if (FloatRect{0, 0, getSize().x, getSize().y}.contains(pos))
        {
            const Vector2f offset = getChildWidgetsOffset();
            for (const auto& widget : m_widgets)
            {
                if (widget->isVisible())
                {
                    if (widget->mouseOnWidget(pos - offset))
                        return true;
                }
            }
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::leftMousePressed(Vector2f pos)
    {
        m_mouseDown = true;
        Container::leftMousePressed(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::rendererChanged(const std::string& property)
    {
        if (property == "padding")
        {
            m_paddingCached = getSharedRenderer()->getPadding();
            setSize(m_size);
        }
        else
            Container::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition().x + m_paddingCached.getLeft(), getPosition().y + m_paddingCached.getTop());

        // Set the clipping for all draw calls that happen until this clipping object goes out of scope
        const Vector2f innerSize = {getSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(),
                                        getSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()};
        const Clipping clipping{target, states, {}, innerSize};

        // Draw the child widgets
        drawWidgetContainer(&target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


#include <TGUI/Widgets/HorizontalLayout.hpp>
#include <numeric>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    HorizontalLayout::HorizontalLayout(const Layout2d& size) :
        BoxLayoutRatios{size}
    {
        m_type = "HorizontalLayout";

        m_renderer = aurora::makeCopied<BoxLayoutRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    HorizontalLayout::Ptr HorizontalLayout::create(const Layout2d& size)
    {
        return std::make_shared<HorizontalLayout>(size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    HorizontalLayout::Ptr HorizontalLayout::copy(HorizontalLayout::ConstPtr layout)
    {
        if (layout)
            return std::static_pointer_cast<HorizontalLayout>(layout->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void HorizontalLayout::updateWidgets()
    {
        const float totalSpaceBetweenWidgets = (m_spaceBetweenWidgetsCached * m_widgets.size()) - m_spaceBetweenWidgetsCached;
        const Vector2f contentSize = {getSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(),
                                          getSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()};

        const float totalRatio = std::accumulate(m_ratios.begin(), m_ratios.end(), 0.f);

        float currentOffset = 0;
        for (std::size_t i = 0; i < m_widgets.size(); ++i)
        {
            auto& widget = m_widgets[i];
            const float width = (contentSize.x - totalSpaceBetweenWidgets) * (m_ratios[i] / totalRatio);

            widget->setSize({width, contentSize.y});
            widget->setPosition({currentOffset, 0});

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

            currentOffset += width + m_spaceBetweenWidgetsCached;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


#include <TGUI/Widgets/Knob.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
    const float pi = 3.14159265358979f;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Knob::Knob()
    {
        m_type = "Knob";
        m_draggableWidget = true;

        m_renderer = aurora::makeCopied<KnobRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setSize(140, 140);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Knob::Ptr Knob::create()
    {
        return std::make_shared<Knob>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Knob::Ptr Knob::copy(Knob::ConstPtr knob)
    {
        if (knob)
            return std::static_pointer_cast<Knob>(knob->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    KnobRenderer* Knob::getSharedRenderer()
    {
        return aurora::downcast<KnobRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const KnobRenderer* Knob::getSharedRenderer() const
    {
        return aurora::downcast<const KnobRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    KnobRenderer* Knob::getRenderer()
    {
        return aurora::downcast<KnobRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const KnobRenderer* Knob::getRenderer() const
    {
        return aurora::downcast<const KnobRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_bordersCached.updateParentSize(getSize());

        if (m_spriteBackground.isSet() && m_spriteForeground.isSet())
        {
            m_spriteBackground.setSize(getInnerSize());
            m_spriteForeground.setSize({m_spriteForeground.getTexture().getImageSize().x / m_spriteBackground.getTexture().getImageSize().x * getInnerSize().x,
                                        m_spriteForeground.getTexture().getImageSize().y / m_spriteBackground.getTexture().getImageSize().y * getInnerSize().y});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setStartRotation(float startRotation)
    {
        while (startRotation >= 360)
            startRotation -= 360;
        while (startRotation < 0)
            startRotation += 360;

        m_startRotation = startRotation;

        // The knob might have to point in a different direction even though it has the same value
        recalculateRotation();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Knob::getStartRotation() const
    {
        return m_startRotation;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setEndRotation(float endRotation)
    {
        while (endRotation >= 360)
            endRotation -= 360;
        while (endRotation < 0)
            endRotation += 360;

        m_endRotation = endRotation;

        // The knob might have to point in a different direction even though it has the same value
        recalculateRotation();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Knob::getEndRotation() const
    {
        return m_endRotation;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setMinimum(int minimum)
    {
        if (m_minimum != minimum)
        {
            // Set the new minimum
            m_minimum = minimum;

            // The maximum can't be below the minimum
            if (m_maximum < m_minimum)
                m_maximum = m_minimum;

            // When the value is below the minimum then adjust it
            if (m_value < m_minimum)
                setValue(m_minimum);

            // The knob might have to point in a different direction even though it has the same value
            recalculateRotation();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int Knob::getMinimum() const
    {
        return m_minimum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setMaximum(int maximum)
    {
        if (m_maximum != maximum)
        {
            // Set the new maximum
            if (maximum > 0)
                m_maximum = maximum;
            else
                m_maximum = 1;

            // The minimum can't be below the maximum
            if (m_minimum > m_maximum)
                m_minimum = m_maximum;

            // When the value is above the maximum then adjust it
            if (m_value > m_maximum)
                setValue(m_maximum);

            // The knob might have to point in a different direction even though it has the same value
            recalculateRotation();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int Knob::getMaximum() const
    {
        return m_maximum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setValue(int value)
    {
        if (m_value != value)
        {
            // Set the new value
            m_value = value;

            // When the value is below the minimum or above the maximum then adjust it
            if (m_value < m_minimum)
                m_value = m_minimum;
            else if (m_value > m_maximum)
                m_value = m_maximum;

            // The knob might have to point in a different direction
            recalculateRotation();

            onValueChange.emit(this, m_value);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int Knob::getValue() const
    {
        return m_value;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setClockwiseTurning(bool clockwise)
    {
        m_clockwiseTurning = clockwise;

        // The knob might have to point in a different direction even though it has the same value
        recalculateRotation();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Knob::getClockwiseTurning() const
    {
        return m_clockwiseTurning;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Knob::mouseOnWidget(Vector2f pos) const
    {
        pos -= getPosition();

        // Check if the mouse is on top of the widget
        if (FloatRect{0, 0, getSize().x, getSize().y}.contains(pos))
        {
            if (m_spriteBackground.isSet() && m_spriteForeground.isSet())
            {
                // Only return true when the pixel under the mouse isn't transparent
                if (!m_transparentTextureCached || !m_spriteBackground.isTransparentPixel(pos - m_bordersCached.getOffset())
                 || !m_spriteForeground.isTransparentPixel(pos - m_bordersCached.getOffset() - ((getInnerSize() - m_spriteForeground.getSize()) / 2.f)))
                    return true;
            }
            else // There is no texture, the widget has a circle shape
            {
                const Vector2f centerPoint = getSize() / 2.f;
                const float distance = std::sqrt(std::pow(centerPoint.x - pos.x, 2.f) + std::pow(centerPoint.y - pos.y, 2.f));
                return (distance <= std::min(getSize().x, getSize().y));
            }
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::leftMousePressed(Vector2f pos)
    {
        // Set the mouse down flag
        m_mouseDown = true;

        // Change the value of the knob depending on where you clicked
        mouseMoved(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::leftMouseReleased(Vector2f)
    {
        m_mouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::mouseMoved(Vector2f pos)
    {
        pos -= getPosition();

        if (!m_mouseHover)
            mouseEnteredWidget();

        const Vector2f centerPosition = getSize() / 2.0f;

        // Check if the mouse button is down
        if (m_mouseDown)
        {
            // Find out the direction that the knob should now point
            if (compareFloats(pos.x, centerPosition.x))
            {
                if (pos.y > centerPosition.y)
                    m_angle = 270;
                else if (pos.y < centerPosition.y)
                    m_angle = 90;
            }
            else
            {
                m_angle = std::atan2(centerPosition.y - pos.y, pos.x - centerPosition.x) * 180.0f / pi;
                if (m_angle < 0)
                    m_angle += 360;
            }

            // The angle might lie on a part where it isn't allowed
            if (m_angle > m_startRotation)
            {
                if ((m_angle < m_endRotation) && (m_clockwiseTurning))
                {
                    if ((m_angle - m_startRotation) <= (m_endRotation - m_angle))
                        m_angle = m_startRotation;
                    else
                        m_angle = m_endRotation;
                }
                else if (m_angle > m_endRotation)
                {
                    if (((m_startRotation > m_endRotation) && (m_clockwiseTurning))
                     || ((m_startRotation < m_endRotation) && (!m_clockwiseTurning)))
                    {
                        if (std::min(m_angle - m_startRotation, 360 - m_angle + m_startRotation) <= std::min(m_angle - m_endRotation, 360 - m_angle + m_endRotation))
                            m_angle = m_startRotation;
                        else
                            m_angle = m_endRotation;
                    }
                }
            }
            else if (m_angle < m_startRotation)
            {
                if (m_angle < m_endRotation)
                {
                    if (((m_startRotation > m_endRotation) && (m_clockwiseTurning))
                     || ((m_startRotation < m_endRotation) && (!m_clockwiseTurning)))
                    {
                        if (std::min(m_startRotation - m_angle, 360 - m_startRotation + m_angle) <= std::min(m_endRotation - m_angle, 360 -m_endRotation + m_angle))
                            m_angle = m_startRotation;
                        else
                            m_angle = m_endRotation;
                    }
                }
                else if ((m_angle > m_endRotation) && (!m_clockwiseTurning))
                {
                    if ((m_startRotation - m_angle) <= (m_angle - m_endRotation))
                        m_angle = m_startRotation;
                    else
                        m_angle = m_endRotation;
                }
            }

            // Calculate the difference in degrees between the start and end rotation
            float allowedAngle = 0;
            if (compareFloats(m_startRotation, m_endRotation))
                allowedAngle = 360;
            else
            {
                if (((m_endRotation > m_startRotation) && (m_clockwiseTurning))
                 || ((m_endRotation < m_startRotation) && (!m_clockwiseTurning)))
                {
                    allowedAngle = 360 - std::abs(m_endRotation - m_startRotation);
                }
                else if (((m_endRotation > m_startRotation) && (!m_clockwiseTurning))
                      || ((m_endRotation < m_startRotation) && (m_clockwiseTurning)))
                {
                    allowedAngle = std::abs(m_endRotation - m_startRotation);
                }
            }

            // Calculate the right value
            if (m_clockwiseTurning)
            {
                if (m_angle < m_startRotation)
                    setValue(static_cast<int>(((m_startRotation - m_angle) / allowedAngle * (m_maximum - m_minimum)) + m_minimum));
                else
                {
                    if (compareFloats(m_angle, m_startRotation))
                        setValue(m_minimum);
                    else
                        setValue(static_cast<int>((((360.0f - m_angle) + m_startRotation) / allowedAngle * (m_maximum - m_minimum)) + m_minimum));
                }
            }
            else // counter-clockwise
            {
                if (m_angle >= m_startRotation)
                    setValue(static_cast<int>(((m_angle - m_startRotation) / allowedAngle * (m_maximum - m_minimum)) + m_minimum));
                else
                {
                    setValue(static_cast<int>(((m_angle + (360.0f - m_startRotation)) / allowedAngle * (m_maximum - m_minimum)) + m_minimum));
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::recalculateRotation()
    {
        // Calculate the difference in degrees between the start and end rotation
        float allowedAngle = 0;
        if (compareFloats(m_startRotation, m_endRotation))
            allowedAngle = 360;
        else
        {
            if (((m_endRotation > m_startRotation) && (m_clockwiseTurning))
             || ((m_endRotation < m_startRotation) && (!m_clockwiseTurning)))
            {
                allowedAngle = 360 - std::abs(m_endRotation - m_startRotation);
            }
            else if (((m_endRotation > m_startRotation) && (!m_clockwiseTurning))
                  || ((m_endRotation < m_startRotation) && (m_clockwiseTurning)))
            {
                allowedAngle = std::abs(m_endRotation - m_startRotation);
            }
        }

        // Calculate the angle for the direction of the knob
        if (m_clockwiseTurning)
        {
            if (m_value == m_minimum)
                m_angle = m_startRotation;
            else
                m_angle = m_startRotation - (((m_value - m_minimum) / static_cast<float>(m_maximum - m_minimum)) * allowedAngle);
        }
        else // counter-clockwise
        {
            m_angle = (((m_value - m_minimum) / static_cast<float>(m_maximum - m_minimum)) * allowedAngle) + m_startRotation;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& Knob::getSignal(std::string signalName)
    {
        if (signalName == toLower(onValueChange.getName()))
            return onValueChange;
        else
            return Widget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            setSize(m_size);
        }
        else if (property == "texturebackground")
        {
            m_spriteBackground.setTexture(getSharedRenderer()->getTextureBackground());
            setSize(m_size);
        }
        else if (property == "textureforeground")
        {
            m_spriteForeground.setTexture(getSharedRenderer()->getTextureForeground());
            setSize(m_size);
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == "thumbcolor")
        {
            m_thumbColorCached = getSharedRenderer()->getThumbColor();
        }
        else if (property == "imagerotation")
        {
            m_imageRotationCached = getSharedRenderer()->getImageRotation();
        }
        else if ((property == "opacity") || (property == "opacitydisabled"))
        {
            Widget::rendererChanged(property);

            m_spriteBackground.setOpacity(m_opacityCached);
            m_spriteForeground.setOpacity(m_opacityCached);
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> Knob::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        node->propertyValuePairs["ClockwiseTurning"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_clockwiseTurning));
        node->propertyValuePairs["StartRotation"] = std::make_unique<DataIO::ValueNode>(to_string(m_startRotation));
        node->propertyValuePairs["EndRotation"] = std::make_unique<DataIO::ValueNode>(to_string(m_endRotation));
        node->propertyValuePairs["Minimum"] = std::make_unique<DataIO::ValueNode>(to_string(m_minimum));
        node->propertyValuePairs["Maximum"] = std::make_unique<DataIO::ValueNode>(to_string(m_maximum));
        node->propertyValuePairs["Value"] = std::make_unique<DataIO::ValueNode>(to_string(m_value));

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs["startrotation"])
            setStartRotation(strToFloat(node->propertyValuePairs["startrotation"]->value));
        if (node->propertyValuePairs["endrotation"])
            setEndRotation(strToFloat(node->propertyValuePairs["endrotation"]->value));
        if (node->propertyValuePairs["minimum"])
            setMinimum(strToInt(node->propertyValuePairs["minimum"]->value));
        if (node->propertyValuePairs["maximum"])
            setMaximum(strToInt(node->propertyValuePairs["maximum"]->value));
        if (node->propertyValuePairs["value"])
            setValue(strToInt(node->propertyValuePairs["value"]->value));
        if (node->propertyValuePairs["clockwiseturning"])
            setClockwiseTurning(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["clockwiseturning"]->value).getBool());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Knob::getInnerSize() const
    {
        return {std::max(0.f, getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight()),
                std::max(0.f, getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom())};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        const float size = std::min(getInnerSize().x, getInnerSize().y);

        // Draw the borders
        const float borderThickness = std::min({m_bordersCached.getLeft(), m_bordersCached.getTop(), m_bordersCached.getRight(), m_bordersCached.getBottom()});
        if (borderThickness > 0)
        {
            states.transform.translate({borderThickness, borderThickness});

            sf::CircleShape bordersShape{size / 2};
            bordersShape.setFillColor(Color::Transparent);
            bordersShape.setOutlineColor(Color::calcColorOpacity(m_borderColorCached, m_opacityCached));
            bordersShape.setOutlineThickness(borderThickness);
            target.draw(bordersShape, states);
        }

        // Draw the background
        if (m_spriteBackground.isSet())
            m_spriteBackground.draw(target, states);
        else
        {
            sf::CircleShape background{size / 2};
            background.setFillColor(Color::calcColorOpacity(m_backgroundColorCached, m_opacityCached));
            target.draw(background, states);
        }

        // Draw the foreground
        if (m_spriteForeground.isSet())
        {
            states.transform.translate((getInnerSize() - m_spriteForeground.getSize()) / 2.f);

            // Give the image the correct rotation
            if (m_imageRotationCached > m_angle)
                states.transform.rotate(m_imageRotationCached - m_angle, (m_spriteForeground.getSize() / 2.f));
            else
                states.transform.rotate(360 - m_angle + m_imageRotationCached, (m_spriteForeground.getSize() / 2.f));

            m_spriteForeground.draw(target, states);
        }
        else
        {
            sf::CircleShape thumb{size / 10.0f};
            thumb.setFillColor(Color::calcColorOpacity(m_thumbColorCached, m_opacityCached));
            thumb.setPosition({(size / 2.0f) - thumb.getRadius() + (std::cos(m_angle / 180 * pi) * (size / 2) * 3/5),
                               (size / 2.0f) - thumb.getRadius() + (-std::sin(m_angle / 180 * pi) * (size / 2) * 3/5)});
            target.draw(thumb, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


#include <TGUI/Widgets/ListBox.hpp>
#include <TGUI/Clipping.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListBox::ListBox()
    {
        m_type = "ListBox";

        m_draggableWidget = true;

        m_renderer = aurora::makeCopied<ListBoxRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setTextSize(getGlobalTextSize());
        setItemHeight(static_cast<unsigned int>(Text::getLineHeight(m_fontCached, m_textSize, m_textStyleCached) * 1.25f));
        setSize({Text::getLineHeight(m_fontCached, m_textSize, m_textStyleCached) * 10,
                 (m_itemHeight * 7) + m_paddingCached.getTop() + m_paddingCached.getBottom() + m_bordersCached.getTop() + m_bordersCached.getBottom()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListBox::Ptr ListBox::create()
    {
        return std::make_shared<ListBox>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListBox::Ptr ListBox::copy(ListBox::ConstPtr listBox)
    {
        if (listBox)
            return std::static_pointer_cast<ListBox>(listBox->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListBoxRenderer* ListBox::getSharedRenderer()
    {
        return aurora::downcast<ListBoxRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ListBoxRenderer* ListBox::getSharedRenderer() const
    {
        return aurora::downcast<const ListBoxRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListBoxRenderer* ListBox::getRenderer()
    {
        return aurora::downcast<ListBoxRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ListBoxRenderer* ListBox::getRenderer() const
    {
        return aurora::downcast<const ListBoxRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setPosition(const Layout2d& position)
    {
        Widget::setPosition(position);

        for (std::size_t i = 0; i < m_items.size(); ++i)
            m_items[i].setPosition({0, (i * m_itemHeight) + ((m_itemHeight - m_items[i].getSize().y) / 2.0f)});

        m_scroll->setPosition(getSize().x - m_bordersCached.getRight() - m_scroll->getSize().x, m_bordersCached.getTop());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_bordersCached.updateParentSize(getSize());
        m_paddingCached.updateParentSize(getSize());

        m_spriteBackground.setSize(getInnerSize());

        m_scroll->setSize({m_scroll->getSize().x, std::max(0.f, getInnerSize().y)});
        m_scroll->setViewportSize(static_cast<unsigned int>(getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()));

        setPosition(m_position);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::addItem(const sf::String& itemName, const sf::String& id)
    {
        // Check if the item limit is reached (if there is one)
        if ((m_maxItems == 0) || (m_items.size() < m_maxItems))
        {
            m_scroll->setMaximum(static_cast<unsigned int>((m_items.size() + 1) * m_itemHeight));

            // Scroll down when auto-scrolling is enabled
            if (m_autoScroll && (m_scroll->getViewportSize() < m_scroll->getMaximum()))
                m_scroll->setValue(m_scroll->getMaximum() - m_scroll->getViewportSize());

            // Create the new item
            Text newItem;
            newItem.setFont(m_fontCached);
            newItem.setColor(m_textColorCached);
            newItem.setOpacity(m_opacityCached);
            newItem.setStyle(m_textStyleCached);
            newItem.setCharacterSize(m_textSize);
            newItem.setString(itemName);
            newItem.setPosition({0, (m_items.size() * m_itemHeight) + ((m_itemHeight - newItem.getSize().y) / 2.0f)});

            // Add the new item to the list
            m_items.push_back(std::move(newItem));
            m_itemIds.push_back(id);
            return true;
        }
        else // The item limit was reached
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::setSelectedItem(const sf::String& itemName)
    {
        for (std::size_t i = 0; i < m_items.size(); ++i)
        {
            if (m_items[i].getString() == itemName)
                return setSelectedItemByIndex(i);
        }

        // No match was found
        deselectItem();
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::setSelectedItemById(const sf::String& id)
    {
        for (std::size_t i = 0; i < m_itemIds.size(); ++i)
        {
            if (m_itemIds[i] == id)
                return setSelectedItemByIndex(i);
        }

        // No match was found
        deselectItem();
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::setSelectedItemByIndex(std::size_t index)
    {
        if (index >= m_items.size())
        {
            deselectItem();
            return false;
        }

        updateSelectedItem(static_cast<int>(index));

        // Move the scrollbar
        if (m_selectedItem * getItemHeight() < m_scroll->getValue())
            m_scroll->setValue(m_selectedItem * getItemHeight());
        else if ((m_selectedItem + 1) * getItemHeight() > m_scroll->getValue() + m_scroll->getViewportSize())
            m_scroll->setValue((m_selectedItem + 1) * getItemHeight() - m_scroll->getViewportSize());

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::deselectItem()
    {
        updateSelectedItem(-1);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::removeItem(const sf::String& itemName)
    {
        for (std::size_t i = 0; i < m_items.size(); ++i)
        {
            if (m_items[i].getString() == itemName)
                return removeItemByIndex(i);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::removeItemById(const sf::String& id)
    {
        for (std::size_t i = 0; i < m_itemIds.size(); ++i)
        {
            if (m_itemIds[i] == id)
                return removeItemByIndex(i);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::removeItemByIndex(std::size_t index)
    {
        if (index >= m_items.size())
            return false;

        // Keep it simple and forget hover when an item is removed
        updateHoveringItem(-1);

        // Check if the selected item should change
        if (m_selectedItem == static_cast<int>(index))
            updateSelectedItem(-1);
        else if (m_selectedItem > static_cast<int>(index))
        {
            // Don't call updateSelectedItem here, there should not be no callback and the item hasn't been erased yet so it would point to the wrong place
            m_selectedItem = m_selectedItem - 1;
        }

        // Remove the item
        m_items.erase(m_items.begin() + index);
        m_itemIds.erase(m_itemIds.begin() + index);

        m_scroll->setMaximum(static_cast<unsigned int>(m_items.size() * m_itemHeight));
        setPosition(m_position);

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::removeAllItems()
    {
        // Unselect any selected item
        updateSelectedItem(-1);
        updateHoveringItem(-1);

        // Clear the list, remove all items
        m_items.clear();
        m_itemIds.clear();

        m_scroll->setMaximum(0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String ListBox::getItemById(const sf::String& id) const
    {
        for (std::size_t i = 0; i < m_itemIds.size(); ++i)
        {
            if (m_itemIds[i] == id)
                return m_items[i].getString();
        }

        return "";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String ListBox::getItemByIndex(std::size_t index) const
    {
        if (index >= m_items.size())
            return "";

        return m_items[index].getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String ListBox::getSelectedItem() const
    {
        return (m_selectedItem >= 0) ? m_items[m_selectedItem].getString() : "";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String ListBox::getSelectedItemId() const
    {
        return (m_selectedItem >= 0) ? m_itemIds[m_selectedItem] : "";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int ListBox::getSelectedItemIndex() const
    {
        return m_selectedItem;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::changeItem(const sf::String& originalValue, const sf::String& newValue)
    {
        for (std::size_t i = 0; i < m_items.size(); ++i)
        {
            if (m_items[i].getString() == originalValue)
                return changeItemByIndex(i, newValue);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::changeItemById(const sf::String& id, const sf::String& newValue)
    {
        for (std::size_t i = 0; i < m_items.size(); ++i)
        {
            if (m_itemIds[i] == id)
                return changeItemByIndex(i, newValue);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::changeItemByIndex(std::size_t index, const sf::String& newValue)
    {
        if (index >= m_items.size())
            return false;

        m_items[index].setString(newValue);
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t ListBox::getItemCount() const
    {
        return m_items.size();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<sf::String> ListBox::getItems() const
    {
        std::vector<sf::String> items;
        for (const auto& item : m_items)
            items.push_back(item.getString());

        return items;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::vector<sf::String>& ListBox::getItemIds() const
    {
        return m_itemIds;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setItemHeight(unsigned int itemHeight)
    {
        // Set the new heights
        m_itemHeight = itemHeight;
        if (m_requestedTextSize == 0)
        {
            m_textSize = Text::findBestTextSize(m_fontCached, itemHeight * 0.8f);
            for (auto& item : m_items)
                item.setCharacterSize(m_textSize);
        }

        m_scroll->setScrollAmount(m_itemHeight);
        m_scroll->setMaximum(static_cast<unsigned int>(m_items.size() * m_itemHeight));
        setPosition(m_position);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ListBox::getItemHeight() const
    {
        return m_itemHeight;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setTextSize(unsigned int textSize)
    {
        m_requestedTextSize = textSize;

        if (textSize)
            m_textSize = textSize;
        else
            m_textSize = Text::findBestTextSize(m_fontCached, m_itemHeight * 0.8f);

        for (auto& item : m_items)
            item.setCharacterSize(m_textSize);

        setPosition(m_position);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setMaximumItems(std::size_t maximumItems)
    {
        // Set the new limit
        m_maxItems = maximumItems;

        // Check if we already passed the limit
        if ((m_maxItems > 0) && (m_maxItems < m_items.size()))
        {
            // Remove the items that passed the limitation
            m_items.erase(m_items.begin() + m_maxItems, m_items.end());
            m_itemIds.erase(m_itemIds.begin() + m_maxItems, m_itemIds.end());

            m_scroll->setMaximum(static_cast<unsigned int>(m_items.size() * m_itemHeight));
            setPosition(m_position);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t ListBox::getMaximumItems() const
    {
        return m_maxItems;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setAutoScroll(bool autoScroll)
    {
        m_autoScroll = autoScroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::getAutoScroll() const
    {
        return m_autoScroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::contains(const sf::String& item) const
    {
        return std::find_if(m_items.begin(), m_items.end(), [item](const Text& text){ return text.getString() == item; }) != m_items.end();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::containsId(const sf::String& id) const
    {
        return std::find(m_itemIds.begin(), m_itemIds.end(), id) != m_itemIds.end();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setScrollbarValue(unsigned int value)
    {
        m_scroll->setValue(value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ListBox::getScrollbarValue() const
    {
        return m_scroll->getValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::mouseOnWidget(Vector2f pos) const
    {
        if (FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(pos))
        {
            if (!m_transparentTextureCached || !m_spriteBackground.isTransparentPixel(pos - getPosition() - m_bordersCached.getOffset()))
                return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::leftMousePressed(Vector2f pos)
    {
        pos -= getPosition();

        m_mouseDown = true;

        if (m_scroll->mouseOnWidget(pos))
        {
            m_scroll->leftMousePressed(pos);
        }
        else
        {
            if (FloatRect{m_bordersCached.getLeft() + m_paddingCached.getLeft(), m_bordersCached.getTop() + m_paddingCached.getTop(),
                          getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(), getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}.contains(pos))
            {
                pos.y -= m_bordersCached.getTop() + m_paddingCached.getTop();

                int hoveringItem = static_cast<int>(((pos.y - (m_itemHeight - (m_scroll->getValue() % m_itemHeight))) / m_itemHeight) + (m_scroll->getValue() / m_itemHeight) + 1);
                if (hoveringItem < static_cast<int>(m_items.size()))
                    updateHoveringItem(hoveringItem);
                else
                    updateHoveringItem(-1);

                if (m_selectedItem != m_hoveringItem)
                {
                    m_possibleDoubleClick = false;

                    updateSelectedItem(m_hoveringItem);
                }

                // Call the MousePress event after the item has already been changed, so that selected item represents the clicked item
                if (m_selectedItem >= 0)
                    onMousePress.emit(this, m_items[m_selectedItem].getString(), m_itemIds[m_selectedItem], m_selectedItem);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::leftMouseReleased(Vector2f pos)
    {
        if (m_mouseDown && !m_scroll->isMouseDown())
        {
            if (m_selectedItem >= 0)
                onMouseRelease.emit(this, m_items[m_selectedItem].getString(), m_itemIds[m_selectedItem], m_selectedItem);

            // Check if you double-clicked
            if (m_possibleDoubleClick)
            {
                m_possibleDoubleClick = false;

                if (m_selectedItem >= 0)
                    onDoubleClick.emit(this, m_items[m_selectedItem].getString(), m_itemIds[m_selectedItem], m_selectedItem);
            }
            else // This is the first click
            {
                m_animationTimeElapsed = {};
                m_possibleDoubleClick = true;
            }
        }

        m_scroll->leftMouseReleased(pos - getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::mouseMoved(Vector2f pos)
    {
        pos -= getPosition();

        if (!m_mouseHover)
            mouseEnteredWidget();

        updateHoveringItem(-1);

        // Check if the mouse event should go to the scrollbar
        if ((m_scroll->isMouseDown() && m_scroll->isMouseDownOnThumb()) || m_scroll->mouseOnWidget(pos))
        {
            m_scroll->mouseMoved(pos);
        }
        else // Mouse not on scrollbar or dragging the scrollbar thumb
        {
            m_scroll->mouseNoLongerOnWidget();

            // Find out on which item the mouse is hovering
            if (FloatRect{m_bordersCached.getLeft() + m_paddingCached.getLeft(),
                          m_bordersCached.getTop() + m_paddingCached.getTop(), getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(), getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}.contains(pos))
            {
                pos.y -= m_bordersCached.getTop() + m_paddingCached.getTop();

                int hoveringItem = static_cast<int>(((pos.y - (m_itemHeight - (m_scroll->getValue() % m_itemHeight))) / m_itemHeight) + (m_scroll->getValue() / m_itemHeight) + 1);
                if (hoveringItem < static_cast<int>(m_items.size()))
                    updateHoveringItem(hoveringItem);
                else
                    updateHoveringItem(-1);

                // If the mouse is held down then select the item below the mouse
                if (m_mouseDown && !m_scroll->isMouseDown())
                {
                    if (m_selectedItem != m_hoveringItem)
                    {
                        m_possibleDoubleClick = false;

                        updateSelectedItem(m_hoveringItem);
                    }
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::mouseWheelScrolled(float delta, Vector2f pos)
    {
        if (m_scroll->isShown())
        {
            m_scroll->mouseWheelScrolled(delta, pos - getPosition());

            // Update on which item the mouse is hovering
            mouseMoved(pos);
            return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::mouseNoLongerOnWidget()
    {
        Widget::mouseNoLongerOnWidget();
        m_scroll->mouseNoLongerOnWidget();

        updateHoveringItem(-1);

        m_possibleDoubleClick = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::leftMouseButtonNoLongerDown()
    {
        Widget::leftMouseButtonNoLongerDown();
        m_scroll->leftMouseButtonNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& ListBox::getSignal(std::string signalName)
    {
        if (signalName == toLower(onItemSelect.getName()))
            return onItemSelect;
        else if (signalName == toLower(onMousePress.getName()))
            return onMousePress;
        else if (signalName == toLower(onMouseRelease.getName()))
            return onMouseRelease;
        else if (signalName == toLower(onDoubleClick.getName()))
            return onDoubleClick;
        else
            return Widget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::rendererChanged(const std::string& property)
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
            updateItemColorsAndStyle();
        }
        else if (property == "textcolorhover")
        {
            m_textColorHoverCached = getSharedRenderer()->getTextColorHover();
            updateItemColorsAndStyle();
        }
        else if (property == "selectedtextcolor")
        {
            m_selectedTextColorCached = getSharedRenderer()->getSelectedTextColor();
            updateItemColorsAndStyle();
        }
        else if (property == "selectedtextcolorhover")
        {
            m_selectedTextColorHoverCached = getSharedRenderer()->getSelectedTextColorHover();
            updateItemColorsAndStyle();
        }
        else if (property == "texturebackground")
        {
            m_spriteBackground.setTexture(getSharedRenderer()->getTextureBackground());
        }
        else if (property == "textstyle")
        {
            m_textStyleCached = getSharedRenderer()->getTextStyle();

            for (auto& item : m_items)
                item.setStyle(m_textStyleCached);

            if ((m_selectedItem >= 0) && m_selectedTextStyleCached.isSet())
                m_items[m_selectedItem].setStyle(m_selectedTextStyleCached);
        }
        else if (property == "selectedtextstyle")
        {
            m_selectedTextStyleCached = getSharedRenderer()->getSelectedTextStyle();

            if (m_selectedItem >= 0)
            {
                if (m_selectedTextStyleCached.isSet())
                    m_items[m_selectedItem].setStyle(m_selectedTextStyleCached);
                else
                    m_items[m_selectedItem].setStyle(m_textStyleCached);
            }
        }
        else if (property == "scrollbar")
        {
            m_scroll->setRenderer(getSharedRenderer()->getScrollbar());

            // If no scrollbar width was set then we may need to use the one from the texture
            if (!getSharedRenderer()->getScrollbarWidth())
            {
                m_scroll->setSize({m_scroll->getDefaultWidth(), m_scroll->getSize().y});
                setSize(m_size);
            }
        }
        else if (property == "scrollbarwidth")
        {
            const float width = getSharedRenderer()->getScrollbarWidth() ? getSharedRenderer()->getScrollbarWidth() : m_scroll->getDefaultWidth();
            m_scroll->setSize({width, m_scroll->getSize().y});
            setSize(m_size);
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == "backgroundcolorhover")
        {
            m_backgroundColorHoverCached = getSharedRenderer()->getBackgroundColorHover();
        }
        else if (property == "selectedbackgroundcolor")
        {
            m_selectedBackgroundColorCached = getSharedRenderer()->getSelectedBackgroundColor();
        }
        else if (property == "selectedbackgroundcolorhover")
        {
            m_selectedBackgroundColorHoverCached = getSharedRenderer()->getSelectedBackgroundColorHover();
        }
        else if ((property == "opacity") || (property == "opacitydisabled"))
        {
            Widget::rendererChanged(property);

            m_scroll->setInheritedOpacity(m_opacityCached);
            m_spriteBackground.setOpacity(m_opacityCached);
            for (auto& item : m_items)
                item.setOpacity(m_opacityCached);
        }
        else if (property == "font")
        {
            Widget::rendererChanged(property);

            for (auto& item : m_items)
                item.setFont(m_fontCached);

            // Recalculate the text size with the new font
            if (m_requestedTextSize == 0)
            {
                m_textSize = Text::findBestTextSize(m_fontCached, m_itemHeight * 0.8f);
                for (auto& item : m_items)
                    item.setCharacterSize(m_textSize);
            }

            setPosition(m_position);
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> ListBox::save(SavingRenderersMap& renderers) const
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

        if (!m_autoScroll)
            node->propertyValuePairs["AutoScroll"] = std::make_unique<DataIO::ValueNode>("false");

        if (m_selectedItem >= 0)
            node->propertyValuePairs["SelectedItemIndex"] = std::make_unique<DataIO::ValueNode>(to_string(m_selectedItem));

        node->propertyValuePairs["TextSize"] = std::make_unique<DataIO::ValueNode>(to_string(m_textSize));
        node->propertyValuePairs["ItemHeight"] = std::make_unique<DataIO::ValueNode>(to_string(m_itemHeight));
        node->propertyValuePairs["MaximumItems"] = std::make_unique<DataIO::ValueNode>(to_string(m_maxItems));

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
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

        if (node->propertyValuePairs["autoscroll"])
            setAutoScroll(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["autoscroll"]->value).getBool());
        if (node->propertyValuePairs["textsize"])
            setTextSize(strToInt(node->propertyValuePairs["textsize"]->value));
        if (node->propertyValuePairs["itemheight"])
            setItemHeight(strToInt(node->propertyValuePairs["itemheight"]->value));
        if (node->propertyValuePairs["maximumitems"])
            setMaximumItems(strToInt(node->propertyValuePairs["maximumitems"]->value));
        if (node->propertyValuePairs["selecteditemindex"])
            setSelectedItemByIndex(strToInt(node->propertyValuePairs["selecteditemindex"]->value));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ListBox::getInnerSize() const
    {
        return {std::max(0.f, getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight()),
                std::max(0.f, getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom())};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::updateSelectedAndHoveringItemColorsAndStyle()
    {
        if (m_selectedItem >= 0)
        {
            if ((m_selectedItem == m_hoveringItem) && m_selectedTextColorHoverCached.isSet())
                m_items[m_selectedItem].setColor(m_selectedTextColorHoverCached);
            else if (m_selectedTextColorCached.isSet())
                m_items[m_selectedItem].setColor(m_selectedTextColorCached);

            if (m_selectedTextStyleCached.isSet())
                m_items[m_selectedItem].setStyle(m_selectedTextStyleCached);
        }

        if ((m_hoveringItem >= 0) && (m_selectedItem != m_hoveringItem))
        {
            if (m_textColorHoverCached.isSet())
                m_items[m_hoveringItem].setColor(m_textColorHoverCached);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::updateItemColorsAndStyle()
    {
        for (auto& item : m_items)
        {
            item.setColor(m_textColorCached);
            item.setStyle(m_textStyleCached);
        }

        updateSelectedAndHoveringItemColorsAndStyle();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::updateHoveringItem(int item)
    {
        if (m_hoveringItem != item)
        {
            if (m_hoveringItem >= 0)
            {
                if ((m_selectedItem == m_hoveringItem) && m_selectedTextColorCached.isSet())
                    m_items[m_hoveringItem].setColor(m_selectedTextColorCached);
                else
                    m_items[m_hoveringItem].setColor(m_textColorCached);
            }

            m_hoveringItem = item;

            updateSelectedAndHoveringItemColorsAndStyle();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::updateSelectedItem(int item)
    {
        if (m_selectedItem != item)
        {
            if (m_selectedItem >= 0)
            {
                if ((m_selectedItem == m_hoveringItem) && m_textColorHoverCached.isSet())
                    m_items[m_selectedItem].setColor(m_textColorHoverCached);
                else
                    m_items[m_selectedItem].setColor(m_textColorCached);

                m_items[m_selectedItem].setStyle(m_textStyleCached);
            }

            m_selectedItem = item;
            if (m_selectedItem >= 0)
                onItemSelect.emit(this, m_items[m_selectedItem].getString(), m_itemIds[m_selectedItem], m_selectedItem);
            else
                onItemSelect.emit(this, "", "", m_selectedItem);

            updateSelectedAndHoveringItemColorsAndStyle();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::update(sf::Time elapsedTime)
    {
        Widget::update(elapsedTime);

        if (m_animationTimeElapsed >= sf::milliseconds(getDoubleClickTime()))
        {
            m_animationTimeElapsed = {};
            m_possibleDoubleClick = false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());
        const sf::RenderStates statesForScrollbar = states;

        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            drawBorders(target, states, m_bordersCached, getSize(), m_borderColorCached);
            states.transform.translate(m_bordersCached.getOffset());
        }

        // Draw the background
        if (m_spriteBackground.isSet())
            m_spriteBackground.draw(target, states);
        else
            drawRectangleShape(target, states, getInnerSize(), m_backgroundColorCached);

        // Draw the items and their selected/hover backgrounds
        {
            // Set the clipping for all draw calls that happen until this clipping object goes out of scope
            float maxItemWidth = getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight();
            if (m_scroll->isShown())
                maxItemWidth -= m_scroll->getSize().x;
            const Clipping clipping{target, states, {m_paddingCached.getLeft(), m_paddingCached.getTop()}, {maxItemWidth, getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}};

            // Find out which items are visible
            std::size_t firstItem = 0;
            std::size_t lastItem = m_items.size();
            if (m_scroll->getViewportSize() < m_scroll->getMaximum())
            {
                firstItem = m_scroll->getValue() / m_itemHeight;
                lastItem = (m_scroll->getValue() + m_scroll->getViewportSize()) / m_itemHeight;

                // Show another item when the scrollbar is standing between two items
                if ((m_scroll->getValue() + m_scroll->getViewportSize()) % m_itemHeight != 0)
                    ++lastItem;
            }

            states.transform.translate({m_paddingCached.getLeft(), m_paddingCached.getTop() - m_scroll->getValue()});

            // Draw the background of the selected item
            if (m_selectedItem >= 0)
            {
                states.transform.translate({0, m_selectedItem * static_cast<float>(m_itemHeight)});

                const Vector2f size = {getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(), static_cast<float>(m_itemHeight)};
                if ((m_selectedItem == m_hoveringItem) && m_selectedBackgroundColorHoverCached.isSet())
                    drawRectangleShape(target, states, size, m_selectedBackgroundColorHoverCached);
                else
                    drawRectangleShape(target, states, size, m_selectedBackgroundColorCached);

                states.transform.translate({0, -m_selectedItem * static_cast<float>(m_itemHeight)});
            }

            // Draw the background of the item on which the mouse is standing
            if ((m_hoveringItem >= 0) && (m_hoveringItem != m_selectedItem) && m_backgroundColorHoverCached.isSet())
            {
                states.transform.translate({0, m_hoveringItem * static_cast<float>(m_itemHeight)});
                drawRectangleShape(target, states, {getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(), static_cast<float>(m_itemHeight)}, m_backgroundColorHoverCached);
                states.transform.translate({0, -m_hoveringItem * static_cast<float>(m_itemHeight)});
            }

            // Draw the items
            states.transform.translate({Text::getExtraHorizontalPadding(m_fontCached, m_textSize, m_textStyleCached), 0});
            for (std::size_t i = firstItem; i < lastItem; ++i)
                m_items[i].draw(target, states);
        }

        // Draw the scrollbar
        m_scroll->draw(target, statesForScrollbar);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


#include <TGUI/Widgets/MenuBar.hpp>
#include <TGUI/Container.hpp>
#include <TGUI/Clipping.hpp>
#include <SFML/Graphics/ConvexShape.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace
    {
        bool removeMenuImpl(const std::vector<sf::String>& hierarchy, bool removeParentsWhenEmpty, unsigned int parentIndex, std::vector<MenuBar::Menu>& menus)
        {
            for (auto it = menus.begin(); it != menus.end(); ++it)
            {
                if (it->text.getString() != hierarchy[parentIndex])
                    continue;

                if (parentIndex + 1 == hierarchy.size())
                {
                    menus.erase(it);
                    return true;
                }
                else
                {
                    // Return false if some menu in the hierarchy couldn't be found
                    if (!removeMenuImpl(hierarchy, removeParentsWhenEmpty, parentIndex + 1, it->menuItems))
                        return false;

                    // If parents don't have to be removed as well then we are done
                    if (!removeParentsWhenEmpty)
                        return true;

                    // Also delete the parent if empty
                    if (it->menuItems.empty())
                        menus.erase(it);

                    return true;
                }
            }

            // The hierarchy doesn't exist
            return false;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool removeSubMenusImpl(const std::vector<sf::String>& hierarchy, unsigned int parentIndex, std::vector<MenuBar::Menu>& menus)
        {
            for (auto it = menus.begin(); it != menus.end(); ++it)
            {
                if (it->text.getString() != hierarchy[parentIndex])
                    continue;

                if (parentIndex + 1 == hierarchy.size())
                {
                    it->menuItems.clear();
                    return true;
                }
                else
                    return removeSubMenusImpl(hierarchy, parentIndex + 1, it->menuItems);
            }

            // The hierarchy doesn't exist
            return false;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void setTextSizeImpl(std::vector<MenuBar::Menu>& menus, unsigned int textSize)
        {
            for (auto& menu : menus)
            {
                menu.text.setCharacterSize(textSize);
                if (!menu.menuItems.empty())
                    setTextSizeImpl(menu.menuItems, textSize);
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef TGUI_REMOVE_DEPRECATED_CODE
        std::vector<std::unique_ptr<MenuBar::GetAllMenusElement>> getAllMenusImpl(const std::vector<MenuBar::Menu>& menus)
        {
            std::vector<std::unique_ptr<MenuBar::GetAllMenusElement>> menuElements;

            for (const auto& menu : menus)
            {
                menuElements.emplace_back(std::make_unique<MenuBar::GetAllMenusElement>());
                menuElements.back()->text = menu.text.getString();
                menuElements.back()->enabled = menu.enabled;
                if (!menu.menuItems.empty())
                    menuElements.back()->menuItems = getAllMenusImpl(menu.menuItems);
            }

            return menuElements;
        }
#endif
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::vector<MenuBar::GetMenuListElement> getMenuListImpl(const std::vector<MenuBar::Menu>& menus)
        {
            std::vector<MenuBar::GetMenuListElement> menuElements;

            for (const auto& menu : menus)
            {
                TGUI_EMPLACE_BACK(element, menuElements)
                element.text = menu.text.getString();
                element.enabled = menu.enabled;
                if (!menu.menuItems.empty())
                    element.menuItems = getMenuListImpl(menu.menuItems);
            }

            return menuElements;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void saveMenus(std::unique_ptr<DataIO::Node>& parentNode, const std::vector<MenuBar::Menu>& menus)
        {
            for (const auto& menu : menus)
            {
                auto menuNode = std::make_unique<DataIO::Node>();
                menuNode->name = "Menu";

                menuNode->propertyValuePairs["Text"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(menu.text.getString()));
                if (!menu.enabled)
                    menuNode->propertyValuePairs["Enabled"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(menu.enabled));

                if (!menu.menuItems.empty())
                {
                    // Save as nested 'Menu' sections only when needed, use the more compact string list when just storing the menu items
                    bool recursionNeeded = false;
                    for (const auto& menuItem : menu.menuItems)
                    {
                        if (!menuItem.enabled || !menuItem.menuItems.empty())
                        {
                            recursionNeeded = true;
                            break;
                        }
                    }

                    if (recursionNeeded)
                        saveMenus(menuNode, menu.menuItems);
                    else
                    {
                        std::string itemList = "[" + Serializer::serialize(menu.menuItems[0].text.getString());
                        for (std::size_t i = 1; i < menu.menuItems.size(); ++i)
                            itemList += ", " + Serializer::serialize(menu.menuItems[i].text.getString());
                        itemList += "]";

                        menuNode->propertyValuePairs["Items"] = std::make_unique<DataIO::ValueNode>(itemList);
                    }
                }

                parentNode->children.push_back(std::move(menuNode));
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuBar::MenuBar()
    {
        m_type = "MenuBar";
        m_distanceToSideCached = Text::getLineHeight(m_fontCached, getGlobalTextSize()) * 0.4f;

        m_renderer = aurora::makeCopied<MenuBarRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setTextSize(getGlobalTextSize());
        setMinimumSubMenuWidth((Text::getLineHeight(m_fontCached, m_textSize) * 4) + (2 * m_distanceToSideCached));
        setSize({"100%", Text::getLineHeight(m_fontCached, m_textSize) * 1.25f});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuBar::Ptr MenuBar::create()
    {
        return std::make_shared<MenuBar>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuBar::Ptr MenuBar::copy(MenuBar::ConstPtr menuBar)
    {
        if (menuBar)
            return std::static_pointer_cast<MenuBar>(menuBar->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuBarRenderer* MenuBar::getSharedRenderer()
    {
        return aurora::downcast<MenuBarRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const MenuBarRenderer* MenuBar::getSharedRenderer() const
    {
        return aurora::downcast<const MenuBarRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuBarRenderer* MenuBar::getRenderer()
    {
        return aurora::downcast<MenuBarRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const MenuBarRenderer* MenuBar::getRenderer() const
    {
        return aurora::downcast<const MenuBarRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_spriteBackground.setSize(getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setEnabled(bool enabled)
    {
        if (m_enabled == enabled)
            return;

        Widget::setEnabled(enabled);

        if (!enabled)
            closeMenu();

        updateTextColors(m_menus, m_visibleMenu);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::addMenu(const sf::String& text)
    {
        createMenu(m_menus, text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::addMenuItem(const sf::String& text)
    {
        if (!m_menus.empty())
            return addMenuItem(m_menus.back().text.getString(), text);
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::addMenuItem(const sf::String& menu, const sf::String& text)
    {
        return addMenuItem({menu, text}, false);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::addMenuItem(const std::vector<sf::String>& hierarchy, bool createParents)
    {
        if (hierarchy.size() < 2)
            return false;

        auto* menu = findMenu(hierarchy, 0, m_menus, createParents);
        if (!menu)
            return false;

        createMenu(menu->menuItems, hierarchy.back());
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::removeAllMenus()
    {
        m_menus.clear();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::removeMenu(const sf::String& menu)
    {
        for (std::size_t i = 0; i < m_menus.size(); ++i)
        {
            if (m_menus[i].text.getString() != menu)
                continue;

            m_menus.erase(m_menus.begin() + i);

            // The menu was removed, so it can't remain open
            if (m_visibleMenu == static_cast<int>(i))
                m_visibleMenu = -1;

            return true;
        }

        // could not find the menu
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::removeMenuItem(const sf::String& menu, const sf::String& menuItem)
    {
        return removeMenuItem({menu, menuItem}, false);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::removeMenuItem(const std::vector<sf::String>& hierarchy, bool removeParentsWhenEmpty)
    {
        if (hierarchy.size() < 2)
            return false;

        return removeMenuImpl(hierarchy, removeParentsWhenEmpty, 0, m_menus);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::removeMenuItems(const sf::String& menu)
    {
        return removeSubMenuItems(std::vector<sf::String>{menu});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::removeSubMenuItems(const std::vector<sf::String>& hierarchy)
    {
        if (hierarchy.empty())
            return false;

        return removeSubMenusImpl(hierarchy, 0, m_menus);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::setMenuEnabled(const sf::String& menu, bool enabled)
    {
        for (std::size_t i = 0; i < m_menus.size(); ++i)
        {
            if (m_menus[i].text.getString() != menu)
                continue;

            if (!enabled && (m_visibleMenu == static_cast<int>(i)))
                closeMenu();

            m_menus[i].enabled = enabled;
            updateMenuTextColor(m_menus[i], (m_visibleMenu == static_cast<int>(i)));
            return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::getMenuEnabled(const sf::String& menuText) const
    {
        for (auto& menu : m_menus)
        {
            if (menu.text.getString() == menuText)
                return menu.enabled;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::setMenuItemEnabled(const sf::String& menuText, const sf::String& menuItemText, bool enabled)
    {
        return setMenuItemEnabled({menuText, menuItemText}, enabled);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::setMenuItemEnabled(const std::vector<sf::String>& hierarchy, bool enabled)
    {
        if (hierarchy.size() < 2)
            return false;

        auto* menu = findMenu(hierarchy, 0, m_menus, false);
        if (!menu)
            return false;

        for (unsigned int j = 0; j < menu->menuItems.size(); ++j)
        {
            auto& menuItem = menu->menuItems[j];
            if (menuItem.text.getString() != hierarchy.back())
                continue;

            if (!enabled && (menu->selectedMenuItem == static_cast<int>(j)))
                menu->selectedMenuItem = -1;

            menuItem.enabled = enabled;
            updateMenuTextColor(menuItem, (menu->selectedMenuItem == static_cast<int>(j)));
            return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::getMenuItemEnabled(const sf::String& menuText, const sf::String& menuItemText) const
    {
        return getMenuItemEnabled({menuText, menuItemText});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::getMenuItemEnabled(const std::vector<sf::String>& hierarchy) const
    {
        if (hierarchy.size() < 2)
            return false;

        auto* menuItem = findMenuItem(hierarchy);
        if (!menuItem)
            return false;

        return menuItem->enabled;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setTextSize(unsigned int size)
    {
        m_textSize = size;
        setTextSizeImpl(m_menus, size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setMinimumSubMenuWidth(float minimumWidth)
    {
        m_minimumSubMenuWidth = minimumWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float MenuBar::getMinimumSubMenuWidth() const
    {
        return m_minimumSubMenuWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setInvertedMenuDirection(bool invertDirection)
    {
        m_invertedMenuDirection = invertDirection;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::getInvertedMenuDirection() const
    {
        return m_invertedMenuDirection;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TGUI_REMOVE_DEPRECATED_CODE
    std::vector<std::pair<sf::String, std::vector<sf::String>>> MenuBar::getMenus() const
    {
        std::vector<std::pair<sf::String, std::vector<sf::String>>> menus;

        for (const auto& menu : m_menus)
        {
            std::vector<sf::String> items;
            for (const auto& item : menu.menuItems)
                items.push_back(item.text.getString());

            menus.emplace_back(menu.text.getString(), std::move(items));
        }

        return menus;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<std::unique_ptr<MenuBar::GetAllMenusElement>> MenuBar::getAllMenus() const
    {
        return getAllMenusImpl(m_menus);
    }
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<MenuBar::GetMenuListElement> MenuBar::getMenuList() const
    {
        return getMenuListImpl(m_menus);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::closeMenu()
    {
        if (m_visibleMenu != -1)
            closeSubMenus(m_menus, m_visibleMenu);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::mouseOnWidget(Vector2f pos) const
    {
        if (FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(pos))
            return true;
        else if (m_visibleMenu != -1)
        {
            Vector2f menuPos = getPosition();
            for (int i = 0; i < m_visibleMenu; ++i)
                menuPos.x += m_menus[i].text.getSize().x + (2 * m_distanceToSideCached);

            if (m_invertedMenuDirection)
                menuPos.y -= getSize().y * m_menus[m_visibleMenu].menuItems.size();
            else
                menuPos.y += getSize().y;

            // The menu is moved to the left if it otherwise falls off the screen
            bool openSubMenuToRight = true;
            const float menuWidth = calculateMenuWidth(m_menus[m_visibleMenu]);
            if (getParent() && (menuPos.x + menuWidth > getParent()->getInnerSize().x))
            {
                menuPos.x = std::max(0.f, getParent()->getInnerSize().x - menuWidth);
                openSubMenuToRight = false;
            }

            return isMouseOnTopOfMenu(menuPos, pos, openSubMenuToRight, m_menus[m_visibleMenu], menuWidth);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::leftMousePressed(Vector2f pos)
    {
        // Check if a menu should be opened or closed
        pos -= getPosition();
        if (FloatRect{0, 0, getSize().x, getSize().y}.contains(pos))
        {
            // Loop through the menus to check if the mouse is on top of them
            float menuWidth = 0;
            for (std::size_t i = 0; i < m_menus.size(); ++i)
            {
                menuWidth += m_menus[i].text.getSize().x + (2 * m_distanceToSideCached);
                if (pos.x >= menuWidth)
                    continue;

                // Close the menu when it was already open
                if (m_visibleMenu == static_cast<int>(i))
                    closeMenu();

                // If this menu can be opened then do so
                else if (m_menus[i].enabled && !m_menus[i].menuItems.empty())
                {
                    updateMenuTextColor(m_menus[i], true);
                    m_visibleMenu = static_cast<int>(i);
                }

                break;
            }
        }

        m_mouseDown = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::leftMouseReleased(Vector2f pos)
    {
        if (!m_mouseDown)
            return;

        pos -= getPosition();

        // Check if the mouse is on top of one of the menus
        if (FloatRect{0, 0, getSize().x, getSize().y}.contains(pos))
        {
            // Loop through the menus to check if the mouse is on top of them
            float menuWidth = 0;
            for (std::size_t i = 0; i < m_menus.size(); ++i)
            {
                menuWidth += m_menus[i].text.getSize().x + (2 * m_distanceToSideCached);
                if (pos.x >= menuWidth)
                    continue;

                // If a menu is clicked that has no menu items then also emit a signal
                if (m_menus[i].menuItems.empty())
                {
                    onMenuItemClick.emit(this, m_menus[i].text.getString(), std::vector<sf::String>(1, m_menus[i].text.getString()));
                    closeMenu();
                }

                break;
            }
        }

        if (m_visibleMenu == -1)
            return;

        auto* menu = &m_menus[m_visibleMenu];
        std::vector<sf::String> hierarchy;
        hierarchy.push_back(m_menus[m_visibleMenu].text.getString());
        while (menu->selectedMenuItem != -1)
        {
            auto& menuItem = menu->menuItems[menu->selectedMenuItem];
            hierarchy.push_back(menuItem.text.getString());
            if (menuItem.menuItems.empty())
            {
                onMenuItemClick.emit(this, menuItem.text.getString(), hierarchy);
                closeMenu();
                break;
            }

            menu = &menuItem;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::mouseMoved(Vector2f pos)
    {
        if (!m_mouseHover)
            mouseEnteredWidget();

        // Don't open a menu without having clicked first
        if (m_visibleMenu == -1)
            return;

        // Check if the mouse is on top of the menu bar (not on an open menus)
        if (FloatRect{getPosition(), getSize()}.contains(pos))
        {
            pos -= getPosition();

            // Loop through the menus to check if the mouse is on top of them
            bool handled = false;
            float menuWidth = 0;
            for (std::size_t i = 0; i < m_menus.size(); ++i)
            {
                menuWidth += m_menus[i].text.getSize().x + (2 * m_distanceToSideCached);
                if (pos.x >= menuWidth)
                    continue;

                // Check if the menu is already open
                if (m_visibleMenu == static_cast<int>(i))
                {
                    // If one of the menu items is selected then unselect it
                    if (m_menus[m_visibleMenu].selectedMenuItem != -1)
                    {
                        updateMenuTextColor(m_menus[i].menuItems[m_menus[m_visibleMenu].selectedMenuItem], false);
                        m_menus[m_visibleMenu].selectedMenuItem = -1;
                    }
                }
                else // The menu isn't open yet
                {
                    // If there is another menu open then close it first
                    closeMenu();

                    // If this menu can be opened then do so
                    if (m_menus[i].enabled && !m_menus[i].menuItems.empty())
                    {
                        updateMenuTextColor(m_menus[i], true);
                        m_visibleMenu = static_cast<int>(i);
                    }
                }

                handled = true;
                break;
            }

            // The mouse is to the right of all menus, deselect the selected item of the deepest submenu
            if (!handled)
                deselectBottomItem();
        }
        else // The mouse is on top of the open menu
        {
            Vector2f menuPos = getPosition();
            for (int i = 0; i < m_visibleMenu; ++i)
                menuPos.x += m_menus[i].text.getSize().x + (2 * m_distanceToSideCached);

            if (m_invertedMenuDirection)
                menuPos.y -= getSize().y * m_menus[m_visibleMenu].menuItems.size();
            else
                menuPos.y += getSize().y;

            // The menu is moved to the left if it otherwise falls off the screen
            bool openSubMenuToRight = true;
            const float menuWidth = calculateMenuWidth(m_menus[m_visibleMenu]);
            if (getParent() && (menuPos.x + menuWidth > getParent()->getInnerSize().x))
            {
                menuPos.x = std::max(0.f, getParent()->getInnerSize().x - menuWidth);
                openSubMenuToRight = false;
            }

            Menu* menuBelowMouse;
            int menuItemIndexBelowMouse;
            if (findMenuItemBelowMouse(menuPos, pos, openSubMenuToRight, m_menus[m_visibleMenu], menuWidth, &menuBelowMouse, &menuItemIndexBelowMouse))
            {
                // Check if the mouse is on a different item than before
                auto& menu = *menuBelowMouse;
                if (menuItemIndexBelowMouse != menu.selectedMenuItem)
                {
                    // If another of the menu items is selected then unselect it
                    if (menu.selectedMenuItem != -1)
                        closeSubMenus(menu.menuItems, menu.selectedMenuItem);

                    // Mark the item below the mouse as selected
                    if (menu.menuItems[menuItemIndexBelowMouse].enabled)
                    {
                        updateMenuTextColor(menu.menuItems[menuItemIndexBelowMouse], true);
                        menu.selectedMenuItem = menuItemIndexBelowMouse;
                    }
                }
                else // We already selected this item
                {
                    // If the selected item has a submenu then unselect its item
                    if (menu.menuItems[menuItemIndexBelowMouse].selectedMenuItem != -1)
                        closeSubMenus(menu.menuItems[menuItemIndexBelowMouse].menuItems, menu.menuItems[menuItemIndexBelowMouse].selectedMenuItem);
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::leftMouseButtonNoLongerDown()
    {
        // Close the open menu, but not when it just opened because of this mouse click
        if (!m_mouseDown)
            closeMenu();

        Widget::leftMouseButtonNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::mouseLeftWidget()
    {
        // Deselect the selected item of the deepest submenu
        if (m_visibleMenu != -1)
            deselectBottomItem();

        Widget::mouseLeftWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& MenuBar::getSignal(std::string signalName)
    {
        if (signalName == toLower(onMenuItemClick.getName()))
            return onMenuItemClick;
        else
            return Widget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::rendererChanged(const std::string& property)
    {
        if (property == "textcolor")
        {
            m_textColorCached = getSharedRenderer()->getTextColor();
            updateTextColors(m_menus, m_visibleMenu);
        }
        else if (property == "selectedtextcolor")
        {
            m_selectedTextColorCached = getSharedRenderer()->getSelectedTextColor();
            updateTextColors(m_menus, m_visibleMenu);
        }
        else if (property == "textcolordisabled")
        {
            m_textColorDisabledCached = getSharedRenderer()->getTextColorDisabled();
            updateTextColors(m_menus, m_visibleMenu);
        }
        else if (property == "texturebackground")
        {
            m_spriteBackground.setTexture(getSharedRenderer()->getTextureBackground());
        }
        else if (property == "textureitembackground")
        {
            m_spriteItemBackground.setTexture(getSharedRenderer()->getTextureItemBackground());
        }
        else if (property == "textureselecteditembackground")
        {
            m_spriteSelectedItemBackground.setTexture(getSharedRenderer()->getTextureSelectedItemBackground());
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == "selectedbackgroundcolor")
        {
            m_selectedBackgroundColorCached = getSharedRenderer()->getSelectedBackgroundColor();
        }
        else if (property == "distancetoside")
        {
            m_distanceToSideCached = getSharedRenderer()->getDistanceToSide();
        }
        else if ((property == "opacity") || (property == "opacitydisabled"))
        {
            Widget::rendererChanged(property);
            updateTextOpacity(m_menus);
            m_spriteBackground.setOpacity(m_opacityCached);
        }
        else if (property == "font")
        {
            Widget::rendererChanged(property);
            updateTextFont(m_menus);
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> MenuBar::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        saveMenus(node, m_menus);

        node->propertyValuePairs["TextSize"] = std::make_unique<DataIO::ValueNode>(to_string(m_textSize));
        node->propertyValuePairs["MinimumSubMenuWidth"] = std::make_unique<DataIO::ValueNode>(to_string(m_minimumSubMenuWidth));
        if (m_invertedMenuDirection)
            node->propertyValuePairs["InvertedMenuDirection"] = std::make_unique<DataIO::ValueNode>("true");

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs["textsize"])
            setTextSize(strToInt(node->propertyValuePairs["textsize"]->value));
        if (node->propertyValuePairs["minimumsubmenuwidth"])
            setMinimumSubMenuWidth(strToFloat(node->propertyValuePairs["minimumsubmenuwidth"]->value));
        if (node->propertyValuePairs["invertedmenudirection"])
            setInvertedMenuDirection(tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::Bool, node->propertyValuePairs["invertedmenudirection"]->value).getBool());

        loadMenus(node, m_menus);

        // Remove the 'menu' nodes as they have been processed
        node->children.erase(std::remove_if(node->children.begin(), node->children.end(),
            [](const std::unique_ptr<DataIO::Node>& child){ return toLower(child->name) == "menu"; }), node->children.end());

        // Update the text colors to properly display disabled menus
        updateTextColors(m_menus, m_visibleMenu);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Draw the background
        if (m_spriteBackground.isSet())
            m_spriteBackground.draw(target, states);
        else
            drawRectangleShape(target, states, getSize(), m_backgroundColorCached);

        if (m_menus.empty())
            return;

        Sprite backgroundSprite = m_spriteItemBackground;
        drawMenusOnBar(target, states, backgroundSprite);

        // Draw the menu if one is opened
        if (m_visibleMenu >= 0)
        {
            // Find the position of the menu
            float leftOffset = 0;
            for (int i = 0; i < m_visibleMenu; ++i)
                leftOffset += m_menus[i].text.getSize().x + (2 * m_distanceToSideCached);

            // Move the menu to the left if it otherwise falls off the screen
            bool openSubMenuToRight = true;
            const float menuWidth = calculateMenuWidth(m_menus[m_visibleMenu]);
            if (getParent() && (getPosition().x + leftOffset + menuWidth > getParent()->getInnerSize().x))
            {
                leftOffset = std::max(0.f, getParent()->getInnerSize().x - menuWidth);
                openSubMenuToRight = false;
            }

            if (m_invertedMenuDirection)
                states.transform.translate({leftOffset, -getSize().y * m_menus[m_visibleMenu].menuItems.size()});
            else
                states.transform.translate({leftOffset, getSize().y});

            drawMenu(target, states, m_menus[m_visibleMenu], menuWidth, backgroundSprite, getPosition().x + leftOffset, openSubMenuToRight);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::createMenu(std::vector<Menu>& menus, const sf::String& text)
    {
        Menu newMenu;
        newMenu.text.setFont(m_fontCached);
        newMenu.text.setColor(m_textColorCached);
        newMenu.text.setOpacity(m_opacityCached);
        newMenu.text.setCharacterSize(m_textSize);
        newMenu.text.setString(text);
        menus.push_back(std::move(newMenu));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuBar::Menu* MenuBar::findMenu(const std::vector<sf::String>& hierarchy, unsigned int parentIndex, std::vector<Menu>& menus, bool createParents)
    {
        for (auto& menu : menus)
        {
            if (menu.text.getString() != hierarchy[parentIndex])
                continue;

            if (parentIndex + 2 == hierarchy.size())
                return &menu;
            else
                return findMenu(hierarchy, parentIndex + 1, menu.menuItems, createParents);
        }

        if (createParents)
        {
            createMenu(menus, hierarchy[parentIndex]);
            if (parentIndex + 2 == hierarchy.size())
                return &menus.back();
            else
                return findMenu(hierarchy, parentIndex + 1, menus.back().menuItems, createParents);
        }

        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const MenuBar::Menu* MenuBar::findMenu(const std::vector<sf::String>& hierarchy, unsigned int parentIndex, const std::vector<Menu>& menus) const
    {
        for (auto& menu : menus)
        {
            if (menu.text.getString() != hierarchy[parentIndex])
                continue;

            if (parentIndex + 2 == hierarchy.size())
                return &menu;
            else
                return findMenu(hierarchy, parentIndex + 1, menu.menuItems);
        }

        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const MenuBar::Menu* MenuBar::findMenuItem(const std::vector<sf::String>& hierarchy) const
    {
        if (hierarchy.size() < 2)
            return nullptr;

        const auto* menu = findMenu(hierarchy, 0, m_menus);
        if (!menu)
            return nullptr;

        for (auto& menuItem : menu->menuItems)
        {
            if (menuItem.text.getString() != hierarchy.back())
                continue;

            return &menuItem;
        }

        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::loadMenus(const std::unique_ptr<DataIO::Node>& node, std::vector<Menu>& menus)
    {
        for (const auto& childNode : node->children)
        {
            if (toLower(childNode->name) != "menu")
                continue;

            // Every menu node should either have a Name (TGUI 0.8.0) or a Text (TGUI >= 0.8.1) property
        #ifdef TGUI_REMOVE_DEPRECATED_CODE
            if (!childNode->propertyValuePairs["text"])
                throw Exception{"Failed to parse 'Menu' property, expected a nested 'Text' propery"};

            const sf::String menuText = Deserializer::deserialize(ObjectConverter::Type::String, childNode->propertyValuePairs["text"]->value).getString();
            createMenu(menus, menuText);
        #else
            if (!childNode->propertyValuePairs["name"] && !childNode->propertyValuePairs["text"])
                throw Exception{"Failed to parse 'Menu' property, expected a nested 'Text' propery"};

            const sf::String menuText = Deserializer::deserialize(ObjectConverter::Type::String,
                childNode->propertyValuePairs[childNode->propertyValuePairs["text"] ? "text" : "name"]->value).getString();
            createMenu(menus, menuText);
        #endif

            if (childNode->propertyValuePairs["enabled"])
                menus.back().enabled = Deserializer::deserialize(ObjectConverter::Type::Bool, childNode->propertyValuePairs["enabled"]->value).getBool();

            // Recursively handle the menu nodes
            if (!childNode->children.empty())
                loadMenus(childNode, menus.back().menuItems);

            // Menu items can also be stored in an string array in the 'Items' property instead of as a nested Menu section
            if (childNode->propertyValuePairs["items"])
            {
                if (!childNode->propertyValuePairs["items"]->listNode)
                    throw Exception{"Failed to parse 'Items' property inside 'Menu' property, expected a list as value"};

            #ifndef TGUI_REMOVE_DEPRECATED_CODE
                // The ItemsEnabled property existed in TGUI 0.8.0 but was replaced with nested Menu sections in TGUI 0.8.1
                if (childNode->propertyValuePairs["itemsenabled"])
                {
                    if (!childNode->propertyValuePairs["itemsenabled"]->listNode)
                        throw Exception{"Failed to parse 'ItemsEnabled' property inside 'Menu' property, expected a list as value"};
                    if (childNode->propertyValuePairs["items"]->valueList.size() != childNode->propertyValuePairs["itemsenabled"]->valueList.size())
                        throw Exception{"Failed to parse 'ItemsEnabled' property inside 'Menu' property, length differs from 'Items' propery"};
                }
            #endif

                for (std::size_t i = 0; i < childNode->propertyValuePairs["items"]->valueList.size(); ++i)
                {
                    const sf::String menuItemText = Deserializer::deserialize(ObjectConverter::Type::String, childNode->propertyValuePairs["items"]->valueList[i]).getString();
                    createMenu(menus.back().menuItems, menuItemText);

                #ifndef TGUI_REMOVE_DEPRECATED_CODE
                    if (childNode->propertyValuePairs["itemsenabled"])
                        menus.back().menuItems.back().enabled = Deserializer::deserialize(ObjectConverter::Type::Bool, childNode->propertyValuePairs["itemsenabled"]->valueList[i]).getBool();
                #endif
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::closeSubMenus(std::vector<Menu>& menus, int& selectedMenu)
    {
        if (menus[selectedMenu].selectedMenuItem != -1)
            closeSubMenus(menus[selectedMenu].menuItems, menus[selectedMenu].selectedMenuItem);

        updateMenuTextColor(menus[selectedMenu], false);
        selectedMenu = -1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::deselectBottomItem()
    {
        auto* menu = &m_menus[m_visibleMenu];
        while (menu->selectedMenuItem != -1)
        {
            auto& menuItem = menu->menuItems[menu->selectedMenuItem];
            if (menuItem.menuItems.empty())
            {
                closeSubMenus(menu->menuItems, menu->selectedMenuItem);
                break;
            }

            menu = &menuItem;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::updateMenuTextColor(Menu& menu, bool selected)
    {
        if ((!m_enabled || !menu.enabled) && m_textColorDisabledCached.isSet())
            menu.text.setColor(m_textColorDisabledCached);
        else if (selected && m_selectedTextColorCached.isSet())
            menu.text.setColor(m_selectedTextColorCached);
        else
            menu.text.setColor(m_textColorCached);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::updateTextColors(std::vector<Menu>& menus, int selectedMenu)
    {
        for (std::size_t i = 0; i < menus.size(); ++i)
        {
            updateMenuTextColor(menus[i], (selectedMenu == static_cast<int>(i)));
            updateTextColors(menus[i].menuItems, menus[i].selectedMenuItem);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::updateTextOpacity(std::vector<Menu>& menus)
    {
        for (auto& menu : menus)
        {
            menu.text.setOpacity(m_opacityCached);
            updateTextOpacity(menu.menuItems);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::updateTextFont(std::vector<Menu>& menus)
    {
        for (auto& menu : menus)
        {
            menu.text.setFont(m_fontCached);
            updateTextFont(menu.menuItems);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float MenuBar::calculateMenuWidth(const Menu& menu) const
    {
        float maxWidth = m_minimumSubMenuWidth;
        for (const auto& item : menu.menuItems)
        {
            float width = item.text.getSize().x + (2.f * m_distanceToSideCached);

            // Reserve space for an arrow if there are submenus
            if (!item.menuItems.empty())
                width += (getSize().y / 4.f) + m_distanceToSideCached;

            if (width > maxWidth)
                maxWidth = width;
        }

        return maxWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f MenuBar::calculateSubmenuOffset(const Menu& menu, float globalLeftPos, float menuWidth, float subMenuWidth, bool& openSubMenuToRight) const
    {
        float leftOffset;
        if (openSubMenuToRight)
        {
            leftOffset = menuWidth;
            if (getParent() && (globalLeftPos + leftOffset + subMenuWidth > getParent()->getInnerSize().x))
            {
                if (globalLeftPos + leftOffset + subMenuWidth - getParent()->getInnerSize().x < globalLeftPos)
                {
                    leftOffset = -subMenuWidth;
                    openSubMenuToRight = false;
                }
            }
        }
        else // Submenu opens to the left side
        {
            leftOffset = -subMenuWidth;
            if (getParent() && (globalLeftPos < subMenuWidth))
            {
                if (getParent()->getInnerSize().x - menuWidth - globalLeftPos > globalLeftPos)
                {
                    leftOffset = menuWidth;
                    openSubMenuToRight = true;
                }
            }
        }

        float topOffset = getSize().y * menu.selectedMenuItem;
        if (m_invertedMenuDirection)
            topOffset -= getSize().y * (menu.menuItems[menu.selectedMenuItem].menuItems.size() - 1);

        return {leftOffset, topOffset};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::isMouseOnTopOfMenu(Vector2f menuPos, Vector2f mousePos, bool openSubMenuToRight, const Menu& menu, float menuWidth) const
    {
        // Check if the mouse is on top of the menu
        if (FloatRect{menuPos.x, menuPos.y, menuWidth, menu.menuItems.size() * getSize().y}.contains(mousePos))
            return true;

        // Check if the mouse is on one of the submenus
        if ((menu.selectedMenuItem >= 0) && !menu.menuItems[menu.selectedMenuItem].menuItems.empty())
        {
            const float subMenuWidth = calculateMenuWidth(menu.menuItems[menu.selectedMenuItem]);
            const Vector2f offset = calculateSubmenuOffset(menu, menuPos.x, menuWidth, subMenuWidth, openSubMenuToRight);
            if (isMouseOnTopOfMenu(menuPos + offset, mousePos, openSubMenuToRight, menu.menuItems[menu.selectedMenuItem], subMenuWidth))
                return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::findMenuItemBelowMouse(Vector2f menuPos, Vector2f mousePos, bool openSubMenuToRight, Menu& menu, float menuWidth, Menu** resultMenu, int* resultSelectedMenuItem)
    {
        // Loop over the open submenus and make sure to handle them first as menus can overlap
        if ((menu.selectedMenuItem >= 0) && !menu.menuItems[menu.selectedMenuItem].menuItems.empty())
        {
            const float subMenuWidth = calculateMenuWidth(menu.menuItems[menu.selectedMenuItem]);
            const Vector2f offset = calculateSubmenuOffset(menu, menuPos.x, menuWidth, subMenuWidth, openSubMenuToRight);
            if (findMenuItemBelowMouse(menuPos + offset, mousePos, openSubMenuToRight, menu.menuItems[menu.selectedMenuItem], subMenuWidth, resultMenu, resultSelectedMenuItem))
                return true;
        }

        // Check if the mouse is on top of the menu
        if (FloatRect{menuPos.x, menuPos.y, menuWidth, menu.menuItems.size() * getSize().y}.contains(mousePos))
        {
            *resultMenu = &menu;
            *resultSelectedMenuItem = static_cast<int>((mousePos.y - menuPos.y) / getSize().y);
            return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::drawMenusOnBar(sf::RenderTarget& target, sf::RenderStates states, Sprite& backgroundSprite) const
    {
        sf::Transform oldTransform = states.transform;

        // Draw the backgrounds
        for (std::size_t i = 0; i < m_menus.size(); ++i)
        {
            const bool isMenuOpen = (m_visibleMenu == static_cast<int>(i));
            const float width = m_menus[i].text.getSize().x + (2 * m_distanceToSideCached);
            if (backgroundSprite.isSet())
            {
                if (isMenuOpen && m_spriteSelectedItemBackground.isSet())
                {
                    Sprite selectedBackgroundSprite = m_spriteSelectedItemBackground;
                    selectedBackgroundSprite.setSize({width, getSize().y});
                    selectedBackgroundSprite.draw(target, states);
                }
                else // Not selected or no different texture for selected menu
                {
                    backgroundSprite.setSize({width, getSize().y});
                    backgroundSprite.draw(target, states);
                }
            }
            else // No textures where loaded
            {
                if (isMenuOpen && m_selectedBackgroundColorCached.isSet())
                    drawRectangleShape(target, states, {width, getSize().y}, m_selectedBackgroundColorCached);
            }

            states.transform.translate({width, 0});
        }

        states.transform = oldTransform;

        // Draw the texts
        const float textHeight = m_menus[0].text.getSize().y;
        states.transform.translate({m_distanceToSideCached, (getSize().y - textHeight) / 2.f});
        for (std::size_t i = 0; i < m_menus.size(); ++i)
        {
            m_menus[i].text.draw(target, states);

            const float width = m_menus[i].text.getSize().x + (2 * m_distanceToSideCached);
            states.transform.translate({width, 0});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::drawMenu(sf::RenderTarget& target, sf::RenderStates states, const Menu& menu, float menuWidth, Sprite& backgroundSprite, float globalLeftPos, bool openSubMenuToRight) const
    {
        if (menu.menuItems.empty())
            return;

        sf::Transform oldTransform = states.transform;

        // Draw the backgrounds
        if ((menu.selectedMenuItem == -1) && !backgroundSprite.isSet() && !m_selectedBackgroundColorCached.isSet())
        {
            drawRectangleShape(target, states, {menuWidth, getSize().y * menu.menuItems.size()}, m_backgroundColorCached);
        }
        else // We can't draw the entire menu with a singe draw call
        {
            for (std::size_t j = 0; j < menu.menuItems.size(); ++j)
            {
                const bool isMenuItemSelected = (menu.selectedMenuItem == static_cast<int>(j));
                if (backgroundSprite.isSet())
                {
                    if (isMenuItemSelected && m_spriteSelectedItemBackground.isSet())
                    {
                        Sprite selectedBackgroundSprite = m_spriteSelectedItemBackground;
                        selectedBackgroundSprite.setSize({menuWidth, getSize().y});
                        selectedBackgroundSprite.draw(target, states);
                    }
                    else // Not selected or no different texture for selected menu
                    {
                        backgroundSprite.setSize({menuWidth, getSize().y});
                        backgroundSprite.draw(target, states);
                    }
                }
                else // No textures where loaded
                {
                    if (isMenuItemSelected && m_selectedBackgroundColorCached.isSet())
                        drawRectangleShape(target, states, {menuWidth, getSize().y}, m_selectedBackgroundColorCached);
                    else
                        drawRectangleShape(target, states, {menuWidth, getSize().y}, m_backgroundColorCached);
                }

                states.transform.translate({0, getSize().y});
            }

            states.transform = oldTransform;
        }

        // Draw the texts (and arrows when there are submenus)
        states.transform.translate({m_distanceToSideCached, (getSize().y - menu.text.getSize().y) / 2.f});
        for (std::size_t j = 0; j < menu.menuItems.size(); ++j)
        {
            menu.menuItems[j].text.draw(target, states);

            // Draw an arrow next to the text if there is a submenu
            if (!menu.menuItems[j].menuItems.empty())
            {
                sf::Transform textTransform = states.transform;
                const float arrowHeight = getSize().y / 2.f;
                const float arrowWidth = arrowHeight / 2.f;
                states.transform.translate({menuWidth - 2*m_distanceToSideCached - arrowWidth, // 2x m_distanceToSideCached because we already translated once
                                            (menu.menuItems[j].text.getSize().y - arrowHeight) / 2.f});

                sf::ConvexShape arrow{3};
                arrow.setPoint(0, {0, 0});
                arrow.setPoint(1, {arrowWidth, arrowHeight / 2.f});
                arrow.setPoint(2, {0, arrowHeight});

                if ((!m_enabled || !menu.menuItems[j].enabled) && m_textColorDisabledCached.isSet())
                    arrow.setFillColor(Color::calcColorOpacity(m_textColorDisabledCached, m_opacityCached));
                else if ((menu.selectedMenuItem == static_cast<int>(j)) && m_selectedTextColorCached.isSet())
                    arrow.setFillColor(Color::calcColorOpacity(m_selectedTextColorCached, m_opacityCached));
                else
                    arrow.setFillColor(Color::calcColorOpacity(m_textColorCached, m_opacityCached));

                target.draw(arrow, states);
                states.transform = textTransform;
            }

            states.transform.translate({0, getSize().y});
        }

        // Draw the submenu if one is opened
        if ((menu.selectedMenuItem >= 0) && !menu.menuItems[menu.selectedMenuItem].menuItems.empty())
        {
            states.transform = oldTransform;

            // If there isn't enough room on the right side then open the menu to the left if it has more room
            const float subMenuWidth = calculateMenuWidth(menu.menuItems[menu.selectedMenuItem]);
            float leftOffset;
            if (openSubMenuToRight)
            {
                leftOffset = menuWidth;
                if (getParent() && (globalLeftPos + leftOffset + subMenuWidth > getParent()->getInnerSize().x))
                {
                    if (globalLeftPos + leftOffset + subMenuWidth - getParent()->getInnerSize().x < globalLeftPos)
                    {
                        leftOffset = -subMenuWidth;
                        openSubMenuToRight = false;
                    }
                }
            }
            else // Submenu opens to the left side
            {
                leftOffset = -subMenuWidth;
                if (getParent() && (globalLeftPos < subMenuWidth))
                {
                    if (getParent()->getInnerSize().x - menuWidth - globalLeftPos > globalLeftPos)
                    {
                        leftOffset = menuWidth;
                        openSubMenuToRight = true;
                    }
                }
            }

            float topOffset = getSize().y * menu.selectedMenuItem;
            if (m_invertedMenuDirection)
                topOffset -= getSize().y * (menu.menuItems[menu.selectedMenuItem].menuItems.size() - 1);

            states.transform.translate({leftOffset, topOffset});
            drawMenu(target, states, menu.menuItems[menu.selectedMenuItem], subMenuWidth, backgroundSprite, globalLeftPos + leftOffset, openSubMenuToRight);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


#include <TGUI/Widgets/Panel.hpp>
#include <TGUI/Clipping.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel::Panel(const Layout2d& size)
    {
        m_type = "Panel";
        m_isolatedFocus = true;

        m_renderer = aurora::makeCopied<PanelRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setSize(size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel::Ptr Panel::create(Layout2d size)
    {
        return std::make_shared<Panel>(size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel::Ptr Panel::copy(Panel::ConstPtr panel)
    {
        if (panel)
            return std::static_pointer_cast<Panel>(panel->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    PanelRenderer* Panel::getSharedRenderer()
    {
        return aurora::downcast<PanelRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const PanelRenderer* Panel::getSharedRenderer() const
    {
        return aurora::downcast<const PanelRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    PanelRenderer* Panel::getRenderer()
    {
        return aurora::downcast<PanelRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const PanelRenderer* Panel::getRenderer() const
    {
        return aurora::downcast<const PanelRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::setSize(const Layout2d& size)
    {
        m_bordersCached.updateParentSize(size.getValue());

        Group::setSize(size);

        m_spriteBackground.setSize({getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                                    getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Panel::getInnerSize() const
    {
        return {std::max(0.f, getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight() - m_paddingCached.getLeft() - m_paddingCached.getRight()),
                std::max(0.f, getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom() - m_paddingCached.getTop() - m_paddingCached.getBottom())};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Panel::getChildWidgetsOffset() const
    {
        return {m_paddingCached.getLeft() + m_bordersCached.getLeft(),
                m_paddingCached.getTop() + m_bordersCached.getTop()};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Panel::mouseOnWidget(Vector2f pos) const
    {
        return FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::leftMousePressed(Vector2f pos)
    {
        m_mouseDown = true;

        onMousePress.emit(this, pos - getPosition());

        Container::leftMousePressed(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::leftMouseReleased(Vector2f pos)
    {
        onMouseRelease.emit(this, pos - getPosition());

        if (m_mouseDown)
            onClick.emit(this, pos - getPosition());

        m_mouseDown = false;

        Container::leftMouseReleased(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::rightMousePressed(Vector2f pos)
    {
        m_rightMouseDown = true;
        onRightMousePress.emit(this, pos - getPosition());

        Container::rightMousePressed(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::rightMouseReleased(Vector2f pos)
    {
        onRightMouseRelease.emit(this, pos - getPosition());

        if (m_rightMouseDown)
            onRightClick.emit(this, pos - getPosition());

        m_rightMouseDown = false;

        Container::rightMouseReleased(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::rightMouseButtonNoLongerDown()
    {
        m_rightMouseDown = false;
        Container::rightMouseButtonNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& Panel::getSignal(std::string signalName)
    {
        if (signalName == toLower(onMousePress.getName()))
            return onMousePress;
        else if (signalName == toLower(onMouseRelease.getName()))
            return onMouseRelease;
        else if (signalName == toLower(onClick.getName()))
            return onClick;
        else if (signalName == toLower(onRightMousePress.getName()))
            return onRightMousePress;
        else if (signalName == toLower(onRightMouseRelease.getName()))
            return onRightMouseRelease;
        else if (signalName == toLower(onRightClick.getName()))
            return onRightClick;
        else
            return Group::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            setSize(m_size);
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == "texturebackground")
        {
            m_spriteBackground.setTexture(getSharedRenderer()->getTextureBackground());
        }
        else if ((property == "opacity") || (property == "opacitydisabled"))
        {
            Container::rendererChanged(property);
            m_spriteBackground.setOpacity(m_opacityCached);
        }
        else
            Group::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            drawBorders(target, states, m_bordersCached, getSize(), m_borderColorCached);
            states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop()});
        }

        // Draw the background
        const Vector2f innerSize = {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                                    getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()};
        if (m_spriteBackground.isSet())
            m_spriteBackground.draw(target, states);
        else
            drawRectangleShape(target, states, innerSize, m_backgroundColorCached);

        states.transform.translate(m_paddingCached.getLeft(), m_paddingCached.getTop());
        const Vector2f contentSize = {innerSize.x - m_paddingCached.getLeft() - m_paddingCached.getRight(),
                                      innerSize.y - m_paddingCached.getTop() - m_paddingCached.getBottom()};

        // Draw the child widgets
        const Clipping clipping{target, states, {}, contentSize};
        drawWidgetContainer(&target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


#include <TGUI/Widgets/ProgressBar.hpp>
#include <TGUI/Clipping.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ProgressBar::ProgressBar()
    {
        m_type = "ProgressBar";
        m_textBack.setFont(m_fontCached);
        m_textFront.setFont(m_fontCached);

        m_renderer = aurora::makeCopied<ProgressBarRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setTextSize(getGlobalTextSize());
        setSize({m_textBack.getLineHeight() * 15,
                 m_textBack.getLineHeight() * 1.25f + m_bordersCached.getTop() + m_bordersCached.getBottom()});

    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ProgressBar::Ptr ProgressBar::create()
    {
        return std::make_shared<ProgressBar>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ProgressBar::Ptr ProgressBar::copy(ProgressBar::ConstPtr progressBar)
    {
        if (progressBar)
            return std::static_pointer_cast<ProgressBar>(progressBar->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ProgressBarRenderer* ProgressBar::getSharedRenderer()
    {
        return aurora::downcast<ProgressBarRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ProgressBarRenderer* ProgressBar::getSharedRenderer() const
    {
        return aurora::downcast<const ProgressBarRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ProgressBarRenderer* ProgressBar::getRenderer()
    {
        return aurora::downcast<ProgressBarRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ProgressBarRenderer* ProgressBar::getRenderer() const
    {
        return aurora::downcast<const ProgressBarRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_bordersCached.updateParentSize(getSize());

        m_spriteBackground.setSize(getInnerSize());

        // Recalculate the size of the front image
        recalculateFillSize();

        // Recalculate the text size
        setText(getText());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::setMinimum(unsigned int minimum)
    {
        // Set the new minimum
        m_minimum = minimum;

        // The minimum can never be greater than the maximum
        if (m_minimum > m_maximum)
            m_maximum = m_minimum;

        // When the value is below the minimum then adjust it
        if (m_value < m_minimum)
            m_value = m_minimum;

        // Recalculate the size of the front image (the size of the part that will be drawn)
        recalculateFillSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ProgressBar::getMinimum() const
    {
        return m_minimum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::setMaximum(unsigned int maximum)
    {
        // Set the new maximum
        m_maximum = maximum;

        // The maximum can never be below the minimum
        if (m_maximum < m_minimum)
            m_minimum = m_maximum;

        // When the value is above the maximum then adjust it
        if (m_value > m_maximum)
            m_value = m_maximum;

        // Recalculate the size of the front image (the size of the part that will be drawn)
        recalculateFillSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ProgressBar::getMaximum() const
    {
        return m_maximum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::setValue(unsigned int value)
    {
        // When the value is below the minimum or above the maximum then adjust it
        if (value < m_minimum)
            value = m_minimum;
        else if (value > m_maximum)
            value = m_maximum;

        if (m_value != value)
        {
            m_value = value;

            onValueChange.emit(this, m_value);

            if (m_value == m_maximum)
                onFull.emit(this);

            // Recalculate the size of the front image (the size of the part that will be drawn)
            recalculateFillSize();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ProgressBar::getValue() const
    {
        return m_value;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ProgressBar::incrementValue()
    {
        // When the value is still below the maximum then adjust it
        if (m_value < m_maximum)
            setValue(m_value + 1);

        // return the new value
        return m_value;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::setText(const sf::String& text)
    {
        // Set the new text
        m_textBack.setString(text);
        m_textFront.setString(text);

        // Check if the text is auto sized
        if (m_textSize == 0)
        {
            unsigned int textSize;
            if (m_spriteFill.isSet())
                textSize = Text::findBestTextSize(m_fontCached, m_spriteFill.getSize().y * 0.8f);
            else
                textSize = Text::findBestTextSize(m_fontCached, getInnerSize().y * 0.8f);

            m_textBack.setCharacterSize(textSize);

            // Make the text smaller when it's too width
            if (m_textBack.getSize().x > (getInnerSize().x * 0.85f))
                m_textBack.setCharacterSize(static_cast<unsigned int>(textSize * ((getInnerSize().x * 0.85f) / m_textBack.getSize().x)));
        }
        else // When the text has a fixed size
        {
            // Set the text size
            m_textBack.setCharacterSize(m_textSize);
        }

        m_textFront.setCharacterSize(m_textBack.getCharacterSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::String& ProgressBar::getText() const
    {
        return m_textBack.getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::setTextSize(unsigned int size)
    {
        // Change the text size
        m_textSize = size;
        setText(getText());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ProgressBar::getTextSize() const
    {
        return m_textBack.getCharacterSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::setFillDirection(FillDirection direction)
    {
        m_fillDirection = direction;
        recalculateFillSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ProgressBar::FillDirection ProgressBar::getFillDirection() const
    {
        return m_fillDirection;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ProgressBar::mouseOnWidget(Vector2f pos) const
    {
        if (ClickableWidget::mouseOnWidget(pos))
        {
            if (!m_transparentTextureCached || !m_spriteBackground.isTransparentPixel(pos - getPosition() - m_bordersCached.getOffset()))
                return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& ProgressBar::getSignal(std::string signalName)
    {
        if (signalName == toLower(onValueChange.getName()))
            return onValueChange;
        else if (signalName == toLower(onFull.getName()))
            return onFull;
        else
            return ClickableWidget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            setSize(m_size);
        }
        else if ((property == "textcolor") || (property == "textcolorfilled"))
        {
            m_textBack.setColor(getSharedRenderer()->getTextColor());

            if (getSharedRenderer()->getTextColorFilled().isSet())
                m_textFront.setColor(getSharedRenderer()->getTextColorFilled());
            else
                m_textFront.setColor(getSharedRenderer()->getTextColor());
        }
        else if (property == "texturebackground")
        {
            m_spriteBackground.setTexture(getSharedRenderer()->getTextureBackground());
        }
        else if (property == "texturefill")
        {
            m_spriteFill.setTexture(getSharedRenderer()->getTextureFill());
            recalculateFillSize();
        }
        else if (property == "textstyle")
        {
            m_textBack.setStyle(getSharedRenderer()->getTextStyle());
            m_textFront.setStyle(getSharedRenderer()->getTextStyle());
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == "fillcolor")
        {
            m_fillColorCached = getSharedRenderer()->getFillColor();
        }
        else if ((property == "opacity") || (property == "opacitydisabled"))
        {
            Widget::rendererChanged(property);

            m_spriteBackground.setOpacity(m_opacityCached);
            m_spriteFill.setOpacity(m_opacityCached);

            m_textBack.setOpacity(m_opacityCached);
            m_textFront.setOpacity(m_opacityCached);
        }
        else if (property == "font")
        {
            Widget::rendererChanged(property);

            m_textBack.setFont(m_fontCached);
            m_textFront.setFont(m_fontCached);
            setText(getText());
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> ProgressBar::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        if (!getText().isEmpty())
            node->propertyValuePairs["Text"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(getText()));

        if (m_fillDirection != ProgressBar::FillDirection::LeftToRight)
        {
            if (m_fillDirection == ProgressBar::FillDirection::RightToLeft)
                node->propertyValuePairs["FillDirection"] = std::make_unique<DataIO::ValueNode>("RightToLeft");
            else if (m_fillDirection == ProgressBar::FillDirection::TopToBottom)
                node->propertyValuePairs["FillDirection"] = std::make_unique<DataIO::ValueNode>("TopToBottom");
            else if (m_fillDirection == ProgressBar::FillDirection::BottomToTop)
                node->propertyValuePairs["FillDirection"] = std::make_unique<DataIO::ValueNode>("BottomToTop");
        }

        node->propertyValuePairs["Minimum"] = std::make_unique<DataIO::ValueNode>(to_string(m_minimum));
        node->propertyValuePairs["Maximum"] = std::make_unique<DataIO::ValueNode>(to_string(m_maximum));
        node->propertyValuePairs["Value"] = std::make_unique<DataIO::ValueNode>(to_string(m_value));
        node->propertyValuePairs["TextSize"] = std::make_unique<DataIO::ValueNode>(to_string(m_textSize));

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs["minimum"])
            setMinimum(strToInt(node->propertyValuePairs["minimum"]->value));
        if (node->propertyValuePairs["maximum"])
            setMaximum(strToInt(node->propertyValuePairs["maximum"]->value));
        if (node->propertyValuePairs["value"])
            setValue(strToInt(node->propertyValuePairs["value"]->value));
        if (node->propertyValuePairs["text"])
            setText(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["text"]->value).getString());
        if (node->propertyValuePairs["textsize"])
            setTextSize(strToInt(node->propertyValuePairs["textsize"]->value));

        if (node->propertyValuePairs["filldirection"])
        {
            std::string requestedStyle = toLower(trim(node->propertyValuePairs["filldirection"]->value));
            if (requestedStyle == "lefttoright")
                setFillDirection(ProgressBar::FillDirection::LeftToRight);
            else if (requestedStyle == "righttoleft")
                setFillDirection(ProgressBar::FillDirection::RightToLeft);
            else if (requestedStyle == "toptobottom")
                setFillDirection(ProgressBar::FillDirection::TopToBottom);
            else if (requestedStyle == "bottomtotop")
                setFillDirection(ProgressBar::FillDirection::BottomToTop);
            else
                throw Exception{"Failed to parse FillDirection property, found unknown value."};
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ProgressBar::getInnerSize() const
    {
        return {std::max(0.f, getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight()),
                std::max(0.f, getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom())};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::recalculateFillSize()
    {
        Vector2f size;
        if (m_spriteFill.isSet())
        {
            const Vector2f frontSize = getFrontImageSize();
            m_spriteFill.setSize(frontSize);
            size = frontSize;
        }
        else
            size = getInnerSize();

        switch (getFillDirection())
        {
            case FillDirection::LeftToRight:
                m_frontRect =  {0, 0, size.x * ((m_value - m_minimum) / static_cast<float>(m_maximum - m_minimum)), size.y};
                m_backRect = {m_frontRect.width, 0, size.x - m_frontRect.width, size.y};
                break;

            case FillDirection::RightToLeft:
                m_frontRect =  {0, 0, size.x * ((m_value - m_minimum) / static_cast<float>(m_maximum - m_minimum)), size.y};
                m_frontRect.left = size.x - m_frontRect.width;
                m_backRect = {0, 0, size.x - m_frontRect.width, size.y};
                break;

            case FillDirection::TopToBottom:
                m_frontRect =  {0, 0, size.x, size.y * ((m_value - m_minimum) / static_cast<float>(m_maximum - m_minimum))};
                m_backRect = {0, m_frontRect.height, size.x, size.y - m_frontRect.height};
                break;

            case FillDirection::BottomToTop:
                m_frontRect =  {0, 0, size.x, size.y * ((m_value - m_minimum) / static_cast<float>(m_maximum - m_minimum))};
                m_frontRect.top = size.y - m_frontRect.height;
                m_backRect = {0, 0, size.x, size.y - m_frontRect.height};
                break;
        }

        if (m_spriteFill.isSet())
            m_spriteFill.setVisibleRect(m_frontRect);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ProgressBar::getFrontImageSize() const
    {
        if (m_spriteBackground.isSet())
        {
            switch (m_spriteBackground.getScalingType())
            {
            case Sprite::ScalingType::Normal:
                return {m_spriteFill.getTexture().getImageSize().x * getInnerSize().x / m_spriteBackground.getTexture().getImageSize().x,
                        m_spriteFill.getTexture().getImageSize().y * getInnerSize().y / m_spriteBackground.getTexture().getImageSize().y};

            case Sprite::ScalingType::Horizontal:
                return {getInnerSize().x - ((m_spriteBackground.getTexture().getImageSize().x - m_spriteFill.getTexture().getImageSize().x) * (getInnerSize().y / m_spriteBackground.getTexture().getImageSize().y)),
                        m_spriteFill.getTexture().getImageSize().y * getInnerSize().y / m_spriteBackground.getTexture().getImageSize().y};

            case Sprite::ScalingType::Vertical:
                return {m_spriteFill.getTexture().getImageSize().x * getInnerSize().x / m_spriteBackground.getTexture().getImageSize().x,
                        getInnerSize().y - ((m_spriteBackground.getTexture().getImageSize().y - m_spriteFill.getTexture().getImageSize().y) * (getInnerSize().x / m_spriteBackground.getTexture().getImageSize().x))};

            case Sprite::ScalingType::NineSlice:
                return {getInnerSize().x - (m_spriteBackground.getTexture().getImageSize().x - m_spriteFill.getTexture().getImageSize().x),
                        getInnerSize().y - (m_spriteBackground.getTexture().getImageSize().y - m_spriteFill.getTexture().getImageSize().y)};
            }
        }

        return getInnerSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            drawBorders(target, states, m_bordersCached, getSize(), m_borderColorCached);
            states.transform.translate(m_bordersCached.getOffset());
        }

        // Draw the background
        if (m_spriteBackground.isSet())
            m_spriteBackground.draw(target, states);
        else
        {
            Vector2f positionOffset = {m_backRect.left, m_backRect.top};

            states.transform.translate(positionOffset);
            drawRectangleShape(target, states, {m_backRect.width, m_backRect.height}, m_backgroundColorCached);
            states.transform.translate(-positionOffset);
        }

        // Draw the filled area
        Vector2f imageShift;
        if (m_spriteFill.isSet())
        {
            if (m_spriteBackground.isSet() && (m_spriteBackground.getSize() != m_spriteFill.getSize()))
            {
                imageShift = (m_spriteBackground.getSize() - m_spriteFill.getSize()) / 2.f;

                states.transform.translate(imageShift);
                m_spriteFill.draw(target, states);
                states.transform.translate(-imageShift);
            }
            else
                m_spriteFill.draw(target, states);
        }
        else // Using colors instead of a texture
        {
            Vector2f positionOffset = {m_frontRect.left, m_frontRect.top};

            states.transform.translate(positionOffset);
            drawRectangleShape(target, states, {m_frontRect.width, m_frontRect.height}, m_fillColorCached);
            states.transform.translate(-positionOffset);
        }

        // Draw the text
        if (m_textBack.getString() != "")
        {
            Vector2f textTranslation = (getInnerSize() - m_textBack.getSize()) / 2.f;

            if (m_textBack.getColor() == m_textFront.getColor())
            {
                states.transform.translate(textTranslation);
                m_textBack.draw(target, states);
                states.transform.translate(-textTranslation);
            }
            else
            {
                // Draw the text on top of the unfilled part
                {
                    Clipping clipping{target, states, imageShift + Vector2f{m_backRect.left, m_backRect.top}, {m_backRect.width, m_backRect.height}};

                    states.transform.translate(textTranslation);
                    m_textBack.draw(target, states);
                    states.transform.translate(-textTranslation);
                }

                // Draw the text on top of the filled part
                {
                    Clipping clipping{target, states, imageShift + Vector2f{m_frontRect.left, m_frontRect.top}, {m_frontRect.width, m_frontRect.height}};

                    states.transform.translate(textTranslation);
                    m_textFront.draw(target, states);
                    states.transform.translate(-textTranslation);
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


#include <TGUI/Widgets/RadioButtonGroup.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButtonGroup::RadioButtonGroup()
    {
        m_type = "RadioButtonGroup";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButtonGroup::Ptr RadioButtonGroup::create()
    {
        return std::make_shared<RadioButtonGroup>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButtonGroup::Ptr RadioButtonGroup::copy(RadioButtonGroup::ConstPtr group)
    {
        if (group)
            return std::static_pointer_cast<RadioButtonGroup>(group->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonGroup::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());
        drawWidgetContainer(&target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


#include <TGUI/Widgets/ScrollablePanel.hpp>
#include <TGUI/Vector2f.hpp>
#include <TGUI/Clipping.hpp>
#include <TGUI/SignalImpl.hpp>

#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollablePanel::ScrollablePanel(const Layout2d& size, Vector2f contentSize) :
        Panel{size}
    {
        m_type = "ScrollablePanel";

        // Rotate the horizontal scrollbar
        m_horizontalScrollbar->setSize(m_horizontalScrollbar->getSize().y, m_horizontalScrollbar->getSize().x);

        m_renderer = aurora::makeCopied<ScrollablePanelRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setContentSize(contentSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollablePanel::ScrollablePanel(const ScrollablePanel& other) :
        Panel                      {other},
        m_contentSize              {other.m_contentSize},
        m_mostBottomRightPosition  {other.m_mostBottomRightPosition},
        m_verticalScrollbar        {other.m_verticalScrollbar},
        m_horizontalScrollbar      {other.m_horizontalScrollbar},
        m_verticalScrollbarPolicy  {other.m_verticalScrollbarPolicy},
        m_horizontalScrollbarPolicy{other.m_horizontalScrollbarPolicy},
        m_connectedCallbacks       {}
    {
        if (m_contentSize == Vector2f{0, 0})
        {
            for (auto& widget : m_widgets)
                connectPositionAndSize(widget);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollablePanel::ScrollablePanel(ScrollablePanel&& other) :
        Panel                      {std::move(other)},
        m_contentSize              {std::move(other.m_contentSize)},
        m_mostBottomRightPosition  {std::move(other.m_mostBottomRightPosition)},
        m_verticalScrollbar        {std::move(other.m_verticalScrollbar)},
        m_horizontalScrollbar      {std::move(other.m_horizontalScrollbar)},
        m_verticalScrollbarPolicy  {std::move(other.m_verticalScrollbarPolicy)},
        m_horizontalScrollbarPolicy{std::move(other.m_horizontalScrollbarPolicy)},
        m_connectedCallbacks       {std::move(other.m_connectedCallbacks)}
    {
        disconnectAllChildWidgets();

        if (m_contentSize == Vector2f{0, 0})
        {
            for (auto& widget : m_widgets)
                connectPositionAndSize(widget);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollablePanel& ScrollablePanel::operator= (const ScrollablePanel& other)
    {
        if (this != &other)
        {
            Panel::operator=(other);
            m_contentSize               = other.m_contentSize;
            m_mostBottomRightPosition   = other.m_mostBottomRightPosition;
            m_verticalScrollbar         = other.m_verticalScrollbar;
            m_horizontalScrollbar       = other.m_horizontalScrollbar;
            m_verticalScrollbarPolicy   = other.m_verticalScrollbarPolicy;
            m_horizontalScrollbarPolicy = other.m_horizontalScrollbarPolicy;

            disconnectAllChildWidgets();

            if (m_contentSize == Vector2f{0, 0})
            {
                for (auto& widget : m_widgets)
                    connectPositionAndSize(widget);
            }
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollablePanel& ScrollablePanel::operator= (ScrollablePanel&& other)
    {
        if (this != &other)
        {
            Panel::operator=(std::move(other));
            m_contentSize               = std::move(other.m_contentSize);
            m_mostBottomRightPosition   = std::move(other.m_mostBottomRightPosition);
            m_verticalScrollbar         = std::move(other.m_verticalScrollbar);
            m_horizontalScrollbar       = std::move(other.m_horizontalScrollbar);
            m_verticalScrollbarPolicy   = std::move(other.m_verticalScrollbarPolicy);
            m_horizontalScrollbarPolicy = std::move(other.m_horizontalScrollbarPolicy);

            disconnectAllChildWidgets();

            if (m_contentSize == Vector2f{0, 0})
            {
                for (auto& widget : m_widgets)
                    connectPositionAndSize(widget);
            }
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollablePanel::Ptr ScrollablePanel::create(Layout2d size, Vector2f contentSize)
    {
        return std::make_shared<ScrollablePanel>(size, contentSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollablePanel::Ptr ScrollablePanel::copy(ScrollablePanel::ConstPtr panel)
    {
        if (panel)
            return std::static_pointer_cast<ScrollablePanel>(panel->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollablePanelRenderer* ScrollablePanel::getSharedRenderer()
    {
        return aurora::downcast<ScrollablePanelRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ScrollablePanelRenderer* ScrollablePanel::getSharedRenderer() const
    {
        return aurora::downcast<const ScrollablePanelRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollablePanelRenderer* ScrollablePanel::getRenderer()
    {
        return aurora::downcast<ScrollablePanelRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ScrollablePanelRenderer* ScrollablePanel::getRenderer() const
    {
        return aurora::downcast<const ScrollablePanelRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::setSize(const Layout2d& size)
    {
        Panel::setSize(size);
        updateScrollbars();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ScrollablePanel::getAbsolutePosition() const
    {
        return Panel::getAbsolutePosition() - getContentOffset();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::add(const Widget::Ptr& widget, const sf::String& widgetName)
    {
        Panel::add(widget, widgetName);

        if (m_contentSize == Vector2f{0, 0})
        {
            const Vector2f bottomRight = widget->getPosition() + widget->getFullSize();
            if (bottomRight.x > m_mostBottomRightPosition.x)
                m_mostBottomRightPosition.x = bottomRight.x;
            if (bottomRight.y > m_mostBottomRightPosition.y)
                m_mostBottomRightPosition.y = bottomRight.y;

            updateScrollbars();

            connectPositionAndSize(widget);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ScrollablePanel::remove(const Widget::Ptr& widget)
    {
        const auto callbackIt = m_connectedCallbacks.find(widget);
        if (callbackIt != m_connectedCallbacks.end())
        {
            widget->disconnect(callbackIt->second-1);
            widget->disconnect(callbackIt->second);
            m_connectedCallbacks.erase(callbackIt);
        }

        const bool ret = Panel::remove(widget);

        if (m_contentSize == Vector2f{0, 0})
        {
            const Vector2f bottomRight = widget->getPosition() + widget->getFullSize();
            if ((bottomRight.x == m_mostBottomRightPosition.x) || (bottomRight.y == m_mostBottomRightPosition.y))
            {
                recalculateMostBottomRightPosition();
                updateScrollbars();
            }
        }

        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::removeAllWidgets()
    {
        disconnectAllChildWidgets();

        Panel::removeAllWidgets();

        if (m_contentSize == Vector2f{0, 0})
        {
            recalculateMostBottomRightPosition();
            updateScrollbars();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::setContentSize(Vector2f size)
    {
        m_contentSize = size;

        disconnectAllChildWidgets();

        if (m_contentSize == Vector2f{0, 0})
        {
            recalculateMostBottomRightPosition();

            // Automatically recalculate the bottom right position when the position or size of a widget changes
            for (auto& widget : m_widgets)
                connectPositionAndSize(widget);
        }

        updateScrollbars();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ScrollablePanel::getContentSize() const
    {
        if (m_contentSize != Vector2f{0, 0})
            return m_contentSize;
        else if (m_widgets.empty())
            return getInnerSize();
        else
            return m_mostBottomRightPosition;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ScrollablePanel::getContentOffset() const
    {
        return {static_cast<float>(m_horizontalScrollbar->getValue()), static_cast<float>(m_verticalScrollbar->getValue())};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef TGUI_REMOVE_DEPRECATED_CODE
    void ScrollablePanel::setScrollbarWidth(float width)
    {
        m_verticalScrollbar->setSize({width, m_verticalScrollbar->getSize().y});
        m_horizontalScrollbar->setSize({m_horizontalScrollbar->getSize().x, width});
        updateScrollbars();
    }
#endif
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float ScrollablePanel::getScrollbarWidth() const
    {
        return m_verticalScrollbar->getSize().x;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::setVerticalScrollbarPolicy(Scrollbar::Policy policy)
    {
        m_verticalScrollbarPolicy = policy;

        if (policy == Scrollbar::Policy::Always)
        {
            m_verticalScrollbar->setVisible(true);
            m_verticalScrollbar->setAutoHide(false);
        }
        else if (policy == Scrollbar::Policy::Never)
        {
            m_verticalScrollbar->setVisible(false);
        }
        else // Scrollbar::Policy::Automatic
        {
            m_verticalScrollbar->setVisible(true);
            m_verticalScrollbar->setAutoHide(true);
        }

        updateScrollbars();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Scrollbar::Policy ScrollablePanel::getVerticalScrollbarPolicy() const
    {
        return m_verticalScrollbarPolicy;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::setHorizontalScrollbarPolicy(Scrollbar::Policy policy)
    {
        m_horizontalScrollbarPolicy = policy;

        if (policy == Scrollbar::Policy::Always)
        {
            m_horizontalScrollbar->setVisible(true);
            m_horizontalScrollbar->setAutoHide(false);
        }
        else if (policy == Scrollbar::Policy::Never)
        {
            m_horizontalScrollbar->setVisible(false);
        }
        else // Scrollbar::Policy::Automatic
        {
            m_horizontalScrollbar->setVisible(true);
            m_horizontalScrollbar->setAutoHide(true);
        }

        updateScrollbars();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Scrollbar::Policy ScrollablePanel::getHorizontalScrollbarPolicy() const
    {
        return m_horizontalScrollbarPolicy;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::setVerticalScrollAmount(unsigned int scrollAmount)
    {
        m_verticalScrollAmount = scrollAmount;

        if (scrollAmount == 0)
        {
            const float verticalSpeed = 40.f * (static_cast<float>(m_verticalScrollbar->getMaximum() - m_verticalScrollbar->getViewportSize()) / m_verticalScrollbar->getViewportSize());
            m_verticalScrollbar->setScrollAmount(static_cast<unsigned int>(std::ceil(std::sqrt(verticalSpeed))));
        }
        else
            m_verticalScrollbar->setScrollAmount(scrollAmount);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ScrollablePanel::getVerticalScrollAmount() const
    {
        return m_verticalScrollAmount;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::setHorizontalScrollAmount(unsigned int scrollAmount)
    {
        m_horizontalScrollAmount = scrollAmount;

        if (scrollAmount == 0)
        {
            const float horizontalSpeed = 40.f * (static_cast<float>(m_horizontalScrollbar->getMaximum() - m_horizontalScrollbar->getViewportSize()) / m_horizontalScrollbar->getViewportSize());
            m_horizontalScrollbar->setScrollAmount(static_cast<unsigned int>(std::ceil(std::sqrt(horizontalSpeed))));
        }
        else
            m_horizontalScrollbar->setScrollAmount(scrollAmount);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ScrollablePanel::getHorizontalScrollAmount() const
    {
        return m_horizontalScrollAmount;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::setVerticalScrollbarValue(unsigned int value)
    {
        m_verticalScrollbar->setValue(value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ScrollablePanel::getVerticalScrollbarValue() const
    {
        return m_verticalScrollbar->getValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::setHorizontalScrollbarValue(unsigned int value)
    {
        m_horizontalScrollbar->setValue(value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ScrollablePanel::getHorizontalScrollbarValue() const
    {
        return m_horizontalScrollbar->getValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::leftMousePressed(Vector2f pos)
    {
        m_mouseDown = true;

        if (m_verticalScrollbar->mouseOnWidget(pos - getPosition()))
            m_verticalScrollbar->leftMousePressed(pos - getPosition());
        else if (m_horizontalScrollbar->mouseOnWidget(pos - getPosition()))
            m_horizontalScrollbar->leftMousePressed(pos - getPosition());
        else if (FloatRect{getPosition().x + getChildWidgetsOffset().x, getPosition().y + getChildWidgetsOffset().y, getInnerSize().x, getInnerSize().y}.contains(pos))
        {
            Panel::leftMousePressed({pos.x + static_cast<float>(m_horizontalScrollbar->getValue()),
                                     pos.y + static_cast<float>(m_verticalScrollbar->getValue())});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::leftMouseReleased(Vector2f pos)
    {
        if (m_verticalScrollbar->mouseOnWidget(pos - getPosition()))
            m_verticalScrollbar->leftMouseReleased(pos - getPosition());
        else if (m_horizontalScrollbar->mouseOnWidget(pos - getPosition()))
            m_horizontalScrollbar->leftMouseReleased(pos - getPosition());
        else if (FloatRect{getPosition().x + getChildWidgetsOffset().x, getPosition().y + getChildWidgetsOffset().y, getInnerSize().x, getInnerSize().y}.contains(pos))
        {
            Panel::leftMouseReleased({pos.x + static_cast<float>(m_horizontalScrollbar->getValue()),
                                      pos.y + static_cast<float>(m_verticalScrollbar->getValue())});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::mouseMoved(Vector2f pos)
    {
        // Check if the mouse event should go to the scrollbar
        if ((m_verticalScrollbar->isMouseDown() && m_verticalScrollbar->isMouseDownOnThumb()) || m_verticalScrollbar->mouseOnWidget(pos - getPosition()))
        {
            m_verticalScrollbar->mouseMoved(pos - getPosition());
        }
        else if ((m_horizontalScrollbar->isMouseDown() && m_horizontalScrollbar->isMouseDownOnThumb()) || m_horizontalScrollbar->mouseOnWidget(pos - getPosition()))
        {
            m_horizontalScrollbar->mouseMoved(pos - getPosition());
        }
        else // Mouse not on scrollbar or dragging the scrollbar thumb
        {
            if (FloatRect{getPosition().x + getChildWidgetsOffset().x, getPosition().y + getChildWidgetsOffset().y, getInnerSize().x, getInnerSize().y}.contains(pos))
            {
                Panel::mouseMoved({pos.x + static_cast<float>(m_horizontalScrollbar->getValue()),
                                   pos.y + static_cast<float>(m_verticalScrollbar->getValue())});
            }

            m_verticalScrollbar->mouseNoLongerOnWidget();
            m_horizontalScrollbar->mouseNoLongerOnWidget();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ScrollablePanel::mouseWheelScrolled(float delta, Vector2f pos)
    {
        const bool horizontalScrollbarVisible = m_horizontalScrollbar->isVisible() && (!m_horizontalScrollbar->getAutoHide() || (m_horizontalScrollbar->getMaximum() > m_horizontalScrollbar->getViewportSize()));
        const bool verticalScrollbarVisible = m_verticalScrollbar->isVisible() && (!m_verticalScrollbar->getAutoHide() || (m_verticalScrollbar->getMaximum() > m_verticalScrollbar->getViewportSize()));

        Vector2f innerSize = getInnerSize();
        if (verticalScrollbarVisible)
            innerSize.x -= m_verticalScrollbar->getSize().x;
        if (horizontalScrollbarVisible)
            innerSize.y -= m_horizontalScrollbar->getSize().y;

        if (FloatRect{getPosition().x + getChildWidgetsOffset().x, getPosition().y + getChildWidgetsOffset().y, innerSize.x, innerSize.y}.contains(pos))
        {
            if (Container::mouseWheelScrolled(delta, pos + getContentOffset()))
                return true; // A child widget swallowed the event
        }

        if (m_horizontalScrollbar->isShown()
            && (!m_verticalScrollbar->isShown()
                || m_horizontalScrollbar->mouseOnWidget(pos - getPosition())
                || sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)
                || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift)))
        {
            m_horizontalScrollbar->mouseWheelScrolled(delta, pos - getPosition());
            mouseMoved(pos);
        }
        else if (m_verticalScrollbar->isShown())
        {
            m_verticalScrollbar->mouseWheelScrolled(delta, pos - getPosition());
            mouseMoved(pos);
        }

        return true; // We swallowed the event
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::mouseNoLongerOnWidget()
    {
        Panel::mouseNoLongerOnWidget();
        m_verticalScrollbar->mouseNoLongerOnWidget();
        m_horizontalScrollbar->mouseNoLongerOnWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::leftMouseButtonNoLongerDown()
    {
        Panel::leftMouseButtonNoLongerDown();
        m_verticalScrollbar->leftMouseButtonNoLongerDown();
        m_horizontalScrollbar->leftMouseButtonNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr ScrollablePanel::askToolTip(Vector2f mousePos)
    {
        if (mouseOnWidget(mousePos))
        {
            Widget::Ptr toolTip = nullptr;

            mousePos -= getPosition() + getChildWidgetsOffset() - getContentOffset();

            Widget::Ptr widget = mouseOnWhichWidget(mousePos);
            if (widget)
            {
                toolTip = widget->askToolTip(mousePos);
                if (toolTip)
                    return toolTip;
            }

            if (m_toolTip)
                return getToolTip();
        }

        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        const auto oldStates = states;

        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            drawBorders(target, states, m_bordersCached, getSize(), m_borderColorCached);
            states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop()});
        }

        // Draw the background
        const Vector2f innerSize = {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                                    getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()};
        drawRectangleShape(target, states, innerSize, m_backgroundColorCached);

        states.transform.translate(m_paddingCached.getLeft(), m_paddingCached.getTop());
        Vector2f contentSize = {innerSize.x - m_paddingCached.getLeft() - m_paddingCached.getRight(),
                                innerSize.y - m_paddingCached.getTop() - m_paddingCached.getBottom()};

        if (m_verticalScrollbar->isVisible() && (m_verticalScrollbar->getMaximum() > m_verticalScrollbar->getViewportSize()))
            contentSize.x -= m_verticalScrollbar->getSize().x;
        if (m_horizontalScrollbar->isVisible() && (m_horizontalScrollbar->getMaximum() > m_horizontalScrollbar->getViewportSize()))
            contentSize.y -= m_horizontalScrollbar->getSize().y;

        // If the content size is manually specified and smaller than the panel itself, then use it for clipping
        if ((m_contentSize.x > 0) && (contentSize.x > m_contentSize.x))
            contentSize.x = m_contentSize.x;
        if ((m_contentSize.y > 0) && (contentSize.y > m_contentSize.y))
            contentSize.y = m_contentSize.y;

        // Draw the child widgets
        {
            const Clipping clipping{target, states, {}, contentSize};

            states.transform.translate(-static_cast<float>(m_horizontalScrollbar->getValue()),
                                       -static_cast<float>(m_verticalScrollbar->getValue()));

            drawWidgetContainer(&target, states);
        }

        if (m_verticalScrollbar->isVisible())
            m_verticalScrollbar->draw(target, oldStates);

        if (m_horizontalScrollbar->isVisible())
            m_horizontalScrollbar->draw(target, oldStates);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::rendererChanged(const std::string& property)
    {
        if (property == "scrollbar")
        {
            m_verticalScrollbar->setRenderer(getSharedRenderer()->getScrollbar());
            m_horizontalScrollbar->setRenderer(getSharedRenderer()->getScrollbar());

            // If no scrollbar width was set then we may need to use the one from the texture
            if (!getSharedRenderer()->getScrollbarWidth())
            {
                const float width = m_verticalScrollbar->getDefaultWidth();
                m_verticalScrollbar->setSize({width, m_verticalScrollbar->getSize().y});
                m_horizontalScrollbar->setSize({m_horizontalScrollbar->getSize().x, width});
                updateScrollbars();
            }
        }
        else if (property == "scrollbarwidth")
        {
            const float width = getSharedRenderer()->getScrollbarWidth() ? getSharedRenderer()->getScrollbarWidth() : m_verticalScrollbar->getDefaultWidth();
            m_verticalScrollbar->setSize({width, m_verticalScrollbar->getSize().y});
            m_horizontalScrollbar->setSize({m_horizontalScrollbar->getSize().x, width});
            updateScrollbars();
        }
        else
            Panel::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> ScrollablePanel::save(SavingRenderersMap& renderers) const
    {
        auto node = Panel::save(renderers);

        if (m_verticalScrollbarPolicy != Scrollbar::Policy::Automatic)
        {
            if (m_verticalScrollbarPolicy == Scrollbar::Policy::Always)
                node->propertyValuePairs["VerticalScrollbarPolicy"] = std::make_unique<DataIO::ValueNode>("Always");
            else if (m_verticalScrollbarPolicy == Scrollbar::Policy::Never)
                node->propertyValuePairs["VerticalScrollbarPolicy"] = std::make_unique<DataIO::ValueNode>("Never");
        }
        if (m_horizontalScrollbarPolicy != Scrollbar::Policy::Automatic)
        {
            if (m_horizontalScrollbarPolicy == Scrollbar::Policy::Always)
                node->propertyValuePairs["HorizontalScrollbarPolicy"] = std::make_unique<DataIO::ValueNode>("Always");
            else if (m_horizontalScrollbarPolicy == Scrollbar::Policy::Never)
                node->propertyValuePairs["HorizontalScrollbarPolicy"] = std::make_unique<DataIO::ValueNode>("Never");
        }

        node->propertyValuePairs["ContentSize"] = std::make_unique<DataIO::ValueNode>("(" + to_string(m_contentSize.x) + ", " + to_string(m_contentSize.y) + ")");
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Panel::load(node, renderers);

        if (node->propertyValuePairs["contentsize"])
            setContentSize(Vector2f{node->propertyValuePairs["contentsize"]->value});

        if (node->propertyValuePairs["verticalscrollbarpolicy"])
        {
            std::string policy = toLower(trim(node->propertyValuePairs["verticalscrollbarpolicy"]->value));
            if (policy == "automatic")
                setVerticalScrollbarPolicy(Scrollbar::Policy::Automatic);
            else if (policy == "always")
                setVerticalScrollbarPolicy(Scrollbar::Policy::Always);
            else if (policy == "never")
                setVerticalScrollbarPolicy(Scrollbar::Policy::Never);
            else
                throw Exception{"Failed to parse VerticalScrollbarPolicy property, found unknown value."};
        }

        if (node->propertyValuePairs["horizontalscrollbarpolicy"])
        {
            std::string policy = toLower(trim(node->propertyValuePairs["horizontalscrollbarpolicy"]->value));
            if (policy == "automatic")
                setHorizontalScrollbarPolicy(Scrollbar::Policy::Automatic);
            else if (policy == "always")
                setHorizontalScrollbarPolicy(Scrollbar::Policy::Always);
            else if (policy == "never")
                setHorizontalScrollbarPolicy(Scrollbar::Policy::Never);
            else
                throw Exception{"Failed to parse HorizontalScrollbarPolicy property, found unknown value."};
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::updateScrollbars()
    {
        const Vector2f scrollbarSpace = {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                                         getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()};

        const Vector2f visibleSize = getInnerSize();
        m_horizontalScrollbar->setViewportSize(static_cast<unsigned int>(visibleSize.x));
        m_verticalScrollbar->setViewportSize(static_cast<unsigned int>(visibleSize.y));

        const Vector2f contentSize = getContentSize();
        m_horizontalScrollbar->setMaximum(static_cast<unsigned int>(contentSize.x));
        m_verticalScrollbar->setMaximum(static_cast<unsigned int>(contentSize.y));

        const bool horizontalScrollbarVisible = m_horizontalScrollbar->isVisible() && (!m_horizontalScrollbar->getAutoHide() || (m_horizontalScrollbar->getMaximum() > m_horizontalScrollbar->getViewportSize()));
        if (horizontalScrollbarVisible)
        {
            m_verticalScrollbar->setSize(m_verticalScrollbar->getSize().x, scrollbarSpace.y - m_horizontalScrollbar->getSize().y);
            m_verticalScrollbar->setViewportSize(static_cast<unsigned int>(m_verticalScrollbar->getViewportSize() - m_horizontalScrollbar->getSize().y));

            const bool verticalScrollbarVisible = m_verticalScrollbar->isVisible() && (!m_verticalScrollbar->getAutoHide() || (m_verticalScrollbar->getMaximum() > m_verticalScrollbar->getViewportSize()));
            if (verticalScrollbarVisible)
            {
                m_horizontalScrollbar->setViewportSize(static_cast<unsigned int>(m_horizontalScrollbar->getViewportSize() - m_verticalScrollbar->getSize().x));
                m_horizontalScrollbar->setSize(scrollbarSpace.x - m_verticalScrollbar->getSize().x, m_horizontalScrollbar->getSize().y);
            }
            else
                m_horizontalScrollbar->setSize(scrollbarSpace.x, m_horizontalScrollbar->getSize().y);
        }
        else
        {
            m_verticalScrollbar->setSize(m_verticalScrollbar->getSize().x, scrollbarSpace.y);

            const bool verticalScrollbarVisible = m_verticalScrollbar->isVisible() && (!m_verticalScrollbar->getAutoHide() || (m_verticalScrollbar->getMaximum() > m_verticalScrollbar->getViewportSize()));
            if (verticalScrollbarVisible)
            {
                m_horizontalScrollbar->setSize(scrollbarSpace.x - m_verticalScrollbar->getSize().x, m_horizontalScrollbar->getSize().y);
                m_horizontalScrollbar->setViewportSize(static_cast<unsigned int>(m_horizontalScrollbar->getViewportSize() - m_verticalScrollbar->getSize().x));

                if (m_horizontalScrollbar->isVisible() && (!m_horizontalScrollbar->getAutoHide() || (m_horizontalScrollbar->getMaximum() > m_horizontalScrollbar->getViewportSize())))
                    m_verticalScrollbar->setSize(m_verticalScrollbar->getSize().x, scrollbarSpace.y - m_horizontalScrollbar->getSize().y);
            }
            else
                m_horizontalScrollbar->setSize(scrollbarSpace.x, m_horizontalScrollbar->getSize().y);
        }

        m_verticalScrollbar->setPosition(m_bordersCached.getLeft() + scrollbarSpace.x - m_verticalScrollbar->getSize().x, m_bordersCached.getTop());
        m_horizontalScrollbar->setPosition(m_bordersCached.getLeft(), m_bordersCached.getTop() + scrollbarSpace.y - m_horizontalScrollbar->getSize().y);

        if (m_verticalScrollAmount == 0)
            setVerticalScrollAmount(0);

        if (m_horizontalScrollAmount == 0)
            setHorizontalScrollAmount(0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::recalculateMostBottomRightPosition()
    {
        m_mostBottomRightPosition = {0, 0};

        for (const auto& widget : m_widgets)
        {
            const Vector2f bottomRight = widget->getPosition() + widget->getFullSize();
            if (bottomRight.x > m_mostBottomRightPosition.x)
                m_mostBottomRightPosition.x = bottomRight.x;
            if (bottomRight.y > m_mostBottomRightPosition.y)
                m_mostBottomRightPosition.y = bottomRight.y;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::connectPositionAndSize(Widget::Ptr widget)
    {
        m_connectedCallbacks[widget] = widget->connect({"PositionChanged", "SizeChanged"}, [this](){ recalculateMostBottomRightPosition(); updateScrollbars(); });
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::disconnectAllChildWidgets()
    {
        for (const auto& pair : m_connectedCallbacks)
        {
            pair.first->disconnect(pair.second-1);
            pair.first->disconnect(pair.second);
        }

        m_connectedCallbacks.clear();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


#include <TGUI/Widgets/Slider.hpp>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider::Slider()
    {
        m_type = "Slider";

        m_draggableWidget = true;

        m_renderer = aurora::makeCopied<SliderRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setSize(200, 16);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider::Ptr Slider::create(float minimum, float maximum)
    {
        auto slider = std::make_shared<Slider>();

        slider->setMinimum(minimum);
        slider->setMaximum(maximum);

        return slider;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider::Ptr Slider::copy(Slider::ConstPtr slider)
    {
        if (slider)
            return std::static_pointer_cast<Slider>(slider->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SliderRenderer* Slider::getSharedRenderer()
    {
        return aurora::downcast<SliderRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const SliderRenderer* Slider::getSharedRenderer() const
    {
        return aurora::downcast<const SliderRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SliderRenderer* Slider::getRenderer()
    {
        return aurora::downcast<SliderRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const SliderRenderer* Slider::getRenderer() const
    {
        return aurora::downcast<const SliderRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_bordersCached.updateParentSize(getSize());

        if (getSize().x < getSize().y)
            m_verticalScroll = true;
        else if (getSize().x > getSize().y)
            m_verticalScroll = false;

        if (m_spriteTrack.isSet() && m_spriteThumb.isSet())
        {
            float scaleFactor;
            if (m_verticalImage == m_verticalScroll)
            {
                m_spriteTrack.setSize(getInnerSize());
                m_spriteTrackHover.setSize(getInnerSize());

                if (m_verticalScroll)
                    scaleFactor = getInnerSize().x / m_spriteTrack.getTexture().getImageSize().x;
                else
                    scaleFactor = getInnerSize().y / m_spriteTrack.getTexture().getImageSize().y;
            }
            else // The image is rotated
            {
                m_spriteTrack.setSize({getInnerSize().y, getInnerSize().x});
                m_spriteTrackHover.setSize({getInnerSize().y, getInnerSize().x});

                if (m_verticalScroll)
                    scaleFactor = getInnerSize().x / m_spriteTrack.getTexture().getImageSize().y;
                else
                    scaleFactor = getInnerSize().y / m_spriteTrack.getTexture().getImageSize().x;
            }

            m_thumb.width = scaleFactor * m_spriteThumb.getTexture().getImageSize().x;
            m_thumb.height = scaleFactor * m_spriteThumb.getTexture().getImageSize().y;

            m_spriteThumb.setSize({m_thumb.width, m_thumb.height});
            m_spriteThumbHover.setSize({m_thumb.width, m_thumb.height});

            // Apply the rotation now that the size has been set
            if (m_verticalScroll != m_verticalImage)
            {
                m_spriteTrack.setRotation(-90);
                m_spriteTrackHover.setRotation(-90);
                m_spriteThumb.setRotation(-90);
                m_spriteThumbHover.setRotation(-90);
            }
            else
            {
                m_spriteTrack.setRotation(0);
                m_spriteTrackHover.setRotation(0);
                m_spriteThumb.setRotation(0);
                m_spriteThumbHover.setRotation(0);
            }
        }
        else // There are no textures
        {
            if (m_verticalScroll)
            {
                m_thumb.width = getSize().x * 1.6f;
                m_thumb.height = m_thumb.width / 2.0f;
            }
            else
            {
                m_thumb.height = getSize().y * 1.6f;
                m_thumb.width = m_thumb.height / 2.0f;
            }
        }

        updateThumbPosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Slider::getFullSize() const
    {
        if (m_verticalScroll)
            return {std::max(getSize().x, m_thumb.width), getSize().y + m_thumb.height};
        else
            return {getSize().x + m_thumb.width, std::max(getSize().y, m_thumb.height)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Slider::getWidgetOffset() const
    {
        if (m_verticalScroll)
            return {std::min(0.f, (getSize().x - m_thumb.width) / 2.f), -m_thumb.height / 2.f};
        else
            return {-m_thumb.width / 2.f, std::min(0.f, (getSize().y - m_thumb.height) / 2.f)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setMinimum(float minimum)
    {
        // Set the new minimum
        m_minimum = minimum;

        // The maximum can't be below the minimum
        if (m_maximum < m_minimum)
            m_maximum = m_minimum;

        // When the value is below the minimum then adjust it
        if (m_value < m_minimum)
            setValue(m_minimum);

        updateThumbPosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Slider::getMinimum() const
    {
        return m_minimum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setMaximum(float maximum)
    {
        // Set the new maximum
        m_maximum = maximum;

        // The minimum can't be below the maximum
        if (m_minimum > m_maximum)
            setMinimum(m_maximum);

        // When the value is above the maximum then adjust it
        if (m_value > m_maximum)
            setValue(m_maximum);

        updateThumbPosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Slider::getMaximum() const
    {
        return m_maximum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setValue(float value)
    {
        // Round to nearest allowed value
        if (m_step != 0)
           value = m_minimum + (std::round((value - m_minimum) / m_step) * m_step);

        // When the value is below the minimum or above the maximum then adjust it
        if (value < m_minimum)
            value = m_minimum;
        else if (value > m_maximum)
            value = m_maximum;

        if (m_value != value)
        {
            m_value = value;

            onValueChange.emit(this, m_value);

            updateThumbPosition();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Slider::getValue() const
    {
        return m_value;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setStep(float step)
    {
        m_step = step;

        // Reset the value in case it does not match the step
        setValue(m_value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Slider::getStep() const
    {
        return m_step;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setVerticalScroll(bool vertical)
    {
        if (m_verticalScroll == vertical)
            return;

        m_verticalScroll = vertical;
        setSize(getSize().y, getSize().x);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider::getVerticalScroll() const
    {
        return m_verticalScroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setInvertedDirection(bool invertedDirection)
    {
        m_invertedDirection = invertedDirection;
        updateThumbPosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider::getInvertedDirection() const
    {
        return m_invertedDirection;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setChangeValueOnScroll(bool changeValueOnScroll)
    {
        m_changeValueOnScroll = changeValueOnScroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider::getChangeValueOnScroll() const
    {
        return m_changeValueOnScroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider::mouseOnWidget(Vector2f pos) const
    {
        pos -= getPosition();

        // Check if the mouse is on top of the thumb
        if (FloatRect(m_thumb.left, m_thumb.top, m_thumb.width, m_thumb.height).contains(pos))
            return true;

        // Check if the mouse is on top of the track
        if (FloatRect{0, 0, getSize().x, getSize().y}.contains(pos))
            return true;

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::leftMousePressed(Vector2f pos)
    {
        m_mouseDown = true;

        if (FloatRect(m_thumb.left, m_thumb.top, m_thumb.width, m_thumb.height).contains(pos))
        {
            m_mouseDownOnThumb = true;
            m_mouseDownOnThumbPos.x = pos.x - m_thumb.left;
            m_mouseDownOnThumbPos.y = pos.y - m_thumb.top;
        }
        else // The mouse is not on top of the thumb
            m_mouseDownOnThumb = false;

        // Refresh the value
        mouseMoved(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::leftMouseReleased(Vector2f)
    {
        // The thumb might have been dragged between two values
        if (m_mouseDown)
            updateThumbPosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::mouseMoved(Vector2f pos)
    {
        pos -= getPosition();

        if (!m_mouseHover)
            mouseEnteredWidget();

        if (!m_mouseDown)
            return;

        // Check in which direction the slider goes
        if (m_verticalScroll)
        {
            // Check if the click occurred on the track
            if (!m_mouseDownOnThumb)
            {
                m_mouseDownOnThumb = true;
                m_mouseDownOnThumbPos.x = pos.x - m_thumb.left;
                m_mouseDownOnThumbPos.y = m_thumb.height / 2.0f;
            }

            float value = m_maximum - (((pos.y + (m_thumb.height / 2.0f) - m_mouseDownOnThumbPos.y) / getSize().y) * (m_maximum - m_minimum));
            if (m_invertedDirection)
                value = m_maximum - (value - m_minimum);

            setValue(value);

            // Set the thumb position for smooth scrolling
            const float thumbTop = pos.y - m_mouseDownOnThumbPos.y;
            if ((thumbTop + (m_thumb.height / 2.0f) > 0) && (thumbTop + (m_thumb.height / 2.0f) < getSize().y))
                m_thumb.top = thumbTop;
            else
            {
                m_thumb.top = (getSize().y / (m_maximum - m_minimum) * (m_maximum - m_value)) - (m_thumb.height / 2.0f);
                if (m_invertedDirection)
                    m_thumb.top = getSize().y - m_thumb.top - m_thumb.height;
            }
        }
        else // the slider lies horizontal
        {
            // Check if the click occurred on the track
            if (!m_mouseDownOnThumb)
            {
                m_mouseDownOnThumb = true;
                m_mouseDownOnThumbPos.x = m_thumb.width / 2.0f;
                m_mouseDownOnThumbPos.y = pos.y - m_thumb.top;
            }

            float value = (((pos.x + (m_thumb.width / 2.0f) - m_mouseDownOnThumbPos.x) / getSize().x) * (m_maximum - m_minimum)) + m_minimum;
            if (m_invertedDirection)
                value = m_maximum - (value - m_minimum);

            setValue(value);

            // Set the thumb position for smooth scrolling
            const float thumbLeft = pos.x - m_mouseDownOnThumbPos.x;
            if ((thumbLeft + (m_thumb.width / 2.0f) > 0) && (thumbLeft + (m_thumb.width / 2.0f) < getSize().x))
                m_thumb.left = thumbLeft;
            else
            {
                m_thumb.left = (getSize().x / (m_maximum - m_minimum) * (m_value - m_minimum)) - (m_thumb.width / 2.0f);
                if (m_invertedDirection)
                    m_thumb.left = getSize().x - m_thumb.left - m_thumb.width;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider::mouseWheelScrolled(float delta, Vector2f)
    {
        // Don't do anything when changing value on scroll is disabled
        if (!m_changeValueOnScroll)
            return false;

        if (m_invertedDirection)
            delta = -delta;

        if (m_step == 0)
            setValue(m_value + delta);
        else
        {
            if (std::abs(delta) <= 1)
            {
                if (delta > 0)
                    setValue(m_value + m_step);
                else if (delta < 0)
                    setValue(m_value - m_step);
            }
            else
                setValue(m_value + std::round(delta) * m_step);
        }

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::leftMouseButtonNoLongerDown()
    {
        // The thumb might have been dragged between two values
        if (m_mouseDown)
            updateThumbPosition();

        Widget::leftMouseButtonNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& Slider::getSignal(std::string signalName)
    {
        if (signalName == toLower(onValueChange.getName()))
            return onValueChange;
        else
            return Widget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            setSize(m_size);
        }
        else if (property == "texturetrack")
        {
            m_spriteTrack.setTexture(getSharedRenderer()->getTextureTrack());

            if (m_spriteTrack.getTexture().getImageSize().x < m_spriteTrack.getTexture().getImageSize().y)
                m_verticalImage = true;
            else
                m_verticalImage = false;

            setSize(m_size);
        }
        else if (property == "texturetrackhover")
        {
            m_spriteTrackHover.setTexture(getSharedRenderer()->getTextureTrackHover());
        }
        else if (property == "texturethumb")
        {
            m_spriteThumb.setTexture(getSharedRenderer()->getTextureThumb());
            setSize(m_size);
        }
        else if (property == "texturethumbhover")
        {
            m_spriteThumbHover.setTexture(getSharedRenderer()->getTextureThumbHover());
        }
        else if (property == "trackcolor")
        {
            m_trackColorCached = getSharedRenderer()->getTrackColor();
        }
        else if (property == "trackcolorhover")
        {
            m_trackColorHoverCached = getSharedRenderer()->getTrackColorHover();
        }
        else if (property == "thumbcolor")
        {
            m_thumbColorCached = getSharedRenderer()->getThumbColor();
        }
        else if (property == "thumbcolorhover")
        {
            m_thumbColorHoverCached = getSharedRenderer()->getThumbColorHover();
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == "bordercolorhover")
        {
            m_borderColorHoverCached = getSharedRenderer()->getBorderColorHover();
        }
        else if ((property == "opacity") || (property == "opacitydisabled"))
        {
            Widget::rendererChanged(property);

            m_spriteTrack.setOpacity(m_opacityCached);
            m_spriteTrackHover.setOpacity(m_opacityCached);
            m_spriteThumb.setOpacity(m_opacityCached);
            m_spriteThumbHover.setOpacity(m_opacityCached);
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> Slider::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);
        node->propertyValuePairs["Minimum"] = std::make_unique<DataIO::ValueNode>(to_string(m_minimum));
        node->propertyValuePairs["Maximum"] = std::make_unique<DataIO::ValueNode>(to_string(m_maximum));
        node->propertyValuePairs["Value"] = std::make_unique<DataIO::ValueNode>(to_string(m_value));
        node->propertyValuePairs["Step"] = std::make_unique<DataIO::ValueNode>(to_string(m_step));
        node->propertyValuePairs["InvertedDirection"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_invertedDirection));
        node->propertyValuePairs["ChangeValueOnScroll"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_changeValueOnScroll));
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs["minimum"])
            setMinimum(strToFloat(node->propertyValuePairs["minimum"]->value));
        if (node->propertyValuePairs["maximum"])
            setMaximum(strToFloat(node->propertyValuePairs["maximum"]->value));
        if (node->propertyValuePairs["value"])
            setValue(strToFloat(node->propertyValuePairs["value"]->value));
        if (node->propertyValuePairs["step"])
            setStep(strToFloat(node->propertyValuePairs["step"]->value));
        if (node->propertyValuePairs["inverteddirection"])
            setInvertedDirection(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["inverteddirection"]->value).getBool());
        if (node->propertyValuePairs["changevalueonscroll"])
            setChangeValueOnScroll(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["changevalueonscroll"]->value).getBool());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Slider::getInnerSize() const
    {
        return {std::max(0.f, getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight()),
                std::max(0.f, getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom())};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::updateThumbPosition()
    {
        if (m_verticalScroll)
        {
            m_thumb.left = m_bordersCached.getLeft() + (getInnerSize().x - m_thumb.width) / 2.0f;
            m_thumb.top = (getSize().y / (m_maximum - m_minimum) * (m_maximum - m_value)) - (m_thumb.height / 2.0f);

            if (m_invertedDirection)
                m_thumb.top = getSize().y - m_thumb.top - m_thumb.height;
        }
        else
        {
            m_thumb.left = (getSize().x / (m_maximum - m_minimum) * (m_value - m_minimum)) - (m_thumb.width / 2.0f);
            m_thumb.top = m_bordersCached.getTop() + (getInnerSize().y - m_thumb.height) / 2.0f;

            if (m_invertedDirection)
                m_thumb.left = getSize().x - m_thumb.left - m_thumb.width;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Draw the borders around the track
        if (m_bordersCached != Borders{0})
        {
            if (m_mouseHover && m_borderColorHoverCached.isSet())
                drawBorders(target, states, m_bordersCached, getSize(), m_borderColorHoverCached);
            else
                drawBorders(target, states, m_bordersCached, getSize(), m_borderColorCached);

            states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop()});
        }

        // Draw the track
        if (m_spriteTrack.isSet() && m_spriteThumb.isSet())
        {
            if (m_mouseHover && m_spriteTrackHover.isSet())
                m_spriteTrackHover.draw(target, states);
            else
                m_spriteTrack.draw(target, states);
        }
        else // There are no textures
        {
            if (m_mouseHover && m_trackColorHoverCached.isSet())
                drawRectangleShape(target, states, getInnerSize(), m_trackColorHoverCached);
            else
                drawRectangleShape(target, states, getInnerSize(), m_trackColorCached);
        }

        states.transform.translate({-m_bordersCached.getLeft() + m_thumb.left, -m_bordersCached.getTop() + m_thumb.top});

        // Draw the borders around the thumb when using colors
        if ((m_bordersCached != Borders{0}) && !(m_spriteTrack.isSet() && m_spriteThumb.isSet()))
        {
            if (m_mouseHover && m_borderColorHoverCached.isSet())
                drawBorders(target, states, m_bordersCached, {m_thumb.width, m_thumb.height}, m_borderColorHoverCached);
            else
                drawBorders(target, states, m_bordersCached, {m_thumb.width, m_thumb.height}, m_borderColorCached);

            states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop()});
        }

        // Draw the thumb
        if (m_spriteTrack.isSet() && m_spriteThumb.isSet())
        {
            if (m_mouseHover && m_spriteThumbHover.isSet())
                m_spriteThumbHover.draw(target, states);
            else
                m_spriteThumb.draw(target, states);
        }
        else // There are no textures
        {
            const Vector2f thumbInnerSize = {m_thumb.width - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                                             m_thumb.height - m_bordersCached.getTop() - m_bordersCached.getBottom()};

            if (m_mouseHover && m_thumbColorHoverCached.isSet())
                drawRectangleShape(target, states, thumbInnerSize, m_thumbColorHoverCached);
            else
                drawRectangleShape(target, states, thumbInnerSize, m_thumbColorCached);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


#include <TGUI/Widgets/Tabs.hpp>
#include <TGUI/Clipping.hpp>

#if TGUI_COMPILED_WITH_CPP_VER >= 17
    #include <optional>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tabs::Tabs()
    {
        m_type = "Tabs";
        m_distanceToSideCached = Text::getLineHeight(m_fontCached, getGlobalTextSize()) * 0.4f;

        m_renderer = aurora::makeCopied<TabsRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setTextSize(getGlobalTextSize());
        setTabHeight(Text::getLineHeight(m_fontCached, m_textSize) * 1.25f + m_bordersCached.getTop() + m_bordersCached.getBottom());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tabs::Ptr Tabs::create()
    {
        return std::make_shared<Tabs>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tabs::Ptr Tabs::copy(Tabs::ConstPtr tabs)
    {
        if (tabs)
            return std::static_pointer_cast<Tabs>(tabs->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TabsRenderer* Tabs::getSharedRenderer()
    {
        return aurora::downcast<TabsRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const TabsRenderer* Tabs::getSharedRenderer() const
    {
        return aurora::downcast<const TabsRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TabsRenderer* Tabs::getRenderer()
    {
        return aurora::downcast<TabsRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const TabsRenderer* Tabs::getRenderer() const
    {
        return aurora::downcast<const TabsRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        // Tabs is no longer auto-sizing
        m_autoSize = false;
        recalculateTabsWidth();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::setEnabled(bool enabled)
    {
        Widget::setEnabled(enabled);
        updateTextColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::setAutoSize(bool autoSize)
    {
        if (m_autoSize != autoSize)
        {
            m_autoSize = autoSize;
            recalculateTabsWidth();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tabs::getAutoSize() const
    {
        return m_autoSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t Tabs::add(const sf::String& text, bool selectTab)
    {
        // Use the insert function to put the tab in the right place
        insert(m_tabs.size(), text, selectTab);

        // Return the index of the new tab
        return m_tabs.size()-1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::insert(std::size_t index, const sf::String& text, bool selectTab)
    {
        // If the index is too high then just insert at the end
        if (index > m_tabs.size())
            index = m_tabs.size();

        // Create the new tab
        Tab newTab;
        newTab.visible = true;
        newTab.enabled = true;
        newTab.width = 0;
        newTab.text.setFont(m_fontCached);
        newTab.text.setColor(m_textColorCached);
        newTab.text.setOpacity(m_opacityCached);
        newTab.text.setCharacterSize(getTextSize());
        newTab.text.setString(text);

        m_tabs.insert(m_tabs.begin() + index, std::move(newTab));
        recalculateTabsWidth();

        // New hovered tab depends on several factors, we keep it simple and just remove the hover state
        m_hoveringTab = -1;

        if (m_selectedTab >= static_cast<int>(index))
            m_selectedTab++;

        // If the tab has to be selected then do so
        if (selectTab)
            select(index);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String Tabs::getText(std::size_t index) const
    {
        if (index >= m_tabs.size())
            return "";
        else
            return m_tabs[index].text.getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tabs::changeText(std::size_t index, const sf::String& text)
    {
        if (index >= m_tabs.size())
            return false;

        m_tabs[index].text.setString(text);
        recalculateTabsWidth();
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tabs::select(const sf::String& text)
    {
        for (unsigned int i = 0; i < m_tabs.size(); ++i)
        {
            if (m_tabs[i].text.getString() == text)
                return select(i);
        }

        deselect();
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tabs::select(std::size_t index)
    {
        // Don't select a tab that is already selected
        if (m_selectedTab == static_cast<int>(index))
            return true;

        // If the index is too high or if the tab is invisible or disabled then we can't select it
        if ((index >= m_tabs.size()) || !m_enabled || !m_tabs[index].visible || !m_tabs[index].enabled)
        {
            deselect();
            return false;
        }

        if (m_selectedTab >= 0)
            m_tabs[m_selectedTab].text.setColor(m_textColorCached);

        // Select the tab
        m_selectedTab = static_cast<int>(index);
        m_tabs[m_selectedTab].text.setColor(m_selectedTextColorCached);

        // Send the callback
        onTabSelect.emit(this, m_tabs[index].text.getString());
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::deselect()
    {
        if (m_selectedTab >= 0)
        {
            m_tabs[m_selectedTab].text.setColor(m_textColorCached);
            m_selectedTab = -1;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tabs::remove(const sf::String& text)
    {
        for (unsigned int i = 0; i < m_tabs.size(); ++i)
        {
            if (m_tabs[i].text.getString() == text)
                return remove(i);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tabs::remove(std::size_t index)
    {
        // The index can't be too high
        if (index > m_tabs.size() - 1)
            return false;

        // Remove the tab
        m_tabs.erase(m_tabs.begin() + index);

        // Check if the selected tab should be updated
        if (m_selectedTab == static_cast<int>(index))
            m_selectedTab = -1;
        else if (m_selectedTab > static_cast<int>(index))
            --m_selectedTab;

        // New hovered tab depends on several factors, we keep it simple and just remove the hover state
        m_hoveringTab = -1;
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::removeAll()
    {
        m_tabs.clear();
        m_selectedTab = -1;
        m_hoveringTab = -1;

        recalculateTabsWidth();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String Tabs::getSelected() const
    {
        if (m_selectedTab >= 0)
            return m_tabs[m_selectedTab].text.getString();
        else
            return "";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int Tabs::getSelectedIndex() const
    {
        return m_selectedTab;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::setTabVisible(std::size_t index, bool visible)
    {
        if (index >= m_tabs.size())
            return;

        m_tabs[index].visible = visible;
        recalculateTabsWidth();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tabs::getTabVisible(std::size_t index) const
    {
        if (index >= m_tabs.size())
            return false;

        return m_tabs[index].visible;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::setTabEnabled(std::size_t index, bool enabled)
    {
        if (index >= m_tabs.size())
            return;

        m_tabs[index].enabled = enabled;
        updateTextColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tabs::getTabEnabled(std::size_t index) const
    {
        if (index >= m_tabs.size())
            return false;

        return m_tabs[index].enabled;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::setTextSize(unsigned int size)
    {
        if ((size == 0) || (m_requestedTextSize != size))
        {
            m_requestedTextSize = size;

            if (size == 0)
                m_textSize = Text::findBestTextSize(m_fontCached, (getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()) * 0.8f);
            else
                m_textSize = size;

            for (auto& tab : m_tabs)
                tab.text.setCharacterSize(m_textSize);

            recalculateTabsWidth();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::setTabHeight(float height)
    {
        Widget::setSize({getSizeLayout().x, height});
        m_bordersCached.updateParentSize(getSize());

        // Recalculate the size when the text is auto sizing
        if (m_requestedTextSize == 0)
            setTextSize(0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::setMaximumTabWidth(float maximumWidth)
    {
        m_maximumTabWidth = maximumWidth;

        recalculateTabsWidth();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Tabs::getMaximumTabWidth() const
    {
        return m_maximumTabWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::setMinimumTabWidth(float minimumWidth)
    {
        m_minimumTabWidth = minimumWidth;

        recalculateTabsWidth();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Tabs::getMinimumTabWidth() const
    {
        return m_minimumTabWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t Tabs::getTabsCount() const
    {
        return m_tabs.size();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tabs::mouseOnWidget(Vector2f pos) const
    {
        return FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::leftMousePressed(Vector2f pos)
    {
        pos -= getPosition();

        float width = m_bordersCached.getLeft() / 2.f;
        for (unsigned int i = 0; i < m_tabs.size(); ++i)
        {
            if (!m_tabs[i].visible)
                continue;

            // Append the width of the tab
            width += (m_bordersCached.getLeft() / 2.f) + m_tabs[i].width + (m_bordersCached.getRight() / 2.0f);

            // If the mouse went down on this tab then select it
            if (pos.x < width)
            {
                select(i);
                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::mouseMoved(Vector2f pos)
    {
        Widget::mouseMoved(pos);

        pos -= getPosition();
        m_hoveringTab = -1;
        float width = m_bordersCached.getLeft() / 2.f;

        for (unsigned int i = 0; i < m_tabs.size(); ++i)
        {
            if (!m_tabs[i].visible)
                continue;

            // Append the width of the tab
            width += (m_bordersCached.getLeft() / 2.f) + m_tabs[i].width + (m_bordersCached.getRight() / 2.0f);

            // If the mouse is on top of this tab then remember it
            if (pos.x < width)
            {
                if (m_tabs[i].enabled)
                    m_hoveringTab = i;

                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::mouseNoLongerOnWidget()
    {
        Widget::mouseNoLongerOnWidget();

        m_hoveringTab = -1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::recalculateTabsWidth()
    {
        unsigned int visibleTabs = 0;
        for (unsigned int i = 0; i < m_tabs.size(); ++i)
        {
            if (m_tabs[i].visible)
                visibleTabs++;
        }

        if (m_autoSize)
        {
            if (m_tabs.empty())
                Widget::setSize({m_bordersCached.getLeft() + m_bordersCached.getRight(), getSizeLayout().y});
            else
            {
                // First calculate the width of the tabs as if there aren't any borders
                float totalWidth = 0;
                for (unsigned int i = 0; i < m_tabs.size(); ++i)
                {
                    if (!m_tabs[i].visible)
                        continue;

                    m_tabs[i].width = m_tabs[i].text.getSize().x + std::max(m_minimumTabWidth, 2 * m_distanceToSideCached);
                    if ((m_maximumTabWidth > 0) && (m_maximumTabWidth < m_tabs[i].width))
                        m_tabs[i].width = m_maximumTabWidth;

                    totalWidth += m_tabs[i].width;
                }

                // Now add the borders to the tabs
                totalWidth += (visibleTabs + 1) * ((m_bordersCached.getLeft() + m_bordersCached.getRight()) / 2.f);

                Widget::setSize({totalWidth, getSizeLayout().y});
            }
        }
        else // A size was provided
        {
            const float tabWidth = (getSize().x - ((visibleTabs + 1) * ((m_bordersCached.getLeft() + m_bordersCached.getRight()) / 2.f))) / visibleTabs;
            for (unsigned int i = 0; i < m_tabs.size(); ++i)
                m_tabs[i].width = tabWidth;
        }

        m_bordersCached.updateParentSize(getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& Tabs::getSignal(std::string signalName)
    {
        if (signalName == toLower(onTabSelect.getName()))
            return onTabSelect;
        else
            return Widget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            recalculateTabsWidth();
        }
        else if (property == "textcolor")
        {
            m_textColorCached = getSharedRenderer()->getTextColor();
            updateTextColors();
        }
        else if (property == "textcolorhover")
        {
            m_textColorHoverCached = getSharedRenderer()->getTextColorHover();
            updateTextColors();
        }
        else if (property == "textcolordisabled")
        {
            m_textColorDisabledCached = getSharedRenderer()->getTextColorDisabled();
            updateTextColors();
        }
        else if (property == "selectedtextcolor")
        {
            m_selectedTextColorCached = getSharedRenderer()->getSelectedTextColor();
            updateTextColors();
        }
        else if (property == "selectedtextcolorhover")
        {
            m_selectedTextColorHoverCached = getSharedRenderer()->getSelectedTextColorHover();
            updateTextColors();
        }
        else if (property == "texturetab")
        {
            m_spriteTab.setTexture(getSharedRenderer()->getTextureTab());
        }
        else if (property == "texturetabhover")
        {
            m_spriteTabHover.setTexture(getSharedRenderer()->getTextureTabHover());
        }
        else if (property == "textureselectedtab")
        {
            m_spriteSelectedTab.setTexture(getSharedRenderer()->getTextureSelectedTab());
        }
        else if (property == "textureselectedtabhover")
        {
            m_spriteSelectedTabHover.setTexture(getSharedRenderer()->getTextureSelectedTabHover());
        }
        else if (property == "texturedisabledtab")
        {
            m_spriteDisabledTab.setTexture(getSharedRenderer()->getTextureDisabledTab());
        }
        else if (property == "distancetoside")
        {
            m_distanceToSideCached = getSharedRenderer()->getDistanceToSide();
            recalculateTabsWidth();
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
        else if (property == "selectedbackgroundcolor")
        {
            m_selectedBackgroundColorCached = getSharedRenderer()->getSelectedBackgroundColor();
        }
        else if (property == "selectedbackgroundcolorhover")
        {
            m_selectedBackgroundColorHoverCached = getSharedRenderer()->getSelectedBackgroundColorHover();
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if ((property == "opacity") || (property == "opacitydisabled"))
        {
            Widget::rendererChanged(property);

            m_spriteTab.setOpacity(m_opacityCached);
            m_spriteTabHover.setOpacity(m_opacityCached);
            m_spriteSelectedTab.setOpacity(m_opacityCached);
            m_spriteSelectedTabHover.setOpacity(m_opacityCached);
            m_spriteDisabledTab.setOpacity(m_opacityCached);

            for (auto& tab : m_tabs)
                tab.text.setOpacity(m_opacityCached);
        }
        else if (property == "font")
        {
            Widget::rendererChanged(property);

            for (auto& tab : m_tabs)
                tab.text.setFont(m_fontCached);

            // Recalculate the size when the text is auto sizing
            if (m_requestedTextSize == 0)
                setTextSize(0);
            else
                recalculateTabsWidth();
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> Tabs::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        if (m_tabs.size() > 0)
        {
            bool allTabsVisible = true;
            bool allTabsEnabled = true;
            std::string tabList = "[" + Serializer::serialize(getText(0));
            std::string tabVisibleList = "[" + Serializer::serialize(getTabVisible(0));
            std::string tabEnabledList = "[" + Serializer::serialize(getTabEnabled(0));
            for (std::size_t i = 1; i < m_tabs.size(); ++i)
            {
                tabList += ", " + Serializer::serialize(getText(i));
                tabVisibleList += ", " + Serializer::serialize(getTabVisible(i));
                tabEnabledList += ", " + Serializer::serialize(getTabEnabled(i));

                if (!getTabVisible(i))
                    allTabsVisible = false;
                if (!getTabEnabled(i))
                    allTabsEnabled = false;
            }

            tabList += "]";
            tabVisibleList += "]";
            tabEnabledList += "]";

            node->propertyValuePairs["Tabs"] = std::make_unique<DataIO::ValueNode>(tabList);
            if (!allTabsVisible)
                node->propertyValuePairs["TabsVisible"] = std::make_unique<DataIO::ValueNode>(tabVisibleList);
            if (!allTabsEnabled)
                node->propertyValuePairs["TabsEnabled"] = std::make_unique<DataIO::ValueNode>(tabEnabledList);
        }

        if (getSelectedIndex() >= 0)
            node->propertyValuePairs["Selected"] = std::make_unique<DataIO::ValueNode>(to_string(getSelectedIndex()));

        if (m_maximumTabWidth > 0)
            node->propertyValuePairs["MaximumTabWidth"] = std::make_unique<DataIO::ValueNode>(to_string(m_maximumTabWidth));

        if (m_autoSize)
        {
            node->propertyValuePairs.erase("Size");
            node->propertyValuePairs["TabHeight"] = std::make_unique<DataIO::ValueNode>(to_string(getSize().y));
        }

        node->propertyValuePairs["TextSize"] = std::make_unique<DataIO::ValueNode>(to_string(m_textSize));
        node->propertyValuePairs["AutoSize"] = std::make_unique<DataIO::ValueNode>(to_string(m_autoSize));

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs["tabs"])
        {
            if (!node->propertyValuePairs["tabs"]->listNode)
                throw Exception{"Failed to parse 'Tabs' property, expected a list as value"};

            for (const auto& tabText : node->propertyValuePairs["tabs"]->valueList)
                add(Deserializer::deserialize(ObjectConverter::Type::String, tabText).getString(), false);
        }

        if (node->propertyValuePairs["tabsvisible"])
        {
            if (!node->propertyValuePairs["tabsvisible"]->listNode)
                throw Exception{"Failed to parse 'TabsVisible' property, expected a list as value"};

            const auto& values = node->propertyValuePairs["tabsvisible"]->valueList;
            for (unsigned int i = 0; i < values.size(); ++i)
                setTabVisible(i, Deserializer::deserialize(ObjectConverter::Type::Bool, values[i]).getBool());
        }

        if (node->propertyValuePairs["tabsenabled"])
        {
            if (!node->propertyValuePairs["tabsenabled"]->listNode)
                throw Exception{"Failed to parse 'TabsEnabled' property, expected a list as value"};

            const auto& values = node->propertyValuePairs["tabsenabled"]->valueList;
            for (unsigned int i = 0; i < values.size(); ++i)
                setTabEnabled(i, Deserializer::deserialize(ObjectConverter::Type::Bool, values[i]).getBool());
        }

        if (node->propertyValuePairs["maximumtabwidth"])
            setMaximumTabWidth(strToFloat(node->propertyValuePairs["maximumtabwidth"]->value));
        if (node->propertyValuePairs["textsize"])
            setTextSize(strToInt(node->propertyValuePairs["textsize"]->value));
        if (node->propertyValuePairs["tabheight"])
            setTabHeight(strToFloat(node->propertyValuePairs["tabheight"]->value));
        if (node->propertyValuePairs["selected"])
            select(strToInt(node->propertyValuePairs["selected"]->value));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::updateTextColors()
    {
        for (auto& tab : m_tabs)
        {
            if ((!m_enabled || !tab.enabled) && m_textColorDisabledCached.isSet())
                tab.text.setColor(m_textColorDisabledCached);
            else
                tab.text.setColor(m_textColorCached);
        }

        if (m_enabled)
        {
            if (m_selectedTab >= 0)
            {
                if ((m_selectedTab == m_hoveringTab) && m_selectedTextColorHoverCached.isSet())
                    m_tabs[m_selectedTab].text.setColor(m_selectedTextColorHoverCached);
                else if (m_selectedTextColorCached.isSet())
                    m_tabs[m_selectedTab].text.setColor(m_selectedTextColorCached);
            }

            if ((m_hoveringTab >= 0) && (m_selectedTab != m_hoveringTab))
            {
                if (m_textColorHoverCached.isSet())
                    m_tabs[m_hoveringTab].text.setColor(m_textColorHoverCached);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());
        states.transform.translate({});

        // Draw the borders around the tabs
        if (m_bordersCached != Borders{0})
        {
            drawBorders(target, states, m_bordersCached, getSize(), m_borderColorCached);
            states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop()});
        }

        const float usableHeight = getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom();
        for (unsigned int i = 0; i < m_tabs.size(); ++i)
        {
            if (!m_tabs[i].visible)
                continue;

            sf::RenderStates textStates = states;

            // Draw the background of the tab
            if (m_spriteTab.isSet() && m_spriteSelectedTab.isSet())
            {
                Sprite spriteTab;
                if ((!m_enabled || !m_tabs[i].enabled) && m_spriteDisabledTab.isSet())
                    spriteTab = m_spriteDisabledTab;
                else if (m_selectedTab == static_cast<int>(i))
                {
                    if ((m_hoveringTab == static_cast<int>(i)) && m_spriteSelectedTabHover.isSet())
                        spriteTab = m_spriteSelectedTabHover;
                    else
                        spriteTab = m_spriteSelectedTab;
                }
                else if ((m_hoveringTab == static_cast<int>(i)) && m_spriteTabHover.isSet())
                    spriteTab = m_spriteTabHover;
                else
                    spriteTab = m_spriteTab;

                spriteTab.setSize({m_tabs[i].width, usableHeight});
                spriteTab.draw(target, states);
            }
            else // No texture was loaded
            {
                if ((!m_enabled || !m_tabs[i].enabled) && m_backgroundColorDisabledCached.isSet())
                    drawRectangleShape(target, states, {m_tabs[i].width, usableHeight}, m_backgroundColorDisabledCached);
                else if (m_selectedTab == static_cast<int>(i))
                {
                    if ((m_hoveringTab == static_cast<int>(i)) && m_selectedBackgroundColorHoverCached.isSet())
                        drawRectangleShape(target, states, {m_tabs[i].width, usableHeight}, m_selectedBackgroundColorHoverCached);
                    else
                        drawRectangleShape(target, states, {m_tabs[i].width, usableHeight}, m_selectedBackgroundColorCached);
                }
                else if ((m_hoveringTab == static_cast<int>(i)) && m_backgroundColorHoverCached.isSet())
                    drawRectangleShape(target, states, {m_tabs[i].width, usableHeight}, m_backgroundColorHoverCached);
                else
                    drawRectangleShape(target, states, {m_tabs[i].width, usableHeight}, m_backgroundColorCached);
            }

            // Draw the borders between the tabs
            states.transform.translate({m_tabs[i].width, 0});
            if ((m_bordersCached != Borders{0}) && (i < m_tabs.size() - 1))
            {
                drawRectangleShape(target, states, {(m_bordersCached.getLeft() + m_bordersCached.getRight()) / 2.f, usableHeight}, m_borderColorCached);
                states.transform.translate({(m_bordersCached.getLeft() + m_bordersCached.getRight()) / 2.f, 0});
            }

            // Apply clipping if required for the text in this tab
            const float usableWidth = m_tabs[i].width - (2 * m_distanceToSideCached);
#if TGUI_COMPILED_WITH_CPP_VER >= 17
            std::optional<Clipping> clipping;
            if (m_tabs[i].text.getSize().x > usableWidth)
                clipping.emplace(target, textStates, Vector2f{m_distanceToSideCached, 0}, Vector2f{usableWidth, usableHeight});
#else
            std::unique_ptr<Clipping> clipping;
            if (m_tabs[i].text.getSize().x > usableWidth)
                clipping = std::make_unique<Clipping>(target, textStates, Vector2f{m_distanceToSideCached, 0}, Vector2f{usableWidth, usableHeight});
#endif

            // Draw the text
            textStates.transform.translate({m_distanceToSideCached + ((usableWidth - m_tabs[i].text.getSize().x) / 2.f), ((usableHeight - m_tabs[i].text.getSize().y) / 2.f)});
            m_tabs[i].text.draw(target, textStates);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


#include <TGUI/Widgets/TreeView.hpp>
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
                || sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)
                || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift)))
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

    void TreeView::update(sf::Time elapsedTime)
    {
        Widget::update(elapsedTime);

        if (m_animationTimeElapsed >= sf::milliseconds(getDoubleClickTime()))
        {
            m_animationTimeElapsed = {};
            m_possibleDoubleClick = false;
        }
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

