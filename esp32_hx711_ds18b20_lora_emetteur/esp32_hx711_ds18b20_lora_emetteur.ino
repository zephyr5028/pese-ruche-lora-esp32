/********************************************************************************************
   Balance connectée avec ESP32 et HX711 et ds18b20 et lora

   jlm le 19 septembre 2022   -  suivi par GIT

   https://randomnerdtutorials.com/esp32-deep-sleep-arduino-ide-wake-up-sources/
   https://randomnerdtutorials.com/esp32-lora-rfm95-transceiver-arduino-ide/
   https://github.com/nopnop2002/Arduino-LoRa-Ra01S
   https://docs.ai-thinker.com/_media/lora/docs/ra-01sh_specification.pdf
   https://github.com/tinytronix/SX126x
   https://www.semtech.com/products/wireless-rf/lora-connect/sx1262
   https://github.com/sandeepmistry/arduino-LoRa/blob/master/API.md
   https://lora.readthedocs.io/en/latest/

********************************************************************************************/

//=============================
// inclusion des bibliothèques
//=============================
#include "variables.h" // fichier variables
#include "secrets.h"   // fichier secrets

// bibliotheque load cell hx711
#include "HX711.h"
/*
   Note: we’ve read in some places that you need to slow down the ESP32 processor because of the HX711 frequency.
   I’m not sure if this is really needed or not.
   We’ve experimented with and without slowing down and everything worked fine in both scenarios.
   Nonetheless, we’ve added that option to the code. You can always remove it.
   To do that you need to include soc/rtc.h.
*/
//#include "soc/rtc.h"

//Libraries for DS18B20
#include <OneWire.h>       //Librairie du bus OneWire
//#include <DallasTemperature.h>

//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>

#include <Wire.h>

#if oled
//Libraries for OLED Display
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#endif

// HX711 circuit wiring
#define DOUT  23
#define CLK  25

HX711 scale;  // objet scale

//----------------------
// ruche jlm autonome
//----------------------
#if RUCHE_NUMERO == 04 or RUCHE_NUMERO == 05
// pour sauvegarder donnees a chaque coupure de l'alimentation
#include <Preferences.h>

// Libraries BME280
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>

// include button pour le contacteur
#include <Button.h>

Preferences preferences;  // Save Data Permanently
#endif

//=================================
// structure de donnees d'une ruche
//=================================
struct ruche {
  int numRuche;  // numero de la ruche
  float poids;   // poids de la ruche
  float tempe;   // temperature de la ruche
  float vBat;    // tension batterie
};

// objet Ruche et control
ruche Ruche = { .numRuche = RUCHE_NUMERO, .poids = 0.00, .tempe = 0.00, .vBat = 0.00 }; // definition de la ruche
ruche RucheControl = { .numRuche = RUCHE_NUMERO, .poids = 0.00, .tempe = 0.00, .vBat = 0.00 }; // definition de la ruche control

//=========
// DB18B20
//=========
/*
  // Data wire is plugged into port 2 on the Arduino
  #define ONE_WIRE_BUS 2

  // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
  OneWire oneWire(ONE_WIRE_BUS);

  // Pass our oneWire reference to Dallas Temperature.
  DallasTemperature sensors(&oneWire);
*/
// Broche du bus 1-Wire pour la sonde temperature ds18b20
#define ONEWIRE_BUS 17               // Pin de connexion des sondes DS18B20
#define TEMPERATURE_PRECISION 12     // precision convertisseur ad du ds18b20 (9 à 12)
const int DS18B20_ID = 0x28;         // debut de l'adresse d'une sonde ds18b20

// Code de retour de la fonction getTemperature()
enum DS18B20_RCODES {
  READ_OK,          // Lecture ok
  NO_SENSOR_FOUND,  // Pas de capteur
  INVALID_ADDRESS,  // Adresse reçue invalide
  INVALID_SENSOR    // Capteur invalide (pas un DS18B20)
};

// Création de l'objet OneWire pour manipuler le bus 1-Wire
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
// 4.7K resistor is necessary between +3.3V ou 5v and Data wire
OneWire ds18b20(ONEWIRE_BUS);
byte i;
byte type_s;
byte data[12];
byte addr[8];

//----------------------
// ruche jlm autonome
//----------------------
#if RUCHE_NUMERO == 04 or RUCHE_NUMERO == 05
//=========
// BME280
//=========
Adafruit_BME280 bme280;                                     // I2C
//Adafruit_BME280 bme(BME_CS);                              // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

