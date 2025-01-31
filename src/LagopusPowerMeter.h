#include "utils/XIAOBLE.h"
#include "LsPinAssign.h"


class LagopusPowerMeter
{
private:
  XIAOBLE xiao;
public:
  LagopusPowerMeter();
  ~LagopusPowerMeter();
  typedef struct{
    const uint8_t HEADER = 0xF5;
    unsigned long program_time;
    uint8_t cadence = 0;
    uint8_t power = 0;
    float airSpeed = 0.0;
    uint8_t dummy = 0x00;
    uint8_t dummy2 = 0x00;
    uint8_t dummy3 = 0x00;
  }power;
  power _power;
  power* _powerptr = &_power;
  bool initPower();
  void updatePower(unsigned long millisTime);
  void serialPowerOutput();
  bool AirMeterIsReady();
  bool PowerMeterIsReady();
};
