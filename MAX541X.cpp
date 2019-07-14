/*
 FILENAME:    MAX541X.cpp
 AUTHOR:      Orlando S. Hoilett and Rohit Srivastava
 EMAIL:       ohoilett@purdue.edu
 VERSION:     1.0.0
 
 
 DESCRIPTION
 Class for communicating with the MAX5417/8/9 digital potentiometers.
 
 
 A FEW INSTRUCTIONS
 All methods are defined and coded according to the instructions given in
 Revision 4 of the MAX541X datsheet from Maxim Integrated. All references to
 the "datasheet" refer to this specific revision. The datasheet is referenced
 in the code so that the user can have further consult if he/she needs more
 information. A copy of the datasheet is included in the software download.
 
 All references to "the device" refer to the MAX541X 256-Tap, Nonvolatile,
 I2C-Interface, Digital Potentiometers.
 
 
 UPDATES
 Version 0.0
 2015/09/18:1200>
 		Initialization of code development.
 2015/10/12:1010>
          Testing methods.
 2015/10/12:1041>
          Noticed that objects cannot be instantiated in the "setup()" method.
          No idea why that is.
 
 * SOURCES
 * Some code snippets were taken from
 * vicatcu. "LMP91000." Authored: Oct 27, 2014. Accessed:
 *      September 18, 2015. GitHub. <https://github.com/WickedDevice/LMP91000>
 * jorgenro1. "lmp91000." Authord: Jan 26, 2015. Acccessed:
 *      September 18, 2015. GitHub. <https://github.com/dgnest/lmp91000>
 
 * A couple of other useful links from TI's forum
 * https://e2e.ti.com/support/interface/etc_interface/f/146/t/258263
 * https://e2e.ti.com/support/amplifiers/precision_amplifiers/f/14/t/189399
 * https://e2e.ti.com/support/interface/etc_interface/f/146/t/195448
 * https://e2e.ti.com/support/amplifiers/precision_amplifiers/f/14/t/317192
 
 
 * DISCLAIMER
 The MIT License (MIT)
 Copyright (c) 2019 Linnes Lab, Purdue University, West Lafayette, IN, USA
 
 */



#include "MAX541X.h"


const uint16_t taps = 256;
const uint16_t Rwiper = 375; //Wiper resistance in Ohms as given by the datasheet


//DEFAULT CONSTRUCTOR
MAX541X::MAX541X()
{
}


//Main Constructor
//addr			device's i2c address
//m				model MAX5417, 5418, or 5419. The model number sets the full
//					scale resistance.
//					MAX5417 = 50 kOhms
//					MAX5418 = 100 kOhms
//					MAX5419 = 200 kOhms
//mode			sets how the resistance is being used. If the resistor is used
//					from Wiper to L, then the calculations for setting the
//					potentiometer value are different if the resistor is being
//					used from Wiper to H. Please see setR() method for more info.
//code			initializes resistor code to 0
//
//Must be called as no other set or get methods are available to set these
//member variables. Really just no reason to in this type of class.
MAX541X::MAX541X(const uint8_t addr, const Model m, const uint8_t mode):
_addr(addr),_Rfull(m),_mode(mode),_Rcode(0)
{
}


//uint8_t MAX541X::write(uint8_t reg, uint8_t data)
//reg			register address
//data			code to be written
//@return		error message from the Wire.endTransmission() method
//					0:success
//					1:data too long to fit in transmit buffer
//					2:received NACK on transmit of address
//					3:received NACK on transmit of data
//					4:other error
//
//Writes data to the MAX541X chips using the Arduino I2C protocol.
//Please refer to pages 8-11 of the datasheet for more information.
uint8_t MAX541X::write(uint8_t reg, uint8_t data)
{
	//Only update the _Rcode member variable when upating the volatile memory
	if(reg == MAX541X_VREG) _Rcode = data;
	
	Wire.beginTransmission(_addr);
	Wire.write(reg);
	Wire.write(data);
	return Wire.endTransmission();
}


