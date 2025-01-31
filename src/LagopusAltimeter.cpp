#include "LagopusAltimeter.h"

LagopusAltimeter::LagopusAltimeter(/* args */)
{
}

LagopusAltimeter::~LagopusAltimeter()
{
}

// ******ICP10111******

bool LagopusAltimeter::initAir()
{
  // begin、0の場合true,-1の場合falseを出力するようにライブラリを改変
  while(!icp.begin()){
    delay(100);
  }
  icp.setWorkPattern(icp.eNormal);
  return true;
}

bool LagopusAltimeter::initUltrasonic(Stream &stream)
{
  delay(100);
  ultrasonic.init(stream, PIN_RS485_CTRL);
  return true;
}

bool LagopusAltimeter::initControl(){
  pinMode(PIN_ADC0, INPUT);
  return true;
}

void LagopusAltimeter::updateAirSensor(unsigned long millsTime){
  alti.program_time = millsTime;
  // kusojissou
  ultrasonic.request(ULTRASONIC, REALTIME);
  delay(100);
  if (ultrasonic.read() != -1){
    alti.height_1 = ultrasonic.dist;
  }
  alti.temp = icp.getTemperature();
  // 温度を取得するときに大気圧も取得しているのでその値をそのまま使う
  alti.air_pressure_1 = icp.getAirPressure();

  // 磁気センサ、操舵のパルスの取得
  alti.ctlValue = pulseIn(PIN_ADC0, HIGH, 1500000);

}

void LagopusAltimeter::serialAltimeterOutput(){
  Serial.printf("height,%f,%ld, %ld\n", alti.air_pressure_1, alti.height_1, alti.ctlValue);
}
