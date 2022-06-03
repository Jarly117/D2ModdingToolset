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
#include "attackview.h"
#include "categoryids.h"
#include "currencyview.h"
#include "dynupgradeview.h"
#include "idview.h"
#include "itembaseview.h"
#include "itemview.h"
#include "locationview.h"
#include "log.h"
#include "midstack.h"
#include "point.h"
#include "scenariovariableview.h"
#include "scenarioview.h"
#include "scenvariablesview.h"
#include "stackview.h"
#include "tileview.h"
#include "unitimplview.h"
#include "unitslotview.h"
#include "unitview.h"
#include "utils.h"
#include <map>
#include <mutex>
#include <thread>

namespace hooks {

struct PathHash
{
    std::size_t operator()(std::filesystem::path const& p) const noexcept
    {
        return std::filesystem::hash_value(p);
    }
};

static void bindApi(sol::state& lua)
{
    using namespace game;

    // clang-format off
    lua.new_enum("Race",
        "Human", RaceId::Human,
        "Undead", RaceId::Undead,
        "Heretic", RaceId::Heretic,
        "Dwarf", RaceId::Dwarf,
        "Neutral", RaceId::Neutral,
        "Elf", RaceId::Elf
    );

    lua.new_enum("Subrace",
        "Custom", SubRaceId::Custom,
        "Human", SubRaceId::Human,
        "Undead", SubRaceId::Undead,
        "Heretic", SubRaceId::Heretic,
        "Dwarf", SubRaceId::Dwarf,
        "Neutral", SubRaceId::Neutral,
        "NeutralHuman", SubRaceId::NeutralHuman,
        "NeutralElf", SubRaceId::NeutralElf,
        "NeutralGreenSkin", SubRaceId::NeutralGreenSkin,
        "NeutralDragon", SubRaceId::NeutralDragon,
        "NeutralMarsh", SubRaceId::NeutralMarsh,
        "NeutralWater", SubRaceId::NeutralWater,
        "NeutralBarbarian", SubRaceId::NeutralBarbarian,
        "NeutralWolf", SubRaceId::NeutralWolf,
        "Elf", SubRaceId::Elf
    );

    lua.new_enum("Terrain",
        "Human", TerrainId::Human,
        "Dwarf", TerrainId::Dwarf,
        "Heretic", TerrainId::Heretic,
        "Undead", TerrainId::Undead,
        "Neutral", TerrainId::Neutral,
        "Elf", TerrainId::Elf
    );

    lua.new_enum("Ground",
        "Plain", GroundId::Plain,
        "Forest", GroundId::Forest,
        "Water", GroundId::Water,
        "Mountain", GroundId::Mountain
    );

    lua.new_enum("Unit",
        "Soldier", UnitId::Soldier,
        "Noble", UnitId::Noble,
        "Leader", UnitId::Leader,
        "Summon", UnitId::Summon,
        "Illusion", UnitId::Illusion,
        "Guardian", UnitId::Guardian
    );

    lua.new_enum("Leader",
        "Fighter", LeaderId::Fighter,
        "Explorer", LeaderId::Explorer,
        "Mage", LeaderId::Mage,
        "Rod", LeaderId::Rod,
        "Noble", LeaderId::Noble
    );

    lua.new_enum("Ability",
        "Incorruptible", LeaderAbilityId::Incorruptible,
        "WeaponMaster", LeaderAbilityId::WeaponMaster,
        "WandScrollUse", LeaderAbilityId::WandScrollUse,
        "WeaponArmorUse", LeaderAbilityId::WeaponArmorUse,
        "BannerUse", LeaderAbilityId::BannerUse,
        "JewelryUse", LeaderAbilityId::JewelryUse,
        "Rod", LeaderAbilityId::Rod,
        "OrbUse", LeaderAbilityId::OrbUse,
        "TalismanUse", LeaderAbilityId::TalismanUse,
        "TravelItemUse", LeaderAbilityId::TravelItemUse,
        "CriticalHit", LeaderAbilityId::CriticalHit
    );

    lua.new_enum("Attack",
        "Damage", AttackClassId::Damage,
        "Drain", AttackClassId::Drain,
        "Paralyze", AttackClassId::Paralyze,
        "Heal", AttackClassId::Heal,
        "Fear", AttackClassId::Fear,
        "BoostDamage", AttackClassId::BoostDamage,
        "Petrify", AttackClassId::Petrify,
        "LowerDamage", AttackClassId::LowerDamage,
        "LowerInitiative", AttackClassId::LowerInitiative,
        "Poison", AttackClassId::Poison,
        "Frostbite", AttackClassId::Frostbite,
        "Revive", AttackClassId::Revive,
        "DrainOverflow", AttackClassId::DrainOverflow,
        "Cure", AttackClassId::Cure,
        "Summon", AttackClassId::Summon,
        "DrainLevel", AttackClassId::DrainLevel,
        "GiveAttack", AttackClassId::GiveAttack,
        "Doppelganger", AttackClassId::Doppelganger,
        "TransformSelf", AttackClassId::TransformSelf,
        "TransformOther", AttackClassId::TransformOther,
        "Blister", AttackClassId::Blister,
        "BestowWards", AttackClassId::BestowWards,
        "Shatter", AttackClassId::Shatter
    );

    lua.new_enum("Source",
        "Weapon", AttackSourceId::Weapon,
        "Mind", AttackSourceId::Mind,
        "Life", AttackSourceId::Life,
        "Death", AttackSourceId::Death,
        "Fire", AttackSourceId::Fire,
        "Water", AttackSourceId::Water,
        "Earth", AttackSourceId::Earth,
        "Air", AttackSourceId::Air
    );

    lua.new_enum("Reach",
        "All", AttackReachId::All,
        "Any", AttackReachId::Any,
        "Adjacent", AttackReachId::Adjacent
    );

    lua.new_enum("Item",
        "Armor", ItemId::Armor,
        "Jewel", ItemId::Jewel,
        "Weapon", ItemId::Weapon,
        "Banner", ItemId::Banner,
        "PotionBoost", ItemId::PotionBoost,
        "PotionHeal", ItemId::PotionHeal,
        "PotionRevive", ItemId::PotionRevive,
        "PotionPermanent", ItemId::PotionPermanent,
        "Scroll", ItemId::Scroll,
        "Wand", ItemId::Wand,
        "Valuable", ItemId::Valuable,
        "Orb", ItemId::Orb,
        "Talisman", ItemId::Talisman,
        "TravelItem", ItemId::TravelItem,
        "Special", ItemId::Special
    );

    lua.new_enum("Equipment",
        "Banner", EquippedItemIdx::Banner,
        "Tome", EquippedItemIdx::Tome,
        "Battle1", EquippedItemIdx::Battle1,
        "Battle2", EquippedItemIdx::Battle2,
        "Artifact1", EquippedItemIdx::Artifact1,
        "Artifact2", EquippedItemIdx::Artifact2,
        "Boots", EquippedItemIdx::Boots
    );

    lua.new_enum("Immune",
        "NotImmune", ImmuneId::Notimmune,
        "Once", ImmuneId::Once,
        "Always", ImmuneId::Always
    );

    lua.new_enum("DeathAnimation",
        "Human", DeathAnimationId::Human,
        "Heretic", DeathAnimationId::Heretic,
        "Dwarf", DeathAnimationId::Dwarf,
        "Undead", DeathAnimationId::Undead,
        "Neutral", DeathAnimationId::Neutral,
        "Dragon", DeathAnimationId::Dragon,
        "Ghost", DeathAnimationId::Ghost,
        "Elf", DeathAnimationId::Elf
    );
    // clang-format on

    bindings::UnitView::bind(lua);
    bindings::UnitImplView::bind(lua);
    bindings::UnitSlotView::bind(lua);
    bindings::DynUpgradeView::bind(lua);
    bindings::ScenarioView::bind(lua);
    bindings::LocationView::bind(lua);
    bindings::Point::bind(lua);
    bindings::IdView::bind(lua);
    bindings::ScenVariablesView::bind(lua);
    bindings::ScenarioVariableView::bind(lua);
    bindings::TileView::bind(lua);
    bindings::StackView::bind(lua);
    bindings::GroupView::bind(lua);
    bindings::AttackView::bind(lua);
    bindings::CurrencyView::bind(lua);
    bindings::ItemBaseView::bind(lua);
    bindings::ItemView::bind(lua);

    lua.set_function("log", [](const std::string& message) { logDebug("luaDebug.log", message); });
}

// https://sol2.readthedocs.io/en/latest/threading.html
// Lua has no thread safety. sol does not force thread safety bottlenecks anywhere.
// Treat access and object handling like you were dealing with a raw int reference (int&).
sol::state& getLua()
{
    static std::map<std::thread::id, sol::state> states;
    static std::mutex statesMutex;

    const std::lock_guard<std::mutex> lock(statesMutex);

    auto key = std::this_thread::get_id();
    auto it = states.find(key);
    if (it != states.end())
        return it->second;

    auto& lua = states[key];
    lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::table,
                       sol::lib::os);
    bindApi(lua);
    return lua;
}

