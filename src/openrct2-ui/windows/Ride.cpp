/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../interface/Theme.h"

#include <algorithm>
#include <cmath>
#include <iterator>
#include <limits>
#include <memory>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Cheats.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/Input.h>
#include <openrct2/Limits.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/actions/GameAction.h>
#include <openrct2/actions/ParkSetParameterAction.h>
#include <openrct2/actions/RideSetAppearanceAction.h>
#include <openrct2/actions/RideSetColourSchemeAction.h>
#include <openrct2/actions/RideSetNameAction.h>
#include <openrct2/actions/RideSetPriceAction.h>
#include <openrct2/actions/RideSetSettingAction.h>
#include <openrct2/actions/RideSetStatusAction.h>
#include <openrct2/audio/audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/String.hpp>
#include <openrct2/entity/EntityList.h>
#include <openrct2/entity/Staff.h>
#include <openrct2/localisation/Date.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/localisation/LocalisationService.h>
#include <openrct2/localisation/StringIds.h>
#include <openrct2/network/network.h>
#include <openrct2/object/MusicObject.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/ObjectRepository.h>
#include <openrct2/object/StationObject.h>
#include <openrct2/rct1/RCT1.h>
#include <openrct2/rct2/T6Exporter.h>
#include <openrct2/ride/RideConstruction.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/ShopItem.h>
#include <openrct2/ride/Station.h>
#include <openrct2/ride/Track.h>
#include <openrct2/ride/TrackData.h>
#include <openrct2/ride/TrackDesign.h>
#include <openrct2/ride/TrackDesignRepository.h>
#include <openrct2/ride/Vehicle.h>
#include <openrct2/sprites.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Park.h>
#include <optional>
#include <string>
#include <vector>

using namespace OpenRCT2;
using namespace OpenRCT2::TrackMetaData;

static constexpr const StringId WINDOW_TITLE = STR_RIDE_WINDOW_TITLE;
static constexpr const int32_t WH = 207;
static constexpr const int32_t WW = 316;

enum
{
    WINDOW_RIDE_PAGE_MAIN,
    WINDOW_RIDE_PAGE_VEHICLE,
    WINDOW_RIDE_PAGE_OPERATING,
    WINDOW_RIDE_PAGE_MAINTENANCE,
    WINDOW_RIDE_PAGE_COLOUR,
    WINDOW_RIDE_PAGE_MUSIC,
    WINDOW_RIDE_PAGE_MEASUREMENTS,
    WINDOW_RIDE_PAGE_GRAPHS,
    WINDOW_RIDE_PAGE_INCOME,
    WINDOW_RIDE_PAGE_CUSTOMER,
    WINDOW_RIDE_PAGE_COUNT
};

#pragma region Widgets

// clang-format off
enum {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_PAGE_BACKGROUND,
    WIDX_TAB_1,
    WIDX_TAB_2,
    WIDX_TAB_3,
    WIDX_TAB_4,
    WIDX_TAB_5,
    WIDX_TAB_6,
    WIDX_TAB_7,
    WIDX_TAB_8,
    WIDX_TAB_9,
    WIDX_TAB_10,

    WIDX_VIEWPORT = 14,
    WIDX_VIEW,
    WIDX_VIEW_DROPDOWN,
    WIDX_STATUS,
    WIDX_OPEN,
    WIDX_CONSTRUCTION,
    WIDX_RENAME,
    WIDX_LOCATE,
    WIDX_DEMOLISH,
    WIDX_CLOSE_LIGHT,
    WIDX_SIMULATE_LIGHT,
    WIDX_TEST_LIGHT,
    WIDX_OPEN_LIGHT,
    WIDX_RIDE_TYPE,
    WIDX_RIDE_TYPE_DROPDOWN,

    WIDX_VEHICLE_TYPE = 14,
    WIDX_VEHICLE_TYPE_DROPDOWN,
    WIDX_VEHICLE_REVERSED_TRAINS_CHECKBOX,
    WIDX_VEHICLE_TRAINS_PREVIEW,
    WIDX_VEHICLE_TRAINS,
    WIDX_VEHICLE_TRAINS_INCREASE,
    WIDX_VEHICLE_TRAINS_DECREASE,
    WIDX_VEHICLE_CARS_PER_TRAIN,
    WIDX_VEHICLE_CARS_PER_TRAIN_INCREASE,
    WIDX_VEHICLE_CARS_PER_TRAIN_DECREASE,

    WIDX_MODE_TWEAK = 14,
    WIDX_MODE_TWEAK_INCREASE,
    WIDX_MODE_TWEAK_DECREASE,
    WIDX_LIFT_HILL_SPEED,
    WIDX_LIFT_HILL_SPEED_INCREASE,
    WIDX_LIFT_HILL_SPEED_DECREASE,
    WIDX_LOAD_CHECKBOX,
    WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX,
    WIDX_MINIMUM_LENGTH_CHECKBOX,
    WIDX_MINIMUM_LENGTH,
    WIDX_MINIMUM_LENGTH_INCREASE,
    WIDX_MINIMUM_LENGTH_DECREASE,
    WIDX_MAXIMUM_LENGTH_CHECKBOX,
    WIDX_MAXIMUM_LENGTH,
    WIDX_MAXIMUM_LENGTH_INCREASE,
    WIDX_MAXIMUM_LENGTH_DECREASE,
    WIDX_SYNCHRONISE_WITH_ADJACENT_STATIONS_CHECKBOX,
    WIDX_MODE_TWEAK_LABEL,
    WIDX_LIFT_HILL_SPEED_LABEL,
    WIDX_MODE,
    WIDX_MODE_DROPDOWN,
    WIDX_LOAD,
    WIDX_LOAD_DROPDOWN,
    WIDX_OPERATE_NUMBER_OF_CIRCUITS_LABEL,
    WIDX_OPERATE_NUMBER_OF_CIRCUITS,
    WIDX_OPERATE_NUMBER_OF_CIRCUITS_INCREASE,
    WIDX_OPERATE_NUMBER_OF_CIRCUITS_DECREASE,

    WIDX_INSPECTION_INTERVAL = 14,
    WIDX_INSPECTION_INTERVAL_DROPDOWN,
    WIDX_LOCATE_MECHANIC,
    WIDX_REFURBISH_RIDE,
    WIDX_FORCE_BREAKDOWN,

    WIDX_TRACK_PREVIEW = 14,
    WIDX_TRACK_COLOUR_SCHEME,
    WIDX_TRACK_COLOUR_SCHEME_DROPDOWN,
    WIDX_TRACK_MAIN_COLOUR,
    WIDX_TRACK_ADDITIONAL_COLOUR,
    WIDX_TRACK_SUPPORT_COLOUR,
    WIDX_MAZE_STYLE,
    WIDX_MAZE_STYLE_DROPDOWN,
    WIDX_PAINT_INDIVIDUAL_AREA,
    WIDX_ENTRANCE_PREVIEW,
    WIDX_ENTRANCE_STYLE,
    WIDX_ENTRANCE_STYLE_DROPDOWN,
    WIDX_VEHICLE_PREVIEW,
    WIDX_VEHICLE_COLOUR_SCHEME,
    WIDX_VEHICLE_COLOUR_SCHEME_DROPDOWN,
    WIDX_VEHICLE_COLOUR_INDEX,
    WIDX_VEHICLE_COLOUR_INDEX_DROPDOWN,
    WIDX_VEHICLE_BODY_COLOUR,
    WIDX_VEHICLE_TRIM_COLOUR,
    WIDX_VEHICLE_TERNARY_COLOUR,
    WIDX_SELL_ITEM_RANDOM_COLOUR_CHECKBOX,

    WIDX_PLAY_MUSIC = 14,
    WIDX_MUSIC,
    WIDX_MUSIC_DROPDOWN,

    WIDX_SAVE_TRACK_DESIGN = 14,
    WIDX_SELECT_NEARBY_SCENERY,
    WIDX_RESET_SELECTION,
    WIDX_SAVE_DESIGN,
    WIDX_CANCEL_DESIGN,

    WIDX_GRAPH = 14,
    WIDX_GRAPH_VELOCITY,
    WIDX_GRAPH_ALTITUDE,
    WIDX_GRAPH_VERTICAL,
    WIDX_GRAPH_LATERAL,

    WIDX_PRIMARY_PRICE_LABEL = 14,
    WIDX_PRIMARY_PRICE,
    WIDX_PRIMARY_PRICE_INCREASE,
    WIDX_PRIMARY_PRICE_DECREASE,
    WIDX_PRIMARY_PRICE_SAME_THROUGHOUT_PARK,
    WIDX_SECONDARY_PRICE_LABEL,
    WIDX_SECONDARY_PRICE,
    WIDX_SECONDARY_PRICE_INCREASE,
    WIDX_SECONDARY_PRICE_DECREASE,
    WIDX_SECONDARY_PRICE_SAME_THROUGHOUT_PARK,

    WIDX_SHOW_GUESTS_THOUGHTS = 14,
    WIDX_SHOW_GUESTS_ON_RIDE,
    WIDX_SHOW_GUESTS_QUEUING
};

constexpr int32_t RCT1_LIGHT_OFFSET = 4;

#define MAIN_RIDE_WIDGETS \
    WINDOW_SHIM(WINDOW_TITLE, WW, WH), \
    MakeWidget({  0, 43}, {316, 137}, WindowWidgetType::Resize, WindowColour::Secondary), \
    MakeTab   ({  3, 17}, STR_VIEW_OF_RIDE_ATTRACTION_TIP                ), \
    MakeTab   ({ 34, 17}, STR_VEHICLE_DETAILS_AND_OPTIONS_TIP            ), \
    MakeTab   ({ 65, 17}, STR_OPERATING_OPTIONS_TIP                      ), \
    MakeTab   ({ 96, 17}, STR_MAINTENANCE_OPTIONS_TIP                    ), \
    MakeTab   ({127, 17}, STR_COLOUR_SCHEME_OPTIONS_TIP                  ), \
    MakeTab   ({158, 17}, STR_SOUND_AND_MUSIC_OPTIONS_TIP                ), \
    MakeTab   ({189, 17}, STR_MEASUREMENTS_AND_TEST_DATA_TIP             ), \
    MakeTab   ({220, 17}, STR_GRAPHS_TIP                                 ), \
    MakeTab   ({251, 17}, STR_INCOME_AND_COSTS_TIP                       ), \
    MakeTab   ({282, 17}, STR_CUSTOMER_INFORMATION_TIP                   )

// 0x009ADC34
static Widget window_ride_main_widgets[] = {
    MAIN_RIDE_WIDGETS,
    MakeWidget({  3,  60}, {288, 107}, WindowWidgetType::Viewport,      WindowColour::Secondary, STR_VIEWPORT                                           ),
    MakeWidget({ 35,  46}, {222,  12}, WindowWidgetType::DropdownMenu,      WindowColour::Secondary, 0xFFFFFFFF,                 STR_VIEW_SELECTION         ),
    MakeWidget({245,  47}, { 11,  10}, WindowWidgetType::Button,        WindowColour::Secondary, STR_DROPDOWN_GLYPH,         STR_VIEW_SELECTION         ),
    MakeWidget({  3, 167}, {288,  11}, WindowWidgetType::LabelCentred, WindowColour::Secondary                                                         ),
    MakeWidget({291,  46}, { 24,  24}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, 0xFFFFFFFF,                 STR_OPEN_CLOSE_OR_TEST_RIDE),
    MakeWidget({291,  70}, { 24,  24}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, ImageId(SPR_CONSTRUCTION),           STR_CONSTRUCTION           ),
    MakeWidget({291,  94}, { 24,  24}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, ImageId(SPR_RENAME),                 STR_NAME_RIDE_TIP          ),
    MakeWidget({291, 118}, { 24,  24}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, ImageId(SPR_LOCATE),                 STR_LOCATE_SUBJECT_TIP     ),
    MakeWidget({291, 142}, { 24,  24}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, ImageId(SPR_DEMOLISH),               STR_DEMOLISH_RIDE_TIP      ),
    MakeWidget({296,  48}, { 14,  14}, WindowWidgetType::ImgBtn,        WindowColour::Secondary, ImageId(SPR_G2_RCT1_CLOSE_BUTTON_0), STR_CLOSE_RIDE_TIP         ),
    MakeWidget({296,  62}, { 14,  14}, WindowWidgetType::ImgBtn,        WindowColour::Secondary, ImageId(SPR_G2_RCT1_TEST_BUTTON_0),  STR_SIMULATE_RIDE_TIP      ),
    MakeWidget({296,  62}, { 14,  14}, WindowWidgetType::ImgBtn,        WindowColour::Secondary, ImageId(SPR_G2_RCT1_TEST_BUTTON_0),  STR_TEST_RIDE_TIP          ),
    MakeWidget({296,  76}, { 14,  14}, WindowWidgetType::ImgBtn,        WindowColour::Secondary, ImageId(SPR_G2_RCT1_OPEN_BUTTON_0),  STR_OPEN_RIDE_TIP          ),
    MakeWidget({  3, 180}, {305,  12}, WindowWidgetType::DropdownMenu,      WindowColour::Secondary, STR_ARG_6_STRINGID                                     ),
    MakeWidget({297, 180}, { 11,  12}, WindowWidgetType::Button,        WindowColour::Secondary, STR_DROPDOWN_GLYPH                                     ),
    WIDGETS_END,
};

