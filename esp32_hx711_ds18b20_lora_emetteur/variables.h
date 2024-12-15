//=======================================================
// Use this file to store all of the private credentials
// and connection details
//=======================================================

//=======================================================
//Controls supression of the MonPrintf function to serial
//=======================================================
#define SERIAL_BAUD 115200    // vitesse moniteur serie
#define SerialMonitor  1      // 1 true, 0 false
#define oled  0               // oled 1 : afficheur present

//=============
//Numero Ruche
//=============
//#define RUCHE_NUMERO  01      // numero de la ruche jlm1
//#define RUCHE_NUMERO  02      // numero de la ruche jlm2
//#define RUCHE_NUMERO  03      // numero de la ruche jlm3
//#define RUCHE_NUMERO  04      // numero de la ruche jlm4
//#define RUCHE_NUMERO  05      // numero de la ruche jlm5 autonome
//#define RUCHE_NUMERO  06      // numero de la ruche jlm5 autonome
#define RUCHE_NUMERO  07      // numero de la ruche jlm7

//#define RUCHE_NUMERO  11      // numero de la ruche loic1
//#define RUCHE_NUMERO  12      // numero de la ruche loic2
//#define RUCHE_NUMERO  13      // numero de la ruche loic3
//#define RUCHE_NUMERO  14      // numero de la ruche loic4
//#define RUCHE_NUMERO  15      // numero de la ruche loic5
//#define RUCHE_NUMERO  16      // numero de la ruche loic6
//#define RUCHE_NUMERO  17      // numero de la ruche loic7
//#define RUCHE_NUMERO  18      // numero de la ruche loic8
//#define RUCHE_NUMERO  19      // numero de la ruche loic9

//=======
//TPL5110
//=======
#define DONEPIN  32  // done du tpl5110 en gipo32

//=====================
//Load Cell with HX711
//=====================

const int numberOfReadings = 20;
const int poidsAberrant = 12;      // test du poids aberrant pour relancer une mesure

// Change sync word (0xF3) to match the receiver
// The sync word assures you don't get LoRa messages from other LoRa transceivers
// ranges from 0-0xFF
byte synchroLora = 0xF3;

// Change this calibration factor as per your load cell once it is found you many need to vary it in thousands
// mettre a 0 pour avoir la valeur au demarrage de la balance sans charge
// mettre ensuite la valeur retournee pour obtenir une tare fictive.
// calibration
#if RUCHE_NUMERO == 01
// pour ruche jlm1 rfm95
float calibration_factor = -20332;       // calibration factor   -20332

#elif RUCHE_NUMERO == 02
// pour ruche jlm2 rfm95
float calibration_factor = -20200;       // calibration factor   -20200

#elif RUCHE_NUMERO == 03
// pour ruche jlm3 rfm95
float calibration_factor = -20900;       // calibration factor   -20900

#elif RUCHE_NUMERO == 04
// pour ruche jlm4 rfm95
float calibration_factor = -19900;       // calibration factor   -19900

#elif RUCHE_NUMERO == 05
// pour ruche jlm5 rfm95 autonome
float calibration_factor = -21450;       // calibration factor    -21450

#elif RUCHE_NUMERO == 06
// pour ruche jlm6 rfm95 autonome
float calibration_factor = -20000;       // calibration factor    -20000

#elif RUCHE_NUMERO == 07
// pour ruche jlm7 rfm95
float calibration_factor = -21800;       // calibration factor   -21800


#elif RUCHE_NUMERO == 11
// pour ruche loic1 rfm95
float calibration_factor = -20100;       // calibration factor   -20100

#elif RUCHE_NUMERO == 12
// pour ruche loic2 rfm95
float calibration_factor = -22100;       // calibration factor   -22100

#elif RUCHE_NUMERO == 13
// pour ruche loic3 rfm95
float calibration_factor = -20800;       // calibration factor   -20800

#elif RUCHE_NUMERO == 14
// pour ruche loic4 rfm95
float calibration_factor = -20500;       // calibration factor   -20500

#elif RUCHE_NUMERO == 15
// pour ruche loic5 rfm95
float calibration_factor = -20400;       // calibration factor   -20400

