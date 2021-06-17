#include <iostream>

#include <dirent.h>

#include <sys/stat.h>

#include <TGUI/TGUI.hpp>

#include "IniParser/ParserInfoFile.h"

#include "WorkFunctions.h"

using namespace std;

using namespace IniParser;

void fix(sf::Sprite& sprite, tgui::Gui& gui, sf::RenderWindow& window)
{
    sf::Vector2f scalesp = sprite.getScale();
    sf::Vector2f possp = sprite.getPosition();
    sf::View view{sf::FloatRect(-possp.x, -possp.y, 1.f / scalesp.x * window.getSize().x, 1.f / scalesp.y * window.getSize().y)};
    gui.setView(view);
}

vector<string> getDirs(string path, string filetest)
{
    vector<string> dirs;
    const char* PATH = path.c_str();

    DIR *dir = opendir(PATH);

    struct dirent *entry = readdir(dir);

    while (entry != NULL)
    {
        //if (entry->d_type == DT_DIR)

        struct stat buff;
        stat(entry->d_name, &buff);
        string fname = string(entry->d_name);
        if(fname != "." && fname != ".." && WorkFunctions::FileFunctions::isFileExists(path + "/" + fname + "/" + filetest))
            dirs.push_back(entry->d_name);

        entry = readdir(dir);
    }

    closedir(dir);

    return dirs;
}

