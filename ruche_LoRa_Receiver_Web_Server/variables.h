//========================================================
//Controls supression of the MonPrintf function to serial
//========================================================
#define SERIAL_BAUD 115200   // vitesse moniteur serie
#define SerialMonitor   1
#define debug 1  // true for use Display log message if True

//===========================
//Restart ESP32 after a while
//===========================
unsigned long resetAfterMillis = 86400000 ;  // 24h
//unsigned long resetAfterMillis = 43200000; // 12h
//unsigned long resetAfterMillis = 60000 ;   // 1 minute
unsigned long lastResetWas;


//==========================
//Enable Blynk or Thingspeak
//==========================
// configuration control constant for use of either Blynk or Thingspeak
// const String App = "BLYNK";    //  alternative is line below
const String App = "Thingspeak";  //  alternative is line above

//=================
//Blynk connection
//=================
//char auth[] = "password";
//const char* serverBlynk = "api.blynk.com";

//=====================
// ntp server for time
//=====================
#define SECRET_NTP_SERVER "pool.ntp.org" // serveur ntp time

//====================
//Battery calibration
//====================
//measured battery voltage/ADC reading
//#define batteryCalFactor .001167
#define batteryCalFactor .0010158