#elif RUCHE_NUMERO == 16
// pour ruche loic6 rfm95
float calibration_factor = -22900;       // calibration factor   -22900

#elif RUCHE_NUMERO == 17
// pour ruche loic7 rfm95
float calibration_factor = -19800;       // calibration factor   -19800

#elif RUCHE_NUMERO == 18
// pour ruche loic8 rfm95
float calibration_factor = -20500;       // calibration factor   -19800

#elif RUCHE_NUMERO == 19
// pour ruche loic9 rfm95
float calibration_factor = -20200;       // calibration factor   -20500
#endif

// offset
#if RUCHE_NUMERO == 01
// pour ruche jlm1 rfm95
const long LOADCELL_OFFSET = -104200;    // offset    -104200

#elif RUCHE_NUMERO == 02
// pour ruche jlm2 rfm95
const long LOADCELL_OFFSET = -38308;      // offset  -38308

#elif RUCHE_NUMERO == 03
// pour ruche jlm3 rfm95
const long LOADCELL_OFFSET = -80400;      // offset  -80400

#elif RUCHE_NUMERO == 04
// pour ruche jlm4 rfm95
const long LOADCELL_OFFSET = -114050;      // offset -114050

#elif RUCHE_NUMERO == 05
// pour ruche jlm5 rfm95 autonome
const long LOADCELL_OFFSET = -159508;        // offset -159508

#elif RUCHE_NUMERO == 06
// pour ruche jlm6 rfm95 autonome
const long LOADCELL_OFFSET = -78000;        // offset -78000

#elif RUCHE_NUMERO == 07
// pour ruche jlm7 rfm95
const long LOADCELL_OFFSET = -40572;        // offset -40572


#elif RUCHE_NUMERO == 11
// pour ruche loic1 rfm95
const long LOADCELL_OFFSET = -22680;      // offset   -22680

#elif RUCHE_NUMERO == 12
// pour ruche loic2 rfm95
const long LOADCELL_OFFSET = -125750;     // offset   -125750

#elif RUCHE_NUMERO == 13
// pour ruche loic3 rfm95
const long LOADCELL_OFFSET = -82700;      // offset   -82700

#elif RUCHE_NUMERO == 14
// pour ruche loic4 rfm95
const long LOADCELL_OFFSET = -95850;      // offset   -95650

#elif RUCHE_NUMERO == 15
// pour ruche loic5 rfm95
const long LOADCELL_OFFSET = -139000;     // offset   -139000

#elif RUCHE_NUMERO == 16
// pour ruche loic6 rfm95
const long LOADCELL_OFFSET = -102500;     // offset   -102500

#elif RUCHE_NUMERO == 17
// pour ruche loic7 rfm95
const long LOADCELL_OFFSET = -86750;      // offset   -86750

#elif RUCHE_NUMERO == 18
// pour ruche loic8 rfm95
const long LOADCELL_OFFSET = -78300;      // offset   -86750

#elif RUCHE_NUMERO == 19
// pour ruche loic9 rfm95
const long LOADCELL_OFFSET = -59140;      // offset   -95850
#endif

//==============
//Time to Sleep
//==============
// nombres premiers 41, 61 , 127, 181, 241, 307, 313, 331, 601, 607, 613, 617, 619, 631, 907
// nombres premiers 577, 587, 593, 599, 641, 643, 647, 653, 659, 661, 673, 677, 683, 691, 701
#if RUCHE_NUMERO == 01
#define TIME_TO_SLEEP   331    // Time ESP32 will go to sleep (in seconds)

#elif RUCHE_NUMERO == 02
#define TIME_TO_SLEEP   241    // Time ESP32 will go to sleep (in seconds)

#elif RUCHE_NUMERO == 03
#define TIME_TO_SLEEP   307    // Time ESP32 will go to sleep (in seconds)

#elif RUCHE_NUMERO == 04
#define TIME_TO_SLEEP   313    // Time ESP32 will go to sleep (in seconds)

#elif RUCHE_NUMERO == 05
#define TIME_TO_SLEEP   61    // Time ESP32 will go to sleep (in seconds)

