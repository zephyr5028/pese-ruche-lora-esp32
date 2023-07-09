/********************************************************************************************
   Test Balance et tension de la batterie avec ESP32 et HX711

   jlm mars 2023  suivi par GIT

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

HX711 loadcell;   // objet loadcell

//=========================================
// calibration suivi du reglage de l'offset
//=========================================
// pour le reglage de la calibration puis de l'offset
// pour le reglage de la correction batterie
bool calibration = 1 ;                 // 1 = reglage calibration et 0 = reglage offset
bool batterie = 0 ;                    // 1 = reglage de la correction batterie et 0 calibration / offset
//#define BAT  12             // tension batterie 12v
#define BAT  5                // tension batterie 5v

// 1er : apres tare et offset a zero, placer une charge connue et regler
//float calibration_factor = -22200;

// 2eme : enlever tare au demarrage puis regler offset (utiliser le zero factor disponible au demarrage)
//long LOADCELL_OFFSET = 78200;

//==============
// Read sensors
//==============
float tensionBatterie() {
  float voutBat = 0.0;
  int AnGpioResult = 0;
  // mesures tension d'alimentation
  for (int i = 0; i < numberOfReadingsBat; i++) {
    // The voltage measured is then assigned to a value between 0 and 4095
    // in which 0 V corresponds to 0, and 3.3 V corresponds to 4095
    AnGpioResult = AnGpioResult + analogRead(AnGpio);
    delay(50);
  }
  AnGpioResult = AnGpioResult / numberOfReadingsBat; // resultat apres plusieurs mesures
  // calcul du resultat en volt
  voutBat = (((AnGpioResult * tensionEsp32) / cad) + offsetCalcule);
  Serial.print("tension mesuree adc : ");
  Serial.println(AnGpioResult);
  Serial.print("tension voutBat : ");
  Serial.println(voutBat);
  Serial.print("tension batterie : ");

  // calcul de la tension en sortie du pont de resistance
  // utilisation d'un pont de resistances : voutBat = vBat * R2 / R1 + R2
  // voutBat correspond à la tension de la batterie
  Serial.println(((voutBat * (R1 + R2)) / R2) + correction + tensionDiode);
  return (((voutBat * (R1 + R2)) / R2) + correction + tensionDiode);
}

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(300);
  pinMode(AnGpio, INPUT);         // anGpio lecture de la tension de la batterie

  Serial.println("");
  Serial.println("HX711 calibration sketch");
  Serial.println("Remove all weight from loadcell");
  Serial.println("After readings begin, place known weight on loadcell");
  Serial.println("Press + or a to increase calibration factor or offset");
  Serial.println("Press - or z to decrease calibration factor or offset");

  loadcell.begin(DOUT, CLK);      // loadcell hx711 broches DOUT et CLK
  loadcell.power_down();          // put the ADC in sleep mode
  delay(3000);
  loadcell.power_up();            // reveil du hx711
  delay(500);
  loadcell.set_scale(calibration_factor);
  loadcell.set_offset(LOADCELL_OFFSET);
  if (calibration) {
    loadcell.tare();               //Reset the scale to 0
  }

  long zero_factor = loadcell.read_average(20);   //Get a baseline reading
  //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.print("Zero factor: ");
  Serial.println(zero_factor);
}
void loop() {
  if (calibration) {
    loadcell.set_scale(calibration_factor); //Adjust to this calibration factor
  } else {
    loadcell.set_offset(LOADCELL_OFFSET);   //adjust to this offset
  }
  delay(400);
  if (loadcell.is_ready()) {
    weight = loadcell.get_units(10);
    //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
    //Serial.print(" lbs");
  } else {
    Serial.println("HX711 not found.");
  }
  Serial.print("Kilogram:");
  Serial.print( weight, 3);
  Serial.print(" Kg");
  Serial.print(" calibration_factor: ");
  Serial.print(calibration_factor);
  Serial.print(" / offset: ");
  Serial.print(LOADCELL_OFFSET);
  Serial.println("");

  Serial.print("tension batterie = ");
  Serial.print(tensionBatterie());
  Serial.print("   correction batterie = ");
  Serial.println(correction);
  // reglages calibration / offset / corection batterie avec  + - a et z
  if (Serial.available())  {
    char temp = Serial.read();
    //Serial.println(temp);
    if (temp == '+' || temp == 'a') {
      if (calibration and !batterie) {
        if (temp == '+') {
          calibration_factor += 100;
        } else if (temp == 'a') {
          calibration_factor += 50;
        }
      } else if (!calibration and !batterie) {
        if (temp == '+') {
          LOADCELL_OFFSET += 50;
        } else if (temp == 'a') {
          LOADCELL_OFFSET += 100;
        }
      } else if (batterie) {
        if (temp == '+') {
          correction += 0.05;
        }
      }
    } else if (temp == '-' || temp == 'z') {
      if (calibration and !batterie) {
        if (temp == '-') {
          calibration_factor -= 100;
        } else if (temp == 'z') {
          calibration_factor -= 50;
        }
      } else if (!calibration and !batterie) {
        if (temp == '-') {
          LOADCELL_OFFSET -= 50;
        } else if (temp == 'z') {
          LOADCELL_OFFSET -= 100;
        }
      } else if (batterie) {
        if (temp == '-') {
          correction -= 0.05;
        }
      }
    }
  }
}