//==================================
// objets de donnees des Capteurs
//==================================
// objet capteurs et control
boitierCapteur BoitierCapteur = {
  .numBoitierCapteur = BOITIER_CAPTEUR_NUMERO,
  .nameBoitierCapteur = NAME_BOITIER,
  .interrupteur = true,
  .tempeDS18B20 = 0.00,
  .vBat = 0.00,
  .poids = 0.00
};

// objet bme280
capteur_bme280 Capteur_bme280 = {
  .tempe = "",
  .humi = "",
  .pression = "",
  .hum_stat = "",
  .bar_for = ""
};

// conversion de string to int : toInt()  string to float :  toFloat()
// Capteur_ds18b20.tempe = String(temp_ds18b20, 2); // convert float to string avec 2 decimal apres la virgule

//=======
// Button
//=======
Button interrupteur (33);   // contacteur gpio33
#endif

//================================================================
// brochage de l'emetteur/recepteur lora en fonction de la carte
// define the pins used by the LoRa transceiver module ttgo sx1276
// rfm95... distance faible et sx1262...ne fonctionne pas
//================================================================
#define DIO0 26
#define RST 14
#define SS 18
#define SCK 5
#define MOSI 27
#define MISO 19

// bande frequence lora
//433E6 for Asia
//866E6 for Europe
//915E6 for North America
//#define BAND 866E6
#define BAND 868300000

int txPower = 20; // power of tx lora 17 par default (2 to 20)
#define PABOOST true

#if oled
//==========
// OLED pins
//==========
#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);
#endif


/****************************************************
  Simple Deep Sleep with Timer Wake Up
  =====================================
  ESP32 offers a deep sleep mode for effective power
  saving as power is an important factor for IoT
  applications. In this mode CPUs, most of the RAM,
  and all the digital peripherals which are clocked
  from APB_CLK are powered off. The only parts of
  the chip which can still be powered on are:
  RTC controller, RTC peripherals ,and RTC memories

  This code displays the most basic deep sleep with
  a timer to wake it up and how to store data in
  RTC memory to use it over reboots

  This code is under Public Domain License.

  Author:
  Pranav Cherukupalli <cherukupallip@gmail.com>
********************************************************/

#define uS_TO_S_FACTOR 1000000  // Conversion factor for micro seconds to seconds

RTC_DATA_ATTR int bootCount = 0;   // sauvegarde de la variable avant le mode sleep dans la memoire rtc de l'esp32

//===========================================
//Method to print the reason by which ESP32
//has been awaken from sleep
//===========================================
void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason); break;
  }
}

// packet counter
RTC_DATA_ATTR  int readingID = 0; // sauvegarde de la variable avant le mode sleep dans la memoire rtc de l'esp32
unsigned int counterID = 0;       // compteur ID
int counter = 0;                  // compteur
String LoRaMessage = "";          // message lora avant envoi
long lastMsg = 0;

//======================
// tension alimentation
//======================
/*
  adcAttachPin(pin) : attachez une broche d'entrée analogique à l'ADC
  analogRead(pin) : Récupère la valeur ADC pour la broche spécifiée.
  analogSetWidth(bits) : Définit la résolution ADC (en bits). La valeur par défaut est 12 bits mais la plage est de 9 à 12.
  analogSetAttenuation(attenuation) : Règle l'atténuation pour tous les canaux. La valeur par défaut est 11db mais les valeurs possibles sont 0db, 2_5db, 6db et 11db.
  analogSetPinAttenuation(pin, attenuation) : définit l'atténuation pour une broche particulière.
  analogSetClockDiv(clockDiv) : définit le diviseur pour l'horloge ADC.
  analogSetVRefPin(pin) : définit la broche à utiliser pour l'étalonnage ADC si l'ESP32 n'est pas déjà calibré. Les broches possibles sont 25, 26 ou 27.
  analogReadMilliVolts(pin) : lit une voie analogique, convertit la valeur brute en tension (en mV) et renvoie cette valeur de tension.
  The voltage measured is then assigned to a value between 0 and 4095,
  in which 0 V corresponds to 0,
  and 3.3 V corresponds to 4095
*/
// mesure tension alimentation 12v
int  AnGpio = 35; // GPIO 35 is Now AN Input 1

//=====
// led
//=====
//#define LED_BOARD 2  // gpio2

#if oled
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
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) {    // Address 0x3C for 128x32
    if (SerialMonitor) {
      Serial.println(F("SSD1306 allocation failed"));
    }
    for (;;);     // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("PROJET LORA SENDER");
}
#endif

