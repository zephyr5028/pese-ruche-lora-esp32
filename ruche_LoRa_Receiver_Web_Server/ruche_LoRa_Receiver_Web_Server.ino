/********************************************************************************************
   Reception de balances connectées avec ESP32 lora et serveur web
   utilisation d'un tpl5110, batterie nimh et panneau solaire 5w 12v pour une autonomie complete


   jlm le 21 septembre 2022   -  suivi par GIT

   https://RandomNerdTutorials.com/esp32-lora-sensor-web-server/
   https://randomnerdtutorials.com/esp32-deep-sleep-arduino-ide-wake-up-sources/
   https://randomnerdtutorials.com/esp32-lora-rfm95-transceiver-arduino-ide/
   https://github.com/nopnop2002/Arduino-LoRa-Ra01S
   https://docs.ai-thinker.com/_media/lora/docs/ra-01sh_specification.pdf
   https://github.com/tinytronix/SX126x
   https://www.semtech.com/products/wireless-rf/lora-connect/sx1262
   https://github.com/sandeepmistry/arduino-LoRa/blob/master/API.md
   https://lora.readthedocs.io/en/latest/

   carte esp32 par expressifs systems version 2.0.17
********************************************************************************************/

//=============================================================================================
// inclusion des bibliothèques
//=============================================================================================
// Wi-Fi library
#include <WiFi.h>
#include <WiFiMulti.h>
#include "ESPmDNS.h"
#include "ESPAsyncWebServer.h"
#include "ThingSpeak.h"
#include "secrets.h"  // fichier codes
#include "variables.h"

// memoire flash esp32
#include "SPIFFS.h"

//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>

//Libraries for OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#if MQTT
#include <PubSubClient.h>
#include <ArduinoJson.h>
#endif

//===============================
// objet des donnees d'une ruche
//===============================
// objet ruche
ruche Ruche = { .numRuche = "", .poids = "", .tempeDs18b20 = "", .vBat = "", .tempeBme280 = "" };  // definition de la ruche

//============================================
// objet des donnees d'un boitier de capteurs
//============================================
// objet boitier
boitierCapteur BoitierCapteurs = {
  .tempeDS18B20 = "",
  .numero = "",
  .poids = "",
  .vBat = "",
  .tempeBME280 = "",
  .pressionBME280 = "",
  .humiBME280 = "",
  .comfortBME280 = "",
  .forecastBME280 = "",
  .nom = "",
  .interrupteur = ""
};

// conversion de string to int : toInt()  string to float :  toFloat()

//======================
// Librarie to get time
//======================
#include "time.h"
bool Daylight_isdst;  // heure ete
//Minimal Arduino library for sunrise and sunset time
#include <Dusk2Dawn.h>
/*  Multiple instances can be created. Arguments are longitude, latitude, and
      time zone offset in hours from UTC.

      The first two must be in decimal degrees (DD), e.g. 10.001, versus the
      more common degrees, minutes, and seconds format (DMS), e.g. 10° 00′ 3.6″.
      The time zone offset can be expressed in float in the few cases where the
      the zones are offset by 30 or 45 minutes, e.g. "5.75" for Nepal Standard
      Time.

      HINT: An easy way to find the longitude and latitude for any location is
      to find the spot in Google Maps, right click the place on the map, and
      select "What's here?". At the bottom, you’ll see a card with the
      coordinates.
*/
Dusk2Dawn dognon(SECRET_LATITUDE, SECRET_LONGITUDE, 1);  // 1 pour +1 utc !!!
/*  Available methods are sunrise() and sunset(). Arguments are year, month,
    day, and if Daylight (heure d'ete) Saving Time is in effect.
*/

//============
// objet OLED
//============
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

//============
// Wifi Multi
//============
WiFiMulti wifiMulti;  // declaration d'un objet multi wifi

// WiFi connect timeout per AP. Increase when connecting takes longer.
const uint32_t connectTimeoutMs = 10000;

String adresse_ip;  // the IP address of your shield

WiFiClient wifiClient;  // objet wifi

//=========================================
// Create AsyncWebServer object on port 80
//=========================================
AsyncWebServer server(httpPort);

//=========
// page web
//=========
// dans le fichier data/index.html

//=====================================
// Replaces placeholder with DHT values
//=====================================
String processor(const String& var) {
  //Serial.println(var);
  if (var == "TEMPERATURE") {
    return Ruche.tempeDs18b20;
  } else if (var == "READINGID") {
    return readingID;
  } else if (var == "NUMRUCHE") {
    return Ruche.numRuche;
  } else if (var == "POIDS") {
    return Ruche.poids;
  } else if (var == "VBAT") {
    return Ruche.vBat;
  } else if (var == "TIMESTAMP") {
    return TimeStamp;
  } else if (var == "RSSI") {
    return String(rssi);
  } else if (var == "SNR") {
    return String(snr);
  } else if (var == "SUNSET") {
    return SunSet;
  } else if (var == "SUNRISE") {
    return SunRise;
  } else if (var == "SOLARNOON") {
    return SolarNoon;
  }
  return String();
}

//=========================
// Initialize OLED display
//=========================
void startOLED() {
  //reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);

  //initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) {  // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("PROJET LORA RECEIVER");
  display.ssd1306_command(SSD1306_SETCONTRAST);
  display.ssd1306_command(contrast);  // Where contrast is a value from 0 to 255 (sets contrast e.g. brightness)
  display.display();
}

//========================
// Initialize LoRa module
//========================
void startLoRA() {
  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);

  //while (!LoRa.begin(BAND) && counter < 10) {
  while (!LoRa.begin(BAND) && counter < 10) {
    Serial.print(".");
    counter++;
    delay(500);
  }
  if (counter == 10) {
    // Increment readingID on every new reading
    Serial.println("Starting LoRa failed!");
  } else {
    //LoRa.enableCrc(); //N'oubliez pas que chaque fois que je démarre la radio, je dois donner une activation dans le CRC
    //LoRa.setSyncWord(0x97);
    //Facteur d’étalement (SF) : De 6 à 12, plus la valeur est grande, plus la portée est grande
    LoRa.setSpreadingFactor(10);  // Setup Spreading Factor (6 ~ 12)
    // Setup BandWidth, option: 7800,10400,15600,20800,31250,41700,62500,125000,250000,500000
    LoRa.setSignalBandwidth(125000);  // Setup BandWidth, option: 7800,10400,15600,20800,31250,41700,62500,125000,250000,500000
    //LoRa.setCodingRate4(8);                           // Setup Coding Rate:5(4/5),6(4/6),7(4/7),8(4/8)
    //LoRa.dumpRegisters(Serial);
    //LoRa.setTxPower(20,RF_PACONFIG_PASELECT_PABOOST); //20dB output must via PABOOST

    /*
      LoRa.setTxPower(txPower,RFOUT_pin);
      txPower -- 0 ~ 20
      RFOUT_pin could be RF_PACONFIG_PASELECT_PABOOST or RF_PACONFIG_PASELECT_RFO
        - RF_PACONFIG_PASELECT_PABOOST -- LoRa single output via PABOOST, maximum output 20dBm
        - RF_PACONFIG_PASELECT_RFO     -- LoRa single output via RFO_HF / RFO_LF, maximum output 14dBm
    */

    Serial.println("LoRa Initialization OK!");
    display.setCursor(0, 10);
    display.clearDisplay();
    display.print("LoRa Initializing OK!");
    display.display();
  }
  // Change sync word (0xF3) to match the receiver
  // The sync word assures you don't get LoRa messages from other LoRa transceivers
  // ranges from 0-0xFF
  //LoRa.setSyncWord(0xF3);
  LoRa.setSyncWord(synchroLora);
  delay(300);
}

//======
// mqtt
//======
#if MQTT
PubSubClient clientMqtt(wifiClient);
int WLcountMQTT = 0;
//const int taille = 30;                     // taille du tableau pour l'envoi des valeurs des sondes
//char chaine[taille - 1] = "" ;             // tableau pour l'envoi des valeurs des sondes
#endif

