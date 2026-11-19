#ifndef LAMP_ANIMATIONS_H

#include "RPU_Config.h"
#include "RPU.h"


// This file can define a series of animations, stored 
// with each lamp as a bit in the following array.
// Lamp 0 = the first bit of the first byte, so "{0x01," below.
// The animations can be played with either of the helper functions
// at the bottom of this file.
// 
// CUSTOMIZATION TODO:
// These demonstration animations should be replaced
// or removed for each specific implementation.
// There's a generator for this at: https://www.pinballrefresh.com/animation-generator


// Lamp animation arrays
#define NUM_LAMP_ANIMATIONS       3
#define LAMP_ANIMATION_STEPS      16
#define NUM_LAMP_ANIMATION_BYTES  8


/*
All lamps

    {321,870},
    {176,784},
    {292,788},
    {292,813},
    {292,839},
    {292,865},
    {292,892},
    {292,918},
    {292,943},
    {191,550},
    {323,788},
    {323,813},
    {323,839},
    {323,865},
    {323,892},
    {323,918},
    {323,943},
    {264,343},
    {351,788},
    {351,813},
    {351,839},
    {351,865},
    {351,892},
    {351,918},
    {351,943},
    {457,689},
    {556,784},
    {508,404},
    {479,426},
    {449,449},
    {416,469},
    {388,489},
    {355,514},
    {225,384},
    {302,302},
    {481,363},
    {451,386},
    {420,408},
    {390,428},
    {359,451},
    {327,473},
    {369,1166},
    {119,380},
    {595,528},
    {583,561},
    {571,591},
    {559,624},
    {544,654},
    {530,687},
    {374,48},
    {512,311},
    {402,48},
    {388,60},
    {455,731},
    {567,829},
    {182,589},
    {164,831},
    {467,987},
    {467,943},
    {465,898},
    {412,67}



*/ 

byte LampAnimations[NUM_LAMP_ANIMATIONS][LAMP_ANIMATION_STEPS][NUM_LAMP_ANIMATION_BYTES] = {
  {{0x00, 0x10, 0x30, 0x00, 0x00, 0x00, 0x00, 0x04},
  {0x00, 0x30, 0x70, 0x00, 0x00, 0x00, 0x00, 0x03},
  {0x00, 0x30, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x01},
  {0x00, 0xF0, 0xE0, 0x00, 0x00, 0x01, 0x00, 0x00},
  {0xE0, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0xE0, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x70, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x30, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x18, 0x10, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00},
  {0x1D, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x0E, 0x19, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00},
  {0x0E, 0x1F, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00},
  {0x00, 0x1E, 0x0E, 0xFC, 0xFC, 0x00, 0x00, 0x00},
  {0x00, 0x1C, 0x0E, 0x05, 0x00, 0x7C, 0x10, 0x00},
  {0x00, 0x18, 0x1C, 0x00, 0x00, 0xF0, 0x10, 0x00},
  {0x00, 0x10, 0x18, 0x02, 0x00, 0x00, 0x20, 0x00}},
  {{0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x10, 0x38, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x38, 0x7C, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x7C, 0x6C, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0xEE, 0xC6, 0xEE, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0xC6, 0x82, 0xC6, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x82, 0x00, 0x82, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x06},
  {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90, 0x07},
  {0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x10, 0x01},
  {0x00, 0x00, 0x00, 0x03, 0x00, 0x80, 0x20, 0x00},
  {0x00, 0x00, 0x00, 0x02, 0x00, 0xC1, 0x60, 0x00},
  {0x00, 0x01, 0x00, 0x80, 0x00, 0x61, 0x40, 0x00},
  {0x00, 0x01, 0x00, 0xE0, 0x80, 0x38, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x70, 0xE0, 0x0C, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x1C, 0x3C, 0x04, 0x00, 0x00}},
  {{0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x03},
  {0x60, 0x60, 0x60, 0x00, 0x00, 0x00, 0x00, 0x04},
  {0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0xA0, 0x00},
  {0x07, 0x06, 0x06, 0x02, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00},
  {0x00, 0x00, 0x00, 0x01, 0x00, 0x80, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x40, 0x00},
  {0x00, 0x01, 0x00, 0x80, 0x00, 0x0C, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x60, 0x80, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x18, 0x70, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x04, 0x0C, 0x00, 0x00, 0x00}}
};





