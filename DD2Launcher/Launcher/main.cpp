#include <iostream>

#include <dirent.h>
#include <cstdio>

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

vector<string> getDirs(string path, string filetest, string A = "", string B = "", string C = "")
{
    bool addit_test = true;
    if(A == "" && B == "" && C == "") addit_test = false;
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
            if(!addit_test) dirs.push_back(entry->d_name);
            else
            {
                ParserInfoFile prs;
                PostParsingStruct* pps = prs.getParsedFromFile(path + "/" + fname + "/" + filetest);
                if(pps->isExists(A, B) && pps->getValue(A, B) == C) dirs.push_back(entry->d_name);
                delete pps;
            }

        entry = readdir(dir);
    }

    closedir(dir);

    return dirs;
}

vector<string> getFiles(string path, string ext = "")
{
    vector<string> files;
    const char* PATH = path.c_str();

    DIR *dir = opendir(PATH);

    struct dirent *entry = readdir(dir);

    while (entry != NULL)
    {
        //if (entry->d_type == DT_DIR)

        struct stat buff;
        stat(entry->d_name, &buff);
        string fname = string(entry->d_name);
        if(fname != "." && fname != "..")
        {
            bool adding = true;
            if(ext != "" && !( string(entry->d_name).find("." + ext) != string::npos && string(entry->d_name).find("." + ext) + ext.size() + 1 == string(entry->d_name).size() ) ) adding = false;
            if(adding) files.push_back(entry->d_name);
        }

        entry = readdir(dir);
    }

    closedir(dir);

    return files;
}

