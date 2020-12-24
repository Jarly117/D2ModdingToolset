/*
 * This file is part of the modding toolset for Disciples 2.
 * (https://github.com/VladimirMakeev/D2ModdingToolset)
 * Copyright (C) 2020 Vladimir Makeev.
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

#ifndef MIDGARDOBJECTMAP_H
#define MIDGARDOBJECTMAP_H

#include "midgardid.h"

namespace game {

struct IMidgardObjectMap;
struct IMidScenarioObject;

struct IMidgardObjectMapVftable
{
    void* destructor;
    void* unknown[4];

    /**
     * Finds scenario object by its id.
     * Returns nullptr if object could not be found.
     */
    using FindScenarioObjectById =
        IMidScenarioObject*(__thiscall*)(const IMidgardObjectMap* thisptr, const CMidgardID* id);
    FindScenarioObjectById findScenarioObjectById;

    void* unknown2[2];

    /** Assumption: stores id of scenario object. */
    using InsertObjectId = bool(__thiscall*)(IMidgardObjectMap* thisptr,
                                             const IMidScenarioObject* object);
    InsertObjectId insertObjectId;
    void* unknownMethod;

    /** Creates id with IdCategory::Scenario and specified type. */
    using CreateScenarioId = void(__thiscall*)(IMidgardObjectMap* thisptr,
                                               CMidgardID* scenarioId,
                                               IdType type);
    CreateScenarioId createScenarioId;

    void* unknown3[2];
};

static_assert(sizeof(IMidgardObjectMapVftable) == 13 * sizeof(void*),
              "IMidgardObjectMap vftable must have exactly 13 methods");

struct IMidgardObjectMap
{
    const IMidgardObjectMapVftable* vftable;
};

} // namespace game

#endif // MIDGARDOBJECTMAP_H
