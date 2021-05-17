// EventSlot.h

#ifndef _EVENT_SLOT_H_
#define _EVENT_SLOT_H_

#include "RadioEvent.h"

class EventSlot : public RadioEvent
{
public:
  EventSlot(void (*rxCallback_)(uint8_t *RxBuffer, uint8_t RxBufferSize));
  ~EventSlot() {}

  virtual void MacEvent(LoRaMacEventFlags *flags, LoRaMacEventInfo *info);

protected:
  void (*rxCallback)(uint8_t *RxBuffer, uint8_t RxBufferSize);
};

#endif