const std::string& getSource(const std::filesystem::path& path)
{
    static std::unordered_map<std::filesystem::path, std::string, PathHash> sources;
    static std::mutex sourcesMutex;

    const std::lock_guard<std::mutex> lock(sourcesMutex);

    auto it = sources.find(path);
    if (it != sources.end())
        return it->second;

    auto& source = sources[path];
    source = readFile(path);
    return source;
}

sol::environment executeScript(const std::string& source, sol::protected_function_result& result)
{
    auto& lua = getLua();

    // Environment prevents cluttering of global namespace by scripts
    // making each script run isolated from others.
    sol::environment env{lua, sol::create, lua.globals()};
    result = lua.safe_script(source, env,
                             [](lua_State*, sol::protected_function_result pfr) { return pfr; });
    return env;
}

std::optional<sol::environment> executeScriptFile(const std::filesystem::path& path,
                                                  bool alwaysExists)
{
    if (!alwaysExists && !std::filesystem::exists(path))
        return std::nullopt;

    const auto& source = getSource(path);
    if (source.empty()) {
        showErrorMessageBox(fmt::format("Failed to read '{:s}' script file.", path.string()));
        return std::nullopt;
    }

    sol::protected_function_result result;
    auto env = executeScript(source, result);
    if (!result.valid()) {
        const sol::error err = result;
        showErrorMessageBox(fmt::format("Failed to execute script '{:s}'.\n"
                                        "Reason: '{:s}'",
                                        path.string(), err.what()));
        return std::nullopt;
    }

    return {std::move(env)};
}

} // namespace hooks