#elif RUCHE_NUMERO == 06
#define TIME_TO_SLEEP   61    // Time ESP32 will go to sleep (in seconds)

#elif RUCHE_NUMERO == 07
#define TIME_TO_SLEEP   587    // Time ESP32 will go to sleep (in seconds)


#elif RUCHE_NUMERO == 11
#define TIME_TO_SLEEP  601     // Time ESP32 will go to sleep (in seconds)

#elif RUCHE_NUMERO == 12
#define TIME_TO_SLEEP   613    // Time ESP32 will go to sleep (in seconds)

#elif RUCHE_NUMERO == 13
#define TIME_TO_SLEEP   631    // Time ESP32 will go to sleep (in seconds)

#elif RUCHE_NUMERO == 14
#define TIME_TO_SLEEP   641    // Time ESP32 will go to sleep (in seconds)

#elif RUCHE_NUMERO == 15
#define TIME_TO_SLEEP   599   // Time ESP32 will go to sleep (in seconds)

#elif RUCHE_NUMERO == 16
#define TIME_TO_SLEEP   593    // Time ESP32 will go to sleep (in seconds)

#elif RUCHE_NUMERO == 17
#define TIME_TO_SLEEP   587    // Time ESP32 will go to sleep (in seconds)

#elif RUCHE_NUMERO == 18
#define TIME_TO_SLEEP   577    // Time ESP32 will go to sleep (in seconds)

#elif RUCHE_NUMERO == 19
#define TIME_TO_SLEEP   607    // Time ESP32 will go to sleep (in seconds)
#endif

//----------------------
// ruche jlm autonome
//----------------------
#if RUCHE_NUMERO == 05 or RUCHE_NUMERO == 06
//==================================
// structure de donnees des Capteurs
//==================================

struct boitierCapteur {
  int numBoitierCapteur;     // numero du boitier
  String nameBoitierCapteur; // nom du boitier
  bool interrupteur;         // interrupteur
  float tempeDS18B20;        // temperature DS18B20
  float vBat;                // tension batterie
  float poids;               // poids
  
};

struct capteur_bme280 {
  String tempe;          // temperature
  String humi;           // humidite
  String pression;       // pression
  String hum_stat;       // Humidity status: 0 Normal, 1 Comfort, 2 Dry, 3 Wet
  String bar_for;        // Forecast: 0 None, 1 Sunny, 2 PartlyCloudy, 3 Cloudy, 4 Rain
};

//==============
// Sonde BME280
//==============
#define ADDRESS 0x76                                  // premiere partie adresse ic2 bme280
#define tempsPause 30                                 // Nbre de secondes de pause (3600 = 1H00)
#define SEALEVELPRESSURE_HPA (1013.25)                // pression au niveau de la mer, reference
//----------------------
// ruche jlm autonome
//----------------------
#if RUCHE_NUMERO == 05
#define COMPENSATION -1.20                            // compensation de la valeur de temperature du bme280
#elif RUCHE_NUMERO == 06
#define COMPENSATION -1.20                            // compensation de la valeur de temperature du bme280
#endif
float temp = 0.0;                                     // Variables contenant la valeur de température de la sonde bme280.
float hum = 0.0;                                      // Valeurs contenant la valeur d'humidite de la sonde bme280.
int   hum_stat = 0;                                   // Humidity status: 0 - Normal, 1 - Comfort, 2 - Dry, 3 - Wet, de la sonde bme280
float  pre = 0.0;                                     // pression de la sonde bme280
int  preStep = 0;                                     // pression de la sonde bme280 stepper
int  memPreStep = 0;                                  // pression de la sonde bme280 stepper memorisation
int bar_for = 0;                                      // Forecast: 0 - None, 1 - Sunny, 2 - PartlyCloudy, 3 - Cloudy, 4 - Rain, de la sonde bme280
float alt = 0.0;                                      // altitude pour les calculs avec la sonde bme280

//========
// Sondes
//========
// chaine (tableau de caracteres) pour l'envoi des données des sondes
const int taille = 30;                                // taille du tableau pour l'envoi des valeurs des sondes
char chaine[taille - 1] = "" ;                        // tableau pour l'envoi des valeurs des sondes
int num_sensors;                                      // Create variables to hold the device addresses
#endif


