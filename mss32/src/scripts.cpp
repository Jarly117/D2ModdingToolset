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

#include "scripts.h"
#include "dynupgradeview.h"
#include "log.h"
#include "unitimplview.h"
#include "unitview.h"
#include "utils.h"

namespace hooks {

static void doBindApi(sol::state& lua)
{
    bindings::UnitView::bind(lua);
    bindings::UnitImplView::bind(lua);
    bindings::DynUpgradeView::bind(lua);
    lua.set_function("log", [](const std::string& message) { logDebug("luaDebug.log", message); });
}

std::optional<sol::state> loadScriptFile(const std::filesystem::path& path,
                                         bool alwaysExists,
                                         bool bindApi)
{
    if (!alwaysExists && !std::filesystem::exists(path))
        return std::nullopt;

    sol::state lua;
    sol::protected_function_result result;
    std::string pathString = path.string();
    if (bindApi) {
        static std::unordered_map<std::string, std::string> sources;
        if (sources.find(pathString) == sources.end())
            sources[pathString] = readFile(path);

        if (sources[pathString].empty()) {
            showErrorMessageBox(fmt::format("Failed to read '{:s}' script file", pathString));
            return std::nullopt;
        }

        lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math);
        doBindApi(lua);

        result = lua.safe_script(sources[pathString].c_str(),
                                 [](lua_State*, sol::protected_function_result pfr) {
                                     return pfr;
                                 });
    } else {
        result = lua.load_file(pathString)();
    }

    if (!result.valid()) {
        const sol::error err = result;
        showErrorMessageBox(fmt::format("Failed to load script '{:s}'.\n"
                                        "Reason: '{:s}'",
                                        pathString, err.what()));
        return std::nullopt;
    }

    return {std::move(lua)};
}

} // namespace hooks
