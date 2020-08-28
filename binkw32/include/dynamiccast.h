/*
 * This file is part of the binkw32 proxy dll for Disciples 2.
 * (https://github.com/VladimirMakeev/D2Binkw32Proxy)
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

#ifndef DYNAMICCAST_H
#define DYNAMICCAST_H

namespace game {

/**
 * RTTI type descriptor structure used in game.
 * Must be aligned on 8 byte boundary.
 * @see http://www.openrce.org/articles/full_view/21 for additional info.
 */
struct alignas(8) TypeDescriptor
{
    const void* vftable; /**< Vtable of type_info class. */
    void* spare;         /**< Used to keep the demangled name returned by type_info::name(). */

// Nonstandard extension: zero-sized array in struct/union
#pragma warning(suppress : 4200)
    char name[0]; /**< Mangled type name. */
};

namespace RttiApi {

struct Api
{
    /**
     * Runtime implementation of dynamic_cast operator used in game.
     * @param[in] ptr pointer to a polymorphic object.
     * @param vfDelta offset of virtual function pointer in object.
     * @param[in] srcType static type of object pointed to by the ptr.
     * @dstType[in] intended result of a cast.
     * @isReference true if input is a reference.
     * @returns pointer to object of type dstType or NULL.
     */
    using DynamicCast = void*(__cdecl*)(void* ptr,
                                        int vfDelta,
                                        const TypeDescriptor* srcType,
                                        const TypeDescriptor* dstType,
                                        int isReference);
    DynamicCast dynamicCast;
};

Api& get();

struct Rtti
{
    TypeDescriptor* IMidScenarioObjectType;
    TypeDescriptor* CMidPlayerType;
    TypeDescriptor* CPlayerBuildingsType;
};

const Rtti& rtti();

} // namespace RttiApi

} // namespace game

#endif // DYNAMICCAST_H