class Launcher
{
    public:
        Launcher():
            s_DD2Ini(0),
            s_GuiFile(""),
            s_Window(0),
            s_DD2FileName("")
        {
            //...
        }
        ~Launcher()
        {
            delete s_DD2Ini;
            delete s_DD2General;
        }
        void resetToDefault()
        {
            if(!s_DD2Ini) delete s_DD2Ini;
            ParserInfoFile prs;
            s_DD2Ini = prs.getParsedFromFile("Launcher/DD2Default.ini");
            loadData();
        }
        void saveConfig()
        {
            ParserInfoFile prs;
            if(!s_DD2Ini) return;

            tgui::CheckBox::Ptr stnd = s_TGUI.get<tgui::CheckBox>("Standard");
            if(stnd->isChecked()) s_DD2Ini->setValue("resources", "standard", "true");
            else s_DD2Ini->setValue("resources", "standard", "false");

            tgui::CheckBox::Ptr pooling = s_TGUI.get<tgui::CheckBox>("Pooling");
            if(pooling->isChecked()) s_DD2Ini->setValue("resources", "pooling", "true");
            else s_DD2Ini->setValue("resources", "pooling", "false");

            tgui::ComboBox::Ptr modpack = s_TGUI.get<tgui::ComboBox>("Modpack");
            s_DD2Ini->setValue("resources", "modpack", modpack->getSelectedItem());

            tgui::ComboBox::Ptr levelpack = s_TGUI.get<tgui::ComboBox>("Levelpack");
            s_DD2Ini->setValue("resources", "levelpack", levelpack->getSelectedItem());

            tgui::ComboBox::Ptr texturepack = s_TGUI.get<tgui::ComboBox>("Texturepack");
            s_DD2Ini->setValue("resources", "texturepack", texturepack->getSelectedItem());

            tgui::ComboBox::Ptr monsterpack = s_TGUI.get<tgui::ComboBox>("Monsterpack");
            s_DD2Ini->setValue("resources", "monsterpack", monsterpack->getSelectedItem());

            tgui::ComboBox::Ptr bonuspack = s_TGUI.get<tgui::ComboBox>("Bonuspack");
            s_DD2Ini->setValue("resources", "bonuspack", bonuspack->getSelectedItem());

            tgui::ComboBox::Ptr screenpack = s_TGUI.get<tgui::ComboBox>("Screenpack");
            s_DD2Ini->setValue("resources", "screenpack", screenpack->getSelectedItem());

            // SoundPacks

            tgui::ComboBox::Ptr davepack = s_TGUI.get<tgui::ComboBox>("Davepack");
            s_DD2Ini->setValue("resources", "davepack", davepack->getSelectedItem());

            prs.writeParsedToFile(s_DD2Ini, "DD2.ini");
        }
        void addTextsEditBoxes(PostParsingStruct* pps, tgui::ScrollablePanel::Ptr layout, string prefix)
        {
            int i = 0;
            map<string, map<string, string> >::iterator iter;
            map<string, string>::iterator iter1;
            for(iter = pps->getMapVariables().begin(); iter != pps->getMapVariables().end(); iter++, i++)
            {
                auto label = tgui::Label::create();
                label->setText(iter->first);
                label->setPosition(5, i * 30);
                label->setTextSize(25);
                layout->add(label);
                i++;
                i++;
                for(iter1 = iter->second.begin(); iter1 != iter->second.end(); iter1++, i++)
                {
                    auto label = tgui::Label::create();
                    label->setText(iter1->first);
                    label->setPosition(15, i * 30);
                    label->setTextSize(15);
                    layout->add(label);

                    auto editBox = tgui::EditBox::create();
                    editBox->setTextSize(18);
                    editBox->setPosition(250, i * 30);
                    editBox->setText(iter1->second);
                    editBox->setDefaultText("Empty");
                    editBox->setWidgetName(prefix + "/" + iter->first + "/" + iter1->first);
                    layout->add(editBox);
                }
            }
            auto label = tgui::Label::create();
            label->setText("");
            label->setPosition(15, i * 30);
            label->setTextSize(15);
            layout->add(label);
        }
        void editAllVariables()
        {
            auto child = tgui::ChildWindow::create();
            child->setSize("100%", "97%");
            //child->setPosition(0, 0);
            child->setTitle("Edit all variables");
            child->setPositionLocked(true);
            s_TGUI.add(child);

            auto layout = tgui::ScrollablePanel::create();
            layout->setSize("40%", "100%");
            layout->setVerticalScrollAmount(40);
            child->add(layout);

            auto layout_gen = tgui::ScrollablePanel::create();
            layout_gen->setSize("40%", "100%");
            layout_gen->setPosition("40%", "0%");
            layout_gen->setVerticalScrollAmount(40);
            child->add(layout_gen);


            addTextsEditBoxes(s_DD2Ini, layout, "ini");
            addTextsEditBoxes(s_DD2General, layout_gen, "gen");

            auto button = tgui::Button::create();
            button->setPosition("85%", "5%");
            button->setText("Save");
            button->setSize("10%", "5%");
            //button->connect("pressed", [=](){ child->setVisible(false); });
            child->add(button);

            button = tgui::Button::create();
            button->setPosition("85%", "12%");
            button->setText("Close");
            button->setSize("10%", "5%");
            button->connect("pressed", [=](){ s_TGUI.remove(child); });
            child->add(button);
        }
        void loadWidgets()
        {
            s_TGUI.removeAllWidgets();
            s_TGUI.loadWidgetsFromFile(s_GuiFile);

            tgui::Button::Ptr resetdefault = s_TGUI.get<tgui::Button>("ResetToDefault");
            resetdefault->connect("pressed", Launcher::resetToDefault, this);

            tgui::Button::Ptr saveconfig = s_TGUI.get<tgui::Button>("SaveConfig");
            saveconfig->connect("pressed", Launcher::saveConfig, this);

            tgui::Button::Ptr savelaunchgame = s_TGUI.get<tgui::Button>("SaveLaunchGame");
            savelaunchgame->connect("pressed", Launcher::saveLaunchGame, this);

            tgui::Button::Ptr exitbutton = s_TGUI.get<tgui::Button>("Exit");
            exitbutton->connect("pressed", [=](){ s_Window->close(); });

            tgui::Button::Ptr EdAllVar = s_TGUI.get<tgui::Button>("EdAllVar");
            EdAllVar->connect("pressed", Launcher::editAllVariables, this);
        }
        void setDifficulty(const sf::String& difvalue)
        {
            map<string, string> textsdif;
            textsdif["Baby"] = "You have infinite lives. You have health points (you will withstand several blows before death). The level at your death does restart.";
            textsdif["Newbie"] = "You have infinite lives. The level at your death does restart.";
            textsdif["Very easy"] = "You have health points (you will withstand several blows before death). The level at your death does restart.";
            textsdif["Easy"] = "The level at your death does not restart. All killed monsters remain killed.";
            textsdif["Normal"] = "The level at your death does restart.";
            textsdif["Hard"] = "You have one life per level. When you die, you move to the previous level.";
            textsdif["Very hard"] = "You have one life per game. When you die, the game is over.";
            tgui::Group::Ptr customdiffield = s_TGUI.get<tgui::Group>("CustomDifficlutyField");
            tgui::Label::Ptr textdif = s_TGUI.get<tgui::Label>("TextDifficulty");
            customdiffield->setPosition(textdif->getPosition());
            if(difvalue == "Custom")
            {
                customdiffield->setVisible(true);
                textdif->setVisible(false);
            }
            else
            {
                customdiffield->setVisible(false);
                textdif->setVisible(true);
                textdif->setText(textsdif[difvalue]);
            }
        }
        void loadData()
        {
            if(!s_DD2Ini || s_GuiFile == "") return;

            loadWidgets();

            tgui::CheckBox::Ptr stnd = s_TGUI.get<tgui::CheckBox>("Standard");
            if(s_DD2Ini->getValue("resources", "standard") == "true") stnd->setChecked(true);
            else stnd->setChecked(false);

            tgui::CheckBox::Ptr pooling = s_TGUI.get<tgui::CheckBox>("Pooling");
            if(s_DD2Ini->getValue("resources", "pooling") == "true") pooling->setChecked(true);
            else pooling->setChecked(false);

            tgui::ComboBox::Ptr modpack = s_TGUI.get<tgui::ComboBox>("Modpack");
            vector<string> modpacks = getDirs("ModPacks", "mod.info");
            for(unsigned int i = 0; i < modpacks.size(); i++)
                if(modpacks[i] != "StandardDave") modpack->addItem(modpacks[i]);
            if(s_DD2Ini->getValue("resources", "modpack") != "") modpack->setSelectedItem(s_DD2Ini->getValue("resources", "modpack"));
            else modpack->setSelectedItem("");

            tgui::ComboBox::Ptr levelpack = s_TGUI.get<tgui::ComboBox>("Levelpack");
            vector<string> levelpacks = getDirs("PacksData/LevelPacks", "levels.dat");
            for(unsigned int i = 0; i < levelpacks.size(); i++) levelpack->addItem(levelpacks[i]);
            if(s_DD2Ini->getValue("resources", "levelpack") != "") levelpack->setSelectedItem(s_DD2Ini->getValue("resources", "levelpack"));
            else levelpack->setSelectedItem("");

            tgui::ComboBox::Ptr texturepack = s_TGUI.get<tgui::ComboBox>("Texturepack");
            vector<string> texturepacks = getDirs("PacksData/TexturePacks", "tiles.info");
            for(unsigned int i = 0; i < texturepacks.size(); i++) texturepack->addItem(texturepacks[i]);
            if(s_DD2Ini->getValue("resources", "texturepack") != "") texturepack->setSelectedItem(s_DD2Ini->getValue("resources", "texturepack"));
            else texturepack->setSelectedItem("");

            tgui::ComboBox::Ptr monsterpack = s_TGUI.get<tgui::ComboBox>("Monsterpack");
            vector<string> monsterpacks = getDirs("PacksData/MonsterPacks", "monsters.dat");
            for(unsigned int i = 0; i < monsterpacks.size(); i++) monsterpack->addItem(monsterpacks[i]);
            if(s_DD2Ini->getValue("resources", "monsterpack") != "") monsterpack->setSelectedItem(s_DD2Ini->getValue("resources", "monsterpack"));
            else monsterpack->setSelectedItem("");

            tgui::ComboBox::Ptr bonuspack = s_TGUI.get<tgui::ComboBox>("Bonuspack");
            vector<string> bonuspacks = getDirs("PacksData/BonusPacks", "bonuses.dat");
            for(unsigned int i = 0; i < bonuspacks.size(); i++) bonuspack->addItem(bonuspacks[i]);
            if(s_DD2Ini->getValue("resources", "bonuspack") != "") bonuspack->setSelectedItem(s_DD2Ini->getValue("resources", "bonuspack"));
            else bonuspack->setSelectedItem("");

            tgui::ComboBox::Ptr screenpack = s_TGUI.get<tgui::ComboBox>("Screenpack");
            vector<string> screenpacks = getDirs("PacksData/ScreenPacks", "screens.dat");
            for(unsigned int i = 0; i < screenpacks.size(); i++) screenpack->addItem(screenpacks[i]);
            if(s_DD2Ini->getValue("resources", "screenpack") != "") screenpack->setSelectedItem(s_DD2Ini->getValue("resources", "screenpack"));
            else screenpack->setSelectedItem("");

            tgui::ComboBox::Ptr soundpack = s_TGUI.get<tgui::ComboBox>("Soundpack");
            vector<string> soundpacks = getDirs("PacksData/SoundPacks", "sounds.info");
            for(unsigned int i = 0; i < soundpacks.size(); i++) soundpack->addItem(soundpacks[i]);
            if(s_DD2Ini->getValue("resources", "soundpack") != "") soundpack->setSelectedItem(s_DD2Ini->getValue("resources", "soundpack"));
            else soundpack->setSelectedItem("");

            tgui::ComboBox::Ptr musicpack = s_TGUI.get<tgui::ComboBox>("Musicpack");
            vector<string> musicpacks = getDirs("PacksData/MusicPacks", "music.info");
            for(unsigned int i = 0; i < musicpacks.size(); i++) musicpack->addItem(musicpacks[i]);
            if(s_DD2Ini->getValue("resources", "musicpack") != "") musicpack->setSelectedItem(s_DD2Ini->getValue("resources", "musicpack"));
            else musicpack->setSelectedItem("");

            tgui::ComboBox::Ptr davepack = s_TGUI.get<tgui::ComboBox>("Davepack");
            vector<string> davepacks = getDirs("PacksData/DavePacks", "dave.dat");
            for(unsigned int i = 0; i < davepacks.size(); i++) davepack->addItem(davepacks[i]);
            if(s_DD2Ini->getValue("resources", "davepack") != "") davepack->setSelectedItem(s_DD2Ini->getValue("resources", "davepack"));
            else davepack->setSelectedItem("");

            tgui::Tabs::Ptr dif = s_TGUI.get<tgui::Tabs>("Difficulty");
            setDifficulty("Normal");
            dif->connect("TabSelected", Launcher::setDifficulty, this);
        }
        void saveLaunchGame()
        {
            saveConfig();
            if(s_DD2Ini->getValue("resources", "standard") == "false" && s_DD2Ini->getValue("resources", "pooling") == "false")
            {
                auto child = tgui::ChildWindow::create();
                child->setSize("40%", "20%");
                child->setPosition("30%", "30%");
                child->setTitle("Error launch game!");
                s_TGUI.add(child);

                auto label = tgui::Label::create();
                label->setText("Standard and Pooling variables can not be false at the same time!");
                label->setSize("100%", "85%");
                label->setPosition("0%", "15%");
                label->setTextSize(25);
                label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
                child->add(label);

                auto button = tgui::Button::create();
                button->setPosition("35%", "70%");
                button->setText("Ok");
                button->setSize("30%", "20%");
                button->connect("pressed", [=](){ s_TGUI.remove(child); });
                child->add(button);

                return;
            }
            system(("start " + s_DD2FileName).c_str());
            s_Window->close();
        }
        PostParsingStruct* s_DD2Ini;
        PostParsingStruct* s_DD2General;
        PostParsingStruct* s_LauncherConfig;
        string s_GuiFile;
        sf::RenderWindow* s_Window;
        tgui::Gui s_TGUI;
        string s_DD2FileName;
};

