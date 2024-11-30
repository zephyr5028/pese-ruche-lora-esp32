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
  Serial.print("tension mesuree DAC : ");
  Serial.println(AnGpioResult);
  Serial.print("tension voutBat : ");
  Serial.println(voutBat);
  Serial.print("tension batterie : ");

  // calcul de la tension en sortie du pont de resistance
  // utilisation d'un pont de resistances : voutBat = vBat * R2 / R1 + R2
  // voutBat correspond à la tension de la batterie
  Serial.print(((voutBat * (R1 + R2)) / R2) + correction + tensionDiode);
  return (((voutBat * (R1 + R2)) / R2) + correction + tensionDiode);
}

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(300);
  pinMode(AnGpio, INPUT);         // anGpio lecture de la tension de la batterie
  /*
    Serial.println("HX711 Demo");
    Serial.println("Initializing the scale");
    //#define DOUT  23
    //#define CLK   25
    loadcell.begin(DOUT, CLK);

    Serial.println("Before setting up the scale:");
    Serial.print("read: \t\t");
    Serial.println(loadcell.read());      // print a raw reading from the ADC

    Serial.print("read average: \t\t");
    Serial.println(loadcell.read_average(20));   // print the average of 20 readings from the ADC

    Serial.print("get value: \t\t");
    Serial.println(loadcell.get_value(5));   // print the average of 5 readings from the ADC minus the tare weight (not set yet)

    Serial.print("get units: \t\t");
    Serial.println(loadcell.get_units(5), 1);  // print the average of 5 readings from the ADC minus tare weight (not set) divided
    // by the SCALE parameter (not set yet)

    loadcell.set_scale(-459.542);
    //loadcell.set_scale(-471.497);                      // this value is obtained by calibrating the scale with known weights; see the README for details
    loadcell.tare();               // reset the scale to 0

    Serial.println("After setting up the scale:");

    Serial.print("read: \t\t");
    Serial.println(loadcell.read());                 // print a raw reading from the ADC

    Serial.print("read average: \t\t");
    Serial.println(loadcell.read_average(20));       // print the average of 20 readings from the ADC

    Serial.print("get value: \t\t");
    Serial.println(loadcell.get_value(5));   // print the average of 5 readings from the ADC minus the tare weight, set with tare()

    Serial.print("get units: \t\t");
    Serial.println(loadcell.get_units(5), 1);        // print the average of 5 readings from the ADC minus tare weight, divided
    // by the SCALE parameter set with set_scale
    Serial.println("Readings:");
  */
/*
//On fait la lecture times fois et on retourne la moyenne de cette série de mesure
long HX711::read_average(byte times) {
	long sum = 0;
	for (byte i = 0; i < times; i++) {
		sum += read();
	}
	return sum / times;
} 

//On prend la moyenne de la mesure et on déduit le offset pour palier un décalage éventuel
double HX711::get_value(byte times) {
	return read_average(times) - OFFSET;
}

//Et ici on divise la moyenne de la série de mesure par ce fameux facteur de calibration (ici SCALE)
float HX711::get_units(byte times) {
	return get_value(times) / SCALE;
}
*/
  Serial.println("");
  Serial.println("HX711 calibration sketch");
  Serial.println("Remove all weight from loadcell");
  Serial.println("After readings begin, place known weight on loadcell");
  Serial.println("Press + or a to increase calibration factor or offset");
  Serial.println("Press - or z to decrease calibration factor or offset");

  loadcell.begin(DOUT, CLK);      // loadcell hx711 broches DOUT et CLK
  delay(200);
  loadcell.power_down();          // put the ADC in sleep mode
  delay(2500);
  loadcell.power_up();            // reveil du hx711
  delay(500);
  loadcell.set_scale(calibration_factor);
  loadcell.set_offset(LOADCELL_OFFSET);
  loadcell.set_gain(128); // channel A can be set for a 128 or 64 gain; channel B has a fixed 32 gain
  if (calibration) {
    LOADCELL_OFFSET = 0;
    loadcell.set_offset(LOADCELL_OFFSET);
    delay(500);
    loadcell.tare();               //Reset the scale to 0
  }

  long zero_factor = loadcell.read_average(20);   //Get a baseline reading
  //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.print("Zero factor: ");
  Serial.println(zero_factor);

}
void loop() {
  delay(200);
  if (loadcell.is_ready()) {
    /*
        Serial.print("one reading:\t");
        Serial.print(loadcell.get_units(), 1);
        Serial.print("\t| average:\t");
        Serial.println(loadcell.get_units(10), 5);
    */
    if (calibration) {
      loadcell.set_scale(calibration_factor); //Adjust to this calibration factor
    } else {
      loadcell.set_offset(LOADCELL_OFFSET);   //adjust to this offset
    }
    //weight = (loadcell.get_units(numberOfReadings),0.1);
    weight = loadcell.get_units(numberOfReadings);
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
  Serial.println("");
  if (batterie) {
    tensionBatterie();
    Serial.print("   correction batterie = ");
    Serial.print(correction);
    Serial.println("");
  }

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
