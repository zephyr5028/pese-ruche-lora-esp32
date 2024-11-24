//=======================================================
// Use this file to store all of the private credentials
// and connection details
//=======================================================

//=======================================================
//Controls supression of the MonPrintf function to serial
//=======================================================
#define SERIAL_BAUD 115200    // vitesse moniteur serie
#define SerialMonitor  1      // 1 true, 0 false
#define debug 1               // true for use  Display log message if True

//=============
//Numero Ruche
//=============
//#define RUCHE_NUMERO  01      // numero de la ruche jlm1
//#define RUCHE_NUMERO  02      // numero de la ruche jlm2
//#define RUCHE_NUMERO  03      // numero de la ruche jlm3
//#define RUCHE_NUMERO  04      // numero de la ruche jlm4
#define RUCHE_NUMERO  05      // numero de la ruche jlm5
//#define RUCHE_NUMERO  11      // numero de la ruche loic1
//#define RUCHE_NUMERO  12      // numero de la ruche loic2
//#define RUCHE_NUMERO  13      // numero de la ruche loic3
//#define RUCHE_NUMERO  14      // numero de la ruche loic4
//#define RUCHE_NUMERO  15      // numero de la ruche loic5
//#define RUCHE_NUMERO  16      // numero de la ruche loic6
//#define RUCHE_NUMERO  17      // numero de la ruche loic7
//#define RUCHE_NUMERO  18      // numero de la ruche loic8
//#define RUCHE_NUMERO  19      // numero de la ruche loic9


//=====================
//Load Cell with HX711
//=====================
// HX711 circuit wiring
#define DOUT  23
#define CLK   25
float weight = 0.00;
const int numberOfReadings = 10;

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
// pour ruche jlm5 rfm95
float calibration_factor = -21650;       // calibration factor    -21650

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
float calibration_factor = -20500;       // calibration factor   -20500

#elif RUCHE_NUMERO == 19
// pour ruche loic9 rfm95
float calibration_factor = -20500;       // calibration factor   -20500
#endif

// offset
#if RUCHE_NUMERO == 01
// pour ruche jlm1 rfm95
long LOADCELL_OFFSET = -104200;    // offset    -104200

#elif RUCHE_NUMERO == 02
// pour ruche jlm2 rfm95
long LOADCELL_OFFSET = -38308;      // offset  -38308

#elif RUCHE_NUMERO == 03
// pour ruche jlm3 rfm95
long LOADCELL_OFFSET = -80400;      // offset  -80400

#elif RUCHE_NUMERO == 04
// pour ruche jlm4 rfm95
long LOADCELL_OFFSET = -114050;      // offset -114050

#elif RUCHE_NUMERO == 05
// pour ruche jlm5 rfm95
long LOADCELL_OFFSET = 161921;       // offset 161921

#elif RUCHE_NUMERO == 11
// pour ruche loic1 rfm95
long LOADCELL_OFFSET = -22680;      // offset   -22680

#elif RUCHE_NUMERO == 12
// pour ruche loic2 rfm95
long LOADCELL_OFFSET = -125750;     // offset   -125750

#elif RUCHE_NUMERO == 13
// pour ruche loic3 rfm95
long LOADCELL_OFFSET = -82700;      // offset   -82700

#elif RUCHE_NUMERO == 14
// pour ruche loic4 rfm95
long LOADCELL_OFFSET = -95850;      // offset   -95650

#elif RUCHE_NUMERO == 15
// pour ruche loic5 rfm95
long LOADCELL_OFFSET = -139000;     // offset   -139000

#elif RUCHE_NUMERO == 16
// pour ruche loic6 rfm95
long LOADCELL_OFFSET = -102500;     // offset   -102500

#elif RUCHE_NUMERO == 17
// pour ruche loic7 rfm95
long LOADCELL_OFFSET = -86750;      // offset   -86750

#elif RUCHE_NUMERO == 18
// pour ruche loic8 rfm95
long LOADCELL_OFFSET = -78300;      // offset   -78300

#elif RUCHE_NUMERO == 19
// pour ruche loic9 rfm95
long LOADCELL_OFFSET = -95850;      // offset   -95850
#endif

//====================
//Battery calibration
//====================
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
int cad = 4095 ;                        // pas du convertisseur
float tensionEsp32 = 3.3 ;              // tension de reference
const int tensionAberrante = 3;         // test de le tension aberrante pour relancer une mesure
const float numberOfReadingsBat = 10.0; // nombre de lectures du port externe
// float analogReadReference  = 1.1 ;   // reference theorique

#if RUCHE_NUMERO == 01
// batterie 12v
int R1 = 100000;                      // resistance r1 du pont
int R2 = 22000;                       // resistance r2 du pont
float correction = 0.15 ;             // correction
float offsetCalcule = 0.226;          // offset mesure par voltmetre
float tensionDiode = 0.74 ;           // tension de la diode de protection invertion 1n4007

