/*
 FILENAME:	MAX541X.h
 AUTHORS:	Orlando S. Hoilett and Rohit Srivastava
 EMAIL:     ohoilett@purdue.edu
 
 
 Please see .cpp file for extended descriptions, instructions, and version updates
 
 
 Linnes Lab code, firmware, and software is released under the MIT License
 (http://opensource.org/licenses/MIT).
 
 The MIT License (MIT)
 
 Copyright (c) 2019 Linnes Lab, Purdue University, West Lafayette, IN, USA
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 of the Software, and to permit persons to whom the Software is furnished to do
 so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 
 */



#ifndef MAX541X_h
#define MAX541X_h


#include <Arduino.h>
#include <Wire.h>


//Register addresses
const uint8_t MAX541X_VREG = 0x11;
const uint8_t MAX541X_NVREG	= 0x21;
const uint8_t MAX541X_NVREGxVREG = 0x61;
const uint8_t MAX541X_VREGxNVREG = 0x51;


//Configuration of the resistor
//WL is Wiper to L an WH is Wiper to H
const uint8_t MAX541X_WL = 0;
const uint8_t MAX541X_WH = 1;


class MAX541X
{
	
private:
	
	uint8_t _addr;
	uint32_t _Rfull;
	uint8_t _mode;
	uint8_t _Rcode;
	
	
public:
	
	//The model of the device sets the value of the full-scale resisatance
	enum Model {
		MAX5417 = 50000,
		MAX5418 = 100000,
		MAX5419 = 200000
	};
	
	
	//Constructors
	MAX541X(); //Default Constructor
    MAX541X(const uint8_t addr, const Model m, const uint8_t mode);
	
	//Normal write method to the device
	uint8_t write(uint8_t reg, uint8_t data);
	
	//Sets and gets the value of the resistor
	uint8_t setR(uint32_t r);
	uint32_t getR() const;

	//Switching between volatile and non-volatile memory
	uint8_t writeNonvolatile(const uint8_t data);
	uint8_t switchVtoNV();
	uint8_t switchNVtoV();

};

#endif