// 0x009ADDA8
static Widget window_ride_vehicle_widgets[] = {
    MAIN_RIDE_WIDGETS,
    MakeWidget        ({  7,  50}, {302, 12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                                                    ),
    MakeWidget        ({297,  51}, { 11, 10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH                                ),
    MakeWidget        ({  7, 137}, {302, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_OPTION_REVERSE_TRAINS, STR_OPTION_REVERSE_TRAINS_TIP  ),
    MakeWidget        ({  7, 154}, {302, 43}, WindowWidgetType::Scroll,   WindowColour::Secondary, STR_EMPTY                                         ),
    MakeSpinnerWidgets({  7, 203}, {145, 12}, WindowWidgetType::Spinner,  WindowColour::Secondary, STR_RIDE_VEHICLE_COUNT, STR_MAX_VEHICLES_TIP      ),
    MakeSpinnerWidgets({164, 203}, {145, 12}, WindowWidgetType::Spinner,  WindowColour::Secondary, STR_1_CAR_PER_TRAIN,    STR_MAX_CARS_PER_TRAIN_TIP),
    WIDGETS_END,
};

// 0x009ADEFC
static Widget window_ride_operating_widgets[] = {
    MAIN_RIDE_WIDGETS,
    MakeSpinnerWidgets({157,  61}, {152, 12}, WindowWidgetType::Spinner,  WindowColour::Secondary, STR_ARG_18_STRINGID                                                                 ), // NB: 3 widgets
    MakeSpinnerWidgets({157,  75}, {152, 12}, WindowWidgetType::Spinner,  WindowColour::Secondary, STR_LIFT_HILL_CHAIN_SPEED_VALUE                                                     ), // NB: 3 widgets
    MakeWidget        ({  7, 109}, { 80, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_WAIT_FOR,                           STR_WAIT_FOR_PASSENGERS_BEFORE_DEPARTING_TIP),
    MakeWidget        ({  7, 124}, {302, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary                                                                                      ),
    MakeWidget        ({  7, 139}, {150, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_MINIMUM_WAITING_TIME,               STR_MINIMUM_LENGTH_BEFORE_DEPARTING_TIP     ),
    MakeSpinnerWidgets({157, 139}, {152, 12}, WindowWidgetType::Spinner,  WindowColour::Secondary, STR_ARG_10_STRINGID                                                                 ), // NB: 3 widgets
    MakeWidget        ({  7, 154}, {150, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_MAXIMUM_WAITING_TIME,               STR_MAXIMUM_LENGTH_BEFORE_DEPARTING_TIP     ),
    MakeSpinnerWidgets({157, 154}, {152, 12}, WindowWidgetType::Spinner,  WindowColour::Secondary, STR_ARG_14_STRINGID                                                                 ), // NB: 3 widgets
    MakeWidget        ({  7, 169}, {302, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_SYNCHRONISE_WITH_ADJACENT_STATIONS, STR_SYNCHRONISE_WITH_ADJACENT_STATIONS_TIP  ),
    MakeWidget        ({ 21,  61}, {129, 12}, WindowWidgetType::Label,    WindowColour::Secondary                                                                                      ),
    MakeWidget        ({ 21,  75}, {129, 12}, WindowWidgetType::Label,    WindowColour::Secondary, STR_LIFT_HILL_CHAIN_SPEED                                                           ),
    MakeWidget        ({  7,  47}, {302, 12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, 0xFFFFFFFF,                             STR_SELECT_OPERATING_MODE                   ),
    MakeWidget        ({297,  48}, { 11, 10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH,                     STR_SELECT_OPERATING_MODE                   ),
    MakeWidget        ({ 87, 109}, {222, 12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                                                                                      ),
    MakeWidget        ({297, 110}, { 11, 10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH                                                                  ),
    MakeWidget        ({ 21,  89}, {129, 12}, WindowWidgetType::Label,    WindowColour::Secondary, STR_NUMBER_OF_CIRCUITS,                 STR_NUMBER_OF_CIRCUITS_TIP                  ),
    MakeSpinnerWidgets({157,  89}, {152, 12}, WindowWidgetType::Spinner,  WindowColour::Secondary, STR_NUMBER_OF_CIRCUITS_VALUE                                                        ), // NB: 3 widgets
    WIDGETS_END,
};

// 0x009AE190
static Widget window_ride_maintenance_widgets[] = {
    MAIN_RIDE_WIDGETS,
    MakeWidget({107,  71}, {202, 12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, STR_EMPTY,          STR_SELECT_HOW_OFTEN_A_MECHANIC_SHOULD_CHECK_THIS_RIDE),
    MakeWidget({297,  72}, { 11, 10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH, STR_SELECT_HOW_OFTEN_A_MECHANIC_SHOULD_CHECK_THIS_RIDE),
    MakeWidget({289, 108}, { 24, 24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, 0xFFFFFFFF,         STR_LOCATE_NEAREST_AVAILABLE_MECHANIC_TIP             ),
    MakeWidget({265, 108}, { 24, 24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_CONSTRUCTION),   STR_REFURBISH_RIDE_TIP                                ),
    MakeWidget({241, 108}, { 24, 24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_NO_ENTRY),       STR_DEBUG_FORCE_BREAKDOWN_TIP                         ),
    WIDGETS_END,
};

// 0x009AE2A4
static Widget window_ride_colour_widgets[] = {
    MAIN_RIDE_WIDGETS,
    MakeWidget({  3,  47}, { 68, 47}, WindowWidgetType::Spinner,   WindowColour::Secondary                                                                    ),
    MakeWidget({ 74,  49}, {239, 12}, WindowWidgetType::DropdownMenu,  WindowColour::Secondary, STR_ARG_14_STRINGID                                               ),
    MakeWidget({301,  50}, { 11, 10}, WindowWidgetType::Button,    WindowColour::Secondary, STR_DROPDOWN_GLYPH,  STR_COLOUR_SCHEME_TO_CHANGE_TIP              ),
    MakeWidget({ 79,  74}, { 12, 12}, WindowWidgetType::ColourBtn, WindowColour::Secondary, 0xFFFFFFFF,          STR_SELECT_MAIN_COLOUR_TIP                   ),
    MakeWidget({ 99,  74}, { 12, 12}, WindowWidgetType::ColourBtn, WindowColour::Secondary, 0xFFFFFFFF,          STR_SELECT_ADDITIONAL_COLOUR_1_TIP           ),
    MakeWidget({119,  74}, { 12, 12}, WindowWidgetType::ColourBtn, WindowColour::Secondary, 0xFFFFFFFF,          STR_SELECT_SUPPORT_STRUCTURE_COLOUR_TIP      ),
    MakeWidget({ 74,  49}, {239, 12}, WindowWidgetType::DropdownMenu,  WindowColour::Secondary                                                                    ),
    MakeWidget({301,  50}, { 11, 10}, WindowWidgetType::Button,    WindowColour::Secondary, STR_DROPDOWN_GLYPH                                                ),
    MakeWidget({289,  68}, { 24, 24}, WindowWidgetType::FlatBtn,   WindowColour::Secondary, ImageId(SPR_PAINTBRUSH),      STR_PAINT_INDIVIDUAL_AREA_TIP                ),
    MakeWidget({245, 101}, { 68, 47}, WindowWidgetType::Spinner,   WindowColour::Secondary                                                                    ),
    MakeWidget({103, 103}, {139, 12}, WindowWidgetType::DropdownMenu,  WindowColour::Secondary, STR_EMPTY                                                         ),
    MakeWidget({230, 104}, { 11, 10}, WindowWidgetType::Button,    WindowColour::Secondary, STR_DROPDOWN_GLYPH,  STR_SELECT_STYLE_OF_ENTRANCE_EXIT_STATION_TIP),
    MakeWidget({  3, 157}, { 68, 47}, WindowWidgetType::Scroll,    WindowColour::Secondary, STR_EMPTY                                                         ),
    MakeWidget({ 74, 157}, {239, 12}, WindowWidgetType::DropdownMenu,  WindowColour::Secondary, STR_ARG_6_STRINGID                                                ),
    MakeWidget({301, 158}, { 11, 10}, WindowWidgetType::Button,    WindowColour::Secondary, STR_DROPDOWN_GLYPH,  STR_SELECT_VEHICLE_COLOUR_SCHEME_TIP         ),
    MakeWidget({ 74, 173}, {239, 12}, WindowWidgetType::DropdownMenu,  WindowColour::Secondary                                                                    ),
    MakeWidget({301, 174}, { 11, 10}, WindowWidgetType::Button,    WindowColour::Secondary, STR_DROPDOWN_GLYPH,  STR_SELECT_VEHICLE_TO_MODIFY_TIP             ),
    MakeWidget({ 79, 190}, { 12, 12}, WindowWidgetType::ColourBtn, WindowColour::Secondary, 0xFFFFFFFF,          STR_SELECT_MAIN_COLOUR_TIP                   ),
    MakeWidget({ 99, 190}, { 12, 12}, WindowWidgetType::ColourBtn, WindowColour::Secondary, 0xFFFFFFFF,          STR_SELECT_ADDITIONAL_COLOUR_1_TIP           ),
    MakeWidget({119, 190}, { 12, 12}, WindowWidgetType::ColourBtn, WindowColour::Secondary, 0xFFFFFFFF,          STR_SELECT_ADDITIONAL_COLOUR_2_TIP           ),
    MakeWidget({100,  74}, {239, 12}, WindowWidgetType::Checkbox,  WindowColour::Secondary, STR_RANDOM_COLOUR                                                 ),
    WIDGETS_END,
};

// 0x009AE4C8
static Widget window_ride_music_widgets[] = {
    MAIN_RIDE_WIDGETS,
    MakeWidget({  7, 47}, {302, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_PLAY_MUSIC,     STR_SELECT_MUSIC_TIP      ),
    MakeWidget({  7, 62}, {302, 12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, STR_EMPTY                                     ),
    MakeWidget({297, 63}, { 11, 10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH, STR_SELECT_MUSIC_STYLE_TIP),
    WIDGETS_END,
};

// 0x009AE5DC
static Widget window_ride_measurements_widgets[] = {
    MAIN_RIDE_WIDGETS,
    MakeWidget({288, 194}, { 24, 24}, WindowWidgetType::FlatBtn, WindowColour::Secondary, ImageId(SPR_FLOPPY),                STR_SAVE_TRACK_DESIGN),
    MakeWidget({  4, 127}, {154, 14}, WindowWidgetType::Button,  WindowColour::Secondary, STR_SELECT_NEARBY_SCENERY                       ),
    MakeWidget({158, 127}, {154, 14}, WindowWidgetType::Button,  WindowColour::Secondary, STR_RESET_SELECTION                             ),
    MakeWidget({  4, 177}, {154, 14}, WindowWidgetType::Button,  WindowColour::Secondary, STR_DESIGN_SAVE                                 ),
    MakeWidget({158, 177}, {154, 14}, WindowWidgetType::Button,  WindowColour::Secondary, STR_DESIGN_CANCEL                               ),
    WIDGETS_END,
};

// 0x009AE710
static Widget window_ride_graphs_widgets[] = {
    MAIN_RIDE_WIDGETS,
    MakeWidget({  3,  46}, {306, 112}, WindowWidgetType::Scroll, WindowColour::Secondary, SCROLL_HORIZONTAL,       STR_LOGGING_DATA_FROM_TIP                               ),
    MakeWidget({  3, 163}, { 73,  14}, WindowWidgetType::Button, WindowColour::Secondary, STR_RIDE_STATS_VELOCITY, STR_SHOW_GRAPH_OF_VELOCITY_AGAINST_TIME_TIP             ),
    MakeWidget({ 76, 163}, { 73,  14}, WindowWidgetType::Button, WindowColour::Secondary, STR_RIDE_STATS_ALTITUDE, STR_SHOW_GRAPH_OF_ALTITUDE_AGAINST_TIME_TIP             ),
    MakeWidget({149, 163}, { 73,  14}, WindowWidgetType::Button, WindowColour::Secondary, STR_RIDE_STATS_VERT_G,   STR_SHOW_GRAPH_OF_VERTICAL_ACCELERATION_AGAINST_TIME_TIP),
    MakeWidget({222, 163}, { 73,  14}, WindowWidgetType::Button, WindowColour::Secondary, STR_RIDE_STATS_LAT_G,    STR_SHOW_GRAPH_OF_LATERAL_ACCELERATION_AGAINST_TIME_TIP ),
    WIDGETS_END,
};

// 0x009AE844
static Widget window_ride_income_widgets[] = {
    MAIN_RIDE_WIDGETS,
    MakeWidget        ({ 19,  50}, {126, 14}, WindowWidgetType::Label,    WindowColour::Secondary                                                                    ),
    MakeSpinnerWidgets({147,  50}, {162, 14}, WindowWidgetType::Spinner,  WindowColour::Secondary, STR_ARG_6_CURRENCY2DP                                             ), // NB: 3 widgets
    MakeWidget        ({  5,  62}, {306, 13}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_SAME_PRICE_THROUGHOUT_PARK, STR_SAME_PRICE_THROUGHOUT_PARK_TIP),
    MakeWidget        ({ 19,  94}, {126, 14}, WindowWidgetType::Label,    WindowColour::Secondary                                                                    ),
    MakeSpinnerWidgets({147,  94}, {162, 14}, WindowWidgetType::Spinner,  WindowColour::Secondary, STR_RIDE_SECONDARY_PRICE_VALUE                                    ), // NB: 3 widgets
    MakeWidget        ({  5, 106}, {306, 13}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_SAME_PRICE_THROUGHOUT_PARK, STR_SAME_PRICE_THROUGHOUT_PARK_TIP),
    WIDGETS_END,
};

// 0x009AE9C8
static Widget window_ride_customer_widgets[] = {
    MAIN_RIDE_WIDGETS,
    MakeWidget({289,  54}, {24, 24}, WindowWidgetType::FlatBtn, WindowColour::Secondary, ImageId(SPR_SHOW_GUESTS_THOUGHTS_ABOUT_THIS_RIDE_ATTRACTION), STR_SHOW_GUESTS_THOUGHTS_ABOUT_THIS_RIDE_ATTRACTION_TIP),
    MakeWidget({289,  78}, {24, 24}, WindowWidgetType::FlatBtn, WindowColour::Secondary, ImageId(SPR_SHOW_GUESTS_ON_THIS_RIDE_ATTRACTION),             STR_SHOW_GUESTS_ON_THIS_RIDE_ATTRACTION_TIP            ),
    MakeWidget({289, 102}, {24, 24}, WindowWidgetType::FlatBtn, WindowColour::Secondary, ImageId(SPR_SHOW_GUESTS_QUEUING_FOR_THIS_RIDE_ATTRACTION),    STR_SHOW_GUESTS_QUEUING_FOR_THIS_RIDE_ATTRACTION_TIP   ),
    WIDGETS_END,
};

static Widget *window_ride_page_widgets[] = {
    window_ride_main_widgets,
    window_ride_vehicle_widgets,
    window_ride_operating_widgets,
    window_ride_maintenance_widgets,
    window_ride_colour_widgets,
    window_ride_music_widgets,
    window_ride_measurements_widgets,
    window_ride_graphs_widgets,
    window_ride_income_widgets,
    window_ride_customer_widgets,
};

static constexpr const uint64_t window_ride_page_hold_down_widgets[] = {
    0,
    (1uLL << WIDX_VEHICLE_TRAINS_INCREASE) |
        (1uLL << WIDX_VEHICLE_TRAINS_DECREASE) |
        (1uLL << WIDX_VEHICLE_CARS_PER_TRAIN_INCREASE) |
        (1uLL << WIDX_VEHICLE_CARS_PER_TRAIN_DECREASE),
    (1uLL << WIDX_MODE_TWEAK_INCREASE) |
        (1uLL << WIDX_MODE_TWEAK_DECREASE) |
        (1uLL << WIDX_LIFT_HILL_SPEED_INCREASE) |
        (1uLL << WIDX_LIFT_HILL_SPEED_DECREASE) |
        (1uLL << WIDX_MINIMUM_LENGTH_INCREASE) |
        (1uLL << WIDX_MINIMUM_LENGTH_DECREASE) |
        (1uLL << WIDX_MAXIMUM_LENGTH_INCREASE) |
        (1uLL << WIDX_MAXIMUM_LENGTH_DECREASE) |
        (1uLL << WIDX_OPERATE_NUMBER_OF_CIRCUITS_INCREASE) |
        (1uLL << WIDX_OPERATE_NUMBER_OF_CIRCUITS_DECREASE),
    0,
    0,
    0,
    0,
    0,
    (1uLL << WIDX_PRIMARY_PRICE_INCREASE) |
        (1uLL << WIDX_PRIMARY_PRICE_DECREASE) |
        (1uLL << WIDX_SECONDARY_PRICE_INCREASE) |
        (1uLL << WIDX_SECONDARY_PRICE_DECREASE),
    0,
};
// clang-format on

#pragma endregion

static void CancelScenerySelection();

static bool _collectTrackDesignScenery = false;
static int32_t _lastSceneryX = 0;
static int32_t _lastSceneryY = 0;
static std::unique_ptr<TrackDesign> _trackDesign;

// Cached overall view for each ride
// (Re)calculated when the ride window is opened
struct RideOverallView
{
    CoordsXYZ loc;
    ZoomLevel zoom;
};

static std::vector<RideOverallView> ride_overall_views = {};

static constexpr const int32_t window_ride_tab_animation_divisor[] = {
    0, // WINDOW_RIDE_PAGE_MAIN
    0, // WINDOW_RIDE_PAGE_VEHICLE
    2, // WINDOW_RIDE_PAGE_OPERATING
    2, // WINDOW_RIDE_PAGE_MAINTENANCE
    4, // WINDOW_RIDE_PAGE_COLOUR
    2, // WINDOW_RIDE_PAGE_MUSIC
    8, // WINDOW_RIDE_PAGE_MEASUREMENTS
    8, // WINDOW_RIDE_PAGE_GRAPHS
    2, // WINDOW_RIDE_PAGE_INCOME
    0, // WINDOW_RIDE_PAGE_CUSTOMER
};
static_assert(std::size(window_ride_tab_animation_divisor) == WINDOW_RIDE_PAGE_COUNT);

static constexpr const int32_t window_ride_tab_animation_frames[] = {
    0,  // WINDOW_RIDE_PAGE_MAIN
    0,  // WINDOW_RIDE_PAGE_VEHICLE
    4,  // WINDOW_RIDE_PAGE_OPERATING
    16, // WINDOW_RIDE_PAGE_MAINTENANCE
    8,  // WINDOW_RIDE_PAGE_COLOUR
    16, // WINDOW_RIDE_PAGE_MUSIC
    8,  // WINDOW_RIDE_PAGE_MEASUREMENTS
    8,  // WINDOW_RIDE_PAGE_GRAPHS
    8,  // WINDOW_RIDE_PAGE_INCOME
    0,  // WINDOW_RIDE_PAGE_CUSTOMER
};
static_assert(std::size(window_ride_tab_animation_frames) == WINDOW_RIDE_PAGE_COUNT);

// clang-format off
static constexpr const StringId RatingNames[] = {
    STR_RATING_LOW,
    STR_RATING_MEDIUM,
    STR_RATING_HIGH,
    STR_RATING_VERY_HIGH,
    STR_RATING_EXTREME,
    STR_RATING_ULTRA_EXTREME,
};
// clang-format on

static constexpr const StringId RideBreakdownReasonNames[] = {
    STR_RIDE_BREAKDOWN_SAFETY_CUT_OUT,          // BREAKDOWN_SAFETY_CUT_OUT
    STR_RIDE_BREAKDOWN_RESTRAINTS_STUCK_CLOSED, // BREAKDOWN_RESTRAINTS_STUCK_CLOSED
    STR_RIDE_BREAKDOWN_RESTRAINTS_STUCK_OPEN,   // BREAKDOWN_RESTRAINTS_STUCK_OPEN
    STR_RIDE_BREAKDOWN_DOORS_STUCK_CLOSED,      // BREAKDOWN_DOORS_STUCK_CLOSED
    STR_RIDE_BREAKDOWN_DOORS_STUCK_OPEN,        // BREAKDOWN_DOORS_STUCK_OPEN
    STR_RIDE_BREAKDOWN_VEHICLE_MALFUNCTION,     // BREAKDOWN_VEHICLE_MALFUNCTION
    STR_RIDE_BREAKDOWN_BRAKES_FAILURE,          // BREAKDOWN_BRAKES_FAILURE
    STR_RIDE_BREAKDOWN_CONTROL_FAILURE,         // BREAKDOWN_CONTROL_FAILURE
};
static_assert(std::size(RideBreakdownReasonNames) == BREAKDOWN_COUNT);

const StringId ColourSchemeNames[] = {
    STR_MAIN_COLOUR_SCHEME,          // RIDE_COLOUR_SCHEME_MAIN
    STR_ALTERNATIVE_COLOUR_SCHEME_1, // RIDE_COLOUR_SCHEME_ADDITIONAL_1
    STR_ALTERNATIVE_COLOUR_SCHEME_2, // RIDE_COLOUR_SCHEME_ADDITIONAL_2
    STR_ALTERNATIVE_COLOUR_SCHEME_3, // RIDE_COLOUR_SCHEME_ADDITIONAL_3
};
static_assert(std::size(ColourSchemeNames) == RIDE_COLOUR_SCHEME_COUNT);

static constexpr const StringId VehicleLoadNames[] = {
    STR_QUARTER_LOAD,       //  WAIT_FOR_LOAD_QUARTER
    STR_HALF_LOAD,          //  WAIT_FOR_LOAD_HALF
    STR_THREE_QUARTER_LOAD, //  WAIT_FOR_LOAD_THREE_QUARTER
    STR_FULL_LOAD,          //  WAIT_FOR_LOAD_FULL
    STR_ANY_LOAD,           //  WAIT_FOR_LOAD_ANY
};
static_assert(std::size(VehicleLoadNames) == WAIT_FOR_LOAD_COUNT);

static constexpr const StringId VehicleColourSchemeNames[] = {
    STR_ALL_VEHICLES_IN_SAME_COLOURS,  // RIDE_COLOUR_SCHEME_MODE_ALL_SAME,
    STR_DIFFERENT_COLOURS_PER,         // RIDE_COLOUR_SCHEME_MODE_DIFFERENT_PER_TRAIN,
    STR_DIFFERENT_COLOURS_PER_VEHICLE, // RIDE_COLOUR_SCHEME_MODE_DIFFERENT_PER_CAR,
};
static_assert(std::size(VehicleColourSchemeNames) == RIDE_COLOUR_SCHEME_MODE_COUNT);

static constexpr const StringId VehicleStatusNames[] = {
    STR_MOVING_TO_END_OF,          // Vehicle::Status::MovingToEndOfStation
    STR_WAITING_FOR_PASSENGERS_AT, // Vehicle::Status::WaitingForPassengers
    STR_WAITING_TO_DEPART,         // Vehicle::Status::WaitingToDepart
    STR_DEPARTING,                 // Vehicle::Status::Departing
    STR_TRAVELLING_AT_0,           // Vehicle::Status::Travelling
    STR_ARRIVING_AT,               // Vehicle::Status::Arriving
    STR_UNLOADING_PASSENGERS_AT,   // Vehicle::Status::UnloadingPassengers
    STR_TRAVELLING_AT_1,           // Vehicle::Status::TravellingBoat
    STR_CRASHING,                  // Vehicle::Status::Crashing
    STR_CRASHED_0,                 // Vehicle::Status::Crashed
    STR_TRAVELLING_AT_2,           // Vehicle::Status::TravellingDodgems
    STR_SWINGING,                  // Vehicle::Status::Swinging
    STR_ROTATING_0,                // Vehicle::Status::Rotating
    STR_ROTATING_1,                // Vehicle::Status::FerrisWheelRotating
    STR_OPERATING_0,               // Vehicle::Status::SimulatorOperating
    STR_SHOWING_FILM,              // Vehicle::Status::ShowingFilm
    STR_ROTATING_2,                // Vehicle::Status::SpaceRingsOperating
    STR_OPERATING_1,               // Vehicle::Status::TopSpinOperating
    STR_OPERATING_2,               // Vehicle::Status::HauntedHouseOperating
    STR_DOING_CIRCUS_SHOW,         // Vehicle::Status::DoingCircusShow
    STR_OPERATING_3,               // Vehicle::Status::CrookedHouseOperating
    STR_WAITING_FOR_CABLE_LIFT,    // Vehicle::Status::WaitingForCableLift
    STR_TRAVELLING_AT_3,           // Vehicle::Status::TravellingCableLift
    STR_STOPPING_0,                // Vehicle::Status::Stopping
    STR_WAITING_FOR_PASSENGERS,    // Vehicle::Status::WaitingForPassengers17
    STR_WAITING_TO_START,          // Vehicle::Status::WaitingToStart
    STR_STARTING,                  // Vehicle::Status::Starting
    STR_OPERATING,                 // Vehicle::Status::Operating1A
    STR_STOPPING_1,                // Vehicle::Status::Stopping1B
    STR_UNLOADING_PASSENGERS,      // Vehicle::Status::UnloadingPassengers1C
    STR_STOPPED_BY_BLOCK_BRAKES,   // Vehicle::Status::StoppedByBlockBrakes
};

static constexpr const StringId SingleSessionVehicleStatusNames[] = {
    STR_STOPPING_0,             // Vehicle::Status::MovingToEndOfStation
    STR_WAITING_FOR_PASSENGERS, // Vehicle::Status::WaitingForPassengers
    STR_WAITING_TO_START,       // Vehicle::Status::WaitingToDepart
    STR_STARTING,               // Vehicle::Status::Departing
    STR_OPERATING,              // Vehicle::Status::Travelling
    STR_STOPPING_1,             // Vehicle::Status::Arriving
    STR_UNLOADING_PASSENGERS,   // Vehicle::Status::UnloadingPassengers
};

struct WindowRideMazeDesignOption
{
    StringId text;
    uint32_t sprite;
};

static constexpr const WindowRideMazeDesignOption MazeOptions[] = {
    { STR_RIDE_DESIGN_MAZE_BRICK_WALLS, SPR_RIDE_DESIGN_PREVIEW_MAZE_BRICK_WALLS },
    { STR_RIDE_DESIGN_MAZE_HEDGES, SPR_RIDE_DESIGN_PREVIEW_MAZE_HEDGES },
    { STR_RIDE_DESIGN_MAZE_ICE_BLOCKS, SPR_RIDE_DESIGN_PREVIEW_MAZE_ICE_BLOCKS },
    { STR_RIDE_DESIGN_MAZE_WOODEN_FENCES, SPR_RIDE_DESIGN_PREVIEW_MAZE_WOODEN_FENCES },
};

struct GraphsYAxis
{
    uint8_t interval;
    int8_t unit;
    int8_t unit_interval;
    StringId label;
};

/** rct2: 0x0098DD98 */
static constexpr const GraphsYAxis window_graphs_y_axi[] = {
    { 11, 0, 10, STR_RIDE_STATS_VELOCITY_FORMAT }, // GRAPH_VELOCITY
    { 10, 0, 15, STR_RIDE_STATS_ALTITUDE_FORMAT }, // GRAPH_ALTITUDE
    { 13, -3, 1, STR_RIDE_STATS_G_FORCE_FORMAT },  // GRAPH_VERTICAL
    { 13, -4, 1, STR_RIDE_STATS_G_FORCE_FORMAT },  // GRAPH_LATERAL
};

static constexpr auto RIDE_G_FORCES_RED_NEG_VERTICAL = -FIXED_2DP(2, 50);
static constexpr auto RIDE_G_FORCES_RED_LATERAL = FIXED_2DP(2, 80);

// Used for sorting the ride type cheat dropdown.
struct RideTypeLabel
{
    ride_type_t ride_type_id;
    StringId label_id;
    const char* label_string;
};

static int32_t RideDropdownDataLanguage = LANGUAGE_UNDEFINED;
static std::vector<RideTypeLabel> RideDropdownData;

// Used for sorting the vehicle type dropdown.
struct VehicleTypeLabel
{
    ObjectEntryIndex subtype_id;
    StringId label_id;
    const char* label_string;
};

static int32_t VehicleDropdownDataLanguage = LANGUAGE_UNDEFINED;
static const RideObjectEntry* VehicleDropdownRideType = nullptr;
static bool VehicleDropdownExpanded = false;
static std::vector<VehicleTypeLabel> VehicleDropdownData;

class RideWindow final : public Window
{
    int16_t _viewIndex;

public:
    RideWindow(const Ride& ride)
    {
        rideId = ride.id;
        widgets = window_ride_page_widgets[WINDOW_RIDE_PAGE_MAIN];
        hold_down_widgets = window_ride_page_hold_down_widgets[WINDOW_RIDE_PAGE_MAIN];

        page = WINDOW_RIDE_PAGE_MAIN;
        vehicleIndex = 0;
        frame_no = 0;
        list_information_type = 0;
        picked_peep_frame = 0;
        ride_colour = 0;
        DisableTabs();
        min_width = 316;
        min_height = 180;
        max_width = 500;
        max_height = 450;

        UpdateOverallView(ride);

        PopulateVehicleTypeDropdown(ride, true);
    }

    virtual void OnClose() override
    {
        switch (page)
        {
            case WINDOW_RIDE_PAGE_COLOUR:
                ColourClose();
                break;
            case WINDOW_RIDE_PAGE_MEASUREMENTS:
                MeasurementsClose();
                break;
        }
    }
    virtual void OnResize() override
    {
        switch (page)
        {
            case WINDOW_RIDE_PAGE_MAIN:
                MainResize();
                break;
            case WINDOW_RIDE_PAGE_VEHICLE:
                VehicleResize();
                break;
            case WINDOW_RIDE_PAGE_OPERATING:
                OperatingResize();
                break;
            case WINDOW_RIDE_PAGE_MAINTENANCE:
                MaintenanceResize();
                break;
            case WINDOW_RIDE_PAGE_COLOUR:
                ColourResize();
                break;
            case WINDOW_RIDE_PAGE_MUSIC:
                MusicResize();
                break;
            case WINDOW_RIDE_PAGE_MEASUREMENTS:
                MeasurementsResize();
                break;
            case WINDOW_RIDE_PAGE_GRAPHS:
                GraphsResize();
                break;
            case WINDOW_RIDE_PAGE_INCOME:
                IncomeResize();
                break;
            case WINDOW_RIDE_PAGE_CUSTOMER:
                CustomerResize();
                break;
        }
    }
    virtual void OnUpdate() override
    {
        switch (page)
        {
            case WINDOW_RIDE_PAGE_MAIN:
                MainUpdate();
                break;
            case WINDOW_RIDE_PAGE_VEHICLE:
                VehicleUpdate();
                break;
            case WINDOW_RIDE_PAGE_OPERATING:
                OperatingUpdate();
                break;
            case WINDOW_RIDE_PAGE_MAINTENANCE:
                MaintenanceUpdate();
                break;
            case WINDOW_RIDE_PAGE_COLOUR:
                ColourUpdate();
                break;
            case WINDOW_RIDE_PAGE_MUSIC:
                MusicUpdate();
                break;
            case WINDOW_RIDE_PAGE_MEASUREMENTS:
                MeasurementsUpdate();
                break;
            case WINDOW_RIDE_PAGE_GRAPHS:
                GraphsUpdate();
                break;
            case WINDOW_RIDE_PAGE_INCOME:
                IncomeUpdate();
                break;
            case WINDOW_RIDE_PAGE_CUSTOMER:
                CustomerUpdate();
                break;
        }
    }

    virtual void OnPrepareDraw() override
    {
        switch (page)
        {
            case WINDOW_RIDE_PAGE_MAIN:
                MainOnPrepareDraw();
                break;
            case WINDOW_RIDE_PAGE_VEHICLE:
                VehicleOnPrepareDraw();
                break;
            case WINDOW_RIDE_PAGE_OPERATING:
                OperatingOnPrepareDraw();
                break;
            case WINDOW_RIDE_PAGE_MAINTENANCE:
                MaintenanceOnPrepareDraw();
                break;
            case WINDOW_RIDE_PAGE_COLOUR:
                ColourOnPrepareDraw();
                break;
            case WINDOW_RIDE_PAGE_MUSIC:
                MusicOnPrepareDraw();
                break;
            case WINDOW_RIDE_PAGE_MEASUREMENTS:
                MeasurementsOnPrepareDraw();
                break;
            case WINDOW_RIDE_PAGE_GRAPHS:
                GraphsOnPrepareDraw();
                break;
            case WINDOW_RIDE_PAGE_INCOME:
                IncomeOnPrepareDraw();
                break;
            case WINDOW_RIDE_PAGE_CUSTOMER:
                CustomerOnPrepareDraw();
                break;
        }
    }
    virtual void OnDraw(DrawPixelInfo& dpi) override
    {
        switch (page)
        {
            case WINDOW_RIDE_PAGE_MAIN:
                MainOnDraw(dpi);
                break;
            case WINDOW_RIDE_PAGE_VEHICLE:
                VehicleOnDraw(dpi);
                break;
            case WINDOW_RIDE_PAGE_OPERATING:
                OperatingOnDraw(dpi);
                break;
            case WINDOW_RIDE_PAGE_MAINTENANCE:
                MaintenanceOnDraw(dpi);
                break;
            case WINDOW_RIDE_PAGE_COLOUR:
                ColourOnDraw(dpi);
                break;
            case WINDOW_RIDE_PAGE_MUSIC:
                MusicOnDraw(dpi);
                break;
            case WINDOW_RIDE_PAGE_MEASUREMENTS:
                MeasurementsOnDraw(dpi);
                break;
            case WINDOW_RIDE_PAGE_GRAPHS:
                GraphsOnDraw(dpi);
                break;
            case WINDOW_RIDE_PAGE_INCOME:
                IncomeOnDraw(dpi);
                break;
            case WINDOW_RIDE_PAGE_CUSTOMER:
                CustomerOnDraw(dpi);
                break;
        }
    }

    virtual OpenRCT2String OnTooltip(WidgetIndex widgetIndex, StringId fallback) override
    {
        switch (page)
        {
            case WINDOW_RIDE_PAGE_VEHICLE:
                return VehicleTooltip(widgetIndex, fallback);
            case WINDOW_RIDE_PAGE_GRAPHS:
                return GraphsTooltip(widgetIndex, fallback);
        }
        return { fallback, {} };
    }
    virtual void OnMouseDown(WidgetIndex widgetIndex) override
    {
        switch (page)
        {
            case WINDOW_RIDE_PAGE_MAIN:
                MainOnMouseDown(widgetIndex);
                break;
            case WINDOW_RIDE_PAGE_VEHICLE:
                VehicleOnMouseDown(widgetIndex);
                break;
            case WINDOW_RIDE_PAGE_OPERATING:
                OperatingOnMouseDown(widgetIndex);
                break;
            case WINDOW_RIDE_PAGE_MAINTENANCE:
                MaintenanceOnMouseDown(widgetIndex);
                break;
            case WINDOW_RIDE_PAGE_COLOUR:
                ColourOnMouseDown(widgetIndex);
                break;
            case WINDOW_RIDE_PAGE_MUSIC:
                MusicOnMouseDown(widgetIndex);
                break;
            case WINDOW_RIDE_PAGE_MEASUREMENTS:
                MeasurementsOnMouseDown(widgetIndex);
                break;
            case WINDOW_RIDE_PAGE_GRAPHS:
                GraphsOnMouseDown(widgetIndex);
                break;
            case WINDOW_RIDE_PAGE_INCOME:
                IncomeOnMouseDown(widgetIndex);
                break;
        }
    }
    virtual void OnMouseUp(WidgetIndex widgetIndex) override
    {
        switch (page)
        {
            case WINDOW_RIDE_PAGE_MAIN:
                MainOnMouseUp(widgetIndex);
                break;
            case WINDOW_RIDE_PAGE_VEHICLE:
                VehicleOnMouseUp(widgetIndex);
                break;
            case WINDOW_RIDE_PAGE_OPERATING:
                OperatingOnMouseUp(widgetIndex);
                break;
            case WINDOW_RIDE_PAGE_MAINTENANCE:
                MaintenanceOnMouseUp(widgetIndex);
                break;
            case WINDOW_RIDE_PAGE_COLOUR:
                ColourOnMouseUp(widgetIndex);
                break;
            case WINDOW_RIDE_PAGE_MUSIC:
                MusicOnMouseUp(widgetIndex);
                break;
            case WINDOW_RIDE_PAGE_MEASUREMENTS:
                MeasurementsOnMouseUp(widgetIndex);
                break;
            case WINDOW_RIDE_PAGE_GRAPHS:
                GraphsOnMouseUp(widgetIndex);
                break;
            case WINDOW_RIDE_PAGE_INCOME:
                IncomeOnMouseUp(widgetIndex);
                break;
            case WINDOW_RIDE_PAGE_CUSTOMER:
                CustomerOnMouseUp(widgetIndex);
                break;
        }
    }
    virtual void OnDropdown(WidgetIndex widgetIndex, int32_t selectedIndex) override
    {
        switch (page)
        {
            case WINDOW_RIDE_PAGE_MAIN:
                MainOnDropdown(widgetIndex, selectedIndex);
                break;
            case WINDOW_RIDE_PAGE_VEHICLE:
                VehicleOnDropdown(widgetIndex, selectedIndex);
                break;
            case WINDOW_RIDE_PAGE_OPERATING:
                OperatingOnDropdown(widgetIndex, selectedIndex);
                break;
            case WINDOW_RIDE_PAGE_MAINTENANCE:
                MaintenanceOnDropdown(widgetIndex, selectedIndex);
                break;
            case WINDOW_RIDE_PAGE_COLOUR:
                ColourOnDropdown(widgetIndex, selectedIndex);
                break;
            case WINDOW_RIDE_PAGE_MUSIC:
                MusicOnDropdown(widgetIndex, selectedIndex);
                break;
            case WINDOW_RIDE_PAGE_MEASUREMENTS:
                MeasurementsOnDropdown(widgetIndex, selectedIndex);
                break;
        }
    }
    virtual void OnTextInput(WidgetIndex widgetIndex, std::string_view text) override
    {
        switch (page)
        {
            case WINDOW_RIDE_PAGE_MAIN:
                MainOnTextInput(widgetIndex, text);
                break;
            case WINDOW_RIDE_PAGE_OPERATING:
                OperatingOnTextInput(widgetIndex, text);
                break;
            case WINDOW_RIDE_PAGE_INCOME:
                IncomeOnTextInput(widgetIndex, text);
                break;
        }
    }
    virtual ScreenSize OnScrollGetSize(int32_t scrollIndex) override
    {
        switch (page)
        {
            case WINDOW_RIDE_PAGE_GRAPHS:
                return GraphsScrollGetSize(scrollIndex);
        }
        return {};
    }
    virtual void OnScrollSelect(int32_t scrollIndex, int32_t scrollAreaType) override
    {
        switch (page)
        {
            case WINDOW_RIDE_PAGE_GRAPHS:
                Graphs15(scrollIndex, scrollAreaType);
                break;
        }
    }
    virtual void OnScrollDraw(int32_t scrollIndex, DrawPixelInfo& dpi) override
    {
        switch (page)
        {
            case WINDOW_RIDE_PAGE_VEHICLE:
                VehicleOnScrollDraw(dpi, scrollIndex);
                break;
            case WINDOW_RIDE_PAGE_COLOUR:
                ColourOnScrollDraw(dpi, scrollIndex);
                break;
            case WINDOW_RIDE_PAGE_GRAPHS:
                GraphsOnScrollDraw(dpi, scrollIndex);
                break;
        }
    }
    virtual void OnToolDown(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
    {
        switch (page)
        {
            case WINDOW_RIDE_PAGE_COLOUR:
                ColourOnToolDown(widgetIndex, screenCoords);
                break;
            case WINDOW_RIDE_PAGE_MEASUREMENTS:
                MeasurementsOnToolDown(widgetIndex, screenCoords);
                break;
        }
    }
    virtual void OnToolDrag(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
    {
        switch (page)
        {
            case WINDOW_RIDE_PAGE_COLOUR:
                ColourOnToolDrag(widgetIndex, screenCoords);
                break;
            case WINDOW_RIDE_PAGE_MEASUREMENTS:
                MeasurementsOnToolDrag(widgetIndex, screenCoords);
                break;
        }
    }
    virtual void OnToolAbort(WidgetIndex widgetIndex) override
    {
        switch (page)
        {
            case WINDOW_RIDE_PAGE_MEASUREMENTS:
                MeasurementsOnToolAbort(widgetIndex);
                break;
        }
    }
    virtual void OnViewportRotate() override
    {
        switch (page)
        {
            case WINDOW_RIDE_PAGE_MAIN:
                MainViewportRotate();
                break;
        }
    }

    void SetPage(int32_t newPage)
    {
        int32_t listen;

        if (InputTestFlag(INPUT_FLAG_TOOL_ACTIVE))
            if (classification == gCurrentToolWidget.window_classification && number == gCurrentToolWidget.window_number)
                ToolCancel();

        if (newPage == WINDOW_RIDE_PAGE_VEHICLE)
        {
            auto constructionWindow = WindowFindByClass(WindowClass::RideConstruction);
            if (constructionWindow != nullptr && constructionWindow->number == number)
            {
                WindowCloseByClass(WindowClass::RideConstruction);
                // Closing the construction window sets the tab to the first page, which we don't want here,
                // as user just clicked the Vehicle page
                SetPage(WINDOW_RIDE_PAGE_VEHICLE);
            }
        }

        // Set listen only to viewport
        listen = 0;
        if (newPage == WINDOW_RIDE_PAGE_MAIN && page == WINDOW_RIDE_PAGE_MAIN && viewport != nullptr
            && !(viewport->flags & VIEWPORT_FLAG_SOUND_ON))
            listen++;

        page = newPage;
        frame_no = 0;
        picked_peep_frame = 0;

        // There doesn't seem to be any need for this call, and it can sometimes modify the reported number of cars per train,
        // so I've removed it if (newPage == WINDOW_RIDE_PAGE_VEHICLE) { ride_update_max_vehicles(ride);
        //}

        RemoveViewport();

        hold_down_widgets = window_ride_page_hold_down_widgets[page];
        pressed_widgets = 0;
        widgets = window_ride_page_widgets[page];
        DisableTabs();
        Invalidate();

        OnResize();
        OnPrepareDraw();
        InitScrollWidgets();
        Invalidate();

        if (listen != 0 && viewport != nullptr)
            viewport->flags |= VIEWPORT_FLAG_SOUND_ON;
    }

    void SetViewIndex(int16_t newIndex)
    {
        _viewIndex = newIndex;
        OnViewportRotate();
    }
    int16_t GetViewIndex() const
    {
        return _viewIndex;
    }

private:
    void DrawTabImage(DrawPixelInfo& dpi, int32_t tab, int32_t spriteIndex)
    {
        WidgetIndex widgetIndex = WIDX_TAB_1 + tab;

        if (!WidgetIsDisabled(*this, widgetIndex))
        {
            if (page == tab)
            {
                int32_t frame = frame_no / window_ride_tab_animation_divisor[page];
                spriteIndex += (frame % window_ride_tab_animation_frames[page]);
            }

            const auto& widget = widgets[widgetIndex];
            GfxDrawSprite(dpi, ImageId(spriteIndex), windowPos + ScreenCoordsXY{ widget.left, widget.top });
        }
    }

    void DrawTabMain(DrawPixelInfo& dpi)
    {
        WidgetIndex widgetIndex = WIDX_TAB_1 + static_cast<int32_t>(WINDOW_RIDE_PAGE_MAIN);
        if (!WidgetIsDisabled(*this, widgetIndex))
        {
            auto ride = GetRide(rideId);
            if (ride != nullptr)
            {
                int32_t spriteIndex = 0;
                switch (ride->GetClassification())
                {
                    case RideClassification::Ride:
                        spriteIndex = SPR_TAB_RIDE_0;
                        if (page == WINDOW_RIDE_PAGE_MAIN)
                            spriteIndex += (frame_no / 4) % 16;
                        break;
                    case RideClassification::ShopOrStall:
                        spriteIndex = SPR_TAB_SHOPS_AND_STALLS_0;
                        if (page == WINDOW_RIDE_PAGE_MAIN)
                            spriteIndex += (frame_no / 4) % 16;
                        break;
                    case RideClassification::KioskOrFacility:
                        spriteIndex = SPR_TAB_KIOSKS_AND_FACILITIES_0;
                        if (page == WINDOW_RIDE_PAGE_MAIN)
                            spriteIndex += (frame_no / 4) % 8;
                        break;
                }

                const auto& widget = widgets[widgetIndex];
                GfxDrawSprite(dpi, ImageId(spriteIndex), windowPos + ScreenCoordsXY{ widget.left, widget.top });
            }
        }
    }

    void DrawTabVehicle(DrawPixelInfo& dpi)
    {
        WidgetIndex widgetIndex = WIDX_TAB_1 + static_cast<int32_t>(WINDOW_RIDE_PAGE_VEHICLE);
        const auto& widget = widgets[widgetIndex];

        if (!WidgetIsDisabled(*this, widgetIndex))
        {
            auto screenCoords = ScreenCoordsXY{ widget.left + 1, widget.top + 1 };
            int32_t clipWidth = widget.right - screenCoords.x;
            int32_t clipHeight = widget.bottom - 3 - screenCoords.y;
            if (page == WINDOW_RIDE_PAGE_VEHICLE)
                clipHeight += 4;

            screenCoords += windowPos;

            DrawPixelInfo clipDPI;
            if (!ClipDrawPixelInfo(clipDPI, dpi, screenCoords, clipWidth, clipHeight))
            {
                return;
            }

            screenCoords = ScreenCoordsXY{ widget.width() / 2, widget.height() - 12 };

            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            auto rideEntry = ride->GetRideEntry();
            if (rideEntry == nullptr)
                return;

            if (rideEntry->flags & RIDE_ENTRY_FLAG_VEHICLE_TAB_SCALE_HALF)
            {
                clipDPI.zoom_level = ZoomLevel{ 1 };
                clipDPI.width *= 2;
                clipDPI.height *= 2;
                screenCoords.x *= 2;
                screenCoords.y *= 2;
                clipDPI.x *= 2;
                clipDPI.y *= 2;
            }

            // For any suspended rides, move image higher in the vehicle tab on the rides window
            if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_IS_SUSPENDED))
            {
                screenCoords.y /= 4;
            }

            const auto vehicle = RideEntryGetVehicleAtPosition(ride->subtype, ride->num_cars_per_train, rideEntry->TabCar);
            const auto& carEntry = rideEntry->Cars[vehicle];

            auto vehicleId = ((ride->colour_scheme_type & 3) == VEHICLE_COLOUR_SCHEME_PER_VEHICLE) ? rideEntry->TabCar : 0;
            VehicleColour vehicleColour = RideGetVehicleColour(*ride, vehicleId);

            // imageIndex represents a precision of 64
            auto imageIndex = OpenRCT2::Entity::Yaw::YawFrom4(2) * 2;
            if (page == WINDOW_RIDE_PAGE_VEHICLE)
                imageIndex += frame_no;
            imageIndex = carEntry.SpriteByYaw(imageIndex / 2, SpriteGroupType::SlopeFlat);
            imageIndex &= carEntry.TabRotationMask;
            imageIndex *= carEntry.base_num_frames;
            imageIndex += carEntry.base_image_id;
            auto imageId = ImageId(imageIndex, vehicleColour.Body, vehicleColour.Trim, vehicleColour.Tertiary);
            GfxDrawSprite(clipDPI, imageId, screenCoords);
        }
    }

    void DrawTabCustomer(DrawPixelInfo& dpi)
    {
        WidgetIndex widgetIndex = WIDX_TAB_1 + static_cast<int32_t>(WINDOW_RIDE_PAGE_CUSTOMER);

        if (!WidgetIsDisabled(*this, widgetIndex))
        {
            const auto& widget = widgets[widgetIndex];
            int32_t spriteIndex = 0;
            if (page == WINDOW_RIDE_PAGE_CUSTOMER)
                spriteIndex = picked_peep_frame & ~3;

            spriteIndex += GetPeepAnimation(PeepSpriteType::Normal).base_image + 1;

            GfxDrawSprite(
                dpi, ImageId(spriteIndex, COLOUR_BRIGHT_RED, COLOUR_TEAL),
                windowPos + ScreenCoordsXY{ widget.midX(), widget.bottom - 6 });
        }
    }

    void DrawTabImages(DrawPixelInfo& dpi)
    {
        DrawTabVehicle(dpi);
        DrawTabImage(dpi, WINDOW_RIDE_PAGE_OPERATING, SPR_TAB_GEARS_0);
        DrawTabImage(dpi, WINDOW_RIDE_PAGE_MAINTENANCE, SPR_TAB_WRENCH_0);
        DrawTabImage(dpi, WINDOW_RIDE_PAGE_INCOME, SPR_TAB_ADMISSION_0);
        DrawTabMain(dpi);
        DrawTabImage(dpi, WINDOW_RIDE_PAGE_MEASUREMENTS, SPR_TAB_TIMER_0);
        DrawTabImage(dpi, WINDOW_RIDE_PAGE_COLOUR, SPR_TAB_PAINT_0);
        DrawTabImage(dpi, WINDOW_RIDE_PAGE_GRAPHS, SPR_TAB_GRAPH_A_0);
        DrawTabCustomer(dpi);
        DrawTabImage(dpi, WINDOW_RIDE_PAGE_MUSIC, SPR_TAB_MUSIC_0);
    }

    void DisableTabs()
    {
        uint32_t disabled_tabs = 0;
        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        const auto& rtd = ride->GetRideTypeDescriptor();

        if (!rtd.HasFlag(RIDE_TYPE_FLAG_HAS_DATA_LOGGING))
            disabled_tabs |= (1uLL << WIDX_TAB_8); // 0x800

        if (ride->type == RIDE_TYPE_MINI_GOLF)
            disabled_tabs |= (1uLL << WIDX_TAB_2 | 1uLL << WIDX_TAB_3 | 1uLL << WIDX_TAB_4); // 0xE0

        if (rtd.HasFlag(RIDE_TYPE_FLAG_NO_VEHICLES))
            disabled_tabs |= (1uLL << WIDX_TAB_2); // 0x20

        if (!rtd.HasFlag(RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_MAIN) && !rtd.HasFlag(RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_ADDITIONAL)
            && !rtd.HasFlag(RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_SUPPORTS) && !rtd.HasFlag(RIDE_TYPE_FLAG_HAS_VEHICLE_COLOURS)
            && !rtd.HasFlag(RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT))
        {
            disabled_tabs |= (1uLL << WIDX_TAB_5); // 0x100
        }

        if (rtd.HasFlag(RIDE_TYPE_FLAG_IS_SHOP_OR_FACILITY))
            disabled_tabs |= (1uLL << WIDX_TAB_3 | 1uLL << WIDX_TAB_4 | 1uLL << WIDX_TAB_7); // 0x4C0

        if (!rtd.HasFlag(RIDE_TYPE_FLAG_ALLOW_MUSIC))
        {
            disabled_tabs |= (1uLL << WIDX_TAB_6); // 0x200
        }

        if (rtd.HasFlag(RIDE_TYPE_FLAG_IS_CASH_MACHINE) || rtd.HasFlag(RIDE_TYPE_FLAG_IS_FIRST_AID)
            || (gParkFlags & PARK_FLAGS_NO_MONEY) != 0)
            disabled_tabs |= (1uLL << WIDX_TAB_9); // 0x1000

        if ((gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER) != 0)
            disabled_tabs |= (1uLL << WIDX_TAB_4 | 1uLL << WIDX_TAB_6 | 1uLL << WIDX_TAB_9 | 1uLL << WIDX_TAB_10); // 0x3280

        const auto* rideEntry = GetRideEntryByIndex(ride->subtype);

        if (rideEntry == nullptr)
        {
            disabled_tabs |= 1uLL << WIDX_TAB_2 | 1uLL << WIDX_TAB_3 | 1uLL << WIDX_TAB_4 | 1uLL << WIDX_TAB_5
                | 1uLL << WIDX_TAB_6 | 1uLL << WIDX_TAB_7 | 1uLL << WIDX_TAB_8 | 1uLL << WIDX_TAB_9 | 1uLL << WIDX_TAB_10;
        }
        else if ((rideEntry->flags & RIDE_ENTRY_FLAG_DISABLE_COLOUR_TAB) != 0)
        {
            disabled_tabs |= (1uLL << WIDX_TAB_5);
        }

        disabled_widgets = disabled_tabs;
    }

    void UpdateOverallView(const Ride& ride) const
    {
        // Calculate x, y, z bounds of the entire ride using its track elements
        TileElementIterator it;

        TileElementIteratorBegin(&it);

        CoordsXYZ min = { std::numeric_limits<int32_t>::max(), std::numeric_limits<int32_t>::max(),
                          std::numeric_limits<int32_t>::max() };
        CoordsXYZ max = { std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::min(),
                          std::numeric_limits<int32_t>::min() };

        while (TileElementIteratorNext(&it))
        {
            if (it.element->GetType() != TileElementType::Track)
                continue;

            if (it.element->AsTrack()->GetRideIndex() != ride.id)
                continue;

            auto location = TileCoordsXY(it.x, it.y).ToCoordsXY();
            int32_t baseZ = it.element->GetBaseZ();
            int32_t clearZ = it.element->GetClearanceZ();

            min.x = std::min(min.x, location.x);
            min.y = std::min(min.y, location.y);
            min.z = std::min(min.z, baseZ);

            max.x = std::max(max.x, location.x);
            max.y = std::max(max.y, location.y);
            max.z = std::max(max.z, clearZ);
        }

        const auto rideIndex = ride.id.ToUnderlying();
        if (rideIndex >= ride_overall_views.size())
        {
            ride_overall_views.resize(rideIndex + 1);
        }

        auto& view = ride_overall_views[rideIndex];
        view.loc = CoordsXYZ{ (min.x + max.x) / 2, (min.y + max.y) / 2, (min.z + max.z) / 2 } + CoordsXYZ{ 16, 16, -8 };

        // Calculate size to determine from how far away to view the ride
        const auto diff = max - min;

        const int32_t size = static_cast<int32_t>(std::sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z));

        if (size >= 80)
        {
            // Each farther zoom level shows twice as many tiles (log)
            // Appropriate zoom is lowered by one to fill the entire view with the ride
            const auto zoomValue = static_cast<int8_t>(std::ceil(std::log(size / 80)) - 1);
            view.zoom = std::clamp(ZoomLevel{ zoomValue }, ZoomLevel{ 0 }, ZoomLevel::max());
        }
        else
        {
            // Small rides or stalls are zoomed in all the way.
            view.zoom = ZoomLevel{ 0 };
        }
    }

    void SetPressedTab()
    {
        int32_t i;
        for (i = 0; i < WINDOW_RIDE_PAGE_COUNT; i++)
            pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
        pressed_widgets |= 1LL << (WIDX_TAB_1 + page);
    }

    void AnchorBorderWidgets()
    {
        ResizeFrameWithPage();
    }

#pragma region Main

    std::optional<StationIndex> GetStationIndexFromViewSelection() const
    {
        const auto* ride = GetRide(RideId::FromUnderlying(number));
        if (ride == nullptr)
            return std::nullopt;

        int32_t viewSelectionIndex = this->_viewIndex - 1 - ride->NumTrains;
        if (viewSelectionIndex < 0)
        {
            return std::nullopt;
        }

        for (const auto& station : ride->GetStations())
        {
            if (!station.Start.IsNull() && viewSelectionIndex-- == 0)
            {
                const auto stationIndex = ride->GetStationIndex(&station);
                return std::make_optional(stationIndex);
            }
        }
        return std::nullopt;
    }

    void InitViewport()
    {
        if (page != WINDOW_RIDE_PAGE_MAIN)
            return;

        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        int32_t viewSelectionIndex = this->_viewIndex - 1;

        std::optional<Focus> newFocus;

        if (viewSelectionIndex >= 0 && viewSelectionIndex < ride->NumTrains && ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK)
        {
            auto vehId = ride->vehicles[viewSelectionIndex];
            const auto* rideEntry = ride->GetRideEntry();
            if (rideEntry != nullptr && rideEntry->TabCar != 0)
            {
                Vehicle* vehicle = GetEntity<Vehicle>(vehId);
                if (vehicle == nullptr)
                {
                    vehId = EntityId::GetNull();
                }
                else if (!vehicle->next_vehicle_on_train.IsNull())
                {
                    vehId = vehicle->next_vehicle_on_train;
                }
            }
            if (!vehId.IsNull())
            {
                newFocus = Focus(vehId);
            }
        }
        else if (viewSelectionIndex >= ride->NumTrains && viewSelectionIndex < (ride->NumTrains + ride->num_stations))
        {
            auto stationIndex = GetStationIndexFromViewSelection();
            if (stationIndex)
            {
                const auto location = ride->GetStation(*stationIndex).GetStart();
                newFocus = Focus(location);
            }
        }
        else
        {
            if (viewSelectionIndex > 0)
            {
                this->_viewIndex = 0;
            }
            if (number < ride_overall_views.size())
            {
                const auto& view = ride_overall_views[number];
                newFocus = Focus(view.loc, view.zoom);
            }
        }

        uint16_t viewport_flags = 0;
        if (viewport != nullptr)
        {
            if (focus == newFocus)
            {
                return;
            }
            viewport_flags = viewport->flags;
            RemoveViewport();
        }
        else if (gConfigGeneral.AlwaysShowGridlines)
        {
            viewport_flags |= VIEWPORT_FLAG_GRIDLINES;
        }

        OnPrepareDraw();

        focus = newFocus;

        // rct2: 0x006aec9c only used here so brought it into the function
        if (viewport == nullptr && !ride->overall_view.IsNull() && focus.has_value())
        {
            const auto& view_widget = widgets[WIDX_VIEWPORT];

            auto screenPos = windowPos + ScreenCoordsXY{ view_widget.left + 1, view_widget.top + 1 };
            int32_t viewWidth = view_widget.width() - 1;
            int32_t viewHeight = view_widget.height() - 1;

            ViewportCreate(this, screenPos, viewWidth, viewHeight, focus.value());

            flags |= WF_NO_SCROLLING;
            Invalidate();
        }
        if (viewport != nullptr)
        {
            viewport->flags = viewport_flags;
            Invalidate();
        }
    }

    void Rename()
    {
        auto ride = GetRide(rideId);
        if (ride != nullptr)
        {
            auto rideName = ride->GetName();
            WindowTextInputRawOpen(
                this, WIDX_RENAME, STR_RIDE_ATTRACTION_NAME, STR_ENTER_NEW_NAME_FOR_THIS_RIDE_ATTRACTION, {}, rideName.c_str(),
                32);
        }
    }

    void MainOnMouseUp(WidgetIndex widgetIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                Close();
                return;
            case WIDX_TAB_1:
            case WIDX_TAB_2:
            case WIDX_TAB_3:
            case WIDX_TAB_4:
            case WIDX_TAB_5:
            case WIDX_TAB_6:
            case WIDX_TAB_7:
            case WIDX_TAB_8:
            case WIDX_TAB_9:
            case WIDX_TAB_10:
                SetPage(widgetIndex - WIDX_TAB_1);
                break;
            case WIDX_CONSTRUCTION:
            {
                auto ride = GetRide(rideId);
                if (ride != nullptr)
                {
                    RideConstructionStart(*ride);
                    if (WindowFindByNumber(WindowClass::RideConstruction, ride->id.ToUnderlying()) != nullptr)
                    {
                        Close();
                        return;
                    }
                }
                break;
            }
            case WIDX_RENAME:
                Rename();
                break;
            case WIDX_DEMOLISH:
                ContextOpenDetailWindow(WD_DEMOLISH_RIDE, number);
                break;
            case WIDX_CLOSE_LIGHT:
            case WIDX_SIMULATE_LIGHT:
            case WIDX_TEST_LIGHT:
            case WIDX_OPEN_LIGHT:
            {
                auto ride = GetRide(rideId);
                if (ride != nullptr)
                {
                    RideStatus status;
                    switch (widgetIndex)
                    {
                        default:
                        case WIDX_CLOSE_LIGHT:
                            status = RideStatus::Closed;
                            break;
                        case WIDX_SIMULATE_LIGHT:
                            status = RideStatus::Simulating;
                            break;
                        case WIDX_TEST_LIGHT:
                            status = RideStatus::Testing;
                            break;
                        case WIDX_OPEN_LIGHT:
                            status = RideStatus::Open;
                            break;
                    }
                    auto gameAction = RideSetStatusAction(ride->id, status);
                    GameActions::Execute(&gameAction);
                }
                break;
            }
        }
    }

    void MainResize()
    {
        int32_t minHeight = 180;
        if (ThemeGetFlags() & UITHEME_FLAG_USE_LIGHTS_RIDE)
        {
            minHeight += 20 + RCT1_LIGHT_OFFSET;

            auto ride = GetRide(rideId);
            if (ride != nullptr)
            {
#ifdef __SIMULATE_IN_RIDE_WINDOW__
                if (ride->SupportsStatus(RideStatus::Simulating))
                {
                    minHeight += 14;
                }
#endif
                if (ride->SupportsStatus(RideStatus::Testing))
                {
                    minHeight += 14;
                }
            }
        }
        if (gCheatsAllowArbitraryRideTypeChanges)
        {
            minHeight += 15;
        }

        flags |= WF_RESIZABLE;
        WindowSetResize(*this, 316, minHeight, 500, 450);
        // Unlike with other windows, the focus needs to be recentred so it’s best to just reset it.
        focus = std::nullopt;
        InitViewport();
    }

    size_t GetNumPeepsInTrain(const Ride& ride, int32_t trainIndex) const
    {
        auto numPeepsInTrain = 0;
        const auto* vehicle = TryGetVehicle(ride.vehicles[trainIndex]);
        while (vehicle != nullptr)
        {
            numPeepsInTrain += vehicle->num_peeps;
            vehicle = TryGetVehicle(vehicle->next_vehicle_on_train);
        }
        return numPeepsInTrain;
    }

    bool TrainMustBeHidden(const Ride& ride, int32_t trainIndex) const
    {
        if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK))
            return true;

        const auto* rideEntry = ride.GetRideEntry();
        if (rideEntry == nullptr)
            return false;

        if (!(rideEntry->flags & RIDE_ENTRY_FLAG_HIDE_EMPTY_TRAINS))
            return false;

        return GetNumPeepsInTrain(ride, trainIndex) == 0;
    }

    void ShowViewDropdown(Widget* widget)
    {
        Widget* dropdownWidget = widget - 1;
        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        const auto& rtd = ride->GetRideTypeDescriptor();

        int32_t numItems = 1;
        if (!rtd.HasFlag(RIDE_TYPE_FLAG_NO_VEHICLES))
        {
            numItems += ride->num_stations;
            numItems += ride->NumTrains;
        }

        WindowDropdownShowTextCustomWidth(
            { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1, colours[1],
            0, 0, numItems, widget->right - dropdownWidget->left);

        // First item
        gDropdownItems[0].Format = STR_DROPDOWN_MENU_LABEL;
        gDropdownItems[0].Args = STR_OVERALL_VIEW;
        int32_t currentItem = 1;

        // Vehicles
        int32_t name = GetRideComponentName(rtd.NameConvention.vehicle).number;
        for (int32_t i = 0; i < ride->NumTrains; i++)
        {
            gDropdownItems[currentItem].Format = STR_DROPDOWN_MENU_LABEL;
            gDropdownItems[currentItem].Args = name | (currentItem << 16);
            if (TrainMustBeHidden(*ride, i))
            {
                Dropdown::SetDisabled(currentItem, true);
            }
            currentItem++;
        }

        // Stations
        name = GetRideComponentName(rtd.NameConvention.station).number;
        for (int32_t i = 1; i <= ride->num_stations; i++)
        {
            gDropdownItems[currentItem].Format = STR_DROPDOWN_MENU_LABEL;
            gDropdownItems[currentItem].Args = name | (i << 16);
            currentItem++;
        }

        // Set checked item
        Dropdown::SetChecked(this->_viewIndex, true);
    }

    RideStatus GetNextDefaultStatus(const Ride& ride) const
    {
        switch (ride.status)
        {
            default:
            case RideStatus::Closed:
                if ((ride.lifecycle_flags & RIDE_LIFECYCLE_CRASHED)
                    || (ride.lifecycle_flags & RIDE_LIFECYCLE_HAS_STALLED_VEHICLE))
                {
                    return RideStatus::Closed;
                }
                if (ride.SupportsStatus(RideStatus::Testing) && !(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
                {
                    return RideStatus::Testing;
                }
                return RideStatus::Open;
            case RideStatus::Simulating:
                return RideStatus::Testing;
            case RideStatus::Testing:
                return (ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED) ? RideStatus::Open : RideStatus::Closed;
            case RideStatus::Open:
                return RideStatus::Closed;
        }
    }

    struct RideStatusDropdownInfo
    {
        struct Ride* Ride{};
        RideStatus CurrentStatus{};
        RideStatus DefaultStatus{};

        int32_t NumItems{};
        int32_t CheckedIndex = -1;
        int32_t DefaultIndex = -1;
    };

    void SetDropdown(RideStatusDropdownInfo& info, RideStatus status, StringId text) const
    {
        if (info.Ride->SupportsStatus(status))
        {
            auto index = info.NumItems;
            gDropdownItems[index].Format = STR_DROPDOWN_MENU_LABEL;
            gDropdownItems[index].Args = text;
            if (info.CurrentStatus == status)
            {
                info.CheckedIndex = index;
            }
            if (info.DefaultStatus == status)
            {
                info.DefaultIndex = index;
            }
            info.NumItems++;
        }
    }

    void ShowOpenDropdown(Widget* widget)
    {
        RideStatusDropdownInfo info;
        info.Ride = GetRide(rideId);
        if (info.Ride == nullptr)
            return;

        info.CurrentStatus = info.Ride->status;
        info.DefaultStatus = GetNextDefaultStatus(*info.Ride);
        SetDropdown(info, RideStatus::Closed, STR_CLOSE_RIDE);
#ifdef __SIMULATE_IN_RIDE_WINDOW__
        SetDropdown(info, RideStatus::Simulating, STR_SIMULATE_RIDE);
#endif
        SetDropdown(info, RideStatus::Testing, STR_TEST_RIDE);
        SetDropdown(info, RideStatus::Open, STR_OPEN_RIDE);
        WindowDropdownShowText(
            { windowPos.x + widget->left, windowPos.y + widget->top }, widget->height() + 1, colours[1], 0, info.NumItems);
        Dropdown::SetChecked(info.CheckedIndex, true);
        gDropdownDefaultIndex = info.DefaultIndex;
    }

    static constexpr StringId GetRideTypeNameForDropdown(ride_type_t rideType)
    {
        switch (rideType)
        {
            case RIDE_TYPE_1D:
                return STR_RIDE_NAME_1D;
            case RIDE_TYPE_1F:
                return STR_RIDE_NAME_1F;
            case RIDE_TYPE_22:
                return STR_RIDE_NAME_22;
            case RIDE_TYPE_50:
                return STR_RIDE_NAME_50;
            case RIDE_TYPE_52:
                return STR_RIDE_NAME_52;
            case RIDE_TYPE_53:
                return STR_RIDE_NAME_53;
            case RIDE_TYPE_54:
                return STR_RIDE_NAME_54;
            case RIDE_TYPE_55:
                return STR_RIDE_NAME_55;
            case RIDE_TYPE_59:
                return STR_RIDE_NAME_59;
            default:
                return GetRideTypeDescriptor(rideType).Naming.Name;
        }
    }

    void PopulateRideTypeDropdown() const
    {
        auto& ls = OpenRCT2::GetContext()->GetLocalisationService();
        if (RideDropdownDataLanguage == ls.GetCurrentLanguage())
            return;

        RideDropdownData.clear();

        for (uint8_t i = 0; i < RIDE_TYPE_COUNT; i++)
        {
            auto name = GetRideTypeNameForDropdown(i);
            RideDropdownData.push_back({ i, name, ls.GetString(name) });
        }

        std::sort(RideDropdownData.begin(), RideDropdownData.end(), [](auto& a, auto& b) {
            return String::Compare(a.label_string, b.label_string, true) < 0;
        });

        RideDropdownDataLanguage = ls.GetCurrentLanguage();
    }

    void ShowRideTypeDropdown(Widget* widget)
    {
        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        PopulateRideTypeDropdown();

        for (size_t i = 0; i < RideDropdownData.size(); i++)
        {
            gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
            gDropdownItems[i].Args = RideDropdownData[i].label_id;
        }

        Widget* dropdownWidget = widget - 1;
        WindowDropdownShowText(
            { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1, colours[1],
            Dropdown::Flag::StayOpen, RIDE_TYPE_COUNT);

        // Find the current ride type in the ordered list.
        int32_t pos = 0;
        for (int32_t i = 0; i < RIDE_TYPE_COUNT; i++)
        {
            if (RideDropdownData[i].ride_type_id == ride->type)
            {
                pos = i;
                break;
            }
        }

        gDropdownHighlightedIndex = pos;
        gDropdownDefaultIndex = pos;
        Dropdown::SetChecked(pos, true);
    }

    void ShowLocateDropdown(Widget* widget)
    {
        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        gDropdownItems[0].Format = STR_LOCATE_SUBJECT_TIP;
        gDropdownItems[1].Format = STR_FOLLOW_SUBJECT_TIP;

        WindowDropdownShowText(
            { windowPos.x + widget->left, windowPos.y + widget->top }, widget->height() + 1, colours[1], 0, 2);
        gDropdownDefaultIndex = 0;
        if (!ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_TRACK) || this->_viewIndex == 0
            || this->_viewIndex > ride->NumTrains)
        {
            // Disable if we're a flat ride, 'overall view' is selected or a station is selected
            Dropdown::SetDisabled(1, true);
        }
    }

    void MainFollowRide()
    {
        auto* ride = GetRide(rideId);
        if (ride != nullptr)
        {
            if (!(ride->window_invalidate_flags & RIDE_INVALIDATE_RIDE_MAIN))
            {
                if (this->_viewIndex > 0)
                {
                    if (this->_viewIndex <= ride->NumTrains)
                    {
                        Vehicle* vehicle = GetEntity<Vehicle>(ride->vehicles[this->_viewIndex - 1]);
                        if (vehicle != nullptr)
                        {
                            auto headVehicleSpriteIndex = vehicle->Id;
                            WindowBase* w_main = WindowGetMain();
                            WindowFollowSprite(*w_main, headVehicleSpriteIndex);
                        }
                    }
                }
            }
        }
    }

    void PopulateVehicleTypeDropdown(const Ride& ride, bool forceRefresh = false) const
    {
        auto& objManager = GetContext()->GetObjectManager();
        const auto* rideEntry = ride.GetRideEntry();

        bool selectionShouldBeExpanded;
        int32_t rideTypeIterator, rideTypeIteratorMax;

        const auto& rtd = ride.GetRideTypeDescriptor();
        if (gCheatsShowVehiclesFromOtherTrackTypes
            && !(
                rtd.HasFlag(RIDE_TYPE_FLAG_FLAT_RIDE) || rtd.HasFlag(RIDE_TYPE_FLAG_IS_MAZE)
                || ride.type == RIDE_TYPE_MINI_GOLF))
        {
            selectionShouldBeExpanded = true;
            rideTypeIterator = 0;
            rideTypeIteratorMax = RIDE_TYPE_COUNT - 1;
        }
        else
        {
            selectionShouldBeExpanded = false;
            rideTypeIterator = ride.type;
            rideTypeIteratorMax = ride.type;
        }

        // Don't repopulate the list if we just did.
        auto& ls = OpenRCT2::GetContext()->GetLocalisationService();
        if (!forceRefresh && VehicleDropdownExpanded == selectionShouldBeExpanded && VehicleDropdownRideType == rideEntry
            && VehicleDropdownDataLanguage == ls.GetCurrentLanguage())
            return;

        VehicleDropdownData.clear();

        for (; rideTypeIterator <= rideTypeIteratorMax; rideTypeIterator++)
        {
            const auto& rtdIterator = GetRideTypeDescriptor(rideTypeIterator);
            if (selectionShouldBeExpanded && rtdIterator.HasFlag(RIDE_TYPE_FLAG_FLAT_RIDE))
                continue;
            if (selectionShouldBeExpanded
                && (rtdIterator.HasFlag(RIDE_TYPE_FLAG_IS_MAZE) || rideTypeIterator == RIDE_TYPE_MINI_GOLF))
                continue;

            auto& rideEntries = objManager.GetAllRideEntries(rideTypeIterator);
            for (auto rideEntryIndex : rideEntries)
            {
                const auto* currentRideEntry = GetRideEntryByIndex(rideEntryIndex);
                if (currentRideEntry == nullptr)
                    continue;

                // Skip if vehicle type has not been invented yet
                if (!RideEntryIsInvented(rideEntryIndex) && !gCheatsIgnoreResearchStatus)
                    continue;

                VehicleDropdownData.push_back(
                    { rideEntryIndex, currentRideEntry->naming.Name, ls.GetString(currentRideEntry->naming.Name) });
            }
        }

        std::sort(VehicleDropdownData.begin(), VehicleDropdownData.end(), [](auto& a, auto& b) {
            return String::Compare(a.label_string, b.label_string, true) < 0;
        });

        VehicleDropdownExpanded = selectionShouldBeExpanded;
        VehicleDropdownRideType = rideEntry;
        VehicleDropdownDataLanguage = ls.GetCurrentLanguage();
    }

    void ShowVehicleTypeDropdown(Widget* widget)
    {
        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        PopulateVehicleTypeDropdown(*ride);

        size_t numItems = std::min<size_t>(VehicleDropdownData.size(), Dropdown::ItemsMaxSize);

        for (size_t i = 0; i < numItems; i++)
        {
            gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
            gDropdownItems[i].Args = VehicleDropdownData[i].label_id;
        }

        Widget* dropdownWidget = widget - 1;
        auto ddWidth = WindowDropDownHasMultipleColumns(numItems) ? dropdownWidget->width() - 24 : dropdownWidget->width();
        WindowDropdownShowTextCustomWidth(
            { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1, colours[1],
            0, Dropdown::Flag::StayOpen, numItems, ddWidth);

        // Find the current vehicle type in the ordered list.
        int32_t pos = 0;
        for (int32_t i = 0; i < static_cast<int32_t>(VehicleDropdownData.size()); i++)
        {
            if (VehicleDropdownData[i].subtype_id == ride->subtype)
            {
                pos = i;
                break;
            }
        }

        gDropdownHighlightedIndex = pos;
        gDropdownDefaultIndex = pos;
        Dropdown::SetChecked(pos, true);
    }

    void MainOnMouseDown(WidgetIndex widgetIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_VIEW_DROPDOWN:
                ShowViewDropdown(&widgets[widgetIndex]);
                break;
            case WIDX_OPEN:
                ShowOpenDropdown(&widgets[widgetIndex]);
                break;
            case WIDX_RIDE_TYPE_DROPDOWN:
                ShowRideTypeDropdown(&widgets[widgetIndex]);
                break;
            case WIDX_LOCATE:
                ShowLocateDropdown(&widgets[widgetIndex]);
                break;
        }
    }

    void MainOnDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_VIEW_DROPDOWN:
                if (dropdownIndex == -1)
                {
                    dropdownIndex = _viewIndex + 1;
                    auto ride = GetRide(rideId);
                    if (ride != nullptr)
                    {
                        if (dropdownIndex != 0 && dropdownIndex <= ride->NumTrains
                            && !(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK))
                        {
                            dropdownIndex = ride->NumTrains + 1;
                        }
                        if (dropdownIndex >= gDropdownNumItems)
                        {
                            dropdownIndex = 0;
                        }
                    }
                }

                _viewIndex = dropdownIndex;
                InitViewport();
                Invalidate();
                break;
            case WIDX_OPEN:
            {
                auto ride = GetRide(rideId);
                if (ride != nullptr)
                {
                    auto status = RideStatus::Closed;
                    if (dropdownIndex < 0)
                    {
                        dropdownIndex = gDropdownHighlightedIndex;
                    }
                    if (dropdownIndex < static_cast<int32_t>(std::size(gDropdownItems)))
                    {
                        switch (gDropdownItems[dropdownIndex].Args)
                        {
                            case STR_CLOSE_RIDE:
                                status = RideStatus::Closed;
                                break;
                            case STR_SIMULATE_RIDE:
                                status = RideStatus::Simulating;
                                break;
                            case STR_TEST_RIDE:
                                status = RideStatus::Testing;
                                break;
                            case STR_OPEN_RIDE:
                                status = RideStatus::Open;
                                break;
                        }
                    }
                    auto gameAction = RideSetStatusAction(ride->id, status);
                    GameActions::Execute(&gameAction);
                }
                break;
            }
            case WIDX_RIDE_TYPE_DROPDOWN:
                if (dropdownIndex != -1 && dropdownIndex < RIDE_TYPE_COUNT)
                {
                    auto rideLabelId = std::clamp(dropdownIndex, 0, RIDE_TYPE_COUNT - 1);
                    auto rideType = RideDropdownData[rideLabelId].ride_type_id;
                    if (rideType < RIDE_TYPE_COUNT)
                    {
                        auto rideSetSetting = RideSetSettingAction(rideId, RideSetSetting::RideType, rideType);
                        rideSetSetting.SetCallback([](const GameAction* ga, const GameActions::Result* result) {
                            // Reset ghost track if ride construction window is open, prevents a crash
                            // Will get set to the correct Alternative variable during set_default_next_piece.
                            // TODO: Rework construction window to prevent the need for this.
                            _currentTrackAlternative = RIDE_TYPE_NO_ALTERNATIVES;
                            RideConstructionSetDefaultNextPiece();
                        });
                        GameActions::Execute(&rideSetSetting);
                    }
                }
                break;
            case WIDX_LOCATE:
            {
                if (dropdownIndex == 0)
                {
                    ScrollToViewport();
                }
                else if (dropdownIndex == 1)
                {
                    MainFollowRide();
                }
                break;
            }
        }
    }

    void MainUpdate()
    {
        // Update tab animation
        frame_no++;
        OnPrepareDraw();
        WidgetInvalidate(*this, WIDX_TAB_1);

        // Update status
        auto ride = GetRide(rideId);
        if (ride != nullptr)
        {
            if (!(ride->window_invalidate_flags & RIDE_INVALIDATE_RIDE_MAIN))
            {
                if (this->_viewIndex == 0)
                    return;

                if (this->_viewIndex <= ride->NumTrains)
                {
                    Vehicle* vehicle = GetEntity<Vehicle>(ride->vehicles[this->_viewIndex - 1]);
                    if (vehicle == nullptr
                        || (vehicle->status != Vehicle::Status::Travelling
                            && vehicle->status != Vehicle::Status::TravellingCableLift
                            && vehicle->status != Vehicle::Status::TravellingDodgems
                            && vehicle->status != Vehicle::Status::TravellingBoat))
                    {
                        return;
                    }
                }
            }
            ride->window_invalidate_flags &= ~RIDE_INVALIDATE_RIDE_MAIN;
        }
        WidgetInvalidate(*this, WIDX_STATUS);
    }

    void MainOnTextInput(WidgetIndex widgetIndex, std::string_view text)
    {
        if (widgetIndex != WIDX_RENAME || text.empty())
            return;

        auto ride = GetRide(rideId);
        if (ride != nullptr)
        {
            auto strText = std::string(text);
            auto gameAction = RideSetNameAction(ride->id, strText);
            GameActions::Execute(&gameAction);
        }
    }

    void MainViewportRotate()
    {
        InitViewport();
    }

    void MainOnPrepareDraw()
    {
        Widget* newWidgets;
        int32_t i, widgetHeight;

        newWidgets = window_ride_page_widgets[page];
        if (widgets != newWidgets)
        {
            widgets = newWidgets;
            InitScrollWidgets();
        }

        SetPressedTab();

        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        disabled_widgets &= ~((1uLL << WIDX_DEMOLISH) | (1uLL << WIDX_CONSTRUCTION));
        if (ride->lifecycle_flags & (RIDE_LIFECYCLE_INDESTRUCTIBLE | RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK)
            && !gCheatsMakeAllDestructible)
            disabled_widgets |= (1uLL << WIDX_DEMOLISH);

        auto ft = Formatter::Common();
        ride->FormatNameTo(ft);

        uint32_t spriteIds[] = {
            SPR_CLOSED,
            SPR_OPEN,
            SPR_TESTING,
            SPR_G2_SIMULATE,
        };
        window_ride_main_widgets[WIDX_OPEN].image = ImageId(spriteIds[EnumValue(ride->status)]);

#ifdef __SIMULATE_IN_RIDE_WINDOW__
        window_ride_main_widgets[WIDX_CLOSE_LIGHT].image = SPR_G2_RCT1_CLOSE_BUTTON_0 + (ride->status == RideStatus::Closed) * 2
            + WidgetIsPressed(*this, WIDX_CLOSE_LIGHT);
        window_ride_main_widgets[WIDX_SIMULATE_LIGHT].image = SPR_G2_RCT1_SIMULATE_BUTTON_0
            + (ride->status == RideStatus::Simulating) * 2 + WidgetIsPressed(*w, WIDX_SIMULATE_LIGHT);
        window_ride_main_widgets[WIDX_TEST_LIGHT].image = SPR_G2_RCT1_TEST_BUTTON_0 + (ride->status == RideStatus::Testing) * 2
            + WidgetIsPressed(*this, WIDX_TEST_LIGHT);
#else
        const auto closeLightImage = SPR_G2_RCT1_CLOSE_BUTTON_0 + (ride->status == RideStatus::Closed) * 2
            + WidgetIsPressed(*this, WIDX_CLOSE_LIGHT);
        window_ride_main_widgets[WIDX_CLOSE_LIGHT].image = ImageId(closeLightImage);

        auto baseSprite = ride->status == RideStatus::Simulating ? SPR_G2_RCT1_SIMULATE_BUTTON_0 : SPR_G2_RCT1_TEST_BUTTON_0;
        const auto testLightImage = baseSprite
            + (ride->status == RideStatus::Testing || ride->status == RideStatus::Simulating) * 2
            + WidgetIsPressed(*this, WIDX_TEST_LIGHT);
        window_ride_main_widgets[WIDX_TEST_LIGHT].image = ImageId(testLightImage);
#endif
        const auto openLightImage = SPR_G2_RCT1_OPEN_BUTTON_0 + (ride->status == RideStatus::Open) * 2
            + WidgetIsPressed(*this, WIDX_OPEN_LIGHT);
        window_ride_main_widgets[WIDX_OPEN_LIGHT].image = ImageId(openLightImage);

        AnchorBorderWidgets();

        const int32_t offset = gCheatsAllowArbitraryRideTypeChanges ? 15 : 0;
        // Anchor main page specific widgets
        window_ride_main_widgets[WIDX_VIEWPORT].right = width - 26;
        window_ride_main_widgets[WIDX_VIEWPORT].bottom = height - (14 + offset);
        window_ride_main_widgets[WIDX_STATUS].right = width - 26;
        window_ride_main_widgets[WIDX_STATUS].top = height - (13 + offset);
        window_ride_main_widgets[WIDX_STATUS].bottom = height - (3 + offset);
        window_ride_main_widgets[WIDX_VIEW].right = width - 60;
        window_ride_main_widgets[WIDX_VIEW_DROPDOWN].right = width - 61;
        window_ride_main_widgets[WIDX_VIEW_DROPDOWN].left = width - 71;
        window_ride_main_widgets[WIDX_RIDE_TYPE].right = width - 26;
        window_ride_main_widgets[WIDX_RIDE_TYPE].top = height - 17;
        window_ride_main_widgets[WIDX_RIDE_TYPE].bottom = height - 4;
        window_ride_main_widgets[WIDX_RIDE_TYPE_DROPDOWN].left = width - 37;
        window_ride_main_widgets[WIDX_RIDE_TYPE_DROPDOWN].right = width - 27;
        window_ride_main_widgets[WIDX_RIDE_TYPE_DROPDOWN].top = height - 16;
        window_ride_main_widgets[WIDX_RIDE_TYPE_DROPDOWN].bottom = height - 5;

        if (!gCheatsAllowArbitraryRideTypeChanges)
        {
            window_ride_main_widgets[WIDX_RIDE_TYPE].type = WindowWidgetType::Empty;
            window_ride_main_widgets[WIDX_RIDE_TYPE_DROPDOWN].type = WindowWidgetType::Empty;
        }
        else
        {
            window_ride_main_widgets[WIDX_RIDE_TYPE].type = WindowWidgetType::DropdownMenu;
            window_ride_main_widgets[WIDX_RIDE_TYPE].text = ride->GetRideTypeDescriptor().Naming.Name;
            window_ride_main_widgets[WIDX_RIDE_TYPE_DROPDOWN].type = WindowWidgetType::Button;
        }

        WindowAlignTabs(this, WIDX_TAB_1, WIDX_TAB_10);

        if (ThemeGetFlags() & UITHEME_FLAG_USE_LIGHTS_RIDE)
        {
            window_ride_main_widgets[WIDX_OPEN].type = WindowWidgetType::Empty;
            window_ride_main_widgets[WIDX_CLOSE_LIGHT].type = WindowWidgetType::ImgBtn;
            window_ride_main_widgets[WIDX_SIMULATE_LIGHT].type = WindowWidgetType::Empty;
#ifdef __SIMULATE_IN_RIDE_WINDOW__
            if (ride->SupportsStatus(RideStatus::Simulating))
                window_ride_main_widgets[WIDX_SIMULATE_LIGHT].type = WindowWidgetType::ImgBtn;
#endif
            window_ride_main_widgets[WIDX_TEST_LIGHT].type = ride->SupportsStatus(RideStatus::Testing)
                ? WindowWidgetType::ImgBtn
                : WindowWidgetType::Empty;
            window_ride_main_widgets[WIDX_OPEN_LIGHT].type = WindowWidgetType::ImgBtn;

            widgetHeight = 62;
            if (window_ride_main_widgets[WIDX_SIMULATE_LIGHT].type != WindowWidgetType::Empty)
            {
                window_ride_main_widgets[WIDX_SIMULATE_LIGHT].top = widgetHeight;
                window_ride_main_widgets[WIDX_SIMULATE_LIGHT].bottom = widgetHeight + 13;
                widgetHeight += 14;
            }
            if (window_ride_main_widgets[WIDX_TEST_LIGHT].type != WindowWidgetType::Empty)
            {
                window_ride_main_widgets[WIDX_TEST_LIGHT].top = widgetHeight;
                window_ride_main_widgets[WIDX_TEST_LIGHT].bottom = widgetHeight + 13;
                widgetHeight += 14;
            }
            window_ride_main_widgets[WIDX_OPEN_LIGHT].top = widgetHeight;
            window_ride_main_widgets[WIDX_OPEN_LIGHT].bottom = widgetHeight + 13;
            widgetHeight += 14 - 24 + RCT1_LIGHT_OFFSET;
        }
        else
        {
            window_ride_main_widgets[WIDX_OPEN].type = WindowWidgetType::FlatBtn;
            window_ride_main_widgets[WIDX_CLOSE_LIGHT].type = WindowWidgetType::Empty;
            window_ride_main_widgets[WIDX_SIMULATE_LIGHT].type = WindowWidgetType::Empty;
            window_ride_main_widgets[WIDX_TEST_LIGHT].type = WindowWidgetType::Empty;
            window_ride_main_widgets[WIDX_OPEN_LIGHT].type = WindowWidgetType::Empty;
            widgetHeight = 46;
        }
        for (i = WIDX_CLOSE_LIGHT; i <= WIDX_OPEN_LIGHT; i++)
        {
            window_ride_main_widgets[i].left = width - 20;
            window_ride_main_widgets[i].right = width - 7;
        }
        for (i = WIDX_OPEN; i <= WIDX_DEMOLISH; i++, widgetHeight += 24)
        {
            window_ride_main_widgets[i].left = width - 25;
            window_ride_main_widgets[i].right = width - 2;
            window_ride_main_widgets[i].top = widgetHeight;
            window_ride_main_widgets[i].bottom = widgetHeight + 23;
        }
    }

    StringId GetStatusOverallView(Formatter& ft) const
    {
        auto stringId = STR_NONE;
        auto ride = GetRide(rideId);
        if (ride != nullptr)
        {
            ride->FormatStatusTo(ft);
            stringId = STR_BLACK_STRING;
            if ((ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN) || (ride->lifecycle_flags & RIDE_LIFECYCLE_CRASHED))
            {
                stringId = STR_RED_OUTLINED_STRING;
            }
        }
        return stringId;
    }

    StringId GetStatusVehicle(Formatter& ft) const
    {
        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return STR_EMPTY;

        auto vehicle = GetEntity<Vehicle>(ride->vehicles[this->_viewIndex - 1]);
        if (vehicle == nullptr)
            return STR_EMPTY;

        if (vehicle->status != Vehicle::Status::Crashing && vehicle->status != Vehicle::Status::Crashed)
        {
            auto trackType = vehicle->GetTrackType();
            if (trackType == TrackElemType::BlockBrakes || trackType == TrackElemType::CableLiftHill
                || trackType == TrackElemType::Up25ToFlat || trackType == TrackElemType::Up60ToFlat
                || trackType == TrackElemType::DiagUp25ToFlat || trackType == TrackElemType::DiagUp60ToFlat)
            {
                if (ride->GetRideTypeDescriptor().SupportsTrackPiece(TRACK_BLOCK_BRAKES) && vehicle->velocity == 0)
                {
                    ft.Add<StringId>(STR_STOPPED_BY_BLOCK_BRAKES);
                    return STR_BLACK_STRING;
                }
            }
        }

        if (ride->type == RIDE_TYPE_MINI_GOLF)
            return STR_EMPTY;

        auto stringId = VehicleStatusNames[static_cast<size_t>(vehicle->status)];
        if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_SINGLE_SESSION)
            && vehicle->status <= Vehicle::Status::UnloadingPassengers)
        {
            stringId = SingleSessionVehicleStatusNames[static_cast<size_t>(vehicle->status)];
        }

        ft.Add<StringId>(stringId);
        uint16_t speedInMph = (abs(vehicle->velocity) * 9) >> 18;
        ft.Add<uint16_t>(speedInMph);
        const RideComponentName stationName = GetRideComponentName(ride->GetRideTypeDescriptor().NameConvention.station);
        ft.Add<StringId>(ride->num_stations > 1 ? stationName.number : stationName.singular);
        ft.Add<uint16_t>(vehicle->current_station.ToUnderlying() + 1);
        return stringId != STR_CRASHING && stringId != STR_CRASHED_0 ? STR_BLACK_STRING : STR_RED_OUTLINED_STRING;
    }

    StringId GetStatusStation(Formatter& ft) const
    {
        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return STR_NONE;

        const auto stationIndex = GetStationIndexFromViewSelection();
        if (!stationIndex)
        {
            return STR_NONE;
        }

        const auto& station = ride->GetStation(*stationIndex);
        StringId stringId = STR_EMPTY;
        // Entrance / exit
        if (ride->status == RideStatus::Closed)
        {
            if (station.Entrance.IsNull())
                stringId = STR_NO_ENTRANCE;
            else if (station.Exit.IsNull())
                stringId = STR_NO_EXIT;
        }
        else
        {
            if (station.Entrance.IsNull())
                stringId = STR_EXIT_ONLY;
        }
        // Queue length
        if (stringId == STR_EMPTY)
        {
            stringId = STR_QUEUE_EMPTY;
            uint16_t queueLength = ride->GetStation(*stationIndex).QueueLength;
            if (queueLength == 1)
                stringId = STR_QUEUE_ONE_PERSON;
            else if (queueLength > 1)
                stringId = STR_QUEUE_PEOPLE;

            ft.Add<StringId>(stringId);
            ft.Add<uint16_t>(queueLength);
        }
        else
        {
            ft.Add<StringId>(stringId);
        }

        return STR_BLACK_STRING;
    }

    StringId GetStatus(Formatter& ft) const
    {
        auto ride = GetRide(rideId);
        if (this->_viewIndex == 0)
            return GetStatusOverallView(ft);
        if (ride != nullptr && this->_viewIndex <= ride->NumTrains)
            return GetStatusVehicle(ft);
        if (ride != nullptr && ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
            return GetStatusOverallView(ft);
        return GetStatusStation(ft);
    }

    void MainOnDraw(DrawPixelInfo& dpi)
    {
        Widget* widget;

        WindowDrawWidgets(*this, dpi);
        DrawTabImages(dpi);

        // Viewport and ear icon
        if (viewport != nullptr)
        {
            WindowDrawViewport(dpi, *this);
            if (viewport->flags & VIEWPORT_FLAG_SOUND_ON)
                GfxDrawSprite(dpi, ImageId(SPR_HEARING_VIEWPORT), WindowGetViewportSoundIconPos(*this));
        }

        // View dropdown
        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        auto ft = Formatter();
        if (this->_viewIndex != 0)
        {
            if (this->_viewIndex > ride->NumTrains)
            {
                ft.Add<StringId>(GetRideComponentName(ride->GetRideTypeDescriptor().NameConvention.station).number);
                ft.Add<uint16_t>(this->_viewIndex - ride->NumTrains);
            }
            else
            {
                ft.Add<StringId>(GetRideComponentName(ride->GetRideTypeDescriptor().NameConvention.vehicle).number);
                ft.Add<uint16_t>(this->_viewIndex);
            }
        }
        else
        {
            ft.Add<StringId>(STR_OVERALL_VIEW);
        }

        widget = &window_ride_main_widgets[WIDX_VIEW];
        DrawTextBasic(
            dpi, { windowPos.x + (widget->left + widget->right - 11) / 2, windowPos.y + widget->top },
            STR_WINDOW_COLOUR_2_STRINGID, ft, { TextAlignment::CENTRE });

        // Status
        ft = Formatter();
        widget = &window_ride_main_widgets[WIDX_STATUS];
        StringId rideStatus = GetStatus(ft);
        DrawTextEllipsised(
            dpi, windowPos + ScreenCoordsXY{ (widget->left + widget->right) / 2, widget->top }, widget->width(), rideStatus, ft,
            { TextAlignment::CENTRE });
    }

#pragma endregion

#pragma region Vehicle

    void VehicleOnMouseUp(WidgetIndex widgetIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                Close();
                return;
            case WIDX_TAB_1:
            case WIDX_TAB_2:
            case WIDX_TAB_3:
            case WIDX_TAB_4:
            case WIDX_TAB_5:
            case WIDX_TAB_6:
            case WIDX_TAB_7:
            case WIDX_TAB_8:
            case WIDX_TAB_9:
            case WIDX_TAB_10:
                SetPage(widgetIndex - WIDX_TAB_1);
                break;
        }
    }

    void VehicleResize()
    {
        WindowSetResize(*this, 316, 221, 316, 221);
    }

    void VehicleOnMouseDown(WidgetIndex widgetIndex)
    {
        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        switch (widgetIndex)
        {
            case WIDX_VEHICLE_TYPE_DROPDOWN:
                ShowVehicleTypeDropdown(&widgets[widgetIndex]);
                break;
            case WIDX_VEHICLE_REVERSED_TRAINS_CHECKBOX:
                ride->SetReversedTrains(!ride->HasLifecycleFlag(RIDE_LIFECYCLE_REVERSED_TRAINS));
                break;
            case WIDX_VEHICLE_TRAINS_INCREASE:
                if (ride->NumTrains < OpenRCT2::Limits::MaxTrainsPerRide)
                    ride->SetNumTrains(ride->NumTrains + 1);
                break;
            case WIDX_VEHICLE_TRAINS_DECREASE:
                if (ride->NumTrains > 1)
                    ride->SetNumTrains(ride->NumTrains - 1);
                break;
            case WIDX_VEHICLE_CARS_PER_TRAIN_INCREASE:
                if (ride->num_cars_per_train < OpenRCT2::Limits::MaxCarsPerTrain)
                    ride->SetNumCarsPerVehicle(ride->num_cars_per_train + 1);
                break;
            case WIDX_VEHICLE_CARS_PER_TRAIN_DECREASE:
                if (ride->num_cars_per_train > 1)
                    ride->SetNumCarsPerVehicle(ride->num_cars_per_train - 1);
                break;
        }
    }

    void VehicleOnDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
    {
        if (dropdownIndex == -1)
            return;

        switch (widgetIndex)
        {
            case WIDX_VEHICLE_TYPE_DROPDOWN:
                if (dropdownIndex >= 0 && static_cast<std::size_t>(dropdownIndex) < VehicleDropdownData.size())
                {
                    auto ride = GetRide(rideId);
                    if (ride != nullptr)
                    {
                        auto newRideType = VehicleDropdownData[dropdownIndex].subtype_id;
                        ride->SetRideEntry(newRideType);
                    }
                }
                break;
        }
    }

    void VehicleUpdate()
    {
        frame_no++;
        OnPrepareDraw();
        WidgetInvalidate(*this, WIDX_TAB_2);
    }

    OpenRCT2String VehicleTooltip(const WidgetIndex widgetIndex, StringId fallback)
    {
        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return { STR_NONE, {} };

        switch (widgetIndex)
        {
            case WIDX_VEHICLE_TRAINS:
            case WIDX_VEHICLE_TRAINS_DECREASE:
            case WIDX_VEHICLE_TRAINS_INCREASE:
            {
                auto ft = Formatter();
                ft.Increment(12);

                RideComponentType vehicleType = ride->GetRideTypeDescriptor().NameConvention.vehicle;
                StringId stringId = GetRideComponentName(vehicleType).count;
                if (ride->max_trains > 1)
                {
                    stringId = GetRideComponentName(vehicleType).count_plural;
                }
                ft.Add<StringId>(stringId);
                ft.Add<uint16_t>(ride->max_trains);
                return { fallback, ft };
            }
            case WIDX_VEHICLE_CARS_PER_TRAIN:
            case WIDX_VEHICLE_CARS_PER_TRAIN_DECREASE:
            case WIDX_VEHICLE_CARS_PER_TRAIN_INCREASE:
            {
                auto rideEntry = ride->GetRideEntry();
                if (rideEntry == nullptr)
                    return { STR_NONE, {} };

                auto ft = Formatter();
                ft.Increment(16);
                ft.Add<uint16_t>(std::max(uint8_t(1), ride->MaxCarsPerTrain) - rideEntry->zero_cars);

                StringId stringId = GetRideComponentName(RideComponentType::Car).singular;
                if (ride->MaxCarsPerTrain - rideEntry->zero_cars > 1)
                {
                    stringId = GetRideComponentName(RideComponentType::Car).plural;
                }
                ft.Add<StringId>(stringId);
                return { fallback, ft };
            }
        }
        return { fallback, {} };
    }

    void VehicleOnPrepareDraw()
    {
        StringId stringId;
        int32_t carsPerTrain;

        auto* newWidgets = window_ride_page_widgets[page];
        if (widgets != newWidgets)
        {
            widgets = newWidgets;
            InitScrollWidgets();
        }

        SetPressedTab();

        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        const auto* rideEntry = ride->GetRideEntry();

        widgets[WIDX_TITLE].text = STR_ARG_20_STRINGID;

        // Widget setup
        carsPerTrain = ride->num_cars_per_train - rideEntry->zero_cars;

        // Vehicle type
        window_ride_vehicle_widgets[WIDX_VEHICLE_TYPE].text = rideEntry->naming.Name;

        // Trains
        if (rideEntry->cars_per_flat_ride > 1 || gCheatsDisableTrainLengthLimit)
        {
            window_ride_vehicle_widgets[WIDX_VEHICLE_TRAINS].type = WindowWidgetType::Spinner;
            window_ride_vehicle_widgets[WIDX_VEHICLE_TRAINS_INCREASE].type = WindowWidgetType::Button;
            window_ride_vehicle_widgets[WIDX_VEHICLE_TRAINS_DECREASE].type = WindowWidgetType::Button;
        }
        else
        {
            window_ride_vehicle_widgets[WIDX_VEHICLE_TRAINS].type = WindowWidgetType::Empty;
            window_ride_vehicle_widgets[WIDX_VEHICLE_TRAINS_INCREASE].type = WindowWidgetType::Empty;
            window_ride_vehicle_widgets[WIDX_VEHICLE_TRAINS_DECREASE].type = WindowWidgetType::Empty;
        }

        // Cars per train
        if (rideEntry->zero_cars + 1 < rideEntry->max_cars_in_train || gCheatsDisableTrainLengthLimit)
        {
            window_ride_vehicle_widgets[WIDX_VEHICLE_CARS_PER_TRAIN].type = WindowWidgetType::Spinner;
            window_ride_vehicle_widgets[WIDX_VEHICLE_CARS_PER_TRAIN_INCREASE].type = WindowWidgetType::Button;
            window_ride_vehicle_widgets[WIDX_VEHICLE_CARS_PER_TRAIN_DECREASE].type = WindowWidgetType::Button;
        }
        else
        {
            window_ride_vehicle_widgets[WIDX_VEHICLE_CARS_PER_TRAIN].type = WindowWidgetType::Empty;
            window_ride_vehicle_widgets[WIDX_VEHICLE_CARS_PER_TRAIN_INCREASE].type = WindowWidgetType::Empty;
            window_ride_vehicle_widgets[WIDX_VEHICLE_CARS_PER_TRAIN_DECREASE].type = WindowWidgetType::Empty;
        }

        if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_ALLOW_REVERSED_TRAINS)
            || (gCheatsDisableTrainLengthLimit && !ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_FLAT_RIDE)))
        {
            window_ride_vehicle_widgets[WIDX_VEHICLE_REVERSED_TRAINS_CHECKBOX].type = WindowWidgetType::Checkbox;
            if (ride->HasLifecycleFlag(RIDE_LIFECYCLE_REVERSED_TRAINS))
            {
                pressed_widgets |= (1uLL << WIDX_VEHICLE_REVERSED_TRAINS_CHECKBOX);
            }
            else
            {
                pressed_widgets &= ~(1uLL << WIDX_VEHICLE_REVERSED_TRAINS_CHECKBOX);
            }
        }
        else
        {
            window_ride_vehicle_widgets[WIDX_VEHICLE_REVERSED_TRAINS_CHECKBOX].type = WindowWidgetType::Empty;
        }

        auto ft = Formatter::Common();
        ft.Increment(6);
        ft.Add<uint16_t>(carsPerTrain);
        RideComponentType vehicleType = ride->GetRideTypeDescriptor().NameConvention.vehicle;
        stringId = GetRideComponentName(vehicleType).count;
        if (ride->NumTrains > 1)
        {
            stringId = GetRideComponentName(vehicleType).count_plural;
        }
        ft.Add<StringId>(stringId);
        ft.Add<uint16_t>(ride->NumTrains);

        ft.Increment(8);

        ride->FormatNameTo(ft);

        AnchorBorderWidgets();
        WindowAlignTabs(this, WIDX_TAB_1, WIDX_TAB_10);

        if (abs(ride->num_cars_per_train - rideEntry->zero_cars) == 1)
        {
            window_ride_vehicle_widgets[WIDX_VEHICLE_CARS_PER_TRAIN].text = STR_1_CAR_PER_TRAIN;
        }
        else
        {
            window_ride_vehicle_widgets[WIDX_VEHICLE_CARS_PER_TRAIN].text = STR_X_CARS_PER_TRAIN;
        }
    }

    void VehicleOnDraw(DrawPixelInfo& dpi)
    {
        WindowDrawWidgets(*this, dpi);
        DrawTabImages(dpi);

        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        auto rideEntry = ride->GetRideEntry();
        if (rideEntry == nullptr)
            return;

        auto screenCoords = windowPos + ScreenCoordsXY{ 8, 64 };

        // Description
        auto ft = Formatter();
        ft.Add<StringId>(rideEntry->naming.Description);
        screenCoords.y += DrawTextWrapped(dpi, screenCoords, 300, STR_BLACK_STRING, ft, { TextAlignment::LEFT });
        screenCoords.y += 2;

        // Capacity
        ft = Formatter();
        ft.Add<StringId>(rideEntry->capacity);
        DrawTextBasic(dpi, screenCoords, STR_CAPACITY, ft);

        // Excitement Factor
        if (rideEntry->excitement_multiplier != 0)
        {
            screenCoords.y += LIST_ROW_HEIGHT;

            ft = Formatter();
            ft.Add<int16_t>(abs(rideEntry->excitement_multiplier));
            StringId stringId = rideEntry->excitement_multiplier > 0 ? STR_EXCITEMENT_FACTOR : STR_EXCITEMENT_FACTOR_NEGATIVE;
            DrawTextBasic(dpi, screenCoords, stringId, ft);
        }

        // Intensity Factor
        if (rideEntry->intensity_multiplier != 0)
        {
            int32_t lineHeight = FontGetLineHeight(FontStyle::Medium);
            if (lineHeight != 10)
                screenCoords.x += 150;
            else
                screenCoords.y += LIST_ROW_HEIGHT;

            ft = Formatter();
            ft.Add<int16_t>(abs(rideEntry->intensity_multiplier));
            StringId stringId = rideEntry->intensity_multiplier > 0 ? STR_INTENSITY_FACTOR : STR_INTENSITY_FACTOR_NEGATIVE;
            DrawTextBasic(dpi, screenCoords, stringId, ft);

            if (lineHeight != 10)
                screenCoords.x -= 150;
        }

        // Nausea Factor
        if (rideEntry->nausea_multiplier != 0)
        {
            screenCoords.y += LIST_ROW_HEIGHT;

            ft = Formatter();
            ft.Add<int16_t>(abs(rideEntry->nausea_multiplier));
            StringId stringId = rideEntry->nausea_multiplier > 0 ? STR_NAUSEA_FACTOR : STR_NAUSEA_FACTOR_NEGATIVE;
            DrawTextBasic(dpi, screenCoords, stringId, ft);
        }
    }

    struct VehicleDrawInfo
    {
        int16_t x;
        int16_t y;
        ImageId imageId;
    };

    void VehicleOnScrollDraw(DrawPixelInfo& dpi, int32_t scrollIndex)
    {
        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        const auto* rideEntry = ride->GetRideEntry();

        // Background
        GfxFillRect(dpi, { { dpi.x, dpi.y }, { dpi.x + dpi.width, dpi.y + dpi.height } }, PALETTE_INDEX_12);

        Widget* widget = &window_ride_vehicle_widgets[WIDX_VEHICLE_TRAINS_PREVIEW];
        int32_t startX = std::max(2, (widget->width() - ((ride->NumTrains - 1) * 36)) / 2 - 25);
        int32_t startY = widget->height() - 4;

        bool isReversed = ride->HasLifecycleFlag(RIDE_LIFECYCLE_REVERSED_TRAINS);
        int32_t carIndex = (isReversed) ? ride->num_cars_per_train - 1 : 0;

        const auto& firstCarEntry = rideEntry->Cars[RideEntryGetVehicleAtPosition(
            ride->subtype, ride->num_cars_per_train, carIndex)];
        startY += firstCarEntry.tab_height;

        // For each train
        for (int32_t i = 0; i < ride->NumTrains; i++)
        {
            VehicleDrawInfo trainCarImages[OpenRCT2::Limits::MaxCarsPerTrain];
            VehicleDrawInfo* nextSpriteToDraw = trainCarImages;
            int32_t x = startX;
            int32_t y = startY;

            // For each car in train
            static_assert(std::numeric_limits<decltype(ride->num_cars_per_train)>::max() <= std::size(trainCarImages));
            for (int32_t j = 0; j < ride->num_cars_per_train; j++)
            {
                carIndex = (isReversed) ? (ride->num_cars_per_train - 1) - j : j;

                const auto& carEntry = rideEntry->Cars[RideEntryGetVehicleAtPosition(
                    ride->subtype, ride->num_cars_per_train, carIndex)];
                x += carEntry.spacing / 17432;
                y -= (carEntry.spacing / 2) / 17432;

                // Get colour of vehicle
                int32_t vehicleColourIndex = 0;
                switch (ride->colour_scheme_type & 3)
                {
                    case VEHICLE_COLOUR_SCHEME_SAME:
                        vehicleColourIndex = 0;
                        break;
                    case VEHICLE_COLOUR_SCHEME_PER_TRAIN:
                        vehicleColourIndex = i;
                        break;
                    case VEHICLE_COLOUR_SCHEME_PER_VEHICLE:
                        vehicleColourIndex = carIndex;
                        break;
                }
                VehicleColour vehicleColour = RideGetVehicleColour(*ride, vehicleColourIndex);

                ImageIndex imageIndex = carEntry.SpriteByYaw(
                    OpenRCT2::Entity::Yaw::BaseRotation / 2, SpriteGroupType::SlopeFlat);
                if (isReversed)
                {
                    auto baseRotation = carEntry.NumRotationSprites(SpriteGroupType::SlopeFlat);
                    imageIndex = carEntry.SpriteByYaw(
                        (imageIndex + (baseRotation / 2)) & (baseRotation - 1), SpriteGroupType::SlopeFlat);
                }

                imageIndex &= carEntry.TabRotationMask;
                imageIndex *= carEntry.base_num_frames;
                imageIndex += carEntry.base_image_id;

                auto imageId = ImageId(imageIndex, vehicleColour.Body, vehicleColour.Trim, vehicleColour.Tertiary);

                nextSpriteToDraw->x = x;
                nextSpriteToDraw->y = y;
                nextSpriteToDraw->imageId = imageId;
                nextSpriteToDraw++;

                x += carEntry.spacing / 17432;
                y -= (carEntry.spacing / 2) / 17432;
            }

            if (ride->type == RIDE_TYPE_REVERSER_ROLLER_COASTER)
            {
                VehicleDrawInfo tmp = *(nextSpriteToDraw - 1);
                *(nextSpriteToDraw - 1) = *(nextSpriteToDraw - 2);
                *(nextSpriteToDraw - 2) = tmp;
            }

            VehicleDrawInfo* current = nextSpriteToDraw;
            while (--current >= trainCarImages)
                GfxDrawSprite(dpi, current->imageId, { current->x, current->y });

            startX += 36;
        }
    }

#pragma endregion

#pragma region Operating

    void ModeTweakIncrease()
    {
        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        const auto& operatingSettings = ride->GetRideTypeDescriptor().OperatingSettings;
        uint8_t maxValue = operatingSettings.MaxValue;
        uint8_t minValue = gCheatsUnlockOperatingLimits ? 0 : operatingSettings.MinValue;

        if (gCheatsUnlockOperatingLimits)
        {
            maxValue = OpenRCT2::Limits::CheatsMaxOperatingLimit;
        }

        uint8_t increment = ride->mode == RideMode::Dodgems ? 10 : 1;

        SetOperatingSetting(
            rideId, RideSetSetting::Operation, std::clamp<int16_t>(ride->operation_option + increment, minValue, maxValue));
    }

    void ModeTweakDecrease()
    {
        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        const auto& operatingSettings = ride->GetRideTypeDescriptor().OperatingSettings;
        uint8_t maxValue = operatingSettings.MaxValue;
        uint8_t minValue = gCheatsUnlockOperatingLimits ? 0 : operatingSettings.MinValue;
        if (gCheatsUnlockOperatingLimits)
        {
            maxValue = OpenRCT2::Limits::CheatsMaxOperatingLimit;
        }

        uint8_t decrement = ride->mode == RideMode::Dodgems ? 10 : 1;

        SetOperatingSetting(
            rideId, RideSetSetting::Operation, std::clamp<int16_t>(ride->operation_option - decrement, minValue, maxValue));
    }

    void ModeDropdown(Widget* widget)
    {
        Widget* dropdownWidget;

        dropdownWidget = widget - 1;
        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        auto availableModes = ride->GetAvailableModes();

        // Create dropdown list
        auto numAvailableModes = 0;
        auto checkedIndex = -1;
        for (auto i = 0; i < static_cast<uint8_t>(RideMode::Count); i++)
        {
            if (availableModes & (1uLL << i))
            {
                gDropdownItems[numAvailableModes].Format = STR_DROPDOWN_MENU_LABEL;
                gDropdownItems[numAvailableModes].Args = RideModeNames[i];

                if (ride->mode == static_cast<RideMode>(i))
                    checkedIndex = numAvailableModes;

                numAvailableModes++;
            }
        }

        WindowDropdownShowTextCustomWidth(
            { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1, colours[1],
            0, Dropdown::Flag::StayOpen, numAvailableModes, widget->right - dropdownWidget->left);

        if (checkedIndex != -1)
        {
            Dropdown::SetChecked(checkedIndex, true);
        }
    }

    void LoadDropdown(Widget* widget)
    {
        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        auto dropdownWidget = widget - 1;
        for (auto i = 0; i < 5; i++)
        {
            gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
            gDropdownItems[i].Args = VehicleLoadNames[i];
        }
        WindowDropdownShowTextCustomWidth(
            { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1, colours[1],
            0, Dropdown::Flag::StayOpen, 5, widget->right - dropdownWidget->left);

        Dropdown::SetChecked(ride->depart_flags & RIDE_DEPART_WAIT_FOR_LOAD_MASK, true);
    }

    void OperatingOnMouseUp(WidgetIndex widgetIndex)
    {
        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                Close();
                return;
            case WIDX_TAB_1:
            case WIDX_TAB_2:
            case WIDX_TAB_3:
            case WIDX_TAB_4:
            case WIDX_TAB_5:
            case WIDX_TAB_6:
            case WIDX_TAB_7:
            case WIDX_TAB_8:
            case WIDX_TAB_9:
            case WIDX_TAB_10:
                SetPage(widgetIndex - WIDX_TAB_1);
                break;
            case WIDX_LOAD_CHECKBOX:
                SetOperatingSetting(rideId, RideSetSetting::Departure, ride->depart_flags ^ RIDE_DEPART_WAIT_FOR_LOAD);
                break;
            case WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX:
                SetOperatingSetting(
                    rideId, RideSetSetting::Departure, ride->depart_flags ^ RIDE_DEPART_LEAVE_WHEN_ANOTHER_ARRIVES);
                break;
            case WIDX_MINIMUM_LENGTH_CHECKBOX:
                SetOperatingSetting(
                    rideId, RideSetSetting::Departure, ride->depart_flags ^ RIDE_DEPART_WAIT_FOR_MINIMUM_LENGTH);
                break;
            case WIDX_MAXIMUM_LENGTH_CHECKBOX:
                SetOperatingSetting(
                    rideId, RideSetSetting::Departure, ride->depart_flags ^ RIDE_DEPART_WAIT_FOR_MAXIMUM_LENGTH);
                break;
            case WIDX_SYNCHRONISE_WITH_ADJACENT_STATIONS_CHECKBOX:
                SetOperatingSetting(
                    rideId, RideSetSetting::Departure, ride->depart_flags ^ RIDE_DEPART_SYNCHRONISE_WITH_ADJACENT_STATIONS);
                break;
        }
    }

    void OperatingResize()
    {
        WindowSetResize(*this, 316, 186, 316, 186);
    }

    void OperatingOnMouseDown(WidgetIndex widgetIndex)
    {
        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        uint8_t upper_bound, lower_bound;
        switch (widgetIndex)
        {
            case WIDX_MODE_TWEAK:
                OperatingTweakTextInput(*ride);
                break;
            case WIDX_MODE_TWEAK_INCREASE:
                ModeTweakIncrease();
                break;
            case WIDX_MODE_TWEAK_DECREASE:
                ModeTweakDecrease();
                break;
            case WIDX_LIFT_HILL_SPEED_INCREASE:
                upper_bound = gCheatsUnlockOperatingLimits ? OpenRCT2::Limits::CheatsMaxOperatingLimit
                                                           : ride->GetRideTypeDescriptor().LiftData.maximum_speed;
                lower_bound = gCheatsUnlockOperatingLimits ? 0 : ride->GetRideTypeDescriptor().LiftData.minimum_speed;
                SetOperatingSetting(
                    rideId, RideSetSetting::LiftHillSpeed,
                    std::clamp<int16_t>(ride->lift_hill_speed + 1, lower_bound, upper_bound));
                break;
            case WIDX_LIFT_HILL_SPEED_DECREASE:
                upper_bound = gCheatsUnlockOperatingLimits ? OpenRCT2::Limits::CheatsMaxOperatingLimit
                                                           : ride->GetRideTypeDescriptor().LiftData.maximum_speed;
                lower_bound = gCheatsUnlockOperatingLimits ? 0 : ride->GetRideTypeDescriptor().LiftData.minimum_speed;
                SetOperatingSetting(
                    rideId, RideSetSetting::LiftHillSpeed,
                    std::clamp<int16_t>(ride->lift_hill_speed - 1, lower_bound, upper_bound));
                break;
            case WIDX_MINIMUM_LENGTH:
                OperatingLengthWindow(WIDX_MINIMUM_LENGTH);
                break;
            case WIDX_MAXIMUM_LENGTH:
                OperatingLengthWindow(WIDX_MAXIMUM_LENGTH);
                break;
            case WIDX_MINIMUM_LENGTH_INCREASE:
                upper_bound = OpenRCT2::Limits::MaxWaitingTime;
                lower_bound = 0;
                SetOperatingSetting(
                    rideId, RideSetSetting::MinWaitingTime,
                    std::clamp<int16_t>(ride->min_waiting_time + 1, lower_bound, upper_bound));
                break;
            case WIDX_MINIMUM_LENGTH_DECREASE:
                upper_bound = OpenRCT2::Limits::MaxWaitingTime;
                lower_bound = 0;
                SetOperatingSetting(
                    rideId, RideSetSetting::MinWaitingTime,
                    std::clamp<int16_t>(ride->min_waiting_time - 1, lower_bound, upper_bound));
                break;
            case WIDX_MAXIMUM_LENGTH_INCREASE:
                upper_bound = OpenRCT2::Limits::MaxWaitingTime;
                lower_bound = 0;
                SetOperatingSetting(
                    rideId, RideSetSetting::MaxWaitingTime,
                    std::clamp<int16_t>(ride->max_waiting_time + 1, lower_bound, upper_bound));
                break;
            case WIDX_MAXIMUM_LENGTH_DECREASE:
                upper_bound = OpenRCT2::Limits::MaxWaitingTime;
                lower_bound = 0;
                SetOperatingSetting(
                    rideId, RideSetSetting::MaxWaitingTime,
                    std::clamp<int16_t>(ride->max_waiting_time - 1, lower_bound, upper_bound));
                break;
            case WIDX_MODE_DROPDOWN:
                ModeDropdown(&widgets[widgetIndex]);
                break;
            case WIDX_LOAD_DROPDOWN:
                LoadDropdown(&widgets[widgetIndex]);
                break;
            case WIDX_OPERATE_NUMBER_OF_CIRCUITS_INCREASE:
                upper_bound = gCheatsUnlockOperatingLimits ? OpenRCT2::Limits::CheatsMaxOperatingLimit
                                                           : OpenRCT2::Limits::MaxCircuitsPerRide;
                lower_bound = 1;
                SetOperatingSetting(
                    rideId, RideSetSetting::NumCircuits, std::clamp<int16_t>(ride->num_circuits + 1, lower_bound, upper_bound));
                break;
            case WIDX_OPERATE_NUMBER_OF_CIRCUITS_DECREASE:
                upper_bound = gCheatsUnlockOperatingLimits ? OpenRCT2::Limits::CheatsMaxOperatingLimit
                                                           : OpenRCT2::Limits::MaxCircuitsPerRide;
                lower_bound = 1;
                SetOperatingSetting(
                    rideId, RideSetSetting::NumCircuits, std::clamp<int16_t>(ride->num_circuits - 1, lower_bound, upper_bound));
                break;
        }
    }

    void OperatingLengthWindow(WidgetIndex widgetIndex)
    {
        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        uint8_t upperBound = OpenRCT2::Limits::MaxWaitingTime;
        uint8_t lowerBound = 0;
        Formatter ft;
        ft.Add<int16_t>(lowerBound);
        ft.Add<int16_t>(upperBound);
        auto title = (widgetIndex == WIDX_MINIMUM_LENGTH) ? STR_MINIMUM_WAITING_TIME : STR_MAXIMUM_WAITING_TIME;
        auto currentValue = (widgetIndex == WIDX_MINIMUM_LENGTH) ? ride->min_waiting_time : ride->max_waiting_time;
        char buffer[5]{};
        snprintf(buffer, std::size(buffer), "%u", currentValue);
        WindowTextInputRawOpen(this, widgetIndex, title, STR_ENTER_VALUE, ft, buffer, 4);
    }

    void OperatingTweakTextInput(const Ride& ride)
    {
        switch (ride.mode)
        {
            case RideMode::PoweredLaunchPasstrough:
            case RideMode::PoweredLaunch:
            case RideMode::UpwardLaunch:
            case RideMode::PoweredLaunchBlockSectioned:
            case RideMode::StationToStation:
            case RideMode::Dodgems:
                return;
            default:
                break;
        }

        const auto& operatingSettings = ride.GetRideTypeDescriptor().OperatingSettings;
        int16_t maxValue = gCheatsUnlockOperatingLimits ? OpenRCT2::Limits::CheatsMaxOperatingLimit
                                                        : operatingSettings.MaxValue;
        int16_t minValue = gCheatsUnlockOperatingLimits ? 0 : operatingSettings.MinValue;

        const auto& title = window_ride_operating_widgets[WIDX_MODE_TWEAK_LABEL].text;
        Formatter ft;
        ft.Add<int16_t>(minValue * operatingSettings.OperatingSettingMultiplier);
        ft.Add<int16_t>(maxValue * operatingSettings.OperatingSettingMultiplier);

        uint16_t currentValue = static_cast<uint16_t>(ride.operation_option) * operatingSettings.OperatingSettingMultiplier;
        char buffer[6]{};
        snprintf(buffer, std::size(buffer), "%u", currentValue);

        WindowTextInputRawOpen(this, WIDX_MODE_TWEAK, title, STR_ENTER_VALUE, ft, buffer, 4);
    }

    void OperatingOnDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
    {
        if (dropdownIndex == -1)
            return;

        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        switch (widgetIndex)
        {
            case WIDX_MODE_DROPDOWN:
            {
                RideMode rideMode = RideMode::NullMode;
                auto availableModes = ride->GetAvailableModes();
                auto modeInDropdownIndex = -1;
                for (RideMode rideModeIndex = RideMode::Normal; rideModeIndex < RideMode::Count; rideModeIndex++)
                {
                    if (availableModes & EnumToFlag(rideModeIndex))
                    {
                        modeInDropdownIndex++;
                        if (modeInDropdownIndex == dropdownIndex)
                        {
                            rideMode = rideModeIndex;
                            break;
                        }
                    }
                }
                if (rideMode != RideMode::NullMode)
                    SetOperatingSetting(rideId, RideSetSetting::Mode, static_cast<uint8_t>(rideMode));
                break;
            }
            case WIDX_LOAD_DROPDOWN:
                SetOperatingSetting(
                    rideId, RideSetSetting::Departure, (ride->depart_flags & ~RIDE_DEPART_WAIT_FOR_LOAD_MASK) | dropdownIndex);
                break;
        }
    }

    void OperatingUpdate()
    {
        frame_no++;
        OnPrepareDraw();
        WidgetInvalidate(*this, WIDX_TAB_3);

        auto ride = GetRide(rideId);
        if (ride != nullptr && ride->window_invalidate_flags & RIDE_INVALIDATE_RIDE_OPERATING)
        {
            ride->window_invalidate_flags &= ~RIDE_INVALIDATE_RIDE_OPERATING;
            Invalidate();
        }
    }

    void OperatingOnTextInput(WidgetIndex widgetIndex, std::string_view text)
    {
        if (text.empty())
            return;

        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        if (widgetIndex == WIDX_MODE_TWEAK)
        {
            const auto& operatingSettings = ride->GetRideTypeDescriptor().OperatingSettings;
            uint32_t maxValue = gCheatsUnlockOperatingLimits ? OpenRCT2::Limits::CheatsMaxOperatingLimit
                                                             : operatingSettings.MaxValue;
            uint32_t minValue = gCheatsUnlockOperatingLimits ? 0 : operatingSettings.MinValue;
            auto multiplier = ride->GetRideTypeDescriptor().OperatingSettings.OperatingSettingMultiplier;

            try
            {
                uint32_t origSize = std::stol(std::string(text)) / multiplier;
                uint8_t size = static_cast<uint8_t>(std::clamp(origSize, minValue, maxValue));
                SetOperatingSetting(ride->id, RideSetSetting::Operation, size);
            }
            catch (const std::logic_error&)
            {
                // std::stol can throw std::out_of_range or std::invalid_argument
            }
        }
        else if (widgetIndex == WIDX_MINIMUM_LENGTH || widgetIndex == WIDX_MAXIMUM_LENGTH)
        {
            try
            {
                auto rideSetSetting = widgetIndex == WIDX_MINIMUM_LENGTH ? RideSetSetting::MinWaitingTime
                                                                         : RideSetSetting::MaxWaitingTime;

                uint16_t upperBound = OpenRCT2::Limits::MaxWaitingTime;
                uint16_t lowerBound = 0;
                uint16_t size = std::stol(std::string(text));
                size = std::clamp(size, lowerBound, upperBound);
                SetOperatingSetting(ride->id, rideSetSetting, size);
            }
            catch (const std::logic_error&)
            {
                // std::stol can throw std::out_of_range or std::invalid_argument
            }
        }
    }

    void OperatingOnPrepareDraw()
    {
        StringId format, caption, tooltip;

        auto* newWidgets = window_ride_page_widgets[page];
        if (widgets != newWidgets)
        {
            widgets = newWidgets;
            InitScrollWidgets();
        }

        SetPressedTab();

        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        auto ft = Formatter::Common();
        ride->FormatNameTo(ft);

        // Widget setup
        pressed_widgets &= ~(
            (1uLL << WIDX_LOAD_CHECKBOX) | (1uLL << WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX)
            | (1uLL << WIDX_MINIMUM_LENGTH_CHECKBOX) | (1uLL << WIDX_MAXIMUM_LENGTH_CHECKBOX)
            | (1uLL << WIDX_SYNCHRONISE_WITH_ADJACENT_STATIONS_CHECKBOX));

        // Sometimes, only one of the alternatives support lift hill pieces. Make sure to check both.
        bool hasAlternativeType = ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_ALTERNATIVE_TRACK_TYPE);
        if (ride->GetRideTypeDescriptor().SupportsTrackPiece(TRACK_LIFT_HILL)
            || (hasAlternativeType
                && GetRideTypeDescriptor(ride->GetRideTypeDescriptor().AlternateType).SupportsTrackPiece(TRACK_LIFT_HILL)))
        {
            window_ride_operating_widgets[WIDX_LIFT_HILL_SPEED_LABEL].type = WindowWidgetType::Label;
            window_ride_operating_widgets[WIDX_LIFT_HILL_SPEED].type = WindowWidgetType::Spinner;
            window_ride_operating_widgets[WIDX_LIFT_HILL_SPEED_INCREASE].type = WindowWidgetType::Button;
            window_ride_operating_widgets[WIDX_LIFT_HILL_SPEED_DECREASE].type = WindowWidgetType::Button;
            ft.Rewind();
            ft.Increment(20);
            ft.Add<uint16_t>(ride->lift_hill_speed);
        }
        else
        {
            window_ride_operating_widgets[WIDX_LIFT_HILL_SPEED_LABEL].type = WindowWidgetType::Empty;
            window_ride_operating_widgets[WIDX_LIFT_HILL_SPEED].type = WindowWidgetType::Empty;
            window_ride_operating_widgets[WIDX_LIFT_HILL_SPEED_INCREASE].type = WindowWidgetType::Empty;
            window_ride_operating_widgets[WIDX_LIFT_HILL_SPEED_DECREASE].type = WindowWidgetType::Empty;
        }

        // Number of circuits
        if (ride->CanHaveMultipleCircuits())
        {
            window_ride_operating_widgets[WIDX_OPERATE_NUMBER_OF_CIRCUITS_LABEL].type = WindowWidgetType::Label;
            window_ride_operating_widgets[WIDX_OPERATE_NUMBER_OF_CIRCUITS].type = WindowWidgetType::Spinner;
            window_ride_operating_widgets[WIDX_OPERATE_NUMBER_OF_CIRCUITS_INCREASE].type = WindowWidgetType::Button;
            window_ride_operating_widgets[WIDX_OPERATE_NUMBER_OF_CIRCUITS_DECREASE].type = WindowWidgetType::Button;
            ft.Rewind();
            ft.Increment(22);
            ft.Add<uint16_t>(ride->num_circuits);
        }
        else
        {
            window_ride_operating_widgets[WIDX_OPERATE_NUMBER_OF_CIRCUITS_LABEL].type = WindowWidgetType::Empty;
            window_ride_operating_widgets[WIDX_OPERATE_NUMBER_OF_CIRCUITS].type = WindowWidgetType::Empty;
            window_ride_operating_widgets[WIDX_OPERATE_NUMBER_OF_CIRCUITS_INCREASE].type = WindowWidgetType::Empty;
            window_ride_operating_widgets[WIDX_OPERATE_NUMBER_OF_CIRCUITS_DECREASE].type = WindowWidgetType::Empty;
        }

        // Leave if another vehicle arrives at station
        if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_LEAVE_WHEN_ANOTHER_VEHICLE_ARRIVES_AT_STATION)
            && ride->NumTrains > 1 && !ride->IsBlockSectioned())
        {
            window_ride_operating_widgets[WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX].type = WindowWidgetType::Checkbox;
            window_ride_operating_widgets[WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX].tooltip
                = STR_LEAVE_IF_ANOTHER_VEHICLE_ARRIVES_TIP;
            window_ride_operating_widgets[WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX].text = ride->GetRideTypeDescriptor()
                                                                                               .NameConvention.vehicle
                    == RideComponentType::Boat
                ? STR_LEAVE_IF_ANOTHER_BOAT_ARRIVES
                : STR_LEAVE_IF_ANOTHER_TRAIN_ARRIVES;
        }
        else
        {
            window_ride_operating_widgets[WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX].type = WindowWidgetType::Empty;
        }

        // Synchronise with adjacent stations
        if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_CAN_SYNCHRONISE_ADJACENT_STATIONS))
        {
            window_ride_operating_widgets[WIDX_SYNCHRONISE_WITH_ADJACENT_STATIONS_CHECKBOX].type = WindowWidgetType::Checkbox;
            window_ride_operating_widgets[WIDX_SYNCHRONISE_WITH_ADJACENT_STATIONS_CHECKBOX].text
                = STR_SYNCHRONISE_WITH_ADJACENT_STATIONS;
            window_ride_operating_widgets[WIDX_SYNCHRONISE_WITH_ADJACENT_STATIONS_CHECKBOX].tooltip
                = STR_SYNCHRONISE_WITH_ADJACENT_STATIONS_TIP;
        }
        else
        {
            window_ride_operating_widgets[WIDX_SYNCHRONISE_WITH_ADJACENT_STATIONS_CHECKBOX].type = WindowWidgetType::Empty;
        }

        // Mode
        window_ride_operating_widgets[WIDX_MODE].text = RideModeNames[static_cast<int>(ride->mode)];

        // Waiting
        window_ride_operating_widgets[WIDX_LOAD].text = VehicleLoadNames[(ride->depart_flags & RIDE_DEPART_WAIT_FOR_LOAD_MASK)];
        if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_LOAD_OPTIONS))
        {
            window_ride_operating_widgets[WIDX_LOAD_CHECKBOX].type = WindowWidgetType::Checkbox;
            window_ride_operating_widgets[WIDX_LOAD].type = WindowWidgetType::DropdownMenu;
            window_ride_operating_widgets[WIDX_LOAD_DROPDOWN].type = WindowWidgetType::Button;

            window_ride_operating_widgets[WIDX_MINIMUM_LENGTH_CHECKBOX].type = WindowWidgetType::Checkbox;
            window_ride_operating_widgets[WIDX_MINIMUM_LENGTH].type = WindowWidgetType::Spinner;
            window_ride_operating_widgets[WIDX_MINIMUM_LENGTH_INCREASE].type = WindowWidgetType::Button;
            window_ride_operating_widgets[WIDX_MINIMUM_LENGTH_DECREASE].type = WindowWidgetType::Button;

            window_ride_operating_widgets[WIDX_MAXIMUM_LENGTH_CHECKBOX].type = WindowWidgetType::Checkbox;
            window_ride_operating_widgets[WIDX_MAXIMUM_LENGTH].type = WindowWidgetType::Spinner;
            window_ride_operating_widgets[WIDX_MAXIMUM_LENGTH_INCREASE].type = WindowWidgetType::Button;
            window_ride_operating_widgets[WIDX_MAXIMUM_LENGTH_DECREASE].type = WindowWidgetType::Button;

            ft.Rewind();
            ft.Increment(10);
            ft.Add<StringId>(STR_FORMAT_SECONDS);
            ft.Add<uint16_t>(ride->min_waiting_time);
            ft.Add<StringId>(STR_FORMAT_SECONDS);
            ft.Add<uint16_t>(ride->max_waiting_time);

            if (ride->depart_flags & RIDE_DEPART_WAIT_FOR_LOAD)
                pressed_widgets |= (1uLL << WIDX_LOAD_CHECKBOX);
        }
        else
        {
            window_ride_operating_widgets[WIDX_LOAD_CHECKBOX].type = WindowWidgetType::Empty;
            window_ride_operating_widgets[WIDX_LOAD].type = WindowWidgetType::Empty;
            window_ride_operating_widgets[WIDX_LOAD_DROPDOWN].type = WindowWidgetType::Empty;

            window_ride_operating_widgets[WIDX_MINIMUM_LENGTH_CHECKBOX].type = WindowWidgetType::Empty;
            window_ride_operating_widgets[WIDX_MINIMUM_LENGTH].type = WindowWidgetType::Empty;
            window_ride_operating_widgets[WIDX_MINIMUM_LENGTH_INCREASE].type = WindowWidgetType::Empty;
            window_ride_operating_widgets[WIDX_MINIMUM_LENGTH_DECREASE].type = WindowWidgetType::Empty;

            window_ride_operating_widgets[WIDX_MAXIMUM_LENGTH_CHECKBOX].type = WindowWidgetType::Empty;
            window_ride_operating_widgets[WIDX_MAXIMUM_LENGTH].type = WindowWidgetType::Empty;
            window_ride_operating_widgets[WIDX_MAXIMUM_LENGTH_INCREASE].type = WindowWidgetType::Empty;
            window_ride_operating_widgets[WIDX_MAXIMUM_LENGTH_DECREASE].type = WindowWidgetType::Empty;
        }

        if (ride->depart_flags & RIDE_DEPART_LEAVE_WHEN_ANOTHER_ARRIVES)
            pressed_widgets |= (1uLL << WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX);
        if (ride->depart_flags & RIDE_DEPART_SYNCHRONISE_WITH_ADJACENT_STATIONS)
            pressed_widgets |= (1uLL << WIDX_SYNCHRONISE_WITH_ADJACENT_STATIONS_CHECKBOX);
        if (ride->depart_flags & RIDE_DEPART_WAIT_FOR_MINIMUM_LENGTH)
            pressed_widgets |= (1uLL << WIDX_MINIMUM_LENGTH_CHECKBOX);
        if (ride->depart_flags & RIDE_DEPART_WAIT_FOR_MAXIMUM_LENGTH)
            pressed_widgets |= (1uLL << WIDX_MAXIMUM_LENGTH_CHECKBOX);

        // Mode specific functionality
        auto multiplier = ride->GetRideTypeDescriptor().OperatingSettings.OperatingSettingMultiplier;
        ft.Rewind();
        ft.Increment(18);
        ft.Add<uint16_t>(static_cast<uint16_t>(ride->operation_option) * multiplier);
        switch (ride->mode)
        {
            case RideMode::PoweredLaunchPasstrough:
            case RideMode::PoweredLaunch:
            case RideMode::UpwardLaunch:
            case RideMode::PoweredLaunchBlockSectioned:
                ft.Rewind();
                ft.Increment(18);
                ft.Add<uint16_t>((ride->launch_speed * 9) / 4);
                format = STR_RIDE_MODE_SPEED_VALUE;
                caption = STR_LAUNCH_SPEED;
                tooltip = STR_LAUNCH_SPEED_TIP;
                break;
            case RideMode::StationToStation:
                ft.Rewind();
                ft.Increment(18);
                ft.Add<uint16_t>((ride->speed * 9) / 4);
                format = STR_RIDE_MODE_SPEED_VALUE;
                caption = STR_SPEED;
                tooltip = STR_SPEED_TIP;
                break;
            case RideMode::Race:
                ft.Rewind();
                ft.Increment(18);
                ft.Add<uint16_t>(ride->NumLaps);
                format = STR_NUMBER_OF_LAPS_VALUE;
                caption = STR_NUMBER_OF_LAPS;
                tooltip = STR_NUMBER_OF_LAPS_TIP;
                break;
            case RideMode::Dodgems:
                format = STR_RIDE_MODE_TIME_LIMIT_VALUE;
                caption = STR_TIME_LIMIT;
                tooltip = STR_TIME_LIMIT_TIP;
                break;
            case RideMode::Swing:
                format = STR_RIDE_MODE_NUMBER_OF_SWINGS_VALUE;
                caption = STR_NUMBER_OF_SWINGS;
                tooltip = STR_NUMBER_OF_SWINGS_TIP;
                break;
            case RideMode::Rotation:
            case RideMode::ForwardRotation:
            case RideMode::BackwardRotation:
                format = STR_NUMBER_OF_ROTATIONS_VALUE;
                caption = STR_NUMBER_OF_ROTATIONS;
                tooltip = STR_NUMBER_OF_ROTATIONS_TIP;
                break;
            default:
                format = STR_MAX_PEOPLE_ON_RIDE_VALUE;
                caption = STR_MAX_PEOPLE_ON_RIDE;
                tooltip = STR_MAX_PEOPLE_ON_RIDE_TIP;
                if (!ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_NO_VEHICLES))
                    format = 0;
                break;
        }

        if (format != 0)
        {
            window_ride_operating_widgets[WIDX_MODE_TWEAK_LABEL].type = WindowWidgetType::Label;
            window_ride_operating_widgets[WIDX_MODE_TWEAK_LABEL].text = caption;
            window_ride_operating_widgets[WIDX_MODE_TWEAK_LABEL].tooltip = tooltip;
            window_ride_operating_widgets[WIDX_MODE_TWEAK].type = WindowWidgetType::Spinner;
            window_ride_operating_widgets[WIDX_MODE_TWEAK].text = format;
            window_ride_operating_widgets[WIDX_MODE_TWEAK_INCREASE].type = WindowWidgetType::Button;
            window_ride_operating_widgets[WIDX_MODE_TWEAK_DECREASE].type = WindowWidgetType::Button;
            pressed_widgets &= ~(1uLL << WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX);
        }
        else
        {
            window_ride_operating_widgets[WIDX_MODE_TWEAK_LABEL].type = WindowWidgetType::Empty;
            window_ride_operating_widgets[WIDX_MODE_TWEAK].type = WindowWidgetType::Empty;
            window_ride_operating_widgets[WIDX_MODE_TWEAK_INCREASE].type = WindowWidgetType::Empty;
            window_ride_operating_widgets[WIDX_MODE_TWEAK_DECREASE].type = WindowWidgetType::Empty;
        }

        AnchorBorderWidgets();
        WindowAlignTabs(this, WIDX_TAB_1, WIDX_TAB_10);
    }

    void OperatingOnDraw(DrawPixelInfo& dpi)
    {
        DrawWidgets(dpi);
        DrawTabImages(dpi);

        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        // Horizontal rule between mode settings and depart settings
        GfxFillRectInset(
            dpi,
            { windowPos + ScreenCoordsXY{ window_ride_operating_widgets[WIDX_PAGE_BACKGROUND].left + 4, 103 },
              windowPos + ScreenCoordsXY{ window_ride_operating_widgets[WIDX_PAGE_BACKGROUND].right - 5, 104 } },
            colours[1], INSET_RECT_FLAG_BORDER_INSET);

        // Number of block sections
        if (ride->IsBlockSectioned())
        {
            auto ft = Formatter();
            ft.Add<uint16_t>(ride->num_block_brakes + ride->num_stations);
            DrawTextBasic(
                dpi, windowPos + ScreenCoordsXY{ 21, ride->mode == RideMode::PoweredLaunchBlockSectioned ? 89 : 61 },
                STR_BLOCK_SECTIONS, ft, COLOUR_BLACK);
        }
    }

#pragma endregion

#pragma region Maintenance

    void LocateMechanic()
    {
        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        // First check if there is a mechanic assigned
        Peep* mechanic = RideGetAssignedMechanic(*ride);

        // Otherwise find the closest mechanic
        if (mechanic == nullptr)
            mechanic = RideFindClosestMechanic(*ride, 1);

        if (mechanic == nullptr)
            ContextShowError(STR_UNABLE_TO_LOCATE_MECHANIC, STR_NONE, {});
        else
        {
            auto intent = Intent(WindowClass::Peep);
            intent.PutExtra(INTENT_EXTRA_PEEP, mechanic);
            ContextOpenIntent(&intent);
        }
    }

    void MaintenanceDrawBar(DrawPixelInfo& dpi, const ScreenCoordsXY& coords, int32_t value, int32_t colour) const
    {
        GfxFillRectInset(dpi, { coords, coords + ScreenCoordsXY{ 149, 8 } }, colours[1], INSET_RECT_F_30);
        if (colour & BAR_BLINK)
        {
            colour &= ~BAR_BLINK;
            if (GameIsNotPaused() && (gCurrentRealTimeTicks & 8))
                return;
        }

        value = ((186 * ((value * 2) & 0xFF)) >> 8) & 0xFF;
        if (value > 2)
        {
            GfxFillRectInset(dpi, { coords + ScreenCoordsXY{ 2, 1 }, coords + ScreenCoordsXY{ value + 1, 7 } }, colour, 0);
        }
    }

    void MaintenanceOnMouseUp(WidgetIndex widgetIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                Close();
                return;
            case WIDX_TAB_1:
            case WIDX_TAB_2:
            case WIDX_TAB_3:
            case WIDX_TAB_4:
            case WIDX_TAB_5:
            case WIDX_TAB_6:
            case WIDX_TAB_7:
            case WIDX_TAB_8:
            case WIDX_TAB_9:
            case WIDX_TAB_10:
                SetPage(widgetIndex - WIDX_TAB_1);
                break;
            case WIDX_LOCATE_MECHANIC:
                LocateMechanic();
                break;
            case WIDX_REFURBISH_RIDE:
                ContextOpenDetailWindow(WD_REFURBISH_RIDE, number);
                break;
        }
    }

    void MaintenanceResize()
    {
        WindowSetResize(*this, 316, 135, 316, 135);
    }

    void MaintenanceOnMouseDown(WidgetIndex widgetIndex)
    {
        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        auto rideEntry = ride->GetRideEntry();
        if (rideEntry == nullptr)
            return;

        Widget* dropdownWidget = &widgets[widgetIndex];
        int32_t j, num_items;

        switch (widgetIndex)
        {
            case WIDX_INSPECTION_INTERVAL_DROPDOWN:
                dropdownWidget--;
                for (int32_t i = 0; i < 7; i++)
                {
                    gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
                    gDropdownItems[i].Args = RideInspectionIntervalNames[i];
                }
                WindowDropdownShowTextCustomWidth(
                    { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1,
                    colours[1], 0, Dropdown::Flag::StayOpen, 7, widgets[widgetIndex].right - dropdownWidget->left);

                Dropdown::SetChecked(ride->inspection_interval, true);
                break;

            case WIDX_FORCE_BREAKDOWN:
                num_items = 1;
                for (j = 0; j < RCT2::ObjectLimits::MaxRideTypesPerRideEntry; j++)
                {
                    if (rideEntry->ride_type[j] != RIDE_TYPE_NULL)
                        break;
                }
                gDropdownItems[0].Format = STR_DROPDOWN_MENU_LABEL;
                gDropdownItems[0].Args = STR_DEBUG_FIX_RIDE;
                for (int32_t i = 0; i < 8; i++)
                {
                    assert(j < static_cast<int32_t>(std::size(rideEntry->ride_type)));
                    if (GetRideTypeDescriptor(rideEntry->ride_type[j]).AvailableBreakdowns & static_cast<uint8_t>(1 << i))
                    {
                        if (i == BREAKDOWN_BRAKES_FAILURE && ride->IsBlockSectioned())
                        {
                            if (ride->NumTrains != 1)
                                continue;
                        }
                        gDropdownItems[num_items].Format = STR_DROPDOWN_MENU_LABEL;
                        gDropdownItems[num_items].Args = RideBreakdownReasonNames[i];
                        num_items++;
                    }
                }
                if (num_items == 1)
                {
                    ContextShowError(STR_DEBUG_NO_BREAKDOWNS_AVAILABLE, STR_NONE, {});
                }
                else
                {
                    WindowDropdownShowText(
                        { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1,
                        colours[1], Dropdown::Flag::StayOpen, num_items);

                    num_items = 1;
                    int32_t breakdownReason = ride->breakdown_reason_pending;
                    if (breakdownReason != BREAKDOWN_NONE && (ride->lifecycle_flags & RIDE_LIFECYCLE_BREAKDOWN_PENDING))
                    {
                        for (int32_t i = 0; i < 8; i++)
                        {
                            if (GetRideTypeDescriptor(rideEntry->ride_type[j]).AvailableBreakdowns
                                & static_cast<uint8_t>(1 << i))
                            {
                                if (i == BREAKDOWN_BRAKES_FAILURE && ride->IsBlockSectioned())
                                {
                                    if (ride->NumTrains != 1)
                                        continue;
                                }
                                if (i == breakdownReason)
                                {
                                    Dropdown::SetChecked(num_items, true);
                                    break;
                                }
                                gDropdownItems[num_items].Format = STR_DROPDOWN_MENU_LABEL;
                                gDropdownItems[num_items].Args = RideBreakdownReasonNames[i];
                                num_items++;
                            }
                        }
                    }

                    if ((ride->lifecycle_flags & RIDE_LIFECYCLE_BREAKDOWN_PENDING) == 0)
                    {
                        Dropdown::SetDisabled(0, true);
                    }
                }
                break;
        }
    }

    void MaintenanceOnDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
    {
        if (dropdownIndex == -1)
            return;

        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        auto rideEntry = ride->GetRideEntry();
        if (rideEntry == nullptr)
            return;

        switch (widgetIndex)
        {
            case WIDX_INSPECTION_INTERVAL_DROPDOWN:
                SetOperatingSetting(rideId, RideSetSetting::InspectionInterval, dropdownIndex);
                break;

            case WIDX_FORCE_BREAKDOWN:
                if (dropdownIndex == 0)
                {
                    Vehicle* vehicle;
                    switch (ride->breakdown_reason_pending)
                    {
                        case BREAKDOWN_SAFETY_CUT_OUT:
                            if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK))
                                break;
                            for (int32_t i = 0; i < ride->NumTrains; ++i)
                            {
                                for (vehicle = GetEntity<Vehicle>(ride->vehicles[i]); vehicle != nullptr;
                                     vehicle = GetEntity<Vehicle>(vehicle->next_vehicle_on_train))
                                {
                                    vehicle->ClearFlag(
                                        VehicleFlags::CarIsBroken | VehicleFlags::StoppedOnLift | VehicleFlags::TrainIsBroken);
                                }
                            }
                            break;
                        case BREAKDOWN_RESTRAINTS_STUCK_CLOSED:
                        case BREAKDOWN_RESTRAINTS_STUCK_OPEN:
                        case BREAKDOWN_DOORS_STUCK_CLOSED:
                        case BREAKDOWN_DOORS_STUCK_OPEN:
                            vehicle = GetEntity<Vehicle>(ride->vehicles[ride->broken_vehicle]);
                            if (vehicle != nullptr)
                            {
                                vehicle->ClearFlag(VehicleFlags::CarIsBroken);
                            }
                            break;
                        case BREAKDOWN_VEHICLE_MALFUNCTION:
                            vehicle = GetEntity<Vehicle>(ride->vehicles[ride->broken_vehicle]);
                            if (vehicle != nullptr)
                            {
                                vehicle->ClearFlag(VehicleFlags::TrainIsBroken);
                            }
                            break;
                    }
                    ride->lifecycle_flags &= ~(RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN);
                    WindowInvalidateByNumber(WindowClass::Ride, number);
                    break;
                }
                if (ride->lifecycle_flags
                    & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED))
                {
                    ContextShowError(STR_DEBUG_CANT_FORCE_BREAKDOWN, STR_DEBUG_RIDE_ALREADY_BROKEN, {});
                }
                else if (ride->status == RideStatus::Closed)
                {
                    ContextShowError(STR_DEBUG_CANT_FORCE_BREAKDOWN, STR_DEBUG_RIDE_IS_CLOSED, {});
                }
                else
                {
                    int32_t j;
                    for (j = 0; j < RCT2::ObjectLimits::MaxRideTypesPerRideEntry; j++)
                    {
                        if (rideEntry->ride_type[j] != RIDE_TYPE_NULL)
                            break;
                    }
                    int32_t i;
                    int32_t num_items = 1;
                    for (i = 0; i < BREAKDOWN_COUNT; i++)
                    {
                        assert(j < static_cast<int32_t>(std::size(rideEntry->ride_type)));
                        if (GetRideTypeDescriptor(rideEntry->ride_type[j]).AvailableBreakdowns & static_cast<uint8_t>(1 << i))
                        {
                            if (i == BREAKDOWN_BRAKES_FAILURE && ride->IsBlockSectioned())
                            {
                                if (ride->NumTrains != 1)
                                    continue;
                            }
                            if (num_items == dropdownIndex)
                                break;
                            num_items++;
                        }
                    }
                    RidePrepareBreakdown(*ride, i);
                }
                break;
        }
    }

    void MaintenanceUpdate()
    {
        frame_no++;
        OnPrepareDraw();
        WidgetInvalidate(*this, WIDX_TAB_4);

        auto ride = GetRide(rideId);
        if (ride != nullptr && ride->window_invalidate_flags & RIDE_INVALIDATE_RIDE_MAINTENANCE)
        {
            ride->window_invalidate_flags &= ~RIDE_INVALIDATE_RIDE_MAINTENANCE;
            Invalidate();
        }
    }

    void MaintenanceOnPrepareDraw()
    {
        auto newWidgets = window_ride_page_widgets[page];
        if (widgets != newWidgets)
        {
            widgets = newWidgets;
            InitScrollWidgets();
        }

        SetPressedTab();

        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        auto ft = Formatter::Common();
        ride->FormatNameTo(ft);

        window_ride_maintenance_widgets[WIDX_INSPECTION_INTERVAL].text = RideInspectionIntervalNames[ride->inspection_interval];

        AnchorBorderWidgets();
        WindowAlignTabs(this, WIDX_TAB_1, WIDX_TAB_10);

        if (gConfigGeneral.DebuggingTools && NetworkGetMode() == NETWORK_MODE_NONE)
        {
            window_ride_maintenance_widgets[WIDX_FORCE_BREAKDOWN].type = WindowWidgetType::FlatBtn;
        }
        else
        {
            window_ride_maintenance_widgets[WIDX_FORCE_BREAKDOWN].type = WindowWidgetType::Empty;
        }

        if (ride->GetRideTypeDescriptor().AvailableBreakdowns == 0
            || !(ride->lifecycle_flags & RIDE_LIFECYCLE_EVER_BEEN_OPENED))
        {
            disabled_widgets |= (1uLL << WIDX_REFURBISH_RIDE);
            window_ride_maintenance_widgets[WIDX_REFURBISH_RIDE].tooltip = STR_CANT_REFURBISH_NOT_NEEDED;
        }
        else
        {
            disabled_widgets &= ~(1uLL << WIDX_REFURBISH_RIDE);
            window_ride_maintenance_widgets[WIDX_REFURBISH_RIDE].tooltip = STR_REFURBISH_RIDE_TIP;
        }
    }

    void MaintenanceOnDraw(DrawPixelInfo& dpi)
    {
        DrawWidgets(dpi);
        DrawTabImages(dpi);

        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        // Locate mechanic button image
        Widget* widget = &window_ride_maintenance_widgets[WIDX_LOCATE_MECHANIC];
        auto screenCoords = windowPos + ScreenCoordsXY{ widget->left, widget->top };
        auto image = ImageId(SPR_MECHANIC, COLOUR_BLACK, gStaffMechanicColour);
        GfxDrawSprite(dpi, image, screenCoords);

        // Inspection label
        widget = &window_ride_maintenance_widgets[WIDX_INSPECTION_INTERVAL];
        screenCoords = windowPos + ScreenCoordsXY{ 4, widget->top + 1 };
        DrawTextBasic(dpi, screenCoords, STR_INSPECTION);

        // Reliability
        widget = &window_ride_maintenance_widgets[WIDX_PAGE_BACKGROUND];
        screenCoords = windowPos + ScreenCoordsXY{ widget->left + 4, widget->top + 4 };

        uint16_t reliability = ride->reliability_percentage;
        auto ft = Formatter();
        ft.Add<uint16_t>(reliability);
        DrawTextBasic(dpi, screenCoords, STR_RELIABILITY_LABEL_1757, ft);
        MaintenanceDrawBar(
            dpi, screenCoords + ScreenCoordsXY{ 103, 0 }, std::max<int32_t>(10, reliability), COLOUR_BRIGHT_GREEN);
        screenCoords.y += 11;

        uint16_t downTime = ride->downtime;
        ft = Formatter();
        ft.Add<uint16_t>(downTime);
        DrawTextBasic(dpi, screenCoords, STR_DOWN_TIME_LABEL_1889, ft);
        MaintenanceDrawBar(dpi, screenCoords + ScreenCoordsXY{ 103, 0 }, downTime, COLOUR_BRIGHT_RED);
        screenCoords.y += 26;

        // Last inspection
        StringId stringId;
        if (ride->last_inspection <= 1)
            stringId = STR_TIME_SINCE_LAST_INSPECTION_MINUTE;
        else if (ride->last_inspection <= 240)
            stringId = STR_TIME_SINCE_LAST_INSPECTION_MINUTES;
        else
            stringId = STR_TIME_SINCE_LAST_INSPECTION_MORE_THAN_4_HOURS;

        ft = Formatter();
        ft.Add<uint16_t>(ride->last_inspection);
        DrawTextBasic(dpi, screenCoords, stringId, ft);
        screenCoords.y += 12;

        // Last / current breakdown
        if (ride->breakdown_reason == BREAKDOWN_NONE)
            return;

        stringId = (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN) ? STR_CURRENT_BREAKDOWN : STR_LAST_BREAKDOWN;
        ft = Formatter();
        ft.Add<StringId>(RideBreakdownReasonNames[ride->breakdown_reason]);
        DrawTextBasic(dpi, screenCoords, stringId, ft);
        screenCoords.y += 12;

        // Mechanic status
        if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
        {
            switch (ride->mechanic_status)
            {
                case RIDE_MECHANIC_STATUS_CALLING:
                {
                    stringId = STR_NO_MECHANICS_ARE_HIRED_MESSAGE;

                    for (auto peep : EntityList<Staff>())
                    {
                        if (peep->IsMechanic())
                        {
                            stringId = STR_CALLING_MECHANIC;
                            break;
                        }
                    }
                    break;
                }
                case RIDE_MECHANIC_STATUS_HEADING:
                    stringId = STR_MEHCANIC_IS_HEADING_FOR_THE_RIDE;
                    break;
                case RIDE_MECHANIC_STATUS_FIXING:
                case RIDE_MECHANIC_STATUS_HAS_FIXED_STATION_BRAKES:
                    stringId = STR_MEHCANIC_IS_FIXING_THE_RIDE;
                    break;
                default:
                    stringId = STR_EMPTY;
                    break;
            }

            if (stringId != STR_EMPTY)
            {
                if (stringId == STR_CALLING_MECHANIC || stringId == STR_NO_MECHANICS_ARE_HIRED_MESSAGE)
                {
                    DrawTextWrapped(dpi, screenCoords, 280, stringId, {}, { TextAlignment::LEFT });
                }
                else
                {
                    auto staff = GetEntity<Staff>(ride->mechanic);
                    if (staff != nullptr && staff->IsMechanic())
                    {
                        ft = Formatter();
                        staff->FormatNameTo(ft);
                        DrawTextWrapped(dpi, screenCoords, 280, stringId, ft, { TextAlignment::LEFT });
                    }
                }
            }
        }
    }

#pragma endregion

#pragma region Colour

    int32_t HasTrackColour(const Ride& ride, int32_t trackColour)
    {
        // Get station flags (shops don't have them)
        auto stationObjFlags = 0;
        if (!ride.GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_IS_SHOP_OR_FACILITY))
        {
            auto stationObj = ride.GetStationObject();
            if (stationObj != nullptr)
            {
                stationObjFlags = stationObj->Flags;
            }
        }

        switch (trackColour)
        {
            case 0:
                return (stationObjFlags & STATION_OBJECT_FLAGS::HAS_PRIMARY_COLOUR)
                    || ride.GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_MAIN);
            case 1:
                return (stationObjFlags & STATION_OBJECT_FLAGS::HAS_SECONDARY_COLOUR)
                    || ride.GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_ADDITIONAL);
            case 2:
                return ride.GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_SUPPORTS);
            default:
                return 0;
        }
    }

    void SetTrackColourScheme(const ScreenCoordsXY& screenPos)
    {
        auto newColourScheme = static_cast<uint8_t>(ride_colour);
        auto info = GetMapCoordinatesFromPos(screenPos, EnumsToFlags(ViewportInteractionItem::Ride));

        if (info.SpriteType != ViewportInteractionItem::Ride)
            return;
        if (info.Element->GetType() != TileElementType::Track)
            return;
        if (info.Element->AsTrack()->GetRideIndex() != rideId)
            return;
        if (info.Element->AsTrack()->GetColourScheme() == newColourScheme)
            return;

        auto z = info.Element->GetBaseZ();
        auto direction = info.Element->GetDirection();
        auto gameAction = RideSetColourSchemeAction(
            CoordsXYZD{ info.Loc, z, static_cast<Direction>(direction) }, info.Element->AsTrack()->GetTrackType(),
            newColourScheme);
        GameActions::Execute(&gameAction);
    }

    void ColourClose()
    {
        if (!(InputTestFlag(INPUT_FLAG_TOOL_ACTIVE)))
            return;

        if (gCurrentToolWidget.window_classification != classification)
            return;

        if (gCurrentToolWidget.window_number != number)
            return;

        ToolCancel();
    }

    void ColourOnMouseUp(WidgetIndex widgetIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                Close();
                return;
            case WIDX_TAB_1:
            case WIDX_TAB_2:
            case WIDX_TAB_3:
            case WIDX_TAB_4:
            case WIDX_TAB_5:
            case WIDX_TAB_6:
            case WIDX_TAB_7:
            case WIDX_TAB_8:
            case WIDX_TAB_9:
            case WIDX_TAB_10:
                SetPage(widgetIndex - WIDX_TAB_1);
                break;
            case WIDX_PAINT_INDIVIDUAL_AREA:
                ToolSet(*this, WIDX_PAINT_INDIVIDUAL_AREA, Tool::PaintDown);
                break;
            case WIDX_SELL_ITEM_RANDOM_COLOUR_CHECKBOX:
                auto ride = GetRide(rideId);
                if (ride != nullptr)
                {
                    const bool currentlyEnabled = ride->HasLifecycleFlag(RIDE_LIFECYCLE_RANDOM_SHOP_COLOURS);
                    auto rideSetAppearanceAction = RideSetAppearanceAction(
                        rideId, RideSetAppearanceType::SellingItemColourIsRandom, currentlyEnabled ? 0 : 1, 0);
                    GameActions::Execute(&rideSetAppearanceAction);
                }
                break;
        }
    }

    void ColourResize()
    {
        WindowSetResize(*this, 316, 207, 316, 207);
    }

    void ColourOnMouseDown(WidgetIndex widgetIndex)
    {
        VehicleColour vehicleColour;
        int32_t i, numItems;
        StringId stringId;

        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        auto rideEntry = ride->GetRideEntry();
        if (rideEntry == nullptr)
            return;

        auto colourSchemeIndex = ride_colour;
        auto dropdownWidget = &widgets[widgetIndex] - 1;

        switch (widgetIndex)
        {
            case WIDX_TRACK_COLOUR_SCHEME_DROPDOWN:
                for (i = 0; i < OpenRCT2::Limits::NumColourSchemes; i++)
                {
                    gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
                    gDropdownItems[i].Args = ColourSchemeNames[i];
                }

                WindowDropdownShowTextCustomWidth(
                    { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1,
                    colours[1], 0, Dropdown::Flag::StayOpen, 4, widgets[widgetIndex].right - dropdownWidget->left);

                Dropdown::SetChecked(colourSchemeIndex, true);
                break;
            case WIDX_TRACK_MAIN_COLOUR:
                WindowDropdownShowColour(this, &widgets[widgetIndex], colours[1], ride->track_colour[colourSchemeIndex].main);
                break;
            case WIDX_TRACK_ADDITIONAL_COLOUR:
                WindowDropdownShowColour(
                    this, &widgets[widgetIndex], colours[1], ride->track_colour[colourSchemeIndex].additional);
                break;
            case WIDX_TRACK_SUPPORT_COLOUR:
                WindowDropdownShowColour(
                    this, &widgets[widgetIndex], colours[1], ride->track_colour[colourSchemeIndex].supports);
                break;
            case WIDX_MAZE_STYLE_DROPDOWN:
                for (i = 0; i < 4; i++)
                {
                    gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
                    gDropdownItems[i].Args = MazeOptions[i].text;
                }

                WindowDropdownShowTextCustomWidth(
                    { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1,
                    colours[1], 0, Dropdown::Flag::StayOpen, 4, widgets[widgetIndex].right - dropdownWidget->left);

                Dropdown::SetChecked(ride->track_colour[colourSchemeIndex].supports, true);
                break;
            case WIDX_ENTRANCE_STYLE_DROPDOWN:
            {
                auto ddIndex = 0;
                auto& objManager = GetContext()->GetObjectManager();
                for (i = 0; i < MAX_STATION_OBJECTS; i++)
                {
                    auto stationObj = static_cast<StationObject*>(objManager.GetLoadedObject(ObjectType::Station, i));
                    if (stationObj != nullptr)
                    {
                        gDropdownItems[ddIndex].Format = STR_DROPDOWN_MENU_LABEL;
                        gDropdownItems[ddIndex].Args = stationObj->NameStringId;
                        if (ride->entrance_style == i)
                        {
                            gDropdownItems[ddIndex].Format = STR_DROPDOWN_MENU_LABEL_SELECTED;
                        }
                        ddIndex++;
                    }
                }

                WindowDropdownShowTextCustomWidth(
                    { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1,
                    colours[1], 0, Dropdown::Flag::StayOpen, ddIndex, widgets[widgetIndex].right - dropdownWidget->left);
                break;
            }
            case WIDX_VEHICLE_COLOUR_SCHEME_DROPDOWN:
                for (i = 0; i < 3; i++)
                {
                    gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
                    gDropdownItems[i].Args = (GetRideComponentName(ride->GetRideTypeDescriptor().NameConvention.vehicle)
                                                  .singular
                                              << 16)
                        | VehicleColourSchemeNames[i];
                }

                WindowDropdownShowTextCustomWidth(
                    { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1,
                    colours[1], 0, Dropdown::Flag::StayOpen, rideEntry->max_cars_in_train > 1 ? 3 : 2,
                    widgets[widgetIndex].right - dropdownWidget->left);

                Dropdown::SetChecked(ride->colour_scheme_type & 3, true);
                break;
            case WIDX_VEHICLE_COLOUR_INDEX_DROPDOWN:
                numItems = ride->NumTrains;
                if ((ride->colour_scheme_type & 3) != VEHICLE_COLOUR_SCHEME_PER_TRAIN)
                    numItems = ride->num_cars_per_train;

                stringId = (ride->colour_scheme_type & 3) == VEHICLE_COLOUR_SCHEME_PER_TRAIN ? STR_RIDE_COLOUR_TRAIN_OPTION
                                                                                             : STR_RIDE_COLOUR_VEHICLE_OPTION;
                for (i = 0; i < std::min(numItems, Dropdown::ItemsMaxSize); i++)
                {
                    gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
                    gDropdownItems[i].Args = (static_cast<int64_t>(i + 1) << 32)
                        | ((GetRideComponentName(ride->GetRideTypeDescriptor().NameConvention.vehicle).capitalised) << 16)
                        | stringId;
                }

                WindowDropdownShowTextCustomWidth(
                    { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1,
                    colours[1], 0, Dropdown::Flag::StayOpen, numItems, widgets[widgetIndex].right - dropdownWidget->left);

                Dropdown::SetChecked(vehicleIndex, true);
                break;
            case WIDX_VEHICLE_BODY_COLOUR:
                vehicleColour = RideGetVehicleColour(*ride, vehicleIndex);
                WindowDropdownShowColour(this, &widgets[widgetIndex], colours[1], vehicleColour.Body);
                break;
            case WIDX_VEHICLE_TRIM_COLOUR:
                vehicleColour = RideGetVehicleColour(*ride, vehicleIndex);
                WindowDropdownShowColour(this, &widgets[widgetIndex], colours[1], vehicleColour.Trim);
                break;
            case WIDX_VEHICLE_TERNARY_COLOUR:
                vehicleColour = RideGetVehicleColour(*ride, vehicleIndex);
                WindowDropdownShowColour(this, &widgets[widgetIndex], colours[1], vehicleColour.Tertiary);
                break;
        }
    }

    void ColourOnDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
    {
        if (dropdownIndex == -1)
            return;

        switch (widgetIndex)
        {
            case WIDX_TRACK_COLOUR_SCHEME_DROPDOWN:
                ride_colour = static_cast<uint16_t>(dropdownIndex);
                Invalidate();
                break;
            case WIDX_TRACK_MAIN_COLOUR:
            {
                auto rideSetAppearanceAction = RideSetAppearanceAction(
                    rideId, RideSetAppearanceType::TrackColourMain, ColourDropDownIndexToColour(dropdownIndex), ride_colour);
                GameActions::Execute(&rideSetAppearanceAction);
            }
            break;
            case WIDX_TRACK_ADDITIONAL_COLOUR:
            {
                auto rideSetAppearanceAction = RideSetAppearanceAction(
                    rideId, RideSetAppearanceType::TrackColourAdditional, ColourDropDownIndexToColour(dropdownIndex),
                    ride_colour);
                GameActions::Execute(&rideSetAppearanceAction);
            }
            break;
            case WIDX_TRACK_SUPPORT_COLOUR:
            {
                auto rideSetAppearanceAction = RideSetAppearanceAction(
                    rideId, RideSetAppearanceType::TrackColourSupports, ColourDropDownIndexToColour(dropdownIndex),
                    ride_colour);
                GameActions::Execute(&rideSetAppearanceAction);
            }
            break;
            case WIDX_MAZE_STYLE_DROPDOWN:
            {
                auto rideSetAppearanceAction = RideSetAppearanceAction(
                    rideId, RideSetAppearanceType::MazeStyle, dropdownIndex, ride_colour);
                GameActions::Execute(&rideSetAppearanceAction);
            }
            break;
            case WIDX_ENTRANCE_STYLE_DROPDOWN:
            {
                auto ddIndex = 0;
                auto& objManager = GetContext()->GetObjectManager();
                for (auto i = 0; i < MAX_STATION_OBJECTS; i++)
                {
                    auto stationObj = static_cast<StationObject*>(objManager.GetLoadedObject(ObjectType::Station, i));
                    if (stationObj != nullptr)
                    {
                        if (ddIndex == dropdownIndex)
                        {
                            auto rideSetAppearanceAction = RideSetAppearanceAction(
                                rideId, RideSetAppearanceType::EntranceStyle, ddIndex, 0);
                            rideSetAppearanceAction.SetCallback([ddIndex](const GameAction*, const GameActions::Result* res) {
                                if (res->Error != GameActions::Status::Ok)
                                    return;
                                gLastEntranceStyle = ddIndex;
                            });
                            GameActions::Execute(&rideSetAppearanceAction);
                            break;
                        }
                        ddIndex++;
                    }
                }
                break;
            }
            case WIDX_VEHICLE_COLOUR_SCHEME_DROPDOWN:
            {
                auto rideSetAppearanceAction = RideSetAppearanceAction(
                    rideId, RideSetAppearanceType::VehicleColourScheme, dropdownIndex, 0);
                GameActions::Execute(&rideSetAppearanceAction);
                vehicleIndex = 0;
            }
            break;
            case WIDX_VEHICLE_COLOUR_INDEX_DROPDOWN:
                vehicleIndex = dropdownIndex;
                Invalidate();
                break;
            case WIDX_VEHICLE_BODY_COLOUR:
            {
                auto rideSetAppearanceAction = RideSetAppearanceAction(
                    rideId, RideSetAppearanceType::VehicleColourBody, ColourDropDownIndexToColour(dropdownIndex), vehicleIndex);
                GameActions::Execute(&rideSetAppearanceAction);
            }
            break;
            case WIDX_VEHICLE_TRIM_COLOUR:
            {
                auto rideSetAppearanceAction = RideSetAppearanceAction(
                    rideId, RideSetAppearanceType::VehicleColourTrim, ColourDropDownIndexToColour(dropdownIndex), vehicleIndex);
                GameActions::Execute(&rideSetAppearanceAction);
            }
            break;
            case WIDX_VEHICLE_TERNARY_COLOUR:
            {
                auto rideSetAppearanceAction = RideSetAppearanceAction(
                    rideId, RideSetAppearanceType::VehicleColourTernary, ColourDropDownIndexToColour(dropdownIndex),
                    vehicleIndex);
                GameActions::Execute(&rideSetAppearanceAction);
            }
            break;
        }
    }

    void ColourUpdate()
    {
        frame_no++;
        OnPrepareDraw();
        WidgetInvalidate(*this, WIDX_TAB_5);
        WidgetInvalidate(*this, WIDX_VEHICLE_PREVIEW);
    }

    void ColourOnToolDown(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
    {
        if (widgetIndex == WIDX_PAINT_INDIVIDUAL_AREA)
            SetTrackColourScheme(screenCoords);
    }

    void ColourOnToolDrag(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
    {
        if (widgetIndex == WIDX_PAINT_INDIVIDUAL_AREA)
            SetTrackColourScheme(screenCoords);
    }

    void ColourOnPrepareDraw()
    {
        TrackColour trackColour;
        VehicleColour vehicleColour;

        auto newWidgets = window_ride_page_widgets[page];
        if (widgets != newWidgets)
        {
            widgets = newWidgets;
            InitScrollWidgets();
        }

        SetPressedTab();

        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        auto rideEntry = ride->GetRideEntry();
        if (rideEntry == nullptr)
            return;

        widgets[WIDX_TITLE].text = STR_ARG_16_STRINGID;
        auto ft = Formatter::Common();
        ft.Increment(16);
        ride->FormatNameTo(ft);

        // Track colours
        int32_t colourScheme = ride_colour;
        trackColour = ride->track_colour[colourScheme];

        // Maze style
        const auto& rtd = ride->GetRideTypeDescriptor();
        if (rtd.HasFlag(RIDE_TYPE_FLAG_IS_MAZE))
        {
            window_ride_colour_widgets[WIDX_MAZE_STYLE].type = WindowWidgetType::DropdownMenu;
            window_ride_colour_widgets[WIDX_MAZE_STYLE_DROPDOWN].type = WindowWidgetType::Button;
            window_ride_colour_widgets[WIDX_MAZE_STYLE].text = MazeOptions[trackColour.supports].text;
        }
        else
        {
            window_ride_colour_widgets[WIDX_MAZE_STYLE].type = WindowWidgetType::Empty;
            window_ride_colour_widgets[WIDX_MAZE_STYLE_DROPDOWN].type = WindowWidgetType::Empty;
        }

        // Track, multiple colour schemes
        if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_SUPPORTS_MULTIPLE_TRACK_COLOUR))
        {
            window_ride_colour_widgets[WIDX_TRACK_COLOUR_SCHEME].type = WindowWidgetType::DropdownMenu;
            window_ride_colour_widgets[WIDX_TRACK_COLOUR_SCHEME_DROPDOWN].type = WindowWidgetType::Button;
            window_ride_colour_widgets[WIDX_PAINT_INDIVIDUAL_AREA].type = WindowWidgetType::FlatBtn;
        }
        else
        {
            window_ride_colour_widgets[WIDX_TRACK_COLOUR_SCHEME].type = WindowWidgetType::Empty;
            window_ride_colour_widgets[WIDX_TRACK_COLOUR_SCHEME_DROPDOWN].type = WindowWidgetType::Empty;
            window_ride_colour_widgets[WIDX_PAINT_INDIVIDUAL_AREA].type = WindowWidgetType::Empty;
        }

        // Track main colour
        if (HasTrackColour(*ride, 0))
        {
            window_ride_colour_widgets[WIDX_TRACK_MAIN_COLOUR].type = WindowWidgetType::ColourBtn;
            window_ride_colour_widgets[WIDX_TRACK_MAIN_COLOUR].image = GetColourButtonImage(trackColour.main);
        }
        else
        {
            window_ride_colour_widgets[WIDX_TRACK_MAIN_COLOUR].type = WindowWidgetType::Empty;
        }

        // Track additional colour
        if (HasTrackColour(*ride, 1))
        {
            window_ride_colour_widgets[WIDX_TRACK_ADDITIONAL_COLOUR].type = WindowWidgetType::ColourBtn;
            window_ride_colour_widgets[WIDX_TRACK_ADDITIONAL_COLOUR].image = GetColourButtonImage(trackColour.additional);
        }
        else
        {
            window_ride_colour_widgets[WIDX_TRACK_ADDITIONAL_COLOUR].type = WindowWidgetType::Empty;
        }

        // Selling item random colour checkbox
        if (ride->HasRecolourableShopItems())
        {
            window_ride_colour_widgets[WIDX_SELL_ITEM_RANDOM_COLOUR_CHECKBOX].type = WindowWidgetType::Checkbox;
            if (ride->HasLifecycleFlag(RIDE_LIFECYCLE_RANDOM_SHOP_COLOURS))
            {
                pressed_widgets |= (1uLL << WIDX_SELL_ITEM_RANDOM_COLOUR_CHECKBOX);
            }
            else
            {
                pressed_widgets &= ~(1uLL << WIDX_SELL_ITEM_RANDOM_COLOUR_CHECKBOX);
            }
        }
        else
        {
            window_ride_colour_widgets[WIDX_SELL_ITEM_RANDOM_COLOUR_CHECKBOX].type = WindowWidgetType::Empty;
        }

        // Track supports colour
        if (HasTrackColour(*ride, 2) && !rtd.HasFlag(RIDE_TYPE_FLAG_IS_MAZE))
        {
            window_ride_colour_widgets[WIDX_TRACK_SUPPORT_COLOUR].type = WindowWidgetType::ColourBtn;
            window_ride_colour_widgets[WIDX_TRACK_SUPPORT_COLOUR].image = GetColourButtonImage(trackColour.supports);
        }
        else
        {
            window_ride_colour_widgets[WIDX_TRACK_SUPPORT_COLOUR].type = WindowWidgetType::Empty;
        }

        // Track preview
        if (ride->GetRideTypeDescriptor().HasFlag(
                RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_MAIN | RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_ADDITIONAL
                | RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_SUPPORTS))
            window_ride_colour_widgets[WIDX_TRACK_PREVIEW].type = WindowWidgetType::Spinner;
        else
            window_ride_colour_widgets[WIDX_TRACK_PREVIEW].type = WindowWidgetType::Empty;

        // Entrance style
        if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT))
        {
            window_ride_colour_widgets[WIDX_ENTRANCE_PREVIEW].type = WindowWidgetType::Spinner;
            window_ride_colour_widgets[WIDX_ENTRANCE_STYLE].type = WindowWidgetType::DropdownMenu;
            window_ride_colour_widgets[WIDX_ENTRANCE_STYLE_DROPDOWN].type = WindowWidgetType::Button;

            auto stringId = STR_NONE;
            auto stationObj = ride->GetStationObject();
            if (stationObj != nullptr)
            {
                stringId = stationObj->NameStringId;
            }
            window_ride_colour_widgets[WIDX_ENTRANCE_STYLE].text = stringId;
        }
        else
        {
            window_ride_colour_widgets[WIDX_ENTRANCE_PREVIEW].type = WindowWidgetType::Empty;
            window_ride_colour_widgets[WIDX_ENTRANCE_STYLE].type = WindowWidgetType::Empty;
            window_ride_colour_widgets[WIDX_ENTRANCE_STYLE_DROPDOWN].type = WindowWidgetType::Empty;
        }

        // Vehicle colours
        if (!ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_NO_VEHICLES)
            && ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_VEHICLE_COLOURS))
        {
            int32_t vehicleColourSchemeType = ride->colour_scheme_type & 3;
            if (vehicleColourSchemeType == 0)
                vehicleIndex = 0;

            vehicleColour = RideGetVehicleColour(*ride, vehicleIndex);

            window_ride_colour_widgets[WIDX_VEHICLE_PREVIEW].type = WindowWidgetType::Scroll;
            window_ride_colour_widgets[WIDX_VEHICLE_BODY_COLOUR].type = WindowWidgetType::ColourBtn;
            window_ride_colour_widgets[WIDX_VEHICLE_BODY_COLOUR].image = GetColourButtonImage(vehicleColour.Body);

            bool allowChangingTrimColour = false;
            bool allowChangingTernaryColour = false;

            for (int32_t i = 0; i < ride->num_cars_per_train; i++)
            {
                uint8_t vehicleTypeIndex = RideEntryGetVehicleAtPosition(ride->subtype, ride->num_cars_per_train, i);

                if (rideEntry->Cars[vehicleTypeIndex].flags & CAR_ENTRY_FLAG_ENABLE_TRIM_COLOUR)
                {
                    allowChangingTrimColour = true;
                }
                if (rideEntry->Cars[vehicleTypeIndex].flags & CAR_ENTRY_FLAG_ENABLE_TERNARY_COLOUR)
                {
                    allowChangingTernaryColour = true;
                }
            }

            // Additional colours
            if (allowChangingTrimColour)
            {
                window_ride_colour_widgets[WIDX_VEHICLE_TRIM_COLOUR].type = WindowWidgetType::ColourBtn;
                window_ride_colour_widgets[WIDX_VEHICLE_TRIM_COLOUR].image = GetColourButtonImage(vehicleColour.Trim);
                if (allowChangingTernaryColour)
                {
                    window_ride_colour_widgets[WIDX_VEHICLE_TERNARY_COLOUR].type = WindowWidgetType::ColourBtn;
                    window_ride_colour_widgets[WIDX_VEHICLE_TERNARY_COLOUR].image = GetColourButtonImage(
                        vehicleColour.Tertiary);
                }
                else
                {
                    window_ride_colour_widgets[WIDX_VEHICLE_TERNARY_COLOUR].type = WindowWidgetType::Empty;
                }
            }
            else
            {
                window_ride_colour_widgets[WIDX_VEHICLE_TRIM_COLOUR].type = WindowWidgetType::Empty;
                window_ride_colour_widgets[WIDX_VEHICLE_TERNARY_COLOUR].type = WindowWidgetType::Empty;
            }

            // Vehicle colour scheme type
            if (!ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_VEHICLE_IS_INTEGRAL)
                && (ride->num_cars_per_train | ride->NumTrains) > 1)
            {
                window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_SCHEME].type = WindowWidgetType::DropdownMenu;
                window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_SCHEME_DROPDOWN].type = WindowWidgetType::Button;
            }
            else
            {
                window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_SCHEME].type = WindowWidgetType::Empty;
                window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_SCHEME_DROPDOWN].type = WindowWidgetType::Empty;
            }
            ft.Rewind();
            ft.Increment(6);
            ft.Add<StringId>(VehicleColourSchemeNames[vehicleColourSchemeType]);
            ft.Add<StringId>(GetRideComponentName(ride->GetRideTypeDescriptor().NameConvention.vehicle).singular);
            ft.Add<StringId>(GetRideComponentName(ride->GetRideTypeDescriptor().NameConvention.vehicle).capitalised);
            ft.Add<uint16_t>(vehicleIndex + 1);

            // Vehicle index
            if (vehicleColourSchemeType != 0)
            {
                window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_INDEX].type = WindowWidgetType::DropdownMenu;
                window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_INDEX_DROPDOWN].type = WindowWidgetType::Button;
                window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_INDEX].text = vehicleColourSchemeType == 1
                    ? STR_RIDE_COLOUR_TRAIN_VALUE
                    : STR_RIDE_COLOUR_VEHICLE_VALUE;
            }
            else
            {
                window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_INDEX].type = WindowWidgetType::Empty;
                window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_INDEX_DROPDOWN].type = WindowWidgetType::Empty;
            }
        }
        else
        {
            window_ride_colour_widgets[WIDX_VEHICLE_PREVIEW].type = WindowWidgetType::Empty;
            window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_SCHEME].type = WindowWidgetType::Empty;
            window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_SCHEME_DROPDOWN].type = WindowWidgetType::Empty;
            window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_INDEX].type = WindowWidgetType::Empty;
            window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_INDEX_DROPDOWN].type = WindowWidgetType::Empty;
            window_ride_colour_widgets[WIDX_VEHICLE_BODY_COLOUR].type = WindowWidgetType::Empty;
            window_ride_colour_widgets[WIDX_VEHICLE_TRIM_COLOUR].type = WindowWidgetType::Empty;
            window_ride_colour_widgets[WIDX_VEHICLE_TERNARY_COLOUR].type = WindowWidgetType::Empty;
        }

        ft.Rewind();
        ft.Increment(14);
        ft.Add<StringId>(ColourSchemeNames[colourScheme]);

        AnchorBorderWidgets();
        WindowAlignTabs(this, WIDX_TAB_1, WIDX_TAB_10);
    }

    void ColourOnDraw(DrawPixelInfo& dpi)
    {
        // TODO: This should use lists and identified sprites
        DrawPixelInfo clippedDpi;

        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        DrawWidgets(dpi);
        DrawTabImages(dpi);

        // Track / shop item preview
        const auto& trackPreviewWidget = window_ride_colour_widgets[WIDX_TRACK_PREVIEW];
        if (trackPreviewWidget.type != WindowWidgetType::Empty)
            GfxFillRect(
                dpi,
                { { windowPos + ScreenCoordsXY{ trackPreviewWidget.left + 1, trackPreviewWidget.top + 1 } },
                  { windowPos + ScreenCoordsXY{ trackPreviewWidget.right - 1, trackPreviewWidget.bottom - 1 } } },
                PALETTE_INDEX_12);

        auto trackColour = ride->track_colour[ride_colour];

        //
        auto rideEntry = ride->GetRideEntry();
        if (rideEntry == nullptr || rideEntry->shop_item[0] == ShopItem::None)
        {
            auto screenCoords = windowPos + ScreenCoordsXY{ trackPreviewWidget.left, trackPreviewWidget.top };

            // Track
            const auto& rtd = ride->GetRideTypeDescriptor();
            if (rtd.HasFlag(RIDE_TYPE_FLAG_IS_MAZE))
            {
                GfxDrawSprite(dpi, ImageId(MazeOptions[trackColour.supports].sprite), screenCoords);
            }
            else
            {
                auto typeDescriptor = ride->GetRideTypeDescriptor();
                int32_t spriteIndex = typeDescriptor.ColourPreview.Track;
                if (spriteIndex != 0)
                {
                    GfxDrawSprite(dpi, ImageId(spriteIndex, trackColour.main, trackColour.additional), screenCoords);
                }

                // Supports
                spriteIndex = typeDescriptor.ColourPreview.Supports;
                if (spriteIndex != 0)
                {
                    GfxDrawSprite(dpi, ImageId(spriteIndex, trackColour.supports), screenCoords);
                }
            }
        }
        else
        {
            auto screenCoords = windowPos
                + ScreenCoordsXY{ (trackPreviewWidget.left + trackPreviewWidget.right) / 2 - 8,
                                  (trackPreviewWidget.bottom + trackPreviewWidget.top) / 2 - 6 };

            ShopItem shopItem = rideEntry->shop_item[1] == ShopItem::None ? rideEntry->shop_item[0] : rideEntry->shop_item[1];
            if (ride->HasLifecycleFlag(RIDE_LIFECYCLE_RANDOM_SHOP_COLOURS))
            {
                colour_t spriteColour = COLOUR_BLACK;
                // Limit update rate of preview to avoid making people dizzy.
                if ((gCurrentTicks % 64) == 0)
                {
                    spriteColour++;
                    if (spriteColour >= COLOUR_NUM_NORMAL)
                    {
                        spriteColour = COLOUR_BLACK;
                    }
                }

                GfxDrawSprite(dpi, ImageId(GetShopItemDescriptor(shopItem).Image, spriteColour), screenCoords);
            }
            else
            {
                GfxDrawSprite(dpi, ImageId(GetShopItemDescriptor(shopItem).Image, ride->track_colour[0].main), screenCoords);
            }
        }

        // Entrance preview
        trackColour = ride->track_colour[0];
        const auto& entrancePreviewWidget = widgets[WIDX_ENTRANCE_PREVIEW];
        if (entrancePreviewWidget.type != WindowWidgetType::Empty)
        {
            if (ClipDrawPixelInfo(
                    clippedDpi, dpi,
                    windowPos + ScreenCoordsXY{ entrancePreviewWidget.left + 1, entrancePreviewWidget.top + 1 },
                    entrancePreviewWidget.width(), entrancePreviewWidget.height()))
            {
                GfxClear(&clippedDpi, PALETTE_INDEX_12);

                auto stationObj = ride->GetStationObject();
                if (stationObj != nullptr && stationObj->BaseImageId != ImageIndexUndefined)
                {
                    auto imageId = ImageId(stationObj->BaseImageId, trackColour.main, trackColour.additional);

                    // Back
                    GfxDrawSprite(clippedDpi, imageId, { 34, 20 });

                    // Front
                    GfxDrawSprite(clippedDpi, imageId.WithIndexOffset(4), { 34, 20 });

                    // Glass
                    if (stationObj->Flags & STATION_OBJECT_FLAGS::IS_TRANSPARENT)
                    {
                        auto glassImageId = ImageId(stationObj->BaseImageId + 20).WithTransparency(trackColour.main);
                        GfxDrawSprite(clippedDpi, glassImageId, { 34, 20 });
                    }
                }
            }

            DrawTextEllipsised(dpi, { windowPos.x + 3, windowPos.y + 103 }, 97, STR_STATION_STYLE, {});
        }
    }

    void ColourOnScrollDraw(DrawPixelInfo& dpi, int32_t scrollIndex) const
    {
        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        auto rideEntry = ride->GetRideEntry();
        if (rideEntry == nullptr)
            return;

        auto vehiclePreviewWidget = &window_ride_colour_widgets[WIDX_VEHICLE_PREVIEW];
        auto vehicleColour = RideGetVehicleColour(*ride, vehicleIndex);

        // Background colour
        GfxFillRect(dpi, { { dpi.x, dpi.y }, { dpi.x + dpi.width - 1, dpi.y + dpi.height - 1 } }, PALETTE_INDEX_12);

        // ?
        auto screenCoords = ScreenCoordsXY{ vehiclePreviewWidget->width() / 2, vehiclePreviewWidget->height() - 15 };

        // ?
        auto trainCarIndex = (ride->colour_scheme_type & 3) == RIDE_COLOUR_SCHEME_MODE_DIFFERENT_PER_CAR ? vehicleIndex
                                                                                                         : rideEntry->TabCar;

        const auto& carEntry = rideEntry->Cars[RideEntryGetVehicleAtPosition(
            ride->subtype, ride->num_cars_per_train, trainCarIndex)];

        screenCoords.y += carEntry.tab_height;

        // Draw the coloured spinning vehicle
        // frame_no represents a SpritePrecision of 64
        ImageIndex imageIndex = carEntry.SpriteByYaw(frame_no / 2, SpriteGroupType::SlopeFlat);
        imageIndex &= carEntry.TabRotationMask;
        imageIndex *= carEntry.base_num_frames;
        imageIndex += carEntry.base_image_id;
        auto imageId = ImageId(imageIndex, vehicleColour.Body, vehicleColour.Trim, vehicleColour.Tertiary);
        GfxDrawSprite(dpi, imageId, screenCoords);
    }