#elif RUCHE_NUMERO == 02
// batterie 12v
int R1 = 100000;                      // resistance r1 du pont
int R2 = 22000;                       // resistance r2 du pont
float correction = -0.35 ;            // correction
float offsetCalcule = 0.226;          // offset mesure par voltmetre
float tensionDiode = 0.74 ;           // tension de la diode de protection invertion 1n4007

#elif RUCHE_NUMERO == 03
// batterie 12v
int R1 = 100000;                      // resistance r1 du pont
int R2 = 22000;                       // resistance r2 du pont
float correction = -0.15 ;            // correction
float offsetCalcule = 0.226;          // offset mesure par voltmetre
float tensionDiode = 0.74 ;           // tension de la diode de protection invertion 1n4007

#elif RUCHE_NUMERO == 04
// batterie 5v
int R1 = 27000;                       // resistance r1 du pont
int R2 = 10000;                       // resistance r2 du pont
float correction = -0.15;             // correction
float offsetCalcule = 0.226;          // offset mesure par voltmetre
float tensionDiode = 0.80 ;           // tension tpl5110 regulateur diode

#elif RUCHE_NUMERO == 05
// batterie 5v
float offsetCalcule = 0.16;          // offset mesure par voltmetre gpio35 sans le tpl5110
int R1 = 27000;                       // resistance r1 du pont
int R2 = 10000;                       // resistance r2 du pont
float correction = -0.18;              // correction resistances - mesure en drv sans le tpl5110
float tensionDiode = 0.21;           // tension tpl5110 regulateur diode - en reliant vdd et drv sans le tpl5110

#elif RUCHE_NUMERO == 11
// batterie 12v
int R1 = 100000;                      // resistance r1 du pont
int R2 = 22000;                       // resistance r2 du pont
float correction = -0.35 ;            // correction
float offsetCalcule = 0.226;          // offset mesure par voltmetre
float tensionDiode = 0.74 ;           // tension de la diode de protection invertion 1n4007

#elif RUCHE_NUMERO == 12
// batterie 12v
int R1 = 100000;                      // resistance r1 du pont
int R2 = 22000;                       // resistance r2 du pont
float correction = -0.35 ;            // correction
float offsetCalcule = 0.226;          // offset mesure par voltmetre
float tensionDiode = 0.74 ;           // tension de la diode de protection invertion 1n4007

#elif RUCHE_NUMERO == 13
// batterie 12v
int R1 = 100000;                      // resistance r1 du pont
int R2 = 22000;                       // resistance r2 du pont
float correction = -0.35 ;            // correction
float offsetCalcule = 0.226;          // offset mesure par voltmetre
float tensionDiode = 0.74 ;           // tension de la diode de protection invertion 1n4007

#elif RUCHE_NUMERO == 14
// batterie 12v
int R1 = 100000;                      // resistance r1 du pont
int R2 = 22000;                       // resistance r2 du pont
float correction = -0.35 ;            // correction
float offsetCalcule = 0.226;          // offset mesure par voltmetre
float tensionDiode = 0.74 ;           // tension de la diode de protection invertion 1n4007

#elif RUCHE_NUMERO == 15
// batterie 12v
int R1 = 100000;                      // resistance r1 du pont
int R2 = 22000;                       // resistance r2 du pont
float correction = -0.35 ;            // correction
float offsetCalcule = 0.226;          // offset mesure par voltmetre
float tensionDiode = 0.74 ;           // tension de la diode de protection invertion 1n4007

#elif RUCHE_NUMERO == 16
// batterie 12v
int R1 = 100000;                      // resistance r1 du pont
int R2 = 22000;                       // resistance r2 du pont
float correction = -0.25 ;            // correction
float offsetCalcule = 0.226;          // offset mesure par voltmetre
float tensionDiode = 0.74 ;           // tension de la diode de protection invertion 1n4007

#elif RUCHE_NUMERO == 17
// batterie 12v
int R1 = 100000;                      // resistance r1 du pont
int R2 = 22000;                       // resistance r2 du pont
float correction = 0.35 ;             // correction
float offsetCalcule = 0.226;          // offset mesure par voltmetre
float tensionDiode = 0.74 ;           // tension de la diode de protection invertion 1n4007

#elif RUCHE_NUMERO == 18
// batterie 12v
int R1 = 100000;                      // resistance r1 du pont
int R2 = 22000;                       // resistance r2 du pont
float correction = -0.35 ;            // correction
float offsetCalcule = 0.226;          // offset mesure par voltmetre
float tensionDiode = 0.74 ;           // tension de la diode de protection invertion 1n4007

#elif RUCHE_NUMERO == 19
// batterie 12v
int R1 = 100000;                      // resistance r1 du pont
int R2 = 22000;                       // resistance r2 du pont
float correction = -0.35 ;            // correction
float offsetCalcule = 0.226;          // offset mesure par voltmetre
float tensionDiode = 0.74 ;           // tension de la diode de protection invertion 1n4007
#endif
