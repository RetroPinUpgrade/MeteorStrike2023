/**************************************************************************
       This file is part of MeteorStrike2023

    I, Dick Hamill, the author of this program disclaim all copyright
    in order to make this program freely available in perpetuity to
    anyone who would like to use it. Dick Hamill, 12/1/2020

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

#define COINS_PER_CREDIT    1

#include "RPU_Config.h"
#include "RPU.h"
#include "MeteorStrike.h"
#include "OperatorMenus.h"
#include "DisplayHandler.h"
#include "AudioHandler.h"
#include "LampAnimations.h"
#include <EEPROM.h>

#define METEORSTRIKE2023_MAJOR_VERSION  2026
#define METEORSTRIKE2023_MINOR_VERSION  12
#define DEBUG_MESSAGES  0

/*********************************************************************
    Bugs & ToDo
    * Drain when game says "Orpheus is Incoming" makes it crash
    * Add difficulty adjustments


    Difficulty Adjustments
    * EB on/off (standup target & wow?)
    * Number of meteors before Orpheus
    * Number of spins for Russian help
    * Remember bonus multiplier
    * MeteorStormDescentSpeed
    * Ball save time
    * Goals for Wizard Mode


*********************************************************************/


/*********************************************************************

    Game specific code

*********************************************************************/

// MachineState
//  0 - Attract Mode
//  negative - self-test modes
//  positive - game play
boolean InOperatorMenu = false;
char MachineState = 0;
boolean MachineStateChanged = true;
#define MACHINE_STATE_ATTRACT         0
#define MACHINE_STATE_INIT_GAMEPLAY   1
#define MACHINE_STATE_INIT_NEW_BALL   2
#define MACHINE_STATE_NORMAL_GAMEPLAY 4
#define MACHINE_STATE_COUNTDOWN_BONUS 99
#define MACHINE_STATE_BALL_OVER       100
#define MACHINE_STATE_MATCH_MODE      110
#define MACHINE_STATE_DIAGNOSTICS     120

#define GAME_MODE_SKILL_SHOT                    0
#define GAME_MODE_UNSTRUCTURED_PLAY             2
#define GAME_MODE_METEOR_STORM_START            3
#define GAME_MODE_METEOR_STORM                  4
#define GAME_MODE_METEOR_STORM_FINISHED         5
#define GAME_MODE_WIZARD                        6
#define GAME_MODE_ORPHEUS_START                 7
#define GAME_MODE_ORPHEUS                       8
#define GAME_MODE_ORPHEUS_FRAGMENTED            9
#define GAME_MODE_ORPHEUS_FINISH                10
#define GAME_MODE_ORPHEUS_FAIL                  11

// Indices of EEPROM save locations
#define EEPROM_RPOS_INIT_PROOF_UL                 90
#define RPOS_INIT_PROOF                           0x4D533233

#define EEPROM_BALL_SAVE_BYTE           100
#define EEPROM_FREE_PLAY_BYTE           101
#define EEPROM_MUSIC_LEVEL_BYTE         102
#define EEPROM_SKILL_SHOT_BYTE          103
#define EEPROM_TILT_WARNING_BYTE        104
#define EEPROM_AWARD_OVERRIDE_BYTE      105
#define EEPROM_BALLS_OVERRIDE_BYTE      106
#define EEPROM_TOURNAMENT_SCORING_BYTE  107
#define EEPROM_SFX_VOLUME_BYTE          108
#define EEPROM_MUSIC_VOLUME_BYTE        109
#define EEPROM_SCROLLING_SCORES_BYTE    110
#define EEPROM_CALLOUTS_VOLUME_BYTE     111
#define EEPROM_ATTRACT_MODE_SOUND_BYTE  112
#define EEPROM_DIM_LEVEL_BYTE           113
#define EEPROM_MATCH_FEATURE_BYTE       114
#define EEPROM_CRB_HOLD_TIME            118
#define EEPROM_GAME_RULES_SELECTION     137
#define EEPROM_EXTRA_BALL_SCORE_UL      140
#define EEPROM_SPECIAL_SCORE_UL         144

#define SOUND_EFFECT_NONE                     0
#define SOUND_EFFECT_SPINNER                  1
#define SOUND_EFFECT_UNLIT_ROLLOVER           2
#define SOUND_EFFECT_LEFT_INLANE              3
#define SOUND_EFFECT_LEFT_OUTLANE             4
#define SOUND_EFFECT_RIGHT_INLANE             5
#define SOUND_EFFECT_RIGHT_OUTLANE            6
#define SOUND_EFFECT_LAMP_METEOR_DROP_1            7
#define SOUND_EFFECT_LAMP_METEOR_DROP_2            8
#define SOUND_EFFECT_LAMP_METEOR_DROP_3            9
#define SOUND_EFFECT_LAMP_METEOR_DROP_4            10
#define SOUND_EFFECT_LAMP_METEOR_DROP_5            11
#define SOUND_EFFECT_LAMP_METEOR_DROP_6            12
#define SOUND_EFFECT_ADD_CREDIT               13
#define SOUND_EFFECT_THUMPER_BUMPER_HIT       14
#define SOUND_EFFECT_SHOOT_AGAIN              15
#define SOUND_EFFECT_MISSION_FAILED           16
#define SOUND_EFFECT_ROCKET_1_UNLIT           17
#define SOUND_EFFECT_FRAGMENT_ELIMINATED      18
#define SOUND_EFFECT_JACKPOT                  19
#define SOUND_EFFECT_ADD_PLAYER_1             20
#define SOUND_EFFECT_ADD_PLAYER_2             SOUND_EFFECT_ADD_PLAYER_1+1
#define SOUND_EFFECT_ADD_PLAYER_3             SOUND_EFFECT_ADD_PLAYER_1+2
#define SOUND_EFFECT_ADD_PLAYER_4             SOUND_EFFECT_ADD_PLAYER_1+3
#define SOUND_EFFECT_PLAYER_1_UP              24
#define SOUND_EFFECT_PLAYER_2_UP              SOUND_EFFECT_PLAYER_1_UP+1
#define SOUND_EFFECT_PLAYER_3_UP              SOUND_EFFECT_PLAYER_1_UP+2
#define SOUND_EFFECT_PLAYER_4_UP              SOUND_EFFECT_PLAYER_1_UP+3
#define SOUND_EFFECT_NICE_SHOT                28
#define SOUND_EFFECT_WIZARD_SCORING           29
#define SOUND_EFFECT_BALL_OVER                30
#define SOUND_EFFECT_GAME_OVER                31
#define SOUND_EFFECT_BONUS_COUNT              32
#define SOUND_EFFECT_2X_BONUS_COUNT           33
#define SOUND_EFFECT_3X_BONUS_COUNT           34
#define SOUND_EFFECT_4X_BONUS_COUNT           35
#define SOUND_EFFECT_5X_BONUS_COUNT           36
#define SOUND_EFFECT_6X_BONUS_COUNT           37
#define SOUND_EFFECT_7X_BONUS_COUNT           38
#define SOUND_EFFECT_SUPER_SPINNER            39
#define SOUND_EFFECT_EXTRA_BALL               40
#define SOUND_EFFECT_TILT_WARNING             41
#define SOUND_EFFECT_MATCH_SPIN               42
#define SOUND_EFFECT_SCORE_TICK               42
#define SOUND_EFFECT_SUPER_JACKPOT            43
#define SOUND_EFFECT_1_TARGET_HIT             44
#define SOUND_EFFECT_2_TARGET_HIT             45
#define SOUND_EFFECT_3_TARGET_HIT             46
#define SOUND_EFFECT_WOW_HIT                  47
#define SOUND_EFFECT_SPECIAL_HIT              48
#define SOUND_EFFECT_STANDUP_UNLIT            49
#define SOUND_EFFECT_SKILL_SHOT               50
#define SOUND_EFFECT_SUPER_SKILL_SHOT         51
#define SOUND_EFFECT_ROCKET_3_UNLIT           53
#define SOUND_EFFECT_SLING1                   54
#define SOUND_EFFECT_SLING2                   55
#define SOUND_EFFECT_SLING3                   56
#define SOUND_EFFECT_METEOR_STORM_START       60
#define SOUND_EFFECT_METEOR_STORM_SECTOR_1    61
#define SOUND_EFFECT_METEOR_STORM_SECTOR_2    62
#define SOUND_EFFECT_METEOR_STORM_SECTOR_1_2  63
#define SOUND_EFFECT_METEOR_STORM_SECTOR_3    64
#define SOUND_EFFECT_METEOR_STORM_SECTOR_3_1  65
#define SOUND_EFFECT_METEOR_STORM_SECTOR_3_2  66
#define SOUND_EFFECT_METEOR_STORM_SECTOR_3_2_1  67
#define SOUND_EFFECT_METEOR_IMPACT            68
#define SOUND_EFFECT_METEOR_HIT               69
#define SOUND_EFFECT_ROCKET_1_ARMED           70
#define SOUND_EFFECT_ROCKET_2_ARMED           71
#define SOUND_EFFECT_ROCKET_3_ARMED           72
#define SOUND_EFFECT_ROCKET_LAUNCHED          73
#define SOUND_EFFECT_ROCKET_1_MISSED          74
#define SOUND_EFFECT_ROCKET_2_MISSED          75
#define SOUND_EFFECT_ROCKET_3_MISSED          76
#define SOUND_EFFECT_TILT                     77
#define SOUND_EFFECT_1_METEOR_REMAINING       78
#define SOUND_EFFECT_2_METEORS_REMAINING      79
#define SOUND_EFFECT_3_METEORS_REMAINING      80
#define SOUND_EFFECT_ORPHEUS_START            81
#define SOUND_EFFECT_ORPHEUS_FRAGMENTED       82
#define SOUND_EFFECT_ORPHEUS_HIT_1            83
#define SOUND_EFFECT_ORPHEUS_HIT_2            84
#define SOUND_EFFECT_METEOR_STORM_START_2     85
#define SOUND_EFFECT_METEOR_STORM_START_3     86
#define SOUND_EFFECT_METEOR_STORM_START_4     87
#define SOUND_EFFECT_GAME_START_INTRO         88
#define SOUND_EFFECT_METEOR_INTRO             89
#define SOUND_EFFECT_BACKGROUND_1             90
#define SOUND_EFFECT_BACKGROUND_2             91
#define SOUND_EFFECT_BACKGROUND_3             92
#define SOUND_EFFECT_BACKGROUND_4             93
#define SOUND_EFFECT_BACKGROUND_5             94
#define SOUND_EFFECT_BACKGROUND_6             95
#define SOUND_EFFECT_WIZARD_MODE              96
#define SOUND_EFFECT_BACKGROUND_ORPHEUS       97
#define SOUND_EFFECT_ORPHEUS_FINISH           98

#define SOUND_EFFECT_VP_GAME_RULES_EASY               423
#define SOUND_EFFECT_VP_GAME_RULES_MEDIUM             424
#define SOUND_EFFECT_VP_GAME_RULES_HARD               425



// Defines for scorings
#define SCORE_ORPHEUS_TARGET_WOW            50000
#define SCORE_ORPHEUS_TARGET_NORMAL         2000
#define SCORE_BONUS_LAMP                    1000
#define SCORE_WIZARD_SWITCH_HIT             20000
#define SCORE_SKILL_SHOT                    25000
#define SCORE_SUPER_SKILL_SHOT              100000
#define SCORE_ROLLOVER_AND_INLANE           500
#define SCORE_OUTLANE                       1000
#define SCORE_SPINNER_VAL_ADD_200           200
#define SCORE_SPINNER_VAL_ADD_600           600
#define SCORE_SPINNER_SPIN                  200
#define SCORE_SPINNER_SUPER_SPIN            800
#define SCORE_SPINNER_SUPER_SPIN_MULTIPLIER 4
#define SCORE_THUMPER_BUMPER                1000
#define SCORE_BUMPER                        500
#define SCORE_BUMPER_BONUS                  1000
#define SCORE_SLINGSHOT                     10
#define SCORE_STANDUP_TARGET_UNLIT          500
#define SCORE_DROP_TARGET_6000              6000
#define SCORE_DROP_TARGET_2000              2000
#define SCORE_METEOR_HIT_REWARD             8000
#define SCORE_JACKPOT                       100000
#define SCORE_SUPER_JACKPOT                 200000

#define SKILL_SHOT_DURATION             15
#define MAX_DISPLAY_BONUS               7
#define TILT_WARNING_DEBOUNCE_TIME      1000
#define BONUS_UNDERLIGHTS_OFF           0
#define BONUS_UNDERLIGHTS_DIM           1
#define BONUS_UNDERLIGHTS_FULL          2
#define ATTRACT_MODE_SOUND_DELAY_MS     330000        

#define MODE_QUALIFY_TIME               45000
#define SPINNER_ANIMATION_DURATION      3000
#define DROP_SWITCH_BLACKOUT_PERIOD     1000
#define ROCKET_ANIMATION_DURATION       2000
#define METEOR_STORM_START_DURATION     6500
#define METEOR_STORM_TIME_TO_ARM        15000
#define ORPHEUS_START_DURATION          8000
#define ORPHEUS_DURATION                60000
#define ORPHEUS_FRAGMENTED_DURATION     30000
#define ORPHEUS_FAIL_DURATION           5000
#define ORPHEUS_FINISH_DURATION_SHORT   10000  
#define ORPHEUS_FINISH_DURATION_LONG    20000  
#define METEOR_HIT_SHORT_DISPLAY        3000
#define METEOR_HIT_LONG_DISPLAY         6000
#define SUPER_LAMP_SPINNER_DURATION          40000
#define WIZARD_MODE_DURATION            60000

#define SPOT_METEOR_TARGET_LIGHT_1            0x01
#define SPOT_METEOR_TARGET_LIGHT_2            0x02
#define SPOT_METEOR_TARGET_LIGHT_3            0x04

#define ROCKET_1_COLLECT_LIGHT                0x01
#define ROCKET_3_COLLECT_LIGHT                0x02

#define METEOR_STORM_SECTOR_1           0x01
#define METEOR_STORM_SECTOR_2           0x02
#define METEOR_STORM_SECTOR_3           0x04

#define GOAL_ORPHEUS_COMPLETED          0x01
#define GOAL_ORPHEUS_FRAGMENT_COMPLETED 0x02
#define GOAL_BONUS_MAXED_OUT            0x04
#define GOAL_SUPER_LAMP_SPINNER_ACHIEVED     0x08
#define GOAL_WIZARD_MODE_COLLECTED      0x10

#define AWARD_SCORING_ORPHEUS           1
#define AWARD_SCORING_WIZARD            2

// These defines dictate the ways rockets can be launched
#define LAUNCH_ALL_ROCKETS_WITH_STANDUP_UNTIL_LEVEL   1
#define LAUNCH_ROCKETS_WITH_BANK_CLEAR_UNTIL_LEVEL    2
#define LAUNCH_ROCKETS_WITH_DEAD_BUMPER_UNTIL_LEVEL   4

#define BALL_SAVE_GRACE_PERIOD            3000

/*********************************************************************

    Machine state and options

*********************************************************************/
unsigned long HighScore = 0;
unsigned long AwardScores[3];
byte Credits = 0;
byte ChuteCoinsInProgress[3] = {0, 0, 0};
boolean FreePlayMode = false;
byte MusicLevel = 3;
byte BallSaveNumSeconds = 0;
unsigned long ExtraBallValue = 0;
unsigned long SpecialValue = 0;
unsigned long CurrentTime = 0;
byte MaximumCredits = 40;
byte BallsPerGame = 3;
byte DimLevel = 2;
byte ScoreAwardReplay = 0;
byte MusicVolume = 6;
byte SoundEffectsVolume = 8;
byte CalloutsVolume = 10;
byte TotalBallsLoaded = 1;
byte TimeRequiredToResetGame = 1;
boolean HighScoreReplay = true;
boolean MatchFeature = true;
boolean TournamentScoring = false;
boolean AttractModeSound = false;
unsigned long LastTimeBallServed;

#ifdef RPU_SIMPLIFY_DISPLAY_FOR_7VOLUTION
boolean ScrollingScores = false;
#else
boolean ScrollingScores = true;
#endif

unsigned long CreditResetPressStarted = 0;
unsigned long OperatorSwitchPressStarted = 0;

#define NUM_CPC_PAIRS 9
boolean CPCSelectionsHaveBeenRead = false;
byte CPCPairs[NUM_CPC_PAIRS][2] = {
  {1, 5},
  {1, 4},
  {1, 3},
  {1, 2},
  {1, 1},
  {2, 3},
  {2, 1},
  {3, 1},
  {4, 1}
};
byte CPCSelection[3];

AudioHandler  Audio;
OperatorMenus Menus;



/*********************************************************************

    Game State

*********************************************************************/
byte CurrentPlayer = 0;
byte CurrentBallInPlay = 1;
byte CurrentNumPlayers = 0;
byte Bonus[3];
byte BonusX[4];
byte GameMode = GAME_MODE_SKILL_SHOT;
byte MaxTiltWarnings = 2;
byte NumTiltWarnings = 0;
byte GameRulesSelection;

boolean SamePlayerShootsAgain = false;
boolean BallSaveUsed = false;
boolean CurrentlyShowingBallSave = false;
boolean ExtraBallCollected = false;
boolean SpecialCollected = false;
boolean ShowingModeStats = false;

unsigned long CurrentScores[4];
byte CurrentAchievements[4];
unsigned long BallFirstSwitchHitTime = 0;
unsigned long BallTimeInTrough = 0;
unsigned long GameModeStartTime = 0;
unsigned long GameModeEndTime = 0;
unsigned long LastTiltWarningTime = 0;
unsigned long BallSaveEndTime;



int LastReportedValue = 0;

byte SkillShotWowLit = 0;
byte StandupAward = 0; 
byte StandupHits = 0;
byte SpinnerValue = 0;
byte SpinnerHitPhase = 11;
byte MeteorStatus[4];
byte CurrentMeteorTargetsValid = 0;
byte Current3BankTargetsHit[3]; 
byte MeteorSolenoid[6] = {13, 11, 10, 7, 4, 3};
byte BonusAnimationShown[3];
byte SpotMeteorTargetByte = 0x00;
byte CollectRocketsByte = 0x00;
byte CollectRocketCurrentLevel = 0;
byte MeteorStormSectors = 0;
byte MeteorStormDescent[3] = {0, 0, 0};
byte MeteorStormDescentSpeed[3] = {0, 0, 0};
byte RocketsArmed = 0;
byte NumberOfMeteorsHit[4];
byte OrpheusPhase;
byte OrpheusRound[4];
byte AwardScoring = 0;
byte TargetAwardPhase = 0;
byte TargetAward = 0;
byte NumberOfSpins[4];
byte GoalsCompletedFlags[4];
byte NumberOfBallsLocked;
byte NumberOfBallsInPlay;
byte NumberOfBallSavesRemaining;

boolean SkillShotHit;
boolean JackpotLit = false;
boolean SpotMeteorReverse = false;
boolean RocketsFiredFromBank = false;
boolean MeteorHitStarted = false;

unsigned long LastTimeSpinnerHit = 0;
unsigned long BonusAnimationStart[3];
unsigned long BonusXAnimationStart = 0;
unsigned long Rocket1BumperAnimationTime = 0;
unsigned long Rocket3BumperAnimationTime = 0;
unsigned long StandupResetTime = 0;
unsigned long CollectRocketAnimationStart = 0;
unsigned long LastTimeRocketBonusPlayed = 0; 
unsigned long MeteorStormStart = 0;
unsigned long RocketLaunchedTime[3] = {0, 0, 0};
unsigned long LastMeteorCheckTime = 0;
unsigned long MeteorHitStartTime = 0;
unsigned long MeteorHitEndTime = 0;
unsigned long LastOrpheusHitTime = 0;
unsigned long SuperSpinnerEndTime = 0;


#define GAME_RULES_EASY         1
#define GAME_RULES_MEDIUM       2
#define GAME_RULES_HARD         3
#define GAME_RULES_PROGRESSIVE  4
#define GAME_RULES_CUSTOM       5


/******************************************************

   Adjustments Serialization

*/
void SetAllParameterDefaults() {

  // In the event that the EEPROM has not been initialized,
  // these are the values that will be used
  HighScore = 10000;
  Credits = 4;
  FreePlayMode = false;
  BallSaveNumSeconds = 15;
  MusicVolume = 10;
  SoundEffectsVolume = 10;
  CalloutsVolume = 10;
  AwardScores[0] = 1000000;
  AwardScores[1] = 3000000;
  AwardScores[2] = 5000000;
  TournamentScoring = false;
  MaxTiltWarnings = 2;
  ScoreAwardReplay = 0x07;
  BallsPerGame = 3;
  ScrollingScores = true;
  MatchFeature = true;
  ExtraBallValue = 20000;
  SpecialValue = 40000;
  TimeRequiredToResetGame = 2;
  CPCSelection[0] = 4;
  CPCSelection[1] = 4;
  CPCSelection[2] = 4;

  // EASY / MEDIUM / HARD rules
  GameRulesSelection = GAME_RULES_MEDIUM;

  AttractModeSound = true;
  DimLevel = 2;
}


