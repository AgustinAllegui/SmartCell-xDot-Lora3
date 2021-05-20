#include "dot_util.h"
// #include "RadioEvent.h"
#include "EventSlot.h"

#if ACTIVE_EXAMPLE == LUMINARY

// [START] Luminary Includes

#include "CurrentSensor.h"
#include "DimmingCurves.h"
#include "PhotoCell.h"
#include "OnOffByTime.h"
#include "LightController.h"
#include "LightOutput.h"
#include "DimmingDemo.h"

#include "SmartCell_util.h"

#include "LedHandler.h"

// [END] Luminary Includes

/////////////////////////////////////////////////////////////
// * these options must match the settings on your gateway //
// * edit their values to match your configuration         //
// * frequency sub band is only relevant for the 915 bands //
// * either the network name and passphrase can be used or //
//     the network ID (8 bytes) and KEY (16 bytes)         //
/////////////////////////////////////////////////////////////
static std::string network_name = "MultiTech";
static std::string network_passphrase = "MultiTech";

// config for OTA
static uint8_t network_id[] = APP_EUI;
static uint8_t network_key[] = APP_KEY;

// config for ABP
static uint8_t network_address[] = ABP_ADDRESS;
static uint8_t network_session_key[] = ABP_NET_KEY;
static uint8_t data_session_key[] = ABP_DATA_KEY;

static uint8_t frequency_sub_band = LORA_SUB_BAND;
static lora::NetworkType network_type = lora::PUBLIC_LORAWAN;
static uint8_t join_delay = 5;
static uint8_t ack = LORA_ACK_ENABLE;
static bool adr = true;

mDot *dot = NULL;
lora::ChannelPlan *plan = NULL;

Serial pc(USBTX, USBRX);

// [START] Luminary global
LedHandler ledStatus(PA_4, false); // led indicador de estado
LedHandler ledLora(PA_5, false);   // led Indicador de alimentacion

// Declaracion componentes
CurrentSensor currentSensor(PB_12);
PhotoCell photoCell(PB_13, 0.65, 0.7);
OnOffByTime onOffByTime(18, 0, 6, 0);
DimmingCurves dimmingCurves;
uint8_t customCurve[12];

DimmingDemo dimmingDemo;
LightController lightController(&photoCell, &dimmingCurves, &onOffByTime, &dimmingDemo, LightController::OpMode::Manual);
LightOutput lightOutput(PB_2, PB_0);

// otras variables
uint16_t loopDelay = 30; // amount of seconds between loops
uint8_t loopsToSend = 20; // cantidad de bucles entre transmisiones
uint8_t loopsCount = 0; // contador para saber si toca transmitir
float lastDimming = 0;  // memoria para detectar cambios en el dimming
bool bypassLoopDelay = false; // bandera para saltarse loops

bool isJoined = false;

Timer lastMesureTimer; // timer para medicion de energia
LowPowerTimer lastClockSyncTimer;

// banderas para uplink

bool pendingSyncClock = false;
bool pendingSendTimestamp = false;
bool pendingSendConfig = false;

bool pendingSaveConfig = false;


// [END] Luminary global variables

// [START] Luminary global functions

