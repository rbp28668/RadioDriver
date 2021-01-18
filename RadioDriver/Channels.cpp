#include <Arduino.h>
#include "Channel.h"

extern const Channel defaultChannels[] = {
  {"Emergency",121500},
  {"Safetycom", 135480},
  {"CGFF-1", 129980},
  {"CGFF-2", 118695},
  {"Sit Aware 1", 130105},
  {"Sit Aware 2", 130130},
  {"Cloud Flying", 130535},
  {"Comp 1", 129890},
  {"Comp 2", 130405},
  {0,0}
  
};
