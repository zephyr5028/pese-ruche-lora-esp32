#include "HX711.h"
#define DOUT  23
#define CLK  25
HX711 scale;
float weight;

// pour reglage de la calibration puis de l'offset
bool calibration = 0; // 1 = reglage calibration et 0 = reglage offset

//float calibration_factor = -20332; // for me this value works just perfect 419640

// 1er : apres tare et offset a zero, placer une charge connue et regler
float calibration_factor = -20200; 

long LOADCELL_OFFSET;

// long LOADCELL_OFFSET = -103222;
// long LOADCELL_OFFSET = -103495;
// float LOADCELL_DIVIDER = -20332;

const int numberOfReadings = 10;

void setup() {
  if (calibration) {
    LOADCELL_OFFSET = 0;
  } else {
    LOADCELL_OFFSET = -38308; // 2eme : enlever tare au demarrage puis regler offset
  }
  Serial.begin(9600);
  scale.begin(DOUT, CLK);
  Serial.println("");
  Serial.println("HX711 calibration sketch");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Press + or a to increase calibration factor or offset");
  Serial.println("Press - or z to decrease calibration factor or offset");
  scale.set_scale();
  scale.set_offset(LOADCELL_OFFSET);
  // enlever pour le reglage de l'offset
  if (calibration) {
    scale.tare(); //Reset the scale to 0
  }
  scale.set_scale(calibration_factor);
  long zero_factor = scale.read_average(); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);
}
void loop() {
  if (calibration) {
    scale.set_scale(calibration_factor); //Adjust to this calibration factor
  } else {
    scale.set_offset(LOADCELL_OFFSET); //adjust to this offset
  }
  Serial.print("Reading: ");
  weight = scale.get_units(5);
  Serial.print(scale.get_units(), 2);
  //Serial.print(" lbs"); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
  Serial.print("Kilogram:");
  Serial.print( weight, 3);
  Serial.print(" Kg");
  Serial.print(" calibration_factor: ");
  Serial.print(calibration_factor);
  Serial.print(" / offset: ");
  Serial.print(LOADCELL_OFFSET);
  Serial.println("");
  if (Serial.available())  {
    char temp = Serial.read();
    //Serial.println(temp);
    if (temp == '+' || temp == 'a') {
      if (calibration) {
        calibration_factor += 100;
      } else {
        LOADCELL_OFFSET += 50;
      }
    } else if (temp == '-' || temp == 'z') {
      if (calibration) {
        calibration_factor -= 100;
      } else {
        LOADCELL_OFFSET -= 50;
      }
    }
  }
}
