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

#ifndef SORTSELECTIONSCREEN_HPP
#define SORTSELECTIONSCREEN_HPP

#include "SelectionScreen.hpp"
#include "HidVertical.hpp"

enum SortType
{
    NONE,
    DEX,
    SPECIESNAME,
    FORM,
    TYPE1,
    TYPE2,
    HP,
    ATK,
    DEF,
    SATK,
    SDEF,
    SPE,
    HPIV,
    ATKIV,
    DEFIV,
    SATKIV,
    SDEFIV,
    SPEIV,
    NATURE,
    LEVEL,
    TID,
    HIDDENPOWER,
    FRIENDSHIP,
    NICKNAME,
    OTNAME,
    SHINY
};

static constexpr std::string_view sortTypeToString(SortType type)
{
    switch (type)
    {
        case NONE:
        default:
            return "NONE";
        case DEX:
            return "SPECIES";
        case FORM:
            return "FORM";
        case TYPE1:
            return "TYPE1";
        case TYPE2:
            return "TYPE2";
        case HP:
            return "HP";
        case ATK:
            return "ATTACK";
        case DEF:
            return "DEFENSE";
        case SATK:
            return "SPATK";
        case SDEF:
            return "SPDEF";
        case SPE:
            return "SPEED";
        case HPIV:
            return "HP_IV";
        case ATKIV:
            return "ATTACK_IV";
        case DEFIV:
            return "DEFENSE_IV";
        case SATKIV:
            return "SPATK_IV";
        case SDEFIV:
            return "SPDEF_IV";
        case SPEIV:
            return "SPEED_IV";
        case NATURE:
            return "NATURE";
        case LEVEL:
            return "LEVEL";
        case TID:
            return "TID";
        case HIDDENPOWER:
            return "HIDDEN_POWER";
        case FRIENDSHIP:
            return "FRIENDSHIP";
        case NICKNAME:
            return "NICKNAME";
        case SPECIESNAME:
            return "SPECIES_NAME";
        case OTNAME:
            return "OT_NAME";
        case SHINY:
            return "SHINY";
    }
}

class SortSelectionScreen : public Screen
{
public:
    SortSelectionScreen(SortType type) : hid(40, 2), original(type) { hid.update(vals.size()); hid.select(original); }
    SortType run();
    void draw() const override;
    void update(touchPosition* touch) override;
    ScreenType type() const override { return ScreenType::SELECTOR; }
private:
    HidVertical hid;
    mutable bool firstDraw = true;
    bool finished = false;
    SortType original;
    static constexpr std::array<SortType, 26> vals = {
        NONE,
        DEX,
        SPECIESNAME,
        FORM,
        TYPE1,
        TYPE2,
        HP,
        ATK,
        DEF,
        SATK,
        SDEF,
        SPE,
        HPIV,
        ATKIV,
        DEFIV,
        SATKIV,
        SDEFIV,
        SPEIV,
        NATURE,
        LEVEL,
        TID,
        HIDDENPOWER,
        FRIENDSHIP,
        NICKNAME,
        OTNAME,
        SHINY
    };
};

#endif