//========================
// Initialize LoRa module
//========================
void startLoRA() {
  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);

  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);

  while (!LoRa.begin(BAND) && counter < 10) {
    if (SerialMonitor) {
      Serial.print(".");
    }
    counter++;
    delay(400);
  }
  if (counter == 10) {
    if (SerialMonitor) {
      Serial.println("Starting LoRa failed!");
    }
#if oled
    display.setCursor(0, 10);
    display.clearDisplay();
    display.print("Starting LoRa failed!");
    display.display();
#endif

  } else {
    // chaque demarrage de la radio; activation dans le CRC
    //LoRa.enableCrc();
    //Facteur d’etalement (SF) : De 6 à 12, plus la valeur est grande, plus la portee est grande
    LoRa.setSpreadingFactor(10);       // Setup Spreading Factor (6 ~ 12)
    // Setup BandWidth, option: 7800,10400,15600,20800,31250,41700,62500,125000,250000,500000
    LoRa.setSignalBandwidth(125000);
    //LoRa.setCodingRate4(8);          // Setup Coding Rate:5(4/5),6(4/6),7(4/7),8(4/8)
    //LoRa.dumpRegisters(Serial);
    //LoRa.setTxPower(20,RF_PACONFIG_PASELECT_PABOOST); // 20dB output must via PABOOST

    /*
      LoRa.setTxPower(txPower,RFOUT_pin);
      txPower -- 0 ~ 20
      RFOUT_pin could be RF_PACONFIG_PASELECT_PABOOST or RF_PACONFIG_PASELECT_RFO
        - RF_PACONFIG_PASELECT_PABOOST -- LoRa single output via PABOOST, maximum output 20dBm
        - RF_PACONFIG_PASELECT_RFO     -- LoRa single output via RFO_HF / RFO_LF, maximum output 14dBm
    */
    if (SerialMonitor) {
      Serial.print("txPower : ");
      Serial.println(txPower);
    }
    LoRa.setTxPower(txPower);
    //LoRa.setTxPower(txPower,PABOOST);
    if (SerialMonitor) {
      Serial.println("LoRa Initialization OK!");
    }
#if oled
    display.setCursor(0, 10);
    display.clearDisplay();
    display.print("LoRa Initializing OK!");
    display.display();
#endif
  }

  // Change sync word (0xF3) to match the receiver
  // The sync word assures you don't get LoRa messages from other LoRa transceivers
  // ranges from 0-0xFF
  LoRa.setSyncWord(synchroLora);

  delay(1000);
}

//===============================================
// Write LoRa Send
//===============================================
void sendReadings() {
  //----------------------
  // ruche jlm autonome
  //----------------------
#if RUCHE_NUMERO == 04 or RUCHE_NUMERO == 05
  // message a envoyer
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

  String message = LoRaMessage;
  //Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.print(LoRaMessage);
  LoRa.endPacket();
  delay(200);
  if (SerialMonitor) {
    Serial.print("Message LoRa envoye: ");
    Serial.println(message);
    Serial.print("Sending packet counterID : ");
    Serial.println(counterID);
    Serial.print("Temperature DS18B20: ");
    Serial.print(BoitierCapteur.tempeDS18B20);
    Serial.println(" °C");
    Serial.print("Boitier capteur N°: ");
    Serial.println(BoitierCapteur.numBoitierCapteur);
    Serial.print("Temperature BME280: ");
    Serial.println(Capteur_bme280.tempe);
    Serial.print("tension d'alimentation : ");
    Serial.print(BoitierCapteur.vBat, 2);
    Serial.println(" V");
    Serial.println();
  }

#else
  // message a envoyer
  LoRaMessage = String(readingID) +
                "/" + String(Ruche.tempe) +
                "&" + String(Ruche.numRuche) +
                "#" + String(Ruche.poids) +
                "{" + String(Ruche.vBat);

  String message = LoRaMessage;
  //Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.print(LoRaMessage);
  LoRa.endPacket();
  delay(200);
  if (SerialMonitor) {
    Serial.print("Message LoRa envoye: ");
    Serial.println(message);
    Serial.print("Sending packet: ");
    Serial.println(readingID);
    Serial.print("Temperature: ");
    Serial.print(Ruche.tempe);
    Serial.println(" °C");
    Serial.print("Ruche N°: ");
    Serial.println(Ruche.numRuche);
    Serial.print("Poids : ");
    Serial.print(Ruche.poids);
    Serial.println(" kg");
    Serial.print("tension d'alimentation : ");
    Serial.print(Ruche.vBat, 2);
    Serial.println(" V");
  }
  //Serial.println();

#if oled
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print("LoRa packet sent!");

  display.setCursor(0, 20);
  display.print("Ruche n: ");
  display.setCursor(52, 20);
  display.print(Ruche.numRuche);

  display.setCursor(0, 30);
  display.print("Poids:");
  display.setCursor(42, 30);
  display.print(Ruche.poids);
  display.setCursor(88, 30);
  display.print("kg");

  display.setCursor(0, 40);
  display.print("Temperature: ");
  display.setCursor(76, 40);
  display.print(Ruche.tempe);
  display.setCursor(112, 40);
  display.print("C");

  display.setCursor(0, 50);
  display.print("num ID : ");
  display.setCursor(54, 50);
  display.print(readingID);
  display.display();
#endif
#endif

  delay(400);
  // suivi chronologique de l'envoi des messages lora
  ++readingID;
  if (readingID >= 10) {
    readingID =  0;
  }
}

