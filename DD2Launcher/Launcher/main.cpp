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
    //sf::View view{sf::FloatRect(-possp.x, -possp.y, 1.f / scalesp.x * window.getSize().x, 1.f / scalesp.y * window.getSize().y)};
    //gui.setView(view);
    gui.setAbsoluteView(tgui::FloatRect(-possp.x, -possp.y, 1.f / scalesp.x * window.getSize().x, 1.f / scalesp.y * window.getSize().y));
    //gui.setRelativeView(tgui::FloatRect(-possp.x, -possp.y, 1.f / scalesp.x * window.getSize().x, 1.f / scalesp.y * window.getSize().y));
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
            s_DD2FileName(""),
            s_KeyEditBox(0),
            s_KeyWaitPanel(0)
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
            s_DD2Ini->setValue("resources", "modpack", modpack->getSelectedItem().toStdString() );

            tgui::ComboBox::Ptr levelpack = s_TGUI.get<tgui::ComboBox>("Levelpack");
            s_DD2Ini->setValue("resources", "levelpack", levelpack->getSelectedItem().toStdString() );

            tgui::ComboBox::Ptr texturepack = s_TGUI.get<tgui::ComboBox>("Texturepack");
            s_DD2Ini->setValue("resources", "texturepack", texturepack->getSelectedItem().toStdString() );

            tgui::ComboBox::Ptr monsterpack = s_TGUI.get<tgui::ComboBox>("Monsterpack");
            s_DD2Ini->setValue("resources", "monsterpack", monsterpack->getSelectedItem().toStdString() );

            tgui::ComboBox::Ptr bonuspack = s_TGUI.get<tgui::ComboBox>("Bonuspack");
            s_DD2Ini->setValue("resources", "bonuspack", bonuspack->getSelectedItem().toStdString() );

            tgui::ComboBox::Ptr screenpack = s_TGUI.get<tgui::ComboBox>("Screenpack");
            s_DD2Ini->setValue("resources", "screenpack", screenpack->getSelectedItem().toStdString() );

            tgui::ComboBox::Ptr soundpack = s_TGUI.get<tgui::ComboBox>("Soundpack");
            s_DD2Ini->setValue("resources", "soundpack", soundpack->getSelectedItem().toStdString() );

            tgui::ComboBox::Ptr musicpack = s_TGUI.get<tgui::ComboBox>("Musicpack");
            s_DD2Ini->setValue("resources", "musicpack", musicpack->getSelectedItem().toStdString() );

            tgui::ComboBox::Ptr davepack = s_TGUI.get<tgui::ComboBox>("Davepack");
            s_DD2Ini->setValue("resources", "davepack", (sf::String)davepack->getSelectedItem());

            tgui::ComboBox::Ptr guipack = s_TGUI.get<tgui::ComboBox>("Guipack");
            s_DD2Ini->setValue("resources", "guipack", guipack->getSelectedItem().toStdString() );

            prs.writeParsedToFile(s_DD2Ini, "DD2.ini");
        }
        void addKeysButtons(PostParsingStruct* pps, tgui::ScrollablePanel::Ptr layout, string prefix, string subblock = "")
        {
            int x_start = 15, y_shift = 35;
            int i = 0;
            map<string, map<string, string> >::iterator iter;
            map<string, string>::iterator iter1;
            for(iter = pps->getMapVariables().begin(); iter != pps->getMapVariables().end(); iter++, i++)
            {
                if(subblock == "" || iter->first == subblock)
                {
                    auto label = tgui::Label::create();
                    label->setText(iter->first);
                    label->setPosition(5, i * y_shift);
                    label->setTextSize(25);
                    layout->add(label);
                    i++;
                    i++;
                    for(iter1 = iter->second.begin(); iter1 != iter->second.end(); iter1++, i++)
                    {
                        auto label = tgui::Button::create();
                        label->setText(iter1->first);
                        label->setPosition(x_start, i * y_shift);
                        label->setTextSize(15);
                        layout->add(label);

                        auto editBox = tgui::EditBox::create();
                        editBox->setTextSize(18);
                        editBox->setPosition(250, i * y_shift);
                        editBox->setText(iter1->second);
                        editBox->setDefaultText("Empty");
                        layout->add(editBox);
                        editBox->setWidgetName(prefix + "/" + iter->first + "/" + iter1->first);

                        label->onPress([=](){
                                           auto panel = tgui::Panel::create();
                                           panel->setSize("20%", "10%");
                                           panel->setPosition("40%", "45%");
                                           s_TGUI.add(panel);

                                           auto label_press_key = tgui::Label::create();
                                           label_press_key->setText("Press key...");
                                           label_press_key->setTextSize(15);
                                           label_press_key->setPosition("20%", "40%");
                                           panel->add(label_press_key);

                                           this->s_KeyEditBox = editBox;
                                           this->s_KeyWaitPanel = panel;
                                       });
                    }
                }
            }
            auto label = tgui::Label::create();
            label->setText("");
            label->setPosition(15, i * 30);
            label->setTextSize(15);
            layout->add(label);
        }
        void addTextsEditBoxes(PostParsingStruct* pps, tgui::ScrollablePanel::Ptr layout, string prefix)
        {
            int x_start = 15, y_shift = 30;
            int i = 0;
            map<string, map<string, string> >::iterator iter;
            map<string, string>::iterator iter1;
            for(iter = pps->getMapVariables().begin(); iter != pps->getMapVariables().end(); iter++, i++)
            {
                auto label = tgui::Label::create();
                label->setText(iter->first);
                label->setPosition(5, i * y_shift);
                label->setTextSize(25);
                layout->add(label);
                i++;
                i++;
                for(iter1 = iter->second.begin(); iter1 != iter->second.end(); iter1++, i++)
                {
                    auto label = tgui::Label::create();
                    label->setText(iter1->first);
                    label->setPosition(x_start, i * y_shift);
                    label->setTextSize(15);
                    layout->add(label);

                    auto editBox = tgui::EditBox::create();
                    editBox->setTextSize(18);
                    editBox->setPosition(250, i * y_shift);
                    editBox->setText(iter1->second);
                    editBox->setDefaultText("Empty");
                    layout->add(editBox);
                    editBox->setWidgetName(prefix + "/" + iter->first + "/" + iter1->first);
                }
            }
            auto label = tgui::Label::create();
            label->setText("");
            label->setPosition(15, i * 30);
            label->setTextSize(15);
            layout->add(label);
        }
        void keys()
        {
            auto child = tgui::Panel::create();
            child->setSize("100%", "100%");
            s_TGUI.add(child);

            auto layout = tgui::ScrollablePanel::create();
            layout->setSize("40%", "100%");
            layout->setPosition("30%", "0%");
            layout->setVerticalScrollAmount(40);
            child->add(layout);

            addKeysButtons(s_DD2Ini, layout, "ini", "keys");

            auto button = tgui::Button::create();
            button->setPosition("85%", "5%");
            button->setText("Save");
            button->setSize("10%", "5%");
            //button->connect("pressed", [=](){ child->setVisible(false); });
            button->onPress([=]()
                            {
                                vector<tgui::Widget::Ptr> wdgs = layout->getWidgets();
                                tgui::EditBox::Ptr tmp_ed_box;
                                for(int i = 0; i < wdgs.size(); i++)
                                {
                                    if(wdgs[i]->getWidgetType() == "EditBox")
                                    {
                                        tmp_ed_box = wdgs[i]->cast<tgui::EditBox>();
                                        string str = wdgs[i]->getWidgetName().toStdString();
                                        map<int, string> spl;
                                        int end_mas = WorkFunctions::ParserFunctions::splitMassString(&spl, 0, 0, str, "/");
                                        if(spl[0] == "ini") s_DD2Ini->setValue(spl[1], spl[2], tmp_ed_box->getText().toStdString());
                                    }
                                }
                                ParserInfoFile prs;
                                prs.writeParsedToFile(s_DD2Ini, "DD2.ini");
                                s_TGUI.remove(child);
                            });
            child->add(button);

            button = tgui::Button::create();
            button->setPosition("85%", "12%");
            button->setText("Close");
            button->setSize("10%", "5%");
            //button->connect("pressed", [=](){ s_TGUI.remove(child); });
            button->onPress([=](){ s_TGUI.remove(child); });
            child->add(button);
        }
        void editAllVariables()
        {
            //auto child = tgui::ChildWindow::create();
            auto child = tgui::Panel::create();
            child->setSize("100%", "100%");
            //child->setPosition(0, 0);
            //child->setTitle("Edit all variables");
            //child->setPositionLocked(true);
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
            button->onPress([=]()
                            {
                                s_DD2Ini->clear();
                                s_DD2General->clear();
                                vector<tgui::Widget::Ptr> wdgs = layout->getWidgets();
                                tgui::EditBox::Ptr tmp_ed_box;
                                for(int i = 0; i < wdgs.size(); i++)
                                {
                                    if(wdgs[i]->getWidgetType() == "EditBox")
                                    {
                                        tmp_ed_box = wdgs[i]->cast<tgui::EditBox>();
                                        string str = wdgs[i]->getWidgetName().toStdString();
                                        map<int, string> spl;
                                        int end_mas = WorkFunctions::ParserFunctions::splitMassString(&spl, 0, 0, str, "/");
                                        if(spl[0] == "ini") s_DD2Ini->setValue(spl[1], spl[2], tmp_ed_box->getText().toStdString());
                                        else if(spl[0] == "gen") s_DD2General->setValue(spl[1], spl[2], tmp_ed_box->getText().toStdString());
                                    }
                                }
                                wdgs = layout_gen->getWidgets();
                                for(int i = 0; i < wdgs.size(); i++)
                                {
                                    if(wdgs[i]->getWidgetType() == "EditBox")
                                    {
                                        tmp_ed_box = wdgs[i]->cast<tgui::EditBox>();
                                        string str = wdgs[i]->getWidgetName().toStdString();
                                        map<int, string> spl;
                                        int end_mas = WorkFunctions::ParserFunctions::splitMassString(&spl, 0, 0, str, "/");
                                        if(spl[0] == "ini") s_DD2Ini->setValue(spl[1], spl[2], tmp_ed_box->getText().toStdString());
                                        else if(spl[0] == "gen") s_DD2General->setValue(spl[1], spl[2], tmp_ed_box->getText().toStdString());
                                    }
                                }
                                ParserInfoFile prs;
                                prs.writeParsedToFile(s_DD2Ini, "DD2.ini");
                                prs.writeParsedToFile(s_DD2General, "General.ini");
                                this->loadData();
                            });
            child->add(button);

            button = tgui::Button::create();
            button->setPosition("85%", "12%");
            button->setText("Close");
            button->setSize("10%", "5%");
            //button->connect("pressed", [=](){ s_TGUI.remove(child); });
            button->onPress([=](){ s_TGUI.remove(child); });
            child->add(button);
        }
        void loadWidgets()
        {
            s_TGUI.removeAllWidgets();
            s_TGUI.loadWidgetsFromFile(s_GuiFile);

            tgui::Button::Ptr resetdefault = s_TGUI.get<tgui::Button>("ResetToDefault");
            //resetdefault->connect("pressed", Launcher::resetToDefault, this);
            resetdefault->onPress(Launcher::resetToDefault, this);

            tgui::Button::Ptr saveconfig = s_TGUI.get<tgui::Button>("SaveConfig");
            //saveconfig->connect("pressed", Launcher::saveConfig, this);
            saveconfig->onPress(Launcher::saveConfig, this);

            tgui::Button::Ptr savelaunchgame = s_TGUI.get<tgui::Button>("SaveLaunchGame");
            //savelaunchgame->connect("pressed", Launcher::saveLaunchGame, this);
            savelaunchgame->onPress(Launcher::saveLaunchGame, this);

            tgui::Button::Ptr exitbutton = s_TGUI.get<tgui::Button>("Exit");
            //exitbutton->connect("pressed", [=](){ s_Window->close(); });
            exitbutton->onPress([=](){ s_Window->close(); });

            tgui::Button::Ptr EdAllVar = s_TGUI.get<tgui::Button>("EdAllVar");
            //EdAllVar->connect("pressed", Launcher::editAllVariables, this);
            EdAllVar->onPress(Launcher::editAllVariables, this);

            tgui::Button::Ptr Keys = s_TGUI.get<tgui::Button>("Keys");
            Keys->onPress(Launcher::keys, this);
        }
        void setDifficulty(const tgui::String& difvalue)
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
                textdif->setText(textsdif[difvalue.toStdString()]);
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

            tgui::ComboBox::Ptr guipack = s_TGUI.get<tgui::ComboBox>("Guipack");
            vector<string> guipacks = getDirs("PacksData/GuiPacks", "gui.info");
            for(unsigned int i = 0; i < guipacks.size(); i++) guipack->addItem(guipacks[i]);
            if(s_DD2Ini->getValue("resources", "guipack") != "") guipack->setSelectedItem(s_DD2Ini->getValue("resources", "guipack"));
            else guipack->setSelectedItem("");

            tgui::Tabs::Ptr dif = s_TGUI.get<tgui::Tabs>("Difficulty");
            setDifficulty("Normal");
            //dif->connect("TabSelected", Launcher::setDifficulty, this);
            dif->onTabSelect(Launcher::setDifficulty, this);
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
                //button->connect("pressed", [=](){ s_TGUI.remove(child); });
                button->onPress([=](){ s_TGUI.remove(child); });
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
        //For keys
        tgui::EditBox::Ptr s_KeyEditBox;
        tgui::Panel::Ptr s_KeyWaitPanel;
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

    //tgui::Theme::setDefault("Launcher/themes/BabyBlue.txt");

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

    bool not_pass_event_to_tgui = false;

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
                //ln.s_TGUI.setView(window.getView());
                //window.setView(sf::View(sf::FloatRect(0.f, 0.f, static_cast<float>(event.size.width), static_cast<float>(event.size.height))));
                //rtgui.create(static_cast<float>(event.size.width), static_cast<float>(event.size.height));
                //ln.s_TGUI.setAbsoluteView(tgui::FloatRect(0.f, 0.f, static_cast<float>(event.size.width), static_cast<float>(event.size.height)));
                //ln.s_TGUI.setAbsoluteView(tgui::FloatRect(0.f, 0.f, static_cast<float>(event.size.width), static_cast<float>(event.size.height)));
                //const float windowHeight = ln.s_TGUI.getView().getRect().height;
                //ln.s_TGUI.setTextSize(static_cast<unsigned int>(0.07f * windowHeight));
            }
            else if(event.type == sf::Event::KeyPressed)
            {
                if(ln.s_KeyEditBox != 0 && ln.s_KeyWaitPanel != 0)
                {
                    int key_code = static_cast<int>(event.key.code);
                    tgui::String tmp_str;
                    tmp_str = WorkFunctions::ConvertFunctions::itos(key_code);
                    ln.s_KeyEditBox->setText(tmp_str);
                    ln.s_KeyEditBox = 0;
                    ln.s_KeyWaitPanel->getParent()->remove(ln.s_KeyWaitPanel);
                    ln.s_KeyWaitPanel = 0;
                    not_pass_event_to_tgui = true;
                }
            }

            if(!not_pass_event_to_tgui) ln.s_TGUI.handleEvent(event);
            not_pass_event_to_tgui = false;
        }

        rtgui.clear(sf::Color(255, 255, 255, 255));
        ln.s_TGUI.draw();
        rtgui.display();

        const sf::Texture& guitexture = rtgui.getTexture();

        // draw it to the window
        sf::Sprite guisprite(guitexture);
        //guisprite.setScale(sf::Vector2f((float)window.getSize().x / 1280, (float)window.getSize().y / 720));

        //fix(posguisprite, ln.s_TGUI, window);

        window.clear();
        window.draw(guisprite);
        window.display();
    }
}