//===============================================
// Read LoRa packet and get the sensor readings
//===============================================
void getLoRaData() {
  Serial.print("Lora packet received: ");
  // Read packet
  while (LoRa.available()) {
    String LoRaData = LoRa.readString();
    // LoRaData format: readingID/temperature&soilMoisture#batterylevel
    // String example:  3/21.50&2#-4.27{5.54
    // Ruche.numRuche = ""; Ruche.poids = ""; Ruche.tempeDs18b20 = ""; Ruche.vBat = "";

    Serial.print(LoRaData);
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
    /*
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
    */

    // Get readingID, temperature and soil moisture
    int pos1 = LoRaData.indexOf('/');
    int pos2 = LoRaData.indexOf('&');
    int pos3 = LoRaData.indexOf('#');
    int pos4 = LoRaData.indexOf('{');
    int pos5 = LoRaData.indexOf('}');
    int pos6 = LoRaData.indexOf('(');
    int pos7 = LoRaData.indexOf(')');
    int pos8 = LoRaData.indexOf('@');
    int pos9 = LoRaData.indexOf('~');
    int posA = LoRaData.indexOf('^');
    int posB = LoRaData.indexOf('!');

    // 1/21.25&101#0.00{5.58}23.09(1014)65@chassis~1^2
    // 9/22.50&5#0.03{5.45}21.84(1014)69@ruche_05~1^2!1

    readingID = LoRaData.substring(0, pos1);
    BoitierCapteurs.tempeDS18B20 = LoRaData.substring(pos1 + 1, pos2);
    Ruche.tempeDs18b20 = BoitierCapteurs.tempeDS18B20;
    BoitierCapteurs.numero = LoRaData.substring(pos2 + 1, pos3);
    Ruche.numRuche = BoitierCapteurs.numero;
    BoitierCapteurs.poids = LoRaData.substring(pos3 + 1, pos4);
    Ruche.poids = BoitierCapteurs.poids;
    BoitierCapteurs.vBat = LoRaData.substring(pos4 + 1, pos5);
    Ruche.vBat = BoitierCapteurs.vBat;
    BoitierCapteurs.tempeBME280 = LoRaData.substring(pos5 + 1, pos6);
    BoitierCapteurs.pressionBME280 = LoRaData.substring(pos6 + 1, pos7);
    BoitierCapteurs.humiBME280 = LoRaData.substring(pos7 + 1, pos8);
    BoitierCapteurs.nom = LoRaData.substring(pos8 + 1, pos9);
    BoitierCapteurs.comfortBME280 = LoRaData.substring(pos9 + 1, posA);
    BoitierCapteurs.forecastBME280 = LoRaData.substring(posA + 1, posB);
    BoitierCapteurs.interrupteur = LoRaData.substring(posB + 1, LoRaData.length());

    hum_stat = BoitierCapteurs.comfortBME280.toInt();  // pour l'envoi mqtt domoticz
    bar_for = BoitierCapteurs.forecastBME280.toInt();  // pour l'envoi mqtt domoticz

    //Forecast: 0 None, 1 Sunny, 2 PartlyCloudy, 3 Cloudy, 4 Rain
    if (bar_for == 4) {  // rain
      BoitierCapteurs.forecastBME280 = "rain";
    } else if (bar_for == 3) {  // cloudy
      BoitierCapteurs.forecastBME280 = "cloudy";
    } else if (bar_for == 2) {  // Partly Cloudy
      BoitierCapteurs.forecastBME280 = "partly cloudy";
    } else if (bar_for == 1) {  // wet / sunny
      BoitierCapteurs.forecastBME280 = "wet sunny";
    } else {  // none  bar_for = 0
      BoitierCapteurs.forecastBME280 = "none";
    }

    // Humidity status: 0 Normal, 1 Comfort, 2 Dry, 3 Wet
    if (hum_stat == 2) {  // dry
      BoitierCapteurs.comfortBME280 = "dry";
    } else if (hum_stat == 0) {  // normal
      BoitierCapteurs.comfortBME280 = "normal";
    } else if (hum_stat == 1) {  // confort
      BoitierCapteurs.comfortBME280 = "comfort";
    } else if (hum_stat = 3) {  // humide
      BoitierCapteurs.comfortBME280 = "wet";
    }

    Serial.println("");
    Serial.print("numero du boitier de capteurs : ");
    Serial.println(BoitierCapteurs.numero);

    /*
      // Get readingID, temperature and soil moisture
      int pos1 = LoRaData.indexOf('/');
      int pos2 = LoRaData.indexOf('&');
      int pos3 = LoRaData.indexOf('#');
      int pos4 = LoRaData.indexOf('{');
      int pos5 = LoRaData.indexOf('}');
      readingID = LoRaData.substring(0, pos1);
      Ruche.tempeDs18b20 = LoRaData.substring(pos1 + 1, pos2);
      Ruche.numRuche = LoRaData.substring(pos2 + 1, pos3);
      Ruche.poids = LoRaData.substring(pos3 + 1, pos4);
      Ruche.vBat = LoRaData.substring(pos4 + 1, pos5);
      Ruche.tempeBme280 = LoRaData.substring(pos5 + 1, LoRaData.length());
    */

    //Serial.println("");
    Serial.print("numero de la ruche : ");
    Serial.println(Ruche.numRuche);
  }

  // Get RSSI
  rssi = LoRa.packetRssi();
  snr = LoRa.packetSnr();
  Serial.print("with RSSI ");
  Serial.print(rssi);
  Serial.print(" and SNR ");
  Serial.println(snr);

  adresse_ip = WiFi.localIP().toString();

  display.clearDisplay();
  display.setCursor(0, 10);
  display.print("IP Adr: ");
  display.setCursor(40, 10);
  //display.print(WiFi.localIP());
  display.print(adresse_ip);

  display.setCursor(0, 20);
  display.print("Ruche: ");
  display.setCursor(42, 20);
  display.print(Ruche.numRuche);

  /*
    display.setCursor(0, 20);
    display.print("Boitier: ");
    display.setCursor(48, 20);
    display.print(BoitierCapteurs.numero);
  */

  display.setCursor(68, 20);
  display.print("RSSI: ");
  display.setCursor(98, 20);
  display.print(rssi);

  display.setCursor(0, 30);
  display.print("Poids:");
  display.setCursor(42, 30);
  display.print(Ruche.poids);
  display.setCursor(88, 30);
  display.print("kg");

  display.setCursor(0, 40);
  display.print("Temperature: ");
  display.setCursor(76, 40);
  display.print(Ruche.tempeDs18b20);
  //display.print(BoitierCapteurs.tempeDS18B20);
  display.setCursor(112, 40);
  display.print("C");

  display.setCursor(0, 50);
  display.print("num ID: ");
  display.setCursor(50, 50);
  display.print(readingID);

  display.setCursor(70, 50);
  display.print("SNR: ");
  display.setCursor(100, 50);
  display.print(snr);

  display.display();
}

//======
// time
//======
/*
   struct tm {
   int tm_sec;         // seconds,  range 0 to 59
   int tm_min;         // minutes, range 0 to 59
   int tm_hour;        // hours, range 0 to 23
   int tm_mday;        // day of the month, range 1 to 31
   int tm_mon;         // month, range 0 to 11
   int tm_year;        // The number of years since 1900
   int tm_wday;        // day of the week, range 0 to 6
   int tm_yday;        // day in the year, range 0 to 365
   int tm_isdst;       // daylight saving time
*/
void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }

  /*
      time_t now = time(nullptr);
      str_time = String(ctime(&now));
      str_time.trim();

      struct tm *timeinfo;
      time(&now);
      timeinfo = localtime(&now);
      int current_minute = timeinfo->tm_hour * 60 + timeinfo->tm_min;

      Dusk2Dawn home(LOCATION_LAT, LOCATION_LON, 0);

      int sunrise_minute = home.sunrise(timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_isdst);
      int sunset_minute = home.sunset(timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_isdst);

      is_day = false;
      if (current_minute >= sunrise_minute && current_minute <= sunset_minute)
      {
        is_day = true;
      }
  */

  /*
    Serial.println(TimeStamp);
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    Serial.println(&timeinfo, "%Y-%m-%d %H:%M:%S");
    Serial.println("Time variables");
    char timeHour[3];
    strftime(timeHour,3, "%H", &timeinfo);
    Serial.println(timeHour);
    char timeWeekDay[10];
    strftime(timeWeekDay,10, "%A", &timeinfo);
    Serial.println(timeWeekDay);
    Serial.println();

    byte Krish_day;
    byte Krish_month;
    int Krish_year;
    //Serial.println (&timeinfo, "%m %d %Y / %H:%M:%S");
    //scanf(&timeinfo, "%m %d %Y / %H:%M:%S")
    Krish_hour = timeinfo.tm_hour;
    Krish_min = timeinfo.tm_min;
    Krish_sec = timeinfo.tm_sec;

    Krish_day = timeinfo.tm_mday;
    Krish_month = timeinfo.tm_mon + 1;
    Krish_year = timeinfo.tm_year +1900;
  */
  Daylight_isdst = timeinfo.tm_isdst;
  Serial.print("daylight : ");     // heure ete
  Serial.println(Daylight_isdst);  // heure ete
  Serial.print("year : ");
  Serial.println(timeinfo.tm_year + 1900);
  Serial.print("date / heure : ");
  Serial.println(&timeinfo, "%m %d %Y / %H:%M:%S");

  doSunrise = dognon.sunrise(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, Daylight_isdst);
  doSunset = dognon.sunset(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, Daylight_isdst);

  char timeStringBuff[50];  //50 chars should be enough
  strftime(timeStringBuff, sizeof(timeStringBuff), "%A %d/%m/%Y %H:%M:%S", &timeinfo);
  //print like "const char*"
  //Serial.println(timeStringBuff);

  //Optional: Construct String object
  TimeStamp = String(timeStringBuff);
  Serial.println(TimeStamp);
}

