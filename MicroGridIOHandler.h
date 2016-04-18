#ifndef __MICROGRID_IO_HANDLER_H_
#define __MICROGRID_IO_HANDLER_H_

#include <opendnp3/outstation/ICommandHandler.h>
#include <asiodnp3/IOutstation.h>

class MicroGridIOHandler final : public opendnp3::ICommandHandler
{

private:

	void Start() override{}
	void End() override{}
	
	void DoOperate(const opendnp3::ControlRelayOutputBlock& command, uint8_t index);
	
	
	opendnp3::CommandStatus validateCROB(const opendnp3::ControlRelayOutputBlock& command, uint16_t index);
		
	uint8_t mgioReadInput(void);
	bool isRelayOn(int num);
	bool writeCircuitStatus(uint8_t index, bool value);
	void microgridInit(void);
	

	public:
	
	uint8_t switchStatus = 0;	

	MicroGridIOHandler();
	~MicroGridIOHandler(){};
	
	void ReadMeasurements(asiodnp3::IOutstation* pOutstation); // reads status of each switch and relay, updates ETC
	
	// test functions
	char mgioReadInputTest();
	void isRelayOnTest();
	void writeCircuitStatusTest();

	
	//These two functions are to perform Relay Outputs, aka On/Off controls.
opendnp3::CommandStatus Select(const
opendnp3::ControlRelayOutputBlock& command, uint16_t index);
opendnp3::CommandStatus Operate(const
opendnp3::ControlRelayOutputBlock& command, uint16_t index);

//The following functions perform Analog Output, aka Setpoint controls.
//All of these functions are stubbed out for now, and return a status of "NOT_SUPPORTED" if invoked.
//This is because our Raspberry Pi circuit does not have hardware to handle Setpoints yet.
//If they did work, for example, e-terracontrol would send a setpoint control with the value "10" to the Raspberry Pi RTU.
//The Raspberry Pi RTU would output a pulse train representing "10" to specialized hardware. Then the hardware would set water level to 10ft, for example.
	opendnp3::CommandStatus Select(const opendnp3::AnalogOutputInt16&
									command, uint16_t index) override 
		{return opendnp3::CommandStatus::NOT_SUPPORTED; } 
		opendnp3::CommandStatus Operate(const opendnp3::AnalogOutputInt16& command, uint16_t index)
		override 
		{return opendnp3::CommandStatus::NOT_SUPPORTED; }

	opendnp3::CommandStatus Select(const opendnp3::AnalogOutputInt32&
									command, uint16_t index) override 
		{return	opendnp3::CommandStatus::NOT_SUPPORTED; } 
	opendnp3::CommandStatus Operate(const opendnp3::AnalogOutputInt32& command, uint16_t index) 
		override 
		{return opendnp3::CommandStatus::NOT_SUPPORTED; }

	opendnp3::CommandStatus Select(const opendnp3::AnalogOutputFloat32
									&command, uint16_t index) 
		{return	opendnp3::CommandStatus::NOT_SUPPORTED; } 
	opendnp3::CommandStatus Operate(const opendnp3::AnalogOutputFloat32 &command, uint16_t index)
		{return opendnp3::CommandStatus::NOT_SUPPORTED; }

	opendnp3::CommandStatus Select(const opendnp3::AnalogOutputDouble64&
									command, uint16_t index) override 
		{return opendnp3::CommandStatus::NOT_SUPPORTED; } 
	opendnp3::CommandStatus Operate(const opendnp3::AnalogOutputDouble64& command, uint16_t index)
		override 
		{return opendnp3::CommandStatus::NOT_SUPPORTED; }	
};

#endif
