#include "PifaceIOHandler.h"

extern "C" {
#include <libpiface-1.0/pfio.h>
}

#include <asiodnp3/MeasUpdate.h>

#include <iostream>
#include <chrono>

using namespace opendnp3;
using namespace asiodnp3;

void PifaceIOHandler::DoOperate(const ControlRelayOutputBlock& command, uint8_t index)
{
	uint8_t value = (command.functionCode == ControlCode::LATCH_ON) ? 1 : 0;
	pfio_digital_write(index, value);
}

CommandStatus PifaceIOHandler::ValidateCROB(const ControlRelayOutputBlock& command, uint16_t index)
{
	if(index >= 8)
	{
	  return CommandStatus::NOT_SUPPORTED;
	}

	switch(command.functionCode)
	{
	    case(ControlCode::LATCH_ON):
	    case(ControlCode::LATCH_OFF):
	      return CommandStatus::SUCCESS;
	    default:
	      return CommandStatus::NOT_SUPPORTED;
	}
}

bool PifaceIOHandler::isSwitchOn(int data, int num)
{
	int mask = 1 << num; // 255 is all off, switch 0 is lowest bit
	bool ret = ((data & mask) == 0);
	return ret;
}


PifaceIOHandler::PifaceIOHandler()
{
	int result = pfio_init();
	if(result < 0)
	{
		std::cerr << "Unable to initialize piface" << std::endl;
		exit(result);
	}
}

PifaceIOHandler::~PifaceIOHandler()
{
	pfio_deinit();
}

void PifaceIOHandler::ReadMeasurements(asiodnp3::IOutstation* pOutstation)
{
	const uint8_t ONLINE = 0x01;
	//int data = pfio_read_input();

	MeasUpdate tx(pOutstation);
	tx.Update(Binary(isSwitchOn(data, 0), ONLINE), 0);
	tx.Update(Binary(isSwitchOn(data, 1), ONLINE), 1);
	tx.Update(Binary(isSwitchOn(data, 2), ONLINE), 2);
	tx.Update(Binary(isSwitchOn(data, 3), ONLINE), 3);
}

CommandStatus PifaceIOHandler::Select(const ControlRelayOutputBlock& command, uint16_t index)
{
	return ValidateCROB(command, index);
}

CommandStatus PifaceIOHandler::Operate(const ControlRelayOutputBlock& command, uint16_t index)
{
	CommandStatus validation = ValidateCROB(command, index);
	if(validation == CommandStatus::SUCCESS) DoOperate(command, static_cast<char>(index));
	return validation;
}