// CUSTOMIZATION TODO:
// Load in defaults for easy / medium / hard rules
boolean LoadRuleDefaults(byte ruleLevel) {
  // This function just puts the rules in RAM.
  // It's up to the caller to ensure that they're 
  // written to EEPROM when desired (with WriteParameters)
  if (ruleLevel==GAME_RULES_EASY) {
    BallSaveNumSeconds = 20;
    // Game rules variables initialized to EASY here:
  } else if (ruleLevel==GAME_RULES_MEDIUM) {
    BallSaveNumSeconds = 10;
    // Game rules variables initialized to MEDIUM here:
  } else if (ruleLevel==GAME_RULES_HARD) {
    BallSaveNumSeconds = 0;
    // Game rules variables initialized to HARD here:
  } else {
    return false;
  }

  return true;
}


void WriteParameters(boolean onlyWriteRulesParameters = true) {
  if (!onlyWriteRulesParameters) {
    RPU_WriteULToEEProm(RPU_HIGHSCORE_EEPROM_START_BYTE, HighScore);
    RPU_WriteByteToEEProm(RPU_CREDITS_EEPROM_BYTE, Credits);
    RPU_WriteByteToEEProm(RPU_CPC_CHUTE_1_SELECTION_BYTE, CPCSelection[0]);
    RPU_WriteByteToEEProm(RPU_CPC_CHUTE_2_SELECTION_BYTE, CPCSelection[1]);
    RPU_WriteByteToEEProm(RPU_CPC_CHUTE_3_SELECTION_BYTE, CPCSelection[2]);
        
    RPU_WriteByteToEEProm(EEPROM_FREE_PLAY_BYTE, FreePlayMode);
    RPU_WriteByteToEEProm(EEPROM_MUSIC_VOLUME_BYTE, MusicVolume);
    RPU_WriteByteToEEProm(EEPROM_SFX_VOLUME_BYTE, SoundEffectsVolume);
    RPU_WriteByteToEEProm(EEPROM_CALLOUTS_VOLUME_BYTE, CalloutsVolume);

    RPU_WriteULToEEProm(RPU_AWARD_SCORE_1_EEPROM_START_BYTE, AwardScores[0]);
    RPU_WriteULToEEProm(RPU_AWARD_SCORE_2_EEPROM_START_BYTE, AwardScores[1]);
    RPU_WriteULToEEProm(RPU_AWARD_SCORE_3_EEPROM_START_BYTE, AwardScores[2]);

    RPU_WriteByteToEEProm(EEPROM_TOURNAMENT_SCORING_BYTE, TournamentScoring);
    RPU_WriteByteToEEProm(EEPROM_AWARD_OVERRIDE_BYTE, ScoreAwardReplay);
    RPU_WriteByteToEEProm(EEPROM_BALLS_OVERRIDE_BYTE, BallsPerGame);
    RPU_WriteByteToEEProm(EEPROM_SCROLLING_SCORES_BYTE, ScrollingScores);
    RPU_WriteByteToEEProm(EEPROM_MATCH_FEATURE_BYTE, MatchFeature);
    
    RPU_WriteULToEEProm(EEPROM_EXTRA_BALL_SCORE_UL, ExtraBallValue);
    RPU_WriteULToEEProm(EEPROM_SPECIAL_SCORE_UL, SpecialValue);
    RPU_WriteByteToEEProm(EEPROM_CRB_HOLD_TIME, TimeRequiredToResetGame);

    // Set baseline for audits
    RPU_WriteByteToEEProm(RPU_CHUTE_1_COINS_START_BYTE, 0);
    RPU_WriteByteToEEProm(RPU_CHUTE_2_COINS_START_BYTE, 0);
    RPU_WriteByteToEEProm(RPU_CHUTE_3_COINS_START_BYTE, 0);
    RPU_WriteULToEEProm(RPU_TOTAL_PLAYS_EEPROM_START_BYTE, 0);
    RPU_WriteULToEEProm(RPU_TOTAL_REPLAYS_EEPROM_START_BYTE, 0);
    RPU_WriteULToEEProm(RPU_TOTAL_HISCORE_BEATEN_START_BYTE, 0);

    RPU_WriteByteToEEProm(EEPROM_ATTRACT_MODE_SOUND_BYTE, AttractModeSound);
  }

  RPU_WriteByteToEEProm(EEPROM_BALL_SAVE_BYTE, BallSaveNumSeconds);
  RPU_WriteByteToEEProm(EEPROM_TILT_WARNING_BYTE, MaxTiltWarnings); 
  
}

void ReadStoredParameters() {
  for (byte count = 0; count < 3; count++) {
    ChuteCoinsInProgress[count] = 0;
  }

// The first time the EEPROM has been written with good values for this game,
  // the EEPROM_RPOS_INIT_PROOF_UL will be written to a known state (RPOS_INIT_PROOF)
  // if that value hasn't been written, then we load defaults and save them to EEPROM.
  // This should only happen the first time a device is run with this game code.
  unsigned long RPUProofValue = RPU_ReadULFromEEProm(EEPROM_RPOS_INIT_PROOF_UL, 0);
  if (RPUProofValue!=RPOS_INIT_PROOF) {
    // Doesn't look like this memory has been initialized
    RPU_WriteULToEEProm(EEPROM_RPOS_INIT_PROOF_UL, RPOS_INIT_PROOF);
    SetAllParameterDefaults();
    WriteParameters(false);
  } else {
    // Read machine settings
    HighScore = RPU_ReadULFromEEProm(RPU_HIGHSCORE_EEPROM_START_BYTE, 10000);
    Credits = RPU_ReadByteFromEEProm(RPU_CREDITS_EEPROM_BYTE);
    if (Credits > MaximumCredits) Credits = MaximumCredits;
  
    FreePlayMode = ReadSetting(EEPROM_FREE_PLAY_BYTE, false, true);
    MusicVolume = ReadSetting(EEPROM_MUSIC_VOLUME_BYTE, 10, 10);
    SoundEffectsVolume = ReadSetting(EEPROM_SFX_VOLUME_BYTE, 10, 10);
    CalloutsVolume = ReadSetting(EEPROM_CALLOUTS_VOLUME_BYTE, 10, 10);
    Audio.SetMusicVolume(MusicVolume);
    Audio.SetSoundFXVolume(SoundEffectsVolume);
    Audio.SetNotificationsVolume(CalloutsVolume);

    AwardScores[0] = RPU_ReadULFromEEProm(RPU_AWARD_SCORE_1_EEPROM_START_BYTE);
    AwardScores[1] = RPU_ReadULFromEEProm(RPU_AWARD_SCORE_2_EEPROM_START_BYTE);
    AwardScores[2] = RPU_ReadULFromEEProm(RPU_AWARD_SCORE_3_EEPROM_START_BYTE);
  
    TournamentScoring = ReadSetting(EEPROM_TOURNAMENT_SCORING_BYTE, false, true);
    ScoreAwardReplay = ReadSetting(EEPROM_AWARD_OVERRIDE_BYTE, 0x03, 0x07);
    BallsPerGame = ReadSetting(EEPROM_BALLS_OVERRIDE_BYTE, 3, 10);
    ScrollingScores = ReadSetting(EEPROM_SCROLLING_SCORES_BYTE, true, true);
#ifdef RPU_SIMPLIFY_DISPLAY_FOR_7VOLUTION
    ScrollingScores = false;
#endif
    MatchFeature = ReadSetting(EEPROM_MATCH_FEATURE_BYTE, true, true);

    CPCSelection[0] = ReadSetting(RPU_CPC_CHUTE_1_SELECTION_BYTE, 4, 8);
    CPCSelection[1] = ReadSetting(RPU_CPC_CHUTE_2_SELECTION_BYTE, 4, 8);
    CPCSelection[2] = ReadSetting(RPU_CPC_CHUTE_3_SELECTION_BYTE, 4, 8);
    CPCSelectionsHaveBeenRead = true;

    ExtraBallValue = RPU_ReadULFromEEProm(EEPROM_EXTRA_BALL_SCORE_UL);
    if (ExtraBallValue % 1000 || ExtraBallValue > 1000000) ExtraBallValue = 20000;
  
    SpecialValue = RPU_ReadULFromEEProm(EEPROM_SPECIAL_SCORE_UL);
    if (SpecialValue % 1000 || SpecialValue > 1000000) SpecialValue = 40000;
  
    TimeRequiredToResetGame = ReadSetting(EEPROM_CRB_HOLD_TIME, 1, 99);
    if (TimeRequiredToResetGame > 3 && TimeRequiredToResetGame != 99) TimeRequiredToResetGame = 1;
    
    // Read game rules
    GameRulesSelection = ReadSetting(EEPROM_GAME_RULES_SELECTION, GAME_RULES_MEDIUM, GAME_RULES_CUSTOM);

    BallSaveNumSeconds = ReadSetting(EEPROM_BALL_SAVE_BYTE, 15, 20);  
    MaxTiltWarnings = ReadSetting(EEPROM_TILT_WARNING_BYTE, 2, 3);

    // Game customization
    DimLevel = ReadSetting(EEPROM_DIM_LEVEL_BYTE, 2, 2);
    if (DimLevel < 2 || DimLevel > 3) DimLevel = 2;
    RPU_SetDimDivisor(1, DimLevel);

    AttractModeSound = ReadSetting(EEPROM_ATTRACT_MODE_SOUND_BYTE, true, true);

  }
}

byte GetCPCSelection(byte chuteNumber) {
  if (chuteNumber>2) return 0xFF;

  if (CPCSelectionsHaveBeenRead==false) {
    CPCSelection[0] = RPU_ReadByteFromEEProm(RPU_CPC_CHUTE_1_SELECTION_BYTE);
    if (CPCSelection[0]>=NUM_CPC_PAIRS) {
      CPCSelection[0] = 4;
      RPU_WriteByteToEEProm(RPU_CPC_CHUTE_1_SELECTION_BYTE, 4);
    }
    CPCSelection[1] = RPU_ReadByteFromEEProm(RPU_CPC_CHUTE_2_SELECTION_BYTE);  
    if (CPCSelection[1]>=NUM_CPC_PAIRS) {
      CPCSelection[1] = 4;
      RPU_WriteByteToEEProm(RPU_CPC_CHUTE_2_SELECTION_BYTE, 4);
    }
    CPCSelection[2] = RPU_ReadByteFromEEProm(RPU_CPC_CHUTE_3_SELECTION_BYTE);  
    if (CPCSelection[2]>=NUM_CPC_PAIRS) {
      CPCSelection[2] = 4;
      RPU_WriteByteToEEProm(RPU_CPC_CHUTE_3_SELECTION_BYTE, 4);
    }
    CPCSelectionsHaveBeenRead = true;
  }
  
  return CPCSelection[chuteNumber];
}


byte GetCPCCoins(byte cpcSelection) {
  if (cpcSelection>=NUM_CPC_PAIRS) return 1;
  return CPCPairs[cpcSelection][0];
}


byte GetCPCCredits(byte cpcSelection) {
  if (cpcSelection>=NUM_CPC_PAIRS) return 1;
  return CPCPairs[cpcSelection][1];
}



#ifdef RPU_OS_USE_SB300
void InitSB300Registers() {
  RPU_PlaySB300SquareWave(1, 0x00); // Write 0x00 to CR2 (Timer 2 off, continuous mode, 16-bit, C2 clock, CR3 set)
  RPU_PlaySB300SquareWave(0, 0x00); // Write 0x00 to CR3 (Timer 3 off, continuous mode, 16-bit, C3 clock, not prescaled)
  RPU_PlaySB300SquareWave(1, 0x01); // Write 0x00 to CR2 (Timer 2 off, continuous mode, 16-bit, C2 clock, CR1 set)
  RPU_PlaySB300SquareWave(0, 0x00); // Write 0x00 to CR1 (Timer 1 off, continuous mode, 16-bit, C1 clock, timers allowed)
}


void PlaySB300StartupBeep() {
  RPU_PlaySB300SquareWave(1, 0x92); // Write 0x92 to CR2 (Timer 2 on, continuous mode, 16-bit, E clock, CR3 set)
  RPU_PlaySB300SquareWave(0, 0x92); // Write 0x92 to CR3 (Timer 3 on, continuous mode, 16-bit, E clock, not prescaled)
  RPU_PlaySB300SquareWave(4, 0x02); // Set Timer 2 to 0x0200
  RPU_PlaySB300SquareWave(5, 0x00); 
  RPU_PlaySB300SquareWave(6, 0x80); // Set Timer 3 to 0x8000
  RPU_PlaySB300SquareWave(7, 0x00);
  RPU_PlaySB300Analog(0, 0x02);
}
#endif


void QueueDIAGNotification(unsigned short notificationNum) {
  // This is optional, but the machine can play an audio message at boot
  // time to indicate any errors and whether it's going to boot to original
  // or new code.
  //Audio.QueuePrioritizedNotification(notificationNum, 0, 10, CurrentTime);
  (void)notificationNum;
}

// CUSTOMIZATION TODO:
// This function only needs to be updated if the display
// numbers (in test mode) don't match the indices.
// In most games, the operator friendly numbers are just
// the display numbers + 1
// I'm doing this as a function instead of an array because 
// memory is short and spending 44 bytes on a converstion array
// seems wasteful when the board has tons and tons of code space.
// There's a way to store this data in code space and then convert
// it when needed, but that's slow compared to this (ugly) method.
byte LampConvertDisplayNumberToIndex(byte displayNumber) {

  if (displayNumber<60) return displayNumber + 1;
  return OPERATOR_MENU_VALUE_OUT_OF_RANGE;
}

// CUSTOMIZATION TODO:
// Fill in the "operator friendly" solenoid number in the "case" 
// (this is the number reported in the game manual)
// and then the hardware index in the return (from your defines)
unsigned short SolenoidConvertDisplayNumberToIndex(byte displayNumber) {
  switch (displayNumber) {
    case  0: return OPERATOR_MENU_VALUE_UNUSED;
    case  1: return SOL_POP_BUMPER;
    case  2: return SOL_LEFT_SLING;
    case  3: return SOL_KNOCKER;
    case  4: return SOL_RESET_1_BANK;
    case  5: return SOL_RESET_2_BANK;
    case  6: return SOL_DROP_M;
    case  7: return SOL_DROP_E_1;
    case  8: return SOL_DROP_T;
    case  9: return SOL_DROP_E_2;
    case 10: return SOL_DROP_O;
    case 11: return SOL_DROP_R;
    case 12: return SOL_RESET_METEOR_BANK;
    case 13: return SOL_RESET_3_BANK;
    case 14: return SOL_RIGHT_SLING;
    case 15: return 0x4000; // Flipper Mute
    case 16: return SOL_OUTHOLE;
    case 17: return OPERATOR_MENU_VALUE_UNUSED; // unused 0x1000
    case 18: return OPERATOR_MENU_VALUE_UNUSED; // unused 0x8000
    case 19: return 0x2000; // Coin lockout
    default: return OPERATOR_MENU_VALUE_OUT_OF_RANGE;
  }
}

// CUSTOMIZATION TODO:
// In the operator menu's test mode, these are the strengths
// that will be used 
byte SolenoidConvertDisplayNumberToTestStrength(byte displayNumber) {
  switch (displayNumber) {
    case  0: return OPERATOR_MENU_VALUE_UNUSED;
    case  1: return 4;
    case  2: return 4;
    case  3: return 4;
    case  4: return 4;
    case  5: return 4;
    case  6: return 4;
    case  7: return 4;
    case  8: return 4;
    case  9: return 4;
    case 10: return 4;
    case 11: return 4;
    case 12: return 4;
    case 13: return 4;
    case 14: return 4;
    case 15: return 4;
    case 16: return 4;
    case 17: return OPERATOR_MENU_VALUE_UNUSED;
    case 18: return OPERATOR_MENU_VALUE_UNUSED;
    case 19: return 4; 
    default: return OPERATOR_MENU_VALUE_OUT_OF_RANGE;
  }
}




void setup() {
  if (DEBUG_MESSAGES) {
    Serial.begin(57600);
  }

  // Set up the Audio handler in order to play boot messages
  CurrentTime = millis();
  Audio.InitDevices(AUDIO_PLAY_TYPE_WAV_TRIGGER | AUDIO_PLAY_TYPE_ORIGINAL_SOUNDS);
  Audio.StopAllAudio();
  Audio.SetMusicDuckingGain(25);
  Audio.SetSoundFXDuckingGain(20);

  // Tell the OS about game-specific lights and switches
  RPU_SetupGameSwitches(NUM_SWITCHES_WITH_TRIGGERS, NUM_PRIORITY_SWITCHES_WITH_TRIGGERS, TriggeredSwitches);

  if (DEBUG_MESSAGES) Serial.write("Initializing MPU\n");

  // If the hardware has the ability to switch on the Credit/Reset button (requires Rev 4 or greater)
  // then that can be used to choose Original or New code. Otherwise, the hardware switch
  // will choose Original if open, and New if closed
  unsigned long initResult = 0;
  initResult = RPU_InitializeMPU(   RPU_CMD_BOOT_ORIGINAL_IF_CREDIT_RESET | RPU_CMD_BOOT_ORIGINAL_IF_NOT_SWITCH_CLOSED |
                                    RPU_CMD_INIT_AND_RETURN_EVEN_IF_ORIGINAL_CHOSEN | RPU_CMD_PERFORM_MPU_TEST, SW_CREDIT_RESET);

  if (DEBUG_MESSAGES) {
    char buf[128];
    sprintf(buf, "Return from init = 0x%04lX\n", initResult);
    Serial.write(buf);
    if (initResult & RPU_RET_6800_DETECTED) Serial.write("Detected 6800 clock\n");
    else if (initResult & RPU_RET_6802_OR_8_DETECTED) Serial.write("Detected 6802/8 clock\n");
    Serial.write("Back from init\n");
  }

  if (initResult & RPU_RET_ORIGINAL_CODE_REQUESTED) {
    // Stop RPU and allow original code to boot
    while (1);
  }

  RPU_DisableSolenoidStack();
  RPU_SetDisableFlippers(true);

  // Read parameters from EEProm
  ReadStoredParameters();
  RPU_SetCoinLockout((Credits >= MaximumCredits) ? true : false);

  CurrentScores[0] = METEORSTRIKE2023_MAJOR_VERSION;
  CurrentScores[1] = METEORSTRIKE2023_MINOR_VERSION;
  CurrentScores[2] = RPU_OS_MAJOR_VERSION;
  CurrentScores[3] = RPU_OS_MINOR_VERSION;

  CurrentTime = millis();
  OperatorSwitchPressStarted = 0;
  InOperatorMenu = false;

  Menus.SetNavigationButtons(SWITCH_STACK_EMPTY, SWITCH_STACK_EMPTY, SW_CREDIT_RESET, SW_SELF_TEST_SWITCH);
  Menus.SetLampsLookupCallback(LampConvertDisplayNumberToIndex);
  Menus.SetSolenoidIDLookupCallback(SolenoidConvertDisplayNumberToIndex);
  Menus.SetSolenoidStrengthLookupCallback(SolenoidConvertDisplayNumberToTestStrength);
  Menus.SetMenuButtonDebounce(250);

  Audio.StopAllAudio();
  Audio.PlaySound(SOUND_EFFECT_METEOR_INTRO, AUDIO_PLAY_TYPE_WAV_TRIGGER, 10);

#ifdef RPU_OS_USE_SB300
  InitSB300Registers();
  PlaySB300StartupBeep();
#endif
  LastTiltWarningTime = 0;
}

byte ReadSetting(byte setting, byte defaultValue, byte maxValue) {
  byte value = EEPROM.read(setting);
  if (value == 0xFF || value>maxValue) {
    EEPROM.write(setting, defaultValue);
    return defaultValue;
  }
  return value;
}


void StartMeteorStorm() {
  MeteorStormSectors = CurrentTime & 0x07;
  if (MeteorStormSectors==0) MeteorStormSectors = 1;
  PlaySoundEffect(SOUND_EFFECT_METEOR_STORM_SECTOR_1 + (MeteorStormSectors-1));

  for (int count=0; count<3; count++) {
    MeteorStormDescent[count] = (MeteorStormSectors & (1<<count))?7:0;
    MeteorStormDescentSpeed[count] = 4 + ((CurrentTime>>count)&0x01F);
    RocketLaunchedTime[count] = 0;
  }
  MeteorStormStart = CurrentTime;          
}


void SetMeteorDropTargets(byte dropsDown, unsigned long timeOffset = 0, boolean resetBank = true) {
  if (resetBank) RPU_PushToTimedSolenoidStack(SOL_RESET_METEOR_BANK, 10, CurrentTime+timeOffset);
  LastMeteorCheckTime = CurrentTime + timeOffset + 1000;
  unsigned long targetDropTime = CurrentTime + timeOffset + 250;
  for (int count=0; count<6; count++) {
    if ((dropsDown & (1<<count))) {
      // This target should be down        
      RPU_PushToTimedSolenoidStack(MeteorSolenoid[count], 6, targetDropTime);
      targetDropTime += 150;
    }
  }

  CurrentMeteorTargetsValid = dropsDown;
}


