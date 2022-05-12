/*
 * This file is part of the modding toolset for Disciples 2.
 * (https://github.com/VladimirMakeev/D2ModdingToolset)
 * Copyright (C) 2021 Stanislav Egorov.
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

#include "unitutils.h"
#include "attack.h"
#include "attacksourcecat.h"
#include "attacksourcelist.h"
#include "attackutils.h"
#include "battlemsgdata.h"
#include "customattacks.h"
#include "custommodifier.h"
#include "dynamiccast.h"
#include "game.h"
#include "gameutils.h"
#include "globaldata.h"
#include "immunecat.h"
#include "log.h"
#include "midgardid.h"
#include "midstack.h"
#include "midunit.h"
#include "modifierutils.h"
#include "settings.h"
#include "umattack.h"
#include "ummodifier.h"
#include "unitgenerator.h"
#include "ussoldier.h"
#include "ussoldierimpl.h"
#include "usunitimpl.h"
#include "utils.h"
#include <fmt/format.h>

namespace hooks {

void generateUnitImplByAttackId(const game::CMidgardID* attackId)
{
    using namespace game;

    const auto& id = CMidgardIDApi::get();
    int typeIndex = id.getTypeIndex(attackId);
    int categoryIndex = id.getCategoryIndex(attackId);
    IdCategory category = id.getCategory(attackId);

    CMidgardID unitImplId{};
    id.fromParts(&unitImplId, category, categoryIndex, IdType::UnitGenerated, typeIndex);

    CUnitGenerator* unitGenerator = (*(GlobalDataApi::get().getGlobalData()))->unitGenerator;
    unitGenerator->vftable->generateUnitImpl(unitGenerator, &unitImplId);
}

game::IUsSoldier* castUnitImplToSoldierWithLogging(const game::IUsUnit* unitImpl)
{
    using namespace game;

    auto soldier = gameFunctions().castUnitImplToSoldier(unitImpl);
    if (!soldier) {
        hooks::logError("mssProxyError.log", fmt::format("Failed to cast unit impl {:s} to soldier",
                                                         hooks::idToString(&unitImpl->id)));
    }

    return soldier;
}

void getSoldierAttackSourceImmunities(const game::LImmuneCat* immuneCat,
                                      const game::IUsSoldier* soldier,
                                      game::List<game::LAttackSource>* value)
{
    using namespace game;

    const auto& sources = AttackSourceCategories::get();
    const auto& sourceListApi = AttackSourceListApi::get();
    if (immuneCat->id == soldier->vftable->getImmuneByAttackSource(soldier, sources.weapon)->id)
        sourceListApi.add(value, sources.weapon);
    if (immuneCat->id == soldier->vftable->getImmuneByAttackSource(soldier, sources.mind)->id)
        sourceListApi.add(value, sources.mind);
    if (immuneCat->id == soldier->vftable->getImmuneByAttackSource(soldier, sources.life)->id)
        sourceListApi.add(value, sources.life);
    if (immuneCat->id == soldier->vftable->getImmuneByAttackSource(soldier, sources.death)->id)
        sourceListApi.add(value, sources.death);
    if (immuneCat->id == soldier->vftable->getImmuneByAttackSource(soldier, sources.fire)->id)
        sourceListApi.add(value, sources.fire);
    if (immuneCat->id == soldier->vftable->getImmuneByAttackSource(soldier, sources.water)->id)
        sourceListApi.add(value, sources.water);
    if (immuneCat->id == soldier->vftable->getImmuneByAttackSource(soldier, sources.air)->id)
        sourceListApi.add(value, sources.air);
    if (immuneCat->id == soldier->vftable->getImmuneByAttackSource(soldier, sources.earth)->id)
        sourceListApi.add(value, sources.earth);

    for (const auto& custom : getCustomAttacks().sources) {
        if (immuneCat->id == soldier->vftable->getImmuneByAttackSource(soldier, &custom.source)->id)
            sourceListApi.add(value, &custom.source);
    }
}

bool isUnitSmall(const game::CMidUnit* unit)
{
    using namespace game;

    const auto& fn = gameFunctions();

    auto soldier = fn.castUnitImplToSoldier(unit->unitImpl);
    return soldier->vftable->getSizeSmall(soldier);
}

game::CMidgardID getGlobalUnitImplId(const game::CMidUnit* unit)
{
    using namespace game;

    CMidgardID globalImplId{};
    CUnitGenerator* unitGenerator = (*(GlobalDataApi::get().getGlobalData()))->unitGenerator;
    unitGenerator->vftable->getGlobalUnitImplId(unitGenerator, &globalImplId, &unit->unitImpl->id);

    return globalImplId;
}

game::TUsUnitImpl* getGlobalUnitImpl(const game::CMidUnit* unit)
{
    const game::CMidgardID globalImplId{getGlobalUnitImplId(unit)};
    return getGlobalUnitImpl(&globalImplId);
}

game::TUsUnitImpl* getGlobalUnitImpl(const game::CMidgardID* globalUnitImplId)
{
    using namespace game;

    const auto& global = GlobalDataApi::get();

    auto globalData = *global.getGlobalData();
    auto result = (TUsUnitImpl*)global.findById(globalData->units, globalUnitImplId);
    if (!result) {
        logError("mssProxyError.log",
                 fmt::format("Could not find unit impl {:s}", idToString(globalUnitImplId)));
    }

    return result;
}

game::TUsSoldierImpl* getSoldierImpl(const game::IUsUnit* unit)
{
    using namespace game;

    const auto& rtti = RttiApi::rtti();
    const auto dynamicCast = RttiApi::get().dynamicCast;

    auto current = unit;
    while (current) {
        auto soldierImpl = (TUsSoldierImpl*)dynamicCast(current, 0, rtti.IUsUnitType,
                                                        rtti.TUsSoldierImplType, 0);
        if (soldierImpl)
            return soldierImpl;

        auto modifier = (CUmModifier*)dynamicCast(current, 0, rtti.IUsUnitType,
                                                  rtti.CUmModifierType, 0);
        current = modifier ? modifier->data->prev : nullptr;
    }

    return nullptr;
}

game::TUsSoldierImpl* getSoldierImpl(const game::IUsSoldier* soldier)
{
    using namespace game;

    const auto& rtti = RttiApi::rtti();
    const auto dynamicCast = RttiApi::get().dynamicCast;

    auto unit = (IUsUnit*)dynamicCast(soldier, 0, rtti.IUsSoldierType, rtti.IUsUnitType, 0);
    return getSoldierImpl(unit);
}

game::IAttack* getAttack(const game::IUsSoldier* soldier, bool primary, bool checkAltAttack)
{
    using namespace game;

    auto attack = primary ? soldier->vftable->getAttackById(soldier)
                          : soldier->vftable->getSecondAttackById(soldier);

    if (checkAltAttack && attack != nullptr) {
        auto altAttack = getAltAttack(soldier, attack, primary);
        if (altAttack)
            return altAttack;
    }

    return attack;
}

game::IAttack* getAltAttack(const game::IUsSoldier* soldier,
                            const game::IAttack* attack,
                            bool primary)
{
    using namespace game;

    const auto& rtti = RttiApi::rtti();
    const auto dynamicCast = RttiApi::get().dynamicCast;
    const auto& attackModifiedApi = CAttackModifiedApi::get();

    auto altAttackId = attack->vftable->getAltAttackId(attack);
    if (*altAttackId == emptyId)
        return nullptr;

    auto result = getGlobalAttack(altAttackId);
    if (result == nullptr)
        return nullptr;

    auto unitImpl = (IUsUnit*)dynamicCast(soldier, 0, rtti.IUsSoldierType, rtti.IUsUnitType, 0);
    for (CUmModifier* curr = getFirstUmModifier(unitImpl); curr; curr = curr->data->next) {
        auto umAttack = castUmModifierToUmAttack(curr);
        if (umAttack) {
            auto attackModified = &umAttack->data->attackModified;
            attackModifiedApi.wrap(attackModified, result);
            result = attackModified;
        } else {
            auto custom = castModifierToCustomModifier(curr);
            if (custom) {
                result = custom->wrap(result, primary);
            }
        }
    }

    return result;
}

int getArmor(const game::CMidgardID* unitId,
             const game::IUsSoldier* soldier,
             const game::BattleMsgData* battleMsgData,
             bool includeShattered,
             bool includeFortification)
{
    using namespace game;

    const auto& battle = BattleMsgDataApi::get();

    int armor;
    soldier->vftable->getArmor(soldier, &armor);

    if (!includeShattered)
        armor -= battle.getUnitShatteredArmor(battleMsgData, unitId);
    if (!includeFortification)
        armor -= battle.getUnitFortificationArmor(battleMsgData, unitId);

    return armor > 0 ? armor : 0;
}

int computeUnitEffectiveHp(const game::CMidUnit* unit, int armor)
{
    if (!unit || unit->currentHp < 0)
        return 0;

    if (!userSettings().fixEffectiveHpFormula) {
        return unit->currentHp * armor / 100 + unit->currentHp;
    }

    if (armor > 99)
        return std::numeric_limits<int>::max();

    double factor = 1 - (double)armor / 100;
    return lround((double)unit->currentHp / factor);
}

int computeShatterDamage(const game::CMidgardID* unitId,
                         const game::IUsSoldier* soldier,
                         const game::BattleMsgData* battleMsgData,
                         const game::IAttack* attack)
{
    using namespace game;

    const auto& battle = BattleMsgDataApi::get();

    int armor = getArmor(unitId, soldier, battleMsgData, false, false);

    int limit = userSettings().shatteredArmorMax
                - battle.getUnitShatteredArmor(battleMsgData, unitId);

    int result = attack->vftable->getQtyDamage(attack);
    if (result > armor)
        result = armor;
    if (result > limit)
        result = limit;
    if (result > userSettings().shatterDamageMax)
        result = userSettings().shatterDamageMax;

    return result;
}

void updateAttackCountAfterTransformation(game::BattleMsgData* battleMsgData,
                                          const game::CMidUnit* unit,
                                          bool prevAttackTwice)
{
    using namespace game;

    for (auto& turn : battleMsgData->turnsOrder) {
        if (turn.unitId == unit->id) {
            const auto soldier = gameFunctions().castUnitImplToSoldier(unit->unitImpl);
            bool attackTwice = soldier && soldier->vftable->getAttackTwice(soldier);

            if (!prevAttackTwice && attackTwice)
                turn.attackCount++;
            else if (prevAttackTwice && !attackTwice && turn.attackCount > 1)
                turn.attackCount--;

            break;
        }
    }
}

bool isStackLeaderAndAllowedToUseBattleItems(const game::IMidgardObjectMap* objectMap,
                                             const game::CMidgardID* unitId,
                                             const game::BattleMsgData* battleMsgData)
{
    using namespace game;

    const auto& battleApi = BattleMsgDataApi::get();

    auto stack = getStack(objectMap, battleMsgData, unitId);
    if (!stack || stack->leaderId != *unitId)
        return false;

    if (battleApi.getUnitStatus(battleMsgData, unitId, BattleStatus::Transform))
        return userSettings().allowBattleItems.onTransformOther;
    else if (battleApi.getUnitStatus(battleMsgData, unitId, BattleStatus::TransformSelf))
        return userSettings().allowBattleItems.onTransformSelf;
    else if (battleApi.getUnitStatus(battleMsgData, unitId, BattleStatus::TransformDrainLevel))
        return userSettings().allowBattleItems.onDrainLevel;
    else if (battleApi.getUnitStatus(battleMsgData, unitId, BattleStatus::TransformDoppelganger))
        return userSettings().allowBattleItems.onDoppelganger;

    return true;
}

} // namespace hooks
