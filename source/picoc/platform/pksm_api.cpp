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

#include "gui.hpp"
#include "BoxChoice.hpp"
#include "FortyChoice.hpp"
#include "ThirtyChoice.hpp"
#include "loader.hpp"
#include "STDirectory.hpp"
#include "PB7.hpp"
#include "PK4.hpp"
#include "PK5.hpp"
#include "PK6.hpp"
#include "PK7.hpp"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>

extern "C" {
#include "pksm_api.h"

    void gui_warn(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
    {
        char* lineOne = (char*) Param[0]->Val->Pointer;
        char* lineTwo = (char*) Param[1]->Val->Pointer;
        if (lineTwo != nullptr)
        {
            Gui::warn(lineOne, lineTwo);
        }
        else
        {
            Gui::warn(lineOne);
        }
    }

    void gui_choice(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
    {
        char* lineOne = (char*) Param[0]->Val->Pointer;
        char* lineTwo = (char*) Param[1]->Val->Pointer;
        if (lineTwo != nullptr)
        {
            ReturnValue->Val->Integer = (int) Gui::showChoiceMessage(lineOne, lineTwo);
        }
        else
        {
            ReturnValue->Val->Integer = (int) Gui::showChoiceMessage(lineOne);
        }
    }

    void gui_menu6x5(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
    {
        char* question = (char*) Param[0]->Val->Pointer;
        int options = Param[1]->Val->Integer;
        char** labels = (char**) Param[2]->Val->Pointer;
        pkm* pokemon = (pkm*) Param[3]->Val->Pointer;
        Generation gen = Generation(Param[4]->Val->Integer);
        ThirtyChoice screen = ThirtyChoice(question, labels, pokemon, options, gen);
        ReturnValue->Val->Integer = screen.run();
    }

    void gui_menu20x2(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
    {
        char* question = (char*) Param[0]->Val->Pointer;
        int options = Param[1]->Val->Integer;
        char** labels = (char**) Param[2]->Val->Pointer;
        FortyChoice screen = FortyChoice(question, labels, options);
        ReturnValue->Val->Integer = screen.run();
    }

    void sav_sbo(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
    {
        switch (TitleLoader::save->version())
        {
            case 7:
            case 8:
                ReturnValue->Val->Integer = ((SavHGSS*)TitleLoader::save.get())->getSBO();
                break;
            case 10:
            case 11:
                ReturnValue->Val->Integer = ((SavDP*)TitleLoader::save.get())->getSBO();
                break;
            case 12:
                ReturnValue->Val->Integer = ((SavPT*)TitleLoader::save.get())->getSBO();
                break;
            default:
                ReturnValue->Val->Integer = 0;
                break;
        }
    }

    void sav_gbo(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
    {
        switch (TitleLoader::save->version())
        {
            case 7:
            case 8:
                ReturnValue->Val->Integer = ((SavHGSS*)TitleLoader::save.get())->getGBO();
                break;
            case 10:
            case 11:
                ReturnValue->Val->Integer = ((SavDP*)TitleLoader::save.get())->getGBO();
                break;
            case 12:
                ReturnValue->Val->Integer = ((SavPT*)TitleLoader::save.get())->getGBO();
                break;
            default:
                ReturnValue->Val->Integer = 0;
                break;
        }
    }

    void sav_boxDecrypt(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
    {
        TitleLoader::save->cryptBoxData(true);
    }

    void sav_boxEncrypt(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
    {
        TitleLoader::save->cryptBoxData(false);
    }

    void gui_keyboard(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
    {
        char* out = (char*) Param[0]->Val->Pointer;
        char* hint = (char*) Param[1]->Val->Pointer;
        int numChars = Param[2]->Val->Integer;

        C3D_FrameEnd(0);
        
        SwkbdState state;
        swkbdInit(&state, SWKBD_TYPE_NORMAL, 1, numChars);
        swkbdSetHintText(&state, hint);
        swkbdSetValidation(&state, SWKBD_NOTBLANK_NOTEMPTY, 0, 0);
        swkbdInputText(&state, out, numChars);
        out[numChars - 1] = '\0';
    }

    void gui_numpad(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
    {
        int* out = (int*) Param[0]->Val->Pointer;
        std::string hint = (char*) Param[1]->Val->Pointer;
        std::optional<std::string> hint2 = std::nullopt;
        if (hint.find('\n') != std::string::npos)
        {
            hint2 = hint.substr(hint.find('\n')+1);
            hint = hint.substr(0, hint.find('\n'));
        }
        int numChars = Param[2]->Val->Integer;

        char number[numChars + 1] = {0};

        C3D_FrameEnd(0);
        
        SwkbdState state;
        swkbdInit(&state, SWKBD_TYPE_NUMPAD, 2, numChars);
        swkbdSetValidation(&state, SWKBD_NOTBLANK_NOTEMPTY, 0, 0);
        swkbdSetButton(&state, SWKBD_BUTTON_LEFT, "What?", false);
        SwkbdButton button;
        do
        {
            button = swkbdInputText(&state, number, sizeof(number));
            if (button != SWKBD_BUTTON_CONFIRM)
            {
                Gui::warn(hint, hint2);
                C3D_FrameEnd(0); // Just make sure
            }
        }
        while (button != SWKBD_BUTTON_CONFIRM);
        number[numChars] = '\0';
        *out = std::atoi(number);
    }

    void current_directory(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
    {
        std::string fileName = Parser->FileName;
        fileName = fileName.substr(0, fileName.rfind('/'));
        ReturnValue->Val->Pointer = fileName.data();
    }

    void read_directory(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
    {
        std::string dir = (char*)Param[0]->Val->Pointer;
        STDirectory directory(dir);
        struct dirData {
            int amount;
            char** data;
        };
        dirData* ret = (dirData*) malloc(sizeof(dirData));
        if (directory.good())
        {
            ret->amount = directory.count();
            ret->data = (char**) malloc(sizeof(char*) * directory.count());
            for (size_t i = 0; i < directory.count(); i++)
            {
                std::string item = dir + "/" + directory.item(i);
                ret->data[i] = (char*) malloc(sizeof(char) * (item.size() + 1));
                ret->data[i][item.size()] = '\0';
                strcpy(ret->data[i], item.data());
            }
        }
        else
        {
            ret->amount = 0;
            ret->data = nullptr;
        }
        ReturnValue->Val->Pointer = ret;
    }

    void sav_inject_pkx(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
    {
        u8* data = (u8*) Param[0]->Val->Pointer;
        Generation gen = Generation(Param[1]->Val->Integer);
        int box = Param[2]->Val->Integer;
        int slot = Param[3]->Val->Integer;

        std::unique_ptr<PKX> pkm = nullptr;

        switch (gen)
        {
            case Generation::FOUR:
                pkm = std::make_unique<PK4>(data, false);
                break;
            case Generation::FIVE:
                pkm = std::make_unique<PK5>(data, false);
                break;
            case Generation::SIX:
                pkm = std::make_unique<PK6>(data, false);
                break;
            case Generation::SEVEN:
                pkm = std::make_unique<PK7>(data, false);
                break;
            case Generation::LGPE:
                pkm = std::make_unique<PB7>(data, false);
                break;
            default:
                Gui::warn("What did you do?", "Generation is incorrect!");
        }

        if (pkm)
        {
            if (TitleLoader::save->generation() == Generation::LGPE)
            {
                if (pkm->generation() == Generation::LGPE)
                {
                    TitleLoader::save->pkm(*pkm, box, slot);
                }
            }
            else
            {
                if (pkm->generation() != Generation::LGPE)
                {
                    while (pkm->generation() != TitleLoader::save->generation())
                    {
                        if (pkm->generation() < TitleLoader::save->generation())
                        {
                            pkm = pkm->next();
                        }
                        else
                        {
                            pkm = pkm->previous();
                        }
                    }
                    u8 (*formCounter)(u16);
                    switch (TitleLoader::save->generation())
                    {
                        case Generation::FOUR:
                            formCounter = PersonalDPPtHGSS::formCount;
                            break;
                        case Generation::FIVE:
                            formCounter = PersonalBWB2W2::formCount;
                            break;
                        case Generation::SIX:
                            formCounter = PersonalXYORAS::formCount;
                            break;
                        case Generation::SEVEN:
                        default:
                            formCounter = PersonalSMUSUM::formCount;
                            break;
                    }
                    bool moveBad = false;
                    for (int i = 0; i < 4; i++)
                    {
                        if (pkm->move(i) > TitleLoader::save->maxMove())
                        {
                            moveBad = true;
                            break;
                        }
                        if (pkm->generation() == Generation::SIX)
                        {
                            PK6* pk6 = (PK6*) pkm.get();
                            if (pk6->relearnMove(i) > TitleLoader::save->maxMove())
                            {
                                moveBad = true;
                                break;
                            }
                        }
                        else if (pkm->generation() == Generation::SEVEN)
                        {
                            PK7* pk7 = (PK7*) pkm.get();
                            if (pk7->relearnMove(i) > TitleLoader::save->maxMove())
                            {
                                moveBad = true;
                                break;
                            }
                        }
                    }
                    if (pkm->species() > TitleLoader::save->maxSpecies())
                    {
                        Gui::warn(i18n::localize("STORAGE_BAD_TRANFER"), i18n::localize("STORAGE_BAD_SPECIES"));
                        return;
                    }
                    else if (pkm->alternativeForm() > formCounter(pkm->species()))
                    {
                        Gui::warn(i18n::localize("STORAGE_BAD_TRANFER"), i18n::localize("STORAGE_BAD_FORM"));
                        return;
                    }
                    else if (pkm->ability() > TitleLoader::save->maxAbility())
                    {
                        Gui::warn(i18n::localize("STORAGE_BAD_TRANFER"), i18n::localize("STORAGE_BAD_ABILITY"));
                        return;
                    }
                    else if (pkm->heldItem() > TitleLoader::save->maxItem())
                    {
                        
                        Gui::warn(i18n::localize("STORAGE_BAD_TRANFER"), i18n::localize("STORAGE_BAD_ITEM"));
                        return;
                    }
                    else if (pkm->ball() > TitleLoader::save->maxBall())
                    {
                        Gui::warn(i18n::localize("STORAGE_BAD_TRANFER"), i18n::localize("STORAGE_BAD_BALL"));
                        return;
                    }
                    else if (moveBad)
                    {
                        Gui::warn(i18n::localize("STORAGE_BAD_TRANFER"), i18n::localize("STORAGE_BAD_MOVE"));
                        return;
                    }
                    TitleLoader::save->pkm(*pkm, box, slot);
                }
            }
        }
    }

    void sav_inject_ekx(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
    {
        u8* data = (u8*) Param[0]->Val->Pointer;
        Generation gen = Generation(Param[1]->Val->Integer);
        int box = Param[2]->Val->Integer;
        int slot = Param[3]->Val->Integer;

        std::unique_ptr<PKX> pkm = nullptr;

        switch (gen)
        {
            case Generation::FOUR:
                pkm = std::make_unique<PK4>(data, true);
                break;
            case Generation::FIVE:
                pkm = std::make_unique<PK5>(data, true);
                break;
            case Generation::SIX:
                pkm = std::make_unique<PK6>(data, true);
                break;
            case Generation::SEVEN:
                pkm = std::make_unique<PK7>(data, true);
                break;
            case Generation::LGPE:
                pkm = std::make_unique<PB7>(data, true);
                break;
            default:
                Gui::warn("What did you do?", "Generation is incorrect!");
        }

        if (pkm)
        {
            if (TitleLoader::save->generation() == Generation::LGPE)
            {
                if (pkm->generation() == Generation::LGPE)
                {
                    TitleLoader::save->pkm(*pkm, box, slot);
                }
            }
            else
            {
                if (pkm->generation() != Generation::LGPE)
                {
                    while (pkm->generation() != TitleLoader::save->generation())
                    {
                        if (pkm->generation() < TitleLoader::save->generation())
                        {
                            pkm = pkm->next();
                        }
                        else
                        {
                            pkm = pkm->previous();
                        }
                    }
                    u8 (*formCounter)(u16);
                    switch (TitleLoader::save->generation())
                    {
                        case Generation::FOUR:
                            formCounter = PersonalDPPtHGSS::formCount;
                            break;
                        case Generation::FIVE:
                            formCounter = PersonalBWB2W2::formCount;
                            break;
                        case Generation::SIX:
                            formCounter = PersonalXYORAS::formCount;
                            break;
                        case Generation::SEVEN:
                        default:
                            formCounter = PersonalSMUSUM::formCount;
                            break;
                    }
                    bool moveBad = false;
                    for (int i = 0; i < 4; i++)
                    {
                        if (pkm->move(i) > TitleLoader::save->maxMove())
                        {
                            moveBad = true;
                            break;
                        }
                        if (pkm->generation() == Generation::SIX)
                        {
                            PK6* pk6 = (PK6*) pkm.get();
                            if (pk6->relearnMove(i) > TitleLoader::save->maxMove())
                            {
                                moveBad = true;
                                break;
                            }
                        }
                        else if (pkm->generation() == Generation::SEVEN)
                        {
                            PK7* pk7 = (PK7*) pkm.get();
                            if (pk7->relearnMove(i) > TitleLoader::save->maxMove())
                            {
                                moveBad = true;
                                break;
                            }
                        }
                    }
                    if (pkm->species() > TitleLoader::save->maxSpecies())
                    {
                        Gui::warn(i18n::localize("STORAGE_BAD_TRANFER"), i18n::localize("STORAGE_BAD_SPECIES"));
                        return;
                    }
                    else if (pkm->alternativeForm() > formCounter(pkm->species()))
                    {
                        Gui::warn(i18n::localize("STORAGE_BAD_TRANFER"), i18n::localize("STORAGE_BAD_FORM"));
                        return;
                    }
                    else if (pkm->ability() > TitleLoader::save->maxAbility())
                    {
                        Gui::warn(i18n::localize("STORAGE_BAD_TRANFER"), i18n::localize("STORAGE_BAD_ABILITY"));
                        return;
                    }
                    else if (pkm->heldItem() > TitleLoader::save->maxItem())
                    {
                        
                        Gui::warn(i18n::localize("STORAGE_BAD_TRANFER"), i18n::localize("STORAGE_BAD_ITEM"));
                        return;
                    }
                    else if (pkm->ball() > TitleLoader::save->maxBall())
                    {
                        Gui::warn(i18n::localize("STORAGE_BAD_TRANFER"), i18n::localize("STORAGE_BAD_BALL"));
                        return;
                    }
                    else if (moveBad)
                    {
                        Gui::warn(i18n::localize("STORAGE_BAD_TRANFER"), i18n::localize("STORAGE_BAD_MOVE"));
                        return;
                    }
                    TitleLoader::save->pkm(*pkm, box, slot);
                }
            }
        }
    }

    void cfg_default_ot(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
    {
        ReturnValue->Val->Pointer = (void*)Configuration::getInstance().defaultOT().c_str();
    }

    void cfg_default_tid(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
    {
        ReturnValue->Val->UnsignedInteger = Configuration::getInstance().defaultTID();
    }

    void cfg_default_sid(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
    {
        ReturnValue->Val->UnsignedInteger = Configuration::getInstance().defaultSID();
    }

    void cfg_default_day(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
    {
        ReturnValue->Val->Integer = Configuration::getInstance().day();
    }

    void cfg_default_month(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
    {
        ReturnValue->Val->Integer = Configuration::getInstance().month();
    }

    void cfg_default_year(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
    {
        ReturnValue->Val->Integer = Configuration::getInstance().year();
    }

    void gui_boxes(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
    {
        int* fromStorage = (int*) Param[0]->Val->Pointer;
        int* box = (int*) Param[1]->Val->Pointer;
        int* slot = (int*) Param[2]->Val->Pointer;

        BoxChoice screen = BoxChoice();
        auto result = screen.run();

        *fromStorage = std::get<0>(result);
        *box = std::get<1>(result);
        *slot = std::get<2>(result) - 1;
        ReturnValue->Val->Integer = std::get<0>(result) == 0 && std::get<1>(result) == -1 &&std::get<2>(result) == -1 ? -1 : 0;
    }

    void net_udp_receiver(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
    {
        char* buffer = (char*)Param[0]->Val->Pointer;
        int size = (int)Param[1]->Val->Integer;
        int* bytesReceived = (int*)Param[2]->Val->Pointer;

        struct sockaddr_in addr;
        socklen_t addrlen = sizeof(addr);
        int fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (fd < 0)
        {
            ReturnValue->Val->Integer = errno;
            return;
        }
        memset(&addr, 0, addrlen);
        addr.sin_family = AF_INET;
        addr.sin_port = htons(PKSM_PORT);
        addr.sin_addr.s_addr = INADDR_ANY;
        if (bind(fd, (struct sockaddr*)&addr, addrlen) < 0)
        {
            ReturnValue->Val->Integer = errno;
            close(fd);
            return;
        }
        *bytesReceived = 0;
        while (*bytesReceived < size) {
            int n = recvfrom(fd, buffer, size, 0, (struct sockaddr*)&addr, &addrlen);
            *bytesReceived += n;
            if (n <= 0) break;
        }

        close(fd);
        ReturnValue->Val->Integer = 0;
    }

    void net_tcp_receiver(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
    {
        char* buffer = (char*)Param[0]->Val->Pointer;
        int size = (int)Param[1]->Val->Integer;
        int* bytesReceived = (int*)Param[2]->Val->Pointer;

        struct sockaddr_in addr;
        socklen_t addrlen = sizeof(addr);
        int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        if (fd < 0)
        {
            ReturnValue->Val->Integer = errno;
            return;
        }
        memset(&addr, 0, addrlen);
        addr.sin_family = AF_INET;
        addr.sin_port = htons(PKSM_PORT);
        addr.sin_addr.s_addr = INADDR_ANY;
        if (bind(fd, (struct sockaddr*)&addr, addrlen) < 0)
        {
            ReturnValue->Val->Integer = errno;
            close(fd);
            return;
        }
        if (listen(fd, 5) < 0)
        {
            ReturnValue->Val->Integer = errno;
            close(fd);
            return;            
        }
        int fdconn;
        if ((fdconn = accept(fd, (struct sockaddr*)&addr, &addrlen)) < 0)
        {
            ReturnValue->Val->Integer = errno;
            close(fd);
            return;             
        }
        *bytesReceived = 0;
        while (*bytesReceived < size) {
            int n = recv(fdconn, buffer, size, 0);
            *bytesReceived += n;
            if (n <= 0) break;
        }

        close(fdconn);
        close(fd);
        ReturnValue->Val->Integer = 0;
    }

    void net_tcp_sender(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
    {
        char* ip = (char*)Param[0]->Val->Pointer;
        int port = (int)Param[1]->Val->Integer;
        char* buffer = (char*)Param[2]->Val->Pointer;
        int size = (int)Param[3]->Val->Integer;

        struct sockaddr_in addr;
        socklen_t addrlen = sizeof(addr);
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd == -1)
        {
            ReturnValue->Val->Integer = errno;
            return;
        }
        memset(&addr, 0, addrlen);
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        inet_pton(AF_INET, ip, &addr.sin_addr);
        if (connect(fd, (struct sockaddr*)&addr, addrlen) < 0)
        {
            ReturnValue->Val->Integer = errno;
            close(fd);
            return;
        }

        int total = 0;
        int chunk = 1024;
        int n;
        while (total < size) {
            size_t tosend = size - total > chunk ? chunk : size - total;
            n = send(fd, buffer + total, tosend, 0);
            if (n == -1) { break; }
            total += n;
        }

        close(fd);
        ReturnValue->Val->Integer = total == size ? 0 : errno;
    }

    void bank_inject_pkx(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
    {
        u8* data = (u8*) Param[0]->Val->Pointer;
        Generation gen = Generation(Param[1]->Val->Integer);
        int box = Param[2]->Val->Integer;
        int slot = Param[3]->Val->Integer;

        std::unique_ptr<PKX> pkm = nullptr;

        switch (gen)
        {
            case Generation::FOUR:
                pkm = std::make_unique<PK4>(data, false);
                break;
            case Generation::FIVE:
                pkm = std::make_unique<PK5>(data, false);
                break;
            case Generation::SIX:
                pkm = std::make_unique<PK6>(data, false);
                break;
            case Generation::SEVEN:
                pkm = std::make_unique<PK7>(data, false);
                break;
            case Generation::LGPE:
                pkm = std::make_unique<PB7>(data, false);
                break;
            default:
                Gui::warn("What did you do?", "Generation is incorrect!");
                return;
        }

        Bank bank;
        bank.pkm(*pkm, box, slot);
        bank.save();
    }

    void net_ip(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
    {
        char hostbuffer[256];
        if (gethostname(hostbuffer, sizeof(hostbuffer)) == -1)
        {
            ReturnValue->Val->Pointer = (void*)"";
        }
        struct hostent *host_entry = gethostbyname(hostbuffer);
        if (host_entry == NULL)
        {
            ReturnValue->Val->Pointer = (void*)"";
        }
        ReturnValue->Val->Pointer = (void*)inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[0]));
    }

    void sav_get_pkx(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
    {
        u8* data = (u8*) Param[0]->Val->Pointer;
        int box = Param[1]->Val->Integer;
        int slot = Param[2]->Val->Integer;

        auto pkm = TitleLoader::save->pkm(box, slot);
        memcpy(data, pkm.get()->rawData(), pkm.get()->getLength());
    }

    void party_get_pkx(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
    {
        u8* data = (u8*) Param[0]->Val->Pointer;
        int slot = Param[1]->Val->Integer;

        auto pkm = TitleLoader::save->pkm(slot);
        memcpy(data, pkm.get()->rawData(), pkm.get()->getLength());
    }

    void i18n_species(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
    {
        ReturnValue->Val->Pointer = (void*) i18n::species(Configuration::getInstance().language(), Param[0]->Val->Integer).c_str();
    }
}