//===============================================================
// Fonction de lecture de la temperature via un capteur DS18B20.
//===============================================================
byte getTemperature(float *temperature, byte reset_search) {
  // data[] : Données lues depuis le scratchpad
  // addr[] : Adresse du module 1-Wire détecté
  byte data[9], addr[8];

  // Reset le bus 1-Wire ci necessaire (requis pour la lecture du premier capteur)
  if (reset_search) {
    ds18b20.reset_search();
  }

  // Recherche le prochain capteur 1-Wire disponible
  if (!ds18b20.search(addr)) {
    // Pas de capteur
    return NO_SENSOR_FOUND;
  }

  // Verifie que l'adresse a ete correctement recue
  if (OneWire::crc8(addr, 7) != addr[7]) {
    // Adresse invalide
    return INVALID_ADDRESS;
  }

  // Verifie qu'il s'agit bien d'un DS18B20
  if (addr[0] != DS18B20_ID) {
    // Mauvais type de capteur
    return INVALID_SENSOR;
  }

  // Reset le bus 1-Wire et selectionne le capteur
  ds18b20.reset();
  ds18b20.select(addr);

  // Lance une prise de mesure de temperature et attend la fin de la mesure
  ds18b20.write(0x44, 1);
  delay(800);

  // Reset le bus 1-Wire, selectionne le capteur et envoie une demande de lecture du scratchpad
  ds18b20.reset();
  ds18b20.select(addr);
  ds18b20.write(0xBE);

  // Lecture du scratchpad
  for (byte i = 0; i < 9; i++) {
    data[i] = ds18b20.read();
  }
  // Serial.println(addr[0]); // premier octet adresse du ds18b20 : 40 en decimal, 28 hexa

  // Calcul de la temperature en degre Celsius
  *temperature = (int16_t) ((data[1] << 8) | data[0]) * 0.0625;

  // Pas d'erreur
  return READ_OK;
}

