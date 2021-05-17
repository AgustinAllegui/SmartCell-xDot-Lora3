#include "dot_util.h"
#include "RadioEvent.h"

#if ACTIVE_EXAMPLE == PULSE_METER

#include "PulseCounter.h"

/////////////////////////////////////////////////////////////
// * these options must match the settings on your gateway //
// * edit their values to match your configuration         //
// * frequency sub band is only relevant for the 915 bands //
// * either the network name and passphrase can be used or //
//     the network ID (8 bytes) and KEY (16 bytes)         //
/////////////////////////////////////////////////////////////
static std::string network_name = "MultiTech";
static std::string network_passphrase = "MultiTech";

static uint8_t network_id[] = APP_EUI;
static uint8_t network_key[] = APP_KEY;

static uint8_t frequency_sub_band = 2;
static lora::NetworkType network_type = lora::PUBLIC_LORAWAN;
static uint8_t join_delay = 5;
static uint8_t ack = 1;
static bool adr = true;

mDot *dot = NULL;
lora::ChannelPlan *plan = NULL;

Serial pc(USBTX, USBRX);

// [START] Pulse counter global

PulseCounter pulseCounter(PA_11, true);

// [END] Pulse counter global

int main()
{
    // Custom event handler for automatically displaying RX data
    RadioEvent events;

    pc.baud(9600);

    mts::MTSLog::setLogLevel(mts::MTSLog::TRACE_LEVEL);

#if CHANNEL_PLAN == CP_US915
    plan = new lora::ChannelPlan_US915();
#elif CHANNEL_PLAN == CP_AU915
    plan = new lora::ChannelPlan_AU915();
#elif CHANNEL_PLAN == CP_EU868
    plan = new lora::ChannelPlan_EU868();
#elif CHANNEL_PLAN == CP_KR920
    plan = new lora::ChannelPlan_KR920();
#elif CHANNEL_PLAN == CP_AS923
    plan = new lora::ChannelPlan_AS923();
#elif CHANNEL_PLAN == CP_AS923_JAPAN
    plan = new lora::ChannelPlan_AS923_Japan();
#elif CHANNEL_PLAN == CP_IN865
    plan = new lora::ChannelPlan_IN865();
#endif
    assert(plan);

    dot = mDot::getInstance(plan);
    assert(dot);

    logInfo("mbed-os library version: %d.%d.%d", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);

    // start from a well-known state
    logInfo("defaulting Dot configuration");
    dot->resetConfig();
    dot->resetNetworkSession();

    // make sure library logging is turned on
    dot->setLogLevel(mts::MTSLog::INFO_LEVEL);

    // attach the custom events handler
    dot->setEvents(&events);

    // update configuration if necessary
    if (dot->getJoinMode() != mDot::OTA)
    {
        logInfo("changing network join mode to OTA");
        if (dot->setJoinMode(mDot::OTA) != mDot::MDOT_OK)
        {
            logError("failed to set network join mode to OTA");
        }
    }
    // in OTA and AUTO_OTA join modes, the credentials can be passed to the library as a name and passphrase or an ID and KEY
    // only one method or the other should be used!
    // network ID = crc64(network name)
    // network KEY = cmac(network passphrase)
    // update_ota_config_name_phrase(network_name, network_passphrase, frequency_sub_band, network_type, ack);
    update_ota_config_id_key(network_id, network_key, frequency_sub_band, network_type, ack);

    // configure the Dot for class C operation
    // the Dot must also be configured on the gateway for class C
    // use the lora-query application to do this on a Conduit: http://www.multitech.net/developer/software/lora/lora-network-server/
    // to provision your Dot for class C operation with a 3rd party gateway, see the gateway or network provider documentation
    logInfo("changing network mode to class C");
    if (dot->setClass("C") != mDot::MDOT_OK)
    {
        logError("failed to set network mode to class C");
    }

    // enable or disable Adaptive Data Rate
    dot->setAdr(adr);

    // Configure the join delay
    dot->setJoinDelay(join_delay);

    // save changes to configuration
    logInfo("saving configuration");
    if (!dot->saveConfig())
    {
        logError("failed to save configuration");
    }

    // display configuration
    display_config();
    
    // [START] init Pulse counter
    logInfo("========================");
    logInfo("PULSE METER version");
    logInfo("========================");
    
    uint16_t pulses = 0;
		
		#if TEST_DATA_GEN == 1
		const uint16_t minValue = 20;
		const uint16_t maxValue = 100;
		const uint16_t upValue = 2;
		uint16_t lastValue = minValue;
		#endif
    
    // [END] init Pulse counter

    while (true)
    {
        std::vector<uint8_t> tx_data;

        // join network if not joined
        if (!dot->getNetworkJoinStatus())
        {
            join_network();
        }
				
				// ask for time
				logInfo("Timestamp: %lld", dot->getGPSTime() + 315964800000);

        // [START] Pulse counter Loop
        pulses = pulseCounter.read();
        logInfo("Pulses counted: %u", pulses);
        pulseCounter.reset();
				
				#if TEST_DATA_GEN == 1
				pulses = lastValue + upValue;
				if(lastValue > maxValue) {
					pulses = minValue;
				}
				lastValue = pulses;
				logInfo("Pulses simulated: %u", pulses);
				#endif
				
        
        if (dot->getNetworkJoinStatus())
        {
            tx_data.clear();
            tx_data.push_back(0x53);
            tx_data.push_back(0x00);
            tx_data.push_back(0x00);
            tx_data.push_back(0x00);
            tx_data.push_back((uint8_t)(pulses & 0x00FF));
        }
				
				
				
        // [END] Pulse counter Loop
        
        
        if (dot->getNetworkJoinStatus())
        {
            send_data(tx_data);
        }
        
       
        
        // the Dot can't sleep in class C mode
        // it must be waiting for data from the gateway
        // send data every 30s
        logInfo("waiting for 300s");
        wait(30);
				
    }

    return 0;
}

#endif

