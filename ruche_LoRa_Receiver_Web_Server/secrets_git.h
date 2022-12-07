//=======================================================
// Use this file to store all of the private credentials
// and connection details
//=======================================================
//
//================================================================
// IMPORTANT : definir l'utilisateur avant de programmer la carte
//================================================================

// prenom du proprietaire jlm
#define JLM  1   // if 1 true,  0 false

// prenom du proprietaire loic
#define LOIC  0  // if 1 true,  0 false

//================
//WiFi connection
//================
// Set static IP or comment out         // next 4 lines for router to assign
//WiFi.config(ip, gateway, mask, dns);  // apres wifi.config(ssis,password)
IPAddress ip(xxx, xxx, xxx, xxx);       // Put the static IP addres you want to assign here
IPAddress gw(xxx, xxx, xxx, xxx);       // Put your router gateway here
IPAddress mask(255, 255, 255, 0);       // mask
IPAddress dns(xxx, xxx, xxx, xxx);      // dns

#define SECRET_WIFI_TRUE 1              // 1 if wifi true  0 if false

#define SECRET_SSID1 "xxxxxxxxxx"    // replace MySSID with your WiFi network name
#define SECRET_PASS1 "xxxxxxxxxx"    // replace MyPassword with your WiFi password
#define SECRET_SSID2 "xxxxxxxxxx"    // replace MySSID with your WiFi network name
#define SECRET_PASS2 "xxxxxxxxxx"    // replace MyPassword with your WiFi password

//======================
//Thinkspeak connection
//======================
//const char* thingspeak_server = "api.thingspeak.com";
//const char* api_key = "api_key";
#define SECRET_THINGSPEAK_TRUE  1       	    // 1 if thingspeak true,  0 if false

// jlm
unsigned long myChannelNumberJlm = 0000000 ;    // replace 0000000 with your channel number
const char * myWriteAPIKeyJlm = "xxxxxxxxx";    // replace XYZ with your channel write API Key
// loic
unsigned long myChannelNumberLoic = 0000000;	// replace 0000000 with your channel number
const char * myWriteAPIKeyLoic = "xxxxxxxxx";   // replace XYZ with your channel write API Key

const int httpPort = 80;  // http port

//=======================
//MQTT broker connection
//=======================
#if JLM
#define MQTT  1       // if 1 true,  0 false
#elif LOIC
#define MQTT  0       // if 1 true,  0 false
#endif

#if MQTT
const char* mqttServer = "xxx.xxx.xxx.xxx";        // adresse ip du serveur domoticz
const int mqttPort = 0000;
const char* mqttUser = "xxxxxx";
const char* mqttPassword = "xxxxx";
const char mainTopic[20] = "MainTopic/";
const char* topicIn = "domoticz/out";            // Nom du topic envoyé par Domoticz
const char* topicOut = "domoticz/in";            // Nom du topic écouté par Domoticz
String nomModule = "Ruches";    // Nom usuel de ce module. Sera visible uniquement dans les Log Domoticz.

const int idxDeviceRuche1Poids = 000;             // idx du device du poids de la ruche 1
const int idxDeviceRuche1Temperature = 000;       // idx du device de la temperature de la ruche 1
const int idxDeviceRuches1TensionBatterie = 000;  // idx du device de la tension de la batterie des ruches 1
const int idxDeviceRuche2Poids = 000;             // idx du device du poids de la ruche 2
const int idxDeviceRuche2Temperature = 000;       // idx du device de la temperature de la ruche 2
const int idxDeviceRuches2TensionBatterie = 000;  // idx du device de la tension de la batterie des ruches 2
//const int idxDeviceRuche3Poids = xxx;           // idx du device du poids de la ruche 3
//const int idxDeviceRuche3Temperature = xxx;     // idx du device de la temperature de la ruche 3
#endif

//======================
// latitude , longitude
//======================
// jlm
#if JLM
#define SECRET_LATITUDE 00.000        // 
#define SECRET_LONGITUDE 0.000        // 
// loic
#elif LOIC
#define SECRET_LATITUDE 00.000        // 
#define SECRET_LONGITUDE 0.000        // 
#endif