//==============
// Read sensors
//==============
void getReadings() {
  // lecture du poids de la ruche
  delay(200);
  Ruche.poids = scale.get_units(numberOfReadings); // numberOfReadings readings from the ADC minus tare weight
  delay(300);
  RucheControl.poids = scale.get_units(numberOfReadings); // numberOfReadings readings from the ADC minus tare weight
  // test + ou - poidsAberrant pour eviter les mesure aberrantes
  if ((Ruche.poids > (RucheControl.poids + poidsAberrant)) or (Ruche.poids < (RucheControl.poids - poidsAberrant ))) {
    delay(100);
    Ruche.poids = scale.get_units(numberOfReadings); // numberOfReadings readings from the ADC minus tare weight
  } else {
    delay(300);
    RucheControl.poids = scale.get_units(numberOfReadings); // numberOfReadings readings from the ADC minus tare weight
    delay(300);
    Ruche.poids = scale.get_units(numberOfReadings); // numberOfReadings readings from the ADC minus tare weight
    if ((Ruche.poids > (RucheControl.poids + poidsAberrant)) or (Ruche.poids < (RucheControl.poids - poidsAberrant ))) {
      delay(100);
      Ruche.poids = scale.get_units(numberOfReadings); // numberOfReadings readings from the ADC minus tare weight
    } else {
      delay(300);
      Ruche.poids = scale.get_units(numberOfReadings); // numberOfReadings readings from the ADC minus tare weight
    }
  }
  // Lecture de la temperature ambiante a ~1Hz
  if (getTemperature(&Ruche.tempe, true) != READ_OK) {
    if (SerialMonitor) {
      Serial.println(F("Erreur de lecture du capteur"));
    }
  }
  delay(300);
  if (getTemperature(&RucheControl.tempe, true) != READ_OK) {
    if (SerialMonitor) {
      Serial.println(F("Erreur de lecture du capteur"));
    }
  }
  delay(100);
  // test + ou - temperatureAberrante pour eviter les mesure aberrantes
  if ((Ruche.tempe > (RucheControl.tempe + temperatureAberrante)) or (Ruche.tempe < (RucheControl.tempe - temperatureAberrante))) {
    if (getTemperature(&Ruche.tempe, true) != READ_OK) {
      if (SerialMonitor) {
        Serial.println(F("Erreur de lecture du capteur"));
      }
    } else {
      delay(300);
      if (getTemperature(&Ruche.tempe, true) != READ_OK) {
        if (SerialMonitor) {
          Serial.println(F("Erreur de lecture du capteur"));
        }
      }
    }
  }
  //----------------------
  // ruche jlm autonome
  //----------------------
#if RUCHE_NUMERO == 04 or RUCHE_NUMERO == 05
  float BoitierCapteurControlTempeDS18B20 = 0.0; // si temperature aberrante
  // Lecture de la temperature ambiante a ~1Hz
  if (getTemperature(&BoitierCapteur.tempeDS18B20, true) != READ_OK) {
    if (SerialMonitor) {
      Serial.println(F("Erreur de lecture du capteur DS18B20"));
    }
  }
  delay(300);
  if (getTemperature(&BoitierCapteurControlTempeDS18B20, true) != READ_OK) {
    if (SerialMonitor) {
      Serial.println(F("Erreur de lecture du capteur DS18B20"));
    }
  }
  delay(100);
  // test + ou - temperatureAberrante pour eviter les mesure aberrantes
  if ((BoitierCapteur.tempeDS18B20 > (BoitierCapteurControlTempeDS18B20 + temperatureAberrante)) or (BoitierCapteur.tempeDS18B20 < (BoitierCapteurControlTempeDS18B20 - temperatureAberrante))) {
    if (getTemperature(&BoitierCapteur.tempeDS18B20, true) != READ_OK) {
      if (SerialMonitor) {
        Serial.println(F("Erreur de lecture du capteur DS18B20"));
      }
    } else {
      delay(300);
      if (getTemperature(&BoitierCapteur.tempeDS18B20, true) != READ_OK) {
        if (SerialMonitor) {
          Serial.println(F("Erreur de lecture du capteur DS18B20"));
        }
      }
    }
  }
  BoitierCapteur.poids = Ruche.poids;
  // tension de la batterie
  mesureTension = tensionBatterie();
  if (mesureTension > tensionFaible and mesureTension < tensionElevee) {
    BoitierCapteur.vBat = mesureTension; // pour un boitier capteur
  } else {
    delay(300);
    mesureTension = tensionBatterie();
    if (mesureTension > tensionFaible and mesureTension < tensionElevee) {
      BoitierCapteur.vBat = mesureTension; // pour un boitier capteur
    } else {
      delay(300);
      mesureTension = tensionBatterie();
      BoitierCapteur.vBat = mesureTension; // pour un boitier capteur
    }
  }
#endif

  // tension de la batterie
  mesureTension = tensionBatterie();
  if (mesureTension > tensionFaible and mesureTension < tensionElevee) {
    Ruche.vBat = mesureTension; // pour un boitier capteur
  } else {
    delay(300);
    mesureTension = tensionBatterie();
    if (mesureTension > tensionFaible and mesureTension < tensionElevee) {
      Ruche.vBat = mesureTension; // pour un boitier capteur
    } else {
      delay(300);
      mesureTension = tensionBatterie();
      Ruche.vBat = mesureTension; // pour un boitier capteur
    }
  }
  if (SerialMonitor) {
    Serial.print("Weight: ");
    Serial.print(Ruche.poids, 3); //Up to 3 decimal points
    Serial.println(" kg"); //Change this to kg and re-adjust the calibration factor if you follow lbs
  }

  // sur le moniteur serie
  if (SerialMonitor) {
    Serial.println("Press t to tare");
    //Serial.println("Press T or t to tare");
  }
  if (Serial.available()) {
    char temporaire = Serial.read();
    //if (temporaire == 't' || temporaire == 'T') scale.tare(); //Reset the scale to zero
    if (temporaire == 't') scale.tare();    //Reset the scale to zero
  }
}