#pragma endregion

#pragma region Music
    std::vector<ObjectEntryIndex> window_ride_current_music_style_order;

    void ToggleMusic()
    {
        auto ride = GetRide(rideId);
        if (ride != nullptr)
        {
            int32_t activateMusic = (ride->lifecycle_flags & RIDE_LIFECYCLE_MUSIC) ? 0 : 1;
            SetOperatingSetting(rideId, RideSetSetting::Music, activateMusic);
        }
    }

    void MusicOnMouseUp(WidgetIndex widgetIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                Close();
                return;
            case WIDX_TAB_1:
            case WIDX_TAB_2:
            case WIDX_TAB_3:
            case WIDX_TAB_4:
            case WIDX_TAB_5:
            case WIDX_TAB_6:
            case WIDX_TAB_7:
            case WIDX_TAB_8:
            case WIDX_TAB_9:
            case WIDX_TAB_10:
                SetPage(widgetIndex - WIDX_TAB_1);
                break;
            case WIDX_PLAY_MUSIC:
                ToggleMusic();
                break;
        }
    }

    void MusicResize()
    {
        flags |= WF_RESIZABLE;
        WindowSetResize(*this, 316, 81, 316, 81);
    }

    static std::string GetMusicString(ObjectEntryIndex musicObjectIndex)
    {
        auto& objManager = GetContext()->GetObjectManager();
        auto musicObj = static_cast<MusicObject*>(objManager.GetLoadedObject(ObjectType::Music, musicObjectIndex));

        auto name = musicObj->NameStringId;
        return FormatStringID(name, {});
    }

    /**
     *
     *  rct2: 0x006B1EFC
     */
    void MusicOnMouseDown(WidgetIndex widgetIndex)
    {
        if (widgetIndex != WIDX_MUSIC_DROPDOWN)
            return;

        auto dropdownWidget = &widgets[widgetIndex] - 1;
        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        // Construct list of available music
        auto& musicOrder = window_ride_current_music_style_order;
        musicOrder.clear();
        auto& objManager = GetContext()->GetObjectManager();
        for (ObjectEntryIndex i = 0; i < MAX_MUSIC_OBJECTS; i++)
        {
            auto musicObj = static_cast<MusicObject*>(objManager.GetLoadedObject(ObjectType::Music, i));
            if (musicObj != nullptr)
            {
                // Hide custom music if the WAV file does not exist
                auto originalStyleId = musicObj->GetOriginalStyleId();
                if (originalStyleId.has_value()
                    && (originalStyleId == MUSIC_STYLE_CUSTOM_MUSIC_1 || originalStyleId == MUSIC_STYLE_CUSTOM_MUSIC_2))
                {
                    auto numTracks = musicObj->GetTrackCount();
                    if (numTracks != 0)
                    {
                        auto track0 = musicObj->GetTrack(0);
                        if (!track0->Asset.IsAvailable())
                        {
                            continue;
                        }
                    }
                    else
                    {
                        continue;
                    }
                }

                if (gCheatsUnlockOperatingLimits || musicObj->SupportsRideType(ride->type))
                {
                    musicOrder.push_back(i);
                }
            }
        }

        // Sort available music by the alphabetical order
        std::stable_sort(musicOrder.begin(), musicOrder.end(), [](const ObjectEntryIndex& a, const ObjectEntryIndex& b) {
            return String::Compare(GetMusicString(b), GetMusicString(a), false) > 0;
        });

        // Setup dropdown list
        auto numItems = musicOrder.size();
        for (size_t i = 0; i < numItems; i++)
        {
            auto musicObj = static_cast<MusicObject*>(objManager.GetLoadedObject(ObjectType::Music, musicOrder[i]));
            gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
            gDropdownItems[i].Args = musicObj->NameStringId;
        }

        WindowDropdownShowTextCustomWidth(
            { windowPos.x + dropdownWidget->left, windowPos.y + dropdownWidget->top }, dropdownWidget->height() + 1, colours[1],
            0, Dropdown::Flag::StayOpen, numItems, widgets[widgetIndex].right - dropdownWidget->left);

        // Set currently checked item
        for (size_t i = 0; i < numItems; i++)
        {
            if (musicOrder[i] == ride->music)
            {
                Dropdown::SetChecked(static_cast<int32_t>(i), true);
            }
        }
    }

    void MusicOnDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
    {
        if (widgetIndex == WIDX_MUSIC_DROPDOWN && dropdownIndex >= 0
            && static_cast<size_t>(dropdownIndex) < window_ride_current_music_style_order.size())
        {
            auto musicStyle = window_ride_current_music_style_order[dropdownIndex];
            SetOperatingSetting(rideId, RideSetSetting::MusicType, musicStyle);
        }
    }

    void MusicUpdate()
    {
        frame_no++;
        OnPrepareDraw();
        WidgetInvalidate(*this, WIDX_TAB_6);
    }

    void MusicOnPrepareDraw()
    {
        auto newWidgets = window_ride_page_widgets[page];
        if (widgets != newWidgets)
        {
            widgets = newWidgets;
            InitScrollWidgets();
        }

        SetPressedTab();

        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        auto ft = Formatter::Common();
        ride->FormatNameTo(ft);

        // Set selected music
        StringId musicName = STR_NONE;
        auto& objManager = GetContext()->GetObjectManager();
        auto musicObj = static_cast<MusicObject*>(objManager.GetLoadedObject(ObjectType::Music, ride->music));
        if (musicObj != nullptr)
        {
            musicName = musicObj->NameStringId;
        }
        window_ride_music_widgets[WIDX_MUSIC].text = musicName;

        // Set music activated
        auto isMusicActivated = (ride->lifecycle_flags & RIDE_LIFECYCLE_MUSIC) != 0;
        if (isMusicActivated)
        {
            pressed_widgets |= (1uLL << WIDX_PLAY_MUSIC);
            disabled_widgets &= ~(1uLL << WIDX_MUSIC);
            disabled_widgets &= ~(1uLL << WIDX_MUSIC_DROPDOWN);
        }
        else
        {
            pressed_widgets &= ~(1uLL << WIDX_PLAY_MUSIC);
            disabled_widgets |= (1uLL << WIDX_MUSIC);
            disabled_widgets |= (1uLL << WIDX_MUSIC_DROPDOWN);
        }

        AnchorBorderWidgets();
        WindowAlignTabs(this, WIDX_TAB_1, WIDX_TAB_10);
    }

    void MusicOnDraw(DrawPixelInfo& dpi)
    {
        DrawWidgets(dpi);
        DrawTabImages(dpi);
    }

