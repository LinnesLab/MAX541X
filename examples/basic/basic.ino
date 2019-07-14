/*
 * FILENAME:    basic.ino
 * AUTHOR:      Orlando S. Hoilett
 * EMAIL:       ohoilett@purdue.edu
 * VERSION:     1.0.0
 * 
 * 
 * DESCRIPTION
 * Basic example demonstrating how to use the MAX541X library.
 * 
 * 
 * INSTRUCTIONS
 * 1. Ground A0 pin
 * 2. Connect H to VCC
 * 3. Connect L to GND
 * 4. Connect W to an oscilloscope
 * 
 * 
 * UPDATES
 * 2019/07/14:1620>
 *            Put together basic example to display write(), setR(),
 *            and getR() methods.
 * 
 * 
 * DISCLAIMER
 * The MIT License (MIT)
 * Copyright (c) 2019 Linnes Lab, Purdue University, West Lafayette, IN, USA
 * 
 */

#include <Wire.h>
#include "MAX541X.h"


//0x28 is the i2c address
//MAX541X::MAX5419 has a 200 kOhms full-scale resistance
//MAX541X_WL measuring across W and L pins
MAX541X digiPot = MAX541X(0x28, MAX541X::MAX5419, MAX541X_WL);


void setup()
{
  Serial.begin(9600);
  Wire.begin();
}

void loop()
{
  //Increasing output voltage from 0 to VCC
  for(uint32_t i = 0; i < MAX541X::MAX5419; i=i+1600)
  {
    digiPot.setR(i);

    //Set R will be different than programmed R due to
    //the resolution of the MAX541X.That's expected.
    Serial.print("Set R: ");
    Serial.print(i);
    Serial.print(", Programmed R: ");
    Serial.println(digiPot.getR());

    delay(1);
  }


  //generates a triangle wave
  for(int i = 0; i < 256; i++)
  {
    digiPot.write(MAX541X_VREG, i);
    delay(2);
  }
  for(int i = 254; i > 0; i--)
  {
    digiPot.write(MAX541X_VREG, i);
    delay(2);
  }
  
}
