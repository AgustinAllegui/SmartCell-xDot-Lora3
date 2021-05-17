// PhotoCell.h

#ifndef _PHOTO_CELL_H_
#define _PHOTO_CELL_H_

#include "mbed.h"
#include "MTSLog.h"

//recomended Thresholds
// const float thr_l = 0.65;	// umbral de encendido
// const float thr_h = 0.7;	// umbral de apagado

class PhotoCell
{
public:
  PhotoCell(PinName pin_, const float onThreshold_, const float offThreshold_);

  bool shouldBeOn();
  float read(const uint8_t ammount = 10);
  float getLastRead();

  void setThreshold(const float onThreshold_, const float offThreshold_);

protected:
  AnalogIn pin;
  float onThreshold;
  float offThreshold;

  bool lastState;
  float lastRead;
};

#endif
