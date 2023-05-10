# MeteorStrike2023
New rules for a classic
  
This game runs by interfacing an Arduino into an original pinball MPU
Instructions for the hardware interface board can be found here: https://pinballindex.com/index.php/Main_Page 
  
The audio samples for the Wav Trigger board are here: https://drive.google.com/file/d/1gCiV7Mjw2ddW7durrnnC_SxSMiMqCriS/view?usp=sharing  
  
The uploaded version is configured for the Rev 2 board. Use RPU_config.h to configure for different revisions of the hardware. If using Rev 3 or greater, enable coins per credit configuration by removing the following define from RPU_config.h:
```
#define RPU_OS_DISABLE_CPC_FOR_SPACE
```
  
# Coin Door Tests, Audits, and Settings  
```
Tests (test number shown in Credits, Ball in Play is blank)
1 - Lamps
2 - Displays
3 - Solenoids
4 - Switches
5 - Sounds (not applicable)

Settings & Audits (page number shown in Ball in Play, Credits is blank)
1 - Award Score 1
2 - Award Score 2
3 - Award Score 3
4 - High Score
5 - Credits
6 - Total Plays
7 - Total Replays
8 - High Score Beat
9 - Chute 2 Coins
10 - Chute 1 Coins
11 - Chute 3 Coins
12 - Reboot (All displays show 8007 (as in "BOOT"), and Credit/Reset button restarts)
13 - Free Play
14 - Ball Save
15 - Music Level
16 - Tournament Scoring
17 - Tilt Warnings
18 - Award Scores (0 = all extra balls, 7 = all specals)
19 - Number of Balls Per Game
20 - Scrolling Scores
21 - Extra Ball Award (for tournament scoring)
22 - Special Award (for tournament scoring)
23 - Dim Level
24 - Attract Mode Sound
25 - Match Feature
```
