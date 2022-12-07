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

//Libraries for OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// HX711 circuit wiring
#define DOUT  23
#define CLK  25

HX711 scale;  // objet scale

//=================================
// structure de donnees d'une ruche
//=================================
struct ruche {
  int numRuche;  // numero de la ruche
  float poids;   // poids de la ruche
  float tempe;   // temperature de la ruche
  float vBat;  // tension batterie
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

//==========
// OLED pins
//==========
#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);


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

RTC_DATA_ATTR int bootCount = 0; // sauvegarde de la variable avant le mode sleep dans la memoire rtc de l'esp32

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
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);     // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("PROJET LORA SENDER");
}

//========================
// Initialize LoRa module
//========================
void startLoRA() {
  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);

  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);

  while (!LoRa.begin(BAND) && counter < 10) {
    Serial.print(".");
    counter++;
    delay(500);
  }
  if (counter == 10) {
    Serial.println("Starting LoRa failed!");
    display.setCursor(0, 10);
    display.clearDisplay();
    display.print("Starting LoRa failed!");
    display.display();

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

    Serial.print("txPower : ");
    Serial.println(txPower);
    LoRa.setTxPower(txPower);
    //LoRa.setTxPower(txPower,PABOOST);

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

  delay(2000);
}

//===============================================
// Write LoRa Send
//===============================================
void sendReadings() {
  // message a envoyer
  LoRaMessage = String(readingID) + "/" + String(Ruche.tempe) + "&" + String(Ruche.numRuche) + "#" + String(Ruche.poids) + "{" + String(Ruche.vBat);
  String message = LoRaMessage;
  //Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.print(LoRaMessage);
  LoRa.endPacket();

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
  //Serial.println();

  delay(3000);

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
  Ruche.poids = scale.get_units(numberOfReadings); // numberOfReadings readings from the ADC minus tare weight
  delay(200);
  RucheControl.poids = scale.get_units(numberOfReadings); // numberOfReadings readings from the ADC minus tare weight
  // test + ou - poidsAberrant pour eviter les mesure aberrantes
  if ((Ruche.poids > (RucheControl.poids + poidsAberrant)) or (Ruche.poids < (RucheControl.poids - poidsAberrant ))) {
    delay(200);
    Ruche.poids = scale.get_units(numberOfReadings); // numberOfReadings readings from the ADC minus tare weight
  }

  // Lecture de la temperature ambiante a ~1Hz
  if (getTemperature(&Ruche.tempe, true) != READ_OK) {
    Serial.println(F("Erreur de lecture du capteur"));
  }
  delay(200);
  if (getTemperature(&RucheControl.tempe, true) != READ_OK) {
    Serial.println(F("Erreur de lecture du capteur"));
  }
  // test + ou - temperatureAberrante pour eviter les mesure aberrantes
  if ((Ruche.tempe > (RucheControl.tempe + temperatureAberrante)) or (Ruche.tempe < (RucheControl.tempe - temperatureAberrante))) {
    delay(200);
    if (getTemperature(&Ruche.tempe, true) != READ_OK) {
      Serial.println(F("Erreur de lecture du capteur"));
    }
  }

  Ruche.vBat = tensionBatterie();
  delay(20);
  RucheControl.vBat = tensionBatterie();
  // test tension aberrante
  if ((Ruche.vBat > (RucheControl.vBat + tensionAberrante)) or (Ruche.vBat < (RucheControl.vBat - tensionAberrante))) {
    delay(20);
    Ruche.vBat = tensionBatterie();
  }

  Serial.print("Weight: ");
  Serial.print(Ruche.poids, 3); //Up to 3 decimal points
  Serial.println(" kg"); //Change this to kg and re-adjust the calibration factor if you follow lbs

  // sur le moniteur serie
  Serial.println("Press t to tare");
  //Serial.println("Press T or t to tare");
  if (Serial.available()) {
    char temporaire = Serial.read();
    //if (temporaire == 't' || temporaire == 'T') scale.tare(); //Reset the scale to zero
    if (temporaire == 't') scale.tare();    //Reset the scale to zero
  }
}
float tensionBatterie() {
  // mesure tension d'alimentation environ 12v
  int AnGpioResult = analogRead(AnGpio);
  // calcul du resultat en volt
  float voutBat = ((AnGpioResult * tensionEsp32) / cad) + offsetCalcule;
  // calcul de la tension en sortie du pont de resistance
  // utilisation d'un pont de resistances : voutBat = vBat * R2 / R1 + R2. vBat correspond à la tension de la batterie
  return ((voutBat * (R1 + R2)) / R2) + correction + tensionDiode;
}