int main()
{
    sf::RenderWindow window{{1280, 720}, "DD2: ME Launcher"};

    window.setFramerateLimit(60);

    sf::RenderTexture rtgui;

    if (!rtgui.create(1280, 720))
    {
        cout << "Error creating!" << endl;
    }


    Launcher ln;

    ln.s_Window = &window;

    ln.s_TGUI.setTarget(rtgui);
    //tgui::Gui gui{rtgui};

    //gui.set

    ParserInfoFile prs;
    ln.s_DD2Ini = prs.getParsedFromFile("DD2.ini");
    ln.s_DD2General = prs.getParsedFromFile("General.ini");
    ln.s_LauncherConfig = prs.getParsedFromFile("Launcher/Launcher.ini");
    ln.s_GuiFile = "Launcher/LauncherDD2.gui";

    ln.loadWidgets();

    ln.loadData();

    //ln.s_DD2FileName = "DD2Debug.exe";
    ln.s_DD2FileName = ln.s_LauncherConfig->getValue("general", "dd2_filename");

    sf::Sprite posguisprite;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            // When the window is resized, the view is changed
            else if (event.type == sf::Event::Resized)
            {
                window.setView(sf::View(sf::FloatRect(0.f, 0.f, static_cast<float>(event.size.width), static_cast<float>(event.size.height))));
                ln.s_TGUI.setView(window.getView());
            }

            ln.s_TGUI.handleEvent(event); // Pass the event to the widgets
        }

        rtgui.clear(sf::Color(255, 255, 255, 255));
        ln.s_TGUI.draw();
        rtgui.display();

        const sf::Texture& guitexture = rtgui.getTexture();

        // draw it to the window
        sf::Sprite guisprite(guitexture);

        //fix(posguisprite, gui, window);

        window.clear();
        window.draw(guisprite);
        window.display();
    }
}