void saveSmartcellConfig(){
    logTrace("Saving configurations to NVM");
    uint8_t saveBuffer[NVM_SAVE_BUFFER_SIZE];

    // guardartime config
    saveBuffer[DIR_LOOP_DELAY-DIR_INITIAL_NVM_PARAM] = static_cast<uint8_t>((loopDelay & 0xFF00) >> 8);
    saveBuffer[DIR_LOOP_DELAY-DIR_INITIAL_NVM_PARAM+1] = static_cast<uint8_t>(loopDelay & 0x00FF);
    saveBuffer[DIR_LOOP_DELAY-DIR_INITIAL_NVM_PARAM+2] = loopsToSend;

    // guardar modo de operacion
    saveBuffer[DIR_OP_MODE-DIR_INITIAL_NVM_PARAM] = lightController.getMode();

    // guardar curva seleccionada
    saveBuffer[DIR_CURVE-DIR_INITIAL_NVM_PARAM] = dimmingCurves.getCurrentCurve(); 

    // guardar nivel de dimming manual
    saveBuffer[DIR_MANUAL_DIMMING-DIR_INITIAL_NVM_PARAM] = static_cast<uint8_t>(lightController.getManualDimLevel() * 100);

    // guardar horas de on y off
    saveBuffer[DIR_ON_OFF_TIME-DIR_INITIAL_NVM_PARAM+0] = onOffByTime.getOnHour();
    saveBuffer[DIR_ON_OFF_TIME-DIR_INITIAL_NVM_PARAM+1] = onOffByTime.getOnMinute();
    saveBuffer[DIR_ON_OFF_TIME-DIR_INITIAL_NVM_PARAM+2] = onOffByTime.getOffHour();
    saveBuffer[DIR_ON_OFF_TIME-DIR_INITIAL_NVM_PARAM+3] = onOffByTime.getOffMinute();
    dot->nvmWrite(DIR_INITIAL_NVM_PARAM, saveBuffer, NVM_SAVE_BUFFER_SIZE);

    // guardar custom curve
    dot->nvmWrite(DIR_CUSTOM_CURVE, customCurve, 12);

    pendingSaveConfig = false;
}

void payloadParser(uint8_t *RxBuffer, uint8_t RxBufferSize)
{
    logTrace("payloadParser");

    uint8_t saveBuffer[4] = {0, 0, 0, 0};

    // [START] Parseo de payload

    switch (RxBuffer[0])
    {
    case 'A': // setear hora manualmente
    {
        if (RxBufferSize < 5)
            break;
        uint32_t aux32 = 0;
        aux32 = static_cast<uint32_t>(RxBuffer[1] << 24);
        aux32 |= static_cast<uint32_t>(RxBuffer[2] << 16);
        aux32 |= static_cast<uint32_t>(RxBuffer[3] << 8);
        aux32 |= static_cast<uint32_t>(RxBuffer[4]);
        setManualTime(aux32);
        break;
    }

    case 'C': // Cambio de curva de dimming
    {
        if (RxBufferSize < 2)
            break;
        logInfo("Switching to Curve %u", RxBuffer[1]);
        dimmingCurves.selectCurve(RxBuffer[1]);
        lightController.setOpMode(LightController::OpMode::AutoCurve);
        pendingSaveConfig = true;
        break;
    }

    case 'D': // Set manual dimming
    {
        if (RxBufferSize < 2)
            break;
        logInfo("Switching to Manual %u%", RxBuffer[1]);
        lightController.setManualDimming(static_cast<float>(RxBuffer[1]) / 100);
        lightController.setOpMode(LightController::OpMode::Manual);
        pendingSaveConfig = true;
        break;
    }

    case 'E': // Enviar configuracion de light Contoller
    {
        pendingSendConfig = true;
        break;
    }

    case 'H': // Enviar timestamp
    {
        pendingSendTimestamp = true;
        break;
    }

    case 'J': // Configurar parte baja de customCurve
    {
        if (RxBufferSize < 7)
            break;
        logInfo("Setting low part of custom curve");
        memcpy(&customCurve[0], &RxBuffer[1], 6);
        pendingSaveConfig = true;
        break;
    }
    case 'K': // Configurar parte alta de customCurve
    {
        if (RxBufferSize < 7)
            break;
        logInfo("Setting high part of custom curve");
        memcpy(&customCurve[6], &RxBuffer[1], 6);
        pendingSaveConfig = true;
        break;
    }

    case 'M': // Set mode
    {
        if (RxBufferSize < 2)
            break;
        switch (RxBuffer[1])
        {
        case 0x00:
            logInfo("Switch to mode Manual");
            lightController.setOpMode(LightController::OpMode::Manual);
            break;
        case 0x01:
            logInfo("Switch to mode PhotoCell");
            lightController.setOpMode(LightController::OpMode::AutoPhotoCell);
            break;
        case 0x02:
            logInfo("Switch to mode dimming Curve");
            lightController.setOpMode(LightController::OpMode::AutoCurve);
            break;
        case 0x03:
            logInfo("Switch to mode Demo");
            lightController.setOpMode(LightController::OpMode::Demo);
            break;

        default:
            logError("Mode not found");
            break;
        }
        pendingSaveConfig = true;
        break;
    }

    case 'O': // Set On Off time
    {
        if(RxBufferSize < 5)
            break;
        logInfo("Setting On-Off time: %02u:%02u - %02u:%02u", RxBuffer[1], RxBuffer[2], RxBuffer[3], RxBuffer[4]);
        onOffByTime.setOnOffTime(RxBuffer[1], RxBuffer[2], RxBuffer[3], RxBuffer[4]);
        lightController.setOpMode(LightController::OpMode::AutoTime);
        pendingSaveConfig = true;
        break;

    }
    case 'R': // Software reset
    {
        NVIC_SystemReset();
        break;
    }

    case 'S': // Sincronizar hora
    {
        pendingSyncClock = true;
        break;
    }

    case 'T': // Cambiar loop delay
    {
        if (RxBufferSize < 4)
            break;
        uint16_t aux16 = static_cast<uint16_t>(RxBuffer[1]) << 8;
        aux16 += RxBuffer[2];
        if (aux16 < 30)
            aux16 = 30;
        logInfo("Changing loop delay to %u seconds", aux16);
        loopDelay = aux16;
        loopsToSend = RxBuffer[3];
        //save config
        pendingSaveConfig = true;
        break;
    }
    }

    // bypass loop delay
    bypassLoopDelay = true;

    // [STOP] Parseo de payload
}