byte CheckSequentialSwitches(byte startingSwitch, byte numSwitches) {
  byte returnSwitches = 0; 
  for (byte count=0; count<numSwitches; count++) {
    returnSwitches |= (RPU_ReadSingleSwitchState(startingSwitch+count)<<count);
  }
  return returnSwitches;
}



////////////////////////////////////////////////////////////////////////////
//
//  Audio Output functions
//
////////////////////////////////////////////////////////////////////////////
byte CurrentBackgroundSong = SOUND_EFFECT_NONE;

void PlayBackgroundSong(unsigned int songNum) {

  if (MusicVolume == 0) return;
  CurrentBackgroundSong = songNum;
  Audio.PlayBackgroundSong(songNum);

}

void ResumeBackgroundSong() {
  byte curSong = CurrentBackgroundSong;
  CurrentBackgroundSong = SOUND_EFFECT_NONE;
  PlayBackgroundSong(curSong);
}

unsigned long NextSoundEffectTime = 0;

void PlaySoundEffect(unsigned int soundEffectNum) {

  if (MachineState == MACHINE_STATE_INIT_GAMEPLAY) return;

  // Play digital samples on the WAV trigger (numbered same
  // as SOUND_EFFECT_ defines)
  Audio.PlaySound(soundEffectNum, AUDIO_PLAY_TYPE_WAV_TRIGGER);

  // SOUND_EFFECT_ defines can also be translated into
  // commands for the sound card
  switch (soundEffectNum) {
      /*
          case SOUND_EFFECT_LEFT_SHOOTER_LANE:
            Audio.PlaySoundCardWhenPossible(12, CurrentTime, 0, 500, 7);
            break;
          case SOUND_EFFECT_RETURN_TO_SHOOTER_LANE:
            Audio.PlaySoundCardWhenPossible(22, CurrentTime, 0, 500, 8);
            break;
          case SOUND_EFFECT_SAUCER:
            Audio.PlaySoundCardWhenPossible(14, CurrentTime, 0, 500, 7);
            break;
          case SOUND_EFFECT_DROP_TARGET_HURRY:
            Audio.PlaySoundCardWhenPossible(2, CurrentTime, 0, 45, 3);
            break;
          case SOUND_EFFECT_DROP_TARGET_COMPLETE:
            Audio.PlaySoundCardWhenPossible(9, CurrentTime, 0, 1400, 4);
            Audio.PlaySoundCardWhenPossible(19, CurrentTime, 1500, 10, 4);
            break;
          case SOUND_EFFECT_HOOFBEATS:
            Audio.PlaySoundCardWhenPossible(12, CurrentTime, 0, 100, 10);
            break;
          case SOUND_EFFECT_STOP_BACKGROUND:
            Audio.PlaySoundCardWhenPossible(19, CurrentTime, 0, 10, 10);
            break;
          case SOUND_EFFECT_DROP_TARGET_HIT:
            Audio.PlaySoundCardWhenPossible(7, CurrentTime, 0, 150, 5);
            break;
          case SOUND_EFFECT_SPINNER:
            Audio.PlaySoundCardWhenPossible(6, CurrentTime, 0, 25, 2);
            break;
      */
  }
}


void QueueNotification(unsigned int soundEffectNum, byte priority) {
  if (CalloutsVolume == 0) return;

  // With RPU_OS_HARDWARE_REV 4 and above, the WAV trigger has two-way communication,
  // so it's not necesary to tell it the length of a notification. For support for
  // earlier hardware, you'll need an array of VoicePromptLengths for each prompt
  // played (for queueing and ducking)
  //  Audio.QueuePrioritizedNotification(soundEffectNum, VoicePromptLengths[soundEffectNum-SOUND_EFFECT_VP_VOICE_NOTIFICATIONS_START], priority, CurrentTime);
  Audio.QueuePrioritizedNotification(soundEffectNum, 0, priority, CurrentTime);

}


void AlertPlayerUp() {
//  QueueNotification(SOUND_EFFECT_VP_PLAYER_1_UP + CurrentPlayer, 1);
}






////////////////////////////////////////////////////////////////////////////
//
//  Lamp Management functions
//
////////////////////////////////////////////////////////////////////////////

byte BonusStartingLamps[3] = {LAMP_LEFT_ROCKET_1K, LAMP_MIDDLE_ROCKET_1K, LAMP_RIGHT_ROCKET_1K};
unsigned long ZeroArray[3] = {0, 0, 0};
byte SevenArray[3] = {7, 7, 7};

void ShowBonus() {

  if (memcmp(BonusAnimationStart, ZeroArray, 12)==0 && memcmp(Bonus, SevenArray, 3)==0) {
//  if (  !BonusAnimationStart[0] && !BonusAnimationStart[1] && !BonusAnimationStart[2] &&
//        Bonus[0]==7 && Bonus[1]==7 && Bonus[2]==7 ) {
    byte fullAnimationPhase = (CurrentTime/100)%4;
    for (byte count=0; count<7; count++) {
      for (byte i=0; i<3; i++) {
        RPU_SetLampState(BonusStartingLamps[i]-count, ((count-(i%2))%4)==fullAnimationPhase);
      }
    }
    GoalsCompletedFlags[CurrentPlayer] |= GOAL_BONUS_MAXED_OUT;
  } else {
    
    for (byte count=0; count<7; count++) {
      for (byte i=0; i<3; i++) {
        if (BonusAnimationStart[i]==0) RPU_SetLampState(BonusStartingLamps[i]-count, (count+1)==Bonus[i]);
      }
    }
  }
  
}


void ShowMeteorLamps() {
  byte currentSwitches = CheckSequentialSwitches(SW_DROP_TARGET_R, 6);
  
  if (GameMode==GAME_MODE_SKILL_SHOT) {
    for (int count=LAMP_METEOR_DROP_R; count<=LAMP_METEOR_DROP_M; count++) {
      byte alternatingPhase = ((CurrentTime/100)+count%2)%4;
      RPU_SetLampState(count, alternatingPhase, alternatingPhase%2);
    }

    byte wowPhase = 5 - ((CurrentTime/1000)%6);
    SkillShotWowLit = wowPhase + SW_DROP_TARGET_R;
    for (int count=LAMP_METEOR_WOW_R; count<=LAMP_METEOR_WOW_M; count++) {
      RPU_SetLampState(count, (count-LAMP_METEOR_WOW_R)==wowPhase);
    }
  } else if (GameMode==GAME_MODE_ORPHEUS_START) {
    byte lampPhase = ((CurrentTime-GameModeStartTime)/125)%10;
    for (int count=0; count<6; count++) {
      RPU_SetLampState(LAMP_METEOR_DROP_R+count, (count==lampPhase)||(count==(10-lampPhase)));
      RPU_SetLampState(LAMP_METEOR_WOW_M-count, (count==lampPhase)||(count==(10-lampPhase)));
    }
  } else if (GameMode==GAME_MODE_ORPHEUS) {
    byte wowPhase = 5;
    byte numBits = 6-CountBits(CurrentMeteorTargetsValid&0x3F);
    byte bitPos;
    byte posBasedOnTime = 1;
    if (numBits) posBasedOnTime = ((CurrentTime/1500)%numBits)+1;
    for (bitPos = 0; bitPos<6 && posBasedOnTime; bitPos++) {
      if ((CurrentMeteorTargetsValid & (0x20>>bitPos))==0x00) posBasedOnTime -= 1;
    }
    wowPhase = 5-(bitPos-1);
    SkillShotWowLit = wowPhase + SW_DROP_TARGET_R;
    for (int count=0; count<6; count++) {
      RPU_SetLampState(LAMP_METEOR_DROP_R+count, (currentSwitches & (1<<count))?0:1, 0, 200);
      RPU_SetLampState(LAMP_METEOR_WOW_R+count, (count==wowPhase));
    }
  } else {
    for (int count=0; count<6; count++) {
      RPU_SetLampState(LAMP_METEOR_DROP_R+count, currentSwitches&(1<<count)?0:1);
      RPU_SetLampState(LAMP_METEOR_WOW_R+count, 0);
    }
  }
}



void ShowStandupTargetLamps() {
  if (GameMode==GAME_MODE_SKILL_SHOT) {
  } else if (GameMode==GAME_MODE_METEOR_STORM) {
    byte lampPhase = ((CurrentTime-GameModeStartTime)/100)%3;

    RPU_SetLampState(LAMP_STANDUP_TARGET_WOW, RocketsArmed && lampPhase==1);
    RPU_SetLampState(LAMP_STANDUP_TARGET_SPECIAL, RocketsArmed && lampPhase==2);

  } else {
    
    boolean wowOn = (StandupAward==2);
    boolean specialOn = (StandupAward==3);
    boolean flashing = ((StandupResetTime-CurrentTime)<500);
    
    RPU_SetLampState(LAMP_STANDUP_TARGET_WOW, wowOn, 0, flashing?100:0);
    RPU_SetLampState(LAMP_STANDUP_TARGET_SPECIAL, specialOn, 0, flashing?100:0);
  }
  
}


byte ShowBonusAnimation(byte topLamp, unsigned long animationStartTime, byte animationShown, byte animationCap) {
  byte bonusElapsed = ((CurrentTime-animationStartTime)/100) + 1;

  if (animationShown==0) {
    for (int count=0; count<7; count++) {
      RPU_SetLampState(topLamp+count, 0);      
    }

    // First time in loop, we can play initial sound
    //PlaySoundEffect();
  }

  if (bonusElapsed>=(animationCap*2)) {
    return 0xFF;
  }

  if (bonusElapsed>animationShown) {
    if (bonusElapsed<=animationCap) {
      CurrentScores[CurrentPlayer] += SCORE_BONUS_LAMP;
      for (int count=0; count<7; count++) {
        RPU_SetLampState(topLamp+count, ((7-count)<=bonusElapsed));      
      }
    } else {
      for (int count=0; count<7; count++) {
        RPU_SetLampState(topLamp+count, (((7-count)<=animationCap) && (7-count)>(bonusElapsed-animationCap)));      
      }
    }
    byte soundNum = SOUND_EFFECT_3_TARGET_HIT;
    if (topLamp==LAMP_LEFT_ROCKET_7K) soundNum = SOUND_EFFECT_1_TARGET_HIT;
    else if (topLamp==LAMP_MIDDLE_ROCKET_7K) soundNum = SOUND_EFFECT_2_TARGET_HIT;
    if ((CurrentTime-LastTimeRocketBonusPlayed)>300) {        
      PlaySoundEffect(soundNum);
      LastTimeRocketBonusPlayed = CurrentTime;
    }
  }

  if (animationCap==1) return 0xFF;
  return bonusElapsed;
}



/*
void ShowLampAnimation(byte animationNum, unsigned long divisor, unsigned long baseTime, byte subOffset, boolean dim, boolean reverse=false, byte keepLampOn=99) {
  byte currentStep = (baseTime/divisor)%LAMP_ANIMATION_STEPS;
  if (reverse) currentStep = (LAMP_ANIMATION_STEPS-1) - currentStep;

  byte lampNum = 0;
  for (int byteNum=0; byteNum<8; byteNum++) {
    for (byte bitNum=0; bitNum<8; bitNum++) {

      // if there's a subOffset, turn off lights at that offset
      if (subOffset) {
        byte lampOff;
        lampOff = LampAnimations[animationNum][(currentStep+subOffset)%LAMP_ANIMATION_STEPS][byteNum] & (1<<bitNum);
        if (lampOff && lampNum!=keepLampOn) RPU_SetLampState(lampNum, 0);
      }
      
      byte lampOn = LampAnimations[animationNum][currentStep][byteNum] & (1<<bitNum);
      if (lampOn) RPU_SetLampState(lampNum, 1, dim);
      
      lampNum += 1;
    }
  }  
}
*/

byte MeteorGoals[7] = {3, 7, 13, 21, 30, 41, 55};

byte GetNextMeteorGoal() {
  if (OrpheusRound[CurrentPlayer]<7) return MeteorGoals[OrpheusRound[CurrentPlayer]];
  return 100;
}


void ShowBonusLamps(boolean actuallyShowLamps = true) {

  if (CollectRocketAnimationStart!=0) {
    byte totalBonus = Bonus[0]+Bonus[1]+Bonus[2];
    
    if (CollectRocketCurrentLevel<totalBonus) {
      int collectionCount = (byte)((CurrentTime-CollectRocketAnimationStart)/110);
      if (collectionCount>CollectRocketCurrentLevel) {
    
        for (int count=0; count<7; count++) {
          RPU_SetLampState(LAMP_LEFT_ROCKET_1K-count, (count<Bonus[0]) && (count<collectionCount), 0, 20);            
          RPU_SetLampState(LAMP_MIDDLE_ROCKET_1K-count, (count<Bonus[1]) && (count<(collectionCount-(int)Bonus[0])), 0, 20);            
          RPU_SetLampState(LAMP_RIGHT_ROCKET_1K-count, (count<Bonus[2]) && (count<(collectionCount-(int)Bonus[0]-(int)Bonus[1])), 0, 20);          
        }
        
        if ((CollectRocketCurrentLevel%3)==0) PlaySoundEffect(SOUND_EFFECT_1_TARGET_HIT);
        CurrentScores[CurrentPlayer] += SCORE_BONUS_LAMP;
        CollectRocketCurrentLevel = collectionCount;
      }
    } else {
      CollectRocketAnimationStart = 0;
    }
  } else if (GameMode==GAME_MODE_UNSTRUCTURED_PLAY) {
    ShowBonus();

    for (int i=0; i<3; i++) {
      if (BonusAnimationStart[i]) {
        BonusAnimationShown[i] = ShowBonusAnimation(BonusStartingLamps[i]-6, BonusAnimationStart[i], BonusAnimationShown[i], Bonus[i]);
        if (BonusAnimationShown[i]==0xFF) {
          BonusAnimationStart[i] = 0;
          BonusAnimationShown[i] = 0;
        }
      }
    }
 
  } else if (GameMode==GAME_MODE_METEOR_STORM_START) {
    byte scannerPhase = ((CurrentTime-GameModeStartTime)/700)%4;
    byte scannerPhase2 = (((CurrentTime-GameModeStartTime)/50)%13) + 1;
    if (scannerPhase2>7) scannerPhase2 = 14-scannerPhase2;
    for (int count=0; count<7; count++) {
      RPU_SetLampState(LAMP_LEFT_ROCKET_7K+count, (scannerPhase==0)||((count+1)==scannerPhase2));
      RPU_SetLampState(LAMP_MIDDLE_ROCKET_7K+count, (scannerPhase%2)||((count+1)==scannerPhase2));
      RPU_SetLampState(LAMP_RIGHT_ROCKET_7K+count, (scannerPhase==2)||((count+1)==scannerPhase2));
    }
  } else if (GameMode==GAME_MODE_METEOR_STORM) {
    for (int count=0; count<3; count++) {
      byte rocketLamp = LAMP_LEFT_ROCKET_1K;
      if (count==1) rocketLamp = LAMP_MIDDLE_ROCKET_1K;
      else if (count==2) rocketLamp = LAMP_RIGHT_ROCKET_1K;
      for (int lampNum=0; lampNum<7; lampNum++) {
        boolean lampOn = false;
        int flashOn = 0;
        byte rocketHeight = 0; 
        if (RocketLaunchedTime[count]!=0) {
          rocketHeight = ((CurrentTime-RocketLaunchedTime[count])/300) + 1;
          if (rocketHeight==8) {
            RocketLaunchedTime[count] = 0;
            PlaySoundEffect(SOUND_EFFECT_ROCKET_1_MISSED+count);
          }
        }
        
        if (rocketHeight==(lampNum+1)) {
          if (rocketHeight==MeteorStormDescent[count]) {
            RocketLaunchedTime[count] = 0;
            Audio.StopAllAudio();
            PlaySoundEffect(SOUND_EFFECT_METEOR_HIT);
            ResumeBackgroundSong();
            lampOn = true;            
            flashOn = 50;
            if (RocketsFiredFromBank) CurrentScores[CurrentPlayer] += ((unsigned long)rocketHeight * (SCORE_METEOR_HIT_REWARD/2));
            else CurrentScores[CurrentPlayer] += ((unsigned long)rocketHeight * (SCORE_METEOR_HIT_REWARD));
            MeteorStormSectors &= ~(1<<count);
            MeteorStormDescent[count] = 0;
            NumberOfMeteorsHit[CurrentPlayer] += 1;
            byte nextMeteorGoal = GetNextMeteorGoal();

            int meteorsRemaining = (int)nextMeteorGoal - (int)NumberOfMeteorsHit[CurrentPlayer];

            MeteorHitEndTime = CurrentTime + METEOR_HIT_SHORT_DISPLAY;
            if (meteorsRemaining<4 && meteorsRemaining>0) {
              MeteorHitEndTime = CurrentTime + METEOR_HIT_LONG_DISPLAY;
              PlaySoundEffect(SOUND_EFFECT_1_METEOR_REMAINING + (meteorsRemaining-1));
            } 

            MeteorHitStartTime = CurrentTime;
            MeteorHitStarted = true;

          } else {
            lampOn = true;
          }          
        } else if (lampNum==0 && (RocketsArmed & (1<<count))) {
          lampOn = true;
          flashOn = 500;
        } else {
          lampOn = (((lampNum+1)==MeteorStormDescent[count]));
          flashOn = 100;
        }
        if (actuallyShowLamps) RPU_SetLampState(rocketLamp-lampNum, lampOn, 0, flashOn);
      }
    }
  }
}

byte RocketDropLamps[] = {LAMP_BOTTOM_LEFT_BANK_WOW, LAMP_BOTTOM_LEFT_BANK_SPECIAL, LAMP_TOP_LEFT_BANK_WOW, LAMP_TOP_LEFT_DROP_TARGET_SPECIAL, LAMP_RIGHT_BANK_WOW, LAMP_RIGHT_BANK_SPECIAL};

void ShowRocketDropLamps() {
  if (GameMode==GAME_MODE_METEOR_STORM) {
    byte lampPhase = ((CurrentTime-GameModeStartTime)/200)%2;
    byte signalRocket = MeteorStormSectors & (~RocketsArmed);

    for (int count=0; count<3; count++) {
      
      byte signalForThisOne = signalRocket & (1<<count);
      RPU_SetLampState(RocketDropLamps[count*2], lampPhase && signalForThisOne);
      RPU_SetLampState(RocketDropLamps[count*2+1], (lampPhase==0) && signalForThisOne);
      
    }
  } else {
    for (int count=0; count<6; count++) {
      if (TargetAward) RPU_SetLampState(RocketDropLamps[count], count==((TargetAwardPhase)*2 + (TargetAward-1)), 0, 500);
      else RPU_SetLampState(RocketDropLamps[count], 0);
    }
  }
}

byte BonusXLamps[] = {LAMP_BONUS_1X, LAMP_BONUS_2X, LAMP_BONUS_4X};

void ShowBonusXLamps() {
  if (GameMode==GAME_MODE_SKILL_SHOT) {
    
  } else {
    if (BonusXAnimationStart==0) {
      for (int count=0; count<3; count++) {
        RPU_SetLampState(BonusXLamps[count], BonusX[CurrentPlayer] & (1<<count));
      }
    } else {
      byte lampPhase = ((CurrentTime-BonusXAnimationStart)/100)%4;
      byte lampOn = (lampPhase?0xFF:0) & BonusX[CurrentPlayer];
      for (int count=0; count<3; count++) {
        RPU_SetLampState(BonusXLamps[count], lampOn & (1<<count), lampPhase%2);
      }
        
      if ((CurrentTime-BonusXAnimationStart)>5000) BonusXAnimationStart = 0;
    }
  }
}

void ShowSpinnerLamps() {
  byte currentSwitches = CheckSequentialSwitches(SW_DROP_TARGET_R, 6);

  if (SuperSpinnerEndTime) {
    for (int count=0; count<6; count++) {
      RPU_SetLampState(LAMP_SPINNER_1-count, 1, 0, 100);    
    }
  } else if (GameMode==GAME_MODE_SKILL_SHOT) {
  } else {
    if (LastTimeSpinnerHit!=0 && (CurrentTime-LastTimeSpinnerHit)<SPINNER_ANIMATION_DURATION) {
      for (int count=0; count<6; count++) {
        boolean lampDim = !(currentSwitches & (1<<(5-count)));
        RPU_SetLampState(LAMP_SPINNER_1-count, (count>=SpinnerHitPhase) || (SpinnerHitPhase>6 && count<(SpinnerHitPhase-5)), lampDim?2:0);
      }
    } else {
      for (int count=0; count<6; count++) {
        RPU_SetLampState(LAMP_SPINNER_6+count, currentSwitches & (1<<count));
      }
      if (LastTimeSpinnerHit) RPU_SetDisplayCredits(Credits);
      LastTimeSpinnerHit = 0;
      SpinnerHitPhase = 11;
    }
  }
}



