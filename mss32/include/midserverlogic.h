/*
 * This file is part of the modding toolset for Disciples 2.
 * (https://github.com/VladimirMakeev/D2ModdingToolset)
 * Copyright (C) 2021 Vladimir Makeev.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MIDSERVERLOGIC_H
#define MIDSERVERLOGIC_H

#include "d2set.h"
#include "midmsgsender.h"
#include "midserverlogiccore.h"

namespace game {

struct CMidServer;

struct CMidServerLogicData
{
    int unknown;
    int unknown2;
    int unknown3;
};

struct CMidServerLogicData2
{
    CMidgardID unknownId;
    int unknown;
    int unknown2;
    Vector<void*> array;
    Vector<void*> array2;
    Vector<void*> array3;
    Vector<void*> array4;
    Vector<void*> array5;
    Vector<void*> array6;
    Set<CMidgardID> list;
    Set<void> list2;
};

assert_size(CMidServerLogicData2, 164);
assert_offset(CMidServerLogicData2, array, 12);
assert_offset(CMidServerLogicData2, array2, 28);
assert_offset(CMidServerLogicData2, array3, 44);
assert_offset(CMidServerLogicData2, array4, 60);
assert_offset(CMidServerLogicData2, array5, 76);
assert_offset(CMidServerLogicData2, array6, 92);
assert_offset(CMidServerLogicData2, list, 108);
assert_offset(CMidServerLogicData2, list2, 136);

struct CMidServerLogic
    : public CMidServerLogicCore
    , public IMidMsgSender
{
    CMidServer* midServer;
    int unknown2;
    int unknown3;
    CMidServerLogicData data;
    Vector<void*> array;
    int unknown5;
    List<CMidgardID> playersIdList;
    int unknown6;
    int unknown7;
    CMidServerLogicData2 data2;
    int unknown8;
    int unknown9;
    bool turnNumberIsZero;
    char padding[3];
    List<void*> list;
    int unknown11;
    int unknown12;
    std::uint32_t aiMessageId;
    int unknown14;
    bool upgradeLeaders;
    char padding2[3];
    int unknown16;
    CMidgardID winnerPlayerId;
    List<void*> list2;
    char unknown17;
    char padding3[3];
    BattleMsgData* battleMsgData;
};

assert_size(CMidServerLogic, 324);
assert_offset(CMidServerLogic, CMidServerLogic::IMidMsgSender::vftable, 8);
assert_offset(CMidServerLogic, data, 24);
assert_offset(CMidServerLogic, array, 36);
assert_offset(CMidServerLogic, playersIdList, 56);
assert_offset(CMidServerLogic, data2, 80);
assert_offset(CMidServerLogic, unknown8, 244);
assert_offset(CMidServerLogic, unknown11, 272);
assert_offset(CMidServerLogic, list2, 300);
assert_offset(CMidServerLogic, unknown17, 316);

namespace CMidServerLogicApi {

struct Api
{
    using GetObjectMap = IMidgardObjectMap*(__thiscall*)(CMidServerLogic* thisptr);
    GetObjectMap getObjectMap;
};

Api& get();

} // namespace CMidServerLogicApi

} // namespace game

#endif // MIDSERVERLOGIC_H
