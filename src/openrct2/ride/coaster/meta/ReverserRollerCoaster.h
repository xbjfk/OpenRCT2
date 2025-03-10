/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../../sprites.h"
#include "../../RideData.h"
#include "../../ShopItem.h"
#include "../../Track.h"

// clang-format off
constexpr const RideTypeDescriptor ReverserRollerCoasterRTD =
{
    SET_FIELD(AlternateType, RIDE_TYPE_NULL),
    SET_FIELD(Category, RIDE_CATEGORY_ROLLERCOASTER),
    SET_FIELD(EnabledTrackPieces, {TRACK_STRAIGHT, TRACK_STATION_END, TRACK_LIFT_HILL, TRACK_SLOPE, TRACK_S_BEND, TRACK_CURVE_SMALL, TRACK_CURVE, TRACK_BRAKES, TRACK_REVERSER}),
    SET_FIELD(ExtraTrackPieces, {}),
    SET_FIELD(CoveredTrackPieces, {}),
    SET_FIELD(StartTrackPiece, TrackElemType::EndStation),
    SET_FIELD(TrackPaintFunction, GetTrackPaintFunctionReverserRC),
    SET_FIELD(Flags, RIDE_TYPE_FLAGS_TRACK_HAS_3_COLOURS | RIDE_TYPE_FLAG_HAS_LEAVE_WHEN_ANOTHER_VEHICLE_ARRIVES_AT_STATION |
                     RIDE_TYPE_FLAGS_COMMON_COASTER | RIDE_TYPE_FLAGS_COMMON_COASTER_NON_ALT | RIDE_TYPE_FLAG_PEEP_CHECK_GFORCES),
    SET_FIELD(RideModes, EnumsToFlags(RideMode::ContinuousCircuit)),
    SET_FIELD(DefaultMode, RideMode::ContinuousCircuit),
    SET_FIELD(OperatingSettings, { 0, 0, 0, 0, 0, 0 }),
    SET_FIELD(Naming, { STR_RIDE_NAME_REVERSER_ROLLER_COASTER, STR_RIDE_DESCRIPTION_REVERSER_ROLLER_COASTER }),
    SET_FIELD(NameConvention, { RideComponentType::Car, RideComponentType::Track, RideComponentType::Station }),
    SET_FIELD(EnumName, nameof(RIDE_TYPE_REVERSER_ROLLER_COASTER)),
    SET_FIELD(AvailableBreakdowns, (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE)),
    SET_FIELD(Heights, { 18, 24, 8, 11, }),
    SET_FIELD(MaxMass, 15),
    SET_FIELD(LiftData, { OpenRCT2::Audio::SoundId::LiftWood, 3, 4 }),
    SET_FIELD(RatingsMultipliers, { 48, 28, 7 }),
    SET_FIELD(UpkeepCosts, { 39, 20, 80, 10, 3, 10 }),
    SET_FIELD(BuildCosts, { 27.50_GBP, 3.00_GBP, 37, }),
    SET_FIELD(DefaultPrices, { 20, 20 }),
    SET_FIELD(DefaultMusic, MUSIC_OBJECT_TECHNO),
    SET_FIELD(PhotoItem, ShopItem::Photo3),
    SET_FIELD(BonusValue, 65),
    SET_FIELD(ColourPresets, TRACK_COLOUR_PRESETS(
        { COLOUR_DARK_BROWN, COLOUR_LIGHT_BROWN, COLOUR_DARK_BROWN },
        { COLOUR_SATURATED_BROWN, COLOUR_BORDEAUX_RED, COLOUR_SATURATED_BROWN },
    )),
    SET_FIELD(ColourPreview, { SPR_RIDE_DESIGN_PREVIEW_REVERSER_ROLLER_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_REVERSER_ROLLER_COASTER_SUPPORTS }),
    SET_FIELD(ColourKey, RideColourKey::Ride),
    SET_FIELD(Name, "reverser_rc"),
    SET_FIELD(RatingsData,
    {
        RatingsCalculationType::Normal,
        { RIDE_RATING(2, 40), RIDE_RATING(1, 80), RIDE_RATING(1, 70) },
        19,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,           6000,     873, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,        RIDE_RATING(0, 40), RIDE_RATING(0, 05), 0 },
            { RatingsModifierType::BonusTrainLength,      0,        187245, 0, 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,        44281, 88562, 35424 },
            { RatingsModifierType::BonusAverageSpeed,     0,        364088, 655360, 0 },
            { RatingsModifierType::BonusReversals,        6,        RIDE_RATING(0, 20), RIDE_RATING(0, 20), RIDE_RATING(0, 20) },
            { RatingsModifierType::BonusDuration,         150,      26214, 0, 0 },
            { RatingsModifierType::BonusGForces,          0,        28672, 23831, 49648 },
            { RatingsModifierType::BonusTurns,            0,        26749, 43458, 45749 },
            { RatingsModifierType::BonusDrops,            0,        40777, 46811, 49152 },
            { RatingsModifierType::BonusSheltered,        0,        16705, 30583, 35108 },
            { RatingsModifierType::BonusProximity,        0,        22367, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,        11155, 0, 0 },
            { RatingsModifierType::RequirementReversals,  1,        8, 1, 1 },
            { RatingsModifierType::RequirementLength,     0xC80000, 2, 1, 1 },
            { RatingsModifierType::RequirementNumDrops,   2,        2, 1, 1 },
            { RatingsModifierType::PenaltyLateralGs,      0,        28672, 23831, 49648 },
        },
    }),
};
// clang-format on