//====================
// sunset and sunrise
//====================
void SunsetSunrise() {
  /*  Time is returned in minutes elapsed since midnight. If no sunrises or
      sunsets are expected, a "-1" is returned.
  */
  Serial.print("Sunrise en minutes: ");
  Serial.println(doSunrise);  //
  Serial.print("Sunset en minutes: ");
  Serial.println(doSunset);  //

  /*  A static method converts the returned time to a 24-hour clock format.
      Arguments are a character array and time in minutes.
  */
  char time[6];
  Dusk2Dawn::min2str(time, doSunrise);
  SunRise = String(time);
  Serial.print("Sunrise : ");
  Serial.println(SunRise);


  /*  Alternatively, the array could be initialized with a dummy. This may be
      easier to remember.
  */
  char time2[] = "00:00";
  Dusk2Dawn::min2str(time2, doSunset);
  SunSet = String(time2);
  Serial.print("Sunset : ");
  Serial.println(SunSet);

  /*  Do some calculations with the minutes, then convert to time.
  */
  int doSolarNoon = doSunrise + (doSunset - doSunrise) / 2;
  char time3[] = "00:00";
  Dusk2Dawn::min2str(time3, doSolarNoon);
  SolarNoon = String(time3);
  Serial.print("SolarNoon : ");
  Serial.println(SolarNoon);


  /*  In case of an error, an error message is given. The static method also
      returns a false boolean value for error handling purposes.
  */
  char time4[] = "00:00";
  bool response = Dusk2Dawn::min2str(time4, doSunrise);
  if (response == false) {
    Serial.println(time4);  // "ERROR"
    Serial.println("Uh oh!");
  }
}

//===================================================
// chaine pour l'envoi des valeurs de la sonde BME280
//===================================================
void chaine_bme280() {
  // Convert the value to a char array
  // float myFloat = myString.toFloat();

  char valeur_temp[6] = "";  // temperature
  float tempe = BoitierCapteurs.tempeBME280.toFloat();
  dtostrf(tempe, 2, 2, valeur_temp);
  strcat(chaine, valeur_temp);
  strcat(chaine, ";");

  char valeur_hum[5] = "";  // humidite
  float humi = BoitierCapteurs.humiBME280.toFloat();
  dtostrf(humi, 2, 2, valeur_hum);
  strcat(chaine, valeur_hum);
  strcat(chaine, ";");

  char valeur_hum_stat[1] = "";  // humidite confort
  //hum_stat = BoitierCapteurs.comfortBME280.toInt();
  dtostrf(hum_stat, 1, 0, valeur_hum_stat);
  strcat(chaine, valeur_hum_stat);
  strcat(chaine, ";");

  char valeur_pre[6] = "";  // pression barometrique
  float pre = BoitierCapteurs.pressionBME280.toFloat();
  dtostrf(pre, 4, 0, valeur_pre);
  strcat(chaine, valeur_pre);
  strcat(chaine, ";");

  char valeur_bar_for[1] = "";  // prevision meteo avec le barometre
  //bar_for = BoitierCapteurs.forecastBME280.toInt();
  dtostrf(bar_for, 1, 0, valeur_bar_for);
  strcat(chaine, valeur_bar_for);

  if (debug) {
    Serial.print("Chaine Sonde bme280 : ");
    Serial.println(chaine);  // pour l'envoi mqtt domoticz
  }
}

//====================
// thinkspeak
//====================
void thinkspeak() {
  // set the fields with the values
  if (SECRET_THINGSPEAK_TRUE and SECRET_WIFI_TRUE) {
    int num_ruche = Ruche.numRuche.toInt();  // string to int

    if (JLM and num_ruche >= 1 and num_ruche <= 4) {                  // 4 pour l'instant
      field_thinkspeak("1", "2", "3", "4", "5", "6", "7", "8", "9");  // numeros ruhes de 1 a 9
      write_thingspeak(myChannelNumberJlm, myWriteAPIKeyJlm);

    } else if (LOIC and num_ruche >= 11 and num_ruche <= 19) {
      field_thinkspeak("11", "12", "13", "14", "15", "16", "17", "18", "19");  // Numeros ruches de 11 a 19
      write_thingspeak(myChannelNumberLoic, myWriteAPIKeyLoic);
    }
  }
}

/*
   https://github.com/mathworks/thingspeak-arduino
   Return Codes
   Value   Meaning
   200   OK / Success
   404   Incorrect API key (or invalid ThingSpeak server address)
   -101  Value is out of range or string is too long (> 255 characters)
   -201  Invalid field number specified
   -210  setField() was not called before writeFields()
   -301  Failed to connect to ThingSpeak
   -302  Unexpected failure during write to ThingSpeak
   -303  Unable to parse response
   -304  Timeout waiting for server to respond
   -401  Point was not inserted (most probable cause is the rate limit of once every 15 seconds)
   0   Other error
*/

// correlation numero ruche et field
void field_thinkspeak(String r1, String r2, String r3, String r4, String r5, String r6, String r7, String r8, String r9) {
#if JLM
  if (Ruche.numRuche == r1) {
    ThingSpeak.setField(1, Ruche.poids);         // poids de la ruche
    ThingSpeak.setField(2, Ruche.tempeDs18b20);  // temperature de la ruche
    //ThingSpeak.setField(7, Ruche.vBat);   // tension de la batterie
  } else if (Ruche.numRuche == r2) {
    ThingSpeak.setField(3, Ruche.poids);         // poids de la ruche
    ThingSpeak.setField(4, Ruche.tempeDs18b20);  // temperature de la ruche
    //ThingSpeak.setField(8, Ruche.vBat);   // tension de la batterie
  } else if (Ruche.numRuche == r3) {
    ThingSpeak.setField(5, Ruche.poids);         // poids de la ruche
    ThingSpeak.setField(6, Ruche.tempeDs18b20);  // temperature de la ruche
  } else if (Ruche.numRuche == r4) {
    ThingSpeak.setField(7, Ruche.poids);         // poids de la ruche
    ThingSpeak.setField(8, Ruche.tempeDs18b20);  // temperature de la ruche
  } else if (Ruche.numRuche == r5) {

  } else if (Ruche.numRuche == r6) {

  } else if (Ruche.numRuche == r7) {

  } else if (Ruche.numRuche == r8) {

  } else if (Ruche.numRuche == r9) {
  }
#endif

#if LOIC
  if (Ruche.numRuche == r1) {
    ThingSpeak.setField(1, Ruche.poids);  // poids de la ruche
  } else if (Ruche.numRuche == r2) {
    ThingSpeak.setField(2, Ruche.poids);  // poids de la ruche
  } else if (Ruche.numRuche == r3) {
    ThingSpeak.setField(3, Ruche.poids);  // poids de la ruche
  } else if (Ruche.numRuche == r4) {
    ThingSpeak.setField(4, Ruche.poids);  // poids de la ruche
  } else if (Ruche.numRuche == r5) {
    ThingSpeak.setField(5, Ruche.poids);  // poids de la ruche
  } else if (Ruche.numRuche == r6) {
    ThingSpeak.setField(6, Ruche.poids);  // poids de la ruche
  } else if (Ruche.numRuche == r7) {
    ThingSpeak.setField(7, Ruche.poids);  // poids de la ruche
  } else if (Ruche.numRuche == r8) {
    ThingSpeak.setField(8, Ruche.poids);  // poids de la ruche
  } else if (Ruche.numRuche == r9) {
    ThingSpeak.setField(9, Ruche.poids);  // poids de la ruche
  }
#endif
}

// envoi des donnees vers thingspeak
void write_thingspeak(unsigned long myChannelNumber, const char* myWriteAPIKey) {
  // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
  // pieces of information in a channel.  Here, we write to field 1.
  // int x = ThingSpeak.writeField(myChannelNumber, 1, ruche1.poids, myWriteAPIKey);
  // avec plusieurs informations
  Serial.print("thingspeak id : ");
  Serial.println(myChannelNumber);
  Serial.print("api key write : ");
  Serial.println(myWriteAPIKey);
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (x == 200) {
    Serial.println("Channel update successful.");
  } else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
    Serial.println("");
  }
}

//====================
// page web not found
//====================
void notFound(AsyncWebServerRequest* request) {
  request->send(404, "text/plain", "Not found");
}

