/*
CommonBusEncoders.cpp
Created by : Jacques Bellavance, March 26, 2017
Released into the public domain.

This library is designed to use multiple encoders with the least amount of digital pins.
The first encoder requires 4 pins. the remaining ones require only one extra pin per encoder.
For use with quadratic encoders with 2 or 4 steps per detent,
and need to be actuated by hand. This Library does not use interrupts.
See the Tutoriel for more information
*/

#include <Arduino.h>
#include "CommonBusEncoders.h"

//Constructor==============================================================================================
//Connection :
//    4 PinA--------+-----------+----------+-----Bus A        
//    5 PinB----+---|-------+---|------+---|-----Bus B
//    7 --------|---|-------|---|------|-+ |
//    8 --------|---|-------|-+ |      | | |
//    9 --------|-+ |       | | |      | | |
//              ¤ | ¤       ¤ | ¤      ¤ | ¤                  ¤ : 1N4148 diodes, Cathode towards the encoder
//              O O O       O O O      O O O
//                |           |          |
//              +-+         +-+        +-+
//              |           |          |
//              O   O       O   O      O   O
//                  ¤           ¤          ¤
//                  |           |          | 
//    6 PinS--------+-----------+----------+-----Bus S
//
//All the encoder's common pins (7, 8 and 9) are in OUTPUT mode and are set to HIGH
//When it comes the time to read a particular encoder, it's common pin is brought to ground
//The busses are placed in INPUT_PULLUP mode, so there is no need for resistors
//Each encoder has several attributes, described below in the "addEncoder" method
//These attributes are placed in a table called "encoders[]"
//The constructor allocates the exact amount of memory needed for "count" encoders attached to the busses
//The default attribute for the last status (HIGH or LOW) read on the Bus A for each encoder is HIGH
//-------------------------------------------------------------------------------------------------------------
CommonBusEncoders::CommonBusEncoders(int pinA, int pinB, int pinS, int count) {
  MYpinA = pinA;                //Pin of the bus A of the encoders
  MYpinB = pinB;                //Pin of the bus B of the encoders
	MYpinS = pinS;                //Pin of the bus for the switches of the encoders
	MYcount = count;              //The number of encoders attached to those busses
  pinMode(pinA, INPUT_PULLUP);
  pinMode(pinB, INPUT_PULLUP);
  pinMode(pinS, INPUT_PULLUP);

  encoders = (encoder*) calloc(MYcount + 1, sizeof(encoder)); //Allocate memory for the encoders table
  for (int i = 0 ; i <= count ; i++) {                        //For all encoders write defaults
    encoders[i].lastA = 1;                                       //A is HIGH when the encoder is on detent
  }
}//Constructor-------------------------------------------------------------------------------------------------

//addEncoder==================================================================================================
//Used to add a new encoder on the bus
//encoderId : It is the number of the encoder, going from 1 to count (given to the Constructor)
//type      : Can be 2 (two steps per detent) or 4 (four steps per detent)
//pin       : The pin on the Arduino where the common pin of this encoder is connected
//modes     : The number of modes that this encoder can take
//indexE    : The index returned when the encoder is in it's first mode and is turned clockwise
//indexS    : the index of the switch (0 if the encoder is multi mode, any index otherwise)
//------------------------------------------------------------------------------------------------------------
void CommonBusEncoders::addEncoder(int encoderId, int type, int pin, int modes, int indexE, int indexS) {   
  encoders[encoderId].type = type;
  encoders[encoderId].pin = pin;
  encoders[encoderId].modes = modes;
  encoders[encoderId].indexE = indexE;
  encoders[encoderId].indexS = indexS;
	pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
}//addEncoder---------------------------------------------------------------------------------------------------

//debounce======================================================================================================
//debounces the switch connected to "pin"
//The switch is read 16 times (~60us) to look for 16 consecutive HIGH or LOW
//If unsuccessfull, it means that a change is occuring at that same moment
//and that either a rising or falling edge of the signal is actualy occuring.
//The pin is continuously read until the edge is confirmed.
//returns 0 if closed or 1 if open in INPUT_PULLUP mode
//returns 1 if closed or 0 if open in INPUT mode
//See : http://www.ganssle.com/debouncing.htm to learn more about debouncing
//This algorithm is inspired by that article
//---------------------------------------------------------------------------------------------------------------
int CommonBusEncoders::debounce(int pin) {
	unsigned long pinStatus = 0x00000000;
	do {
		for (int i = 1; i <= debounceWidth; i++) pinStatus = (pinStatus << 1) | digitalRead(pin) | debounceDontCare;
	} while ((pinStatus != debounceDontCare) && (pinStatus != 0xffffffff));
	return int(pinStatus & 0x00000001);
}//debounce-------------------------------------------------------------------------------------------------------

