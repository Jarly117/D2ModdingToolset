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

#ifndef SMARTPTR_H
#define SMARTPTR_H

namespace game {

struct SmartPointer
{
    int* referenceCount;
    void* data;
};

namespace SmartPointerApi {

struct Api
{
    /**
     * Sets SmartPointer data or releases it, freeing memory if referenceCount equals zero.
     * @param[in] thisptr SmartPointer to initialize or free.
     * @param[in] data pointer to managed memory.
     * @returns unknown.
     */
    using CreateOrFree = void*(__thiscall*)(SmartPointer* thisptr, void* data);
    CreateOrFree createOrFree;
};

Api& get();

} // namespace SmartPointerApi

} // namespace game

#endif // SMARTPTR_H