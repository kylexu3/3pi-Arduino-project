#include "motors.h"
#include "linesensor.h"
#include "encoders.h"
#include "kinematics.h"
#include "pid.h"
u8 USB_SendSpace(u8 ep);
#define SERIAL_ACTIVE (USB_SendSpace(CDC_TX) >= 50)
#define RESULTS_DIM 10
float results[RESULTS_DIM][RESULTS_DIM];

// State machine
#define STATE_RUNNING_EXPERIMENT 0
#define STATE_FINISHED_EXPERIMENT 1
#define LED_PIN 13  // Pin to activate the orange LED
boolean led_state;  // Variable to "remember" the state
                    // of the LED, and toggle it.
LineSensor_c linesensor;
Motors_c motors;
Kinematics_c kinematics;
int state;
int j_index = 0;
int i_index = 0;

unsigned long update_ts;
float x=0;
float y=0;
// put your setup code here, to run once:
void createFictionalResults() {

  // Loop through array to create some



  // store results in the array [i][j], so when
  // j has reached 10, there is no more room to store
  // results.  Therefore we only attempt to save results
  // whilst j < 10
  if (j_index < 10) {
    // save result into this current index position
    results[i_index][j_index] = angle;

    i_index++;
    if (i_index == 10) {
      i_index = 0;
      j_index++;
    }
    if (j_index > 10) j_index = 10;
  }
}
void reportResultsOverSerial() {

  // Print millis for debug so we can
  // validate this is working in real
  // time, and not glitched somehow
  if (SERIAL_ACTIVE) Serial.print("Time(ms): ");
  if (SERIAL_ACTIVE) Serial.println(millis());
  delay(1);


  // Loop through array to print all
  // results collected
  int i, j;
  for (j = 0; j < 10; j++) {    // row
    for (i = 0; i < 10; i++) {  // col

      // Comma seperated values, to 2 decimal places
      if (SERIAL_ACTIVE) Serial.print(results[i][j], 4);
      delay(1);
      if (SERIAL_ACTIVE) Serial.print(",");
      delay(1);
    }
    if (SERIAL_ACTIVE) Serial.print("\n");  // new row
  }

  if (SERIAL_ACTIVE) Serial.println("---End of Results ---\n\n");
}

void setup() {
  
  Serial.begin(9600);
  delay(1000);
  Serial.println("***RESET***");
  linesensor.initialise();
  setupEncoder0();
  setupEncoder1();
  // Set LED pin as an output
  if (SERIAL_ACTIVE) Serial.println("***RESET***");
  pinMode( LED_PIN, OUTPUT );
  state = STATE_RUNNING_EXPERIMENT;

  // Set initial state of the LED
  led_state = false;
  update_ts = millis();
}


// put your main code here, to run repeatedly:
void loop() {
  countl=count_e1;
  countr=count_e0;
  if (state == STATE_RUNNING_EXPERIMENT) {
   linesensor.Leftsidesensor_read();
  
   kinematics.update();
   linesensor.Linesensors_read();
   if(sensor_read[1]>=1600){
    motors.Motorforward();
   }else if(sensor_read[0]>=1490){
    motors.Motorturnleft();
   }else if(sensor_read[2]>=1330){
    motors.Motorturnright();
   }
    createFictionalResults();// for experiment 1
    //This is for experiment 2,3
    // if(lsensor_read>=900 && dis<=10){

  
    // x=dis;
   // }else if(lsensor_read>=900 && dis>=30){
    // y=dis;
    //state=STATE_FINISHED_EXPERIMENT;
    // }
    
    if(millis()-update_ts>3000){
      state=STATE_FINISHED_EXPERIMENT;
      update_ts = millis();
    }

    

     

  

  } else if (state == STATE_FINISHED_EXPERIMENT) {

    // No transition out of this state, so
    // the robot will now be stuck in a
    // motors off condition and report results
    // periodically.

    // Stop robot
    motors.Motorstop();
    reportResultsOverSerial();//for experiment1

      
    //This is for experiment 2,3
    //Serial.println(y-x);
     
  
    delay(1000);


  } else {
    // error?
  }
  // Using an if statement to toggle a variable
  // with each call of loop()
  if (led_state == true) {
    led_state = false;
  } else {
    led_state = true;
  }

  // We use the variable to set the
  // debug led on or off on the 3Pi+
  digitalWrite(LED_PIN, led_state);


 

  delay(20);
  
 
 
}
