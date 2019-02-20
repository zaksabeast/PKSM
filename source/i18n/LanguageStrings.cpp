/*
*   This file is part of PKSM
*   Copyright (C) 2016-2019 Bernardo Giordano, Admiral Fish, piepie62
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*   Additional Terms 7.b and 7.c of GPLv3 apply to this file:
*       * Requiring preservation of specified reasonable legal notices or
*         author attributions in that material or in the Appropriate Legal
*         Notices displayed by works containing it.
*       * Prohibiting misrepresentation of the origin of that material,
*         or requiring that modified versions of such material be marked in
*         reasonable ways as different from the original version.
*/

#include "LanguageStrings.hpp"

static nlohmann::json& formJson()
{
    static nlohmann::json forms;
    static bool first = true;
    if (first)
    {
        std::ifstream in("romfs:/i18n/forms.json");
        if (in.good())
        {
            in >> forms;
        }
        in.close();
        first = false;
    }
    return forms;
}

std::string LanguageStrings::folder(Language lang) const
{
    switch (lang)
    {
        case Language::EN: return "en";
        case Language::ES: return "es";
        case Language::DE: return "de";
        case Language::FR: return "fr";
        case Language::IT: return "it";
        case Language::JP: return "jp";
        case Language::KO: return "ko";
        case Language::NL: return "nl";
        case Language::PT: return "pt";
        case Language::ZH: return "zh";
        case Language::TW: return "tw";
        default: return "en";
    }

    return "en";
}

LanguageStrings::LanguageStrings(Language lang)
{
    load(lang, "/abilities.txt", abilities);
    load(lang, "/balls.txt", balls);
    load(lang, "/forms.txt", forms);
    load(lang, "/hp.txt", hps);
    load(lang, "/items.txt", items);
    load(lang, "/moves.txt", moves);
    load(lang, "/natures.txt", natures);
    load(lang, "/species.txt", speciess);
    load(lang, "/games.txt", games);
    loadMap(lang, "/locations4.txt", locations4);
    loadMap(lang, "/locations5.txt", locations5);
    loadMap(lang, "/locations6.txt", locations6);
    loadMap(lang, "/locations7.txt", locations7);
    loadMap(lang, "/locationsLGPE.txt", locationsLGPE);
    loadGui(lang);
}

void LanguageStrings::load(Language lang, const std::string name, std::vector<std::string>& array)
{
    static const std::string base = "romfs:/i18n/";
    std::string path = io::exists(base + folder(lang) + name) ? base + folder(lang) + name : base + folder(Language::EN) + name;
    
    std::string tmp;
    std::ifstream values(path);
    if (values.bad()) return;
    while (std::getline(values, tmp))
    {
        array.push_back(tmp.substr(0, tmp.find('\r')));
    }
    values.close();
}

void LanguageStrings::loadMap(Language lang, const std::string name, std::unordered_map<u16, std::string>& map)
{
    static const std::string base = "romfs:/i18n/";
    std::string path = io::exists(base + folder(lang) + name) ? base + folder(lang) + name : base + folder(Language::EN) + name;

    std::string tmp;
    std::ifstream values(path);
    if (values.bad()) return;
    while (std::getline(values, tmp))
    {
        u16 val = std::stoi(tmp.substr(0, 4), 0, 16);
        map[val] = tmp.substr(5);
    }
}

void LanguageStrings::loadGui(Language lang)
{
    static const std::string base = "romfs:/i18n/";
    std::string path = io::exists(base + folder(lang) + "/gui.json") ? base + folder(lang) + "/gui.json" : base + folder(Language::EN) + "/gui.json";

    std::ifstream values(path);
    gui << values;
    values.close();
}

const std::string& LanguageStrings::ability(u8 v) const
{
    return v < abilities.size() ? abilities.at(v) : localize("INVALID_ABILITY");
}

const std::string& LanguageStrings::ball(u8 v) const
{
    return v < balls.size() ? balls.at(v) : localize("INVALID_BALL");
}

const std::string& LanguageStrings::form(u16 species, u8 form, Generation generation) const
{
    std::string sSpecies = std::to_string((int)species);
    if (formJson().find(sSpecies) == formJson().end())
    {
        // Not sure how the json sorts it, so just do a linear search. Aren't that many (only 44) anyways
        for (int i : formJson()["megas"])
        {
            if (i == species)
            {
                if (form == 1)
                {
                    return forms.at(146);
                }
                else if (form == 0)
                {
                    return forms.at(0);
                }
                else
                {
                    return localize("INVALID_FORM");
                }
            }
        }
    }
    else
    {
        std::vector<int> formIndices;
        if (formJson()[sSpecies].is_object())
        {
            if (formJson()[sSpecies].find(genToString(generation)) != formJson()[sSpecies].end())
            {
                formIndices = formJson()[sSpecies][genToString(generation)].get<std::vector<int>>();
            }
        }
        else
        {
            formIndices = formJson()[sSpecies].get<std::vector<int>>();
        }

        if (form < formIndices.size())
        {
            size_t formNameIndex = formIndices[form];
            if (formNameIndex < forms.size())
            {
                return forms.at(formNameIndex);
            }
        }
    }
    return localize("INVALID_FORM");
}

const std::string& LanguageStrings::hp(u8 v) const
{
    return v < hps.size() ? hps.at(v) : localize("INVALID_HP");
}

const std::string& LanguageStrings::item(u16 v) const
{
    return v < items.size() ? items.at(v) : localize("INVALID_ITEM");
}

const std::string& LanguageStrings::move(u16 v) const
{
    return v < moves.size() ? moves.at(v) : localize("INVALID_MOVE");
}

const std::string& LanguageStrings::nature(u8 v) const
{
    return v < natures.size() ? natures.at(v) : localize("INVALID_NATURE");
}

const std::string& LanguageStrings::species(u16 v) const
{
    return v < speciess.size() ? speciess.at(v) : localize("INVALID_SPECIES");
}

const std::string& LanguageStrings::localize(const std::string& v) const
{
    static std::string MISSING = "MISSING: ";
    if (MISSING != "MISSING: ")
    {
        MISSING = "MISSING: ";
    }
    if (gui.count(v))
    {
        return gui.at(v).get_ref<const std::string&>();
    }
    else
    {
        MISSING += v;
        return MISSING;
    }
}

const std::vector<std::string>& LanguageStrings::rawItems() const
{
    return items;
}

const std::vector<std::string>& LanguageStrings::rawMoves() const
{
    return moves;
}

const std::string& LanguageStrings::location(u16 v, Generation generation) const
{
    std::unordered_map<u16, std::string>::const_iterator i;
    switch (generation)
    {
        case Generation::FOUR:
            if ((i = locations4.find(v)) != locations4.end())
            {
                return i->second;
            }
            break;
        case Generation::FIVE:
            if ((i = locations5.find(v)) != locations5.end())
            {
                return i->second;
            }
            break;
        case Generation::SIX:
            if ((i = locations6.find(v)) != locations6.end())
            {
                return i->second;
            }
            break;
        case Generation::SEVEN:
            if ((i = locations7.find(v)) != locations7.end())
            {
                return i->second;
            }
            break;
        case Generation::LGPE:
            if ((i = locationsLGPE.find(v)) != locationsLGPE.end())
            {
                return i->second;
            }
            break;
        default:
            break;
    }
    return localize("INVALID_LOCATION");
}

const std::string& LanguageStrings::game(u8 v) const
{
    if (v < games.size() && games.at(v) != "")
    {
        return games.at(v);
    }
    return localize("INVALID_GAME");
}