void ShowShootAgainLamp() {

  if ( (BallFirstSwitchHitTime == 0 && BallSaveNumSeconds) || (BallSaveEndTime && CurrentTime < BallSaveEndTime) ) {
    unsigned long msRemaining = 5000;
    if (BallSaveEndTime != 0) msRemaining = BallSaveEndTime - CurrentTime;
    RPU_SetLampState(LAMP_SHOOT_AGAIN, 1, 0, (msRemaining < 5000) ? 100 : 500);
  } else {
    RPU_SetLampState(LAMP_SHOOT_AGAIN, SamePlayerShootsAgain);
  }
}

byte RolloverLamps[] = {LAMP_LEFT_ROLLOVER_AND_RIGHT_INLANE, LAMP_RIGHT_ROLLOVER_AND_LEFT_RETURN, LAMP_MIDDLE_ROLLOVER_AND_STANDUP_SPOT};

void ShowSpotMeteorLamps() {
  if (GameMode==GAME_MODE_SKILL_SHOT) {
    byte lampPhase = ((CurrentTime-GameModeStartTime)/300)%4;
    for (byte count=0; count<3; count++) {
      RPU_SetLampState(RolloverLamps[count], lampPhase, lampPhase%2);
    }
    
  } else {
    RPU_SetLampState(LAMP_LEFT_ROLLOVER_AND_RIGHT_INLANE, SpotMeteorTargetByte&SPOT_METEOR_TARGET_LIGHT_1);
    RPU_SetLampState(LAMP_RIGHT_ROLLOVER_AND_LEFT_RETURN, SpotMeteorTargetByte&SPOT_METEOR_TARGET_LIGHT_3);
    RPU_SetLampState(LAMP_MIDDLE_ROLLOVER_AND_STANDUP_SPOT, StandupAward==1 && (SpotMeteorTargetByte&SPOT_METEOR_TARGET_LIGHT_2));    
  }
}


void ShowRocketBumperLamps() {
  if (GameMode==GAME_MODE_METEOR_STORM && OrpheusRound[CurrentPlayer]<LAUNCH_ROCKETS_WITH_DEAD_BUMPER_UNTIL_LEVEL) {
    RPU_SetLampState(LAMP_RIGHT_OUT_LANE_AND_1_BUMPER, MeteorStormSectors&METEOR_STORM_SECTOR_1, 1, 500);
    RPU_SetLampState(LAMP_LEFT_OUT_LANE_AND_3_BUMPER, MeteorStormSectors&METEOR_STORM_SECTOR_3, 1, 500);
  } else {
    boolean flash1 = (Rocket1BumperAnimationTime!=0 && ((CurrentTime-Rocket1BumperAnimationTime)<ROCKET_ANIMATION_DURATION));
    boolean flash3 = (Rocket3BumperAnimationTime!=0 && ((CurrentTime-Rocket3BumperAnimationTime)<ROCKET_ANIMATION_DURATION));
    RPU_SetLampState(LAMP_RIGHT_OUT_LANE_AND_1_BUMPER, (CollectRocketsByte&ROCKET_1_COLLECT_LIGHT)||flash1, 0, flash1?100:0);
    RPU_SetLampState(LAMP_LEFT_OUT_LANE_AND_3_BUMPER, (CollectRocketsByte&ROCKET_3_COLLECT_LIGHT)||flash3, 0, flash3?100:0);
  }
}



////////////////////////////////////////////////////////////////////////////
//
//  Machine State Helper functions
//
////////////////////////////////////////////////////////////////////////////
boolean AddPlayer(boolean resetNumPlayers = false) {

  if (Credits < 1 && !FreePlayMode) return false;
  if (resetNumPlayers) CurrentNumPlayers = 0;
  if (CurrentNumPlayers >= 4) return false;

  CurrentNumPlayers += 1;
  RPU_SetDisplay(CurrentNumPlayers - 1, 0, true, 2);
//  RPU_SetDisplayBlank(CurrentNumPlayers - 1, 0x30);

  if (!FreePlayMode) {
    Credits -= 1;
    RPU_WriteByteToEEProm(RPU_CREDITS_EEPROM_BYTE, Credits);
    RPU_SetDisplayCredits(Credits);
    RPU_SetCoinLockout(false);
  }
  PlaySoundEffect(SOUND_EFFECT_ADD_PLAYER_1 + (CurrentNumPlayers - 1));

  RPU_WriteULToEEProm(RPU_TOTAL_PLAYS_EEPROM_START_BYTE, RPU_ReadULFromEEProm(RPU_TOTAL_PLAYS_EEPROM_START_BYTE) + 1);

  return true;
}


byte SwitchToChuteNum(byte switchHit) {
  byte chuteNum = 0;
  if (switchHit==SW_COIN_2) chuteNum = 1;
  else if (switchHit==SW_COIN_3) chuteNum = 2;
  return chuteNum;   
}

unsigned short ChuteAuditByte[] = {RPU_CHUTE_1_COINS_START_BYTE, RPU_CHUTE_2_COINS_START_BYTE, RPU_CHUTE_3_COINS_START_BYTE};
void AddCoinToAudit(byte chuteNum) {
  if (chuteNum>2) return;
  unsigned short coinAuditStartByte = ChuteAuditByte[chuteNum];
  RPU_WriteULToEEProm(coinAuditStartByte, RPU_ReadULFromEEProm(coinAuditStartByte) + 1);
}

/*
byte CoinsAdded = 0;

void AddCredit(boolean playSound = false, byte numToAdd = 1, boolean coinCredit = false) {

  if (coinCredit) {
    CoinsAdded += 1;
    if (CoinsAdded<COINS_PER_CREDIT) {
      if (playSound) PlaySoundEffect(SOUND_EFFECT_ADD_CREDIT);
      return;
    }
    CoinsAdded = 0;
  }
  
  if (Credits < MaximumCredits) {
    Credits += numToAdd;
    if (Credits > MaximumCredits) Credits = MaximumCredits;
    RPU_WriteByteToEEProm(RPU_CREDITS_EEPROM_BYTE, Credits);
    if (playSound) PlaySoundEffect(SOUND_EFFECT_ADD_CREDIT);
    RPU_SetDisplayCredits(Credits);
    RPU_SetCoinLockout(false);
  } else {
    RPU_SetDisplayCredits(Credits);
    RPU_SetCoinLockout(true);
  }

}
*/

void AddCredit(boolean playSound = false, byte numToAdd = 1) {
  if (Credits < MaximumCredits) {
    Credits += numToAdd;
    if (Credits > MaximumCredits) Credits = MaximumCredits;
    RPU_WriteByteToEEProm(RPU_CREDITS_EEPROM_BYTE, Credits);
    if (playSound) {
      PlaySoundEffect(SOUND_EFFECT_ADD_CREDIT);
    }
    RPU_SetDisplayCredits(Credits, !FreePlayMode);
    RPU_SetCoinLockout(false);
  } else {
    RPU_SetDisplayCredits(Credits, !FreePlayMode);
    RPU_SetCoinLockout(true);
  }

}



boolean AddCoin(byte chuteNum) {
  boolean creditAdded = false;
  if (chuteNum>2) return false;

  byte cpcSelection = GetCPCSelection(chuteNum);

  // Find the lowest chute num with the same ratio selection
  // and use that ChuteCoinsInProgress counter
  byte chuteNumToUse;
  for (chuteNumToUse=0; chuteNumToUse<=chuteNum; chuteNumToUse++) {
    if (GetCPCSelection(chuteNumToUse)==cpcSelection) break;
  }

  PlaySoundEffect(SOUND_EFFECT_ADD_CREDIT);

  byte cpcCoins = GetCPCCoins(cpcSelection);
  byte cpcCredits = GetCPCCredits(cpcSelection);
  byte coinProgressBefore = ChuteCoinsInProgress[chuteNumToUse];
  ChuteCoinsInProgress[chuteNumToUse] += 1;

  if (ChuteCoinsInProgress[chuteNumToUse]==cpcCoins) {
    if (cpcCredits>cpcCoins) AddCredit(false, cpcCredits - (coinProgressBefore));
    else AddCredit(false, cpcCredits);
    ChuteCoinsInProgress[chuteNumToUse] = 0;
    creditAdded = true;
  } else {
    if (cpcCredits>cpcCoins) {
      AddCredit(false, 1);
      creditAdded = true;
    } else {
    }
  }

  return creditAdded;
}




void AddSpecialCredit() {
  AddCredit(false, 1);
  RPU_PushToTimedSolenoidStack(SOL_KNOCKER, 3, CurrentTime, true);
  RPU_WriteULToEEProm(RPU_TOTAL_REPLAYS_EEPROM_START_BYTE, RPU_ReadULFromEEProm(RPU_TOTAL_REPLAYS_EEPROM_START_BYTE) + 1);  
}

void AwardSpecial() {
  if (SpecialCollected) return;
  SpecialCollected = true;
  if (TournamentScoring) {
    CurrentScores[CurrentPlayer] += SpecialValue;
  } else {
    AddSpecialCredit();
  }
}

void AwardExtraBall() {
  if (ExtraBallCollected) return;
  ExtraBallCollected = true;
  if (TournamentScoring) {
    CurrentScores[CurrentPlayer] += ExtraBallValue;
  } else {
    SamePlayerShootsAgain = true;
    RPU_SetLampState(LAMP_SHOOT_AGAIN, SamePlayerShootsAgain);
    Audio.StopAllAudio();
    PlaySoundEffect(SOUND_EFFECT_EXTRA_BALL);
    ResumeBackgroundSong();
  }
}


void SetBallSave(unsigned long duration, byte numberOfSaves = 0xFF, boolean addToBallSave = false) {

  if (duration == 0) {
    BallSaveEndTime = 0;
    NumberOfBallSavesRemaining = 0;
  } else if (addToBallSave) {
    if (BallSaveEndTime) BallSaveEndTime += duration;
  } else {
    BallSaveEndTime = CurrentTime + duration;
    NumberOfBallSavesRemaining = numberOfSaves;
  }
}





////////////////////////////////////////////////////////////////////////////
//
//  Operator Menu
//    
//
////////////////////////////////////////////////////////////////////////////
#define SOUND_EFFECT_OM_CPC_VALUES                  180
#define SOUND_EFFECT_OM_CRB_VALUES                  210
#define SOUND_EFFECT_OM_DIFFICULTY_VALUES           220

#define SOUND_EFFECT_AP_TOP_LEVEL_MENU_ENTRY    1700
#define SOUND_EFFECT_AP_TEST_MENU               1701
#define SOUND_EFFECT_AP_AUDITS_MENU             1702
#define SOUND_EFFECT_AP_BASIC_ADJUSTMENTS_MENU  1703
#define SOUND_EFFECT_AP_GAME_RULES_LEVEL        1704
#define SOUND_EFFECT_AP_GAME_SPECIFIC_ADJ_MENU  1705

#define SOUND_EFFECT_AP_TEST_LAMPS              1710
#define SOUND_EFFECT_AP_TEST_DISPLAYS           1711
#define SOUND_EFFECT_AP_TEST_SOLENOIDS          1712
#define SOUND_EFFECT_AP_TEST_SWITCHES           1713
#define SOUND_EFFECT_AP_TEST_SOUNDS             1714
#define SOUND_EFFECT_AP_TEST_EJECT_BALLS        1715

#define SOUND_EFFECT_AP_AUDIT_TOTAL_PLAYS       1720
#define SOUND_EFFECT_AP_AUDIT_CHUTE_1_COINS     1721
#define SOUND_EFFECT_AP_AUDIT_CHUTE_2_COINS     1722
#define SOUND_EFFECT_AP_AUDIT_CHUTE_3_COINS     1723
#define SOUND_EFFECT_AP_AUDIT_TOTAL_REPLAYS     1724
#define SOUND_EFFECT_AP_AUDIT_AVG_BALL_TIME     1725
#define SOUND_EFFECT_AP_AUDIT_HISCR_BEAT        1726
#define SOUND_EFFECT_AP_AUDIT_TOTAL_BALLS       1727
#define SOUND_EFFECT_AP_AUDIT_NUM_MATCHES       1728
#define SOUND_EFFECT_AP_AUDIT_MATCH_PERCENTAGE  1729
#define SOUND_EFFECT_AP_AUDIT_LIFETIME_PLAYS    1730
#define SOUND_EFFECT_AP_AUDIT_MINUTES_ON        1731
#define SOUND_EFFECT_AP_AUDIT_CLEAR_AUDITS      1732

#define OM_BASIC_ADJ_IDS_FREEPLAY               0
#define OM_BASIC_ADJ_IDS_BALL_SAVE              1
#define OM_BASIC_ADJ_IDS_TILT_WARNINGS          2
#define OM_BASIC_ADJ_IDS_MUSIC_VOLUME           3
#define OM_BASIC_ADJ_IDS_SOUNDFX_VOLUME         4
#define OM_BASIC_ADJ_IDS_CALLOUTS_VOLUME        5
#define OM_BASIC_ADJ_IDS_BALLS_PER_GAME         6
#define OM_BASIC_ADJ_IDS_TOURNAMENT_MODE        7
#define OM_BASIC_ADJ_IDS_EXTRA_BALL_VALUE       8
#define OM_BASIC_ADJ_IDS_SPECIAL_VALUE          9 
#define OM_BASIC_ADJ_IDS_RESET_DURING_GAME      10
#define OM_BASIC_ADJ_IDS_SCORE_LEVEL_1          11
#define OM_BASIC_ADJ_IDS_SCORE_LEVEL_2          12
#define OM_BASIC_ADJ_IDS_SCORE_LEVEL_3          13
#define OM_BASIC_ADJ_IDS_SCORE_AWARDS           14
#define OM_BASIC_ADJ_IDS_SCROLLING_SCORES       15
#define OM_BASIC_ADJ_IDS_HISCR                  16
#define OM_BASIC_ADJ_IDS_CREDITS                17
#define OM_BASIC_ADJ_IDS_CPC_1                  18
#define OM_BASIC_ADJ_IDS_CPC_2                  19
#define OM_BASIC_ADJ_IDS_CPC_3                  20
#define OM_BASIC_ADJ_IDS_MATCH_FEATURE          21
#define OM_BASIC_ADJ_FINISHED                   22
#define SOUND_EFFECT_AP_FREEPLAY                (1740 + OM_BASIC_ADJ_IDS_FREEPLAY)
#define SOUND_EFFECT_AP_BALL_SAVE_SECONDS       (1740 + OM_BASIC_ADJ_IDS_BALL_SAVE)
#define SOUND_EFFECT_AP_TILT_WARNINGS           (1740 + OM_BASIC_ADJ_IDS_TILT_WARNINGS)
#define SOUND_EFFECT_AP_MUSIC_VOLUME            (1740 + OM_BASIC_ADJ_IDS_MUSIC_VOLUME)
#define SOUND_EFFECT_AP_SOUNDFX_VOLUME          (1740 + OM_BASIC_ADJ_IDS_SOUNDFX_VOLUME)
#define SOUND_EFFECT_AP_CALLOUTS_VOLUME         (1740 + OM_BASIC_ADJ_IDS_CALLOUTS_VOLUME)
#define SOUND_EFFECT_AP_BALLS_PER_GAME          (1740 + OM_BASIC_ADJ_IDS_BALLS_PER_GAME)
#define SOUND_EFFECT_AP_TOURNAMENT_MODE         (1740 + OM_BASIC_ADJ_IDS_TOURNAMENT_MODE)
#define SOUND_EFFECT_AP_EXTRA_BALL_VALUE        (1740 + OM_BASIC_ADJ_IDS_EXTRA_BALL_VALUE)
#define SOUND_EFFECT_AP_SPECIAL_VALUE           (1740 + OM_BASIC_ADJ_IDS_SPECIAL_VALUE)
#define SOUND_EFFECT_AP_RESET_DURING_GAME       (1740 + OM_BASIC_ADJ_IDS_RESET_DURING_GAME)
#define SOUND_EFFECT_AP_ADJ_SCORE_LEVEL_1       (1740 + OM_BASIC_ADJ_IDS_SCORE_LEVEL_1)
#define SOUND_EFFECT_AP_ADJ_SCORE_LEVEL_2       (1740 + OM_BASIC_ADJ_IDS_SCORE_LEVEL_2)
#define SOUND_EFFECT_AP_ADJ_SCORE_LEVEL_3       (1740 + OM_BASIC_ADJ_IDS_SCORE_LEVEL_3)
#define SOUND_EFFECT_AP_SCORE_AWARDS            (1740 + OM_BASIC_ADJ_IDS_SCORE_AWARDS)
#define SOUND_EFFECT_AP_SCROLLING_SCORES        (1740 + OM_BASIC_ADJ_SCROLLING_SCORES)
#define SOUND_EFFECT_AP_ADJ_HISCR               (1740 + OM_BASIC_ADJ_IDS_HISCR)
#define SOUND_EFFECT_AP_ADJ_CREDITS             (1740 + OM_BASIC_ADJ_IDS_CREDITS)
#define SOUND_EFFECT_AP_ADJ_CPC_1               (1740 + OM_BASIC_ADJ_IDS_CPC_1)
#define SOUND_EFFECT_AP_ADJ_CPC_2               (1740 + OM_BASIC_ADJ_IDS_CPC_2)
#define SOUND_EFFECT_AP_ADJ_CPC_3               (1740 + OM_BASIC_ADJ_IDS_CPC_3)
#define SOUND_EFFECT_AP_MATCH_FEATURE           (1740 + OM_BASIC_ADJ_IDS_MATCH_FEATURE)

#define SOUND_EFFECT_OM_EASY_RULES_INSTRUCTIONS           1770
#define SOUND_EFFECT_OM_MEDIUM_RULES_INSTRUCTIONS         1771
#define SOUND_EFFECT_OM_HARD_RULES_INSTRUCTIONS           1772
#define SOUND_EFFECT_OM_PROGRESSIVE_RULES_INSTRUCTIONS    1773
#define SOUND_EFFECT_OM_CUSTOM_RULES_INSTRUCTIONS         1774

#define OM_GAME_ADJ_EASY_DIFFICULTY                 0
#define OM_GAME_ADJ_MEDIUM_DIFFICULTY               1
#define OM_GAME_ADJ_HARD_DIFFICULTY                 2
#define OM_GAME_ADJ_PROGRESSIVE_DIFFICULTY          3
#define OM_GAME_ADJ_CUSTOM_DIFFICULTY               4
#define SOUND_EFFECT_AP_DIFFICULTY                  (1790 + OM_GAME_ADJ_EASY_DIFFICULTY)

// CUSTOMIZATION TODO:
// Create defines for custom parameters and be sure to 
// create the associated callouts (starting at 1800)
#define OM_GAME_ADJ_ATTRACT_MODE_SOUND                      0
#define OM_GAME_ADJ_FINISHED                                1
#define SOUND_EFFECT_AP_GAME_ADJUSTMENT_CALLOUTS_START      (1800 + OM_GAME_ADJ_ATTRACT_MODE_SOUND)

unsigned long SoundSettingTimeout;
unsigned long SoundTestStart;
byte SoundTestSequence;
unsigned short RestoreBackgroundTrack = BACKGROUND_TRACK_NONE;
  
