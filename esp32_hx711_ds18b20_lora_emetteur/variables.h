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
#define RUCHE_NUMERO  02      // numero de la ruche jlm2
//#define RUCHE_NUMERO  11      // numero de la ruche loic1
//#define RUCHE_NUMERO  12      // numero de la ruche loic2

//=====================
//Load Cell with HX711
//=====================
// Change this calibration factor as per your load cell once it is found you many need to vary it in thousands
// mettre a 0 pour avoir la valeur au demarrage de la balance sans charge
// mettre ensuite la valeur retournee pour obtenir une tare fictive.
#if RUCHE_NUMERO == 01
// offset pour ruche jlm1 rfm95
float calibration_factor = -20332;       // calibration factor avec valeur connue et tare a 0  -20332

#elif RUCHE_NUMERO == 02
// offset pour ruche jlm2 rfm95
float calibration_factor = -20200;       // calibration factor avec valeur connue et tare a 0, 19°c  -20200

#elif RUCHE_NUMERO == 11
// offset pour ruche loic1 rfm95
float calibration_factor = -20100;       // calibration factor avec valeur connue et tare a 0  -20100

#elif RUCHE_NUMERO == 12
// offset pour ruche loic2 rfm95
float calibration_factor = -20332;       // calibration factor avec valeur connue et tare a 0
#endif

#if RUCHE_NUMERO == 01
// offset pour ruche jlm1 rfm95
const long LOADCELL_OFFSET = -104200;    // -104200

#elif RUCHE_NUMERO == 02
// offset pour ruche jlm2 rfm95
const long LOADCELL_OFFSET = -38308;      // mesure faite a 19°C -38308

#elif RUCHE_NUMERO == 11
// offset pour ruche loic1 rfm95
const long LOADCELL_OFFSET = -22680;      // -22680

#elif RUCHE_NUMERO == 12
// offset pour ruche loic2 rfm95
const long LOADCELL_OFFSET = -104300;
#endif

//const long LOADCELL_OFFSET = -102960;  // offset pour ruche 1 jlm sx1276 ttgo

const int numberOfReadings = 5;
const int poidsAberrant = 12;      // test du poids aberrant pour relancer une mesure

// Change sync word (0xF3) to match the receiver
// The sync word assures you don't get LoRa messages from other LoRa transceivers
// ranges from 0-0xFF
byte synchroLora = 0xF3;

//==============
//Time to Sleep
//==============
// nombres premiers 41, 61 , 127, 181, 241, 307, 313, 331, 601, 613, 631, 907
#if RUCHE_NUMERO == 01
#define TIME_TO_SLEEP  181     // Time ESP32 will go to sleep (in seconds)

#elif RUCHE_NUMERO == 02
#define TIME_TO_SLEEP   241    // Time ESP32 will go to sleep (in seconds)

#elif RUCHE_NUMERO == 11
#define TIME_TO_SLEEP  601     // Time ESP32 will go to sleep (in seconds)

#elif RUCHE_NUMERO == 12
#define TIME_TO_SLEEP   613    // Time ESP32 will go to sleep (in seconds)
#endif

//====================
// Temperature ds18b20
//====================
const int temperatureAberrante = 6; //test de le temperature aberrante pour relancer une mesure

//====================
//Battery calibration
//====================
// float analogReadReference  = 1.1 ; // reference theorique
float offsetCalcule = 0.226;// offset mesure par voltmetre
int R1 = 100000;  // resistance r1 du pont
int R2 = 22000;   // resistance r2 du pont
int cad = 4095 ;  // pas du convertisseur
float tensionEsp32 = 3.3 ; // tension de reference
float tensionDiode = 0.74 ; // correction tension de la diode de protection invertion 1n4007
const int tensionAberrante = 3; //test de le tension aberrante pour relancer une mesure

#if RUCHE_NUMERO == 01
float correction = 0.15 ; // correction erreurs des resistances

#elif RUCHE_NUMERO == 02
float correction = -0.35 ; // correction erreurs des resistances

#elif RUCHE_NUMERO == 11
float correction = -0.35 ; // correction erreurs des resistances

#elif RUCHE_NUMERO == 12
float correction = -0.35 ; // correction erreurs des resistances
#endif
