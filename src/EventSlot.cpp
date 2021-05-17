// EventSlot.cpp

#include "EventSlot.h"

EventSlot::EventSlot(void (*rxCallback_)(uint8_t *RxBuffer, uint8_t RxBufferSize))
    : rxCallback(rxCallback_)
{
}

void EventSlot::MacEvent(LoRaMacEventFlags *flags, LoRaMacEventInfo *info)
{
  logTrace("EventSlot::MacEvent");
  RadioEvent::MacEvent(flags, info);

  if (info->RxBufferSize > 0)
  {
    rxCallback(info->RxBuffer, info->RxBufferSize);
  }
}