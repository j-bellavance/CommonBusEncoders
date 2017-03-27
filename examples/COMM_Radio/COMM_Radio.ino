/*
 *  COMM_Radio.ino
 *  Author : Jacques Bellavance
 *  Date : March 27, 2017
 *  This code is on public domain
 *  
 *  This sketch shows how to use the CommonBusEncoder Library to change frequencies
 *  This simulates selecting a frequency on an airplane COMM1 radio
 *  COMM radios have a range between 118.000 to 136.975 MHz in steps of 25 kHz
 *  Only the first two digits of the kHz part of the frequency is displayed
 *  Ex.: 118.00, 118.02, 118.05, 118.07, 118.10, 118.12, ...
 *  The radio has a selector to change a standby frequency. 
 *  A button is used to swap the standby frequency and the active frequency
 *  Both frequencies are displayed on the radio
 *  A dual concentric encoder is used to change the standby frequency
 *  The outer encoder changes the MHz part of the frequency
 *  The inner encoder changes teh kHz part of the frequency
 *  The switch is used to swap the frequencies
 */

//INCLUDE THE LIBRARIES==============
#include <LiquidCrystal.h>
#include <CommonBusEncoders.h>

//CREATE THE OBJECTS (LCD & ENCODER BANK)==========
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
CommonBusEncoders encoders(34, 35, 36, 2);

//INITIALISE THE VARIABLES===================================================

int stby[2] = {118, 0};
int actv[2] = {118, 0};


//displayFrequencies===========================================
//Sends the frequencies to the LCD
//-------------------------------------------------------------
void displayFrequencies() {
  lcd.setCursor(0, 1); lcd.print("                ");
  lcd.setCursor(0, 1); lcd.print(actv[0]); lcd.print('.');
  if (actv[1] < 100) lcd.print('0'); lcd.print(actv[1]/10);
  lcd.setCursor(10, 1); lcd.print(stby[0]); lcd.print('.');
  if (stby[1] < 100) lcd.print('0'); lcd.print(stby[1]/10);
}//displayFrequencies------------------------------------------

//editFrequency=======================================================================
//Outer encoder  CW : Increase MHz
//Outer encoder CCW : Decrease MHz
//Inner encoder  CW : Increase kHz
//Inner encoder CCW : Decrease kHz
//Switch            : Swap active and standby
//-----------------------------------------------------------------------------------
void editFrequency(int code) {
  switch (code) {
    case 100: {                                   //Increase kHz
      if (stby[1] == 975) stby[1]  =  0;            //If kHz == 975 -> 0
      else                stby[1] += 25;            //Else             increase kHz
      break; 
      }
    case 101: {                                   //Decrease kHz
      if (stby[1] == 0) stby[1]  = 975;            //If kHz == 0 -> 975
      else              stby[1] -=  25;            //Else           decrease kHz
      break;
      }
    case 200: {                                   //Increase MHz
      if (stby[0] == 136) stby[0]  = 117;           //If MHz == 136 -> 117
      else                stby[0] +=   1;           //Else             increase MHz
      break; 
      }
    case 201: {                                   //Decrease MHz
      if (stby[0] == 117) stby[0]  = 136;           //If MHz == 117 -> 136
      else                stby[0] -=   1;           //Else             decrease MHz
      break; 
      }
    case 300: {                                   //Swap Standby & Active
      int swap[2];
      swap[0] = actv[0]; swap[1] = actv[1];
      actv[0] = stby[0]; actv[1] = stby[1];
      stby[0] = swap[0]; stby[1] = swap[1];
    }
  }
}//editFrequency----------------------------------------------------------------------

//setup======================================================================================
//Initialise encoders
//Display prompt
//Initialise alphabet[]
//-------------------------------------------------------------------------------------------
void setup() {
  encoders.addEncoder(1, 2, 22, 1, 100, 300);                //Inner encoder
  encoders.addEncoder(2, 2, 23, 1, 200, 300);                //Outer encoder
  
  lcd.begin(16, 2);                                          //Init LCD
  lcd.setCursor(0, 0);
  lcd.print("ACTIVE   STANDBY");
  displayFrequencies();

}//setup------------------------------------------------------------------------------------

//loop==========================================================================
//Read the encoders and display the changes on the LCD
//------------------------------------------------------------------------------
void loop() {
  delay(100);
  int code = encoders.readAll();          //Read the dual concentric encoder
  if (code > 0) {                         //If moved
    editFrequency(code);                    //Edit the frequency
    displayFrequencies();                   //Send the frequencies to the LCD
  }
}//loop-------------------------------------------------------------------------

