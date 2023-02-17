#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>

bool debug = true;

static const int GPSrx = 4, GPStx = 3;
static const uint32_t GPSBaud = 9600;
static const int TempPin = 2;

const int SDcs = 5;

TinyGPSPlus gps;
SoftwareSerial ss(GPSrx, GPStx);

double temp;

void setup() {
  Serial.begin(115200);
  ss.begin(GPSBaud);

  if (!SD.begin(SDcs)) {
    Serial.println("Card failed, or not present");
    while (1);
  }
  Serial.println("SD card initialized.");

  SD.open("datalog.txt", FILE_WRITE);
  log.println("Note: All times are UTC.");
  log.println("--TIME--|--LAT--|--LONG--|--ALT--");
  log.close();


}

void loop() {
  if (debug) printDebug();
  logGPSInfo();
  logTemperature();
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

static void floatToStr(String & str, float val, char width, unsigned char precision)
{
  char buffer[10];
  dtostrf(val, width, precision, buffer);
  str += buffer;
}

static void logGPSInfo() {
  log.open("datalog.txt", FILE_WRITE);

  if (gps.location.isValid()) {
    double lat = gps.location.lat();
    double lng = gps.location.lng();
    double alt = gps.altitude.meters();
    TinyGPSTime time = gps.time;

    String data = formatTime(time) + ",";
    floatToStr(data, lat, 7, 4);
    data += ",";
    floatToStr(data, lng, 8, 4);
    data += ",";
    floatToStr(data, alt, 8, 2);
    log.print(",");
    log.close();

  } else {
    log.print("Invalid Location");
  }
  log.close();
}

static void printDebug() {
  if (gps.location.isValid()) {

    double lat = gps.location.lat();
    double lng = gps.location.lng();
    double alt = gps.altitude.meters();
    TinyGPSTime time = gps.time;

    Serial.print(formatTime(time));
    Serial.print(",");
    Serial.print(lat, 4);
    Serial.print(",");
    Serial.print(lng, 4);
    Serial.print(",");
    Serial.print(alt, 4);
    Serial.print(",");
    Serial.print(temp, 4);
    Serial.println();

  } else {
    Serial.println("Location Invalid");
  }
}

static String formatTime(TinyGPSTime &t) {
  char sz[32];
  sprintf(sz, "%02d:%02d:%02d", t.hour(), t.minute(), t.second());
  return sz;
}

static void logTemperature() {
  SD.open("datalog.txt", FILE_WRITE);

  digitalWrite(TempPin, HIGH);
  int reading = analogRead(A0);

  double celc = ((4.9 * reading) - 425) / 6.25;
  temp = (celc * 9) / 5 + 32;

  SD.open("datalog.txt", FILE_WRITE);
  log.print(temp, 4);
  log.println();

  log.close();
}


