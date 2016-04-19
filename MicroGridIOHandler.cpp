#include "MicroGridIOHandler.h"

#include <asiodnp3/MeasUpdate.h>
#include <iostream>
#include <istream>
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
using namespace std;

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

#define ON false
#define OFF true

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


// TEST FUNCTION, HELPER FOR isRelayOnTest
bool MicroGridIOHandler::isRelayOn(int num)
{
	return (digitalRead(num));
}

// TEST FUNCTIONALITY OF READING CIRCUIT STATUS FOR EACH POSSIBLE CASE

void MicroGridIOHandler::isRelayOnTest()
{
	// TEST OVERRIDE ON DETECTION. ALL CIRCUITS SHOULD BE ACTIVE.
	cout << "isRelayOnTest\n\n";
	cout << "This will test if active circuits are being read correctly\n";
	cout << "Please set all switches to override \'on\'\n";
	cout << "Press enter to continue\n\n";

	REP(i, FAULTMASTER, FAULT6 + 1)
	{
		if(isRelayOn(i))
		{
			cout << "Circuit " << i << " is active\n";
			cout << "Press enter to continue\n\n"; 
			cin.get();
		}else{
			cout << "Error on circuit " << i << endl;
			cout << "Press enter to continue\n\n";
			cin.get();
		}
	}

	// TEST OVERRIDE OFF DETECTION. ALL CIRCUITS SHOULD BE INACTIVE.	
	cout << "This will test if inactive circuits are being read correctly\n";
	cout << "Please set all switches to override \'off\'\n";
	cout << "Press enter to continue\n\n";
	cin.get();
 
	REP(i, FAULTMASTER, FAULT6 + 1)
	{
		if(!isRelayOn(i))
		{
			cout << "Circuit " << i << " is inactive\n";
			cout << "Press enter to continue\n\n";
			cin.get();
		}else{
			cout << "Error on circuit " << i << endl;
			cout << "Press enter to continue\n\n";
			cin.get();
		}
	}

	// TEST REMOTE CIRCUIT ACTIVATION. EACH CIRCUIT WILL CYCLE, DISPLAYING STATUS TO THE TERMINAL
	cout << "This will test the ability of this program to write the correct status to each circuit\n";
	cout << "Please set all switches to \'Remote\'\n";
	cout << "Press enter to continue\n\n";
	cin.get();
	
	REP(i, FAULTMASTER, FAULT6 + 1)
	{
		writeCircuitStatus(i, ON);
		if(isRelayOn(i))
		{
			cout << "Circuit " << i << " is active\n";
			cout << "Press enter to continue\n\n";
			cin.get();
		}else{
			cout << "Error on circuit " << i << endl;
			cout << "Press enter to continue\n\n";
			cin.get();
		}

		writeCircuitStatus(i, OFF);
		if(!isRelayOn(i))
		{
			cout << "Circuit " << i << " is inactive\n"; 
			cout << "Press enter to continue\n\n";
			cin.get();
		}else{
			cout << "Error on circuit " << i << endl;
			cout << "Press enter to continue\n\n";
			cin.get();
		}
	}
	
	cout << "\n\nTesting complete\n\n";
	cout << "Press any key to continue" << endl << endl;
	cin.get();
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
		digitalWrite(i, OFF);
	}
}

MicroGridIOHandler::MicroGridIOHandler()
{
	microgridInit();
}

void MicroGridIOHandler::ReadMeasurements(asiodnp3::IOutstation* pOutstation)
{
	const uint8_t ONLINE = 0x01;
//	uint8_t data = mgioReadInput(); // get relay values
	
	MeasUpdate tx(pOutstation);
	
	REP(i, FAULTMASTER, FAULT6 + 1)
	{
		tx.Update(Binary(digitalRead(i), ONLINE), i);
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
