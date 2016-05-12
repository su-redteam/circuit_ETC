#ifndef __MICROGRID_IO_HANDLER_H_
#define __MICROGRID_IO_HANDLER_H_

#include <iostream>
#include <stdio.h>
#include <opendnp3/outstation/ICommandHandler.h>
#include <asiodnp3/IOutstation.h>

#define NUM_CIRCUITS 7

#define ON false
#define OFF true


using namespace std;

class MicroGridIOHandler final : public opendnp3::ICommandHandler
{

private:

	void Start() override{}
	void End() override{}

	void DoOperate(const opendnp3::ControlRelayOutputBlock& command, uint8_t index);

	bool status[NUM_CIRCUITS] = {};

	opendnp3::CommandStatus validateCROB(const opendnp3::ControlRelayOutputBlock& command, uint16_t index);


	bool isRelayOn(uint8_t index);
	bool writeCircuitStatus(uint8_t index);
	void microgridInit(void);
	void setStatus(uint8_t index);
	void post();
	void cycleAll(int delayTime);
	void allOff();
	public:

	MicroGridIOHandler();
	~MicroGridIOHandler(){};

	void ReadMeasurements(asiodnp3::IOutstation* pOutstation); // reads status of each switch and relay, updates ETC

	bool getStatus(uint8_t index);
	void checkShutdown();

	int getSize(){
		return( NUM_CIRCUITS );
	}

	// test functions
	void isRelayOnTest();

	void testSelect(uint16_t index);

	//These two functions are to perform Relay Outputs, aka On/Off controls.
	opendnp3::CommandStatus Select(const
					opendnp3::ControlRelayOutputBlock& command, uint16_t index) override final;
	opendnp3::CommandStatus Operate(const
					opendnp3::ControlRelayOutputBlock& command, uint16_t index) override final;

//The following functions perform Analog Output, aka Setpoint controls.
//All of these functions are stubbed out for now, and return a status of "NOT_SUPPORTED" if invoked.
//This is because our Raspberry Pi circuit does not have hardware to handle Setpoints yet.
//If they did work, for example, e-terracontrol would send a setpoint control with the value "10" to the Raspberry Pi RTU.
//The Raspberry Pi RTU would output a pulse train representing "10" to specialized hardware. Then the hardware would set water level to 10ft, for example.
	opendnp3::CommandStatus Select(const opendnp3::AnalogOutputInt16&
									command, uint16_t index) override 
		{	cout << "Select Int16\n\n";
			return opendnp3::CommandStatus::NOT_SUPPORTED; } 
		opendnp3::CommandStatus Operate(const opendnp3::AnalogOutputInt16& command, uint16_t index)
		override 
		{	cout << "Operate Int16\n\n";
			return opendnp3::CommandStatus::NOT_SUPPORTED; }

	opendnp3::CommandStatus Select(const opendnp3::AnalogOutputInt32&
									command, uint16_t index) override 
		{	cout << "Select Int32\n\n";
			return	opendnp3::CommandStatus::NOT_SUPPORTED; } 
	opendnp3::CommandStatus Operate(const opendnp3::AnalogOutputInt32& command, uint16_t index) 
		override 
		{	cout << "Operate Int32\n\n";
			return opendnp3::CommandStatus::NOT_SUPPORTED; }

	opendnp3::CommandStatus Select(const opendnp3::AnalogOutputFloat32
									&command, uint16_t index) 
		{	cout << "Select Float32\n\n";
			return	opendnp3::CommandStatus::NOT_SUPPORTED; } 
	opendnp3::CommandStatus Operate(const opendnp3::AnalogOutputFloat32 &command, uint16_t index)
		{	cout << "Operate Float32\n\n";
			return opendnp3::CommandStatus::NOT_SUPPORTED; }

	opendnp3::CommandStatus Select(const opendnp3::AnalogOutputDouble64&
									command, uint16_t index) override 
		{	cout << "Select Double64\n\n";
			return opendnp3::CommandStatus::NOT_SUPPORTED; } 
	opendnp3::CommandStatus Operate(const opendnp3::AnalogOutputDouble64& command, uint16_t index)
		override 
		{	cout << "Operate Double64\n\n";
			return opendnp3::CommandStatus::NOT_SUPPORTED; }	
};

#endif
