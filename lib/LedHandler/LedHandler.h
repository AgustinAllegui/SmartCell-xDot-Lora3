// LedHandler.h

#ifndef LED_HANDLER_H
#define LED_HANDLER_H

#include "mbed.h"

#include <vector>

class LedHandler
{
public:
    enum class LedMode : uint8_t
    {
        Manual = 0x00,
        Cicle = 0x01,
    };

    /**
     * @brief Construct a new Led Handler object
     * 
     * @param pin Pin al que esta conectado el Led
     * @param activeHigh_ La salida es activa en alto.
     */
    LedHandler(PinName pin, const bool activeHigh_ = true);

    bool isOn();
    LedMode getMode();

    // manejo manual

    /**
     * @brief Enciende o apaga el led manualmente.
     * 
     * @param on Estado del led.
     */
    void setManual(const bool on);

    /**
     * @brief Invierte el estado del led.
     */
    void toggle();

    // manejo de ciclo

    /**
     * @brief Configura ciclo automatico.
     * 
     * @param cicleVector_ Vector de tiempos, comenzando con tiempo de encendido.
     * @param oneTime True para que se ejecute una sola vez y pase a Manual off.
     */
    void setCicle(std::vector<float> cicleVector_, const bool oneTime_ = false);

private:
    DigitalOut led;
    LedMode mode;
    bool activeHigh;

    void setOn();
    void setOff();

    // manejo de ciclo
    std::vector<float> cicleVector;
    uint8_t cicleIndex;
    LowPowerTimeout cicleTimeout;
    void cicleCallback();
    bool oneTime;
};

#endif