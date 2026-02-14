#ifndef __HW_PLATFORM__
#define __HW_PLATFORM__

#include "config.h"
#include "devices/Led.h"
#include "devices/Sonar.h"


class HWPlatform {

public:
  HWPlatform();
  void init();
  void test();

  Led* getLedOK();
  Led* getLedALARM();
  Sonar* getSonar();

private:
  Led* LOK;
  Led* LALARM;
  Sonar* sonar;
};

#endif