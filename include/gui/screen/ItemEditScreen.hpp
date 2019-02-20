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

#ifndef ITEMEDITSCREEN_HPP
#define ITEMEDITSCREEN_HPP

#include "Screen.hpp"
#include "HidVertical.hpp"
#include <vector>
#include <string>
#include "ClickButton.hpp"
#include "gui.hpp"

class ItemEditScreen : public Screen
{
public:
    ItemEditScreen(std::vector<std::pair<const std::string*, int>>& items, size_t selected) : hid(40,2), validItems(items), items(items), origItem(selected)
    {
        searchButton = new ClickButton(75, 30, 170, 23, [this](){ startSearch = true; return false; }, ui_sheet_emulated_box_search_idx, "", 0, 0);
        hid.update(items.size());
        hid.select(selected);
    }
    ~ItemEditScreen()
    {
        delete searchButton;
    }
    int run();
    void draw() const override;
    void update(touchPosition* touch) override;
    ScreenType type() const override { return ITEMEDIT; }
private:
    void searchBar();
    HidVertical hid;
    std::vector<std::pair<const std::string*, int>> validItems;
    std::vector<std::pair<const std::string*, int>>& items;
    bool finished = false;
    mutable bool firstDraw = true;
    int finalVal = 0;
    int origItem;
    bool justSwitched = true;
    std::string searchString = "";
    std::string oldSearchString = "";
    Button* searchButton;
    bool startSearch = false;
};

#endif
