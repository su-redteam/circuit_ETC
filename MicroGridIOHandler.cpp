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

#define TEST_DELAY 500

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

#define SHUTDOWN 22

#define ON false
#define OFF true

//DoOperate is deprecated in this version
void MicroGridIOHandler::DoOperate(const ControlRelayOutputBlock& command, uint8_t index)
{
	cout << "\tMGIO DoOperate\n\n";

//	uint8_t value = (command.functionCode == ControlCode::LATCH_ON) ? 1 : 0;
	//writeCircuitStatus(index, value);
}

CommandStatus MicroGridIOHandler::validateCROB(const ControlRelayOutputBlock& command, uint16_t index)
{
	cout << "\tMGIO validateCROB\n\tControl Code ";
	cout << ControlCodeToType(command.functionCode) << endl;
	cout << "\trawCode " << command.rawCode << endl;
	cout << "\tcount " << command.count << endl;
	cout << "\tonTimeMS " << command.onTimeMS << endl;
	cout << "\toffTimeMS" << command.offTimeMS << endl;
	cout << "\tindex" << index << endl;

	if(command.functionCode == ControlCode::LATCH_ON)
	{
		cout << "\treturning SUCCESS\n\n";
		return CommandStatus::SUCCESS;
	}else if(command.functionCode == ControlCode::LATCH_OFF){
		cout << "\treturning SUCCESS\n\n";
		return CommandStatus::SUCCESS;
	}else{
		cout << "\treturning NOT_SUPPORTED\n\n";
		return CommandStatus::NOT_SUPPORTED;
	}
}


// TEST FUNCTION, HELPER FOR isRelayOnTest
bool MicroGridIOHandler::isRelayOn(uint8_t index)
{
	return ( digitalRead(index) );
}

void MicroGridIOHandler::checkShutdown()
{
	if( digitalRead(SHUTDOWN) )
	{
		allOff();
		system("sudo shutdown -h now");
	}
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
		writeCircuitStatus(i);
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

		writeCircuitStatus(i);
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

// turns each circuit ON/OFF
bool MicroGridIOHandler::writeCircuitStatus(uint8_t index)
{
	cout << "\tMGIO writeCircuitStatus\n";
	index += NUM_CIRCUITS;
	// check if index is valid
	if(index >= RELAYMASTER && index < RELAYMASTER + NUM_CIRCUITS)
	{
		digitalWrite( index, getStatus(index - NUM_CIRCUITS) );
		//delay(25);
		return true;
	}
	printf( "\t%0.0f is not a valid index\n", (double) index );
	return false;
}


// initialize wiringPi status for microGrid
// Switchy monitoring pins set to INPUT
// Relay control pins set to OUTPUT
void MicroGridIOHandler::microgridInit(void)
{
	wiringPiSetup();

	cout << "\tMGIO init\n\n";

	REP( i, FAULTMASTER, NUM_CIRCUITS )
	{
		pinMode( i, INPUT );
		status[i] = OFF;
	}

	REP( i, RELAYMASTER, RELAYMASTER + NUM_CIRCUITS )
	{
		pinMode( i, OUTPUT );
		digitalWrite( i, OFF );
	}
	post();

	pinMode( SHUTDOWN, INPUT );
}

// power on self test
// cycles each branch of the circuit ON/OFF
// will set status of all branches to ON
void MicroGridIOHandler::post()
{

	cout << "\tpost\n\n\tPress Enter to Continue\n\n";
//	cin.get();
	setStatus(FAULTMASTER);
	writeCircuitStatus(FAULTMASTER);

	REP( i, FAULT1, FAULTMASTER + NUM_CIRCUITS )
	{
		setStatus(i);
		writeCircuitStatus(i);
		delay(TEST_DELAY);
		setStatus(i);
		writeCircuitStatus(i);
		delay(TEST_DELAY);
	}
	cout << "\tcycle 1, turn all on\n";
	cycleAll(TEST_DELAY); // all ON
	cout << "\tcycle 2, turn all off\n";
	cycleAll(TEST_DELAY); // all OFF
	cout << "\tcycle 3, turn all on\n";
	cycleAll(TEST_DELAY); // all ON
}

// cycles all circuits ON/OFF simultaneously
void MicroGridIOHandler::cycleAll(int delayTime)
{
	cout << "\tcycleAll\n\n\tPress Enter to Continue\n\n";
//	cin.get();
	REP( i, FAULT1, FAULTMASTER + NUM_CIRCUITS )
	{
		setStatus(i);
		writeCircuitStatus(i);
	}
	delay(delayTime);
}

void MicroGridIOHandler::allOff()
{
	digitalWrite(FAULTMASTER, OFF);
}

void MicroGridIOHandler::setStatus(uint8_t index)
{
	cout << "\tsetStatus\n\n\tPress Enter to Continue\n\n";
//	cin.get();
	cout << "\tSwitching Status of Circuit " << ((int)index ) << " from ";
	cout << status[(int)index];
	cout << " to " << !status[(int)index] << "\n\n";

	status[index] = !status[index];
	//writeCircuitStatus( index );
}

MicroGridIOHandler::MicroGridIOHandler()
{
//	status = baseStatus;
	microgridInit();
}

void MicroGridIOHandler::ReadMeasurements(asiodnp3::IOutstation* pOutstation)
{
//	cout << "\tMGIO ReadMeasurements\n\n";

	const uint8_t ONLINE = 0x01;

	MeasUpdate tx(pOutstation);

	REP( i, FAULTMASTER, FAULT6 + 1 )
	{
		tx.Update( Binary(digitalRead(i), ONLINE), i );
	}
}

bool MicroGridIOHandler::getStatus(uint8_t index)
{
//	cout << "\tgetStatus\n\n\tPress Enter to Continue\n\n";
//	cin.get();
	if( index >= 0 && index < getSize() )
	{
		//cout << status[index];
		cout << "\tgetStatus " << (int)index << " return status " << status[index] << "\n\n";
		return( status[index] );
	}else{
		cout << "\tgetStatus " << (int)index << " return NULL\n\n";
		return( NULL );
	}
}

void MicroGridIOHandler::testSelect(uint16_t index)
{
	cout << "testSelect: Check if functionality of Select operation is correct.\n\n";
	setStatus( (uint8_t) index );
	writeCircuitStatus( (uint8_t) index );
}

CommandStatus MicroGridIOHandler::Select(const ControlRelayOutputBlock& command, uint16_t index)
{
	cout << "\tMGIO select\n\n";
	setStatus( (uint8_t) index );
	writeCircuitStatus( (uint8_t) index );
	delay(25);

	return CommandStatus::NOT_SUPPORTED;
//	validateCROB is deprecated in this version
//	return validateCROB(command, index);
}

// this function is deprecated in the current version

CommandStatus MicroGridIOHandler::Operate(const ControlRelayOutputBlock& command, uint16_t index)
{
	cout << "\tMGIO Operate\n\n";

	CommandStatus validation = validateCROB(command, index);
	if(validation == CommandStatus::SUCCESS) 
	{
		DoOperate(command, static_cast<char>(index));
	}
	return validation;
}


