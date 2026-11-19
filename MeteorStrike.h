/**************************************************************************
 *     This file is part of MeteorStrike2023.

    I, Dick Hamill, the author of this program disclaim all copyright
    in order to make this program freely available in perpetuity to
    anyone who would like to use it. Dick Hamill, 6/1/2020

    MeteorStrike2023 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    MeteorStrike2023 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    See <https://www.gnu.org/licenses/>.
 */

#ifndef STERN_METEOR_1979_H
#define STERN_METEOR_1979_H

// Lamp Numbers (defines for lamps)
#define LAMP_BOTTOM_LEFT_BANK_SPECIAL  0
#define LAMP_LEFT_ROCKET_7K            1
#define LAMP_LEFT_ROCKET_6K            2
#define LAMP_LEFT_ROCKET_5K            3
#define LAMP_LEFT_ROCKET_4K            4
#define LAMP_LEFT_ROCKET_3K            5
#define LAMP_LEFT_ROCKET_2K            6
#define LAMP_LEFT_ROCKET_1K            7
#define LAMP_TOP_LEFT_DROP_TARGET_SPECIAL  8  
#define LAMP_MIDDLE_ROCKET_7K          9
#define LAMP_MIDDLE_ROCKET_6K          10
#define LAMP_MIDDLE_ROCKET_5K          11
#define LAMP_MIDDLE_ROCKET_4K          12
#define LAMP_MIDDLE_ROCKET_3K          13
#define LAMP_MIDDLE_ROCKET_2K          14
#define LAMP_MIDDLE_ROCKET_1K          15
#define LAMP_MIDDLE_ROLLOVER_AND_STANDUP_SPOT  16
#define LAMP_RIGHT_ROCKET_7K           17
#define LAMP_RIGHT_ROCKET_6K           18
#define LAMP_RIGHT_ROCKET_5K           19
#define LAMP_RIGHT_ROCKET_4K           20
#define LAMP_RIGHT_ROCKET_3K           21
#define LAMP_RIGHT_ROCKET_2K           22
#define LAMP_RIGHT_ROCKET_1K           23
#define LAMP_STANDUP_TARGET_SPECIAL    24
#define LAMP_RIGHT_BANK_SPECIAL        25
#define LAMP_METEOR_WOW_R              26
#define LAMP_METEOR_WOW_O              27
#define LAMP_METEOR_WOW_E_2            28
#define LAMP_METEOR_WOW_T              29
#define LAMP_METEOR_WOW_E_1            30
#define LAMP_METEOR_WOW_M              31
#define LAMP_LEFT_ROLLOVER_AND_RIGHT_INLANE  32             
#define LAMP_RIGHT_ROLLOVER_AND_LEFT_RETURN    33
#define LAMP_METEOR_DROP_R             34 
#define LAMP_METEOR_DROP_O             35
#define LAMP_METEOR_DROP_E_2           36
#define LAMP_METEOR_DROP_T             37
#define LAMP_METEOR_DROP_E_1           38
#define LAMP_METEOR_DROP_M             39
#define LAMP_SHOOT_AGAIN                       40
#define LAMP_RIGHT_OUT_LANE_AND_1_BUMPER       41            
#define LAMP_SPINNER_6                         42
#define LAMP_SPINNER_5                         43
#define LAMP_SPINNER_4                         44
#define LAMP_SPINNER_3                         45
#define LAMP_SPINNER_2                         46
#define LAMP_SPINNER_1                         47
#define LAMP_HIGH_SCORE_TO_DATE                48       
#define LAMP_LEFT_OUT_LANE_AND_3_BUMPER        49             
#define LAMP_GAME_OVER                     50
#define LAMP_TILT                          51
#define LAMP_STANDUP_TARGET_WOW            52
#define LAMP_RIGHT_BANK_WOW                53
#define LAMP_TOP_LEFT_BANK_WOW             54
#define LAMP_BOTTOM_LEFT_BANK_WOW          55
#define LAMP_BONUS_1X                  56
#define LAMP_BONUS_2X                  57
#define LAMP_BONUS_4X                  58
#define LAMP_MATCH                     59


// Defines for switches
#define SW_STAND_UP       3
#define SW_SPINNER        4
#define SW_CREDIT_RESET   5
#define SW_TILT           6
#define SW_SLAM           7
#define SW_RIGHT_3_TOP    8
#define SW_RIGHT_3_LOWER  9
#define SW_RIGHT_3_MID    10
#define SW_BUMPER_UPPER_RIGHT   11
#define SW_BUMPER_UPPER_LEFT    12
#define SW_THUMPER_BUMPER       13
#define SW_RIGHT_SLINGSHOT      14
#define SW_LEFT_SLINGSHOT       15
#define SW_TOP_LEFT_3_TOP       18
#define SW_TOP_LEFT_3_LOWER     19
#define SW_TOP_LEFT_3_MID       20
#define SW_LOWER_LEFT_3_TOP     21
#define SW_LOWER_LEFT_3_LOWER   22
#define SW_LOWER_LEFT_3_MID     23
#define SW_DROP_TARGET_R        26
#define SW_DROP_TARGET_O        27
#define SW_DROP_TARGET_E_2      28
#define SW_DROP_TARGET_T        29
#define SW_DROP_TARGET_E_1      30
#define SW_DROP_TARGET_M        31
#define SW_OUTHOLE              32
#define SW_RIGHT_OUT_LANE       33
#define SW_LEFT_OUT_LANE        34
#define SW_RIGHT_IN_LANE        35
#define SW_LEFT_IN_LANE         36
#define SW_UPPER_ROLLOVER       37
#define SW_MIDDLE_ROLLOVER      38
#define SW_LOWER_ROLLOVER       39

#define SW_COIN_1         1
#define SW_COIN_2         0
#define SW_COIN_3         2


// Defines for solenoids
#define SOL_RESET_1_BANK      6
#define SOL_RESET_2_BANK      2
#define SOL_RESET_3_BANK      8
#define SOL_RESET_METEOR_BANK 12
#define SOL_DROP_M            3
#define SOL_DROP_E_1          4
#define SOL_DROP_T            7
#define SOL_DROP_E_2          10
#define SOL_DROP_O            11
#define SOL_DROP_R            13
#define SOL_POP_BUMPER        1
#define SOL_KNOCKER           5
#define SOL_LEFT_SLING        0
#define SOL_RIGHT_SLING       9
#define SOL_OUTHOLE           14

// SWITCHES_WITH_TRIGGERS are for switches that will automatically
// activate a solenoid (like in the case of a chime that rings on a rollover)
// but SWITCHES_WITH_TRIGGERS are fully debounced before being activated
#define NUM_SWITCHES_WITH_TRIGGERS         3

// PRIORITY_SWITCHES_WITH_TRIGGERS are switches that trigger immediately
// (like for pop bumpers or slings) - they are not debounced completely
#define NUM_PRIORITY_SWITCHES_WITH_TRIGGERS 3

// Define automatic solenoid triggers (switch, solenoid, number of 1/120ths of a second to fire)
struct PlayfieldAndCabinetSwitch TriggeredSwitches[] = {
  { SW_THUMPER_BUMPER, SOL_POP_BUMPER, 4 },
  { SW_LEFT_SLINGSHOT, SOL_LEFT_SLING, 4 },
  { SW_RIGHT_SLINGSHOT, SOL_RIGHT_SLING, 4 },
};


#endif