//=======
// setup:
//=======
void setup() {
  // Initialize Serial Monitor
  Serial.begin(SERIAL_BAUD);

  lastResetWas = millis();  // pour reset esp32 intervals reguliers

  /*
  startOLED();
  delay(2000);
  startLoRA();
  delay(3000);
  */
  Serial.println("");
  Serial.println("Demarrage OK");
  delay(300);
  startOLED();
  delay(1000);
  startLoRA();
  Serial.print("Synchro lora : ");
  Serial.println(synchroLora);
  delay(1000);

  // connection wifi
  if (SECRET_WIFI_TRUE) {
    setup_wifi();
    // utilisation de mDNS pour ne plus utiliser l'adresse ip changeante a cause du multiwifi
    if (MDNS.begin(NAME_RESEAU)) {
      MDNS.addService("http", "tcp", 80);
      Serial.print("mDNS advertising started : ");
      Serial.println(NAME_RESEAU);
    } else {
      Serial.println("Error starting mDNS");
    }
  }

  if (SECRET_THINGSPEAK_TRUE and SECRET_WIFI_TRUE) {
    ThingSpeak.begin(wifiClient);  // Initialize ThingSpeak
  }

  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  /*
    // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->printf("<li>URL: %s</li>", request->url().c_str());
    String inputMessage;
    inputMessage= request->contentType();
    //DEBUG_PRINT("Page ");DEBUG_PRINTLN(inputMessage);
    request->send_P(200, "text/html", index_html, processor);
    });
  */

  server.on("/poids", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/plain", Ruche.poids.c_str());
  });
  server.on("/readingid", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/plain", readingID.c_str());
  });
  server.on("/sunset", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/plain", SunSet.c_str());
  });
  server.on("/sunrise", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/plain", SunRise.c_str());
  });
  server.on("/solarnoon", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/plain", SolarNoon.c_str());
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/plain", Ruche.tempeDs18b20.c_str());
  });
  server.on("/vbat", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/plain", Ruche.vBat.c_str());
  });
  server.on("/numruche", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/plain", Ruche.numRuche.c_str());
  });
  server.on("/timestamp", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/plain", TimeStamp.c_str());
  });
  server.on("/rssi", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/plain", String(rssi).c_str());
  });
  server.on("/snr", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/plain", String(snr).c_str());
  });

  server.on("/winter", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SPIFFS, "/winter.jpg", "image/jpg");
  });

  /*
    server.on("/poids", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", BoitierCapteurs.poids.c_str());
    });
    server.on("/nom", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", BoitierCapteurs.nom.c_str());
    });
    server.on("/readingid", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", readingID.c_str());
    });
    server.on("/sunset", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", SunSet.c_str());
    });
    server.on("/sunrise", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", SunRise.c_str());
    });
    server.on("/solarnoon", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", SolarNoon.c_str());
    });
    server.on("/DS18B20temperature", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", BoitierCapteurs.tempeDS18B20.c_str());
    });
    server.on("/BME280temperature", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", BoitierCapteurs.tempeBME280.c_str());
    });
    server.on("/BME280humidity", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", BoitierCapteurs.humiBME280.c_str());
    });
    server.on("/BME280pression", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", BoitierCapteurs.pressionBME280.c_str());
    });
    server.on("/BME280comfort", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", BoitierCapteurs.comfortBME280.c_str());
    });
    server.on("/BME280forecast", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", BoitierCapteurs.forecastBME280.c_str());
    });
    server.on("/interrupteur", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", BoitierCapteurs.interrupteur.c_str());
    });
    server.on("/vbat", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", BoitierCapteurs.vBat.c_str());
    });
    server.on("/numero", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", BoitierCapteurs.numero.c_str());
    });
    server.on("/timestamp", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", TimeStamp.c_str());
    });
    server.on("/rssi", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", String(rssi).c_str());
    });
    server.on("/snr", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", String(snr).c_str());
    });

    server.on("/domotique", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/domotique.jpg", "image/jpg");
    });
  */

  // Init and get the time
  //configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // apperler configTzTime() dans setup
  //La config correspond aux changements d'heure du fuseau horaire Europe / Paris.
  configTzTime("CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00", ntpServer);

  //Voici l'appel au Royaume-Uni que je fais
  //configTime(0 * 3600, 0, "2.pool.ntp.org", "time.nist.gov");
  //setenv("TZ", "GMT+0BST-1,M3.5.0/01:00:00,M10.5.0/02:00:00", 1);

  printLocalTime();
  //Sunset and sunrise
  SunsetSunrise();

  display.clearDisplay();
#if MQTT
  clientMqtt.setServer(mqttServer, mqttPort);  // On défini la connexion MQTT
#endif

  // Start server
  if (SECRET_WIFI_TRUE) {
    server.onNotFound(notFound);
    server.begin();
  }
}