//uint8_t MAX541X::setR(uint32_t r)
//r				resistance to set the potentiometer to
//@return		error message from the Wire.endTransmission() method
//					0:success
//					1:data too long to fit in transmit buffer
//					2:received NACK on transmit of address
//					3:received NACK on transmit of data
//					4:other error
//
//Sets the value of the digital potentiometer to the desired resistance, r.
uint8_t MAX541X::setR(uint32_t r)
{
	//These statements deal with edge conditions to make
	//sure we don't try to program a negative resistance
	//or end up with a code = 256 which is bigger than
	//an 8-bit integer can hold.
	if(r > _Rfull) r = _Rfull;
	if(r < 2*Rwiper) r = 2*Rwiper;
	else r = r-Rwiper;
	if(_mode != MAX541X_WL)
	{
		if (r < _Rfull/(double)taps) r = _Rfull/(double)taps;
	}
	
	
	//The calculation that determines the code for the given resistor value
	//changes if the potentiometer is connected such that the ends of the
	//resistor are Wiper and L vs. Wiper and H.
	//
	//The calculation isn't given explicitly in the MAX541X datasheet, but it's
	//shown in datasheets for similar digital potentiometers. In this case, I
	//referenced the datasheet for the AD5241/AD5242. It was a conincidence that
	//I just happened to using both these potentiometers in one of my designs.
	//https://www.analog.com/media/en/technical-documentation/data-sheets/AD5241_5242.pdf
	uint8_t data = 0;
	if(_mode == MAX541X_WL)
	{
		//Wiper and L
		data = round((r-Rwiper)*(double)taps/_Rfull);
	}
	else
	{
		//Wiper and H
		data = round(-1*(((r-Rwiper)*(double)taps/_Rfull)-taps));
	}

	//returns potential error message from the Wire.endTransmission() method
	return write(MAX541X_VREG, data);
}


//uint32_t MAX541X::getR() const
//
//Returns the value of the potentiometer from the volatile register.
//
//The calculation that determines the code for the given resistor value changes
//if the potentiometer is connected such that the ends of the resistor are
//Wiper and L vs. Wiper and H. Please see the comments in the setR() method.
uint32_t MAX541X::getR() const
{
	if(_mode == MAX541X_WL) return ((double)_Rcode/taps)*_Rfull + Rwiper; //Wiper and L
	else return ((taps-_Rcode)/(double)taps)*_Rfull + Rwiper; //Wiper and H
}


//uint8_t MAX541X::writeNonvolatile(const uint8_t data)
//data			Data between 0-255 to write to the potentiometer. Chose not to
//					use the setR() for the non-volatile memory since the
//					non-volatile memory is not the primary function of the
//					potentiometer.
//
//Please refer to page 11 of the datasheet for more information.
uint8_t MAX541X::writeNonvolatile(const uint8_t data) //Write to nonvolatile memory
{
    return write(MAX541X_NVREG, data);
}


//uint8_t MAX541X::switchNVtoV()
//@return		error message from the Wire.endTransmission() method
//					0:success
//					1:data too long to fit in transmit buffer
//					2:received NACK on transmit of address
//					3:received NACK on transmit of data
//					4:other error
//
//Takes the value stored in the non-volatile memory and places it in the
//volatile memory. The current, active value of the resistor is reflected by
//the value stored in the volatile memory.
//
//Please refer to page 11 of the datasheet for more information.
uint8_t MAX541X::switchNVtoV()
{
    return write(MAX541X_NVREGxVREG, 0);
}


//uint8_t MAX541X::switchVtoNV()
//@return		error message from the Wire.endTransmission() method
//					0:success
//					1:data too long to fit in transmit buffer
//					2:received NACK on transmit of address
//					3:received NACK on transmit of data
//					4:other error
//
//Takes the value stored in the volatile memory and places it in the
//non-volatile memory.
//
//Please refer to page 11 of the datasheet for more information.
uint8_t MAX541X::switchVtoNV()
{
    return write(MAX541X_VREGxNVREG, 0);
}