#pragma endregion

#pragma region Measurements

    static constexpr StringId GetRatingName(ride_rating rating)
    {
        int32_t index = std::clamp<int32_t>(rating >> 8, 0, static_cast<int32_t>(std::size(RatingNames)) - 1);
        return RatingNames[index];
    }

    void SetupScenerySelection()
    {
        if (gTrackDesignSaveMode)
        {
            CancelScenerySelection();
        }

        while (ToolSet(*this, WIDX_BACKGROUND, Tool::Crosshair))
            ;

        gTrackDesignSaveRideIndex = rideId;

        TrackDesignSaveInit();
        gGamePaused |= GAME_PAUSED_SAVING_TRACK;
        gTrackDesignSaveMode = true;

        OpenRCT2::Audio::StopAll();

        WindowBase* w_main = WindowGetMain();
        if (w_main != nullptr)
        {
            w_main->viewport->flags |= (VIEWPORT_FLAG_HIDE_VERTICAL | VIEWPORT_FLAG_HIDE_BASE);
        }

        GfxInvalidateScreen();
    }

    void MeasurementsDesignReset()
    {
        TrackDesignSaveResetScenery();
    }

    void MeasurementsDesignSelectNearbyScenery()
    {
        TrackDesignSaveSelectNearbyScenery(gTrackDesignSaveRideIndex);
    }

    void MeasurementsDesignCancel()
    {
        if (gTrackDesignSaveMode)
        {
            CancelScenerySelection();
        }
    }

    static void TrackDesignCallback(int32_t result, [[maybe_unused]] const utf8* path)
    {
        if (result == MODAL_RESULT_OK)
        {
            TrackRepositoryScan();
        }
        GfxInvalidateScreen();
    };

    void MeasurementsDesignSave()
    {
        TrackDesignState tds{};

        Ride* ride = GetRide(rideId);
        _trackDesign = ride->SaveToTrackDesign(tds);
        if (!_trackDesign)
        {
            return;
        }

        if (gTrackDesignSaveMode)
        {
            auto errMessage = _trackDesign->CreateTrackDesignScenery(tds);
            if (!errMessage.Successful)
            {
                ContextShowError(STR_CANT_SAVE_TRACK_DESIGN, errMessage.Message, {});
                return;
            }
            if (errMessage.HasMessage())
            {
                ContextShowError(errMessage.Message, STR_EMPTY, {});
            }
        }

        auto trackName = ride->GetName();
        auto intent = Intent(WindowClass::Loadsave);
        intent.PutExtra(INTENT_EXTRA_LOADSAVE_TYPE, LOADSAVETYPE_SAVE | LOADSAVETYPE_TRACK);
        intent.PutExtra(INTENT_EXTRA_TRACK_DESIGN, _trackDesign.get());
        intent.PutExtra(INTENT_EXTRA_PATH, trackName);
        intent.PutExtra(INTENT_EXTRA_CALLBACK, reinterpret_cast<void*>(&TrackDesignCallback));

        ContextOpenIntent(&intent);
    }

    void MeasurementsClose()
    {
        MeasurementsDesignCancel();
    }

    void MeasurementsOnMouseUp(WidgetIndex widgetIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                Close();
                return;
            case WIDX_TAB_1:
            case WIDX_TAB_2:
            case WIDX_TAB_3:
            case WIDX_TAB_4:
            case WIDX_TAB_5:
            case WIDX_TAB_6:
            case WIDX_TAB_7:
            case WIDX_TAB_8:
            case WIDX_TAB_9:
            case WIDX_TAB_10:
                SetPage(widgetIndex - WIDX_TAB_1);
                break;
            case WIDX_SELECT_NEARBY_SCENERY:
                MeasurementsDesignSelectNearbyScenery();
                break;
            case WIDX_RESET_SELECTION:
                MeasurementsDesignReset();
                break;
            case WIDX_SAVE_DESIGN:
                MeasurementsDesignSave();
                break;
            case WIDX_CANCEL_DESIGN:
                MeasurementsDesignCancel();
                break;
        }
    }

    void MeasurementsResize()
    {
        WindowSetResize(*this, 316, 234, 316, 234);
    }

    void MeasurementsOnMouseDown(WidgetIndex widgetIndex)
    {
        if (widgetIndex != WIDX_SAVE_TRACK_DESIGN)
            return;

        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        gDropdownItems[0].Format = STR_SAVE_TRACK_DESIGN_ITEM;
        gDropdownItems[1].Format = STR_SAVE_TRACK_DESIGN_WITH_SCENERY_ITEM;

        WindowDropdownShowText(
            { windowPos.x + widgets[widgetIndex].left, windowPos.y + widgets[widgetIndex].top },
            widgets[widgetIndex].height() + 1, colours[1], Dropdown::Flag::StayOpen, 2);
        gDropdownDefaultIndex = 0;
        if (!ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_TRACK))
        {
            // Disable saving without scenery if we're a flat ride
            Dropdown::SetDisabled(0, true);
            gDropdownDefaultIndex = 1;
        }
    }

    void MeasurementsOnDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
    {
        if (widgetIndex != WIDX_SAVE_TRACK_DESIGN)
            return;

        if (dropdownIndex == -1)
            dropdownIndex = gDropdownHighlightedIndex;

        if (dropdownIndex == 0)
        {
            MeasurementsDesignSave();
        }
        else
            SetupScenerySelection();
    }

    void MeasurementsUpdate()
    {
        frame_no++;
        OnPrepareDraw();
        WidgetInvalidate(*this, WIDX_TAB_7);
    }

    void MeasurementsOnToolDown(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
    {
        _lastSceneryX = screenCoords.x;
        _lastSceneryY = screenCoords.y;
        _collectTrackDesignScenery = true; // Default to true in case user does not select anything valid

        constexpr auto interactionFlags = EnumsToFlags(
            ViewportInteractionItem::Scenery, ViewportInteractionItem::Footpath, ViewportInteractionItem::Wall,
            ViewportInteractionItem::LargeScenery);
        auto info = GetMapCoordinatesFromPos(screenCoords, interactionFlags);
        switch (info.SpriteType)
        {
            case ViewportInteractionItem::Scenery:
            case ViewportInteractionItem::LargeScenery:
            case ViewportInteractionItem::Wall:
            case ViewportInteractionItem::Footpath:
                _collectTrackDesignScenery = !TrackDesignSaveContainsTileElement(info.Element);
                TrackDesignSaveSelectTileElement(info.SpriteType, info.Loc, info.Element, _collectTrackDesignScenery);
                break;
            default:
                break;
        }
    }

    void MeasurementsOnToolDrag(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
    {
        if (screenCoords.x == _lastSceneryX && screenCoords.y == _lastSceneryY)
            return;
        _lastSceneryX = screenCoords.x;
        _lastSceneryY = screenCoords.y;

        auto interactionFlags = EnumsToFlags(
            ViewportInteractionItem::Scenery, ViewportInteractionItem::Footpath, ViewportInteractionItem::Wall,
            ViewportInteractionItem::LargeScenery);
        auto info = GetMapCoordinatesFromPos(screenCoords, interactionFlags);
        switch (info.SpriteType)
        {
            case ViewportInteractionItem::Scenery:
            case ViewportInteractionItem::LargeScenery:
            case ViewportInteractionItem::Wall:
            case ViewportInteractionItem::Footpath:
                TrackDesignSaveSelectTileElement(info.SpriteType, info.Loc, info.Element, _collectTrackDesignScenery);
                break;
            default:
                break;
        }
    }

    void MeasurementsOnToolAbort(WidgetIndex widgetIndex)
    {
        MeasurementsDesignCancel();
    }

    void MeasurementsOnPrepareDraw()
    {
        auto newWidgets = window_ride_page_widgets[page];
        if (widgets != newWidgets)
        {
            widgets = newWidgets;
            InitScrollWidgets();
        }

        SetPressedTab();

        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        auto ft = Formatter::Common();
        ride->FormatNameTo(ft);

        window_ride_measurements_widgets[WIDX_SAVE_TRACK_DESIGN].tooltip = STR_SAVE_TRACK_DESIGN_NOT_POSSIBLE;
        window_ride_measurements_widgets[WIDX_SAVE_TRACK_DESIGN].type = WindowWidgetType::Empty;
        if (gTrackDesignSaveMode && gTrackDesignSaveRideIndex == rideId)
        {
            window_ride_measurements_widgets[WIDX_SELECT_NEARBY_SCENERY].type = WindowWidgetType::Button;
            window_ride_measurements_widgets[WIDX_RESET_SELECTION].type = WindowWidgetType::Button;
            window_ride_measurements_widgets[WIDX_SAVE_DESIGN].type = WindowWidgetType::Button;
            window_ride_measurements_widgets[WIDX_CANCEL_DESIGN].type = WindowWidgetType::Button;
        }
        else
        {
            window_ride_measurements_widgets[WIDX_SELECT_NEARBY_SCENERY].type = WindowWidgetType::Empty;
            window_ride_measurements_widgets[WIDX_RESET_SELECTION].type = WindowWidgetType::Empty;
            window_ride_measurements_widgets[WIDX_SAVE_DESIGN].type = WindowWidgetType::Empty;
            window_ride_measurements_widgets[WIDX_CANCEL_DESIGN].type = WindowWidgetType::Empty;

            window_ride_measurements_widgets[WIDX_SAVE_TRACK_DESIGN].type = WindowWidgetType::FlatBtn;
            disabled_widgets |= (1uLL << WIDX_SAVE_TRACK_DESIGN);
            if (ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED)
            {
                if (ride->excitement != RIDE_RATING_UNDEFINED)
                {
                    disabled_widgets &= ~(1uLL << WIDX_SAVE_TRACK_DESIGN);
                    window_ride_measurements_widgets[WIDX_SAVE_TRACK_DESIGN].tooltip = STR_SAVE_TRACK_DESIGN;
                }
            }
        }

        AnchorBorderWidgets();
        WindowAlignTabs(this, WIDX_TAB_1, WIDX_TAB_10);
    }

    void MeasurementsOnDraw(DrawPixelInfo& dpi)
    {
        DrawWidgets(dpi);
        DrawTabImages(dpi);

        if (window_ride_measurements_widgets[WIDX_SAVE_DESIGN].type == WindowWidgetType::Button)
        {
            Widget* widget = &window_ride_measurements_widgets[WIDX_PAGE_BACKGROUND];

            ScreenCoordsXY widgetCoords(windowPos.x + widget->width() / 2, windowPos.y + widget->top + 40);
            DrawTextWrapped(dpi, widgetCoords, width - 8, STR_CLICK_ITEMS_OF_SCENERY_TO_SELECT, {}, { TextAlignment::CENTRE });

            widgetCoords.x = windowPos.x + 4;
            widgetCoords.y = windowPos.y + window_ride_measurements_widgets[WIDX_SELECT_NEARBY_SCENERY].bottom + 17;
            GfxFillRectInset(
                dpi, { widgetCoords, { windowPos.x + 312, widgetCoords.y + 1 } }, colours[1], INSET_RECT_FLAG_BORDER_INSET);
        }
        else
        {
            auto ride = GetRide(rideId);
            if (ride == nullptr)
                return;

            auto screenCoords = windowPos
                + ScreenCoordsXY{ window_ride_measurements_widgets[WIDX_PAGE_BACKGROUND].left + 4,
                                  window_ride_measurements_widgets[WIDX_PAGE_BACKGROUND].top + 4 };

            if (ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED)
            {
                // Excitement
                StringId ratingName = GetRatingName(ride->excitement);
                auto ft = Formatter();
                ft.Add<uint32_t>(ride->excitement);
                ft.Add<StringId>(ratingName);
                StringId stringId = !RideHasRatings(*ride) ? STR_EXCITEMENT_RATING_NOT_YET_AVAILABLE : STR_EXCITEMENT_RATING;
                DrawTextBasic(dpi, screenCoords, stringId, ft);
                screenCoords.y += LIST_ROW_HEIGHT;

                // Intensity
                ratingName = GetRatingName(ride->intensity);
                ft = Formatter();
                ft.Add<uint32_t>(ride->intensity);
                ft.Add<StringId>(ratingName);

                stringId = STR_INTENSITY_RATING;
                if (!RideHasRatings(*ride))
                    stringId = STR_INTENSITY_RATING_NOT_YET_AVAILABLE;
                else if (ride->intensity >= RIDE_RATING(10, 00))
                    stringId = STR_INTENSITY_RATING_RED;

                DrawTextBasic(dpi, screenCoords, stringId, ft);
                screenCoords.y += LIST_ROW_HEIGHT;

                // Nausea
                ratingName = GetRatingName(ride->nausea);
                ft = Formatter();
                ft.Add<uint32_t>(ride->nausea);
                ft.Add<StringId>(ratingName);
                stringId = !RideHasRatings(*ride) ? STR_NAUSEA_RATING_NOT_YET_AVAILABLE : STR_NAUSEA_RATING;
                DrawTextBasic(dpi, screenCoords, stringId, ft);
                screenCoords.y += 2 * LIST_ROW_HEIGHT;

                // Horizontal rule
                GfxFillRectInset(
                    dpi, { screenCoords - ScreenCoordsXY{ 0, 6 }, screenCoords + ScreenCoordsXY{ 303, -5 } }, colours[1],
                    INSET_RECT_FLAG_BORDER_INSET);

                if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_NO_RAW_STATS))
                {
                    if (ride->type == RIDE_TYPE_MINI_GOLF)
                    {
                        // Holes
                        ft = Formatter();
                        ft.Add<uint16_t>(ride->holes);
                        DrawTextBasic(dpi, screenCoords, STR_HOLES, ft);
                        screenCoords.y += LIST_ROW_HEIGHT;
                    }
                    else
                    {
                        // Max speed
                        ft = Formatter();
                        ft.Add<int32_t>((ride->max_speed * 9) >> 18);
                        DrawTextBasic(dpi, screenCoords, STR_MAX_SPEED, ft);
                        screenCoords.y += LIST_ROW_HEIGHT;

                        // Average speed
                        ft = Formatter();
                        ft.Add<int32_t>((ride->average_speed * 9) >> 18);
                        DrawTextBasic(dpi, screenCoords, STR_AVERAGE_SPEED, ft);
                        screenCoords.y += LIST_ROW_HEIGHT;

                        // Ride time
                        ft = Formatter();
                        int32_t numTimes = 0;
                        // TODO: STR_RIDE_TIME only takes up to 4 stations modify to take more
                        // also if modified may need to be split into multiple format strings
                        // as formatter cannot take more than 256 bytes
                        for (int32_t i = 0; i < std::min<int32_t>(ride->num_stations, 4); i++)
                        {
                            StationIndex stationIndex = StationIndex::FromUnderlying(numTimes);
                            auto time = ride->GetStation(stationIndex).SegmentTime;
                            if (time != 0)
                            {
                                ft.Add<uint16_t>(STR_RIDE_TIME_ENTRY_WITH_SEPARATOR);
                                ft.Add<uint16_t>(time);
                                numTimes++;
                            }
                        }
                        if (numTimes == 0)
                        {
                            ft.Add<uint16_t>(STR_RIDE_TIME_ENTRY);
                            ft.Add<uint16_t>(0);
                            numTimes++;
                        }
                        else
                        {
                            // sadly, STR_RIDE_TIME_ENTRY_WITH_SEPARATOR are defined with the separator AFTER an entry
                            // therefore we set the last entry to use the no-separator format now, post-format
                            ft.Rewind();
                            ft.Increment((numTimes - 1) * 4);
                            ft.Add<uint16_t>(STR_RIDE_TIME_ENTRY);
                        }
                        ft.Rewind();
                        ft.Increment(numTimes * 4);
                        ft.Add<uint16_t>(0);
                        ft.Add<uint16_t>(0);
                        ft.Add<uint16_t>(0);
                        ft.Add<uint16_t>(0);
                        DrawTextEllipsised(dpi, screenCoords, 308, STR_RIDE_TIME, ft);
                        screenCoords.y += LIST_ROW_HEIGHT;
                    }

                    // Ride length
                    ft = Formatter();
                    int32_t numLengths = 0;
                    // TODO: see above STR_RIDE_LENGTH is also only able to display max 4
                    for (int32_t i = 0; i < std::min<int32_t>(ride->num_stations, 4); i++)
                    {
                        StationIndex stationIndex = StationIndex::FromUnderlying(i);
                        auto length = ride->GetStation(stationIndex).SegmentLength;
                        if (length != 0)
                        {
                            length >>= 16;
                            ft.Add<StringId>(STR_RIDE_LENGTH_ENTRY_WITH_SEPARATOR);
                            ft.Add<uint16_t>(length & 0xFFFF);
                            numLengths++;
                        }
                    }
                    if (numLengths == 0)
                    {
                        ft.Add<StringId>(STR_RIDE_LENGTH_ENTRY);
                        ft.Add<uint16_t>(0);
                        numLengths++;
                    }
                    else
                    {
                        // sadly, STR_RIDE_LENGTH_ENTRY_WITH_SEPARATOR are defined with the separator AFTER an entry
                        // therefore we set the last entry to use the no-separator format now, post-format
                        ft.Rewind();
                        ft.Increment((numLengths - 1) * 4);
                        ft.Add<StringId>(STR_RIDE_LENGTH_ENTRY);
                    }
                    ft.Rewind();
                    ft.Increment(numLengths * 4);
                    ft.Add<uint16_t>(0);
                    ft.Add<uint16_t>(0);
                    ft.Add<uint16_t>(0);
                    ft.Add<uint16_t>(0);
                    DrawTextEllipsised(dpi, screenCoords, 308, STR_RIDE_LENGTH, ft);

                    screenCoords.y += LIST_ROW_HEIGHT;

                    if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_G_FORCES))
                    {
                        // Max. positive vertical G's
                        stringId = STR_MAX_POSITIVE_VERTICAL_G;

                        ft = Formatter();
                        ft.Add<fixed16_2dp>(ride->max_positive_vertical_g);
                        DrawTextBasic(dpi, screenCoords, stringId, ft);
                        screenCoords.y += LIST_ROW_HEIGHT;

                        // Max. negative vertical G's
                        stringId = ride->max_negative_vertical_g <= RIDE_G_FORCES_RED_NEG_VERTICAL
                            ? STR_MAX_NEGATIVE_VERTICAL_G_RED
                            : STR_MAX_NEGATIVE_VERTICAL_G;
                        ft = Formatter();
                        ft.Add<int32_t>(ride->max_negative_vertical_g);
                        DrawTextBasic(dpi, screenCoords, stringId, ft);
                        screenCoords.y += LIST_ROW_HEIGHT;

                        // Max lateral G's
                        stringId = ride->max_lateral_g > RIDE_G_FORCES_RED_LATERAL ? STR_MAX_LATERAL_G_RED : STR_MAX_LATERAL_G;
                        ft = Formatter();
                        ft.Add<fixed16_2dp>(ride->max_lateral_g);
                        DrawTextBasic(dpi, screenCoords, stringId, ft);
                        screenCoords.y += LIST_ROW_HEIGHT;

                        // Total 'air' time
                        ft = Formatter();
                        ft.Add<fixed32_2dp>(ride->total_air_time * 3);
                        DrawTextBasic(dpi, screenCoords, STR_TOTAL_AIR_TIME, ft);
                        screenCoords.y += LIST_ROW_HEIGHT;
                    }

                    if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_DROPS))
                    {
                        // Drops
                        auto drops = ride->drops & 0x3F;
                        ft = Formatter();
                        ft.Add<uint16_t>(drops);
                        DrawTextBasic(dpi, screenCoords, STR_DROPS, ft);
                        screenCoords.y += LIST_ROW_HEIGHT;

                        // Highest drop height
                        auto highestDropHeight = (ride->highest_drop_height * 3) / 4;
                        ft = Formatter();
                        ft.Add<int32_t>(highestDropHeight);
                        DrawTextBasic(dpi, screenCoords, STR_HIGHEST_DROP_HEIGHT, ft);
                        screenCoords.y += LIST_ROW_HEIGHT;
                    }

                    if (ride->type != RIDE_TYPE_MINI_GOLF)
                    {
                        // Inversions
                        if (ride->inversions != 0)
                        {
                            ft = Formatter();
                            ft.Add<uint16_t>(ride->inversions);
                            DrawTextBasic(dpi, screenCoords, STR_INVERSIONS, ft);
                            screenCoords.y += LIST_ROW_HEIGHT;
                        }
                    }
                }
            }
            else
            {
                DrawTextBasic(dpi, screenCoords, STR_NO_TEST_RESULTS_YET);
            }
        }
    }