class Launcher
{
    public:
        Launcher():
            s_DD2Ini(0),
            s_DD2General(0),
            s_LauncherConfig(0),
            s_GuiFile(""),
            s_Window(0),
            s_DD2FileName(""),
            s_NickName("Player"),
            s_KeyEditBox(0),
            s_KeyWaitPanel(0)
        {
            //...
        }
        ~Launcher()
        {
            delete s_DD2Ini;
            delete s_DD2General;
            delete s_LauncherConfig;
        }
        void resetToDefault(bool easylauncher)
        {
            if(!s_DD2Ini) delete s_DD2Ini;
            if(!s_DD2General) delete s_DD2General;
            ParserInfoFile prs;
            s_DD2Ini = prs.getParsedFromFile("Launcher/DD2Default.ini");
            s_DD2General = prs.getParsedFromFile("Launcher/GeneralDefault.ini");
            loadData(easylauncher);
        }
        void saveConfig(bool easylauncher)
        {
            poolingConfig(easylauncher);
            ParserInfoFile prs;
            if(!s_DD2Ini || !s_DD2General) return;
            prs.writeParsedToFile(s_DD2Ini, "DD2.ini");
            prs.writeParsedToFile(s_DD2General, "General.ini");
        }
        void poolingConfig(bool easylauncher)
        {
            if(!s_DD2Ini) return;

            if(easylauncher)
            {
                auto child = s_TGUI.get<tgui::Panel>("EasyLauncher");

                tgui::Slider::Ptr SoundsSlider = child->get<tgui::Slider>("SoundsSlider");
                string soundsvolume = WorkFunctions::ConvertFunctions::ftos(SoundsSlider->getValue());
                s_DD2Ini->setValue("audio", "soundsvolume", soundsvolume);

                tgui::Slider::Ptr MusicSlider = child->get<tgui::Slider>("MusicSlider");
                string musicvolume = WorkFunctions::ConvertFunctions::ftos(MusicSlider->getValue());
                s_DD2Ini->setValue("audio", "musicvolume", musicvolume);

                tgui::CheckBox::Ptr SoundsCheckBox = child->get<tgui::CheckBox>("SoundsCheckBox");
                if(SoundsCheckBox->isChecked()) s_DD2Ini->setValue("audio", "sounds", "true");
                else s_DD2Ini->setValue("audio", "sounds", "false");

                tgui::CheckBox::Ptr MusicCheckBox = child->get<tgui::CheckBox>("MusicCheckBox");
                if(MusicCheckBox->isChecked()) s_DD2Ini->setValue("audio", "music", "true");
                else s_DD2Ini->setValue("audio", "music", "false");
            }
            else
            {
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

                tgui::ComboBox::Ptr playerpack = s_TGUI.get<tgui::ComboBox>("Playerpack");
                s_DD2Ini->setValue("resources", "playerpack", (sf::String)playerpack->getSelectedItem());

                tgui::ComboBox::Ptr guipack = s_TGUI.get<tgui::ComboBox>("Guipack");
                s_DD2Ini->setValue("resources", "guipack", guipack->getSelectedItem().toStdString() );
            }
        }
        void addKeysButtons(PostParsingStruct* pps, tgui::ScrollablePanel::Ptr layout, string prefix, string subblock = "")
        {
            int x_start = 350, y_shift = 35;
            int i = 1;
            map<string, map<string, string> >::iterator iter;
            map<string, string>::iterator iter1;
            for(iter = pps->getMapVariables().begin(); iter != pps->getMapVariables().end(); iter++, i++)
            {
                if(subblock == "" || iter->first == subblock)
                {
                    /*auto label = tgui::Label::create();
                    label->setText(iter->first);
                    label->setPosition(5, i * y_shift);
                    label->setTextSize(25);
                    layout->add(label);
                    i++;
                    i++;*/
                    for(iter1 = iter->second.begin(); iter1 != iter->second.end(); iter1++, i++)
                    {
                        auto label = tgui::Button::create();
                        label->setText(iter1->first);
                        label->setPosition(x_start, i * y_shift);
                        label->setTextSize(15);
                        layout->add(label);

                        auto editBox = tgui::EditBox::create();
                        editBox->setTextSize(18);
                        editBox->setPosition(x_start + 200, i * y_shift);
                        editBox->setText(iter1->second);
                        editBox->setDefaultText("Empty");
                        layout->add(editBox);
                        editBox->setWidgetName(prefix + "/" + iter->first + "/" + iter1->first);

                        label->onPress([=](){
                                           auto panel = tgui::Panel::create();
                                           panel->setSize("20%", "10%");
                                           panel->setPosition("50%", "43%");
                                           panel->setOrigin(0.5, 0.5);
                                           panel->getRenderer()->setBorders(tgui::Borders(1));
                                           panel->getRenderer()->setBorderColor(tgui::Color(0, 0, 0, 255));
                                           layout->add(panel);

                                           auto label_press_key = tgui::Label::create();
                                           label_press_key->setText("Press key...");
                                           label_press_key->setTextSize(15);
                                           label_press_key->setSize("100%", "100%");
                                           label_press_key->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
                                           label_press_key->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
                                           panel->add(label_press_key);

                                           this->s_KeyEditBox = editBox;
                                           this->s_KeyWaitPanel = panel;
                                       });
                    }
                }
            }
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
            layout->setSize("80%", "100%");
            layout->setVerticalScrollAmount(40);
            child->add(layout);

            auto label = tgui::Label::create();
            label->setText("Keys");
            label->setOrigin(0.5, 0);
            label->setSize("40%", "10%");
            label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
            label->setPosition("50%", "10%");
            label->setTextSize(25);
            layout->add(label);

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
        void loadEasyLauncherWidgets()
        {
            auto childRemove = s_TGUI.get<tgui::Panel>("EasyLauncher");
            if(childRemove) s_TGUI.remove(childRemove);

            auto child = tgui::Panel::create();
            child->setSize("100%", "100%");
            child->setWidgetName("EasyLauncher");
            s_TGUI.add(child);

            child->loadWidgetsFromFile(s_EasyLauncherGuiFile);

            tgui::Button::Ptr resetdefault = child->get<tgui::Button>("ResetToDefault");
            resetdefault->onPress(Launcher::resetToDefault, this, true);

            tgui::Button::Ptr saveconfig = child->get<tgui::Button>("SaveConfig");
            saveconfig->onPress(Launcher::saveConfig, this, true);

            tgui::Button::Ptr exitbutton = child->get<tgui::Button>("Exit");
            exitbutton->onPress([=](){ s_Window->close(); });

            tgui::Button::Ptr Keys = child->get<tgui::Button>("Keys");
            Keys->onPress(Launcher::keys, this);

            tgui::Button::Ptr More = child->get<tgui::Button>("More");
            More->onPress([=](){ this->poolingConfig(true); this->loadData(false); });

            tgui::Button::Ptr ManageSaves = child->get<tgui::Button>("ManageSaves");
            ManageSaves->onPress([=](){
                                        auto saves_management = tgui::Panel::create();
                                        saves_management->setSize("100%", "100%");
                                        saves_management->setPosition(0, 0);
                                        saves_management->setWidgetName("SaveManagement");
                                        child->add(saves_management);

                                        auto label_pl = tgui::Label::create();
                                        label_pl->setText("Saves list:");
                                        label_pl->setPosition("40%", "5%");
                                        label_pl->setSize("20%", "10%");
                                        label_pl->setTextSize(25);
                                        saves_management->add(label_pl);

                                        auto list_saves = tgui::ListBox::create();
                                        list_saves->setSize("51%", "80%");
                                        //list_saves->setItemHeight(32);
                                        list_saves->setTextSize(18);
                                        list_saves->setPosition("20%", "12%");

                                        string saves_folder = "Saves/" + s_NickName + "/";
                                        string save_ext = "ddmesav";
                                        vector<string> files = getFiles(saves_folder, save_ext);
                                        for(int i = 0; i < files.size(); i++) list_saves->addItem(files[i].substr(0, files[i].size() - save_ext.size() - 1));
                                        saves_management->add(list_saves);

                                        auto button = tgui::Button::create();
                                        button->setPosition("80%", "15%");
                                        button->setText("Remove save");
                                        button->setSize("15%", "5%");
                                        button->setTextSize(18);
                                        button->onPress([=]()
                                                        {
                                                            string save_name = list_saves->getSelectedItem().toStdString();
                                                            if(save_name == "") return;
                                                            string save_file_name = saves_folder + save_name + ".ddmesav";

                                                            auto child_window = tgui::Panel::create();
                                                            child_window->setSize("50%", "20%");
                                                            child_window->setPosition("25%", "30%");
                                                            child_window->getRenderer()->setBorders(tgui::Outline(1));
                                                            saves_management->add(child_window);

                                                            auto label = tgui::Label::create();
                                                            label->setText("Do you want delete save \"" + save_name + "\"?");
                                                            label->setSize("90%", "40%");
                                                            label->setPosition("5%", "10%");
                                                            label->setTextSize(20);
                                                            label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
                                                            label->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
                                                            child_window->add(label);

                                                            auto button = tgui::Button::create();
                                                            button->setPosition("10%", "65%");
                                                            button->setTextSize(18);
                                                            button->setText("No");
                                                            button->setSize("30%", "25%");
                                                            button->onPress([=](){ saves_management->remove(child_window); });
                                                            child_window->add(button);

                                                            button = tgui::Button::create();
                                                            button->setPosition("60%", "65%");
                                                            button->setText("Yes");
                                                            button->setTextSize(18);
                                                            button->setSize("30%", "25%");
                                                            button->onPress([=]()
                                                                                {
                                                                                    std::remove(save_file_name.c_str());
                                                                                    list_saves->removeItem(save_name);
                                                                                    saves_management->remove(child_window);
                                                                                }
                                                                            );
                                                            child_window->add(button);
                                                        });
                                        saves_management->add(button);

                                        button = tgui::Button::create();
                                        button->setPosition("80%", "22%");
                                        button->setText("Close");
                                        button->setTextSize(18);
                                        button->setSize("15%", "5%");
                                        button->onPress([=](){ child->remove(saves_management); });
                                        saves_management->add(button);
                                    });

            tgui::Slider::Ptr SoundsSlider = child->get<tgui::Slider>("SoundsSlider");
            SoundsSlider->setValue(stof(s_DD2Ini->getValue("audio", "soundsvolume")));

            tgui::Slider::Ptr MusicSlider = child->get<tgui::Slider>("MusicSlider");
            MusicSlider->setValue(stof(s_DD2Ini->getValue("audio", "musicvolume")));

            tgui::CheckBox::Ptr SoundsCheckBox = child->get<tgui::CheckBox>("SoundsCheckBox");
            if(s_DD2Ini->getValue("audio", "sounds") == "true") SoundsCheckBox->setChecked(true);
            else SoundsCheckBox->setChecked(false);

            tgui::CheckBox::Ptr MusicCheckBox = child->get<tgui::CheckBox>("MusicCheckBox");
            if(s_DD2Ini->getValue("audio", "music") == "true") MusicCheckBox->setChecked(true);
            else MusicCheckBox->setChecked(false);

            tgui::ScrollablePanel::Ptr ModpacksScrollablePanel = child->get<tgui::ScrollablePanel>("ModpacksScrollablePanel");
            int mspX = ModpacksScrollablePanel->getSize().x;
            int mspY = ModpacksScrollablePanel->getSize().y;

            string cur_modpack = "StandardDave";
            if(s_DD2Ini->getValue("resources", "standard") == "false") cur_modpack = s_DD2Ini->getValue("resources", "modpack");

            vector<string> modpacks = getDirs("ModPacks", "/About/mod.info");
            for(unsigned int i = 0; i < modpacks.size(); i++)
                if(cur_modpack == modpacks[i])
                {
                    for(int j = i; j > 0; j--)
                        modpacks[j] = modpacks[j - 1];
                    modpacks[0] = cur_modpack;
                    break;
                }
            float shift_y_pos = 0;
            for(unsigned int i = 0; i < modpacks.size(); i++)
            {
                ParserInfoFile prs;
                string pathtomodpackinfo = "ModPacks/" + modpacks[i] + "/About/";
                PostParsingStruct* pps = prs.getParsedFromFile(pathtomodpackinfo + "mod.info");
                string desc = pps->getValue("info", "desc");
                string name = pps->getValue("info", "name");
                if(name == "") name = "Unnamed";
                if(desc == "") desc = "Empty";
                desc = WorkFunctions::WordFunctions::removeSlashes(desc);
                string logo = pps->getValue("info", "logo");
                tgui::Picture::Ptr modpack_logo_pic;
                auto startmodsettings = [=]()
                {
                    auto child_window = tgui::ChildWindow::create();
                    child_window->setSize("50%", "30%");
                    child_window->setPosition("25%", "30%");
                    child_window->setTitle("Modpack settings");
                    child->add(child_window);

                    auto label = tgui::Label::create();
                    label->setText("In progress...");
                    label->setSize("100%", "85%");
                    label->setPosition("0%", "15%");
                    label->setTextSize(25);
                    label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
                    child_window->add(label);

                    auto button = tgui::Button::create();
                    button->setPosition("35%", "70%");
                    button->setText("Ok");
                    button->setSize("30%", "20%");
                    button->onPress([=](){ child->remove(child_window); });
                    child_window->add(button);
                };

                auto startmodpack = [=]()
                {
                    auto child_window = tgui::Panel::create();
                    child_window->setSize("50%", "50%");
                    child_window->setPosition("25%", "20%");
                    child_window->getRenderer()->setBorders(tgui::Outline(1));
                    child->add(child_window);

                    auto label = tgui::Label::create();
                    label->setText("Do you want to launch the game with the \"" + name + "\" modpack?");
                    label->setSize("90%", "30%");
                    label->setPosition("5%", "10%");
                    label->setTextSize(25);
                    label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
                    child_window->add(label);

                    vector<string> levelpacks = getDirs("PacksData/LevelPacks", "levels.dat", "info", "modpack", modpacks[i]);

                    label = tgui::Label::create();
                    label->setText("Choose levelpack (" + WorkFunctions::ConvertFunctions::itos(levelpacks.size()) + " TA) (empty equals default):");
                    label->setSize("90%", "10%");
                    label->setPosition("5%", "45%");
                    label->setTextSize(20);
                    label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
                    child_window->add(label);

                    auto combobox = tgui::ComboBox::create();
                    combobox->setSize("70%", "10%");
                    combobox->setPosition("15%", "55%");
                    combobox->setTextSize(20);
                    combobox->addItem("");
                    combobox->setItemsToDisplay(10);

                    for(unsigned int i = 0; i < levelpacks.size(); i++) combobox->addItem(levelpacks[i]);
                    if(s_DD2Ini->getValue("resources", "levelpack") != "" && combobox->contains(s_DD2Ini->getValue("resources", "levelpack"))) combobox->setSelectedItem(s_DD2Ini->getValue("resources", "levelpack"));
                    else combobox->setSelectedItem("");

                    combobox->setChangeItemOnScroll(true);
                    child_window->add(combobox);

                    auto button = tgui::Button::create();
                    button->setPosition("10%", "80%");
                    button->setText("No");
                    button->setSize("30%", "10%");
                    button->onPress([=](){ child->remove(child_window); });
                    child_window->add(button);

                    button = tgui::Button::create();
                    button->setPosition("60%", "80%");
                    button->setText("Yes");
                    button->setSize("30%", "10%");
                    button->onPress([=]()
                                    {
                                        if(modpacks[i] == "StandardDave")
                                        {
                                            this->s_DD2Ini->setValue("resources", "standard", "true");
                                            this->s_DD2Ini->setValue("resources", "modpack", "");
                                        }
                                        else
                                        {
                                            this->s_DD2Ini->setValue("resources", "standard", "false");
                                            this->s_DD2Ini->setValue("resources", "modpack", modpacks[i]);
                                        }
                                        string levelpack = combobox->getSelectedItem().toStdString();
                                        if(levelpack != "")
                                        {
                                            this->s_DD2Ini->setValue("resources", "pooling", "true");
                                            this->s_DD2Ini->setValue("resources", "levelpack", levelpack);
                                        }
                                        else
                                        {
                                            this->s_DD2Ini->setValue("resources", "levelpack", "");
                                        }
                                        this->saveLaunchGame(true);
                                    }
                                    );
                    child_window->add(button);
                };

                auto modpack_settings_pic = tgui::Picture::create("Launcher/Settings_Icon.png");
                modpack_settings_pic->setOrigin(0, 0.5);
                modpack_settings_pic->onClick(startmodsettings);

                if(logo != "") modpack_logo_pic = tgui::Picture::create( (pathtomodpackinfo + logo).c_str() );
                else
                {
                    modpack_logo_pic = tgui::Picture::create();
                    modpack_logo_pic->setSize(10, 10);
                }

                modpack_logo_pic->setOrigin(0, 0.5);
                modpack_logo_pic->onClick(startmodpack);
                int x_temp = modpack_logo_pic->getSize().x;
                int y_temp = modpack_logo_pic->getSize().y;
                int perc_for_x = 30;
                float one_perc = float(x_temp) / perc_for_x;
                float perc_for_y = y_temp / one_perc;
                perc_for_y = perc_for_y * float(mspX) / float(mspY);
                modpack_logo_pic->setSize( (WorkFunctions::ConvertFunctions::itos(perc_for_x) + "%").c_str(), (WorkFunctions::ConvertFunctions::ftos(perc_for_y) + "%").c_str() );

                if(perc_for_y < 30) perc_for_y = 30;

                auto modpack_name_label = tgui::Label::create();
                modpack_name_label->setText(name);
                float perc_for_name_y = 12;
                modpack_name_label->setSize("52%", (WorkFunctions::ConvertFunctions::ftos(perc_for_name_y) + "%").c_str() );
                modpack_name_label->setPosition("45%", (WorkFunctions::ConvertFunctions::ftos(shift_y_pos) + "%").c_str() );
                modpack_name_label->setTextSize(25);
                modpack_name_label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
                modpack_name_label->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
                modpack_name_label->getRenderer()->setTextColor(tgui::Color(43, 224, 233, 255));
                modpack_name_label->onClick(startmodpack);
                ModpacksScrollablePanel->add(modpack_name_label);

                tgui::Label::Ptr modpackdesclabel = tgui::Label::create();
                modpackdesclabel->setSize("52%", (WorkFunctions::ConvertFunctions::ftos(perc_for_y - perc_for_name_y - 1) + "%").c_str());
                modpackdesclabel->setPosition("45%", (WorkFunctions::ConvertFunctions::ftos(shift_y_pos + perc_for_name_y + 1) + "%").c_str() );
                modpackdesclabel->setText(desc);
                modpackdesclabel->setTextSize(15);
                modpackdesclabel->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
                modpackdesclabel->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
                modpackdesclabel->onClick(startmodpack);
                ModpacksScrollablePanel->add(modpackdesclabel);

                string pos_y = WorkFunctions::ConvertFunctions::ftos(shift_y_pos + perc_for_y / 2) + "%";

                modpack_logo_pic->setPosition("10%", pos_y.c_str());
                modpack_settings_pic->setPosition("0%", pos_y.c_str());

                ModpacksScrollablePanel->add(modpack_logo_pic);
                ModpacksScrollablePanel->add(modpack_settings_pic);

                shift_y_pos = shift_y_pos + perc_for_y + 5;
                delete pps;
            }

            /*tgui::Button::Ptr savelaunchgame = s_TGUI.get<tgui::Button>("SaveLaunchGame");
            savelaunchgame->onPress(Launcher::saveLaunchGame, this);*/
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
            button->setText("Apply");
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
                                /*ParserInfoFile prs;
                                prs.writeParsedToFile(s_DD2Ini, "DD2.ini");
                                prs.writeParsedToFile(s_DD2General, "General.ini");*/
                                this->loadData(false);
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
            resetdefault->onPress(Launcher::resetToDefault, this, false);

            tgui::Button::Ptr saveconfig = s_TGUI.get<tgui::Button>("SaveConfig");
            //saveconfig->connect("pressed", Launcher::saveConfig, this);
            saveconfig->onPress(Launcher::saveConfig, this, false);

            tgui::Button::Ptr savelaunchgame = s_TGUI.get<tgui::Button>("SaveLaunchGame");
            //savelaunchgame->connect("pressed", Launcher::saveLaunchGame, this);
            savelaunchgame->onPress(Launcher::saveLaunchGame, this, false);

            tgui::Button::Ptr exitbutton = s_TGUI.get<tgui::Button>("Exit");
            //exitbutton->connect("pressed", [=](){ s_Window->close(); });
            exitbutton->onPress([=](){ s_Window->close(); });

            tgui::Button::Ptr EdAllVar = s_TGUI.get<tgui::Button>("EdAllVar");
            //EdAllVar->connect("pressed", Launcher::editAllVariables, this);
            EdAllVar->onPress([=](){ this->poolingConfig(false); this->editAllVariables(); });

            tgui::Button::Ptr Keys = s_TGUI.get<tgui::Button>("Keys");
            Keys->onPress(Launcher::keys, this);

            tgui::Button::Ptr Editor = s_TGUI.get<tgui::Button>("Editor");
            Editor->onPress(Launcher::editor, this);

            tgui::Button::Ptr EditorSettings = s_TGUI.get<tgui::Button>("EditorSettings");
            EditorSettings->onPress(Launcher::editorSettings, this, false);

            tgui::Button::Ptr Back = s_TGUI.get<tgui::Button>("Back");
            Back->onPress([=](){ this->poolingConfig(false); this->loadEasyLauncherWidgets(); });
        }
        void editorSettings(bool error_path = true)
        {
            string PathToTiledExe = s_LauncherConfig->getValue("general", "path_to_tiled_exe");
            auto panel = tgui::Panel::create();
            panel->setSize("40%", "30%");
            panel->setOrigin(0.5, 0.5);
            panel->setPosition("50%", "43%");
            panel->getRenderer()->setBorders(tgui::Borders(1));
            panel->getRenderer()->setBorderColor(tgui::Color(0, 0, 0, 255));
            s_TGUI.add(panel);

            auto label_press_key = tgui::Label::create();
            label_press_key->setTextSize(15);
            label_press_key->setPosition("0%", "10%");
            if(error_path) label_press_key->setText("Tiled editor not found!");
            else label_press_key->setText("Editor settings:");
            label_press_key->setSize("100%");
            label_press_key->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
            panel->add(label_press_key);

            auto editBox = tgui::EditBox::create();
            editBox->setTextSize(15);
            editBox->setOrigin(0.5, 0);
            editBox->setPosition("50%", "30%");
            editBox->setSize("90%", "15%");
            editBox->setText(PathToTiledExe);
            editBox->setAlignment(tgui::EditBox::Alignment::Center);
            editBox->setDefaultText("Choose tiled.exe");
            panel->add(editBox);

            auto button = tgui::Button::create();
            button->setOrigin(0.5, 0);
            button->setPosition("50%", "53%");
            button->setText("Choose file");
            button->onPress([=](){
                            auto openFileDialog = tgui::FileDialog::create("Choose file", "Open");
                            openFileDialog->onFileSelect([=](){ if(openFileDialog->getSelectedPaths().size() > 0) editBox->setText(openFileDialog->getSelectedPaths()[0].asString()); });
                            s_TGUI.add(openFileDialog);
                            });
            panel->add(button);

            button = tgui::Button::create();
            button->setSize("15%");
            button->setOrigin(0.5, 0);
            button->setPosition("40%", "75%");
            button->setText("Save");
            button->onPress([=](){
                            s_LauncherConfig->setValue("general", "path_to_tiled_exe", editBox->getText().toStdString());
                            ParserInfoFile prs;
                            prs.writeParsedToFile(s_LauncherConfig, "Launcher/Launcher.ini");
                            s_TGUI.remove(panel);
                            });
            panel->add(button);

            button = tgui::Button::create();
            button->setSize("15%");
            button->setOrigin(0.5, 0);
            button->setPosition("60%", "75%");
            button->setText("Close");
            button->onPress([=](){ s_TGUI.remove(panel); });
            panel->add(button);
        }
        void editor()
        {
            string PathToTiledExe = s_LauncherConfig->getValue("general", "path_to_tiled_exe");
            if(WorkFunctions::FileFunctions::isFileExists(PathToTiledExe))
            {
                string disk_letter_tiled = PathToTiledExe.substr(0, PathToTiledExe.find("/"));
                string folder_tiled = PathToTiledExe.substr(0, PathToTiledExe.find_last_of("/") + 1);
                string tiled_exe_name = PathToTiledExe.substr(PathToTiledExe.find_last_of("/") + 1);
                string userprofile_set = "%cd%/Launcher/ForTiled";
                string template_path = "%cd%/Launcher/ForTiled/Levels/template.lev";
                system( ("set userprofile=" + userprofile_set + "&& " + disk_letter_tiled + " && cd \"" + folder_tiled + "\" && start " + tiled_exe_name + " " + template_path).c_str() );
                s_Window->close();
            }
            else
            {
                editorSettings();
            }
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
        void loadData(bool easylauncher)
        {
            if(!s_DD2Ini || s_GuiFile == "") return;

            if(easylauncher) loadEasyLauncherWidgets();
            else
            {
                loadWidgets();

                tgui::CheckBox::Ptr stnd = s_TGUI.get<tgui::CheckBox>("Standard");
                if(s_DD2Ini->getValue("resources", "standard") == "true") stnd->setChecked(true);
                else stnd->setChecked(false);

                tgui::CheckBox::Ptr pooling = s_TGUI.get<tgui::CheckBox>("Pooling");
                if(s_DD2Ini->getValue("resources", "pooling") == "true") pooling->setChecked(true);
                else pooling->setChecked(false);

                tgui::ComboBox::Ptr modpack = s_TGUI.get<tgui::ComboBox>("Modpack");
                vector<string> modpacks = getDirs("ModPacks", "/About/mod.info");
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

                tgui::ComboBox::Ptr playerpack = s_TGUI.get<tgui::ComboBox>("Playerpack");
                vector<string> playerpacks = getDirs("PacksData/PlayerPacks", "player.dat");
                for(unsigned int i = 0; i < playerpacks.size(); i++) playerpack->addItem(playerpacks[i]);
                if(s_DD2Ini->getValue("resources", "playerpack") != "") playerpack->setSelectedItem(s_DD2Ini->getValue("resources", "playerpack"));
                else playerpack->setSelectedItem("");

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
        }
        void saveLaunchGame(bool easylauncher)
        {
            saveConfig(easylauncher);
            if(s_DD2Ini->getValue("resources", "standard") == "false" && s_DD2Ini->getValue("resources", "modpack") == "" && s_DD2Ini->getValue("resources", "pooling") == "false")
            {
                auto child = tgui::ChildWindow::create();
                child->setSize("40%", "30%");
                child->setPosition("30%", "30%");
                child->setTitle("Error launch game!");
                s_TGUI.add(child);

                auto label = tgui::Label::create();
                label->setText("Standard and Pooling variables can not be false at the same time with empty modpack variable!");
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
            system(("start " + s_DD2FileName + " nolauncher").c_str());
            s_Window->close();
        }
        PostParsingStruct* s_DD2Ini;
        PostParsingStruct* s_DD2General;
        PostParsingStruct* s_LauncherConfig;
        string s_GuiFile;
        string s_EasyLauncherGuiFile;
        sf::RenderWindow* s_Window;
        tgui::Gui s_TGUI;
        string s_DD2FileName;
        string s_NickName;
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
    ln.s_EasyLauncherGuiFile = "Launcher/EasyLauncherDD2.gui";

    ln.loadWidgets();

    ln.loadData(true);

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
