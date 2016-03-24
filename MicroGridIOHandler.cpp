#include "MicroGridIOHandler.h"

#include <asiodnp3/MeasUpdate.h>
#include <iostream>
#include <chrono>
#include <wiringPi.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <climits>
using namespace opendnp3;
using namespace asiodnp3;

// better for loops
#define REP(i,a,b)      for (int i = int(a); i < int(b); i++)

#define NUM_CIRCUITS 7

// these will be the pins which model faults for each circuit
#define FAULTMASTER 0
#define FAULT1 1
#define FAULT2 2
#define FAULT3 3
#define FAULT4 4
#define FAULT5 5
#define FAULT6 6

// these pins will enable/disable the relays
#define RELAYMASTER 7
#define RELAY1 8
#define RELAY2 9
#define RELAY3 10
#define RELAY4 11
#define RELAY5 12
#define RELAY6 13


void MicroGridIOHandler::DoOperate(const ControlRelayOutputBlock& command, uint8_t index)
{
	uint8_t value = (command.functionCode == ControlCode::LATCH_ON) ? 1 : 0;
	writeCircuitStatus(index, value);
}

CommandStatus MicroGridIOHandler::validateCROB(const ControlRelayOutputBlock& command, uint16_t index)
{
	switch(command.functionCode)
	{
		case(ControlCode::LATCH_ON):
		case(ControlCode::LATCH_OFF):
			return CommandStatus::SUCCESS;
		default:
			return CommandStatus::NOT_SUPPORTED;
	}
}

// read status of all relays, return integer with bits set to 1/on, 0/off_type
// 0 is all off, 255 is all on

uint8_t MicroGridIOHandler::mgioReadInput()
{
	uint8_t mask = 0;
	REP(i, RELAYMASTER, RELAY6 + 1)
	{
		mask = (1 << i);
		switchStatus |= (mask << i);		
	}
	return switchStatus;
}

char MicroGridIOHandler::mgioReadInputTest()
{
	printf("microgridIOHandler::mgioReadInputTest. This will test if the function is reading the correct values for "
			"each relay. Output is an 8-bit binary. For each digit, 1 is on, 0 is off.\n\n");
	char buffer[7];
	uint8_t num = mgioReadInput();
	
	for(uint8_t i = 0; i < (CHAR_BIT)*sizeof(uint8_t); i++)
	{
		uint8_t mask = 0;
		mask = 1 << i;
		if(num & mask)
			buffer[i] = '1';
		else
			buffer[i] = '0';
	}
	printf("Circuit status %s\n", buffer);
	return *buffer;
}

// parses value calculated by mgioReadInput, returns true if circuit num is hot
bool MicroGridIOHandler::isRelayOn(uint8_t data, int num)
{
	int mask = 1 << num;
	return ((data & mask) == 1);
}

// Test if function isRelayOn is returning correct boolean values. Checks relay status with mgioReadInputTest
// Then displays t/f values returned from isRelayOn
void MicroGridIOHandler::isRelayOnTest()
{
	char status = mgioReadInputTest();
	//uint8_t status = mgioReadInput();
	printf("Current system status: %s\n\n", &status);
	uint8_t data = mgioReadInput();
	for(int i = 0; i < NUM_CIRCUITS; i++)
	{
		if(isRelayOn(data, i))
		{
			printf("Relay %0.0f is on\n\n", (double) i);
		}else{
			printf("Relay %0.0f is off\n\n", (double) i);
		}
	}
}

// turns each circuit on/off
bool MicroGridIOHandler::writeCircuitStatus(uint8_t index, bool value)
{
	// check if index is valid
	if(index <= NUM_CIRCUITS)
	{
		digitalWrite(index + NUM_CIRCUITS, value);
		delay(25);
		return true;
	}
	printf("%0.0f is not a valid index\n", (double) index);
	return false;
}

// test function for microgridIOHandler::writeCircuitStatus
void MicroGridIOHandler::writeCircuitStatusTest()
{
	printf("microgridIOHandler::writeCircuitStatusTest. This test cycles each circuit on\n "
				"then off in .5 second intervals, then tests the function\'s bounds\n "
				"by sending an invalid index. Output should be:\n\n"
				"Floor 1 successfully activated\n"
				"Floor 1 successfully deactivated\n\n"
				"Floor 2 successfully activated\n"
				"Floor 2 successfully deactivated\n\n"
				"Floor 3 successfully activated\n"
				"Floor 3 successfully deactivated\n\n"
				"Floor 4 successfully activated\n"
				"Floor 4 successfully deactivated\n\n"
				"Floor 5 successfully activated\n"
				"Floor 5 successfully deactivated\n\n"
				"Floor 6 successfully activated\n"
				"Floor 6 successfully deactivated\n\n"
				"7 is not a valid index\n"
				"7 is not a valid index\n");
				
	for(int i = 0; i <= NUM_CIRCUITS; i++)
	{
		if(writeCircuitStatus(i, true))
		{
			printf("Floor %0.0f successfully activated\n", (double) i);
			delay(500);
		}
		if(writeCircuitStatus(i, false))
		{
			printf("Floor %0.0f successfully deactivated\n", (double) i);
			delay(500);
		}
	}
}

// initialize wiringPi status for microGrid
// Switch monitoring pins set to INPUT
// Relay control pins set to OUTPUT
void MicroGridIOHandler::microgridInit(void)
{
	wiringPiSetup();
	
	REP(i, FAULTMASTER, NUM_CIRCUITS)
	{
		pinMode(i, INPUT);
	}
	
	REP(i, RELAYMASTER, RELAYMASTER + NUM_CIRCUITS)
	{
		pinMode(i, OUTPUT);
	}
}

MicroGridIOHandler::MicroGridIOHandler()
{
	microgridInit();
}

void MicroGridIOHandler::ReadMeasurements(asiodnp3::IOutstation* pOutstation)
{
	const uint8_t ONLINE = 0x01;
	uint8_t data = mgioReadInput(); // get relay values
	
	MeasUpdate tx(pOutstation);
	
	REP(i, 0, 7)
	{
		tx.Update(Binary(isRelayOn(data, i), ONLINE), i);
	}
}

CommandStatus MicroGridIOHandler::Select(const ControlRelayOutputBlock& command, uint16_t index)
{
	return validateCROB(command, index);
}

CommandStatus MicroGridIOHandler::Operate(const ControlRelayOutputBlock& command, uint16_t index)
{
	CommandStatus validation = validateCROB(command, index);
	if(validation == CommandStatus::SUCCESS) 
	{
		DoOperate(command, static_cast<char>(index));
	}
	return validation;
}