#pragma endregion

#pragma region Graphs

    enum
    {
        GRAPH_VELOCITY,
        GRAPH_ALTITUDE,
        GRAPH_VERTICAL,
        GRAPH_LATERAL
    };

    void SetGraph(int32_t type)
    {
        if ((list_information_type & 0xFF) == type)
        {
            list_information_type ^= 0x8000;
        }
        else
        {
            list_information_type &= 0xFF00;
            list_information_type |= type;
        }
        Invalidate();
    }

    void GraphsOnMouseUp(WidgetIndex widgetIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                Close();
                return;
            case WIDX_TAB_1:
            case WIDX_TAB_2:
            case WIDX_TAB_3:
            case WIDX_TAB_4:
            case WIDX_TAB_5:
            case WIDX_TAB_6:
            case WIDX_TAB_7:
            case WIDX_TAB_8:
            case WIDX_TAB_9:
            case WIDX_TAB_10:
                SetPage(widgetIndex - WIDX_TAB_1);
                break;
        }
    }

    void GraphsResize()
    {
        WindowSetResize(*this, 316, 182, 500, 450);
    }

    void GraphsOnMouseDown(WidgetIndex widgetIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_GRAPH_VELOCITY:
                SetGraph(GRAPH_VELOCITY);
                break;
            case WIDX_GRAPH_ALTITUDE:
                SetGraph(GRAPH_ALTITUDE);
                break;
            case WIDX_GRAPH_VERTICAL:
                SetGraph(GRAPH_VERTICAL);
                break;
            case WIDX_GRAPH_LATERAL:
                SetGraph(GRAPH_LATERAL);
                break;
        }
    }

    void GraphsUpdate()
    {
        Widget* widget;
        int32_t x;

        frame_no++;
        OnPrepareDraw();
        WidgetInvalidate(*this, WIDX_TAB_8);
        OnPrepareDraw();
        WidgetInvalidate(*this, WIDX_GRAPH);

        widget = &window_ride_graphs_widgets[WIDX_GRAPH];
        x = scrolls[0].h_left;
        if (!(list_information_type & 0x8000))
        {
            auto ride = GetRide(rideId);
            if (ride != nullptr)
            {
                RideMeasurement* measurement{};
                std::tie(measurement, std::ignore) = ride->GetMeasurement();
                x = measurement == nullptr ? 0 : measurement->current_item - ((widget->width() / 4) * 3);
            }
        }

        scrolls[0].h_left = std::clamp(x, 0, scrolls[0].h_right - (widget->width() - 2));
        WidgetScrollUpdateThumbs(*this, WIDX_GRAPH);
    }

    ScreenSize GraphsScrollGetSize(int32_t scrollIndex)
    {
        OnPrepareDraw();

        ScreenSize size{};
        // Set minimum size
        size.width = window_ride_graphs_widgets[WIDX_GRAPH].width() - 2;

        // Get measurement size
        auto ride = GetRide(rideId);
        if (ride != nullptr)
        {
            RideMeasurement* measurement{};
            std::tie(measurement, std::ignore) = ride->GetMeasurement();
            if (measurement != nullptr)
            {
                size.width = std::max<int32_t>(size.width, measurement->num_items);
            }
        }
        return size;
    }

    void Graphs15(int32_t scrollIndex, int32_t scrollAreaType)
    {
        list_information_type |= 0x8000;
    }

    OpenRCT2String GraphsTooltip(const WidgetIndex widgetIndex, const StringId fallback)
    {
        if (widgetIndex == WIDX_GRAPH)
        {
            auto ride = GetRide(rideId);
            if (ride != nullptr)
            {
                auto [measurement, message] = ride->GetMeasurement();
                if (measurement != nullptr && (measurement->flags & RIDE_MEASUREMENT_FLAG_RUNNING))
                {
                    auto ft = Formatter();
                    ft.Increment(2);
                    ft.Add<StringId>(GetRideComponentName(ride->GetRideTypeDescriptor().NameConvention.vehicle).number);
                    ft.Add<uint16_t>(measurement->vehicle_index + 1);
                    return { fallback, ft };
                }

                return message;
            }
        }
        else
        {
            return { STR_NONE, {} };
        }
        return { fallback, {} };
    }

    void GraphsOnPrepareDraw()
    {
        auto newWidgets = window_ride_page_widgets[page];
        if (widgets != newWidgets)
        {
            widgets = newWidgets;
            InitScrollWidgets();
        }

        SetPressedTab();

        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        auto ft = Formatter::Common();
        ride->FormatNameTo(ft);

        // Set pressed graph button type
        pressed_widgets &= ~(1uLL << WIDX_GRAPH_VELOCITY);
        pressed_widgets &= ~(1uLL << WIDX_GRAPH_ALTITUDE);
        pressed_widgets &= ~(1uLL << WIDX_GRAPH_VERTICAL);
        pressed_widgets &= ~(1uLL << WIDX_GRAPH_LATERAL);
        pressed_widgets |= (1LL << (WIDX_GRAPH_VELOCITY + (list_information_type & 0xFF)));

        // Hide graph buttons that are not applicable
        if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_G_FORCES))
        {
            window_ride_graphs_widgets[WIDX_GRAPH_VERTICAL].type = WindowWidgetType::Button;
            window_ride_graphs_widgets[WIDX_GRAPH_LATERAL].type = WindowWidgetType::Button;
        }
        else
        {
            window_ride_graphs_widgets[WIDX_GRAPH_VERTICAL].type = WindowWidgetType::Empty;
            window_ride_graphs_widgets[WIDX_GRAPH_LATERAL].type = WindowWidgetType::Empty;
        }

        // Anchor graph widget
        auto x = width - 4;
        auto y = height - BUTTON_FACE_HEIGHT - 8;

        window_ride_graphs_widgets[WIDX_GRAPH].right = x;
        window_ride_graphs_widgets[WIDX_GRAPH].bottom = y;
        y += 3;
        window_ride_graphs_widgets[WIDX_GRAPH_VELOCITY].top = y;
        window_ride_graphs_widgets[WIDX_GRAPH_ALTITUDE].top = y;
        window_ride_graphs_widgets[WIDX_GRAPH_VERTICAL].top = y;
        window_ride_graphs_widgets[WIDX_GRAPH_LATERAL].top = y;
        y += BUTTON_FACE_HEIGHT + 1;
        window_ride_graphs_widgets[WIDX_GRAPH_VELOCITY].bottom = y;
        window_ride_graphs_widgets[WIDX_GRAPH_ALTITUDE].bottom = y;
        window_ride_graphs_widgets[WIDX_GRAPH_VERTICAL].bottom = y;
        window_ride_graphs_widgets[WIDX_GRAPH_LATERAL].bottom = y;

        AnchorBorderWidgets();
        WindowAlignTabs(this, WIDX_TAB_1, WIDX_TAB_10);
    }

    void GraphsOnDraw(DrawPixelInfo& dpi)
    {
        DrawWidgets(dpi);
        DrawTabImages(dpi);
    }

    void GraphsOnScrollDraw(DrawPixelInfo& dpi, int32_t scrollIndex)
    {
        GfxClear(&dpi, ColourMapA[COLOUR_SATURATED_GREEN].darker);

        auto widget = &window_ride_graphs_widgets[WIDX_GRAPH];
        auto ride = GetRide(rideId);
        if (ride == nullptr)
        {
            return;
        }

        auto [measurement, message] = ride->GetMeasurement();

        if (measurement == nullptr)
        {
            // No measurement message
            ScreenCoordsXY stringCoords(widget->width() / 2, widget->height() / 2 - 5);
            int32_t txtWidth = widget->width() - 2;
            DrawTextWrapped(dpi, stringCoords, txtWidth, message.str, message.args, { TextAlignment::CENTRE });
            return;
        }

        // Vertical grid lines
        const uint8_t lightColour = ColourMapA[COLOUR_SATURATED_GREEN].mid_light;
        const uint8_t darkColour = ColourMapA[COLOUR_SATURATED_GREEN].mid_dark;

        int32_t time = 0;
        for (int32_t x = 0; x < dpi.x + dpi.width; x += 80)
        {
            if (x + 80 >= dpi.x)
            {
                auto coord1 = ScreenCoordsXY{ x, dpi.y };
                auto coord2 = ScreenCoordsXY{ x, dpi.y + dpi.height - 1 };
                GfxFillRect(dpi, { coord1, coord2 }, lightColour);
                GfxFillRect(dpi, { coord1 + ScreenCoordsXY{ 16, 0 }, coord2 + ScreenCoordsXY{ 16, 0 } }, darkColour);
                GfxFillRect(dpi, { coord1 + ScreenCoordsXY{ 32, 0 }, coord2 + ScreenCoordsXY{ 32, 0 } }, darkColour);
                GfxFillRect(dpi, { coord1 + ScreenCoordsXY{ 48, 0 }, coord2 + ScreenCoordsXY{ 48, 0 } }, darkColour);
                GfxFillRect(dpi, { coord1 + ScreenCoordsXY{ 64, 0 }, coord2 + ScreenCoordsXY{ 64, 0 } }, darkColour);
            }
            time += 5;
        }

        // Horizontal grid lines
        int32_t listType = list_information_type & 0xFF;
        int16_t yUnit = window_graphs_y_axi[listType].unit;
        StringId stringID = window_graphs_y_axi[listType].label;
        int16_t yUnitInterval = window_graphs_y_axi[listType].unit_interval;
        int16_t yInterval = window_graphs_y_axi[listType].interval;

        // Scale modifier
        if (listType == GRAPH_ALTITUDE)
        {
            yUnit -= gMapBaseZ * 3;
        }

        for (int32_t y = widget->height() - 13; y >= 8; y -= yInterval, yUnit += yUnitInterval)
        {
            // Minor / major line
            int32_t colour = yUnit == 0 ? lightColour : darkColour;
            GfxFillRect(dpi, { { dpi.x, y }, { dpi.x + dpi.width - 1, y } }, colour);

            int16_t scaled_yUnit = yUnit;
            // Scale modifier
            if (listType == GRAPH_ALTITUDE)
                scaled_yUnit /= 2;

            auto ft = Formatter();
            ft.Add<int16_t>(scaled_yUnit);

            DrawTextBasic(dpi, { scrolls[0].h_left + 1, y - 4 }, stringID, ft, { FontStyle::Small });
        }

        // Time marks
        time = 0;
        for (int32_t x = 0; x < dpi.x + dpi.width; x += 80)
        {
            auto ft = Formatter();
            ft.Add<int32_t>(time);
            if (x + 80 >= dpi.x)
                DrawTextBasic(dpi, { x + 2, 1 }, STR_RIDE_STATS_TIME, ft, { FontStyle::Small });
            time += 5;
        }

        // Plot
        int32_t x = dpi.x;
        int32_t firstPoint, secondPoint;
        // Uses the force limits (used to draw extreme G's in red on measurement tab) to determine if line should be drawn red.
        int32_t intensityThresholdPositive = 0;
        int32_t intensityThresholdNegative = 0;
        for (int32_t graphWidth = 0; graphWidth < dpi.width; graphWidth++, x++)
        {
            if (x < 0 || x >= measurement->num_items - 1)
                continue;

            constexpr int32_t VerticalGraphHeightOffset = 39;
            constexpr int32_t LateralGraphHeightOffset = 52;

            switch (listType)
            {
                case GRAPH_VELOCITY:
                    firstPoint = measurement->velocity[x] / 2;
                    secondPoint = measurement->velocity[x + 1] / 2;
                    break;
                case GRAPH_ALTITUDE:
                    firstPoint = measurement->altitude[x];
                    secondPoint = measurement->altitude[x + 1];
                    break;
                case GRAPH_VERTICAL:
                    firstPoint = measurement->vertical[x] + VerticalGraphHeightOffset;
                    secondPoint = measurement->vertical[x + 1] + VerticalGraphHeightOffset;
                    intensityThresholdNegative = (RIDE_G_FORCES_RED_NEG_VERTICAL / 8) + VerticalGraphHeightOffset;
                    break;
                case GRAPH_LATERAL:
                    firstPoint = measurement->lateral[x] + LateralGraphHeightOffset;
                    secondPoint = measurement->lateral[x + 1] + LateralGraphHeightOffset;
                    intensityThresholdPositive = (RIDE_G_FORCES_RED_LATERAL / 8) + LateralGraphHeightOffset;
                    intensityThresholdNegative = -(RIDE_G_FORCES_RED_LATERAL / 8) + LateralGraphHeightOffset;
                    break;
                default:
                    LOG_ERROR("Wrong graph type %d", listType);
                    firstPoint = secondPoint = 0;
                    break;
            }

            // Adjust line to match graph widget position.
            firstPoint = widget->height() - firstPoint - 13;
            secondPoint = widget->height() - secondPoint - 13;
            if (firstPoint > secondPoint)
            {
                std::swap(firstPoint, secondPoint);
            }

            // Adjust threshold line position as well
            if (listType == GRAPH_VERTICAL || listType == GRAPH_LATERAL)
            {
                intensityThresholdPositive = widget->height() - intensityThresholdPositive - 13;
                intensityThresholdNegative = widget->height() - intensityThresholdNegative - 13;
            }

            const bool previousMeasurement = x > measurement->current_item;

            // Draw the current line in grey.
            GfxFillRect(
                dpi, { { x, firstPoint }, { x, secondPoint } }, previousMeasurement ? PALETTE_INDEX_17 : PALETTE_INDEX_21);

            // Draw red over extreme values (if supported by graph type).
            if (listType == GRAPH_VERTICAL || listType == GRAPH_LATERAL)
            {
                const auto redLineColour = previousMeasurement ? PALETTE_INDEX_171 : PALETTE_INDEX_173;

                // Line exceeds negative threshold (at bottom of graph).
                if (secondPoint >= intensityThresholdNegative)
                {
                    const auto redLineTop = ScreenCoordsXY{ x, std::max(firstPoint, intensityThresholdNegative) };
                    const auto redLineBottom = ScreenCoordsXY{ x, std::max(secondPoint, intensityThresholdNegative) };
                    GfxFillRect(dpi, { redLineTop, redLineBottom }, redLineColour);
                }

                // Line exceeds positive threshold (at top of graph).
                if (listType == GRAPH_LATERAL && firstPoint < intensityThresholdPositive)
                {
                    const auto redLineTop = ScreenCoordsXY{ x, std::min(firstPoint, intensityThresholdPositive) };
                    const auto redLineBottom = ScreenCoordsXY{ x, std::min(secondPoint, intensityThresholdPositive) };
                    GfxFillRect(dpi, { redLineTop, redLineBottom }, redLineColour);
                }
            }
        }
    }