void RunOperatorMenu() {
  if (!Menus.UpdateMenu(CurrentTime)) {
    // Menu is done
    RPU_SetDisplayCredits(Credits, !FreePlayMode);
    Audio.StopAllAudio();
    RPU_TurnOffAllLamps();
    if (MachineState==MACHINE_STATE_ATTRACT) {
      RPU_SetDisplayBallInPlay(0, true);
    } else {
      RPU_SetDisplayBallInPlay(CurrentBallInPlay);
    }
    SoundSettingTimeout = 0;
    return;
  }

  // It's up to this function to eject balls if requested
  if (Menus.BallEjectInProgress()) {
    if (CountBallsInTrough()) {
      if (CurrentTime > (LastTimeBallServed+1500)) {
        LastTimeBallServed = CurrentTime;
        RPU_PushToSolenoidStack(SOL_OUTHOLE, 4, true);
      }
    }
  } else {
    LastTimeBallServed = 0;
  }
  
  byte topLevel = Menus.GetTopLevel();
  byte subLevel = Menus.GetSubLevel();

  if (Menus.HasTopLevelChanged()) {
    // Play an audio prompt for the top level
    SoundTestStart = 0;
    if (Audio.GetBackgroundSong()!=BACKGROUND_TRACK_NONE) {
      RestoreBackgroundTrack = Audio.GetBackgroundSong();
    }
    Audio.StopAllAudio();
    Audio.PlaySound((unsigned short)topLevel + SOUND_EFFECT_AP_TOP_LEVEL_MENU_ENTRY, AUDIO_PLAY_TYPE_WAV_TRIGGER, 10);
    if (Menus.GetTopLevel()==OPERATOR_MENU_GAME_RULES_LEVEL) Menus.SetNumSubLevels(4);
    if (Menus.GetTopLevel()==OPERATOR_MENU_BASIC_ADJ_MENU) {
      GetCPCSelection(0); // make sure CPC values have been read
      Menus.SetNumSubLevels(OM_BASIC_ADJ_FINISHED);
    }
    if (Menus.GetTopLevel()==OPERATOR_MENU_GAME_ADJ_MENU) Menus.SetNumSubLevels(OM_GAME_ADJ_FINISHED);
  }
  if (Menus.HasSubLevelChanged()) {
    SoundTestStart = 0;
    // Play an audio prompt for the sub level    
    Audio.StopAllAudio();
    if (topLevel==OPERATOR_MENU_SELF_TEST_MENU) {
      Audio.PlaySound((unsigned short)subLevel + SOUND_EFFECT_AP_TEST_LAMPS, AUDIO_PLAY_TYPE_WAV_TRIGGER, 10);

      if (subLevel==OPERATOR_MENU_TEST_SOUNDS) {
        SoundTestStart = CurrentTime + 1000;
        SoundTestSequence = 0;
      } else {
        SoundTestStart = 0;
      }
    } else if (topLevel==OPERATOR_MENU_AUDITS_MENU) {
      unsigned long *currentAdjustmentUL = NULL;
      byte currentAdjustmentStorageByte = 0;
      byte adjustmentType = OPERATOR_MENU_AUD_CLEARABLE;

      switch (subLevel) {
        case 0:
          Audio.PlaySound(SOUND_EFFECT_AP_AUDIT_TOTAL_PLAYS, AUDIO_PLAY_TYPE_WAV_TRIGGER, 10);
          currentAdjustmentStorageByte = RPU_TOTAL_PLAYS_EEPROM_START_BYTE;
          break;
        case 1:
          Audio.PlaySound(SOUND_EFFECT_AP_AUDIT_CHUTE_1_COINS, AUDIO_PLAY_TYPE_WAV_TRIGGER, 10);
          currentAdjustmentStorageByte = RPU_CHUTE_1_COINS_START_BYTE;
          break;
        case 2:
          Audio.PlaySound(SOUND_EFFECT_AP_AUDIT_CHUTE_2_COINS, AUDIO_PLAY_TYPE_WAV_TRIGGER, 10);
          currentAdjustmentStorageByte = RPU_CHUTE_2_COINS_START_BYTE;
          break;
        case 3:
          Audio.PlaySound(SOUND_EFFECT_AP_AUDIT_CHUTE_3_COINS, AUDIO_PLAY_TYPE_WAV_TRIGGER, 10);
          currentAdjustmentStorageByte = RPU_CHUTE_3_COINS_START_BYTE;
          break;
        case 4:
          Audio.PlaySound(SOUND_EFFECT_AP_AUDIT_TOTAL_REPLAYS, AUDIO_PLAY_TYPE_WAV_TRIGGER, 10);
          currentAdjustmentStorageByte = RPU_TOTAL_REPLAYS_EEPROM_START_BYTE;
          break;
        case 5:
          Audio.PlaySound(SOUND_EFFECT_AP_AUDIT_HISCR_BEAT, AUDIO_PLAY_TYPE_WAV_TRIGGER, 10);
          currentAdjustmentStorageByte = RPU_TOTAL_HISCORE_BEATEN_START_BYTE;
          break;
      }

      Menus.SetAuditControls(currentAdjustmentUL, currentAdjustmentStorageByte, adjustmentType);

    } else if (topLevel==OPERATOR_MENU_BASIC_ADJ_MENU) {
      Audio.PlaySound((unsigned short)subLevel + SOUND_EFFECT_AP_FREEPLAY, AUDIO_PLAY_TYPE_WAV_TRIGGER, 10);

      byte *currentAdjustmentByte = NULL;
      byte currentAdjustmentStorageByte = 0;
      byte adjustmentValues[8] = {0};
      byte numAdjustmentValues = 2;
      byte adjustmentType = OPERATOR_MENU_ADJ_TYPE_MIN_MAX;
      short parameterCallout = 0;
      unsigned long *currentAdjustmentUL = NULL;
      
      adjustmentValues[1] = 1;

      switch(subLevel) {
        case OM_BASIC_ADJ_IDS_FREEPLAY:
          currentAdjustmentByte = (byte *)&FreePlayMode;
          currentAdjustmentStorageByte = EEPROM_FREE_PLAY_BYTE;
          break;
        case OM_BASIC_ADJ_IDS_BALL_SAVE:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_LIST;
          numAdjustmentValues = 5;
          adjustmentValues[1] = 5;
          adjustmentValues[2] = 10;
          adjustmentValues[3] = 15;
          adjustmentValues[4] = 20;
          currentAdjustmentByte = &BallSaveNumSeconds;
          currentAdjustmentStorageByte = EEPROM_BALL_SAVE_BYTE;
          break;
        case OM_BASIC_ADJ_IDS_TILT_WARNINGS:
          adjustmentValues[1] = 2;
          currentAdjustmentByte = &MaxTiltWarnings;
          currentAdjustmentStorageByte = EEPROM_TILT_WARNING_BYTE;
          break;
        case OM_BASIC_ADJ_IDS_MUSIC_VOLUME:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_MIN_MAX;
          adjustmentValues[0] = 0;
          adjustmentValues[1] = 10;
          currentAdjustmentByte = &MusicVolume;
          currentAdjustmentStorageByte = EEPROM_MUSIC_VOLUME_BYTE;
          break;
        case OM_BASIC_ADJ_IDS_SOUNDFX_VOLUME:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_MIN_MAX;
          adjustmentValues[0] = 0;
          adjustmentValues[1] = 10;
          currentAdjustmentByte = &SoundEffectsVolume;
          currentAdjustmentStorageByte = EEPROM_SFX_VOLUME_BYTE;
          break;
        case OM_BASIC_ADJ_IDS_CALLOUTS_VOLUME:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_MIN_MAX;
          adjustmentValues[0] = 0;
          adjustmentValues[1] = 10;
          currentAdjustmentByte = &CalloutsVolume;
          currentAdjustmentStorageByte = EEPROM_CALLOUTS_VOLUME_BYTE;
          break;
        case OM_BASIC_ADJ_IDS_BALLS_PER_GAME:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_MIN_MAX;
          numAdjustmentValues = 8;
          adjustmentValues[0] = 3;
          adjustmentValues[1] = 10;
          currentAdjustmentByte = &BallsPerGame;
          currentAdjustmentStorageByte = EEPROM_BALLS_OVERRIDE_BYTE;
          break;
        case OM_BASIC_ADJ_IDS_TOURNAMENT_MODE:
          currentAdjustmentByte = (byte *)&TournamentScoring;
          currentAdjustmentStorageByte = EEPROM_TOURNAMENT_SCORING_BYTE;
          break;
        case OM_BASIC_ADJ_IDS_EXTRA_BALL_VALUE:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_SCORE_WITH_DEFAULT;
          currentAdjustmentUL = &ExtraBallValue;
          currentAdjustmentStorageByte = EEPROM_EXTRA_BALL_SCORE_UL;
          break;
        case OM_BASIC_ADJ_IDS_SPECIAL_VALUE:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_SCORE_WITH_DEFAULT;
          currentAdjustmentUL = &SpecialValue;
          currentAdjustmentStorageByte = EEPROM_SPECIAL_SCORE_UL;
          break;
        case OM_BASIC_ADJ_IDS_RESET_DURING_GAME:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_LIST;
          numAdjustmentValues = 5;
          adjustmentValues[0] = 0;
          adjustmentValues[1] = 1;
          adjustmentValues[2] = 2;
          adjustmentValues[3] = 3;
          adjustmentValues[4] = 99;
          currentAdjustmentByte = &TimeRequiredToResetGame;
          currentAdjustmentStorageByte = EEPROM_CRB_HOLD_TIME;
          parameterCallout = SOUND_EFFECT_OM_CRB_VALUES;
          break;
        case OM_BASIC_ADJ_IDS_SCORE_LEVEL_1:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_SCORE_WITH_DEFAULT;
          currentAdjustmentUL = &AwardScores[0];
          currentAdjustmentStorageByte = RPU_AWARD_SCORE_1_EEPROM_START_BYTE;
          break;
        case OM_BASIC_ADJ_IDS_SCORE_LEVEL_2:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_SCORE_WITH_DEFAULT;
          currentAdjustmentUL = &AwardScores[1];
          currentAdjustmentStorageByte = RPU_AWARD_SCORE_2_EEPROM_START_BYTE;
          break;
        case OM_BASIC_ADJ_IDS_SCORE_LEVEL_3:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_SCORE_WITH_DEFAULT;
          currentAdjustmentUL = &AwardScores[2];
          currentAdjustmentStorageByte = RPU_AWARD_SCORE_3_EEPROM_START_BYTE;
          break;
        case OM_BASIC_ADJ_IDS_SCORE_AWARDS:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_MIN_MAX_DEFAULT;
          adjustmentValues[1] = 7;
          currentAdjustmentByte = &ScoreAwardReplay;
          currentAdjustmentStorageByte = EEPROM_AWARD_OVERRIDE_BYTE;
          break;
        case OM_BASIC_ADJ_IDS_SCROLLING_SCORES:
          currentAdjustmentByte = (byte *)&ScrollingScores;
          currentAdjustmentStorageByte = EEPROM_SCROLLING_SCORES_BYTE;
          break;
        case OM_BASIC_ADJ_IDS_HISCR:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_SCORE_WITH_DEFAULT;
          currentAdjustmentUL = &HighScore;
          currentAdjustmentStorageByte = RPU_HIGHSCORE_EEPROM_START_BYTE;
          break;
        case OM_BASIC_ADJ_IDS_CREDITS:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_MIN_MAX;
          adjustmentValues[0] = 0;
          adjustmentValues[1] = 40;
          currentAdjustmentByte = &Credits;
          currentAdjustmentStorageByte = RPU_CREDITS_EEPROM_BYTE;
          break;
        case OM_BASIC_ADJ_IDS_CPC_1:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_CPC;
          adjustmentValues[0] = 0;
          adjustmentValues[1] = (NUM_CPC_PAIRS-1);
          currentAdjustmentByte = &(CPCSelection[0]);
          currentAdjustmentStorageByte = RPU_CPC_CHUTE_1_SELECTION_BYTE;
          parameterCallout = SOUND_EFFECT_OM_CPC_VALUES;
          break;
        case OM_BASIC_ADJ_IDS_CPC_2:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_CPC;
          adjustmentValues[0] = 0;
          adjustmentValues[1] = (NUM_CPC_PAIRS-1);
          currentAdjustmentByte = &(CPCSelection[1]);
          currentAdjustmentStorageByte = RPU_CPC_CHUTE_2_SELECTION_BYTE;
          parameterCallout = SOUND_EFFECT_OM_CPC_VALUES;
          break;
        case OM_BASIC_ADJ_IDS_CPC_3:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_CPC;
          adjustmentValues[0] = 0;
          adjustmentValues[1] = (NUM_CPC_PAIRS-1);
          currentAdjustmentByte = &(CPCSelection[2]);
          currentAdjustmentStorageByte = RPU_CPC_CHUTE_3_SELECTION_BYTE;
          parameterCallout = SOUND_EFFECT_OM_CPC_VALUES;
          break;
        case OM_BASIC_ADJ_IDS_MATCH_FEATURE:
          currentAdjustmentByte = (byte *)&MatchFeature;
          currentAdjustmentStorageByte = EEPROM_MATCH_FEATURE_BYTE;
          break;
      }

      Menus.SetParameterControls(   adjustmentType, numAdjustmentValues, adjustmentValues, parameterCallout,
                                    currentAdjustmentStorageByte, currentAdjustmentByte, currentAdjustmentUL );
    } else if (topLevel==OPERATOR_MENU_GAME_RULES_LEVEL) {
      Audio.PlaySound((unsigned short)subLevel + SOUND_EFFECT_AP_DIFFICULTY, AUDIO_PLAY_TYPE_WAV_TRIGGER, 10);
      byte *currentAdjustmentByte = &GameRulesSelection;
      byte adjustmentValues[8] = {0};
      adjustmentValues[0] = 0;
      // if one of the below parameters is installed, the "HasParameterChanged" 
      // check below will install Easy / Medium / Hard rules

      switch (subLevel) {
        case 0:
          adjustmentValues[1] = 1;
          break;
        case 1:
          adjustmentValues[1] = 2;
          break;
        case 2:
          adjustmentValues[1] = 3;
          break;
        case 3:
          adjustmentValues[1] = 4;
          break;
      }

      Menus.SetParameterControls(   OPERATOR_MENU_ADJ_TYPE_LIST, 2, adjustmentValues, (short)SOUND_EFFECT_OM_EASY_RULES_INSTRUCTIONS-1,
                                    EEPROM_GAME_RULES_SELECTION, currentAdjustmentByte, NULL );
                  
    } else if (topLevel==OPERATOR_MENU_GAME_ADJ_MENU) {
      Audio.PlaySound((unsigned short)subLevel + SOUND_EFFECT_AP_GAME_ADJUSTMENT_CALLOUTS_START, AUDIO_PLAY_TYPE_WAV_TRIGGER, 10);

      byte *currentAdjustmentByte = NULL;
      byte currentAdjustmentStorageByte = 0;
      byte adjustmentValues[8] = {0};
      byte numAdjustmentValues = 2;
      byte adjustmentType = OPERATOR_MENU_ADJ_TYPE_MIN_MAX;
      short parameterCallout = 0;
      unsigned long *currentAdjustmentUL = NULL;
      
      adjustmentValues[1] = 1;

      // CUSTOMIZATION TODO:
      // Add adjustment values here for game rules

      switch (subLevel) {
        case OM_GAME_ADJ_ATTRACT_MODE_SOUND:
          currentAdjustmentByte = (byte *)&AttractModeSound;
          currentAdjustmentStorageByte = EEPROM_ATTRACT_MODE_SOUND_BYTE;
          break;
      }
      
      Menus.SetParameterControls(   adjustmentType, numAdjustmentValues, adjustmentValues, parameterCallout,
                                    currentAdjustmentStorageByte, currentAdjustmentByte, currentAdjustmentUL );
    }    
  }

  if (Menus.HasParameterChanged()) {
    short parameterCallout = Menus.GetParameterCallout();
    if (parameterCallout) {
      Audio.StopAllAudio();
      Audio.PlaySound((unsigned short)parameterCallout + Menus.GetParameterID(), AUDIO_PLAY_TYPE_WAV_TRIGGER, 10);
    }
    if (Menus.GetTopLevel()==OPERATOR_MENU_GAME_RULES_LEVEL) {
      // Install the new rules level
      if (LoadRuleDefaults(GameRulesSelection)) {
        WriteParameters();
      }
    } else if (Menus.GetTopLevel()==OPERATOR_MENU_BASIC_ADJ_MENU) {
      if (Menus.GetSubLevel()==OM_BASIC_ADJ_IDS_MUSIC_VOLUME) {
        if (SoundSettingTimeout) Audio.StopAllAudio();
        Audio.PlaySound(SOUND_EFFECT_BACKGROUND_1, AUDIO_PLAY_TYPE_WAV_TRIGGER, MusicVolume);
        Audio.SetMusicVolume(MusicVolume);
        SoundSettingTimeout = CurrentTime + 5000;
      } else if (Menus.GetSubLevel()==OM_BASIC_ADJ_IDS_SOUNDFX_VOLUME) {
        if (SoundSettingTimeout) Audio.StopAllAudio();
        Audio.PlaySound(SOUND_EFFECT_SPINNER, AUDIO_PLAY_TYPE_WAV_TRIGGER, SoundEffectsVolume);
        Audio.SetSoundFXVolume(SoundEffectsVolume);
        SoundSettingTimeout = CurrentTime + 5000;
      } else if (Menus.GetSubLevel()==OM_BASIC_ADJ_IDS_CALLOUTS_VOLUME) {
        if (SoundSettingTimeout) Audio.StopAllAudio();
        Audio.PlaySound(SOUND_EFFECT_SHOOT_AGAIN, AUDIO_PLAY_TYPE_WAV_TRIGGER, CalloutsVolume);
        Audio.SetNotificationsVolume(CalloutsVolume);
        SoundSettingTimeout = CurrentTime + 3000;        
      }
    } else if (Menus.GetTopLevel()==OPERATOR_MENU_GAME_ADJ_MENU) {
    }
  }

  if (SoundSettingTimeout && CurrentTime>SoundSettingTimeout) {
    SoundSettingTimeout = 0;
    Audio.StopAllAudio();
  }

  if (SoundTestStart && CurrentTime>SoundTestStart) {
    if (SoundTestSequence==0) {
      PlayBackgroundSong(SOUND_EFFECT_BACKGROUND_1);
      SoundTestSequence = 1;
    } else if (SoundTestSequence==1 && CurrentTime>(SoundTestStart+5000)) {
      PlaySoundEffect(SOUND_EFFECT_SPINNER);
      SoundTestSequence = 2;
    } else if (SoundTestSequence==2 && CurrentTime>(SoundTestStart+10000)) {
      Audio.QueuePrioritizedNotification(SOUND_EFFECT_SHOOT_AGAIN, 0, 10, CurrentTime);
      SoundTestSequence = 3;
    }
  }
  
}



////////////////////////////////////////////////////////////////////////////
//
//  Diagnostics Mode
//
////////////////////////////////////////////////////////////////////////////

int RunDiagnosticsMode(int curState, boolean curStateChanged) {
  (void)curState;
  (void)curStateChanged;
}


////////////////////////////////////////////////////////////////////////////
//
//  Attract Mode
//
////////////////////////////////////////////////////////////////////////////

unsigned long AttractLastLadderTime = 0;
byte AttractLastLadderBonus = 0;
unsigned long AttractDisplayRampStart = 0;
byte AttractLastHeadMode = 255;
byte AttractLastPlayfieldMode = 255;
byte InAttractMode = false;
byte FlashyLights[] = {LAMP_LEFT_ROLLOVER_AND_RIGHT_INLANE, LAMP_RIGHT_ROLLOVER_AND_LEFT_RETURN, LAMP_MIDDLE_ROLLOVER_AND_STANDUP_SPOT, LAMP_LEFT_OUT_LANE_AND_3_BUMPER, LAMP_RIGHT_OUT_LANE_AND_1_BUMPER};
unsigned long AttractModeSoundTimer = 0;
const byte AttractModeSounds[] = {SOUND_EFFECT_METEOR_HIT, SOUND_EFFECT_METEOR_STORM_SECTOR_1, SOUND_EFFECT_LAMP_METEOR_DROP_3, SOUND_EFFECT_ROCKET_1_ARMED, SOUND_EFFECT_FRAGMENT_ELIMINATED, SOUND_EFFECT_5X_BONUS_COUNT, SOUND_EFFECT_LEFT_INLANE, SOUND_EFFECT_ORPHEUS_START, SOUND_EFFECT_ROCKET_2_MISSED, SOUND_EFFECT_NICE_SHOT};