void ShowLampAnimation(byte animationNum, unsigned long divisor, unsigned long baseTime, byte subOffset, boolean dim, boolean reverse = false, byte keepLampOn = 99) {
  if (animationNum>=NUM_LAMP_ANIMATIONS) return;
  
  byte currentStep = (baseTime / divisor) % LAMP_ANIMATION_STEPS;
  if (reverse) currentStep = (LAMP_ANIMATION_STEPS - 1) - currentStep;

  byte curBitmask;
  byte *currentLampOffsetByte = LampAnimations[animationNum][(currentStep + subOffset) % LAMP_ANIMATION_STEPS];
  byte *currentLampByte = LampAnimations[animationNum][currentStep];

  byte lampNum = 0;
  for (int byteNum = 0; byteNum < NUM_LAMP_ANIMATION_BYTES; byteNum++) {
    curBitmask = 0x01;
    
    for (byte bitNum = 0; bitNum < 8; bitNum++) {

      byte lampOn = false;
      lampOn = (*currentLampByte) & curBitmask;

      // if there's a subOffset, turn off lights at that offset
      if (subOffset) {
        byte lampOff = true;
        lampOff = (*currentLampOffsetByte) & curBitmask;
        if (lampOff && lampNum != keepLampOn && !lampOn) RPU_SetLampState(lampNum, 0);
      }

      if (lampOn) RPU_SetLampState(lampNum, 1, dim);

      curBitmask *= 2;
      lampNum += 1;
    }
    currentLampByte += 1;
    currentLampOffsetByte += 1;
  }
}


void ShowLampAnimationSingleStep(byte animationNum, byte currentStep, byte *lampsToAvoid = NULL) {
  if (animationNum>=NUM_LAMP_ANIMATIONS) return;
  
  byte lampNum = 0;
  byte *currentLampByte = LampAnimations[animationNum][currentStep];
  byte *currentAvoidByte = lampsToAvoid;
  byte curBitmask;
  
  for (int byteNum = 0; byteNum < NUM_LAMP_ANIMATION_BYTES; byteNum++) {
    curBitmask = 0x01;
    for (byte bitNum = 0; bitNum < 8; bitNum++) {

      boolean avoidLamp = false;
      if (currentAvoidByte!=NULL) {
        if ((*currentAvoidByte) & curBitmask) avoidLamp = true;
      }

      if (!avoidLamp /*&& (*currentLampByte)&curBitmask*/) RPU_SetLampState(lampNum, (*currentLampByte)&curBitmask);

      lampNum += 1;
      curBitmask *= 2;
    }
    currentLampByte += 1;
    if (currentAvoidByte!=NULL) currentAvoidByte += 1;
  }
}


#if defined(RPU_OS_USE_LAMP_OVERLAYS)

/* 
    ShowAnimationStepOnOverlay
      lampsOnOverlay = true - use overlay to turn on lamps, false - use overlay to turn off lamps
      animationNum = index of array above
      currentStep = index of array above
      lampsToAvoid =  pointer to array that's NUM_LAMP_ANIMATION_BYTES deep - if any bit is turned on, then that lamp won't 
                      be added to the overlay

*/
void ShowAnimationStepOnOverlay(boolean lampsOnOverlay, byte animationNum, byte currentStep, byte *lampsToAvoid = NULL) {
  if (animationNum>=NUM_LAMP_ANIMATIONS) return;
  
  byte *currentLampByte = LampAnimations[animationNum][currentStep];
  byte *currentAvoidByte = lampsToAvoid;

  for (int byteNum = 0; byteNum < NUM_LAMP_ANIMATION_BYTES; byteNum++) {
    byte avoidedLampByte = *currentLampByte;
    if (currentAvoidByte!=NULL) {
      avoidedLampByte &= ~(*currentAvoidByte);
      currentAvoidByte += 1;
    }
    RPU_AddLampByteToOverlay(lampsOnOverlay, byteNum, avoidedLampByte);
    currentLampByte += 1;
  }
}

/* 
    RemoveAnimationStepFromOverlay
      lampsOnOverlay = true - use overlay to remove on lamps, false - use overlay to remove off lamps
      animationNum = index of array above
      currentStep = index of array above
      lampsToAvoid =  pointer to array that's NUM_LAMP_ANIMATION_BYTES deep - if any bit is turned on, then that lamp won't 
                      be added to the overlay

*/
void RemoveAnimationStepFromOverlay(boolean lampsOnOverlay, byte animationNum, byte currentStep, byte *lampsToAvoid = NULL) {
  if (animationNum>=NUM_LAMP_ANIMATIONS) return;
  
  byte *currentLampByte = LampAnimations[animationNum][currentStep];
  byte *currentAvoidByte = lampsToAvoid;

  for (int byteNum = 0; byteNum < NUM_LAMP_ANIMATION_BYTES; byteNum++) {
    byte avoidedLampByte = *currentLampByte;
    if (currentAvoidByte!=NULL) {
      avoidedLampByte &= ~(*currentAvoidByte);
      currentAvoidByte += 1;
    }
    RPU_RemoveLampByteFromOverlay(lampsOnOverlay, byteNum, avoidedLampByte);
    currentLampByte += 1;
  }
}


#endif

#define LAMP_ANIMATIONS_H
#endif