//=======
// loop:
//=======
void loop() {
  // Check if there are LoRa packets available
  int packetSize = 0;
  packetSize = LoRa.parsePacket();
  if (packetSize) {
    getLoRaData();
    // verifie la connection wifi
    if (SECRET_WIFI_TRUE) {
      connect_wifi();
    }
    printLocalTime();
    SunsetSunrise();

    //send data to MQTT
#if MQTT
    // connection mqtt
    if (WiFi.status() == WL_CONNECTED) {
      if (!clientMqtt.connected()) {
        Serial.println("MQTT déconnecté, on reconnecte !");
        reconnect();
      }
      if (clientMqtt.connected()) {
        Serial.println("MQTT connecte");
        clientMqtt.loop();
        // SendDataMQTT(&environment);

#if JLM
        if (Ruche.numRuche == "1") {
          // envoi du poids
          nomModule = "Ruche1_poids";
          // convert string to float
          float poids = atof(Ruche.poids.c_str());
          // pour eliminer les valeurs sans decimales
          if (poids > (int)poids) {
            SendData(idxDeviceRuche1Poids, nomModule, Ruche.poids.c_str(), 0);  // Envoi des donnees via JSON et MQTT
            essaimage(poids);                                                   // verification si essaimage
          }
          // envoi de la temperature
          nomModule = "Ruche1_temperature";
          SendData(idxDeviceRuche1Temperature, nomModule, Ruche.tempeDs18b20.c_str(), 0);  // Envoi des données via JSON et MQTT
          // envoi de la tension de la baterrie
          nomModule = "Ruche1_batterie_tension";
          // convert string to float
          float bat = atof(Ruche.vBat.c_str());
          // pour eliminer les valeurs sans decimales
          if (bat > (int)bat) {
            SendData(idxDeviceRuches1TensionBatterie, nomModule, Ruche.vBat.c_str(), 0);  // Envoi des données via JSON et MQTT
          }

        } else if (Ruche.numRuche == "2") {
          // envoi du poids
          nomModule = "Ruche2_poids";
          // convert string to float
          float poids = atof(Ruche.poids.c_str());
          // pour eliminer les valeurs sans decimales
          if (poids > (int)poids) {
            SendData(idxDeviceRuche2Poids, nomModule, Ruche.poids.c_str(), 0);  // Envoi des donnees via JSON et MQTT
            essaimage(poids);
          }
          // envoi de la temperature
          nomModule = "Ruche2_temperature";
          SendData(idxDeviceRuche2Temperature, nomModule, Ruche.tempeDs18b20.c_str(), 0);  // Envoi des données via JSON et MQTT
          // envoi de la tension de la baterrie
          nomModule = "Ruche2_batterie_tension";
          // convert string to float
          float bat = atof(Ruche.vBat.c_str());
          // pour eliminer les valeurs sans decimales
          if (bat > (int)bat) {
            SendData(idxDeviceRuches2TensionBatterie, nomModule, Ruche.vBat.c_str(), 0);  // Envoi des données via JSON et MQTT
          }

        } else if (Ruche.numRuche == "3") {
          // envoi du poids
          nomModule = "Ruche3_poids";
          // convert string to float
          float poids = atof(Ruche.poids.c_str());
          // pour eliminer les valeurs sans decimales
          if (poids > (int)poids) {
            SendData(idxDeviceRuche3Poids, nomModule, Ruche.poids.c_str(), 0);  // Envoi des donnees via JSON et MQTT
            essaimage(poids);
          }
          // envoi de la temperature
          nomModule = "Ruche3_temperature";
          SendData(idxDeviceRuche3Temperature, nomModule, Ruche.tempeDs18b20.c_str(), 0);  // Envoi des données via JSON et MQTT
          // envoi de la tension de la batterie
          nomModule = "Ruche3_batterie_tension";
          // convert string to float
          float bat = atof(Ruche.vBat.c_str());
          // pour eliminer les valeurs sans decimales
          if (bat > (int)bat) {
            SendData(idxDeviceRuches3TensionBatterie, nomModule, Ruche.vBat.c_str(), 0);  // Envoi des données via JSON et MQTT
          }

        } else if (Ruche.numRuche == "4") {
          // envoi du poids
          nomModule = "Ruche4_poids";
          // convert string to float
          float poids = atof(Ruche.poids.c_str());
          // pour eliminer les valeurs sans decimales
          if (poids > (int)poids) {
            SendData(idxDeviceRuche4Poids, nomModule, Ruche.poids.c_str(), 0);  // Envoi des donnees via JSON et MQTT
            essaimage(poids);
          }
          // envoi de la temperature
          nomModule = "Ruche4_temperature";
          SendData(idxDeviceRuche4Temperature, nomModule, Ruche.tempeDs18b20.c_str(), 0);  // Envoi des données via JSON et MQTT
          // envoi de la tension de la batterie
          nomModule = "Ruche4_batterie_tension";
          // convert string to float
          float bat = atof(Ruche.vBat.c_str());
          // pour eliminer les valeurs sans decimales
          if (bat > (int)bat) {
            SendData(idxDeviceRuches4TensionBatterie, nomModule, Ruche.vBat.c_str(), 0);  // Envoi des données via JSON et MQTT
          }

        } else if (Ruche.numRuche == "7") {
          // envoi du poids
          nomModule = "Ruche7_poids";
          // convert string to float
          float poids = atof(Ruche.poids.c_str());
          // pour eliminer les valeurs sans decimales
          if (poids > (int)poids) {
            SendData(idxDeviceRuche7Poids, nomModule, Ruche.poids.c_str(), 0);  // Envoi des donnees via JSON et MQTT
            essaimage(poids);
          }
          // envoi de la temperature
          nomModule = "Ruche7_temperature";
          SendData(idxDeviceRuche7Temperature, nomModule, Ruche.tempeDs18b20.c_str(), 0);  // Envoi des données via JSON et MQTT
          // envoi de la tension de la batterie
          nomModule = "Ruche7_batterie_tension";
          // convert string to float
          float bat = atof(Ruche.vBat.c_str());
          // pour eliminer les valeurs sans decimales
          if (bat > (int)bat) {
            SendData(idxDeviceRuches7TensionBatterie, nomModule, Ruche.vBat.c_str(), 0);  // Envoi des données via JSON et MQTT
          }

        } else if (BoitierCapteurs.numero == "101") {
          // envoi de la temperature du ds18b20
          nomModule = "BoitierCapteurs_101_temperature_ds18b20";
          SendData(idxDeviceCapteurChassisTemperatureDS18B20, nomModule, BoitierCapteurs.tempeDS18B20.c_str(), 0);  // Envoi des données via JSON et MQTT
          // envoi de la tension de la baterrie
          nomModule = "BoitierCapteurs_101_tension_batterie";
          SendData(idxDeviceCapteurChassisTensionBatterie, nomModule, BoitierCapteurs.vBat.c_str(), 0);  // Envoi des données via JSON et MQTT
          // creation d'une chaine avec les donnees de la sonde bme280.
          chaine[0] = '\0';  // effacement du tableau
          chaine_bme280();   //creation de la chaine a envoyer pour la sonde du bme280
          // Envoi de la données via JSON et MQTT
          nomModule = "BoitierCapteurs_101_BME280";
          SendData(idxDeviceCapteurChassisBME280, nomModule, chaine, 0);  // Envoi des données via JSON et MQTT
          // envoi de la position de l'interrupteur
          nomModule = "BoitierCapteurs_101_Switch";
          // en atendant la correction sur le boitier capteur, il manque la fin du message lora !!!
          //SendData(idxDeviceCapteurChassisSwitch, nomModule, BoitierCapteurs.interrupteur.c_str(), 0);  // Envoi des données via JSON et MQTT

        } else if (BoitierCapteurs.numero == "5") {
          // envoi du poids
          nomModule = "Capteur_Ruche5_poids";
          // convert string to float
          float poids = atof(BoitierCapteurs.poids.c_str());
          // pour eliminer les valeurs sans decimales
          if (poids > (int)poids) {
            SendData(idxDeviceCapteurRuche5Poids, nomModule, BoitierCapteurs.poids.c_str(), 0);  // Envoi des donnees via JSON et MQTT
            essaimage(poids);
          }
          // envoi de la temperature du ds18b20
          nomModule = "Capteur_Ruche5_temperature_ds18b20";
          SendData(idxDeviceCapteurRuche5TemperatureDS18B20, nomModule, BoitierCapteurs.tempeDS18B20.c_str(), 0);  // Envoi des données via JSON et MQTT
          // envoi de la tension de la batterie
          nomModule = "Capteur_Ruche5_tension_batterie";
          // convert string to float
          float bat = atof(BoitierCapteurs.vBat.c_str());
          // pour eliminer les valeurs sans decimales
          if (bat > (int)bat) {
            SendData(idxDeviceCapteurRuches5TensionBatterie, nomModule, BoitierCapteurs.vBat.c_str(), 0);  // Envoi des données via JSON et MQTT
          }
          // creation d'une chaine avec les donnees de la sonde bme280.
          chaine[0] = '\0';  // effacement du tableau
          chaine_bme280();   //creation de la chaine a envoyer pour la sonde du bme280
          // Envoi de la données via JSON et MQTT
          nomModule = "Capteurs_Ruche5_BME280";
          SendData(idxDeviceCapteurRuche5BME280, nomModule, chaine, 0);  // Envoi des données via JSON et MQTT
          // envoi de la position de l'interrupteur
          nomModule = "Capteur_Ruche5_Switch";
          if (BoitierCapteurs.interrupteur == "0") {                                                     // interrupteur ouvert nvalue a 0 ouvert
            SendData(idxDeviceCapteurRuche5Switch, nomModule, BoitierCapteurs.interrupteur.c_str(), 0);  // Envoi des données via JSON et MQTT
          } else {                                                                                       // interrupteur ferme nvalue a 1 ferme
            SendData(idxDeviceCapteurRuche5Switch, nomModule, BoitierCapteurs.interrupteur.c_str(), 1);  // Envoi des données via JSON et MQTT
          }

        } else if (BoitierCapteurs.numero == "6") {
          // envoi du poids
          nomModule = "Capteur_Ruche6_poids";
          // convert string to float
          float poids = atof(BoitierCapteurs.poids.c_str());
          // pour eliminer les valeurs sans decimales
          if (poids > (int)poids) {
            SendData(idxDeviceCapteurRuche6Poids, nomModule, BoitierCapteurs.poids.c_str(), 0);  // Envoi des donnees via JSON et MQTT
            essaimage(poids);
          }
          // envoi de la temperature du ds18b20
          nomModule = "Capteur_Ruche6_temperature_ds18b20";
          SendData(idxDeviceCapteurRuche6TemperatureDS18B20, nomModule, BoitierCapteurs.tempeDS18B20.c_str(), 0);  // Envoi des données via JSON et MQTT
          // envoi de la tension de la batterie
          nomModule = "Capteur_Ruche6_tension_batterie";
          // convert string to float
          float bat = atof(BoitierCapteurs.vBat.c_str());
          // pour eliminer les valeurs sans decimales
          if (bat > (int)bat) {
            SendData(idxDeviceCapteurRuches6TensionBatterie, nomModule, BoitierCapteurs.vBat.c_str(), 0);  // Envoi des données via JSON et MQTT
          }
          // creation d'une chaine avec les donnees de la sonde bme280.
          chaine[0] = '\0';  // effacement du tableau
          chaine_bme280();   //creation de la chaine a envoyer pour la sonde du bme280
          // Envoi de la données via JSON et MQTT
          nomModule = "Capteurs_Ruche6_BME280";
          SendData(idxDeviceCapteurRuche6BME280, nomModule, chaine, 0);  // Envoi des données via JSON et MQTT
          // envoi de la position de l'interrupteur
          nomModule = "Capteur_Ruche6_Switch";
          if (BoitierCapteurs.interrupteur == "0") {                                                     // interrupteur ouvert nvalue a 0 ouvert
            SendData(idxDeviceCapteurRuche6Switch, nomModule, BoitierCapteurs.interrupteur.c_str(), 0);  // Envoi des données via JSON et MQTT
          } else {                                                                                       // interrupteur ferme nvalue a 1 ferme
            SendData(idxDeviceCapteurRuche6Switch, nomModule, BoitierCapteurs.interrupteur.c_str(), 1);  // Envoi des données via JSON et MQTT
          }

        } else if (BoitierCapteurs.numero == "8") {
          // envoi du poids
          nomModule = "Capteur_Ruche8_poids";
          // convert string to float
          float poids = atof(BoitierCapteurs.poids.c_str());
          // pour eliminer les valeurs sans decimales
          if (poids > (int)poids) {
            SendData(idxDeviceCapteurRuche8Poids, nomModule, BoitierCapteurs.poids.c_str(), 0);  // Envoi des donnees via JSON et MQTT
            essaimage(poids);
          }
          // envoi de la temperature du ds18b20
          nomModule = "Capteur_Ruche8_temperature_ds18b20";
          SendData(idxDeviceCapteurRuche8TemperatureDS18B20, nomModule, BoitierCapteurs.tempeDS18B20.c_str(), 0);  // Envoi des données via JSON et MQTT
          // envoi de la tension de la batterie
          nomModule = "Capteur_Ruche8_tension_batterie";
          // convert string to float
          float bat = atof(BoitierCapteurs.vBat.c_str());
          // pour eliminer les valeurs sans decimales
          if (bat > (int)bat) {
            SendData(idxDeviceCapteurRuches8TensionBatterie, nomModule, BoitierCapteurs.vBat.c_str(), 0);  // Envoi des données via JSON et MQTT
          }
          // creation d'une chaine avec les donnees de la sonde bme280.
          chaine[0] = '\0';  // effacement du tableau
          chaine_bme280();   //creation de la chaine a envoyer pour la sonde du bme280
          // Envoi de la données via JSON et MQTT
          nomModule = "Capteurs_Ruche8_BME280";
          SendData(idxDeviceCapteurRuche8BME280, nomModule, chaine, 0);  // Envoi des données via JSON et MQTT
          // envoi de la position de l'interrupteur
          nomModule = "Capteur_Ruche8_Switch";
          if (BoitierCapteurs.interrupteur == "0") {                                                     // interrupteur ouvert nvalue a 0 ouvert
            SendData(idxDeviceCapteurRuche8Switch, nomModule, BoitierCapteurs.interrupteur.c_str(), 0);  // Envoi des données via JSON et MQTT
          } else {                                                                                       // interrupteur ferme nvalue a 1 ferme
            SendData(idxDeviceCapteurRuche8Switch, nomModule, BoitierCapteurs.interrupteur.c_str(), 1);  // Envoi des données via JSON et MQTT
          }
        }

#elif LOIC
        // ruches de 11 a 19
        if (Ruche.numRuche == "11") {
          // envoi du poids
          nomModule = "Ruche11_poids";
          // convert string to float
          float poids = atof(Ruche.poids.c_str());
          // pour eliminer les valeurs sans decimales
          if (poids > (int)poids) {
            SendData(idxDeviceRuche11Poids, nomModule, Ruche.poids.c_str(), 0);  // Envoi des donnees via JSON et MQTT
            essaimage(poids);
          }
          // envoi de la temperature
          nomModule = "Ruche11_temperature";
          SendData(idxDeviceRuche11Temperature, nomModule, Ruche.tempeDs18b20.c_str(), 0);  // Envoi des données via JSON et MQTT
          // envoi de la tension de la baterrie
          nomModule = "Ruche11_batterie_tension";
          // convert string to float
          float bat = atof(Ruche.vBat.c_str());
          // pour eliminer les valeurs sans decimales
          if (bat > (int)bat) {
            SendData(idxDeviceRuches11TensionBatterie, nomModule, Ruche.vBat.c_str(), 0);  // Envoi des données via JSON et MQTT
          }
        } else if (Ruche.numRuche == "12") {
          // envoi du poids
          nomModule = "Ruche12_poids";
          // convert string to float
          float poids = atof(Ruche.poids.c_str());
          // pour eliminer les valeurs sans decimales
          if (poids > (int)poids) {
            SendData(idxDeviceRuche12Poids, nomModule, Ruche.poids.c_str(), 0);  // Envoi des donnees via JSON et MQTT
            essaimage(poids);
          }
          // envoi de la temperature
          nomModule = "Ruche12_temperature";
          SendData(idxDeviceRuche12Temperature, nomModule, Ruche.tempeDs18b20.c_str(), 0);  // Envoi des données via JSON et MQTT
          // envoi de la tension de la baterrie
          nomModule = "Ruche12_batterie_tension";
          // convert string to float
          float bat = atof(Ruche.vBat.c_str());
          // pour eliminer les valeurs sans decimales
          if (bat > (int)bat) {
            SendData(idxDeviceRuches12TensionBatterie, nomModule, Ruche.vBat.c_str(), 0);  // Envoi des données via JSON et MQTT
          }
        } else if (Ruche.numRuche == "13") {
          // envoi du poids
          nomModule = "Ruche13_poids";
          // convert string to float
          float poids = atof(Ruche.poids.c_str());
          // pour eliminer les valeurs sans decimales
          if (poids > (int)poids) {
            SendData(idxDeviceRuche13Poids, nomModule, Ruche.poids.c_str(), 0);  // Envoi des donnees via JSON et MQTT
            essaimage(poids);
          }
          // envoi de la temperature
          nomModule = "Ruche13_temperature";
          SendData(idxDeviceRuche13Temperature, nomModule, Ruche.tempeDs18b20.c_str(), 0);  // Envoi des données via JSON et MQTT
          // envoi de la tension de la baterrie
          nomModule = "Ruche13_batterie_tension";
          // convert string to float
          float bat = atof(Ruche.vBat.c_str());
          // pour eliminer les valeurs sans decimales
          if (bat > (int)bat) {
            SendData(idxDeviceRuches13TensionBatterie, nomModule, Ruche.vBat.c_str(), 0);  // Envoi des données via JSON et MQTT
          }
        } else if (Ruche.numRuche == "14") {
          // envoi du poids
          nomModule = "Ruche14_poids";
          // convert string to float
          float poids = atof(Ruche.poids.c_str());
          // pour eliminer les valeurs sans decimales
          if (poids > (int)poids) {
            SendData(idxDeviceRuche14Poids, nomModule, Ruche.poids.c_str(), 0);  // Envoi des donnees via JSON et MQTT
            essaimage(poids);
          }
          // envoi de la temperature
          nomModule = "Ruche14_temperature";
          SendData(idxDeviceRuche14Temperature, nomModule, Ruche.tempeDs18b20.c_str(), 0);  // Envoi des données via JSON et MQTT
          // envoi de la tension de la baterrie
          nomModule = "Ruche14_batterie_tension";
          // convert string to float
          float bat = atof(Ruche.vBat.c_str());
          // pour eliminer les valeurs sans decimales
          if (bat > (int)bat) {
            SendData(idxDeviceRuches14TensionBatterie, nomModule, Ruche.vBat.c_str(), 0);  // Envoi des données via JSON et MQTT
          }
        } else if (Ruche.numRuche == "15") {
          // envoi du poids
          nomModule = "Ruche15_poids";
          // convert string to float
          float poids = atof(Ruche.poids.c_str());
          // pour eliminer les valeurs sans decimales
          if (poids > (int)poids) {
            SendData(idxDeviceRuche15Poids, nomModule, Ruche.poids.c_str(), 0);  // Envoi des donnees via JSON et MQTT
            essaimage(poids);
          }
          // envoi de la temperature
          nomModule = "Ruche15_temperature";
          SendData(idxDeviceRuche15Temperature, nomModule, Ruche.tempeDs18b20.c_str(), 0);  // Envoi des données via JSON et MQTT
          // envoi de la tension de la baterrie
          nomModule = "Ruche15_batterie_tension";
          // convert string to float
          float bat = atof(Ruche.vBat.c_str());
          // pour eliminer les valeurs sans decimales
          if (bat > (int)bat) {
            SendData(idxDeviceRuches15TensionBatterie, nomModule, Ruche.vBat.c_str(), 0);  // Envoi des données via JSON et MQTT
          }
        } else if (Ruche.numRuche == "16") {
          // envoi du poids
          nomModule = "Ruche16_poids";
          // convert string to float
          float poids = atof(Ruche.poids.c_str());
          // pour eliminer les valeurs sans decimales
          if (poids > (int)poids) {
            SendData(idxDeviceRuche16Poids, nomModule, Ruche.poids.c_str(), 0);  // Envoi des donnees via JSON et MQTT
            essaimage(poids);
          }
          // envoi de la temperature
          nomModule = "Ruche16_temperature";
          SendData(idxDeviceRuche16Temperature, nomModule, Ruche.tempeDs18b20.c_str(), 0);  // Envoi des données via JSON et MQTT
          // envoi de la tension de la baterrie
          nomModule = "Ruche16_batterie_tension";
          // convert string to float
          float bat = atof(Ruche.vBat.c_str());
          // pour eliminer les valeurs sans decimales
          if (bat > (int)bat) {
            SendData(idxDeviceRuches16TensionBatterie, nomModule, Ruche.vBat.c_str(), 0);  // Envoi des données via JSON et MQTT
          }
        } else if (Ruche.numRuche == "17") {
          // envoi du poids
          nomModule = "Ruche17_poids";
          // convert string to float
          float poids = atof(Ruche.poids.c_str());
          // pour eliminer les valeurs sans decimales
          if (poids > (int)poids) {
            SendData(idxDeviceRuche17Poids, nomModule, Ruche.poids.c_str(), 0);  // Envoi des donnees via JSON et MQTT
            essaimage(poids);
          }
          // envoi de la temperature
          nomModule = "Ruche17_temperature";
          SendData(idxDeviceRuche17Temperature, nomModule, Ruche.tempeDs18b20.c_str(), 0);  // Envoi des données via JSON et MQTT
          // envoi de la tension de la baterrie
          nomModule = "Ruche17_batterie_tension";
          // convert string to float
          float bat = atof(Ruche.vBat.c_str());
          // pour eliminer les valeurs sans decimales
          if (bat > (int)bat) {
            SendData(idxDeviceRuches17TensionBatterie, nomModule, Ruche.vBat.c_str(), 0);  // Envoi des données via JSON et MQTT
          }
        } else if (Ruche.numRuche == "18") {
          // envoi du poids
          nomModule = "Ruche18_poids";
          // convert string to float
          float poids = atof(Ruche.poids.c_str());
          // pour eliminer les valeurs sans decimales
          if (poids > (int)poids) {
            SendData(idxDeviceRuche18Poids, nomModule, Ruche.poids.c_str(), 0);  // Envoi des donnees via JSON et MQTT
            essaimage(poids);
          }
          // envoi de la temperature
          nomModule = "Ruche18_temperature";
          SendData(idxDeviceRuche18Temperature, nomModule, Ruche.tempeDs18b20.c_str(), 0);  // Envoi des données via JSON et MQTT
          // envoi de la tension de la baterrie
          nomModule = "Ruche18_batterie_tension";
          // convert string to float
          float bat = atof(Ruche.vBat.c_str());
          // pour eliminer les valeurs sans decimales
          if (bat > (int)bat) {
            SendData(idxDeviceRuches18TensionBatterie, nomModule, Ruche.vBat.c_str(), 0);  // Envoi des données via JSON et MQTT
          }
        } else if (Ruche.numRuche == "19") {
          // envoi du poids
          nomModule = "Ruche19_poids";
          // convert string to float
          float poids = atof(Ruche.poids.c_str());
          // pour eliminer les valeurs sans decimales
          if (poids > (int)poids) {
            SendData(idxDeviceRuche19Poids, nomModule, Ruche.poids.c_str(), 0);  // Envoi des donnees via JSON et MQTT
            essaimage(poids);
          }
          // envoi de la temperature
          nomModule = "Ruche19_temperature";
          SendData(idxDeviceRuche19Temperature, nomModule, Ruche.tempeDs18b20.c_str(), 0);  // Envoi des données via JSON et MQTT
          // envoi de la tension de la baterrie
          nomModule = "Ruche19_batterie_tension";
          // convert string to float
          float bat = atof(Ruche.vBat.c_str());
          // pour eliminer les valeurs sans decimales
          if (bat > (int)bat) {
            SendData(idxDeviceRuches19TensionBatterie, nomModule, Ruche.vBat.c_str(), 0);  // Envoi des données via JSON et MQTT
          }
        } else if (Ruche.numRuche == "21") {
          // envoi du poids
          nomModule = "Ruche21_poids";
          // convert string to float
          float poids = atof(Ruche.poids.c_str());
          // pour eliminer les valeurs sans decimales
          if (poids > (int)poids) {
            SendData(idxDeviceRuche21Poids, nomModule, Ruche.poids.c_str(), 0);  // Envoi des donnees via JSON et MQTT
            essaimage(poids);
          }
          // envoi de la temperature
          nomModule = "Ruche21_temperature";
          SendData(idxDeviceRuche21Temperature, nomModule, Ruche.tempeDs18b20.c_str(), 0);  // Envoi des données via JSON et MQTT
          // envoi de la tension de la baterrie
          nomModule = "Ruche21_batterie_tension";
          // convert string to float
          float bat = atof(Ruche.vBat.c_str());
          // pour eliminer les valeurs sans decimales
          if (bat > (int)bat) {
            SendData(idxDeviceRuches21TensionBatterie, nomModule, Ruche.vBat.c_str(), 0);  // Envoi des données via JSON et MQTT
          }
        } else if (Ruche.numRuche == "22") {
          // envoi du poids
          nomModule = "Ruche22_poids";
          // convert string to float
          float poids = atof(Ruche.poids.c_str());
          // pour eliminer les valeurs sans decimales
          if (poids > (int)poids) {
            SendData(idxDeviceRuche22Poids, nomModule, Ruche.poids.c_str(), 0);  // Envoi des donnees via JSON et MQTT
            essaimage(poids);
          }
          // envoi de la temperature
          nomModule = "Ruche22_temperature";
          SendData(idxDeviceRuche22Temperature, nomModule, Ruche.tempeDs18b20.c_str(), 0);  // Envoi des données via JSON et MQTT
          // envoi de la tension de la baterrie
          nomModule = "Ruche22_batterie_tension";
          // convert string to float
          float bat = atof(Ruche.vBat.c_str());
          // pour eliminer les valeurs sans decimales
          if (bat > (int)bat) {
            SendData(idxDeviceRuches22TensionBatterie, nomModule, Ruche.vBat.c_str(), 0);  // Envoi des données via JSON et MQTT
          }
        } else if (Ruche.numRuche == "23") {
          // envoi du poids
          nomModule = "Ruche23_poids";
          // convert string to float
          float poids = atof(Ruche.poids.c_str());
          // pour eliminer les valeurs sans decimales
          if (poids > (int)poids) {
            SendData(idxDeviceRuche23Poids, nomModule, Ruche.poids.c_str(), 0);  // Envoi des donnees via JSON et MQTT
            essaimage(poids);
          }
          // envoi de la temperature
          nomModule = "Ruche23_temperature";
          SendData(idxDeviceRuche23Temperature, nomModule, Ruche.tempeDs18b20.c_str(), 0);  // Envoi des données via JSON et MQTT
          // envoi de la tension de la baterrie
          nomModule = "Ruche23_batterie_tension";
          // convert string to float
          float bat = atof(Ruche.vBat.c_str());
          // pour eliminer les valeurs sans decimales
          if (bat > (int)bat) {
            SendData(idxDeviceRuches23TensionBatterie, nomModule, Ruche.vBat.c_str(), 0);  // Envoi des données via JSON et MQTT
          }
        }
        /*else if (Ruche.numRuche == "1") {
            // envoi du poids
            SendData(idxDeviceRuche1Poids, "Ruche1_poids", Ruche.poids.c_str(), 0); // Envoi des donnees via JSON et MQTT
            // envoi de la temperature
            SendData(idxDeviceRuche1Temperature, "Ruche1_temperature", Ruche.tempeDs18b20.c_str(), 0); // Envoi des données via JSON et MQTT
            // envoi de la tension de la baterrie
            SendData(idxDeviceRuches1TensionBatterie, "Ruche1_batterie_tension", Ruche.vBat.c_str(), 0); // Envoi des données via JSON et MQTT
          }*/
#endif
      }
      // il manque des messages dans domoticz, donc
      clientMqtt.disconnect();
      Serial.println("MQTT disconnect");
    }
#endif

    // envoi des donnees vers thingspeak
    if (SECRET_THINGSPEAK_TRUE and SECRET_WIFI_TRUE) {
      thinkspeak();
    }
  }
  // reset esp32 intervals reguliers
  unsigned long ceMoment = millis();  // now
  if (ceMoment >= (lastResetWas + resetAfterMillis)) {
    lastResetWas = ceMoment;
    delay(20);
    ESP.restart();
  }
}