int RunAttractMode(int curState, boolean curStateChanged) {

  int returnState = curState;

  if (curStateChanged) {
    RPU_DisableSolenoidStack();
    RPU_TurnOffAllLamps();
    RPU_SetDisableFlippers(true);
    if (DEBUG_MESSAGES) {
      Serial.write("Entering Attract Mode\n\r");
    }

    AttractLastHeadMode = 0;
    AttractLastPlayfieldMode = 0;

    // Set attract mode sound timer value
    AttractModeSoundTimer = CurrentTime + ATTRACT_MODE_SOUND_DELAY_MS;
  }

  // Check attract mode sound timer
  if(CurrentTime >= AttractModeSoundTimer && AttractModeSound) {
    byte RandomEffect = CurrentTime%10;
    PlaySoundEffect(AttractModeSounds[RandomEffect]);
    AttractModeSoundTimer = CurrentTime + ATTRACT_MODE_SOUND_DELAY_MS + 1;
  }

  // Alternate displays between high score and blank
  if (CurrentTime<16000) {
    Display_ClearOverride(0xFF);
    RPU_SetDisplayCredits(Credits, true);
    RPU_SetDisplayBallInPlay(0, true);
    Display_UpdateDisplays(0xFF);
  } else if ((CurrentTime / 8000) % 2 == 0) {
    if (AttractLastHeadMode != 2) {
      RPU_SetLampState(LAMP_HIGH_SCORE_TO_DATE, 1, 0, 250);
      RPU_SetLampState(LAMP_GAME_OVER, 0);
      Display_SetLastTimeScoreChanged(CurrentTime);
    }
    AttractLastHeadMode = 2;
    Display_UpdateDisplays(0xFF, false, false, false, HighScore);
  } else {
    if (AttractLastHeadMode != 3) {
      if (CurrentTime<32000) {
        for (int count = 0; count < 4; count++) {
          CurrentScores[count] = 0;
        }
        CurrentNumPlayers = 0;
      }
      RPU_SetLampState(LAMP_HIGH_SCORE_TO_DATE, 0);
      RPU_SetLampState(LAMP_GAME_OVER, 1);
//      RPU_SetDisplayCredits(Credits, true);
//      RPU_SetDisplayBallInPlay(0, true);
    }
    Display_ClearOverride(0xFF);
    
    AttractLastHeadMode = 3;
  }

  byte attractPlayfieldPhase = ((CurrentTime/5000)%5); 

  if (attractPlayfieldPhase!=AttractLastPlayfieldMode) {
    RPU_TurnOffAllLamps();
    AttractLastPlayfieldMode = attractPlayfieldPhase;
    if (attractPlayfieldPhase==2) GameMode = GAME_MODE_SKILL_SHOT;
    else GameMode = GAME_MODE_UNSTRUCTURED_PLAY;
    AttractLastLadderBonus = 1;
    AttractLastLadderTime = CurrentTime;
    if (attractPlayfieldPhase==2) {
      for (int count=0; count<5; count++) {
        RPU_SetLampState(FlashyLights[count], 1, count%2, 220+(count*100));
      }
    }
  }

  if (attractPlayfieldPhase < 2) {
    ShowLampAnimation(1, 40, CurrentTime, 14, false, false);
  } else if (attractPlayfieldPhase==3) {
    ShowLampAnimation(0, 40, CurrentTime, 11, false, false, 12);
  } else if (attractPlayfieldPhase==2) {
    if ((CurrentTime-AttractLastLadderTime)>200) {
      Bonus[0] = AttractLastLadderBonus;
      Bonus[1] = 7-AttractLastLadderBonus;
      Bonus[2] = AttractLastLadderBonus;
      ShowBonus();
      AttractLastLadderBonus += 1;
      if (AttractLastLadderBonus>7) AttractLastLadderBonus = 0;
      AttractLastLadderTime = CurrentTime;      
    }
    ShowMeteorLamps();
  } else {
    ShowLampAnimation(2, 40, CurrentTime, 14, false, false);
  }

  byte switchHit;
  while ( (switchHit = RPU_PullFirstFromSwitchStack()) != SWITCH_STACK_EMPTY ) {
    if (switchHit == SW_CREDIT_RESET) {
      if (AddPlayer(true)) returnState = MACHINE_STATE_INIT_GAMEPLAY;
    }
    if (switchHit == SW_COIN_1 || switchHit == SW_COIN_2 || switchHit == SW_COIN_3) {
      AddCoinToAudit(SwitchToChuteNum(switchHit));
      AddCoin(SwitchToChuteNum(switchHit));
    }
    if (switchHit == SW_SELF_TEST_SWITCH) {
      Menus.EnterOperatorMenu();
    }
  }

  // If the user was holding the menu button when the game started
  // then kick the balls
  if (CurrentTime < 4000) {
    if (RPU_ReadSingleSwitchState(SW_SELF_TEST_SWITCH)) {
      if (OperatorSwitchPressStarted==0) {
        OperatorSwitchPressStarted = CurrentTime;
      } else if (CurrentTime > (OperatorSwitchPressStarted+500)) {
        Menus.EnterOperatorMenu();
        Menus.BallEjectInProgress(true);
      }
    } else {
      OperatorSwitchPressStarted = 0;
    }
  }

  return returnState;
}





////////////////////////////////////////////////////////////////////////////
//
//  Game Play functions
//
////////////////////////////////////////////////////////////////////////////
byte CountBits(byte byteToBeCounted) {
  byte numBits = 0;

  for (byte count=0; count<8; count++) {
    numBits += (byteToBeCounted&0x01);
    byteToBeCounted = byteToBeCounted>>1;
  }

  return numBits;
}

byte CountBallsInTrough() {

  byte numBalls = RPU_ReadSingleSwitchState(SW_OUTHOLE);

  return numBalls;
}


void SetBonusX(byte newBonusX) {
  BonusX[CurrentPlayer] = newBonusX;
  if (BonusX[CurrentPlayer]>7) BonusX[CurrentPlayer] = 7;

  if (BonusX[CurrentPlayer]<3) {
    SpotMeteorTargetByte = 0x77;
    CollectRocketsByte = 0;
  } else if (BonusX[CurrentPlayer]==3) {
    SpotMeteorTargetByte = 0x33;
    CollectRocketsByte = 0x33;
  } else if (BonusX[CurrentPlayer]==4) {
    SpotMeteorTargetByte = 0x11;
    CollectRocketsByte = 0x77;
  } else {
    SpotMeteorTargetByte = 0x00;
    CollectRocketsByte = 0xFF;
  }  
}


boolean HandleSpotMeteorSwitch(byte switchHit, boolean overrideSpot=false) {
  
  byte lampMask;
  if (switchHit==SW_LEFT_IN_LANE || switchHit==SW_UPPER_ROLLOVER) lampMask = SPOT_METEOR_TARGET_LIGHT_3;
  else if (switchHit==SW_MIDDLE_ROLLOVER || switchHit==SW_STAND_UP) lampMask = SPOT_METEOR_TARGET_LIGHT_2;
  else lampMask = SPOT_METEOR_TARGET_LIGHT_1;

  if (lampMask==SPOT_METEOR_TARGET_LIGHT_2 && StandupAward!=1) return false;

  if ((SpotMeteorTargetByte & lampMask) || overrideSpot) {
    byte meteorTarget = 0;
    byte currentSwitches = CheckSequentialSwitches(SW_DROP_TARGET_R, 6);
    if (currentSwitches<0x3F) {
      if (SpotMeteorReverse) {
        for (int count=0; count<6; count++) {
          if (!(currentSwitches & (1<<count))) {
            meteorTarget = count;
            break;
          }
        }
      } else {
        for (int count=5; count>=0; count--) {
          if (!(currentSwitches & (1<<count))) {
            meteorTarget = count;
            break;
          }
        }
      }
      SpotMeteorReverse = !SpotMeteorReverse;
      RPU_PushToSolenoidStack(MeteorSolenoid[meteorTarget], 6);
      SpotMeteorTargetByte &= ~(lampMask);
  
      return true;
    }
  }

  return false;
}

boolean HandleStandupHit() {
  if (StandupAward==1) {
    // Hit with nothing lit
    CurrentScores[CurrentPlayer] += SCORE_STANDUP_TARGET_UNLIT;
    PlaySoundEffect(SOUND_EFFECT_STANDUP_UNLIT);    
  } else if (StandupAward==2) {
    AwardExtraBall();
  } else if (StandupAward==3) {
    AwardSpecial();
  }
  StandupAward = 1;
  return true;
}


void HandleMeteorDropTargetHit(byte switchHit) {

  byte targetBit = 1<<(switchHit-SW_DROP_TARGET_R);

  // Check to see if this drop target is not already down
  if ((CurrentMeteorTargetsValid&targetBit)==0) {

    if (switchHit==SW_DROP_TARGET_M || switchHit==SW_DROP_TARGET_R) {
      CurrentScores[CurrentPlayer] += SCORE_DROP_TARGET_6000;
    } else {
      CurrentScores[CurrentPlayer] += SCORE_DROP_TARGET_2000;
    }

    byte currentSwitches = CheckSequentialSwitches(SW_DROP_TARGET_R, 6);
    
    if (GameMode<GAME_MODE_ORPHEUS_START) {
      if (currentSwitches==0x3F) {
        RPU_PushToTimedSolenoidStack(SOL_RESET_METEOR_BANK, 15, CurrentTime+500);
        if (AwardScoring==AWARD_SCORING_WIZARD) {
          PlaySoundEffect(SOUND_EFFECT_SUPER_JACKPOT);
          CurrentScores[CurrentPlayer] += SCORE_SUPER_JACKPOT;
        }
        SetBonusX(BonusX[CurrentPlayer]+1);
          if (GameMode!=GAME_MODE_METEOR_STORM_START && GameMode!=GAME_MODE_METEOR_STORM && GameMode!=GAME_MODE_WIZARD) {
            GameMode = GAME_MODE_METEOR_STORM_START;
            GameModeStartTime = 0;
          }
        BonusXAnimationStart = CurrentTime;
        PlaySoundEffect(SOUND_EFFECT_LAMP_METEOR_DROP_6);
      } else {
        PlaySoundEffect(SOUND_EFFECT_LAMP_METEOR_DROP_1 + (CountBits(currentSwitches)-1));
      }
    } else {
      CurrentMeteorTargetsValid |= targetBit;
      // Special rules for Orpheus
      if (switchHit==SkillShotWowLit) {
        CurrentScores[CurrentPlayer] += SCORE_ORPHEUS_TARGET_WOW;
        PlaySoundEffect(SOUND_EFFECT_ORPHEUS_HIT_1 + CurrentTime%2);
        if (currentSwitches==0x3F) {
          CurrentMeteorTargetsValid = 0x3F;
          GoalsCompletedFlags[CurrentPlayer] |= GOAL_ORPHEUS_COMPLETED;
          GameMode = GAME_MODE_ORPHEUS_FINISH;
          GameModeStartTime = 0; 
          GameModeEndTime = CurrentTime + ORPHEUS_FINISH_DURATION_LONG;
        }
        LastOrpheusHitTime = CurrentTime;
      } else {
        CurrentScores[CurrentPlayer] += SCORE_ORPHEUS_TARGET_NORMAL;
        SkillShotWowLit = 0;
        // if they missed the Orpheus chunk, put them into fragmented mode
        if (GameMode!=GAME_MODE_ORPHEUS_FRAGMENTED) {
          GameMode = GAME_MODE_ORPHEUS_FRAGMENTED;
          GameModeStartTime = 0;
        } else {
          
        }
        if (currentSwitches==0x3F) {
          CurrentMeteorTargetsValid = 0x3F;
          GameModeStartTime = 0;
          GoalsCompletedFlags[CurrentPlayer] |= GOAL_ORPHEUS_FRAGMENT_COMPLETED;
          GameMode = GAME_MODE_ORPHEUS_FINISH;
          GameModeEndTime = CurrentTime + ORPHEUS_FINISH_DURATION_SHORT;
        }
        LastOrpheusHitTime = CurrentTime;
      }      
    }
    
  }  
}


byte ResetSolenoid[] = {SOL_RESET_1_BANK, SOL_RESET_2_BANK, SOL_RESET_3_BANK};
byte StartingBankSwitch[] = {SW_LOWER_LEFT_3_TOP, SW_TOP_LEFT_3_TOP, SW_RIGHT_3_TOP};

void Handle3BankHit(byte switchNum, byte bankNum) {

  // Only score if this switch hasn't been hit yet
  byte switchMask = (1<<(switchNum-StartingBankSwitch[bankNum]));
  if (!(Current3BankTargetsHit[bankNum]&switchMask)) {
    if (TargetAward && TargetAwardPhase==(bankNum) && switchMask==0x04) {
      if (TargetAward==1) {
        AwardExtraBall();
      } else if (TargetAward==2) {
        AwardSpecial();
      }
      TargetAward = 0;
    }
    if (GameMode==GAME_MODE_METEOR_STORM) {
      if (!(RocketsArmed&(1<<(bankNum)))) PlaySoundEffect(SOUND_EFFECT_ROCKET_1_ARMED + (bankNum));
    }
    RocketsArmed |= 1<<(bankNum);
    
    Current3BankTargetsHit[bankNum] |= switchMask;
    AddToBonus(bankNum);
/*    Bonus[bankNum] += 1;
    if (Bonus[bankNum]>MAX_DISPLAY_BONUS) Bonus[bankNum] = MAX_DISPLAY_BONUS;
    BonusAnimationStart[bankNum] = CurrentTime;
    BonusAnimationShown[bankNum] = 0;
*/    
  }

  if (CheckSequentialSwitches(StartingBankSwitch[bankNum], 3)==0x07) {
    RPU_PushToTimedSolenoidStack(ResetSolenoid[bankNum], 12, 250);
    Current3BankTargetsHit[bankNum] = 0;
    if (GameMode==GAME_MODE_METEOR_STORM && OrpheusRound[CurrentPlayer]<LAUNCH_ROCKETS_WITH_BANK_CLEAR_UNTIL_LEVEL) {  
      RocketLaunchedTime[bankNum] = CurrentTime;
      PlaySoundEffect(SOUND_EFFECT_ROCKET_LAUNCHED);
      RocketsArmed &= ~(1<<(bankNum));
      RocketsFiredFromBank = true;
    } else if (AwardScoring==AWARD_SCORING_WIZARD) {
      PlaySoundEffect(SOUND_EFFECT_JACKPOT);
      CurrentScores[CurrentPlayer] += SCORE_JACKPOT;
    }
  }

}


int InitGamePlay() {

  if (DEBUG_MESSAGES) {
    Serial.write("Starting game\n\r");
  }

  // The start button has been hit only once to get
  // us into this mode, so we assume a 1-player game
  // at the moment
  RPU_EnableSolenoidStack();
  RPU_SetCoinLockout((Credits >= MaximumCredits) ? true : false);
  RPU_TurnOffAllLamps();
  Audio.StopAllAudio();

  // Reset displays & game state variables
  for (int count = 0; count < 4; count++) {
    // Initialize game-specific variables
    BonusX[count] = 1; 
    MeteorStatus[count] = 0;
    NumberOfMeteorsHit[count] = 0;
    OrpheusRound[count] = 0;
    NumberOfSpins[count] = 0;    
    GoalsCompletedFlags[count] = 0;
    NumberOfBallsLocked = 0;
  }
  memset(CurrentScores, 0, 4*sizeof(unsigned long));
  
  SamePlayerShootsAgain = false;
  CurrentBallInPlay = 1;
  CurrentNumPlayers = 1;
  CurrentPlayer = 0;
  Display_ClearOverride(0xFF);
  NumberOfBallsInPlay = 0;


  return MACHINE_STATE_INIT_NEW_BALL;
}


int InitNewBall(bool curStateChanged, byte playerNum, int ballNum) {

  // If we're coming into this mode for the first time
  // then we have to do everything to set up the new ball
  if (curStateChanged) {

    if (GameRulesSelection==GAME_RULES_PROGRESSIVE) {
      // if the operator has installed "progressive" rules, 
      // then we change the rules based on player number.
      // If we're 1 player, it's EASY
      // First player is EASY, last player is HARD,
      // and in-between players are MEDIUM
      byte rulesLevel = GAME_RULES_EASY;
      if (CurrentPlayer==0) rulesLevel = GAME_RULES_EASY;
      else if (CurrentPlayer==(CurrentNumPlayers-1)) rulesLevel = GAME_RULES_HARD;
      else rulesLevel = GAME_RULES_MEDIUM;
      LoadRuleDefaults(rulesLevel);
      if (CurrentBallInPlay==1) QueueNotification(SOUND_EFFECT_VP_GAME_RULES_EASY + (rulesLevel-GAME_RULES_EASY), 10);
    }

    RPU_TurnOffAllLamps();
    SamePlayerShootsAgain = false;
    BallFirstSwitchHitTime = 0;

    RPU_SetDisableFlippers(false);
    RPU_EnableSolenoidStack();
    RPU_SetDisplayCredits(Credits, true);
    if (CurrentNumPlayers>1 && (ballNum!=1 || playerNum!=0)) PlaySoundEffect(SOUND_EFFECT_PLAYER_1_UP+playerNum);

    RPU_SetDisplayBallInPlay(ballNum);
//    RPU_SetLampState(LAMP_TILT, 0);

    if (BallSaveNumSeconds > 0) {
      RPU_SetLampState(LAMP_SHOOT_AGAIN, 1, 0, 500);
    }

    BallSaveUsed = false;
    BallTimeInTrough = 0;
    NumTiltWarnings = 0;
    LastTiltWarningTime = 0;

    // Initialize game-specific start-of-ball lights & variables
    GameModeStartTime = 0;
    GameMode = GAME_MODE_SKILL_SHOT;
    SkillShotHit = false;
    StandupAward = 1;
    StandupHits = 0;
    StandupResetTime = 0;
    ExtraBallCollected = false;
    SpecialCollected = false;
    SpinnerValue = 0;
//    for (int i=0; i<3; i++) {
//      Bonus[i] = 0;
//      BonusAnimationStart[i] = 0;
//    }
    memset(Bonus, 0, 3*sizeof(byte));
    memset(BonusAnimationStart, 0, 3*sizeof(unsigned long));
    SpotMeteorReverse = false;
    SetBonusX(BonusX[CurrentPlayer]);
    Display_ResetDisplayTrackingVariables();

    // Start appropriate mode music
    if (RPU_ReadSingleSwitchState(SW_OUTHOLE)) {
      RPU_PushToTimedSolenoidStack(SOL_OUTHOLE, 4, CurrentTime + 600);
      LastTimeBallServed = CurrentTime + 600;
    }

    for (byte count=0; count<3; count++) {
      RPU_PushToTimedSolenoidStack(ResetSolenoid[count], 12, CurrentTime + 100*((unsigned long)count));
      Current3BankTargetsHit[count] = 0;
    }
    
    Rocket1BumperAnimationTime = 0;
    Rocket3BumperAnimationTime = 0;
    CollectRocketAnimationStart = 0;
    CollectRocketCurrentLevel = 0;
    SuperSpinnerEndTime = 0;
    MeteorHitStarted = false;
    MeteorHitStartTime = 0;
    MeteorStormSectors = 0;
    OrpheusPhase = 0;
    AwardScoring = 0;
    TargetAward = 0;
    NumberOfBallsInPlay = 1;
    SetBallSave(0); // Set to 0 here, and it will be initialized in RunGamePlayMode

    if (GoalsCompletedFlags[CurrentPlayer]&GOAL_WIZARD_MODE_COLLECTED) {
      GoalsCompletedFlags[CurrentPlayer] = 0;
    }


    // Reset drop targets for skill shot
    SetMeteorDropTargets(0, 450);

  }

  // We should only consider the ball initialized when
  // the ball is no longer triggering the SW_OUTHOLE
  if (RPU_ReadSingleSwitchState(SW_OUTHOLE)) {
    return MACHINE_STATE_INIT_NEW_BALL;
  } else {
    return MACHINE_STATE_NORMAL_GAMEPLAY;
  }

}


void PlayBackgroundSongBasedOnBall(byte ballNum) {
  if (ballNum==1) {
    PlayBackgroundSong(SOUND_EFFECT_BACKGROUND_1);
  } else if (ballNum==BallsPerGame) {
    PlayBackgroundSong(SOUND_EFFECT_BACKGROUND_6);
  } else {
    PlayBackgroundSong(SOUND_EFFECT_BACKGROUND_2 + CurrentTime%4);
  }
}


void AdvanceMeteors() {

  for (int count=0; count<3; count++) {
    if (MeteorStormSectors & (1<<count)) {
      byte newDescent = 7 - ((CurrentTime-MeteorStormStart)/((unsigned long)MeteorStormDescentSpeed[count]*500));
      MeteorStormDescent[count] = newDescent;
      if (newDescent==0) {
        MeteorStormSectors &= ~(1<<count);
        PlaySoundEffect(SOUND_EFFECT_METEOR_IMPACT);
      }
    }
  }
  
}


byte OrpheusMeteorMask[] = {0x2D, 0x17, 0x15, 0x12, 0x07, 0x05, 0x02};
byte GetOrpheusMeteorMask() {
  byte index = OrpheusRound[CurrentPlayer];
  if (index>6) index = 6;
  return OrpheusMeteorMask[index];
}

byte OrpheusFragmentMask[] = {0x2D, 0x15, 0x12, 0x15, 0x12, 0x00, 0x02};
byte GetOrpheusFragmentMask() {
  byte index = OrpheusRound[CurrentPlayer];
  if (index>6) index = 6;
  return OrpheusFragmentMask[index];
}


