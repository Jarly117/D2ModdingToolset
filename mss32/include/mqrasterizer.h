/*
 * This file is part of the modding toolset for Disciples 2.
 * (https://github.com/VladimirMakeev/D2ModdingToolset)
 * Copyright (C) 2022 Vladimir Makeev.
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

#ifndef MQRASTERIZER_H
#define MQRASTERIZER_H

#include <cstdint>
#include <windows.h>

namespace game {

struct IMqRasterizerVftable;
struct DisplaySettings;
struct CMqPoint;
struct SurfaceDecompressData;
struct IDirectDrawSurface7;
struct IDirectDrawPalette;

struct IMqRasterizer
{
    struct SurfaceDecompVftable;

    struct SurfaceDecomp
    {
        SurfaceDecompVftable* vftable;
    };

    IMqRasterizerVftable* vftable;
};

struct IMqRasterizer::SurfaceDecompVftable
{
    using Destructor = void(__thiscall*)(IMqRasterizer::SurfaceDecomp* thisptr, char flags);
    Destructor destructor;

    using GetData = SurfaceDecompressData*(__thiscall*)(IMqRasterizer::SurfaceDecomp* thisptr);
    GetData getData;
};

static_assert(sizeof(IMqRasterizer::SurfaceDecompVftable) == 2 * sizeof(void*),
              "IMqRasterizer::SurfaceDecomp vftable must have exactly 2 methods");

struct IMqRasterizerVftable
{
    using Destructor = void(__thiscall*)(IMqRasterizer* thisptr, char flags);
    Destructor destructor;

    /** Returns true if specified display settings can be applied. */
    using IsSettingsSupported = bool(__thiscall*)(const IMqRasterizer* thisptr,
                                                  const DisplaySettings* displaySettings);
    IsSettingsSupported isSettingsSupported;

    /**
     * Initializes rasterizer surfaces using provided display settings.
     * @returns true if initialization was successfull.
     */
    using InitializeSurfaces = bool(__thiscall*)(IMqRasterizer* thisptr,
                                                 DisplaySettings* displaySettings);
    InitializeSurfaces initializeSurfaces;

    /** Returns current display settings. */
    using GetDisplaySettings = DisplaySettings*(__thiscall*)(const IMqRasterizer* thisptr,
                                                             DisplaySettings* displaySettings);
    GetDisplaySettings getDisplaySettings;

    using Method4 = void*(__thiscall*)(IMqRasterizer* thisptr, void* a2);
    Method4 method4;

    /** Returns size of screen surface. */
    using GetScreenSurfaceSize = CMqPoint*(__thiscall*)(const IMqRasterizer* thisptr,
                                                        CMqPoint* screenSurfaceSize);
    GetScreenSurfaceSize getScreenSurfaceSize;

    void* method6;
    void* method7;

    using Method8 = bool(__thiscall*)(IMqRasterizer* thisptr);
    Method8 method8;

    /** Returns true if game is in fullscreen mode. */
    using IsFullscreen = bool(__thiscall*)(const IMqRasterizer* thisptr);
    IsFullscreen isFullscreen;

    /** Calls IDirectDraw7::FlipToGDISurface. */
    using FlipToGDISurface = void(__thiscall*)(IMqRasterizer* thisptr);
    FlipToGDISurface flipToGDISurface;

    /** Performs a bit block transfer, calls IDirectDrawSurface7::Blt. */
    using Blit = void(__thiscall*)(IMqRasterizer* thisptr);
    Blit blit;

    /** Calls IDirectDrawSurface7::Flip. */
    using SwapBuffers = void(__thiscall*)(IMqRasterizer* thisptr);
    SwapBuffers swapBuffers;

    using SetBool = bool(__thiscall*)(IMqRasterizer* thisptr, bool value);

    SetBool setUnknown;
    SetBool setLowAlpha;

    using Method15 = bool(__thiscall*)(IMqRasterizer* thisptr);
    Method15 method15;

    using AlphaBlend = void(__thiscall*)(IMqRasterizer* thisptr,
                                         IDirectDrawSurface7* srcSurface,
                                         int a9,
                                         CMqPoint* dstPosition,
                                         int a10,
                                         int opacity);
    AlphaBlend alphaBlend;

    /** Assumption: returns maximum dimensions the surface (image) can use in the game. */
    using GetMaxSurfaceSize = CMqPoint*(__thiscall*)(IMqRasterizer* thisptr, CMqPoint* size);
    GetMaxSurfaceSize getMaxSurfaceSize;

    using CreateSurfaceDecomp =
        IMqRasterizer::SurfaceDecomp*(__thiscall*)(IMqRasterizer* thisptr,
                                                   IMqRasterizer::SurfaceDecomp** surfaceDecomp,
                                                   IDirectDrawPalette* palette,
                                                   CMqPoint* textureSize,
                                                   int maybeBitsPerPixel);
    CreateSurfaceDecomp createSurfaceDecomp;

    /** Calls IDirectDraw7::CreateSurface with DDSCAPS_OFFSCREENPLAIN description flag. */
    using CreateOffscreenSurface = void(__thiscall*)(IMqRasterizer* thisptr,
                                                     IDirectDrawSurface7** surface,
                                                     CMqPoint* size,
                                                     std::uint32_t* hintPtr,
                                                     std::uint16_t opacity,
                                                     char a6);
    CreateOffscreenSurface createOffscreenSurface;

    void* method20;

    using GetScreenSurfaceColorBitMask = void(__thiscall*)(IMqRasterizer* thisptr,
                                                           std::uint32_t* rBitMask,
                                                           std::uint32_t* gBitMask,
                                                           std::uint32_t* bBitMask,
                                                           std::uint32_t* alphaBitMask);
    GetScreenSurfaceColorBitMask getScreenSurfaceColorBitMask;

    /** Calls IDirectDraw7::CreatePalette with DDPCAPS_8BIT flag. */
    using CreatePalette = void(__thiscall*)(IMqRasterizer* thisptr,
                                            IDirectDrawPalette** palette,
                                            PALETTEENTRY* paletteEntries);
    CreatePalette createPalette;

    using Method23 = bool(__thiscall*)(IMqRasterizer* thisptr, int a2, int a3, __int16 a4);
    Method23 method23;

    using Method24 = void(__thiscall*)(IMqRasterizer* thisptr, int a2, unsigned int a3, int a4);
    Method24 method24;

    using Method25 = bool(__thiscall*)(IMqRasterizer* thisptr, int a2);
    Method25 method25;
};

static_assert(sizeof(IMqRasterizerVftable) == 26 * sizeof(void*),
              "IMqRasterizer vftable must have exactly 26 methods");

} // namespace game

#endif // MQRASTERIZER_H