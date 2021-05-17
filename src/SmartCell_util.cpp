// SmartCell_util.cpp

#include "SmartCell_util.h"

bool timeIsSynced()
{
  return (time(NULL) > 1514764800); // 1514764800 = UNIX timestamp para 1/1/2018 00:00:00
}

bool syncTime(const uint8_t attempts, const float timeZone)
{
  //! getGPSTime siempre el UTC sin importar la hora del gateway
  // set_time((dot->getGPSTime() / 1000) + 315964800)

  for (uint8_t i = 0; i < attempts; i++)
  {
    // realizar intenito.
    uint64_t recievedTimestamp = dot->getGPSTime();
    // verificar
    logDebug("Timestam received: %lu", recievedTimestamp);
    if (recievedTimestamp > 1198800018000) // 1198800018 = GPS timestamp para 1/1/2018 00:00:00
    {
      // Diferencia (en 2020) entre UNIX epoch con GPS epoch 315964782
      recievedTimestamp = (recievedTimestamp / 1000) + static_cast<uint64_t>(315964782 + (timeZone * 3600));
      set_time(recievedTimestamp);
      time_t currentTimestamp = time(NULL);
      logInfo("Clock Sync to %s", ctime(&currentTimestamp));
      return true;
    }
  }
  // reportar error si no sincronizo
  logError("Clock didn't sync");
  return false;
}

uint8_t getHour()
{
  // obtener la cantidad de horas
  return ((static_cast<uint32_t>(time(NULL)) / 3600) % 24);
}

uint8_t getMinute()
{
  // obtener la cantidad de minutos
  return ((static_cast<uint32_t>(time(NULL)) / 60) % 60);

}

void setManualTime(const uint32_t timestamp)
{
  set_time(timestamp);
}

void printTime()
{
  time_t currentTimestamp = time(NULL);
  logInfo("Current Time: %s", ctime(&currentTimestamp));
}

//------------------------------------------------------------------

bool send_lightStatus(const float dimming, const float power, const float energy, const uint8_t mode)
{
  // Solo ejecutar si se esta unido a la red lora
  if (dot->getNetworkJoinStatus())
  {
    // buffer a enviar
    std::vector<uint8_t> tx_data;

    // cargar el buffer con los datos
    tx_data.push_back('S');

    uint16_t aux16 = static_cast<uint16_t>(power);
    tx_data.push_back(static_cast<uint8_t>(aux16 >> 8));
    tx_data.push_back(static_cast<uint8_t>(aux16));

    tx_data.push_back(static_cast<uint8_t>(dimming * 100));

    aux16 = static_cast<uint16_t>(energy);
    tx_data.push_back(static_cast<uint8_t>(aux16 >> 8));
    tx_data.push_back(static_cast<uint8_t>(aux16));

    tx_data.push_back(mode);

    // enviar el buffer
    return (send_data(tx_data) == mDot::MDOT_OK);
  }
  return false;
}

bool send_smartCellConfig(const uint8_t lightMode, const uint8_t manualDimming, const uint16_t loopDelay, const uint8_t loopsToSend)
{
  // Solo ejecutar si se esta unido a la red lora
  if (dot->getNetworkJoinStatus())
  {
    // buffer a enviar
    std::vector<uint8_t> tx_data;

    // cargar el buffer con los datos
    tx_data.push_back('E');

    tx_data.push_back(lightMode);
    tx_data.push_back(manualDimming);
    tx_data.push_back(static_cast<uint8_t>((loopDelay & 0xFF00) >> 8));
    tx_data.push_back(static_cast<uint8_t>(loopDelay & 0x00FF));
    tx_data.push_back(loopsToSend);

    // enviar el buffer
    return (send_data(tx_data) == mDot::MDOT_OK);
  }
  return false;
}

bool send_currentTime()
{
  // Solo ejecutar si se esta unido a la red lora
  if (dot->getNetworkJoinStatus())
  {
    // obtener timestamp
    time_t currentTimestamp = time(NULL);

    logDebug("Timestamp to be send: %lu", currentTimestamp);

    // buffer a enviar
    std::vector<uint8_t> tx_data;

    // cargar datos en el buffer
    tx_data.push_back('H');

    tx_data.push_back(static_cast<uint8_t>((currentTimestamp & 0xFF000000) >> 24));
    tx_data.push_back(static_cast<uint8_t>((currentTimestamp & 0x00FF0000) >> 16));
    tx_data.push_back(static_cast<uint8_t>((currentTimestamp & 0x0000FF00) >> 8));
    tx_data.push_back(static_cast<uint8_t>(currentTimestamp & 0x000000FF));

    // enviar el buffer
    return (send_data(tx_data) == mDot::MDOT_OK);
  }
  return false;
}