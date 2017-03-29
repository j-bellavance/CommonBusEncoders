/*
 *  Waypoint.ino
 *  Author : Jacques Bellavance
 *  Date : March 26, 2017
 *  This code is on public domain
 *  
 *  This sketch shows how to use the CommonBusEncoder Library to edit a short text
 *  This simulates selecting a waypoint on a Garmin 530 GPS
 *  Waypoints are initialised to K _ _ _ _ (The first letter of most american aitports)
 *  A dual concentric encoder is used to edit a 5 character alphanumerical text
 *  The inner encoder changes the letter of the current character, going up and down the alphabet
 *  The outer encoder changes the position of the cursor. 
 *    Moving right initialises the current character to 'A'
 *    Moving left resets the current character to '_' (No value)
 *  The switch simulates saving the waypoint  
 */

//INCLUDE THE LIBRARIES==============
#include <LiquidCrystal.h>
#include <CommonBusEncoders.h>

//CREATE THE OBJECTS (LCD & ENCODER BANK)==========
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
CommonBusEncoders encoders(34, 35, 36, 2);

//INITIALISE THE VARIABLES===================================================
char alphabet[37];                // _0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ
char wpt[5] = {21, 0, 0, 0, 0};   // K _ _ _ _
int ndx = 0;                      // Current character
String waypoint = "";             // Current waypoint simulated save

//editWaypoint=======================================================================
//Inner encoder  CW : Next letter
//Inner encoder CCW : Previous letter
//Outer encoder  CW : Next Character
//Outer encoder CCW : Previous character
//Switch Simulate a save
//-----------------------------------------------------------------------------------
void editWaypoint(int code) {
  switch (code) {
    case 100: {                                   //Next letter
      if (wpt[ndx] == 36) wpt[ndx] = 1;             //If 'Z' wrap to '0'
      else                wpt[ndx] += 1;            //Else   next letter
      break; 
      }
    case 101: {                                   //Previous letter
      if (wpt[ndx] == 1) wpt[ndx] = 36;             //If '0' wrap to 'Z'
      else               wpt[ndx] -= 1;             //Else   previous letter
      break;
      }
    case 200: {                                   //Next character
      if (ndx < 4) {                                //If not last character
        ndx++;                                      //Next character
        wpt[ndx] = 11;                              //Set to 'A'
      }
      break; 
      }
    case 201: {                                   //Previous character
      if (ndx > 0) {                                //If not first character
        wpt[ndx] = 0;                               //Set current character to '_'
        ndx--;                                      //Previous character
      }
      break; }
    case 300: {                                   //Simulate saving the waypoint
      waypoint = "";
      for (int i = 0 ; i < 5 ; i++) 
        waypoint += alphabet[wpt[i]];
      lcd.setCursor(0, 1);
      lcd.print("Saving: ");
      lcd.print(waypoint);
      delay(1000);
      waypoint = "";
      lcd.setCursor(0, 1);
      lcd.print("Saved        ");
      delay(1000);
      lcd.setCursor(0, 1);
      lcd.print("             ");
      wpt[0] = 21;  //'K'
      for (int i = 1 ; i < 5 ; i++)
        wpt[i] = 0;
      ndx = 0;  
    }
  }
}//editWaypoint----------------------------------------------------------------------

//setup======================================================================================
//Initialise encoders
//Display prompt
//Initialise alphabet[]
//-------------------------------------------------------------------------------------------
void setup() {
  encoders.addEncoder(1, 2, 22, 1, 100, 300);                //Inner encoder
  encoders.addEncoder(2, 2, 23, 1, 200, 300);                //Outer encoder
  
  lcd.begin(16, 2);                                          //Display prompt
  lcd.print("Waypoint: ");

  alphabet[0] = '_';                                         //Initalise alphabet[]
  for (int i = 1 ; i <= 10 ; i++) alphabet[i] = 47 + i;      // 0123456789
  for (int i = 11 ; i < 37 ; i++) alphabet[i] = 54 + i;      // ABCDEFGHIJKLMNOPQRSTUVW

  lcd.setCursor(10, 0);                   //Place the cursor
  for (int i = 0 ; i < 5 ; i++ )          //For all 5 characters
    lcd.print(alphabet[wpt[i]]);            //Display

}//setup------------------------------------------------------------------------------------

//loop========================================================================
//Read the encoders and display the changes on the LCD
//----------------------------------------------------------------------------
void loop() {
  int code = encoders.readAll();          //Read the dual concentric encoder
  if (code > 0) {                         //If moved
    editWaypoint(code);                     //Edit the waypoint - wpt[]
    lcd.setCursor(10, 0);                   //Place the cursor
    for (int i = 0 ; i < 5 ; i++ )          //For all 5 characters
      lcd.print(alphabet[wpt[i]]);            //Display
  }
}//loop-----------------------------------------------------------------------

