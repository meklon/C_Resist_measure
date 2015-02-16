/*
 Capacitive resistance measurement. The speed of discharging
 of the capacitor is controlled by the resistance of solution
 */

int pinCap = A0; //connected resistor
unsigned long valDischargeTime = 0; //Counted discharge time in ms
int valPinCapStatus = 0; //Level on capacitor's pin
unsigned long valStartTime = 0; //Start of the absolute timer
unsigned long valStopTime = 0; //End of the absolute timer
unsigned long valOhm = 0; //Counted resistance of the sample
unsigned long valTimePassed = 0; //Time after measuring start. Used as a timestamp
volatile int valISRBool = 0; //check the interrupt

/*Coeff of Ohm per ms
 This section should be replaced. The capacitor discharge is an exponential process,
 so the table of approximation for different resistance values should be used or

 the direct computation, but it will use double variables and logarithm, which will slow down

 the process badly
*/
 
void setup() {
  Serial.begin(9600);
  Serial.print("Time (milliseconds)"); Serial.print(","); Serial.println("Discharge_Time (microseconds)");
  pinMode(7, INPUT);
 ACSR =
   (0 << ACD) |    // Analog Comparator: Enabled
   (0 << ACBG) |   // Analog Comparator Bandgap Select: AIN0 is applied to the positive input
   (0 << ACO) |    // Analog Comparator Output: Off
   (1 << ACI) |    // Analog Comparator Interrupt Flag: Clear Pending Interrupt
   (1 << ACIE) |   // Analog Comparator Interrupt: Enabled
   (0 << ACIC) |   // Analog Comparator Input Capture: Disabled
   (1 << ACIS1) | (0 << ACIS0);   // Analog Comparator Interrupt Mode: Comparator Interrupt on Falling Output Edge  
}

void loop() {
  pinMode(pinCap, OUTPUT);
  digitalWrite(pinCap, HIGH);
  delay(300);
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
      if (valPinCapStatus < 645) //about 3V
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
  
  //Serial.print("Time passed: ");Serial.print(valTimePassed);Serial.print(" s, ");
  //Serial.print("Discharge time: ");Serial.print(valDischargeTime);Serial.print(" microsec, ");
  //Serial.print("Resistance: ");Serial.print(valkOhm);Serial.println(" kOhm");
}
  

ISR(ANALOG_COMP_vect)
{
  valISRBool = 1;
}
