
#include <asiodnp3/DNP3Manager.h>
#include <asiodnp3/ConsoleLogger.h>
#include <opendnp3/outstation/OutstationStackConfig.h>
#include <opendnp3/link/ChannelRetry.h>
#include <opendnp3/LogLevels.h>

#include <ICommandHandler.h>

#include "MicroGridIOHandler.h"

#include <thread>
#include <chrono>

using namespace std;
using namespace openpal;
using namespace opendnp3;
using namespace asiodnp3;

int main(int argc, char* argv[])
{
	MicroGridIOHandler ioHandler; // handles control request, input polling, and measurement tracking/updates
		
	const uint32_t FILTERS = levels::NORMAL;
	DNP3Manager dnp3(1);
	dnp3.AddLogSubscriber(&ConsoleLogger::Instance());

	auto channel =  dnp3.AddTCPServer("server", FILTERS, opendnp3::ChannelRetry::Default(), "0.0.0.0", 20000);

	OutstationStackConfig stackConfig;
	stackConfig.dbTemplate = DatabaseTemplate::BinaryOnly(4);
	stackConfig.outstation.eventBufferConfig = EventBufferConfig::AllTypes(10);
	stackConfig.outstation.params.allowUnsolicited = true;

	auto outstation = channel->AddOutstation("outstation", ioHandler, DefaultOutstationApplication::Instance(), stackConfig);

	outstation->Enable();

	do {
		ioHandler.read_measurements(outstation);
		this_thread::sleep_for( chrono::milliseconds(100) );
	}
	while(true);

	return 0;
}