//====================
// Temperature ds18b20
//====================
const int temperatureAberrante = 6; //test de le temperature aberrante pour relancer une mesure

//====================
//Battery calibration
//====================
int cad = 4095 ;                        // pas du convertisseur
float tensionEsp32 = 3.3 ;              // tension de reference
const float numberOfReadingsBat = 20.0; // nombre de lectures du port externe
// float analogReadReference  = 1.1 ;   // reference theorique
float mesureTension = 0.0;

#if RUCHE_NUMERO == 01
// batterie 12v
int R1 = 100000;                      // resistance r1 du pont
int R2 = 22000;                       // resistance r2 du pont
float correction = 0.15 ;             // correction
float offsetCalcule = 0.226;          // offset mesure par voltmetre
float tensionDiode = 0.74 ;           // tension de la diode de protection invertion 1n4007
const int tensionFaible = 10;         // tension faible
const int tensionElevee = 16;         // tension elevee

#elif RUCHE_NUMERO == 02
// batterie 12v
int R1 = 100000;                      // resistance r1 du pont
int R2 = 22000;                       // resistance r2 du pont
float correction = -0.35 ;            // correction
float offsetCalcule = 0.226;          // offset mesure par voltmetre
float tensionDiode = 0.74 ;           // tension de la diode de protection invertion 1n4007
const int tensionFaible = 10;         // tension faible
const int tensionElevee = 16;         // tension elevee

#elif RUCHE_NUMERO == 03
// batterie 12v
int R1 = 100000;                      // resistance r1 du pont
int R2 = 22000;                       // resistance r2 du pont
float correction = -0.15 ;            // correction
float offsetCalcule = 0.226;          // offset mesure par voltmetre
float tensionDiode = 0.74 ;           // tension de la diode de protection invertion 1n4007
const int tensionFaible = 10;         // tension faible
const int tensionElevee = 16;         // tension elevee

#elif RUCHE_NUMERO == 04
// batterie 12v
int R1 = 100000;                      // resistance r1 du pont
int R2 = 22000;                       // resistance r2 du pont
float correction = -0.35 ;            // correction
float offsetCalcule = 0.226;          // offset mesure par voltmetre
float tensionDiode = 0.74 ;           // tension de la diode de protection invertion 1n4007
const int tensionFaible = 10;         // tension faible
const int tensionElevee = 16;         // tension elevee

#elif RUCHE_NUMERO == 05
// batterie 5v autonome
float offsetCalcule = 0.16;           // offset mesure par voltmetre gpio35 sans le tpl5110
int R1 = 27000;                       // resistance r1 du pont
int R2 = 10000;                       // resistance r2 du pont
float correction = -0.18;             // correction resistances - mesure en drv sans le tpl5110
float tensionDiode = 0.21;            // tension tpl5110 regulateur diode - en reliant vdd et drv sans le tpl5110
const int tensionFaible = 3;          // tension faible
const int tensionElevee = 7;          // tension elevee

#elif RUCHE_NUMERO == 06
// batterie 5v autonome
int R1 = 27000;                       // resistance r1 du pont
int R2 = 10000;                       // resistance r2 du pont
float correction = -0.15;             // correction
float offsetCalcule = 0.226;          // offset mesure par voltmetre
float tensionDiode = 0.80 ;           // tension tpl5110 regulateur diode
const int tensionFaible = 3;          // tension faible
const int tensionElevee = 7;         // tension elevee

#elif RUCHE_NUMERO == 07
// batterie 12v
int R1 = 100000;                      // resistance r1 du pont
int R2 = 22000;                       // resistance r2 du pont
float correction = -0.55 ;            // correction
float offsetCalcule = 0.226;          // offset mesure par voltmetre
float tensionDiode = 0.74 ;           // tension de la diode de protection invertion 1n4007
const int tensionFaible = 10;         // tension faible
const int tensionElevee = 16;         // tension elevee