#pragma endregion

#pragma region Income

    static void UpdateSamePriceThroughoutFlags(ShopItem shop_item)
    {
        if (GetShopItemDescriptor(shop_item).IsPhoto())
        {
            auto newFlags = gSamePriceThroughoutPark;
            if (gSamePriceThroughoutPark & EnumToFlag(shop_item))
                newFlags &= ~EnumsToFlags(ShopItem::Photo, ShopItem::Photo2, ShopItem::Photo3, ShopItem::Photo4);
            else
                newFlags |= EnumsToFlags(ShopItem::Photo, ShopItem::Photo2, ShopItem::Photo3, ShopItem::Photo4);
            auto parkSetParameter = ParkSetParameterAction(ParkParameter::SamePriceInPark, newFlags);
            GameActions::Execute(&parkSetParameter);
        }
        else
        {
            auto newFlags = gSamePriceThroughoutPark;
            newFlags ^= EnumToFlag(shop_item);
            auto parkSetParameter = ParkSetParameterAction(ParkParameter::SamePriceInPark, newFlags);
            GameActions::Execute(&parkSetParameter);
        }
    }

    void IncomeTogglePrimaryPrice()
    {
        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        ShopItem shop_item;
        const auto& rtd = ride->GetRideTypeDescriptor();
        if (rtd.HasFlag(RIDE_TYPE_FLAG_IS_TOILET))
        {
            shop_item = ShopItem::Admission;
        }
        else
        {
            auto rideEntry = GetRideEntryByIndex(ride->subtype);
            if (rideEntry != nullptr)
            {
                shop_item = rideEntry->shop_item[0];
                if (shop_item == ShopItem::None)
                    return;
            }
            else
            {
                return;
            }
        }

        UpdateSamePriceThroughoutFlags(shop_item);

        auto rideSetPriceAction = RideSetPriceAction(rideId, ride->price[0], true);
        GameActions::Execute(&rideSetPriceAction);
    }

    void IncomeToggleSecondaryPrice()
    {
        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        auto rideEntry = GetRideEntryByIndex(ride->subtype);
        if (rideEntry == nullptr)
            return;

        auto shop_item = rideEntry->shop_item[1];
        if (shop_item == ShopItem::None)
            shop_item = ride->GetRideTypeDescriptor().PhotoItem;

        UpdateSamePriceThroughoutFlags(shop_item);

        auto rideSetPriceAction = RideSetPriceAction(rideId, ride->price[1], false);
        GameActions::Execute(&rideSetPriceAction);
    }

    void IncomeSetPrimaryPrice(money64 price)
    {
        auto rideSetPriceAction = RideSetPriceAction(rideId, price, true);
        GameActions::Execute(&rideSetPriceAction);
    }

    void IncomeIncreasePrimaryPrice()
    {
        if (!IncomeCanModifyPrimaryPrice())
            return;

        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        auto price = ride->price[0];
        if (price < 20.00_GBP)
            price++;

        IncomeSetPrimaryPrice(price);
    }

    void IncomeDecreasePrimaryPrice()
    {
        if (!IncomeCanModifyPrimaryPrice())
            return;

        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        auto price = ride->price[0];
        if (price > 0.00_GBP)
            price--;

        IncomeSetPrimaryPrice(price);
    }

    money64 IncomeGetSecondaryPrice()
    {
        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return 0;

        return ride->price[1];
    }

    void IncomeSetSecondaryPrice(money64 price)
    {
        auto rideSetPriceAction = RideSetPriceAction(rideId, price, false);
        GameActions::Execute(&rideSetPriceAction);
    }

    bool IncomeCanModifyPrimaryPrice()
    {
        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return false;

        auto rideEntry = ride->GetRideEntry();
        const auto& rtd = ride->GetRideTypeDescriptor();
        return ParkRidePricesUnlocked() || rtd.HasFlag(RIDE_TYPE_FLAG_IS_TOILET)
            || (rideEntry != nullptr && rideEntry->shop_item[0] != ShopItem::None);
    }

    void IncomeIncreaseSecondaryPrice()
    {
        auto price = IncomeGetSecondaryPrice();

        if (price < 20.00_GBP)
            price++;

        IncomeSetSecondaryPrice(price);
    }

    void IncomeDecreaseSecondaryPrice()
    {
        auto price = IncomeGetSecondaryPrice();

        if (price > 0.00_GBP)
            price--;

        IncomeSetSecondaryPrice(price);
    }

    void IncomeOnMouseUp(WidgetIndex widgetIndex)
    {
        utf8 _moneyInputText[MONEY_STRING_MAXLENGTH] = {};

        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                Close();
                return;
            case WIDX_TAB_1:
            case WIDX_TAB_2:
            case WIDX_TAB_3:
            case WIDX_TAB_4:
            case WIDX_TAB_5:
            case WIDX_TAB_6:
            case WIDX_TAB_7:
            case WIDX_TAB_8:
            case WIDX_TAB_9:
            case WIDX_TAB_10:
                SetPage(widgetIndex - WIDX_TAB_1);
                break;
            case WIDX_PRIMARY_PRICE:
            {
                if (!IncomeCanModifyPrimaryPrice())
                    return;

                auto ride = GetRide(rideId);
                if (ride != nullptr)
                {
                    MoneyToString(ride->price[0], _moneyInputText, MONEY_STRING_MAXLENGTH, true);
                    WindowTextInputRawOpen(
                        this, WIDX_PRIMARY_PRICE, STR_ENTER_NEW_VALUE, STR_ENTER_NEW_VALUE, {}, _moneyInputText,
                        MONEY_STRING_MAXLENGTH);
                }
                break;
            }
            case WIDX_PRIMARY_PRICE_SAME_THROUGHOUT_PARK:
                IncomeTogglePrimaryPrice();
                break;
            case WIDX_SECONDARY_PRICE:
            {
                auto price64 = IncomeGetSecondaryPrice();

                MoneyToString(price64, _moneyInputText, MONEY_STRING_MAXLENGTH, true);
                WindowTextInputRawOpen(
                    this, WIDX_SECONDARY_PRICE, STR_ENTER_NEW_VALUE, STR_ENTER_NEW_VALUE, {}, _moneyInputText,
                    MONEY_STRING_MAXLENGTH);
            }
            break;
            case WIDX_SECONDARY_PRICE_SAME_THROUGHOUT_PARK:
                IncomeToggleSecondaryPrice();
                break;
        }
    }

    void IncomeResize()
    {
        WindowSetResize(*this, 316, 194, 316, 194);
    }

    void IncomeOnMouseDown(WidgetIndex widgetIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_PRIMARY_PRICE_INCREASE:
                IncomeIncreasePrimaryPrice();
                break;
            case WIDX_PRIMARY_PRICE_DECREASE:
                IncomeDecreasePrimaryPrice();
                break;
            case WIDX_SECONDARY_PRICE_INCREASE:
                IncomeIncreaseSecondaryPrice();
                break;
            case WIDX_SECONDARY_PRICE_DECREASE:
                IncomeDecreaseSecondaryPrice();
                break;
        }
    }

    void IncomeUpdate()
    {
        frame_no++;
        OnPrepareDraw();
        WidgetInvalidate(*this, WIDX_TAB_9);

        auto ride = GetRide(rideId);
        if (ride != nullptr && ride->window_invalidate_flags & RIDE_INVALIDATE_RIDE_INCOME)
        {
            ride->window_invalidate_flags &= ~RIDE_INVALIDATE_RIDE_INCOME;
            Invalidate();
        }
    }

    void IncomeOnTextInput(WidgetIndex widgetIndex, std::string_view text)
    {
        if ((widgetIndex != WIDX_PRIMARY_PRICE && widgetIndex != WIDX_SECONDARY_PRICE) || text.empty())
            return;

        std::string strText{ text };
        money64 price = StringToMoney(strText.c_str());
        if (price == MONEY64_UNDEFINED)
        {
            return;
        }

        price = std::clamp(price, 0.00_GBP, 20.00_GBP);

        if (widgetIndex == WIDX_PRIMARY_PRICE)
        {
            IncomeSetPrimaryPrice(price);
        }
        else
        {
            IncomeSetSecondaryPrice(price);
        }
    }

    void IncomeOnPrepareDraw()
    {
        auto newWidgets = window_ride_page_widgets[page];
        if (widgets != newWidgets)
        {
            widgets = newWidgets;
            InitScrollWidgets();
        }

        SetPressedTab();

        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        widgets[WIDX_TITLE].text = STR_ARG_18_STRINGID;

        auto ft = Formatter::Common();
        ft.Increment(18);
        ride->FormatNameTo(ft);

        auto rideEntry = ride->GetRideEntry();
        if (rideEntry == nullptr)
            return;

        // Primary item
        pressed_widgets &= ~(1uLL << WIDX_PRIMARY_PRICE_SAME_THROUGHOUT_PARK);
        disabled_widgets &= ~(1uLL << WIDX_PRIMARY_PRICE);

        window_ride_income_widgets[WIDX_PRIMARY_PRICE_LABEL].tooltip = STR_NONE;
        window_ride_income_widgets[WIDX_PRIMARY_PRICE].tooltip = STR_NONE;

        // If ride prices are locked, do not allow setting the price, unless we're dealing with a shop or toilet.
        const auto& rtd = ride->GetRideTypeDescriptor();
        if (!ParkRidePricesUnlocked() && rideEntry->shop_item[0] == ShopItem::None && !rtd.HasFlag(RIDE_TYPE_FLAG_IS_TOILET))
        {
            disabled_widgets |= (1uLL << WIDX_PRIMARY_PRICE);
            window_ride_income_widgets[WIDX_PRIMARY_PRICE_LABEL].tooltip = STR_RIDE_INCOME_ADMISSION_PAY_FOR_ENTRY_TIP;
            window_ride_income_widgets[WIDX_PRIMARY_PRICE].tooltip = STR_RIDE_INCOME_ADMISSION_PAY_FOR_ENTRY_TIP;
        }

        window_ride_income_widgets[WIDX_PRIMARY_PRICE_LABEL].text = STR_RIDE_INCOME_ADMISSION_PRICE;
        window_ride_income_widgets[WIDX_SECONDARY_PRICE_LABEL].text = STR_SHOP_ITEM_PRICE_LABEL_ON_RIDE_PHOTO;
        window_ride_income_widgets[WIDX_PRIMARY_PRICE_SAME_THROUGHOUT_PARK].type = WindowWidgetType::Empty;

        window_ride_income_widgets[WIDX_PRIMARY_PRICE].text = STR_BOTTOM_TOOLBAR_CASH;
        auto ridePrimaryPrice = RideGetPrice(*ride);
        ft.Rewind();
        ft.Add<money64>(ridePrimaryPrice);
        if (ridePrimaryPrice == 0)
            window_ride_income_widgets[WIDX_PRIMARY_PRICE].text = STR_FREE;

        ShopItem primaryItem = ShopItem::Admission;
        if (rtd.HasFlag(RIDE_TYPE_FLAG_IS_TOILET) || ((primaryItem = rideEntry->shop_item[0]) != ShopItem::None))
        {
            window_ride_income_widgets[WIDX_PRIMARY_PRICE_SAME_THROUGHOUT_PARK].type = WindowWidgetType::Checkbox;

            if (ShopItemHasCommonPrice(primaryItem))
                pressed_widgets |= (1uLL << WIDX_PRIMARY_PRICE_SAME_THROUGHOUT_PARK);

            window_ride_income_widgets[WIDX_PRIMARY_PRICE_LABEL].text = GetShopItemDescriptor(primaryItem).Naming.PriceLabel;
        }

        // Get secondary item
        auto secondaryItem = ride->GetRideTypeDescriptor().PhotoItem;
        if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO))
        {
            if ((secondaryItem = rideEntry->shop_item[1]) != ShopItem::None)
            {
                window_ride_income_widgets[WIDX_SECONDARY_PRICE_LABEL].text = GetShopItemDescriptor(secondaryItem)
                                                                                  .Naming.PriceLabel;
            }
        }

        if (secondaryItem == ShopItem::None)
        {
            // Hide secondary item widgets
            window_ride_income_widgets[WIDX_SECONDARY_PRICE_LABEL].type = WindowWidgetType::Empty;
            window_ride_income_widgets[WIDX_SECONDARY_PRICE].type = WindowWidgetType::Empty;
            window_ride_income_widgets[WIDX_SECONDARY_PRICE_INCREASE].type = WindowWidgetType::Empty;
            window_ride_income_widgets[WIDX_SECONDARY_PRICE_DECREASE].type = WindowWidgetType::Empty;
            window_ride_income_widgets[WIDX_SECONDARY_PRICE_SAME_THROUGHOUT_PARK].type = WindowWidgetType::Empty;
        }
        else
        {
            // Set same price throughout park checkbox
            pressed_widgets &= ~(1uLL << WIDX_SECONDARY_PRICE_SAME_THROUGHOUT_PARK);
            if (ShopItemHasCommonPrice(secondaryItem))
                pressed_widgets |= (1uLL << WIDX_SECONDARY_PRICE_SAME_THROUGHOUT_PARK);

            // Show widgets
            window_ride_income_widgets[WIDX_SECONDARY_PRICE_LABEL].type = WindowWidgetType::Label;
            window_ride_income_widgets[WIDX_SECONDARY_PRICE].type = WindowWidgetType::Spinner;
            window_ride_income_widgets[WIDX_SECONDARY_PRICE_INCREASE].type = WindowWidgetType::Button;
            window_ride_income_widgets[WIDX_SECONDARY_PRICE_DECREASE].type = WindowWidgetType::Button;
            window_ride_income_widgets[WIDX_SECONDARY_PRICE_SAME_THROUGHOUT_PARK].type = WindowWidgetType::Checkbox;

            // Set secondary item price
            window_ride_income_widgets[WIDX_SECONDARY_PRICE].text = STR_RIDE_SECONDARY_PRICE_VALUE;
            ft.Rewind();
            ft.Increment(10);
            ft.Add<money64>(ride->price[1]);
            if (ride->price[1] == 0)
                window_ride_income_widgets[WIDX_SECONDARY_PRICE].text = STR_FREE;
        }

        AnchorBorderWidgets();
        WindowAlignTabs(this, WIDX_TAB_1, WIDX_TAB_10);
    }

    void IncomeOnDraw(DrawPixelInfo& dpi)
    {
        StringId stringId;
        money64 profit;
        ShopItem primaryItem, secondaryItem;

        DrawWidgets(dpi);
        DrawTabImages(dpi);

        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        auto rideEntry = ride->GetRideEntry();
        if (rideEntry == nullptr)
            return;

        auto screenCoords = windowPos
            + ScreenCoordsXY{ window_ride_income_widgets[WIDX_PAGE_BACKGROUND].left + 4,
                              window_ride_income_widgets[WIDX_PAGE_BACKGROUND].top + 33 };

        // Primary item profit / loss per item sold
        primaryItem = rideEntry->shop_item[0];
        if (primaryItem != ShopItem::None)
        {
            profit = ride->price[0];

            stringId = STR_PROFIT_PER_ITEM_SOLD;
            profit -= GetShopItemDescriptor(primaryItem).Cost;
            if (profit < 0)
            {
                profit *= -1;
                stringId = STR_LOSS_PER_ITEM_SOLD;
            }

            auto ft = Formatter();
            ft.Add<money64>(profit);

            DrawTextBasic(dpi, screenCoords, stringId, ft);
        }
        screenCoords.y += 44;

        // Secondary item profit / loss per item sold
        secondaryItem = ride->GetRideTypeDescriptor().PhotoItem;
        if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO))
            secondaryItem = rideEntry->shop_item[1];

        if (secondaryItem != ShopItem::None)
        {
            profit = ride->price[1];

            stringId = STR_PROFIT_PER_ITEM_SOLD;
            profit -= GetShopItemDescriptor(secondaryItem).Cost;
            if (profit < 0)
            {
                profit *= -1;
                stringId = STR_LOSS_PER_ITEM_SOLD;
            }

            auto ft = Formatter();
            ft.Add<money64>(profit);

            DrawTextBasic(dpi, screenCoords, stringId, ft);
        }
        screenCoords.y += 18;

        // Income per hour
        if (ride->income_per_hour != MONEY64_UNDEFINED)
        {
            auto ft = Formatter();
            ft.Add<money64>(ride->income_per_hour);

            DrawTextBasic(dpi, screenCoords, STR_INCOME_PER_HOUR, ft);
            screenCoords.y += LIST_ROW_HEIGHT;
        }

        // Running cost per hour
        money64 costPerHour = ride->upkeep_cost * 16;
        stringId = ride->upkeep_cost == MONEY64_UNDEFINED ? STR_RUNNING_COST_UNKNOWN : STR_RUNNING_COST_PER_HOUR;
        auto ft = Formatter();
        ft.Add<money64>(costPerHour);
        DrawTextBasic(dpi, screenCoords, stringId, ft);
        screenCoords.y += LIST_ROW_HEIGHT;

        // Profit per hour
        if (ride->profit != MONEY64_UNDEFINED)
        {
            ft = Formatter();
            ft.Add<money64>(ride->profit);
            DrawTextBasic(dpi, screenCoords, STR_PROFIT_PER_HOUR, ft);
            screenCoords.y += LIST_ROW_HEIGHT;
        }
        screenCoords.y += 5;

        // Total profit
        ft = Formatter();
        ft.Add<money64>(ride->total_profit);
        DrawTextBasic(dpi, screenCoords, STR_TOTAL_PROFIT, ft);
    }