void essaimage(float poids) {
  int ruche = atof(Ruche.numRuche.c_str());
  if (ruche > 9 and ruche < 20) {
    ruche = ruche - 10;  // pour ruche loic entre 11 et 19
  } else if (ruche > 20 and ruche < 30) {
    ruche = ruche - 11;  // pour ruches loic entre 21 et 29
  }
  int idx = atof(readingID.c_str());
  float poids_anterieur;  // poids lecture precedente
  // verification essaimage
  tableauIdxLora[ruche][idx] = poids;  // enregistrement du poids en fonction de la ruche et de l'index lora
  if (idx >= 1) {                      // idx de 0 a 9
    poids_anterieur = tableauIdxLora[ruche][idx--];
  } else {
    poids_anterieur = tableauIdxLora[ruche][9];
  }
  if (debug) {
    Serial.print("poids anterieur: ");
    Serial.println(poids_anterieur);
    Serial.print("poids : ");
    Serial.println(poids);
  }
  if ((poids_anterieur > 0.0) and (poids_anterieur - poids > ecart)) {  // ecart represente le poids mini d'un essaim
    nomModule = "alerte_essaimage_ruche";
    int level = 1;                                                                             // alerte Level = (0=gray, 1=green, 2=yellow, 3=orange, 4=red)
    SendData(idxDeviceAlerteEssaimageRuchesPoids, nomModule, Ruche.numRuche.c_str(), level);   // Envoi des donnees via JSON et MQTT - Level = (0=gray, 1=green, 2=yellow, 3=orange, 4=red)
    SendData(idxDeviceAlerteEssaimageRuchesSwitch, nomModule, Ruche.numRuche.c_str(), level);  // Envoi des donnees via JSON et MQTT - alerte 1
  } else {
    int level = 0;                                                                             // alerte Level = (0=gray, 1=green, 2=yellow, 3=orange, 4=red)
    SendData(idxDeviceAlerteEssaimageRuchesPoids, nomModule, Ruche.numRuche.c_str(), level);   // Envoi des donnees via JSON et MQTT - Level = (0=gray, 1=green, 2=yellow, 3=orange, 4=red)
    SendData(idxDeviceAlerteEssaimageRuchesSwitch, nomModule, Ruche.numRuche.c_str(), level);  // Envoi des donnees via JSON et MQTT - pas d'alerte
  }
}