//readEncoder4============================================================================================
// Single encoders (4 steps per detent) (When the encoder goes click and stays there)
//  Detent is when both A and B are high in INPUT_PULLUP mode 
//  Step :   1 2 3 4 5 6 7 8
//      A:   1 0 0 1 1 0 0 1
//      B:   0 0 1 1 0 0 1 1
// CW  (Read left to right) When A Rises (4 and 8), B is high 
// CCW (Read right to left) When A Rises (1 and 5), B is low 
//--------------------------------------------------------------------------------------------------------
int CommonBusEncoders::readEncoder4(int i) {
  int rotation = 0;                                   //Direction of rotation buffer (defaults to not turned)
  int A = debounce(MYpinA);                           //Read the status of the bus on pinA
	if ((!encoders[i].lastA) && (A)) {                  //If pin A is RISING (was low and is high),
    int B = digitalRead(MYpinB);                        //Read the status of the bus on pinB

    if (B) rotation = -1;                               //LOW  -> CCW  -> return -1
    else   rotation =  1;                               //HIGH -> CW   -> return  1
  }
  encoders[i].lastA = A;                              //Update pinA's Status
  return rotation;                                    //Return rotation
}//readEncoder4--------------------------------------------------------------------------------------------

//readEncoder2============================================================================================
//Dual concentric encoders from Propwash (2 steps per detent)
//We have to check when pinA goes from HIGH to LOW and when it goes from LOW to HIGH
//--------------------------------------------------------------------------------------------------------
int CommonBusEncoders::readEncoder2(int i) {
  int rotation = 0 ;                               //Direction of rotation buffer
  int A = debounce(MYpinA);                        //Read the status of the bus on pinA
  int B = digitalRead(MYpinB);                     //Read the status of the bus on pinB
  if ((encoders[i].lastA) && (!A)) {               //If pin A is FALLING (was high and is low),
    if (B) rotation =  1;                            //HIGH -> CW   -> return  1
    else   rotation = -1;                            //LOW  -> CCW  -> return -1
  }
  if ((!encoders[i].lastA) && (A)) {               //If pin A is RISING (was low and is high),
    if (B) rotation = -1;                            //LOW  -> CCW  -> return -1
    else   rotation =  1;                            //HIGH -> CW   -> return  1
  }
  encoders[i].lastA = A;                           //Update pinA's Status
  return rotation;                                 //Return rotation
}//readEncoder2-------------------------------------------------------------------------------------------

//switchPressed========================================================================================
//The encoder's switch can be used change it's mode.
//Modes are selected by pressing the switch
//Every press takes the encoder to the next mode
//When the last mode is active, another press brings the encoder to the first mode
//See : https://www.arduino.cc/en/Reference/Modulo for the maths on how it's done
//If number of modes is set to one, the switch can be used as a normal switch for other purposes
//-----------------------------------------------------------------------------------------------------
bool CommonBusEncoders::switchPressed(int i) {
	if (!debounce(MYpinS)) {                               //If the switch is pressed,
		encoders[i].currentMode =                              //Switch to next mode
    (encoders[i].currentMode + 1) % encoders[i].modes;     //0, 1, 2, 3, 0, 1, 2, 3...
		while (!debounce(MYpinS)) {}                           //Wait for the release of the switch
    return true;                                           //return the fact that it has been pressed
	}
  return false;                                          //return the fact that it has NOT been pressed
}//switchPressed----------------------------------------------------------------------------------------

//readEncoder========================================================================================
//Returns : 0 = No action, 1 = Turned CW, -1 = Turned CCW, 9 = the switch was pressed
//The selected encoder's common pin is brought to ground to enable a read
//The encoder is read depending on the number of steps per detent
//if it was not rotated, it's switch is checked, and a flag will be returned
//The selected encoder's common pin is raised to disable a read
//---------------------------------------------------------------------------------------------------
int CommonBusEncoders::readEncoder(int i) {
  int rotation = 0;
	digitalWrite(encoders[i].pin, LOW);                    //Enable read
	switch (encoders[i].type) {                              //Read encoder according to it's type:
  	case 2: { rotation = readEncoder2(i); break; }           // 2 steps per detent
  	case 4: { rotation = readEncoder4(i); break; }           // 4 steps per detent
	}
	if (rotation == 0 && switchPressed(i)) rotation = 9;    //No rotation : Flag the switch if pressed
	digitalWrite(encoders[i].pin, HIGH);                   //Disable read
  return rotation;                                       //Return the rotation
}//readEncoder----------------------------------------------------------------------------------------