//----------------------
// ruche jlm autonome
//----------------------
#if RUCHE_NUMERO == 04 or RUCHE_NUMERO == 05
//=============
// Read BME280
//=============
// BME280
void getSondes_bme280() {
  temp = bme280.readTemperature(); //lecture de la temperature
  Capteur_bme280.tempe = String(temp, 2); // convert float to string avec 2 decimal apres la virgule
  delay(100); // pause 100ms

  hum = bme280.readHumidity();  // lecture de l'humidite
  Capteur_bme280.humi = String(hum, 0); // convert float to string avec 0 decimal apres la virgule
  delay(100); // pause 100ms
  // Si vous écrivez :
  //float pressure = .01 * bme280.getPressure() ;
  //Serial.print (pressure , 2) ;
  //Vous imprimerez par exemple : 991.17, C’est normal, vous multipliez un flottant (0.01) par un uint32_t (getPressure() ), la conversion implicite vous donne bien un float avec toutes ses décimales.
  //mais si vous écrivez
  //float pressure = bme280.getPressure() / 100;
  //Vous imprimerez : 991.00 en perdant les deux chiffres significatifs ! C’est une faute, vous divisez un uint32_t par un entier, le résultat est un entier, converti ensuite en float et il n’y a donc rien après la virgule.
  //Par contre en écrivant /100.0 , vous divisez bien un uint32_t par un double,  la conversion implicite vous donne bien un float avec toutes ses décimales.
  //Les conversions de types sont complexes et sujettes à bien des erreurs…
  pre = bme280.readPressure() / 100.0;  // lecture de la pression / 100

  //Valeur retenue = (Valeur lue * coefficient multiplicateur) + offset
  //Suivant les cartes, coefficients multiplicateurs entre .99 et 1.1 et offsets de +/- 3 hPa
  pre = (pre * 1.0) + 2; // correction de la lecture de la pression en fonction du composant
  delay(100); // pause 100ms
  pre = bme280.seaLevelForAltitude(ALTITUDE, pre);  // pression calculee
  Capteur_bme280.pression = String(pre, 0); // convert float string avec 0 decimal apres la virgule
  //float alt = bme.readAltitude(SEALEVELPRESSURE_HPA);  // altitude du lieu calcule
  //Forecast: 0 None, 1 Sunny, 2 PartlyCloudy, 3 Cloudy, 4 Rain
  //-(altitude / 8) pour la correction due a l'atltitude !!!

  // Forecast: 0 - None, 1 - Sunny, 2 - PartlyCloudy, 3 - Cloudy, 4 - Rain, de la sonde bme280
  bar_for = 0;
  if (pre < 1006) {
    bar_for = 4; // rain
    Capteur_bme280.bar_for = "rain";
  } else if ((pre >= 1006 ) & (pre < 1013 )) {
    bar_for = 3; // cloudy
    Capteur_bme280.bar_for = "cloudy";
  } else if ((pre >= 1013 ) & (pre < 1020 )) {
    bar_for = 2; // Partly Cloudy
    Capteur_bme280.bar_for = "partly cloudy";
  } else if (pre >= 1020 ) {
    bar_for = 1; // wet / sunny
    Capteur_bme280.bar_for = "wet sunny";
  } else {
    bar_for = 0;  // none
    Capteur_bme280.bar_for = "none";
  }

  // Humidity status: 0 Normal, 1 Comfort, 2 Dry, 3 Wet
  hum_stat = 0;
  if (hum < 30) {
    hum_stat = 2; // dry
    Capteur_bme280.hum_stat = "dry";
  } else if ((hum >= 30) & (hum < 45)) {
    hum_stat = 0; // normal
    Capteur_bme280.hum_stat = "normal";
  } else if ((hum >= 45) & (hum < 70)) {
    hum_stat = 1; // confort
    Capteur_bme280.hum_stat = "comfort";
  } else if (hum >= 70) {
    hum_stat = 3; // humide
    Capteur_bme280.hum_stat = "wet";
  }

  // Uncomment the next line to set temperature in Fahrenheit
  // (and comment the previous temperature line)
  // temperature = 1.8 * bme.readTemperature() + 32; // Temperature in Fahrenheit

  if (SerialMonitor) {
    Serial.print("temperature_BME280 : ");
    Serial.println(Capteur_bme280.tempe);
    Serial.print("humidite_BME280 : ");
    Serial.println(Capteur_bme280.humi);
    Serial.print("barometre_BME280 : ");
    Serial.println(Capteur_bme280.pression);
    Serial.print("forecast_BME280 : ");
    Serial.println(Capteur_bme280.bar_for);
    Serial.print("humidite_status_BME280 : ");
    Serial.println(Capteur_bme280.hum_stat);
    Serial.print("altitude du lieu : "); // pour les calculs avec la sonde bme280
    Serial.println(ALTITUDE);
  }
}

// chaine pour l'envoi des valeurs de la sonde BME280
void chaine_bme280() {
  // Convert the value to a char array
  char valeur_temp[6] = ""; // temperature
  dtostrf(temp, 2, 2, valeur_temp);
  strcat(chaine, valeur_temp);
  strcat(chaine, ";");
  char valeur_hum[5] = ""; // humidite
  dtostrf(hum, 2, 2, valeur_hum);
  strcat(chaine, valeur_hum);
  strcat(chaine, ";");
  char valeur_hum_stat[1] = ""; // humidite confort
  dtostrf(hum_stat, 1, 0, valeur_hum_stat);
  strcat(chaine, valeur_hum_stat);
  strcat(chaine, ";");
  char valeur_pre[6] = ""; // pression barometrique
  dtostrf(pre, 4, 0, valeur_pre);
  strcat(chaine, valeur_pre);
  strcat(chaine, ";");
  char valeur_bar_for[1] = ""; // prevision meteo avec le barometre
  dtostrf(bar_for, 1, 0, valeur_bar_for);
  strcat(chaine, valeur_bar_for);
  if (SerialMonitor) {
    Serial.print("Chaine Sonde bme280 : ");
    Serial.println(chaine);
  }
}