/*
  display.ssd1306_command(SSD1306_SETCONTRAST);
  display.ssd1306_command(c); // Where c is a value from 0 to 255 (sets contrast e.g. brightness)
  display.ssd1306_command(SSD1306_DISPLAYOFF); // To switch display off
  display.ssd1306_command(SSD1306_DISPLAYON); // To switch display back on

  void sleepDisplay(Adafruit_SSD1306* display) {
  display->ssd1306_command(SSD1306_DISPLAYOFF);
  }

  void wakeDisplay(Adafruit_SSD1306* display) {
  display->ssd1306_command(SSD1306_DISPLAYON);
  }

  // To scroll the whole display, run: display.startscrollright(0x00, 0x0F) <-- SEE THIS!!
  void Adafruit_SSD1306::startscrollright(uint8_t start, uint8_t stop) {
  TRANSACTION_START
  static const uint8_t PROGMEM scrollList1a[] = {
  SSD1306_RIGHT_HORIZONTAL_SCROLL,
  0X00 };
  ssd1306_commandList(scrollList1a, sizeof(scrollList1a));
  ssd1306_command1(start);
  ssd1306_command1(0X00);
  ssd1306_command1(stop);
  static const uint8_t PROGMEM scrollList1b[] = {
  0X00,
  0XFF,
  SSD1306_ACTIVATE_SCROLL };
  ssd1306_commandList(scrollList1b, sizeof(scrollList1b));
  TRANSACTION_END
  }
*/