#pragma endregion

#pragma region Customer

    void CustomerOnMouseUp(WidgetIndex widgetIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                Close();
                return;
            case WIDX_TAB_1:
            case WIDX_TAB_2:
            case WIDX_TAB_3:
            case WIDX_TAB_4:
            case WIDX_TAB_5:
            case WIDX_TAB_6:
            case WIDX_TAB_7:
            case WIDX_TAB_8:
            case WIDX_TAB_9:
            case WIDX_TAB_10:
                SetPage(widgetIndex - WIDX_TAB_1);
                break;
            case WIDX_SHOW_GUESTS_THOUGHTS:
            {
                auto intent = Intent(WindowClass::GuestList);
                intent.PutExtra(
                    INTENT_EXTRA_GUEST_LIST_FILTER, static_cast<int32_t>(GuestListFilterType::GuestsThinkingAboutRide));
                intent.PutExtra(INTENT_EXTRA_RIDE_ID, number);
                ContextOpenIntent(&intent);
                break;
            }
            case WIDX_SHOW_GUESTS_ON_RIDE:
            {
                auto intent = Intent(WindowClass::GuestList);
                intent.PutExtra(INTENT_EXTRA_GUEST_LIST_FILTER, static_cast<int32_t>(GuestListFilterType::GuestsOnRide));
                intent.PutExtra(INTENT_EXTRA_RIDE_ID, number);
                ContextOpenIntent(&intent);
                break;
            }
            case WIDX_SHOW_GUESTS_QUEUING:
            {
                auto intent = Intent(WindowClass::GuestList);
                intent.PutExtra(INTENT_EXTRA_GUEST_LIST_FILTER, static_cast<int32_t>(GuestListFilterType::GuestsInQueue));
                intent.PutExtra(INTENT_EXTRA_RIDE_ID, number);
                ContextOpenIntent(&intent);
                break;
            }
        }
    }

    void CustomerResize()
    {
        flags |= WF_RESIZABLE;
        WindowSetResize(*this, 316, 163, 316, 163);
    }

    void CustomerUpdate()
    {
        picked_peep_frame++;
        if (picked_peep_frame >= 24)
            picked_peep_frame = 0;

        OnPrepareDraw();
        WidgetInvalidate(*this, WIDX_TAB_10);

        auto ride = GetRide(rideId);
        if (ride != nullptr && ride->window_invalidate_flags & RIDE_INVALIDATE_RIDE_CUSTOMER)
        {
            ride->window_invalidate_flags &= ~RIDE_INVALIDATE_RIDE_CUSTOMER;
            Invalidate();
        }
    }

    void CustomerOnPrepareDraw()
    {
        auto newWidgets = window_ride_page_widgets[page];
        if (widgets != newWidgets)
        {
            widgets = newWidgets;
            InitScrollWidgets();
        }

        SetPressedTab();

        auto ride = GetRide(rideId);
        if (ride != nullptr)
        {
            auto ft = Formatter::Common();
            ride->FormatNameTo(ft);

            window_ride_customer_widgets[WIDX_SHOW_GUESTS_THOUGHTS].type = WindowWidgetType::FlatBtn;
            if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_IS_SHOP_OR_FACILITY))
            {
                window_ride_customer_widgets[WIDX_SHOW_GUESTS_ON_RIDE].type = WindowWidgetType::Empty;
                window_ride_customer_widgets[WIDX_SHOW_GUESTS_QUEUING].type = WindowWidgetType::Empty;
            }
            else
            {
                window_ride_customer_widgets[WIDX_SHOW_GUESTS_ON_RIDE].type = WindowWidgetType::FlatBtn;
                window_ride_customer_widgets[WIDX_SHOW_GUESTS_QUEUING].type = WindowWidgetType::FlatBtn;
            }

            AnchorBorderWidgets();
            WindowAlignTabs(this, WIDX_TAB_1, WIDX_TAB_10);
        }
    }

    void CustomerOnDraw(DrawPixelInfo& dpi)
    {
        ShopItem shopItem;
        int16_t popularity, satisfaction, queueTime;
        StringId stringId;

        DrawWidgets(dpi);
        DrawTabImages(dpi);

        auto ride = GetRide(rideId);
        if (ride == nullptr)
            return;

        auto screenCoords = windowPos
            + ScreenCoordsXY{ window_ride_customer_widgets[WIDX_PAGE_BACKGROUND].left + 4,
                              window_ride_customer_widgets[WIDX_PAGE_BACKGROUND].top + 4 };

        // Customers currently on ride
        if (ride->IsRide())
        {
            auto ft = Formatter();
            ft.Add<int16_t>(ride->num_riders);
            DrawTextBasic(dpi, screenCoords, STR_CUSTOMERS_ON_RIDE, ft);
            screenCoords.y += LIST_ROW_HEIGHT;
        }

        // Customers per hour
        auto ft = Formatter();
        ft.Add<int32_t>(RideCustomersPerHour(*ride));
        DrawTextBasic(dpi, screenCoords, STR_CUSTOMERS_PER_HOUR, ft);
        screenCoords.y += LIST_ROW_HEIGHT;

        // Popularity
        popularity = ride->popularity;
        if (popularity == 255)
        {
            stringId = STR_POPULARITY_UNKNOWN;
        }
        else
        {
            stringId = STR_POPULARITY_PERCENT;
            popularity *= 4;
        }
        ft = Formatter();
        ft.Add<int16_t>(popularity);
        DrawTextBasic(dpi, screenCoords, stringId, ft);
        screenCoords.y += LIST_ROW_HEIGHT;

        // Satisfaction
        satisfaction = ride->satisfaction;
        if (satisfaction == 255)
        {
            stringId = STR_SATISFACTION_UNKNOWN;
        }
        else
        {
            stringId = STR_SATISFACTION_PERCENT;
            satisfaction *= 5;
        }
        ft = Formatter();
        ft.Add<int16_t>(satisfaction);
        DrawTextBasic(dpi, screenCoords, stringId, ft);
        screenCoords.y += LIST_ROW_HEIGHT;

        // Queue time
        if (ride->IsRide())
        {
            queueTime = ride->GetMaxQueueTime();
            stringId = queueTime == 1 ? STR_QUEUE_TIME_MINUTE : STR_QUEUE_TIME_MINUTES;
            ft = Formatter();
            ft.Add<int32_t>(queueTime);
            screenCoords.y += DrawTextWrapped(dpi, screenCoords, 308, stringId, ft, { TextAlignment::LEFT });
            screenCoords.y += 5;
        }

        // Primary shop items sold
        shopItem = ride->GetRideEntry()->shop_item[0];
        if (shopItem != ShopItem::None)
        {
            ft = Formatter();
            ft.Add<StringId>(GetShopItemDescriptor(shopItem).Naming.Plural);
            ft.Add<uint32_t>(ride->no_primary_items_sold);
            DrawTextBasic(dpi, screenCoords, STR_ITEMS_SOLD, ft);
            screenCoords.y += LIST_ROW_HEIGHT;
        }

        // Secondary shop items sold / on-ride photos sold
        shopItem = (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO) ? ride->GetRideTypeDescriptor().PhotoItem
                                                                          : ride->GetRideEntry()->shop_item[1];
        if (shopItem != ShopItem::None)
        {
            ft = Formatter();
            ft.Add<StringId>(GetShopItemDescriptor(shopItem).Naming.Plural);
            ft.Add<uint32_t>(ride->no_secondary_items_sold);
            DrawTextBasic(dpi, screenCoords, STR_ITEMS_SOLD, ft);
            screenCoords.y += LIST_ROW_HEIGHT;
        }

        // Total customers
        ft = Formatter();
        ft.Add<uint32_t>(ride->total_customers);
        DrawTextBasic(dpi, screenCoords, STR_TOTAL_CUSTOMERS, ft);
        screenCoords.y += LIST_ROW_HEIGHT;

        // Guests favourite
        if (ride->IsRide())
        {
            ft = Formatter();
            ft.Add<uint32_t>(ride->guests_favourite);
            stringId = ride->guests_favourite == 1 ? STR_FAVOURITE_RIDE_OF_GUEST : STR_FAVOURITE_RIDE_OF_GUESTS;
            DrawTextBasic(dpi, screenCoords, stringId, ft);
            screenCoords.y += LIST_ROW_HEIGHT;
        }
        screenCoords.y += 2;

        // Age
        // If the ride has a build date that is in the future, show it as built this year.
        int16_t age = std::max(DateGetYear(ride->GetAge()), 0);
        stringId = age == 0 ? STR_BUILT_THIS_YEAR : age == 1 ? STR_BUILT_LAST_YEAR : STR_BUILT_YEARS_AGO;
        ft = Formatter();
        ft.Add<int16_t>(age);
        DrawTextBasic(dpi, screenCoords, stringId, ft);
    }