// This function manages all timers, flags, and lights
int ManageGameMode() {
  int returnState = MACHINE_STATE_NORMAL_GAMEPLAY;

//  byte currentStep = 0;
  boolean specialAnimationRunning = false;
  byte bitMask;

  switch ( (GameMode) ) {
    case GAME_MODE_SKILL_SHOT:
      if (GameModeStartTime==0) {
        GameModeStartTime = CurrentTime;
        GameModeEndTime = 0;
        if (CurrentBallInPlay==1 && CurrentScores[CurrentPlayer]==0) {
          PlayBackgroundSong(SOUND_EFFECT_GAME_START_INTRO);
        } else {
          PlayBackgroundSongBasedOnBall(CurrentBallInPlay);
        }
      }
      if (BallFirstSwitchHitTime!=0) {
        // Reset Meteor Drop Targets for player
        SetMeteorDropTargets(MeteorStatus[CurrentPlayer], 450);
            
        // Something has been hit, so we shouldn't be in skill shot anymore
        GameMode = GAME_MODE_UNSTRUCTURED_PLAY;
        GameModeStartTime = 0;
      }
    break;    
    
    case GAME_MODE_UNSTRUCTURED_PLAY:
      // If this is the first time in this mode
      if (GameModeStartTime==0) {
        GameModeStartTime = CurrentTime;
        if (CurrentBallInPlay==1) PlayBackgroundSongBasedOnBall(CurrentBallInPlay);
      }

      if (SkillShotHit && (CurrentTime-GameModeStartTime)<1500) {
        specialAnimationRunning = true;
        ShowLampAnimation(2, 40, (CurrentTime-GameModeStartTime), 14, false, true);
      }

      if (CurrentTime>SuperSpinnerEndTime) {
        SuperSpinnerEndTime = 0;
      }

      if (Bonus[0]>0 && Bonus[0]==Bonus[1] && Bonus[0]==Bonus[2]) {
        if (Bonus[0]<7) {
          if (!ExtraBallCollected) TargetAward = 1;
        } else {
          if (!SpecialCollected) TargetAward = 2;
        }
      } else {
        TargetAward = 0;
      }

      if ((GoalsCompletedFlags[CurrentPlayer])==0x0F) {
        GameMode = GAME_MODE_WIZARD;
        GameModeStartTime = 0;
        GameModeEndTime = CurrentTime + WIZARD_MODE_DURATION;
      }

      if (CurrentTime > StandupResetTime) {
        StandupResetTime = 0;
        StandupAward = 1;
      }
      
    break;
    
    case GAME_MODE_METEOR_STORM_START:
      if (GameModeStartTime==0) {
        GameModeStartTime = CurrentTime;
        GameModeEndTime = CurrentTime + METEOR_STORM_START_DURATION;
        if (NumberOfMeteorsHit[CurrentPlayer]==0) PlaySoundEffect(SOUND_EFFECT_METEOR_STORM_START);
        else PlaySoundEffect(SOUND_EFFECT_METEOR_STORM_START_2 + CurrentTime%3);
        MeteorStormSectors = 0;
        RocketsArmed = 0;     
      }
      
      if (CurrentTime>GameModeEndTime) {
        GameMode = GAME_MODE_METEOR_STORM;
        GameModeStartTime = 0;
      }
    break;
    
    case GAME_MODE_METEOR_STORM:
      if (GameModeStartTime==0) {
        GameModeStartTime = CurrentTime;
        GameModeEndTime = CurrentTime + METEOR_STORM_TIME_TO_ARM;
        StartMeteorStorm();
      }

      AdvanceMeteors();

      if (MeteorHitStartTime!=0) {
        if (MeteorHitStarted) {
          MeteorHitStarted = false;
          RPU_TurnOffAllLamps();
        }

        specialAnimationRunning = true;
        ShowBonusLamps(false);
        ShowLampAnimation(1, 40, (CurrentTime-MeteorHitStartTime), 14, false, false);

        if (CurrentTime>MeteorHitEndTime) {
          RPU_TurnOffAllLamps();
          MeteorHitStartTime = 0;
          // If this is the last meteor, start a new one
          if (MeteorStormSectors==0) {
            StartMeteorStorm();
          }
        }
      } else if (MeteorStormSectors==0) {
        GameModeStartTime = 0;
        GameMode = GAME_MODE_UNSTRUCTURED_PLAY;
      } else if (NumberOfMeteorsHit[CurrentPlayer]>=GetNextMeteorGoal()) {
        GameModeStartTime = 0;
        GameMode = GAME_MODE_ORPHEUS_START;
        //NumberOfMeteorsHit[CurrentPlayer] = 0;
      }
    break;
    
    case GAME_MODE_ORPHEUS_START:
      if (GameModeStartTime==0) {
        GameModeStartTime = CurrentTime;
        GameModeEndTime = CurrentTime + ORPHEUS_START_DURATION;
        RPU_TurnOffAllLamps();
        Audio.StopAllAudio();
        PlaySoundEffect(SOUND_EFFECT_ORPHEUS_START);
        SetMeteorDropTargets(0x3F, 0, false);
        OrpheusPhase = 0;
      }

      specialAnimationRunning = true;
      ShowLampAnimation(0, 40, (CurrentTime-GameModeStartTime), 11, false, false, 12);
      if (CurrentTime>GameModeEndTime) {
        GameModeStartTime = 0;
        GameMode = GAME_MODE_ORPHEUS;
      }
    break;
    
    case GAME_MODE_ORPHEUS:
      if (GameModeStartTime==0) {
        GameModeStartTime = CurrentTime;
        GameModeEndTime = CurrentTime + ORPHEUS_DURATION;
        SetMeteorDropTargets(GetOrpheusMeteorMask());
        RPU_TurnOffAllLamps();
        PlayBackgroundSong(SOUND_EFFECT_BACKGROUND_ORPHEUS);
        LastOrpheusHitTime = CurrentTime;
        OrpheusPhase = 0;
      }

      if ((CurrentTime-LastOrpheusHitTime)>10000) {
        OrpheusPhase = (OrpheusPhase+1)%6;
        bitMask = GetOrpheusMeteorMask();
        for (byte count=0; count<OrpheusPhase; count++) {
          bitMask = (bitMask>>1)|(bitMask<<5);
        }
        LastOrpheusHitTime = CurrentTime;
        SetMeteorDropTargets(bitMask);
      }
      
      if (CurrentTime>GameModeEndTime) {
        GameModeStartTime = 0;
        GameMode = GAME_MODE_ORPHEUS_FAIL;
      }
    break;
    
    case GAME_MODE_ORPHEUS_FRAGMENTED:
      if (GameModeStartTime==0) {
        GameModeStartTime = CurrentTime;
        PlaySoundEffect(SOUND_EFFECT_ORPHEUS_FRAGMENTED);
        GameModeEndTime = CurrentTime + ORPHEUS_FRAGMENTED_DURATION;
        SetMeteorDropTargets(0x33);
        OrpheusPhase = 0;
        LastOrpheusHitTime = CurrentTime;
      }

      if ((CurrentTime-LastOrpheusHitTime)>5000) {
        OrpheusPhase = (OrpheusPhase+1)%6;
        bitMask = GetOrpheusFragmentMask();
        for (byte count=0; count<OrpheusPhase; count++) {
          bitMask = (bitMask>>1) | (bitMask<<5);
        }
        SetMeteorDropTargets(bitMask);
        LastOrpheusHitTime = CurrentTime;
      }
      
      if (CurrentTime>GameModeEndTime) {
        GameModeStartTime = 0;
        GameMode = GAME_MODE_ORPHEUS_FAIL;
      }
    break;

    case GAME_MODE_ORPHEUS_FAIL:
      if (GameModeStartTime==0) {
        GameModeStartTime = CurrentTime;
        GameModeEndTime = CurrentTime + ORPHEUS_FAIL_DURATION;
        PlaySoundEffect(SOUND_EFFECT_MISSION_FAILED);
        RPU_TurnOffAllLamps();
      }
      specialAnimationRunning = true;

      if (CurrentTime>GameModeEndTime) {
        GameModeStartTime = 0;
        GameMode = GAME_MODE_UNSTRUCTURED_PLAY;
        SetMeteorDropTargets(0);
        PlayBackgroundSongBasedOnBall(CurrentBallInPlay);
        OrpheusRound[CurrentPlayer] += 1;
      }      
    break;
    
    case GAME_MODE_ORPHEUS_FINISH:
      if (GameModeStartTime==0) {
        RPU_TurnOffAllLamps();
        Audio.StopAllAudio();
        PlayBackgroundSong(SOUND_EFFECT_ORPHEUS_FINISH);
        SetMeteorDropTargets(0x3F, 0, false);
        GameModeStartTime = CurrentTime;
        AwardScoring = AWARD_SCORING_ORPHEUS;
        // Game mode end time is controlled by caller
      }
      
      specialAnimationRunning = true;
      ShowLampAnimation(2, 20, (CurrentTime-GameModeStartTime), 11, false, false);

      if (CurrentTime>GameModeEndTime) {
        GameModeStartTime = 0;
        GameMode = GAME_MODE_UNSTRUCTURED_PLAY;
        SetMeteorDropTargets(0);
        PlayBackgroundSongBasedOnBall(CurrentBallInPlay);
        OrpheusRound[CurrentPlayer] += 1;
        AwardScoring = 0;
      }
    break;

    case GAME_MODE_WIZARD:
      if (GameModeStartTime==0) {
        RPU_TurnOffAllLamps();
        Audio.StopAllAudio();
        PlayBackgroundSong(SOUND_EFFECT_WIZARD_MODE);
        SetMeteorDropTargets(0x00, 0);
        GameModeStartTime = CurrentTime;
        AwardScoring = AWARD_SCORING_WIZARD;
        GoalsCompletedFlags[CurrentPlayer] |= GOAL_WIZARD_MODE_COLLECTED;
      }
      
      specialAnimationRunning = true;
      ShowLampAnimation(1, 80, CurrentTime, 14, false, false);
      
      if (CurrentTime>GameModeEndTime) {
        GameModeStartTime = 0;
        GameMode = GAME_MODE_UNSTRUCTURED_PLAY;
        SetMeteorDropTargets(0);
        PlayBackgroundSongBasedOnBall(CurrentBallInPlay);
        AwardScoring = 0;
      }
    break;

  }

  if ( GameModeStartTime!=0 && !specialAnimationRunning && NumTiltWarnings<=MaxTiltWarnings ) {
    ShowMeteorLamps();
    ShowStandupTargetLamps();
    ShowBonusLamps();
    ShowBonusXLamps();
    ShowSpinnerLamps();
    ShowShootAgainLamp();
    ShowSpotMeteorLamps();
    ShowRocketBumperLamps();
    ShowRocketDropLamps();
  }

  if (Display_UpdateDisplays(0xFF, false, (BallFirstSwitchHitTime == 0) ? true : false, (BallFirstSwitchHitTime > 0 && ((CurrentTime - Display_GetLastTimeScoreChanged()) > 2000)) ? true : false)) {
    Audio.StopSound(SOUND_EFFECT_SCORE_TICK);
    PlaySoundEffect(SOUND_EFFECT_SCORE_TICK);
  }

  // Check to see if ball is in the outhole
  if (CountBallsInTrough() > (TotalBallsLoaded - (NumberOfBallsInPlay + NumberOfBallsLocked))) {

    if (BallTimeInTrough == 0) {
      // If this is the first time we're seeing too many balls in the trough, we'll wait to make sure
      // everything is settled
      BallTimeInTrough = CurrentTime;
    } else {

      // Make sure the ball stays on the sensor for at least
      // 0.5 seconds to be sure that it's not bouncing or passing through
      if ((CurrentTime - BallTimeInTrough) > 750) {

        if ((BallFirstSwitchHitTime == 0 && NumTiltWarnings <= MaxTiltWarnings)) {
          // Nothing hit yet, so return the ball to the player
          RPU_PushToTimedSolenoidStack(SOL_OUTHOLE, 4, CurrentTime);
          BallTimeInTrough = 0;
          returnState = MACHINE_STATE_NORMAL_GAMEPLAY;
        } else {
          // if we haven't used the ball save, and we're under the time limit, then save the ball
          if (BallSaveEndTime && CurrentTime < (BallSaveEndTime + BALL_SAVE_GRACE_PERIOD)) {
            RPU_PushToTimedSolenoidStack(SOL_OUTHOLE, 4, CurrentTime + 100);

            RPU_SetLampState(LAMP_SHOOT_AGAIN, 0);
            BallTimeInTrough = CurrentTime;
            returnState = MACHINE_STATE_NORMAL_GAMEPLAY;

            if (NumberOfBallSavesRemaining && NumberOfBallSavesRemaining != 0xFF) {
              NumberOfBallSavesRemaining -= 1;
              if (NumberOfBallSavesRemaining == 0) {
                BallSaveEndTime = 0;
                if (DEBUG_MESSAGES) {
                  Serial.write("Last ball save\n");
                }
                QueueNotification(SOUND_EFFECT_SHOOT_AGAIN, 10);
              } else {
                if (DEBUG_MESSAGES) {
                  Serial.write("Not last ball save\n");
                }                
              }
            }

          } else {

            NumberOfBallsInPlay -= 1;
            if (NumberOfBallsInPlay == 0) {
              Display_ClearOverride(0xFF);
              Audio.StopAllAudio();

              if (GameMode>=GAME_MODE_ORPHEUS_START) {
                OrpheusRound[CurrentPlayer] += 1;
              }
              // Turn off bonus animations
              memset(BonusAnimationStart, 0, 3*sizeof(unsigned long));

              returnState = MACHINE_STATE_COUNTDOWN_BONUS;
            }
          }
        }
      }
    }
  } else {
    BallTimeInTrough = 0;
  }

  return returnState;
}



unsigned long CountdownStartTime = 0;
unsigned long LastCountdownReportTime = 0;
unsigned long BonusCountDownEndTime = 0;

int CountdownBonus(boolean curStateChanged) {

  // If this is the first time through the countdown loop
  if (curStateChanged) {

    CountdownStartTime = CurrentTime;
    ShowBonus();

    LastCountdownReportTime = CountdownStartTime;
    BonusCountDownEndTime = 0xFFFFFFFF;
  }

  if ((CurrentTime - LastCountdownReportTime) > 200) {

    if (Bonus[0] || Bonus[1] || Bonus[2]) {

      // Only give sound & score if this isn't a tilt
      if (NumTiltWarnings <= MaxTiltWarnings) {
        PlaySoundEffect(SOUND_EFFECT_BONUS_COUNT + (BonusX[CurrentPlayer]-1));
        CurrentScores[CurrentPlayer] += SCORE_BONUS_LAMP*((unsigned long)BonusX[CurrentPlayer]);
      }

      for (int i=0; i<3; i++) {
        if (Bonus[i]) {
          Bonus[i] -= 1;
          break;
        }
      }
      
      ShowBonus();
    } else if (BonusCountDownEndTime == 0xFFFFFFFF) {
      BonusCountDownEndTime = CurrentTime + 1000;
    }
    LastCountdownReportTime = CurrentTime;
  }

  if (CurrentTime > BonusCountDownEndTime) {

    // Reset any lights & variables of goals that weren't completed
    BonusCountDownEndTime = 0xFFFFFFFF;
    return MACHINE_STATE_BALL_OVER;
  }

  return MACHINE_STATE_COUNTDOWN_BONUS;
}



void CheckHighScores() {
  unsigned long highestScore = 0;
  int highScorePlayerNum = 0;
  for (int count = 0; count < CurrentNumPlayers; count++) {
    if (CurrentScores[count] > highestScore) highestScore = CurrentScores[count];
    highScorePlayerNum = count;
  }

  if (highestScore > HighScore) {
    HighScore = highestScore;
    if (HighScoreReplay) {
      AddCredit(false, 3);
      RPU_WriteULToEEProm(RPU_TOTAL_REPLAYS_EEPROM_START_BYTE, RPU_ReadULFromEEProm(RPU_TOTAL_REPLAYS_EEPROM_START_BYTE) + 3);
    }
    RPU_WriteULToEEProm(RPU_HIGHSCORE_EEPROM_START_BYTE, highestScore);
    RPU_WriteULToEEProm(RPU_TOTAL_HISCORE_BEATEN_START_BYTE, RPU_ReadULFromEEProm(RPU_TOTAL_HISCORE_BEATEN_START_BYTE) + 1);

    for (int count = 0; count < 4; count++) {
      if (count == highScorePlayerNum) {
        RPU_SetDisplay(count, CurrentScores[count], true, 2);
      } else {
        RPU_SetDisplayBlank(count, 0x00);
      }
    }

    RPU_PushToTimedSolenoidStack(SOL_KNOCKER, 3, CurrentTime, true);
    RPU_PushToTimedSolenoidStack(SOL_KNOCKER, 3, CurrentTime + 300, true);
    RPU_PushToTimedSolenoidStack(SOL_KNOCKER, 3, CurrentTime + 600, true);
  }
}


void AddToBonus(byte bonusNum) {
  Bonus[bonusNum] += 1;
  if (Bonus[bonusNum]>MAX_DISPLAY_BONUS) Bonus[bonusNum] = MAX_DISPLAY_BONUS;
  BonusAnimationStart[bonusNum] = CurrentTime;
  BonusAnimationShown[bonusNum] = 0;
}


unsigned long MatchSequenceStartTime = 0;
unsigned long MatchDelay = 150;
byte MatchDigit = 0;
byte NumMatchSpins = 0;
byte ScoreMatches = 0;

int ShowMatchSequence(boolean curStateChanged) {
  if (!MatchFeature) return MACHINE_STATE_ATTRACT;

  if (curStateChanged) {
    MatchSequenceStartTime = CurrentTime;
    MatchDelay = 1500;
//    MatchDigit = random(0, 10);
    MatchDigit = CurrentTime%10;
    NumMatchSpins = 0;
    RPU_SetLampState(LAMP_MATCH, 1, 0);
    RPU_SetDisableFlippers();
    ScoreMatches = 0;
  }

  if (NumMatchSpins < 40) {
    if (CurrentTime > (MatchSequenceStartTime + MatchDelay)) {
      MatchDigit += 1;
      if (MatchDigit > 9) MatchDigit = 0;
      //PlaySoundEffect(10+(MatchDigit%2));
      PlaySoundEffect(SOUND_EFFECT_MATCH_SPIN);
      RPU_SetDisplayBallInPlay((int)MatchDigit * 10);
      MatchDelay += 50 + 4 * NumMatchSpins;
      NumMatchSpins += 1;
      RPU_SetLampState(LAMP_MATCH, NumMatchSpins % 2, 0);

      if (NumMatchSpins == 40) {
        RPU_SetLampState(LAMP_MATCH, 0);
        MatchDelay = CurrentTime - MatchSequenceStartTime;
      }
    }
  }

  if (NumMatchSpins >= 40 && NumMatchSpins <= 43) {
    if (CurrentTime > (MatchSequenceStartTime + MatchDelay)) {
      if ( (CurrentNumPlayers > (NumMatchSpins - 40)) && ((CurrentScores[NumMatchSpins - 40] / 10) % 10) == MatchDigit) {
        ScoreMatches |= (1 << (NumMatchSpins - 40));
        AddSpecialCredit();
        MatchDelay += 1000;
        NumMatchSpins += 1;
        RPU_SetLampState(LAMP_MATCH, 1);
      } else {
        NumMatchSpins += 1;
      }
      if (NumMatchSpins == 44) {
        MatchDelay += 5000;
      }
    }
  }

  if (NumMatchSpins > 43) {
    if (CurrentTime > (MatchSequenceStartTime + MatchDelay)) {
      return MACHINE_STATE_ATTRACT;
    }
  }

  for (int count = 0; count < 4; count++) {
    if ((ScoreMatches >> count) & 0x01) {
      // If this score matches, we're going to flash the last two digits
#ifdef RPU_OS_USE_7_DIGIT_DISPLAYS
      if ( (CurrentTime / 200) % 2 ) {
        RPU_SetDisplayBlank(count, RPU_GetDisplayBlank(count) & 0x1F);
      } else {
        RPU_SetDisplayBlank(count, RPU_GetDisplayBlank(count) | 0x60);
      }
#else
      if ( (CurrentTime / 200) % 2 ) {
        RPU_SetDisplayBlank(count, RPU_GetDisplayBlank(count) & 0x0F);
      } else {
        RPU_SetDisplayBlank(count, RPU_GetDisplayBlank(count) | 0x30);
      }
#endif
    }
  }

  return MACHINE_STATE_MATCH_MODE;
}


int HandleSystemSwitches(int curState, byte switchHit) {
  int returnState = curState;
  switch (switchHit) {
    case SW_SELF_TEST_SWITCH:
      Menus.EnterOperatorMenu();
      break;
    case SW_COIN_1:
    case SW_COIN_2:
    case SW_COIN_3:
      AddCoinToAudit(SwitchToChuteNum(switchHit));
      AddCoin(SwitchToChuteNum(switchHit));
      break;
    case SW_CREDIT_RESET:
      if (MachineState == MACHINE_STATE_MATCH_MODE) {
        // If the first ball is over, pressing start again resets the game
        if (Credits >= 1 || FreePlayMode) {
          if (!FreePlayMode) {
            Credits -= 1;
            RPU_WriteByteToEEProm(RPU_CREDITS_EEPROM_BYTE, Credits);
            RPU_SetDisplayCredits(Credits, !FreePlayMode);
          }
          returnState = MACHINE_STATE_INIT_GAMEPLAY;
        }
      } else {
        CreditResetPressStarted = CurrentTime;
      }
      break;
    case SW_OUTHOLE:
      // Some machines have a kicker to move the ball
      // from the outhole to the re-shooter ramp
      break;
    case SW_TILT:      
//  case SW_ROLL_TILT:      
//  case SW_PLUMB_TILT:
      if (BallFirstSwitchHitTime) {
        if ( CurrentTime > (LastTiltWarningTime + TILT_WARNING_DEBOUNCE_TIME) ) {
          LastTiltWarningTime = CurrentTime;
          NumTiltWarnings += 1;
          if (NumTiltWarnings > MaxTiltWarnings) {
            RPU_DisableSolenoidStack();
            RPU_SetDisableFlippers(true);
            RPU_TurnOffAllLamps();
            RPU_SetLampState(LAMP_TILT, 1);
            Audio.StopAllAudio();
            if (BallSaveEndTime) {
              BallSaveEndTime = 0;
              NumberOfBallSavesRemaining = 0;
            }
            PlaySoundEffect(SOUND_EFFECT_TILT);
          } else {
            PlaySoundEffect(SOUND_EFFECT_TILT_WARNING);
          }
        }
      } else {
        // Tilt before ball is plunged -- show a timer in ManageGameMode if desired
        if ( CurrentTime > (LastTiltWarningTime + TILT_WARNING_DEBOUNCE_TIME) ) {
          PlaySoundEffect(SOUND_EFFECT_TILT_WARNING);
        }
        LastTiltWarningTime = CurrentTime;
      }
      break;
  }

  return returnState;
}



