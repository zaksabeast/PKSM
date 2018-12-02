/*
*   This file is part of PKSM
*   Copyright (C) 2016-2018 Bernardo Giordano, Admiral Fish, piepie62
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

#ifndef BAGSCREEN_HPP
#define BAGSCREEN_HPP

#include "Screen.hpp"
#include "Sav.hpp"
#include "Button.hpp"
#include "ItemEditScreen.hpp"

class BagScreen : public Screen
{
public:
    BagScreen();
    virtual ~BagScreen();

    void update(touchPosition* touch) override;
    void draw() const override;

    ScreenType type() const { return ScreenType::BAG; }

private:
    std::vector<std::pair<Pouch, int>> limits;
    std::map<Pouch, std::vector<int>> allowedItems;
    std::vector<Button*> amountButtons;
    int currentPouch = 0;
    std::vector<Button*> buttons;
    int selectedItem = 0;
    int firstItem = 0;
    int firstEmpty = 0;
    bool selectingPouch = false;
    bool clickIndex(int i);
    bool switchPouch(int i);
    void editItem();
    std::unique_ptr<ItemEditScreen> select = nullptr;
    void editCount(bool up, int selected);
};

#endif