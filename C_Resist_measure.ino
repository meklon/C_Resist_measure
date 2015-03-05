/*
 Author: Gumenyuk Ivan
 
 Capacitive resistance measurement. The speed of discharging
 of the capacitor is controlled by the resistance of solution.
 
 */
//OneWire setup
#include <OneWire.h>
#include <DallasTemperature.h>
// Data wire is plugged into port 10 on the Arduino
#define ONE_WIRE_BUS 10
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
// arrays to hold device address
DeviceAddress insideThermometer;

const int pinCap = A0; //connected resistor

unsigned long valDischargeTime = 0; //Counted discharge time in ms
int valPinCapStatus = 0; //Level on capacitor's pin
unsigned long valStartTime = 0; //Start of the absolute timer
unsigned long valStopTime = 0; //End of the absolute timer
unsigned long valOhm = 0; //Counted resistance of the sample
unsigned long valTimePassed = 0; //Time after measuring start. Used as a timestamp
volatile int valISRBool = 0; //check the interrupt

//Delay between the measurements to minimize electrolitic process
//You definitely should use platinum or grafite electrodes
//Delay for charging the capacitor. Depends on its capacity
 unsigned int MeasureDelay = 5000;
 unsigned int ChargeDelay = 20;

/*Coeff of Ohm per ms
 This section should be replaced. The capacitor discharge is an exponential process,
 so the table of approximation for different resistance values should be used or
 the direct computation, but it will use double variables and logarithm, which will slow down
 the process badly
*/
 
void setup() {
  Serial.begin(9600);
  delay(500);
  Serial.print("Time_(milliseconds)"); Serial.print(","); Serial.println("Discharge_Time_(microseconds)");
  pinMode(7, INPUT);
 ACSR =
   (0 << ACD) |    // Analog Comparator: Enabled
   (1 << ACBG) |   // Analog Comparator Bandgap Select: Replaced to internal reference 1.1 V. deprecated:(AIN0 is applied to the positive input)
   (0 << ACO) |    // Analog Comparator Output: Off
   (1 << ACI) |    // Analog Comparator Interrupt Flag: Clear Pending Interrupt
   (1 << ACIE) |   // Analog Comparator Interrupt: Enabled
   (0 << ACIC) |   // Analog Comparator Input Capture: Disabled
   (1 << ACIS1) | (0 << ACIS0);   // Analog Comparator Interrupt Mode: Comparator Interrupt on Falling Output Edge  

//Dallas Temperature IC Control setup

}

void loop() {
  delay(MeasureDelay);
  pinMode(pinCap, OUTPUT);
  digitalWrite(pinCap, HIGH);
  delay(ChargeDelay);
  pinMode(pinCap, INPUT);
  
  valStartTime = micros();
  valISRBool = 0; //reset the boolean
  
  while(valISRBool != 1){
  /*If the capacitor discharges too fast, the interruption may not track it
     Then the cycle would be endless. The other fail maybe with the discharge
     with disconnected pin. To bypass it the additional slower ADC measurement
     should be added with the reaction voltage under the reference, used by the
     interruption.
     NB! This workaround may cause the fast interruption method useless.    
   */ 
  valStopTime = micros();
  valPinCapStatus = analogRead(pinCap);
 
      //Additional break condition
      if (valPinCapStatus < 210) //about 1.1V
      {
        break;
      }
}
  
  valStopTime = micros();
  valDischargeTime = valStopTime - valStartTime;
  valTimePassed = millis();
  SerialOutput();
  
}

void SerialOutput() {
  Serial.print(valTimePassed); Serial.print(","); Serial.println(valDischargeTime);
}
  

ISR(ANALOG_COMP_vect)
{
  valISRBool = 1;
}