//=============
// interrupteur
//=============
void etatInterrupteur() {
  if (interrupteur.read() == 0) {
    if (SerialMonitor) {
      Serial.println("interrupteur ferme");
    }
    BoitierCapteur.interrupteur = false;  // true 1 ouvert et false 0 ferme
  } else {
    if (SerialMonitor) {
      Serial.println("interrupteur ouvert");
    }
    BoitierCapteur.interrupteur = true;  // true 1 ouvert et false 0 ferme
  }
}

//============================================
// memorisation en cas de coupure de l'energie
//============================================
void coupureEnergie () {
  // Open Preferences with my-app namespace. Each application module, library, etc
  // has to use a namespace name to prevent key name collisions. We will open storage in
  // RW-mode (second parameter has to be false).
  // Note: Namespace name is limited to 15 chars.
  preferences.begin("counterID", false);

  // Remove all preferences under the opened namespace
  // preferences.clear();

  // Or remove the counterID key only
  //preferences.remove("counterID");

  // Get the counter value, if the key does not exist, return a default value of 0
  // Note: Key name is limited to 15 chars.
  counterID = preferences.getUInt("counterID", 0);

  // Print the counter to Serial Monitor
  if (SerialMonitor) {
    Serial.printf("Current counterID value: %u\n", counterID);
  }

  // Increase counter by 1
  counterID++;  // suivi chronologique du wakeup

  // Store the counter to the Preferences
  preferences.putUInt("counterID", counterID);

  // Close the Preferences
  preferences.end();

  if (counterID >= 10) {
    if (SerialMonitor) {
      Serial.println("passage dans la boucle");
    }
    preferences.begin("counterID", false);
    preferences.clear();
    preferences.end();
  }
}
#endif

//=======================
// tension de la batterie
//=======================
float tensionBatterie() {
  float vBat = 0.0;
  float voutBat = 0.0;
  int AnGpioResult = 0;
  // mesures tension d'alimentation
  for (int i = 0; i < numberOfReadingsBat; i++) {
    delay(80);
    // The voltage measured is then assigned to a value between 0 and 4095
    // in which 0 V corresponds to 0, and 3.3 V corresponds to 4095
    AnGpioResult = AnGpioResult + analogRead(AnGpio);
    //AnGpioResult =  analogRead(AnGpio);
    //Serial.print(" gpio   : ");
    //Serial.println(AnGpioResult);
  }
  AnGpioResult = AnGpioResult / numberOfReadingsBat; // resultat apres plusieurs mesures
  // calcul du resultat en volt
  voutBat = (((AnGpioResult * tensionEsp32) / cad) + offsetCalcule);
  if (SerialMonitor) {
    Serial.print("tension mesuree adc : ");
    Serial.println(AnGpioResult);
    Serial.print("tension voutBat : ");
    Serial.println(voutBat);
    Serial.print("tension batterie : ");
  }

  // calcul de la tension en sortie du pont de resistance
  // utilisation d'un pont de resistances : voutBat = vBat * R2 / R1 + R2
  // voutBat correspond à la tension de la batterie
  vBat = ((voutBat * (R1 + R2)) / R2) + correction + tensionDiode;
  if (SerialMonitor) {
    Serial.println(vBat);
  }
  return (vBat);
}

