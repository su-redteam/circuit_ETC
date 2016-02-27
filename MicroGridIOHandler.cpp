#include "MicroGridIOHandler.h"

#include <asiodnp3/MeasUpdate.h>
#include <iostream>
#include <chrono>
#include <wiringPi.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

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



void microgridIOHandler::doOperate(const ControlRelayOutputBlock& command, uint8_t index)
{
	uint_8 value = (command.functionCode == ControlCode::LATCH_ON) ? 1 : 0;
	writeCircuitStatus(index, value);
}

CommandStatus microgridIOHandler::validateCROB(const ControlRelayOutputBlock& command, uint16_t index)
{
	switch(command.functionCode)
	{
		case(ControlCode::LATCH_ON)
		case(ControlCode::LATCH_OFF)
			return CommandStatus::SUCCESS;
		default:
			return CommandStatus::NOT_SUPPORTED;
	}
}

// read status of all switches, return integer with bits set to 1/on, 0/off_type
// 0 is all off, 255 is all on

int microgridIOHandler::mgioReadInput(void)
{
	int mask = 0;
	REP(i, RELAYMASTER, RELAY6 + 1)
	{
		mask = (1 << i);
		switch_status |= (mask << i);		
	}
	return switch_status;
}

bool microgridIOHandler::isSwitchOn(int data, int num)
{
	int mask = 1 << num;
	return ((data & mask) == 1);
}

microgridIOHandler::writeCircuitStatus(uint8_t index, bool value)
{
	digitalWrite(index + NUM_CIRCUITS, value);
}

// initialize wiringPi status for microGrid
// Switch monitoring pins set to INPUT
// Relay control pins set to OUTPUT
void microgridIOHandler::microgridInit(void)
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

microgridIOHandler::microgridIOHandler()
{
	microgrid_init();
}

void microgridIOHandler::readMeasurements(asiodnp3::IOutstation* pOutstation)
{
	const uint8_t ONLINE = 0x01;
	int data = mgioReadInput(); // get switch values
	
	MeasUpdate tx(pOutstation);
	
	REP(i, 0, 7)
	{
		tx.Update(Binary(isSwitchOn(data, i), ONLINE), i);
	}
}

CommandStatus microgridIOHandler::Select(const ControlRelayOutputBlock& command, uint16_t index)
{
	return validateCROB(command, index);
}

CommandStatus microgridIOHandler::Operate(const ControlRelayOutputBlock& command, uint16_t index)
{
	CommandStatus validation = validateCROB(command, index);
	if(validation == CommandStatus::SUCCESS) doOperate(command, static_cast<char>(index));
	return validation;
}

