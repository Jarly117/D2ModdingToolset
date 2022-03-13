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

#ifndef BATATTACKSUMMON_H
#define BATATTACKSUMMON_H

#include "batattack.h"
#include "midgardid.h"

namespace game {

struct IAttack;

/** Represents attacks that summon units. */
struct CBatAttackSummon : public CBatAttackBase
{
    CMidgardID unitId;
    CMidgardID unitOrItemId;
    int attackNumber;
    IAttack* attack;
    int unknown;
};

static_assert(sizeof(CBatAttackSummon) == 24,
              "Size of CBatAttackSummon structure must be exactly 24 bytes");

namespace CBatAttackSummonApi {

struct Api
{
    IBatAttackVftable::OnAttack onHit;
};

Api& get();

} // namespace CBatAttackSummonApi

} // namespace game

#endif // BATATTACKSUMMON_H