//=======
// SETUP
//=======
void setup() {
  // broche done du tpl5110
  pinMode(DONEPIN, OUTPUT);
  digitalWrite(DONEPIN, LOW);

  Serial.begin(SERIAL_BAUD);
  pinMode(AnGpio, INPUT); // anGpio lecture de la tension de la batterie
  //pinMode(LED_BOARD,OUTPUT); // led de la carte
  /*
    // Start up the library
    sensors.begin();                            // On initialise la bibliothèque Dallas
    // report parasite power requirements
    Serial.print("Parasite power is: ");
    if (sensors.isParasitePowerMode()) Serial.println("ON");
    else Serial.println("OFF");
  */

  // initialisation load cell
  scale.begin(DOUT, CLK);      // loadcell hx711 broches DOUT et CLK
  scale.power_down();          // put the ADC in sleep mode
  delay(2000);
  scale.power_up();            // reveil du hx711
  delay(400);
  scale.set_offset(LOADCELL_OFFSET);     // offset
  scale.set_scale(calibration_factor);   // Calibration Factor obtained from first sketch (divider)
  //scale.tare(); // Reset the scale to 0

  // Then, it calls several methods that you can use to get readings using the library.
  // read(): gets a raw reading from the sensor
  // read_average(number of readings): gets the average of the latest defined number of readings
  // get_value(number of readings): gets the average of the last defined number of readings minus the tare weight;
  // get_units(number of readings): gets the average of the last defined number of readings minus the tare weight divided by the calibration factor
  // this will output a reading in your desired units.

  //rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);

#if oled
  startOLED();
#endif
  startLoRA();
  if (SerialMonitor) {
    Serial.print("Synchro lora : ");
    Serial.println(synchroLora);
  }

  //----------------------
  // ruche jlm autonome
  //----------------------
#if RUCHE_NUMERO == 04 or RUCHE_NUMERO == 05
  if (!bme280.begin(ADDRESS)) {      // initialisation si bme280 present. 0x76 adresse i2c bme280
    if (SerialMonitor) {
      Serial.println("Could not find a valid BME280 sensor, check wiring!");
    }
  } else {
    // compensation de la lecture de la temperature du bme280
    bme280.setTemperatureCompensation(COMPENSATION);
    if (SerialMonitor) {
      Serial.print("compensation bme280 : ");
      Serial.println(COMPENSATION);
    }
  }

  interrupteur.begin(); // bibliotheque interrupteur pour eviter les rebonds.
#endif
  if (SerialMonitor) {
    Serial.println("Demarrage OK");
  }

  //digitalWrite(LED_BOARD, HIGH); //allumage de la led de la carte
  getReadings();   // lecture des donnees
  //digitalWrite(LED_BOARD, LOW);

  //----------------------
  // ruche jlm autonome
  //----------------------
#if RUCHE_NUMERO == 04 or RUCHE_NUMERO == 05
  getSondes_bme280();  // bme280
  chaine_bme280();
  delay(100);
  etatInterrupteur();  // etat de l'interrupteur
  coupureEnergie ();   // sauvegarde de counter_ID en cas de coupure de l'energie
#endif

  sendReadings();      // envoi des donnees
  delay(300);

  scale.power_down();     // put the ADC du hx711 in sleep mode
  if (SerialMonitor) {
    Serial.println("ADC HX711 Going to sleep mode now");
  }

  LoRa.sleep();           // sleep mode
  Serial.println("LoRa Going to sleep mode now");

  //Increment boot number and print it every reboot
  ++bootCount;
  // suivi chronologique du wakeup
  if (bootCount >= 9) {
    bootCount =  0;
  }
  if (SerialMonitor) {
    Serial.println("Boot number: " + String(bootCount));
  }

  //Print the wakeup reason for ESP32
  print_wakeup_reason();

  delay(200);

  // toggle DONE HIGH to cut power with TPL5110
  digitalWrite(DONEPIN, HIGH); // enclenche l'arret du tpl5110
  delay(100);
  digitalWrite(DONEPIN, LOW);

  //-------------------------------------
  // First we configure the wake up source
  // We set our ESP32 to wake up every 20 seconds
  //----------------------------------------
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  if (SerialMonitor) {
    Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");
  }

  //---------------------------------------------------------------------
  //  Next we decide what all peripherals to shut down/keep on
  //  By default, ESP32 will automatically power down the peripherals
  //  not needed by the wakeup source, but if you want to be a poweruser
  //  this is for you. Read in detail at the API docs
  //  http://esp-idf.readthedocs.io/en/latest/api-reference/system/deep_sleep.html
  //  Left the line commented as an example of how to configure peripherals.
  //  The line below turns off all RTC peripherals in deep sleep.
  //----------------------------------------------------------------------
  //  esp_deep_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
  //  Serial.println("Configured all RTC Peripherals to be powered down in sleep");

  //----------------------------------------------------------
  //  Now that we have setup a wake cause and if needed setup the
  //  peripherals state in deep sleep, we can now start going to
  //  deep sleep.
  //  In the case that no wake up sources were provided but deep
  //  sleep was started, it will sleep forever unless hardware
  //  reset occurs.
  //------------------------------------------------------------
  if (SerialMonitor) {
    Serial.println("esp32 Going to sleep now");
    Serial.println();
  }
  delay(500);
  Serial.flush();
  esp_deep_sleep_start();
  if (SerialMonitor) {
    Serial.println("This will never be printed");
  }
}

//======
// LOOP
//======
void loop() {
  // This is not going to be called
  // utilise pour le reglage de l'offset et de la calibration de la charge
}