//getIndex=================================================================================================
//Each encoder is fitted with two indexes:
//The first index is for the encoder.
//The index returned depends on the rotation of the encoder CW vs CCW and the actual mode of the encoder.
//If an encoder has an index of 100 and is allowed to have four modes, the returned index will be :
//Mode 1 -  CW = 100 : Mode 2  CW = 102 : Mode 3  CW = 104 : Mode 4  CW = 106
//Mode 1 - CCW = 101 : Mode 2 CCW : 103 : Mode 3 CCW = 105 : Mode 4 CCW = 107
//Modes are changed by pressing on the encoder's switch in that sequence : 0, 1, 2, 3, 0, 1, 2, 3...
//The switch is also fitted with an index.
//If the encoder has more than one mode, then the index shall be 0
//If the encoder has only one mode, the switch can be used for anything else and be given it's own index
//---------------------------------------------------------------------------------------------------------
int CommonBusEncoders::getIndex(int i) {
	int rotation = readEncoder(i);

  switch (rotation) {
	  case  1: { return encoders[i].indexE + encoders[i].currentMode * 2; break; }
	  case -1: { return encoders[i].indexE + encoders[i].currentMode * 2 + 1; break; }
		case  9: { return encoders[i].indexS; break; }
		default: { return 0; break; }
  }
}//getIndex------------------------------------------------------------------------------------------------

//readAll========================================================================================================
//Reads all the encoders
//Using an encoder raises a flag to give 
//The priority is canceled after an inactivity is observed beyond "MYactiveTimeLimit" (1/2 second by default)
//If the current encoder is still active, return it's index or return 0
//If not, read all the encoders 
//----------------------------------------------------------------------------------------------------------------
int CommonBusEncoders::readAll() {
  static int currentEncoder = 0;                                  //To concentrate on the current encoder
  int index = 0;                                                  //Default index is 0 (no action)
  if (millis() - MYchrono > MYactiveTimeLimit) {                  //If it is timeout for the current encoder
    encoders[currentEncoder].currentMode = 0;                       //Reset the mode to 0
    currentEncoder = 0;                                             //Cancel current encoder
    isFocussed = false;                                             //not focussed on any encoder
  }
	if (currentEncoder != 0 ) {                                     //If an encoder is currently in use
    index = getIndex(currentEncoder);                               //Get it's index
		if (index != 0) {	MYchrono = millis(); return index; }          //If active, reset chrono and return
		else            { return 0; }                                   //Else, return no index
	}

  for (int i = 1 ; i <= MYcount ; i++) {                          //For each encoder
  	index = getIndex(i);                                            //Get it's index
		if (index != 0) {                                               //If it is active,
      currentEncoder = i;                                             //Make it the current encoder
      MYchrono = millis();                                            //Reset MYchrono
      isFocussed = true;                                              //Now focussed on an encoder
      return index;                                                   //Return it's index
    }
  }
  return 0;                                                       //Else, return no index
}//readAll----------------------------------------------------------------------------------------------------

//resetChronoAfter=====================================================
//When an encoder is activated, it recieves priority for future reads.
//The actual time this priority is kept is set by this method
//"aDelay" is expressed in milliseconds. The default is 500ms
//---------------------------------------------------------------------
void CommonBusEncoders::resetChronoAfter(int aDelay) {
  MYactiveTimeLimit = aDelay;
}//resetChronoAfter----------------------------------------------------


//setDebounce=====================================================================
//Sets the number of times a switch is read repeatedly by the debouncer routine
//It defaults to 16 times. Allowable values are 1..32
//--------------------------------------------------------------------------------
void CommonBusEncoders::setDebounce(int w) {
  debounceWidth = w;
	unsigned long debounceDontCare = 0xffffffff;
	for (int i = 0; i <= w; i++) debounceDontCare = debounceDontCare << 1 | 0;
}//setDebounce--------------------------------------------------------------------

//focussed===================================
//Returns true if an ender has focus
//-------------------------------------------
bool CommonBusEncoders::focussed() {
	return isFocussed;
}//focussed----------------------------------