#pragma endregion
};

/**
 *
 *  rct2: 0x006AEAB4
 */
static RideWindow* WindowRideOpen(const Ride& ride)
{
    return WindowCreate<RideWindow>(WindowClass::Ride, 316, 207, WF_10 | WF_RESIZABLE, ride);
}

/**
 *
 *  rct2: 0x006ACC28
 */
WindowBase* WindowRideMainOpen(const Ride& ride)
{
    if (ride.type >= RIDE_TYPE_COUNT)
    {
        return nullptr;
    }

    RideWindow* w = static_cast<RideWindow*>(WindowBringToFrontByNumber(WindowClass::Ride, ride.id.ToUnderlying()));
    if (w == nullptr)
    {
        w = WindowRideOpen(ride);
        w->SetViewIndex(0);
    }
    else if (w->GetViewIndex() >= (1 + ride.NumTrains + ride.num_stations))
    {
        w->SetViewIndex(0);
    }

    if (InputTestFlag(INPUT_FLAG_TOOL_ACTIVE))
    {
        if (w->classification == gCurrentToolWidget.window_classification && w->number == gCurrentToolWidget.window_number)
        {
            ToolCancel();
        }
    }

    if (w->page != WINDOW_RIDE_PAGE_MAIN)
    {
        w->SetPage(WINDOW_RIDE_PAGE_MAIN);
    }

    w->OnViewportRotate();
    return w;
}

/**
 *
 *  rct2: 0x006ACCCE
 */
static WindowBase* WindowRideOpenStation(const Ride& ride, StationIndex stationIndex)
{
    if (ride.type >= RIDE_TYPE_COUNT)
        return nullptr;

    if (ride.GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_NO_VEHICLES))
        return WindowRideMainOpen(ride);

    auto* w = static_cast<RideWindow*>(WindowBringToFrontByNumber(WindowClass::Ride, ride.id.ToUnderlying()));
    if (w == nullptr)
    {
        w = WindowRideOpen(ride);
    }

    if (InputTestFlag(INPUT_FLAG_TOOL_ACTIVE) && gCurrentToolWidget.window_classification == w->classification
        && gCurrentToolWidget.window_number == w->number)
    {
        ToolCancel();
    }

    // View
    for (int32_t i = stationIndex.ToUnderlying(); i >= 0; i--)
    {
        if (ride.GetStations()[i].Start.IsNull())
        {
            stationIndex = StationIndex::FromUnderlying(stationIndex.ToUnderlying() - 1);
        }
    }

    w->SetViewIndex(1 + ride.NumTrains + stationIndex.ToUnderlying());

    return w;
}

WindowBase* WindowRideOpenTrack(TileElement* tileElement)
{
    assert(tileElement != nullptr);
    auto rideIndex = tileElement->GetRideIndex();
    if (!rideIndex.IsNull())
    {
        auto ride = GetRide(rideIndex);
        if (ride != nullptr)
        {
            const auto type = tileElement->GetType();
            if (type == TileElementType::Entrance)
            {
                // Open ride window in station view
                auto entranceElement = tileElement->AsEntrance();
                auto stationIndex = entranceElement->GetStationIndex();
                return WindowRideOpenStation(*ride, stationIndex);
            }
            else if (type == TileElementType::Track)
            {
                // Open ride window in station view
                auto trackElement = tileElement->AsTrack();
                auto trackType = trackElement->GetTrackType();
                const auto& ted = GetTrackElementDescriptor(trackType);
                if (ted.SequenceProperties[0] & TRACK_SEQUENCE_FLAG_ORIGIN)
                {
                    auto stationIndex = trackElement->GetStationIndex();
                    return WindowRideOpenStation(*ride, stationIndex);
                }
            }

            // Open ride window in overview mode
            return WindowRideMainOpen(*ride);
        }
    }
    return nullptr;
}

/**
 *
 *  rct2: 0x006ACAC2
 */
WindowBase* WindowRideOpenVehicle(Vehicle* vehicle)
{
    if (vehicle == nullptr)
        return nullptr;

    Vehicle* headVehicle = vehicle->TrainHead();
    if (headVehicle == nullptr)
        return nullptr;

    EntityId headVehicleSpriteIndex = headVehicle->Id;
    auto ride = headVehicle->GetRide();
    if (ride == nullptr)
        return nullptr;

    // Get view index
    int16_t view = 1;
    for (int32_t i = 0; i <= OpenRCT2::Limits::MaxTrainsPerRide; i++)
    {
        if (ride->vehicles[i] == headVehicleSpriteIndex)
            break;

        view++;
    }

    auto* w = static_cast<RideWindow*>(WindowFindByNumber(WindowClass::Ride, ride->id.ToUnderlying()));
    if (w != nullptr)
    {
        w->Invalidate();

        if (InputTestFlag(INPUT_FLAG_TOOL_ACTIVE) && gCurrentToolWidget.window_classification == w->classification
            && gCurrentToolWidget.window_number == w->number)
        {
            ToolCancel();
        }

        int32_t openedPeepWindow = 0;
        if (w->GetViewIndex() == view)
        {
            int32_t numPeepsLeft = vehicle->num_peeps;
            for (int32_t i = 0; i < 32 && numPeepsLeft > 0; i++)
            {
                Peep* peep = GetEntity<Guest>(vehicle->peep[i]);
                if (peep == nullptr)
                    continue;

                numPeepsLeft--;
                WindowBase* w2 = WindowFindByNumber(WindowClass::Peep, vehicle->peep[i]);
                if (w2 == nullptr)
                {
                    auto intent = Intent(WindowClass::Peep);
                    intent.PutExtra(INTENT_EXTRA_PEEP, peep);
                    ContextOpenIntent(&intent);
                    openedPeepWindow = 1;

                    break;
                }
            }
        }

        w = static_cast<RideWindow*>(
            openedPeepWindow ? WindowFindByNumber(WindowClass::Ride, ride->id.ToUnderlying())
                             : WindowBringToFrontByNumber(WindowClass::Ride, ride->id.ToUnderlying()));
    }

    if (w == nullptr)
    {
        w = WindowRideOpen(*ride);
    }

    w->SetViewIndex(view);
    w->Invalidate();

    return w;
}

void WindowRideInvalidateVehicle(const Vehicle& vehicle)
{
    auto* w = static_cast<RideWindow*>(WindowFindByNumber(WindowClass::Ride, vehicle.ride.ToUnderlying()));
    if (w == nullptr)
        return;

    auto ride = vehicle.GetRide();
    auto viewVehicleIndex = w->GetViewIndex() - 1;
    if (ride == nullptr || viewVehicleIndex < 0 || viewVehicleIndex >= ride->NumTrains)
        return;

    if (vehicle.Id != ride->vehicles[viewVehicleIndex])
        return;

    w->Invalidate();
}

static void CancelScenerySelection()
{
    gGamePaused &= ~GAME_PAUSED_SAVING_TRACK;
    gTrackDesignSaveMode = false;
    OpenRCT2::Audio::Resume();

    WindowBase* main_w = WindowGetMain();
    if (main_w != nullptr)
    {
        main_w->viewport->flags &= ~(VIEWPORT_FLAG_HIDE_VERTICAL | VIEWPORT_FLAG_HIDE_BASE);
    }

    GfxInvalidateScreen();
    ToolCancel();
}

void WindowRideMeasurementsDesignCancel()
{
    if (gTrackDesignSaveMode)
    {
        CancelScenerySelection();
    }
}