#elif RUCHE_NUMERO == 11
// batterie 12v
int R1 = 100000;                      // resistance r1 du pont
int R2 = 22000;                       // resistance r2 du pont
float correction = -0.35 ;            // correction
float offsetCalcule = 0.226;          // offset mesure par voltmetre
float tensionDiode = 0.74 ;           // tension de la diode de protection invertion 1n4007
const int tensionFaible = 10;         // tension faible
const int tensionElevee = 16;         // tension elevee

#elif RUCHE_NUMERO == 12
// batterie 12v
int R1 = 100000;                      // resistance r1 du pont
int R2 = 22000;                       // resistance r2 du pont
float correction = -0.35 ;            // correction
float offsetCalcule = 0.226;          // offset mesure par voltmetre
float tensionDiode = 0.74 ;           // tension de la diode de protection invertion 1n4007
const int tensionFaible = 10;         // tension faible
const int tensionElevee = 16;         // tension elevee

#elif RUCHE_NUMERO == 13
// batterie 12v
int R1 = 100000;                      // resistance r1 du pont
int R2 = 22000;                       // resistance r2 du pont
float correction = -0.35 ;            // correction
float offsetCalcule = 0.226;          // offset mesure par voltmetre
float tensionDiode = 0.74 ;           // tension de la diode de protection invertion 1n4007
const int tensionFaible = 10;         // tension faible
const int tensionElevee = 16;         // tension elevee

#elif RUCHE_NUMERO == 14
// batterie 12v
int R1 = 100000;                      // resistance r1 du pont
int R2 = 22000;                       // resistance r2 du pont
float correction = -0.35 ;            // correction
float offsetCalcule = 0.226;          // offset mesure par voltmetre
float tensionDiode = 0.74 ;           // tension de la diode de protection invertion 1n4007
const int tensionFaible = 10;         // tension faible
const int tensionElevee = 16;         // tension elevee

#elif RUCHE_NUMERO == 15
// batterie 12v
int R1 = 100000;                      // resistance r1 du pont
int R2 = 22000;                       // resistance r2 du pont
float correction = -0.35 ;            // correction
float offsetCalcule = 0.226;          // offset mesure par voltmetre
float tensionDiode = 0.74 ;           // tension de la diode de protection invertion 1n4007
const int tensionFaible = 10;         // tension faible
const int tensionElevee = 16;         // tension elevee

#elif RUCHE_NUMERO == 16
// batterie 12v
int R1 = 100000;                      // resistance r1 du pont
int R2 = 22000;                       // resistance r2 du pont
float correction = -0.25 ;            // correction
float offsetCalcule = 0.226;          // offset mesure par voltmetre
float tensionDiode = 0.74 ;           // tension de la diode de protection invertion 1n4007
const int tensionFaible = 10;         // tension faible
const int tensionElevee = 16;         // tension elevee

#elif RUCHE_NUMERO == 17
// batterie 12v
int R1 = 100000;                      // resistance r1 du pont
int R2 = 22000;                       // resistance r2 du pont
float correction = 0.35 ;             // correction
float offsetCalcule = 0.226;          // offset mesure par voltmetre
float tensionDiode = 0.74 ;           // tension de la diode de protection invertion 1n4007
const int tensionFaible = 10;         // tension faible
const int tensionElevee = 16;         // tension elevee

#elif RUCHE_NUMERO == 18
// batterie 12v
int R1 = 100000;                      // resistance r1 du pont
int R2 = 22000;                       // resistance r2 du pont
float correction = -0.35 ;            // correction
float offsetCalcule = 0.226;          // offset mesure par voltmetre
float tensionDiode = 0.74 ;           // tension de la diode de protection invertion 1n4007
const int tensionFaible = 10;         // tension faible
const int tensionElevee = 16;         // tension elevee

#elif RUCHE_NUMERO == 19
// batterie 12v
int R1 = 100000;                      // resistance r1 du pont
int R2 = 22000;                       // resistance r2 du pont
float correction = -2.50 ;            // correction
float offsetCalcule = 0.226;          // offset mesure par voltmetre
float tensionDiode = 0.74 ;           // tension de la diode de protection invertion 1n4007
const int tensionFaible = 10;         // tension faible
const int tensionElevee = 16;         // tension elevee
#endif
