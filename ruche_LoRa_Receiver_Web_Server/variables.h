//========================================================
//Controls supression of the MonPrintf function to serial
//========================================================
#define SERIAL_BAUD 115200  // vitesse moniteur serie
#define SerialMonitor   1
#define debug 1  // true for use Display log message if True
//byte error = 0;  // variable for error and I2C address

//===========================
//Restart ESP32 after a while
//===========================
unsigned long resetAfterMillis = 86400000 ;  // 24h
//unsigned long resetAfterMillis = 43200000; // 12h
//unsigned long resetAfterMillis = 60000 ;   // 1 minute
unsigned long lastResetWas;

//==================================
// structure de donnees d'une ruche
//==================================
struct ruche {
  String numRuche;  // numero de la ruche
  String poids;
  String tempeDs18b20;
  String vBat;
  String tempeBme280;
};

//==============================================
// structure de donnees d'un boitier de capteurs
//==============================================
/*
 LoRaMessage = String(counterID) +
                "/" + String(BoitierCapteur.tempeDS18B20) +
                "&" + String(BoitierCapteur.numBoitierCapteur) +
                "#" + String(BoitierCapteur.poids) +
                "{" + String(BoitierCapteur.vBat) +
                "}" + Capteur_bme280.tempe +
                "(" + Capteur_bme280.pression +
                ")" + Capteur_bme280.humi +
                "@" + BoitierCapteur.nameBoitierCapteur +
                "~" + String(hum_stat) +
                "^" + String(bar_for) +
                "!" + String(BoitierCapteur.interrupteur);
*/
struct boitierCapteur {
  String tempeDS18B20;
  String numero;          // du boitier de capteurs
  String poids;
  String vBat;
  String tempeBME280;
  String pressionBME280;
  String humiBME280;
  String comfortBME280;
  String forecastBME280;
  String nom;             
  String interrupteur;// du boitier de capteurs
};

//===============================================================
// brochage de l'emetteur/recepteur lora en fonction de la carte
// define the pins used by the LoRa transceiver module ttgo
//===============================================================
#define DIO0 26
#define RST 14
#define SS 18
#define SCK 5
#define MOSI 27
#define MISO 19

//433E6 for Asia
//866E6 for Europe
//915E6 for North America
//#define BAND 866E6
#define BAND 868300000

#define PABOOST true

// Change sync word (0xF3) to match the receiver
// The sync word assures you don't get LoRa messages from other LoRa transceivers
// ranges from 0-0xFF
//byte synchroLora = 0x14; // sx1276/rfm95 0x12 par defaut, 0x34 for public network
byte synchroLora = 0xF3;

//========
// Sondes
//========
#define ADDRESS 0x76                                  // premiere partie adresse ic2 bme280
// chaine (tableau de caracteres) pour l'envoi des données des sondes
const int taille = 30;                                // taille du tableau pour l'envoi des valeurs des sondes
char chaine[taille - 1] = "" ;                        // tableau pour l'envoi des valeurs des 
int hum_stat = 0;                                     // Humidity status: 0 - Normal, 1 - Comfort, 2 - Dry, 3 - Wet, de la sonde bme280
int bar_for = 0;                                      // Forecast: 0 - None, 1 - Sunny, 2 - PartlyCloudy, 3 - Cloudy, 4 - Rain, de la sonde bme280

//==========
// OLED pins
//==========
#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
int contrast = 18; // Where contrast is a value from 0 to 255 (sets contrast e.g. brightness)

//=====================
// ntp server for time
//=====================
#define SECRET_NTP_SERVER "pool.ntp.org" // serveur ntp time

//=================================
// Variables to save date and time
//=================================
int doSunrise  = 0 ; // dognon.sunrise(2022, 12, 25, false);
int doSunset   = 0 ; // dognon.sunset(2022, 12, 25, false);
const char* ntpServer = SECRET_NTP_SERVER;
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;
String TimeStamp;
String SunSet;
String SunRise;
String SolarNoon;

//===============================================
// Initialize variables to get and save LoRa data
//===============================================
int counter = 0;                 // compteur
int rssi;
int snr;
String loRaMessage;
String readingID;

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
// Configuration Blynk App
//char authentification[] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"; // le jeton d'authentification

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
