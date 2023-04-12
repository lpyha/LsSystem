#include <SPI.h>
#include <Wire.h>
#include <SD.h>
#include "HardwareSerial.h"

#include <ArduinoQueue.h>

// 自作クラス
#include "LagopusImu.h"
#include "LagopusGNSS.h"
#include "LagopusAir.h"
#include "LsPinAssign.h"
#include "LsStateFlow.h"

#define BAUD_RATE 115200
#define DATALOG_FILE "datalog1.bin"

#define QUEUE_SIZE_ITEMS 3

// Queue creation:
ArduinoQueue<byte*> dataQueue(QUEUE_SIZE_ITEMS);

// 自作クラス
LagopusImu LsImu;
LagopusGNSS LsGNSS;
LagopusAir LsAir;

// プログラム内の時間
unsigned long program_time = 0;

bool initSD()
{
  SPI.setRX(PIN_SD_MISO);
  SPI.setTX(PIN_SD_MOSI);
  SPI.setSCK(PIN_SD_SCK);
  SPI.setCS(PIN_SD_CS);
  return SD.begin(PIN_SD_CS);
}

void setup() 
{
  delay(5000);
  Serial.begin(BAUD_RATE);
  Serial.println("System checking...");
  Wire.setSCL(PIN_IMU_SCL);
  Wire.setSDA(PIN_IMU_SDA);
  Wire.begin();
  delay(500);
  Wire1.setSCL(PIN_GNSS_AIR_SCL);
  Wire1.setSDA(PIN_GNSS_AIR_SDA);
  Wire1.begin();
  delay(500);

  while(!LsImu.initImu())Serial.println("IMU...");
  Serial.println("Imu connected");

  while(!LsGNSS.initGNSS())Serial.println("GNSS...");
  Serial.println("GNSS connected");

  while(!LsAir.initAir())Serial.println("AirSensor...");
  Serial.println("AirSensor connected");

  while(!initSD())Serial.println("SD...");
  Serial.println("SD connected");

}

void setup1()
{
  /*
  すべてのセンサーの初期化が終わり次第loop1のロガー状態に入る
  ビットフラグを用いてセンサーの初期化を確認
  */
  delay(3000);
}

void loop()
{
  program_time = millis();
  LsImu.updateImu(program_time);
  byte* dataImu = (byte*)LsImu._quatptr;
  dataQueue.enqueue(dataImu);

  program_time = millis();
  LsGNSS.updateGNSS(program_time);
  byte* dataGnss = (byte*)LsGNSS._gnssptr;
  dataQueue.enqueue(dataGnss);
  
  program_time = millis();
  LsAir.updateAir(program_time);
  byte* dataAir = (byte*)LsAir._airptr;
  dataQueue.enqueue(dataAir);

  LsImu.serialOutput();
  LsGNSS.serialOutput();
  LsAir.serialOutput();

  delay(1000);
}

void loop1()
{
  File dataFile = SD.open(DATALOG_FILE, FILE_WRITE);
  if (dataFile && !dataQueue.isEmpty()) {
      int queueSize = dataQueue.itemCount();
      Serial.println(queueSize);
      for (int i = 0; i < queueSize; i++){
        size_t dataSize = sizeof(LsImu._quat);
        byte* val = dataQueue.dequeue();
        Serial.print("datasize>>");
        Serial.println(dataSize);
        dataFile.write(val, dataSize);
      }
      dataFile.close();
  }
  delay(1000);
}