int main()
{
    // Custom event handler for automatically displaying RX data
    EventSlot events(&payloadParser);

    pc.baud(9600);

    mts::MTSLog::setLogLevel(mts::MTSLog::INFO_LEVEL);


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
    logInfo("mDot library version: %s", dot->getId().c_str());
    logInfo("Promatix SmartCell version: %d.%d.%d", PROMATIX_VERSION_MAJOR, PROMATIX_VERSION_MINOR, PROMATIX_VERSION_PATCH);

    // start from a well-known state
    logInfo("defaulting Dot configuration");
    dot->resetConfig();
    dot->resetNetworkSession();

    // make sure library logging is turned on
    dot->setLogLevel(mts::MTSLog::INFO_LEVEL);

    // attach the custom events handler
    dot->setEvents(&events);

    #if LORA_JOIN_MODE == LORA_MODE_OTA

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

    #elif LORA_JOIN_MODE == LORA_MODE_ABP
    // Configurar join mode ABP

    isJoined = true;

    // config device address como parte baja del devEUI
    {
        std::vector<uint8_t> devEui = dot->getDeviceId();
        network_address[0] = devEui.at(4);
        network_address[1] = devEui.at(5);
        network_address[2] = devEui.at(6);
        network_address[3] = devEui.at(7);

    }
    
    // update configuration if necessary
    if (dot->getJoinMode() != mDot::MANUAL) {
        logInfo("changing network join mode to MANUAL");
        if (dot->setJoinMode(mDot::MANUAL) != mDot::MDOT_OK) {
            logError("failed to set network join mode to MANUAL");
        }
    }
    
    update_manual_config(network_address, network_session_key, data_session_key, frequency_sub_band, network_type, ack);

    #endif

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

    // setear la ganancia de la antena
    dot->setAntennaGain(0);

    // save changes to configuration
    logInfo("saving configuration");
    if (!dot->saveConfig())
    {
        logError("failed to save configuration");
    }

    // display configuration
    display_config();

    // [START] init Luminary
    logInfo("========================");
    logInfo("LUMINARY version");
    logInfo("========================");

    // encender led lora hasta que haya join
    ledStatus.setCicle(LED_SEQUENCE_OK);
    ledLora.setCicle(LED_SEQUENCE_ERROR_1);

    // read config
    uint8_t saveBuffer[4] = {0, 0, 0, 0};

    // leer version de firmware para la que se crearon los datos
    // y comparar con la version actual
    if (dot->nvmRead(DIR_PROMATIX_VERSION_MAJOR, saveBuffer, 3))
    {
        if ((saveBuffer[0] != PROMATIX_VERSION_MAJOR) || (saveBuffer[1] != PROMATIX_VERSION_MINOR) || (saveBuffer[2] != PROMATIX_VERSION_PATCH))
        {
            // la version no coincide, entonces cargar los datos por defecto
            logInfo("Loading default values to memory");

            // resetear el frame counter
            saveBuffer[0] = 0;
            saveBuffer[1] = 0;
            saveBuffer[2] = 0;
            saveBuffer[3] = 0;
            dot->nvmWrite(DIR_LAST_FRAME_COUNTER, saveBuffer, 4);

            saveBuffer[0] = PROMATIX_VERSION_MAJOR;
            saveBuffer[1] = PROMATIX_VERSION_MINOR;
            saveBuffer[2] = PROMATIX_VERSION_PATCH;
            dot->nvmWrite(DIR_PROMATIX_VERSION_MAJOR, saveBuffer, 3);

            saveBuffer[0] = DEFAULT_LOOP_DELAY_HIGH; // loop delay cada 10 minutos
            saveBuffer[1] = DEFAULT_LOOP_DELAY_LOW;
            saveBuffer[2] = DEFAULT_LOOPS_BETWEEN_TX; // 20 loops entre envios
            dot->nvmWrite(DIR_LOOP_DELAY, saveBuffer, 3);

            saveBuffer[0] = 0x01; // Modo de operacion Fotocelda
            dot->nvmWrite(DIR_OP_MODE, saveBuffer, 1);

            saveBuffer[0] = 0x00; // Curva seleccionada 1
            dot->nvmWrite(DIR_CURVE, saveBuffer, 1);

            saveBuffer[0] = 100; // Dimming manual 100%
            dot->nvmWrite(DIR_MANUAL_DIMMING, saveBuffer, 1);

            saveBuffer[0] = 18;  // Hora ON 18:00
            saveBuffer[1] = 0;
            saveBuffer[2] = 6;  // Hora OFF 06:00
            saveBuffer[3] = 0;
            dot->nvmWrite(DIR_ON_OFF_TIME, saveBuffer, 4);

            // En un bloque individual para liberar la memoria del buffer en cuanto se termine.
            {
                uint8_t saveBuffer12[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};    // curva custom
                dot->nvmWrite(DIR_CUSTOM_CURVE, saveBuffer12, 12);
            }
        }
    }

    // leer last frame counter
    if (dot->nvmRead(DIR_LAST_FRAME_COUNTER, saveBuffer, 4))
    {
        uint32_t frameCounter = saveBuffer[0] << 24;
        frameCounter |= saveBuffer[1] << 16;
        frameCounter |= saveBuffer[2] << 8;
        frameCounter |= saveBuffer[3];

        dot->setUpLinkCounter(frameCounter + ABP_FCOUNT_SAFE_GAP);
    }
    else
        logError("Failed to read saved uplink frame counter");

    // leer loop delay
    if (dot->nvmRead(DIR_LOOP_DELAY, saveBuffer, 3))
    {
        loopDelay = static_cast<uint16_t>(saveBuffer[0]) << 8;
        loopDelay += saveBuffer[1];
        loopsToSend = saveBuffer[2];
    }
    else
        logError("Failed to read saved loop delay");

    // leer nivel de dimming manual
    if (dot->nvmRead(DIR_MANUAL_DIMMING, saveBuffer, 1))
        lightController.setManualDimming(static_cast<float>(saveBuffer[0]) / 100);
    else
        logError("Failed to read saved manual dimming level");

    // leer opMode
    if (dot->nvmRead(DIR_OP_MODE, saveBuffer, 1))
    {
        // evitar configurar para curva antes de obtener la hora
        if ((static_cast<LightController::OpMode>(saveBuffer[0]) == LightController::OpMode::AutoCurve) ||
            (static_cast<LightController::OpMode>(saveBuffer[0]) == LightController::OpMode::AutoTime))
            lightController.setOpMode(LightController::OpMode::AutoPhotoCell);
        else
            lightController.setOpMode(static_cast<LightController::OpMode>(saveBuffer[0]));
    }
    else
        logError("Failed to read saved operation mode");

    // leer current curve
    if (dot->nvmRead(DIR_CURVE, saveBuffer, 1))
        dimmingCurves.selectCurve(saveBuffer[0]);
    else
        logError("Failed to read saved curve index");

    // leer custom curve
    if (!dot->nvmRead(DIR_CUSTOM_CURVE, customCurve, 12))
        logError("Failed to read saved custom curve");

    // leer on off time
    if (dot->nvmRead(DIR_ON_OFF_TIME, saveBuffer, 4))
        onOffByTime.setOnOffTime(saveBuffer[0], saveBuffer[1], saveBuffer[2], saveBuffer[3]);
    else
        logError("Failed to read On Off time");

    // Initial delay
    {
        int randomSeed = static_cast<int>(photoCell.read(1) * 1000) % 16;
        randomSeed = (randomSeed << 4) + static_cast<int>(photoCell.read(1) * 1000) % 15;
        randomSeed = (randomSeed << 4) + static_cast<int>(photoCell.read(1) * 1000) % 15;
        randomSeed = (randomSeed << 4) + static_cast<int>(photoCell.read(1) * 1000) % 15;
        srand(randomSeed);                      // Iniciamos la semilla de numeros aleatorios leyendo la luz
        wait_us((rand() % (100 + 1)) * 100000); // esperamos un tiempo aleatorio (entre 0 y 10s) antes de mandar el join
    }

    #if ENABLE_JOIN == 1
    // Intentamos Join y si es exitoso
    if(!isJoined){
        join_network(INITIAL_JOIN_ATEMPTS);
        isJoined = dot->getNetworkJoinStatus();
    }

    #endif

    // iniciar timer de medicion de energia
    lastMesureTimer.start();
    float energy = 0;

    // iniciar timer de sincronizacion de reloj
    lastClockSyncTimer.start();

    // Iniciar con un bucle de transmision
    loopsCount = loopsToSend;

    // [END] init Luminary

    while (true)
    {
        // [START] Luminary Loop
        // guardar datos en NVM si esta pendiente
        if(pendingSaveConfig){
            saveSmartcellConfig();
        }

        // leer y guardar el frame counter
        if (dot->nvmRead(DIR_LAST_FRAME_COUNTER, saveBuffer, 4))
        {
            uint32_t savedFCount = saveBuffer[0] << 24;
            savedFCount |= saveBuffer[1] << 16;
            savedFCount |= saveBuffer[2] << 8;
            savedFCount |= saveBuffer[3];

            uint32_t currentFCount = dot->getUpLinkCounter();

            if(currentFCount >= (savedFCount + ABP_FCOUNT_SAFE_GAP)){

                logInfo("Saving current uplink frame counter: %lu", currentFCount);
                saveBuffer[0] = currentFCount & 0xFF000000 >> 24;
                saveBuffer[1] = currentFCount & 0x00FF0000 >> 16;
                saveBuffer[2] = currentFCount & 0x0000FF00 >> 8;
                saveBuffer[3] = currentFCount & 0x000000FF;

                dot->nvmWrite(DIR_LAST_FRAME_COUNTER, saveBuffer, 4);
            }

        }

        logDebug("LoopsCount: %u", loopsCount);
        if(loopsCount >= loopsToSend){
            logDebug("Sending part 1");

            #if ENABLE_JOIN == 1
            // Intentamos Join.
            if (!isJoined)
            {
                join_network(LOOP_JOIN_ATEMPTS);
                isJoined = dot->getNetworkJoinStatus();
            }

            #endif
            // Show time
            printTime();

            #if ENABLE_JOIN == 1
            // Sincronizar hora si no esta sincronizado o cada 12 horas.
            // if ( pendingSyncClock || !timeIsSynced() || lastClockSyncTimer.read() > 43200)
            if (pendingSyncClock || !timeIsSynced() || lastClockSyncTimer.read() > 43200)
            {
                pendingSyncClock = false;
                logInfo("Attemting to sync clock");
                if (syncTime(5, TIME_ZONE))
                {
                    lastClockSyncTimer.reset();
                }
            }
            #endif

            // enviar timestamp si estaba pendiente
            if (pendingSendTimestamp)
            {
                pendingSendTimestamp = false;
                send_currentTime();
            }

            // si esta en hora, recuperar modo curvas si esta guardado
            if (timeIsSynced())
            {
                if (dot->nvmRead(DIR_OP_MODE, saveBuffer, 1))
                {
                    lightController.setOpMode(static_cast<LightController::OpMode>(saveBuffer[0]));
                }
            }

            // enviar configuracion si estaba pendiente
            if (pendingSendConfig)
            {
                pendingSendConfig = false;
                send_smartCellConfig(lightController.getMode(), static_cast<uint8_t>(lightController.getManualDimLevel() * 100), loopDelay, loopsToSend);
            }

        }else{
            logDebug("Bypassing Sending part 1");
        }

        

        // Energy calculation
        float power = currentSensor.getCurrent(5) * 220;
        float timeSinceLastMesure = lastMesureTimer.read();
        lastMesureTimer.reset();
        energy += (power * timeSinceLastMesure) / 3600;

        // Cambiar el nivel de dimming segun la hora
        float dimming = lightController.getDimming(getHour(), getMinute());
        lightOutput.setOutput(dimming);

        // forzar envio si cambio el nivel de dimming
        if((lastDimming != dimming) && !(loopsCount >= loopsToSend)){
            bypassLoopDelay = true;
        }
        lastDimming = dimming;
        

        // medicion de potencia
        wait_us(500000); // retardo de 500ms para que se estabilice la corriente antes de medirla
        power = currentSensor.getCurrent(5) * 220;

        // print config
        logInfo("========================");
        logInfo("SmartCell configuration");
        logInfo("========================");
        logInfo("Has Joined ====== %s", isJoined ? "true" : "false");
        lightController.printMode();
        logInfo("Dimming ========= %.0f %", dimming * 100);
        logInfo("Power =========== %.2fW", power);
        logInfo("Energy ========== %.2fkW", energy);
        logInfo("Last mesure ===== %.0f seconds ago", timeSinceLastMesure);
        logInfo("Period ========== %us", loopDelay);
        logInfo("Uplink in ======= %u loops", loopsToSend - loopsCount);
        logInfo("Up Frame Count == %lu", dot->getUpLinkCounter());

        // check if error
        if (dimming > 0)
        {
            if (power > 0)
                ledStatus.setCicle(LED_SEQUENCE_OK);
            else
                ledStatus.setCicle(LED_SEQUENCE_ERROR_1);
        }
        else if (power > 0)
            ledStatus.setCicle(LED_SEQUENCE_ERROR_2);

        // Send Light Status
        logDebug("LoopsCount: %u", loopsCount);
        if(loopsCount >= loopsToSend){
            logDebug("Sending part 2");
            if (isJoined)
            {
                if (send_lightStatus(dimming, power, energy, lightController.getMode()))
                {
                    // Quitar parte enviada de la energia.
                    energy -= trunc(energy);
                    ledLora.setCicle(LED_SEQUENCE_OK);
                }
                else
                {
                    ledLora.setCicle(LED_SEQUENCE_ERROR_2);
                }
            }
            else
            {
                ledLora.setCicle(LED_SEQUENCE_ERROR_1);
            }
        }else{
            logDebug("Bypassing Sending part 2");
        }

        // contar loops
        if(loopsCount >= loopsToSend){
            logDebug("Resetting loopsCount");
            loopsCount = 0;
        }
        logTrace("loopsCount %u -> %u", loopsCount, loopsCount+1);
        loopsCount++;

        // [END] Luminary Loop

        // the Dot can't sleep in class C mode
        // it must be waiting for data from the gateway

        uint16_t thisDelay = loopDelay /*+ (rand() % (20 + 1)) - 10*/;
        logInfo("waiting for %us\n\r", thisDelay);
        for (uint16_t i = 0; i < thisDelay; i++)
        {
            // Saltarse el retardo si se activa el bypass
            if (bypassLoopDelay)
            {
                logInfo("Bypassing loop delay");
                loopsCount = loopsToSend;
                bypassLoopDelay = false;
                break;
            }

            // Reducir el retardo si esta en modo demostracion
            if (lightController.getMode() == static_cast<uint8_t>(LightController::OpMode::Demo))
            {
                loopsCount = loopsToSend;
                if (i >= dimmingDemo.demoPeriod)
                {
                    break;
                }
            }
            wait_us(1000000);
        }
    }

    return 0;
}

#endif