// CUSTOMIZATION TODO:
// This is where switch handling happens
void HandleGamePlaySwitches(byte switchHit) {

  if (DEBUG_MESSAGES) {
    char buf[128];
    sprintf(buf, "Switch Hit = %d\n", switchHit);
    Serial.write(buf);
  }
  unsigned long scoreAddition = 0;
        
  if (AwardScoring) {
    PlaySoundEffect(SOUND_EFFECT_WIZARD_SCORING);
    CurrentScores[CurrentPlayer] += SCORE_WIZARD_SWITCH_HIT;
  }

  boolean shiftLamps = false;
  byte currentSwitches = CheckSequentialSwitches(SW_DROP_TARGET_R, 6);
      
  switch (switchHit) {
    case SW_SLAM:
      //          RPU_DisableSolenoidStack();
      //          RPU_SetDisableFlippers(true);
      //          RPU_TurnOffAllLamps();
      //          RPU_SetLampState(LAMP_GAME_OVER, 1);
      //          delay(1000);
      //          return MACHINE_STATE_ATTRACT;
      break;
    case SW_TILT:
      // Tilt only counts if the playfield has been qualified
      if (BallFirstSwitchHitTime) {          
        if ((CurrentTime - LastTiltWarningTime) > TILT_WARNING_DEBOUNCE_TIME) {
          LastTiltWarningTime = CurrentTime;
          NumTiltWarnings += 1;
          if (NumTiltWarnings > MaxTiltWarnings) {
            RPU_DisableSolenoidStack();
            RPU_SetDisableFlippers(true);
            RPU_TurnOffAllLamps();
            Audio.StopAllAudio();
            PlaySoundEffect(SOUND_EFFECT_TILT);
            RPU_SetLampState(LAMP_TILT, 1);
          }
          PlaySoundEffect(SOUND_EFFECT_TILT_WARNING);
        }
      } else {
        // Tilt before ball is plunged -- show a timer in ManageGameMode if desired
        if ( CurrentTime > (LastTiltWarningTime + TILT_WARNING_DEBOUNCE_TIME) ) {
          PlaySoundEffect(SOUND_EFFECT_TILT_WARNING);
        }
        LastTiltWarningTime = CurrentTime;            
      }
      break;
    case SW_SELF_TEST_SWITCH:
      Menus.EnterOperatorMenu();
      break;
    case SW_STAND_UP:
      if (GameMode==GAME_MODE_SKILL_SHOT) {
      } else if (GameMode==GAME_MODE_METEOR_STORM && RocketsArmed) {  
        for (int count=0; count<3; count++) {
          if (RocketsArmed&(1<<count)) {
            RocketLaunchedTime[count] = CurrentTime;
            PlaySoundEffect(SOUND_EFFECT_ROCKET_LAUNCHED);
            RocketsArmed &= ~(1<<count);
            RocketsFiredFromBank = false;
            // If we're at a level where we only launch one rocket, then break
            if (OrpheusRound[CurrentPlayer]>=LAUNCH_ALL_ROCKETS_WITH_STANDUP_UNTIL_LEVEL) break;
          }
        }
      } else {
        if (!HandleSpotMeteorSwitch(switchHit)) {
          HandleStandupHit();
        }
        StandupHits += 1;
        StandupResetTime = CurrentTime + ((unsigned long)StandupHits)*100 + 100;
        if (!ExtraBallCollected) StandupAward = 2;
        else if (!SpecialCollected) StandupAward = 3;
      }
      if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
      break;
    case SW_LEFT_IN_LANE:
    case SW_RIGHT_IN_LANE:
    case SW_UPPER_ROLLOVER:
    case SW_MIDDLE_ROLLOVER:
    case SW_LOWER_ROLLOVER:
      if (GameMode==GAME_MODE_SKILL_SHOT) {
        PlaySoundEffect(SOUND_EFFECT_SKILL_SHOT);
        CurrentScores[CurrentPlayer] += SCORE_SKILL_SHOT;
        HandleSpotMeteorSwitch(switchHit, true);
        SkillShotHit = true;
      } else {
        if (!HandleSpotMeteorSwitch(switchHit)) {
          if (switchHit==SW_LEFT_IN_LANE) {
            PlaySoundEffect(SOUND_EFFECT_LEFT_INLANE);
          } else if (switchHit==SW_RIGHT_IN_LANE) {
            PlaySoundEffect(SOUND_EFFECT_RIGHT_INLANE);
          } else {
            PlaySoundEffect(SOUND_EFFECT_UNLIT_ROLLOVER);
          }
          
          CurrentScores[CurrentPlayer] += SCORE_ROLLOVER_AND_INLANE;
        }
      }
      
      if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
      break;
    case SW_RIGHT_OUT_LANE:
      PlaySoundEffect(SOUND_EFFECT_RIGHT_OUTLANE);
      if (CollectRocketsByte&ROCKET_1_COLLECT_LIGHT) {
        CollectRocketAnimationStart = CurrentTime;
        CollectRocketCurrentLevel = 0;
        CollectRocketsByte &= ~ROCKET_1_COLLECT_LIGHT;
      } else {
        CurrentScores[CurrentPlayer] += SCORE_OUTLANE;
      }
      SetBallSave(4000, 0, true);
      if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
      break;
    case SW_LEFT_OUT_LANE:
      PlaySoundEffect(SOUND_EFFECT_LEFT_OUTLANE);
      if (CollectRocketsByte&ROCKET_3_COLLECT_LIGHT) {
        CollectRocketAnimationStart = CurrentTime;
        CollectRocketCurrentLevel = 0;
        CollectRocketsByte &= ~ROCKET_3_COLLECT_LIGHT;
      } else {
        CurrentScores[CurrentPlayer] += SCORE_OUTLANE;
      }
      SetBallSave(4000, 0, true);
      if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
      break;
    case SW_SPINNER:
      for (int count=0; count<6; count++) {
        if (currentSwitches&(1<<count)) {
          scoreAddition += SCORE_SPINNER_VAL_ADD_200;
          if (count==0 || count==5) scoreAddition += SCORE_SPINNER_VAL_ADD_600;
        }
      }
      if (SuperSpinnerEndTime) {
        CurrentScores[CurrentPlayer] += (SCORE_SPINNER_SUPER_SPIN + SCORE_SPINNER_SUPER_SPIN_MULTIPLIER*scoreAddition);
        GoalsCompletedFlags[CurrentPlayer] |= GOAL_SUPER_LAMP_SPINNER_ACHIEVED;
      }
      else CurrentScores[CurrentPlayer] += (SCORE_SPINNER_SPIN + scoreAddition);
      PlaySoundEffect(SOUND_EFFECT_SPINNER);
      SpinnerHitPhase = (SpinnerHitPhase+1)%12;
      LastTimeSpinnerHit = CurrentTime;
      shiftLamps = true;
      if (GameMode==GAME_MODE_UNSTRUCTURED_PLAY) {
        NumberOfSpins[CurrentPlayer] += 1;
        if (NumberOfSpins[CurrentPlayer]>99) {
          NumberOfSpins[CurrentPlayer] = 0;
          SuperSpinnerEndTime = CurrentTime + SUPER_LAMP_SPINNER_DURATION;
          PlaySoundEffect(SOUND_EFFECT_SUPER_SPINNER);
          RPU_SetDisplayCredits(Credits);
        } else {
          RPU_SetDisplayCredits(99-NumberOfSpins[CurrentPlayer]);
        }
      }
      if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
      break;
    case SW_DROP_TARGET_M:
    case SW_DROP_TARGET_E_1:
    case SW_DROP_TARGET_T:
    case SW_DROP_TARGET_E_2:
    case SW_DROP_TARGET_O:
    case SW_DROP_TARGET_R:
      if ((CurrentTime-GameModeStartTime)>DROP_SWITCH_BLACKOUT_PERIOD || GameMode!=GAME_MODE_SKILL_SHOT) {
        if (GameMode!=GAME_MODE_SKILL_SHOT) {
          HandleMeteorDropTargetHit(switchHit);
        } else {
          if (switchHit==SkillShotWowLit) {
            PlaySoundEffect(SOUND_EFFECT_SUPER_SKILL_SHOT);
            CurrentScores[CurrentPlayer] += SCORE_SUPER_SKILL_SHOT;
            SkillShotHit = true;
          } else {
            PlaySoundEffect(SOUND_EFFECT_SKILL_SHOT);
            CurrentScores[CurrentPlayer] += SCORE_SKILL_SHOT;
            SkillShotHit = true;
          }
        }
        if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
      }
      break;
    case SW_RIGHT_3_TOP:
    case SW_RIGHT_3_LOWER:
    case SW_RIGHT_3_MID:
      if ((CurrentTime-GameModeStartTime)>DROP_SWITCH_BLACKOUT_PERIOD || GameMode!=GAME_MODE_SKILL_SHOT) {
        Handle3BankHit(switchHit, 2);
        if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
      }
    break;
    case SW_TOP_LEFT_3_TOP:
    case SW_TOP_LEFT_3_LOWER:
    case SW_TOP_LEFT_3_MID:
      if ((CurrentTime-GameModeStartTime)>DROP_SWITCH_BLACKOUT_PERIOD || GameMode!=GAME_MODE_SKILL_SHOT) {
        Handle3BankHit(switchHit, 1);
        if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
      }
    break;
    case SW_LOWER_LEFT_3_TOP:
    case SW_LOWER_LEFT_3_LOWER:
    case SW_LOWER_LEFT_3_MID:
      if ((CurrentTime-GameModeStartTime)>DROP_SWITCH_BLACKOUT_PERIOD || GameMode!=GAME_MODE_SKILL_SHOT) {
        Handle3BankHit(switchHit, 0);
        if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
      }
    break;
    case SW_THUMPER_BUMPER:
      CurrentScores[CurrentPlayer] += SCORE_THUMPER_BUMPER;
      PlaySoundEffect(SOUND_EFFECT_THUMPER_BUMPER_HIT);
      shiftLamps = true;
      if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
      break;
    case SW_BUMPER_UPPER_RIGHT:
      if (GameMode==GAME_MODE_METEOR_STORM && (MeteorStormSectors & METEOR_STORM_SECTOR_3) && OrpheusRound[CurrentPlayer]<LAUNCH_ROCKETS_WITH_DEAD_BUMPER_UNTIL_LEVEL) {
        RocketLaunchedTime[2] = CurrentTime;
        PlaySoundEffect(SOUND_EFFECT_ROCKET_LAUNCHED);
        RocketsArmed &= ~(METEOR_STORM_SECTOR_3);
      } else {
        if (CollectRocketsByte&ROCKET_3_COLLECT_LIGHT) {
          CollectRocketsByte &= ~ROCKET_3_COLLECT_LIGHT;
          AddToBonus(2);
/*              Bonus[2] += 1;
          if (Bonus[2]>MAX_DISPLAY_BONUS) Bonus[2] = MAX_DISPLAY_BONUS;
          BonusAnimationStart[2] = CurrentTime;
          BonusAnimationShown[2] = 0;
*/
        } else {
          PlaySoundEffect(SOUND_EFFECT_ROCKET_3_UNLIT);
        }
        Rocket3BumperAnimationTime = CurrentTime;
        if (Bonus[2]) CurrentScores[CurrentPlayer] += (unsigned long) Bonus[2]*SCORE_BUMPER_BONUS;
        else CurrentScores[CurrentPlayer] += SCORE_BUMPER;
      }
      if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
      break;
    case SW_BUMPER_UPPER_LEFT:
      if (GameMode==GAME_MODE_METEOR_STORM && (MeteorStormSectors & METEOR_STORM_SECTOR_1) && OrpheusRound[CurrentPlayer]<LAUNCH_ROCKETS_WITH_DEAD_BUMPER_UNTIL_LEVEL) {
        RocketLaunchedTime[0] = CurrentTime;
        PlaySoundEffect(SOUND_EFFECT_ROCKET_LAUNCHED);
        RocketsArmed &= ~(METEOR_STORM_SECTOR_1);
      } else {
        if (CollectRocketsByte&ROCKET_1_COLLECT_LIGHT) {
          CollectRocketsByte &= ~ROCKET_1_COLLECT_LIGHT;
          AddToBonus(0);
/*              Bonus[0] += 1;
          if (Bonus[0]>MAX_DISPLAY_BONUS) Bonus[0] = MAX_DISPLAY_BONUS;
          BonusAnimationStart[0] = CurrentTime;
          BonusAnimationShown[0] = 0;
*/              
        } else {
          PlaySoundEffect(SOUND_EFFECT_ROCKET_1_UNLIT);
        }
        Rocket1BumperAnimationTime = CurrentTime;
        if (Bonus[0]) CurrentScores[CurrentPlayer] += (unsigned long) Bonus[0]*SCORE_BUMPER_BONUS;
        else CurrentScores[CurrentPlayer] += SCORE_BUMPER;
      }
      if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
      break;
    case SW_LEFT_SLINGSHOT:
    case SW_RIGHT_SLINGSHOT:
      CurrentScores[CurrentPlayer] += SCORE_SLINGSHOT;
      PlaySoundEffect(SOUND_EFFECT_SLING1+(CurrentTime%3));
      shiftLamps = true;
      if (currentSwitches==0x3F && GameMode<GAME_MODE_ORPHEUS_START) SetMeteorDropTargets(0, 450);
      if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
      break;
  }

  if (shiftLamps) {
    SpotMeteorTargetByte = (SpotMeteorTargetByte>>1)|(SpotMeteorTargetByte<<7);
    CollectRocketsByte = (CollectRocketsByte>>1)|(CollectRocketsByte<<7);
    TargetAwardPhase += 1;
    if (TargetAwardPhase>2) TargetAwardPhase = 0;
  }


}


int RunGamePlayMode(int curState, boolean curStateChanged) {
  int returnState = curState;
  unsigned long scoreAtTop = CurrentScores[CurrentPlayer];

  // Very first time into gameplay loop
  if (curState == MACHINE_STATE_INIT_GAMEPLAY) {
    returnState = InitGamePlay();
  } else if (curState == MACHINE_STATE_INIT_NEW_BALL) {
    returnState = InitNewBall(curStateChanged, CurrentPlayer, CurrentBallInPlay);
  } else if (curState == MACHINE_STATE_NORMAL_GAMEPLAY) {
    returnState = ManageGameMode();
  } else if (curState == MACHINE_STATE_COUNTDOWN_BONUS) {
    Display_ClearOverride(0xFF);
    Display_UpdateDisplays(0xFF, true);
    returnState = CountdownBonus(curStateChanged);
  } else if (curState == MACHINE_STATE_BALL_OVER) {
    RPU_SetDisplayCredits(Credits);
    MeteorStatus[CurrentPlayer] = CheckSequentialSwitches(SW_DROP_TARGET_R, 6);
    if (GameMode>=GAME_MODE_ORPHEUS_START) MeteorStatus[CurrentPlayer] = 0;
    
    if (SamePlayerShootsAgain) {
      PlaySoundEffect(SOUND_EFFECT_SHOOT_AGAIN);
      returnState = MACHINE_STATE_INIT_NEW_BALL;
    } else {
      
      CurrentPlayer += 1;
      if (CurrentPlayer >= CurrentNumPlayers) {
        CurrentPlayer = 0;
        CurrentBallInPlay += 1;
      }
      scoreAtTop = CurrentScores[CurrentPlayer];
      
      if (CurrentBallInPlay > BallsPerGame) {
        CheckHighScores();
        PlaySoundEffect(SOUND_EFFECT_GAME_OVER);
        for (int count = 0; count < CurrentNumPlayers; count++) {
          RPU_SetDisplay(count, CurrentScores[count], true, 2);
        }

        returnState = MACHINE_STATE_MATCH_MODE;
      }
      else returnState = MACHINE_STATE_INIT_NEW_BALL;
    }
  } else if (curState == MACHINE_STATE_MATCH_MODE) {
    returnState = ShowMatchSequence(curStateChanged);
  }

  byte switchHit;
  unsigned long lastBallFirstSwitchHitTime = BallFirstSwitchHitTime;

  while ( (switchHit = RPU_PullFirstFromSwitchStack()) != SWITCH_STACK_EMPTY ) {
    returnState = HandleSystemSwitches(returnState, switchHit);
    if (NumTiltWarnings <= MaxTiltWarnings) HandleGamePlaySwitches(switchHit);
  }

  if (CreditResetPressStarted) {
    if (CurrentBallInPlay < 2) {
      // If we haven't finished the first ball, we can add players
      AddPlayer();
      if (DEBUG_MESSAGES) {
        Serial.write("Start game button pressed\n\r");
      }
      CreditResetPressStarted = 0;
    } else {
      if (RPU_ReadSingleSwitchState(SW_CREDIT_RESET)) {
        if (TimeRequiredToResetGame != 99 && (CurrentTime - CreditResetPressStarted) >= ((unsigned long)TimeRequiredToResetGame * 1000)) {
          // If the first ball is over, pressing start again resets the game
          if (Credits >= 1 || FreePlayMode) {
            if (!FreePlayMode) {
              Credits -= 1;
              RPU_WriteByteToEEProm(RPU_CREDITS_EEPROM_BYTE, Credits);
              RPU_SetDisplayCredits(Credits, !FreePlayMode);
            }
            returnState = MACHINE_STATE_INIT_GAMEPLAY;
            CreditResetPressStarted = 0;
          }
        }
      } else {
        CreditResetPressStarted = 0;
      }
    }

  }

  if (lastBallFirstSwitchHitTime == 0 && BallFirstSwitchHitTime != 0) {
    BallSaveEndTime = BallFirstSwitchHitTime + ((unsigned long)BallSaveNumSeconds) * 1000;
    NumberOfBallSavesRemaining = 1;
  }
  if (CurrentTime > (BallSaveEndTime + BALL_SAVE_GRACE_PERIOD)) {
    BallSaveEndTime = 0;
    NumberOfBallSavesRemaining = 0;
  }

  if (!ScrollingScores && CurrentScores[CurrentPlayer] > RPU_OS_MAX_DISPLAY_SCORE) {
    CurrentScores[CurrentPlayer] -= (RPU_OS_MAX_DISPLAY_SCORE+1);
    if (!TournamentScoring) {
      AddSpecialCredit();    
    } else {
      RPU_PushToTimedSolenoidStack(SOL_KNOCKER, 3, CurrentTime, true);
    }
  }

  if (scoreAtTop != CurrentScores[CurrentPlayer]) {
    Display_SetLastTimeScoreChanged(CurrentTime);
    if (!TournamentScoring) {
      for (int awardCount = 0; awardCount < 3; awardCount++) {
        if (AwardScores[awardCount] != 0 && scoreAtTop < AwardScores[awardCount] && CurrentScores[CurrentPlayer] >= AwardScores[awardCount]) {
          // Player has just passed an award score, so we need to award it
          if (((ScoreAwardReplay >> awardCount) & 0x01)) {
            AddSpecialCredit();
          } else if (!ExtraBallCollected) {
            AwardExtraBall();
          }
        }
      }
    }
  
  }

  return returnState;
}


#ifdef DEBUG_SHOW_LOOPS_PER_SECOND
unsigned long NumLoops = 0;
unsigned long LastLoopReportTime = 0;
#endif


void loop() {

  CurrentTime = millis();
  int newMachineState = MachineState;
  
#ifdef DEBUG_SHOW_LOOPS_PER_SECOND
  NumLoops += 1;
  if (LastLoopReportTime==0) LastLoopReportTime = CurrentTime;
  if (CurrentTime>(LastLoopReportTime+1000)) {
    LastLoopReportTime = CurrentTime;
    char buf[128];
    sprintf(buf, "Loop running at %lu Hz\n", NumLoops);
    Serial.write(buf);
    NumLoops = 0;
  }
#endif

  if (Menus.OperatorMenusActive()) {
    RunOperatorMenu();
  } else {
    if (RestoreBackgroundTrack!=BACKGROUND_TRACK_NONE) {
      PlayBackgroundSong(RestoreBackgroundTrack);
      RestoreBackgroundTrack = BACKGROUND_TRACK_NONE;
    }
    if (MachineState < 0) {
      newMachineState = 0;
    } else if (MachineState == MACHINE_STATE_ATTRACT) {
      newMachineState = RunAttractMode(MachineState, MachineStateChanged);
    } else if (MachineState == MACHINE_STATE_DIAGNOSTICS) {
      newMachineState = RunDiagnosticsMode(MachineState, MachineStateChanged);
    } else {
      newMachineState = RunGamePlayMode(MachineState, MachineStateChanged);
    }  
  }
  
  if (newMachineState != MachineState) {
    MachineState = newMachineState;
    MachineStateChanged = true;
  } else {
    MachineStateChanged = false;
  }

  RPU_Update(CurrentTime);
  Audio.Update(CurrentTime);


}
