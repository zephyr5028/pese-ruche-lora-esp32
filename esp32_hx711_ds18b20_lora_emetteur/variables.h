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
#define debug 1               // true for use Display log message

//=============
//Numero Ruche
//=============
//#define RUCHE_NUMERO  01      // numero de la ruche jlm1
//#define RUCHE_NUMERO  02      // numero de la ruche jlm2
//#define RUCHE_NUMERO  03      // numero de la ruche jlm3
//#define RUCHE_NUMERO  04      // numero de la ruche jlm4
//#define RUCHE_NUMERO  05      // numero de la ruche jlm5 autonome
//#define RUCHE_NUMERO  06      // numero de la ruche jlm5 autonome
//#define RUCHE_NUMERO  07      // numero de la ruche jlm7
//#define RUCHE_NUMERO  8       // numero de la ruche jlm8 autonome si 08, le 0 defini un nombre octal et 08 octal n'est pas correct

//#define RUCHE_NUMERO  11      // numero de la ruche loic11
//#define RUCHE_NUMERO  12      // numero de la ruche loic12
//#define RUCHE_NUMERO  13      // numero de la ruche loic13
//#define RUCHE_NUMERO  14      // numero de la ruche loic14
//#define RUCHE_NUMERO  15      // numero de la ruche loic15
//#define RUCHE_NUMERO  16      // numero de la ruche loic16
//#define RUCHE_NUMERO  17      // numero de la ruche loic17
//#define RUCHE_NUMERO  18      // numero de la ruche loic18
//#define RUCHE_NUMERO  19      // numero de la ruche loic19
//#define RUCHE_NUMERO  21      // numero de la ruche loic21
//#define RUCHE_NUMERO  22      // numero de la ruche loic22
//#define RUCHE_NUMERO  23      // numero de la ruche loic23
#define RUCHE_NUMERO 24      // numero de la ruche loic24 autonome


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

#elif RUCHE_NUMERO == 8
// pour ruche jlm8 rfm95 autonome
float calibration_factor = -20000;       // calibration factor    -20000


#elif RUCHE_NUMERO == 11
// pour ruche loic11 rfm95
float calibration_factor = -20100;       // calibration factor   -20100

#elif RUCHE_NUMERO == 12
// pour ruche loic12 rfm95
float calibration_factor = -22100;       // calibration factor   -22100

#elif RUCHE_NUMERO == 13
// pour ruche loic13 rfm95
float calibration_factor = -20800;       // calibration factor   -20800

#elif RUCHE_NUMERO == 14
// pour ruche loic14 rfm95
float calibration_factor = -20500;       // calibration factor   -20500

#elif RUCHE_NUMERO == 15
// pour ruche loic15 rfm95
float calibration_factor = -20400;       // calibration factor   -20400

#elif RUCHE_NUMERO == 16
// pour ruche loic16 rfm95
float calibration_factor = -22900;       // calibration factor   -22900

#elif RUCHE_NUMERO == 17
// pour ruche loic17 rfm95
float calibration_factor = -19800;       // calibration factor   -19800

#elif RUCHE_NUMERO == 18
// pour ruche loic18 rfm95
float calibration_factor = -20500;       // calibration factor   -19800

#elif RUCHE_NUMERO == 19
// pour ruche loic19 rfm95
float calibration_factor = -20200;       // calibration factor   -20500

#elif RUCHE_NUMERO == 21
// pour ruche loic21 rfm95
float calibration_factor = -21300;       // calibration factor   -221300

#elif RUCHE_NUMERO == 22
// pour ruche loic22 rfm95
float calibration_factor = -17800;       // calibration factor   -17800

#elif RUCHE_NUMERO == 23
// pour ruche loic23 rfm95
float calibration_factor = -18400;       // calibration factor   -18400

#elif RUCHE_NUMERO == 24
// pour ruche loic24 rfm95 autonome
float calibration_factor = -18400;       // calibration factor   -18400
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

#elif RUCHE_NUMERO == 8
// pour ruche jlm8 rfm95 autonome
const long LOADCELL_OFFSET = -78000;        // offset -78000


#elif RUCHE_NUMERO == 11
// pour ruche loic11 rfm95
const long LOADCELL_OFFSET = -22680;      // offset   -22680

#elif RUCHE_NUMERO == 12
// pour ruche loic12 rfm95
const long LOADCELL_OFFSET = -125750;     // offset   -125750

#elif RUCHE_NUMERO == 13
// pour ruche loic13 rfm95
const long LOADCELL_OFFSET = -82700;      // offset   -82700

#elif RUCHE_NUMERO == 14
// pour ruche loic14 rfm95
const long LOADCELL_OFFSET = -95850;      // offset   -95650

#elif RUCHE_NUMERO == 15
// pour ruche loic15 rfm95
const long LOADCELL_OFFSET = -139000;     // offset   -139000

#elif RUCHE_NUMERO == 16
// pour ruche loic16 rfm95
const long LOADCELL_OFFSET = -102500;     // offset   -102500

#elif RUCHE_NUMERO == 17
// pour ruche loic17 rfm95
const long LOADCELL_OFFSET = -86750;      // offset   -86750

#elif RUCHE_NUMERO == 18
// pour ruche loic18 rfm95
const long LOADCELL_OFFSET = -78300;      // offset   -86750

#elif RUCHE_NUMERO == 19
// pour ruche loic19 rfm95
const long LOADCELL_OFFSET = -59140;      // offset   -95850

