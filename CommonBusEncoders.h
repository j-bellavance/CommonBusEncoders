/*
  CommonBusEncoders.h
  Created by : Jacques Bellavance, March 26, 2017
  Released into the public domain.

  This library is designed to use multiple encoders with the least amount of digital pins.
  The first encoder requires 4 pins. the remaining ones require only one extra pin per encoder.
  For use with quadratic encoders with 2 or 4 steps per detent, 
  and need to be actuated by hand. This Library does not use interrupts.
  See the Tutoriel for more information
*/


#ifndef CommonBusEncoder_h
#define CommonBusEncoder_h

#include "Arduino.h"

class CommonBusEncoders
{
  public:
    //Methods
    CommonBusEncoders(int pinA, int pinB, int pinS, int count);                              //Constructor
    void addEncoder(int encoderId, int type, int pin, int modes, int indexE, int indexS);    //Add an encoder
    void resetChronoAfter(int aDelay);                                                       //Adjust encoder's priority timeout
    int readAll();                                                                           //Read all encoders
    void setDebounce(int w);                                                                 //Debounce reads (1..32)
    bool focussed();
    
  private:
    //Methods
    int debounce(int i);                          //Generic switch debounce routine
    int readEncoder4(int i);                      //Read an encoder with 4 steps per detent
    int readEncoder2(int i) ;                     //Read an encoder with 2 steps per detent
    bool switchPressed(int i);                    //Confirm that an encoder's switch has been pressed
		int readEncoder(int i);                       //Read a specific encoder (whatever the type)
    int getIndex(int i);                          //Get the index of the action taken by an encoder (CW, CCW or switch pressed)
		//Attributes
    int MYpinA;                                   //Arduino's pin where the Bus A is attached
    int MYpinB;                                   //Arduino's pin where the Bus B is attached
    int MYpinS;                                   //Arduino's pin where the Bus with the switches is attached
    int MYcount;                                  //The number of encoders attached to the busses
    unsigned long MYchrono;                       //Used to timeout an encoder's priority
    unsigned long MYactiveTimeLimit = 500;        //The encoder's priority timeout value (in milliseconds)
		int debounceWidth = 16;                       //The number of conseccutive times a switch is read by the debouncer
		unsigned long debounceDontCare = 0xffff0000;  //Mask to take into account the number of conseccutive times a switch is read by the debouncer
    bool isFocussed;                              //A flag to specify thet a specific encoder is presently read
    struct encoder {                              //For each encoder :
      int type;                                     //2 = 2 steps per detent, 4 = 4 steps per detent
      int pin;                                      //Arduino's pin on which the encoder's common pin is attached
      int modes;                                    //The number of modes this encoder can take
      int indexE;                                   //The index to return when an action is taken by the encoder
      int indexS;                                   //The index to return if the encoder's switch has been pressed
      int lastA;                                    //The last value (HIGH or LOW) that was read at pin A
      int currentMode;                              //The current mode of the encoder
    };
  encoder *encoders;                         //A pointer to the table containing the encoder's attributes
};

#endif;
