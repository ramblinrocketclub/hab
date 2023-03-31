#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>
#include <DHT20.h>

DHT20 DHT;

bool debug = true;

static const int GPSrx = 4, GPStx = 3;
static const uint32_t GPSBaud = 9600;
static const int TempPin = 2;

const int SDcs = 5;

TinyGPSPlus gps;
SoftwareSerial ss(GPSrx, GPStx);

double lat;
double lng;
double alt;
TinyGPSTime time;
int status;
double temp;
double hum;
int valid;

void setup() {
  Serial.begin(115200);
  ss.begin(GPSBaud);

  DHT.begin();
  delay(1000);

  if (!SD.begin(SDcs)) {
    Serial.println("Card failed, or not present");
    while (1);
  }
  Serial.println("SD card initialized.");

  File log = SD.open("datalog.txt", FILE_WRITE);

  log.println("time,valid,lat,lng,alt,temp,hum");
  log.close();
}

void loop() {
  logGPSInfo();
  if (debug) printDebug();
  smartDelay(1000);
}

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

static void logGPSInfo() {
  
  File log = SD.open("datalog.txt", FILE_WRITE);

  lat = gps.location.lat();
  lng = gps.location.lng();
  alt = gps.altitude.meters();
  time = gps.time;
  status = DHT.read();
  temp = DHT.getTemperature();
  hum = DHT.getHumidity();



  if (gps.location.isValid()) {
    valid = 1;
  } else {
    valid = 0;
  }

  log.print(time.value());
  log.print(",");
  log.print(valid);
  log.print(",");
  log.print(lat, 4);
  log.print(",");
  log.print(lng, 4);
  log.print(",");
  log.print(alt, 4);
  log.print(",");
  log.print(temp, 1);
  log.print(",");
  log.print(hum, 1);
  log.println();
  log.close();
}

static void printDebug() {
  String formattedTime = formatTime(time);

  Serial.print(formattedTime);
  Serial.print(",");
  Serial.print(valid);
  Serial.print(",");
  Serial.print(lat, 4);
  Serial.print(",");
  Serial.print(lng, 4);
  Serial.print(",");
  Serial.print(alt, 4);
  Serial.print(",");
  Serial.print(temp, 1);
  Serial.print(",");
  Serial.print(hum, 1);
  Serial.println();
}

static String formatTime(TinyGPSTime &t) {
  char sz[32];
  sprintf(sz, "%02d:%02d:%02d", t.hour(), t.minute(), t.second());
  return sz;
}