#elif RUCHE_NUMERO == 21
// pour ruche loic21 rfm95
const long LOADCELL_OFFSET = -183385;      // offset   -183385
#elif RUCHE_NUMERO == 22
// pour ruche loic22 rfm95
const long LOADCELL_OFFSET = -160615;      // offset   -160615

#elif RUCHE_NUMERO == 23
// pour ruche loic23 rfm95
const long LOADCELL_OFFSET = -117222;      // offset   -117222

#elif RUCHE_NUMERO == 24
// pour ruche loic24 rfm95 autonome
const long LOADCELL_OFFSET = -117222;      // offset   -117222
#endif

//==============
//Time to Sleep
//==============
// nombres premiers 41, 61 , 127, 181, 241, 307, 313, 331, 337, 347, 601, 607, 613, 617, 619, 631, 907
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
#define TIME_TO_SLEEP   331    // Time ESP32 will go to sleep (in seconds)

#elif RUCHE_NUMERO == 07
#define TIME_TO_SLEEP   587    // Time ESP32 will go to sleep (in seconds)

#elif RUCHE_NUMERO == 8
#define TIME_TO_SLEEP   337    // Time ESP32 will go to sleep (in seconds)


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

#elif RUCHE_NUMERO == 21
#define TIME_TO_SLEEP   601    // Time ESP32 will go to sleep (in seconds)

#elif RUCHE_NUMERO == 22
#define TIME_TO_SLEEP   613    // Time ESP32 will go to sleep (in seconds)

#elif RUCHE_NUMERO == 23
#define TIME_TO_SLEEP   617    // Time ESP32 will go to sleep (in seconds)

#elif RUCHE_NUMERO == 24
#define TIME_TO_SLEEP   619    // Time ESP32 will go to sleep (in seconds)
#endif

//----------------------
// ruche jlm autonome
//----------------------
#if RUCHE_NUMERO == 05 or RUCHE_NUMERO == 06 or RUCHE_NUMERO == 8 or RUCHE_NUMERO == 24
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
#elif RUCHE_NUMERO == 8
#define COMPENSATION -1.20                            // compensation de la valeur de temperature du bme280
#elif RUCHE_NUMERO == 24
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
float correction = -0.35;             // correction
float offsetCalcule = 0.226;          // offset mesure par voltmetre
float tensionDiode = 0.80 ;           // tension tpl5110 regulateur diode
const int tensionFaible = 4.7;        // tension faible
const int tensionElevee = 6;          // tension elevee

#elif RUCHE_NUMERO == 07
// batterie 12v
int R1 = 100000;                      // resistance r1 du pont
int R2 = 22000;                       // resistance r2 du pont
float correction = -0.55 ;            // correction
float offsetCalcule = 0.226;          // offset mesure par voltmetre
float tensionDiode = 0.74 ;           // tension de la diode de protection invertion 1n4007
const int tensionFaible = 10;         // tension faible
const int tensionElevee = 16;         // tension elevee

#elif RUCHE_NUMERO == 8
// batterie 5v autonome
int R1 = 27000;                       // resistance r1 du pont
int R2 = 10000;                       // resistance r2 du pont
float correction = -0.35;             // correction
float offsetCalcule = 0.226;          // offset mesure par voltmetre
float tensionDiode = 0.80 ;           // tension tpl5110 regulateur diode
const int tensionFaible = 4.7;        // tension faible
const int tensionElevee = 6;          // tension elevee


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

#elif RUCHE_NUMERO == 21
// batterie 12v
int R1 = 100000;                      // resistance r1 du pont
int R2 = 22000;                       // resistance r2 du pont
float correction = -1.00 ;            // correction
float offsetCalcule = 0.226;          // offset mesure par voltmetre
float tensionDiode = 0.74 ;           // tension de la diode de protection invertion 1n4007
const int tensionFaible = 10;         // tension faible
const int tensionElevee = 16;         // tension elevee

#elif RUCHE_NUMERO == 22
// batterie 12v
int R1 = 100000;                      // resistance r1 du pont
int R2 = 22000;                       // resistance r2 du pont
float correction = -2.35 ;            // correction
float offsetCalcule = 0.226;          // offset mesure par voltmetre
float tensionDiode = 0.74 ;           // tension de la diode de protection invertion 1n4007
const int tensionFaible = 10;         // tension faible
const int tensionElevee = 16;         // tension elevee

#elif RUCHE_NUMERO == 23
// batterie 12v
int R1 = 100000;                      // resistance r1 du pont
int R2 = 22000;                       // resistance r2 du pont
float correction = -2.05 ;            // correction
float offsetCalcule = 0.226;          // offset mesure par voltmetre
float tensionDiode = 0.74 ;           // tension de la diode de protection invertion 1n4007
const int tensionFaible = 10;         // tension faible
const int tensionElevee = 16;         // tension elevee

#elif RUCHE_NUMERO == 24
// batterie 5v autonome
int R1 = 27000;                       // resistance r1 du pont
int R2 = 10000;                       // resistance r2 du pont
float correction = -0.35;             // correction
float offsetCalcule = 0.226;          // offset mesure par voltmetre
float tensionDiode = 0.80 ;           // tension tpl5110 regulateur diode
const int tensionFaible = 4.7;        // tension faible
const int tensionElevee = 6;          // tension elevee
#endif
