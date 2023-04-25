# MeteorStrike2023
New rules for a classic
  
This game runs by interfacing an Arduino into an original pinball MPU
Instructions for the hardware interface board can be found here: https://pinballindex.com/index.php/Main_Page 
  
The audio samples for the Wav Trigger board are here: https://drive.google.com/file/d/1gCiV7Mjw2ddW7durrnnC_SxSMiMqCriS/view?usp=sharing  
  
The uploaded version is configured for the Rev 2 board. Use RPU_config.h to configure for different revisions of the hardware. If using Rev 3 or greater, enable coins per credit configuration by removing the following define from RPU_config.h:
```
#define RPU_OS_DISABLE_CPC_FOR_SPACE
```