//=======
// SETUP
//=======
void setup() {
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
  scale.begin(DOUT, CLK);
  scale.set_offset(LOADCELL_OFFSET);     // offset
  scale.set_scale(calibration_factor);   // Calibration Factor obtained from first sketch (divider)
  //scale.tare(); // Reset the scale to 0
  /*
    Then, it calls several methods that you can use to get readings using the library.
    read(): gets a raw reading from the sensor
    read_average(number of readings): gets the average of the latest defined number of readings
    get_value(number of readings): gets the average of the last defined number of readings minus the tare weight;
    get_units(number of readings): gets the average of the last defined number of readings minus the tare weight divided by the calibration factor
    this will output a reading in your desired units.
  */
  //rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);

  startOLED();
  startLoRA();

  Serial.println("Demarrage OK");
  delay (300);
  //digitalWrite(LED_BOARD, HIGH); //allumage de la led de la carte
  getReadings();   // lecture des donnees
  sendReadings();  // envoi des donnees
  //digitalWrite(LED_BOARD, LOW);

  scale.power_down();     // put the ADC du hx711 in sleep mode
  Serial.println("ADC HX711 Going to sleep mode now");

  LoRa.sleep();           // sleep mode
  Serial.println("LoRa Going to sleep mode now");

  //Increment boot number and print it every reboot
  ++bootCount;
  // suivi chronologique du wakeup
  if (bootCount >= 10) {
    bootCount =  0;
  }
  Serial.println("Boot number: " + String(bootCount));

  //Print the wakeup reason for ESP32
  print_wakeup_reason();

  //-------------------------------------
  // First we configure the wake up source
  // We set our ESP32 to wake up every 20 seconds
  //----------------------------------------
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");

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
  Serial.println("esp32 Going to sleep now");
  Serial.println();
  delay(1000);
  Serial.flush();
  esp_deep_sleep_start();
  Serial.println("This will never be printed");

}
//======
// LOOP
//======
void loop() {
  // This is not going to be called
  // utilise pour le reglage de l'offset et de la calibration de la charge
  /*
    long now = millis();

    //Affichage du poids et de la temperature toutes les 10 secondes
    if (now - lastMsg > 1000 * 10) {
      lastMsg = now;
      getReadings();
      sendReadings();

        Serial.println("Before setting up the scale:");
        Serial.print("read: \t\t");
        Serial.println(scale.read());      // print a raw reading from the ADC

        Serial.print("read average: \t\t");
        Serial.println(scale.read_average(20));   // print the average of 20 readings from the ADC

        Serial.print("get value: \t\t");
        Serial.println(scale.get_value(5));   // print the average of 5 readings from the ADC minus the tare weight (not set yet)

        Serial.print("get units: \t\t");
        Serial.println(scale.get_units(5), 3); // print the average of 5 readings from the ADC minus tare weight (not set) divided

    }
  */
}

/*
   //https://github.com/milesburton/Arduino-Temperature-Control-Library/blob/master/examples/Single/Single.ino
   //https://github.com/milesburton/Arduino-Temperature-Control-Library/tree/master/examples/Simple
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
  float tempC = sensors.getTempCByIndex(0);

  // Check if reading was successful
  if (tempC != DEVICE_DISCONNECTED_C)
  {
    Serial.print("Temperature for the device 1 (index 0) is: ");
    Serial.println(tempC);
  }
  else
  {
    Serial.println("Error: Could not read temperature data");
  }


  // Affichage de la température
  Serial.print(F("Temperature : "));
  Serial.print(temperature, 1);
  Serial.write(176); // Caractère degré
  Serial.write('C');
  Serial.println();

*/