//======
// WIFI
//======
void setup_wifi() {
  // WiFi.mode(WIFI_AP_STA) // station et point d'acces
  WiFi.mode(WIFI_STA);  // mode de connection wifi : station

  // wifiMulti.addAP("ssid_from_AP_3", "your_password_for_AP_3");
  // SSID du réseau Wifi et  Mot de passe du réseau Wifi dans le fichier secrets.h
#if JLM
  wifiMulti.addAP(SECRET_SSID1, SECRET_PASS1);
  wifiMulti.addAP(SECRET_SSID2, SECRET_PASS2);
  wifiMulti.addAP(SECRET_SSID3, SECRET_PASS3);
  wifiMulti.addAP(SECRET_SSID4, SECRET_PASS4);
  wifiMulti.addAP(SECRET_SSID5, SECRET_PASS5);
#elif LOIC
  wifiMulti.addAP(SECRET_SSID1, SECRET_PASS1);
  wifiMulti.addAP(SECRET_SSID2, SECRET_PASS2);
  wifiMulti.addAP(SECRET_SSID3, SECRET_PASS3);
  //wifiMulti.addAP(SECRET_SSID4, SECRET_PASS4);
#endif
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
      delay(100);
    }
  }
  // Connect to Wi-Fi using wifiMulti (connects to the SSID with strongest connection)
  Serial.println("Connecting Wifi");
  if (wifiMulti.run() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected setup");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    display.clearDisplay();
    display.setCursor(0, 10);
    display.print("IP Adr: ");
    display.setCursor(40, 10);
    display.print(WiFi.localIP());
    display.display();
    delay(100);
  }
}

// connection wifi
void connect_wifi() {
  //if the connection to the stongest hotstop is lost, it will connect to the next network on the list
  if (wifiMulti.run(connectTimeoutMs) == WL_CONNECTED) {
    adresse_ip = WiFi.localIP().toString();
#if SerialMonitor
    Serial.print("WiFi connected loop ");
    Serial.print(WiFi.SSID());
    Serial.print(" ");
    Serial.println(WiFi.RSSI());
    Serial.print("IP address: ");
    Serial.println(adresse_ip);
    display.setCursor(0, 10);
    display.print("IP Adr: ");
    display.setCursor(40, 10);
    display.print(adresse_ip);
    display.display();
    delay(100);
#endif
  } else {
    display.setCursor(0, 10);
    display.print("IP Adr: ");
    display.setCursor(40, 10);
    display.print("    no wifi   ");
    display.display();
    if (debug) {
      Serial.println("WiFi not connected!");
      delay(100);
    }
  }
}
/*
  typedef enum {
  WL_NO_SHIELD        = 255,   // for compatibility with WiFi Shield library
  WL_IDLE_STATUS      = 0,
  WL_NO_SSID_AVAIL    = 1,
  WL_SCAN_COMPLETED   = 2,
  WL_CONNECTED        = 3,
  WL_CONNECT_FAILED   = 4,
  WL_CONNECTION_LOST  = 5,
  WL_DISCONNECTED     = 6
  } wl_status_t;

  WL_NO_SHIELD: assigned when no WiFi shield is present;
  la methode WiFi.status renvoie :
  0: WL_IDLE_STATUS it is a temporary status assigned when WiFi.begin() is called and remains active until the number of attempts expires (resulting in WL_CONNECT_FAILED) or a connection is established (resulting in WL_CONNECTED), lorsque le Wi-Fi est en train de changer d’état
  1: WL_NO_SSID_AVAIL assigned when no SSID are available, si le SSID configuré ne peut pas être atteint
  2: WL_SCAN_COMPLETED assigned when the scan networks is completed
  3: WL_CONNECTED assigned when connected to a WiFi network, une fois la connexion établie
  4: WL_CONNECT_FAILED assigned when the connection fails for all the attempts, si le mot de passe est incorrect
  5: WL_CONNECTION_LOST assigned when the connection is lost, si la connection est perdue
  6: WL_DISCONNECTED assigned when disconnected from a network, si le module n’est pas configuré en mode station
*/

//===============
// CONNEXION MQTT
//===============
#if MQTT
void reconnect() {
  // Boucle jusqu'à la connexion MQTT
  while (!clientMqtt.connected() && WLcountMQTT < 50) {
    Serial.println("Tentative de connexion MQTT");
    // Création d'un ID clientMqtt aléatoire
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    // Tentative de connexion
    if (clientMqtt.connect(clientId.c_str(), mqttUser, mqttPassword)) {
      Serial.println("connecte");

      // Connexion effectuee, publication d'un message...
      String message = "Connexion MQTT de " + nomModule + " reussi sous reference technique : " + clientId + ".";
      // String message = "Connexion MQTT de "+ nomModule + " reussi.";
      StaticJsonDocument<256> doc;  // v6
      // On renseigne les variables.
      doc["command"] = "addlogmessage";
      doc["message"] = message;
      // On serialise la variable JSON
      String messageOut;
      if (serializeJson(doc, messageOut) == 0) {
        Serial.println("Erreur lors de la creation du message de connexion pour Domoticz");
      } else {
        // Convertion du message en Char pour envoi dans les Log Domoticz.
        char messageChar[messageOut.length() + 1];
        messageOut.toCharArray(messageChar, messageOut.length() + 1);
        clientMqtt.publish(topicOut, messageChar);
      }
      // On souscrit (ecoute)
      clientMqtt.subscribe("#");
    } else {
      Serial.print("Erreur, rc=");
      Serial.print(clientMqtt.state());
    }
    ++WLcountMQTT;
  }
  WLcountMQTT = 0;
}

// envoi des donnes avec l'idx de domoticz en fonction des sondes
void SendData(int idxDevice, String description, const char* chaine, int nvalue) {
  // Creation et Envoi de la donnee JSON.
  StaticJsonDocument<256> doc;  // v6
  // On renseigne les variables.
  doc["ip"] = adresse_ip;
  doc["descrip"] = nomModule;
  doc["type"] = "command";
  doc["param"] = "udevice";
  doc["idx"] = idxDevice;
  doc["nvalue"] = nvalue;
  doc["svalue"] = chaine;
  // On serialise la variable JSON
  String messageOut;
  if (serializeJson(doc, messageOut) == 0) {
    Serial.println("Erreur lors de la creation du message de connexion pour Domoticz");
  } else {
    // Convertion du message en Char pour envoi dans les Log Domoticz.
    char messageChar[messageOut.length() + 1];
    messageOut.toCharArray(messageChar, messageOut.length() + 1);
    clientMqtt.publish(topicOut, messageChar);
    if (debug) {
      Serial.print("Message envoye a Domoticz : ");
      Serial.println(messageChar);
    }
  }
}
#endif
