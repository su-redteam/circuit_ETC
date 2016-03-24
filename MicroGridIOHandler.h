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
	bool isRelayOn(uint8_t data, int num);
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

	
	// not quite sure what these fxns do. I think we may need to adapt for analog reading of battery level
	
	virtual opendnp3::CommandStatus Select(const opendnp3::ControlRelayOutputBlock& command, uint16_t index);
	virtual opendnp3::CommandStatus Operate(const opendnp3::ControlRelayOutputBlock& command, uint16_t index);
	
	virtual opendnp3::CommandStatus Select(const opendnp3::AnalogOutputInt16& command, uint16_t index)
		override {return opendnp3::CommandStatus::NOT_SUPPORTED; }
	virtual opendnp3::CommandStatus Operate(const opendnp3::AnalogOutputInt16& command, uint16_t index)  
		override {return opendnp3::CommandStatus::NOT_SUPPORTED; }		
		
	virtual opendnp3::CommandStatus Select(const opendnp3::AnalogOutputInt32& command, uint16_t index) 
		override {return opendnp3::CommandStatus::NOT_SUPPORTED; }
	virtual opendnp3::CommandStatus Operate(const opendnp3::AnalogOutputInt32& command, uint16_t index) 
		override {return opendnp3::CommandStatus::NOT_SUPPORTED; }		
		
	virtual opendnp3::CommandStatus Select(const opendnp3::AnalogOutputFloat32& command, uint16_t index) 
		override {return opendnp3::CommandStatus::NOT_SUPPORTED; }
	virtual opendnp3::CommandStatus Operate(const opendnp3::AnalogOutputFloat32& command, uint16_t index) 
		override {return opendnp3::CommandStatus::NOT_SUPPORTED; }		
		
	virtual opendnp3::CommandStatus Select(const opendnp3::AnalogOutputDouble64& command, uint16_t index) 
		override {return opendnp3::CommandStatus::NOT_SUPPORTED; }
	virtual opendnp3::CommandStatus Operate(const opendnp3::AnalogOutputDouble64& command, uint16_t index) 
		override {return opendnp3::CommandStatus::NOT_SUPPORTED; }		
		
};

#endif
