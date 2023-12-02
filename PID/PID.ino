#include "motors.h"
#include "linesensor.h"
#include "encoders.h"
#include "kinematics.h"
#include "pid.h"
#include <USBCore.h>  // To fix serial print behaviour bug.
u8 USB_SendSpace(u8 ep);
#define SERIAL_ACTIVE (USB_SendSpace(CDC_TX) >= 50)
#define LED_PIN 13  // Pin to activate the orange LED
#define RESULTS_DIM 10
float results[RESULTS_DIM][RESULTS_DIM];

// State machine
#define STATE_RUNNING_EXPERIMENT 0
#define STATE_FINISHED_EXPERIMENT 1
int state;

// Time stamp, to schedule some low priority
// activities.
unsigned long update_ts;

boolean led_state;  // Variable to "remember" the state
Motors_c motors;
PID_c l_pid;
PID_c r_pid;
PID_c h_pid;
LineSensor_c linesensor;
Kinematics_c kinematics;
// of the LED, and toggle it.

float countlt;

float countlc;
unsigned long mstart_time;
unsigned long melapsed_time;
unsigned long mcurrent_time;

float lupdate_ts;
float rupdate_ts;
float relapsed;
float lelapsed;
float lvelocity;

float countrt;

float countrc;
float pwml;
float pwmr;
float rvelocity;
float lpf_lvelocity=0.0;
float lpf_rvelocity=0.0;
// low pass filter on heading demand to
// remove some noise.
float lpf_dem = 0.0;
float alpha = 0.3;
float x=0;
float y=0;
float d=0.2;
// indexes we will use to record results
// sequentially over time.
int j_index = 0;
int i_index = 0;


// put your setup code here, to run once:
float melapsed_timef() {
 mcurrent_time = millis();
 melapsed_time = (mcurrent_time - mstart_time) ;
 mstart_time = millis();
 return melapsed_time;
 }
void lmotorpid() {
  lelapsed=millis()-lupdate_ts;
  if(lelapsed>20){
  lupdate_ts=millis();
  
  float demm = 1.0;
  countlt = countl;
  countl = count_e1;
  countlc = countl - countlt;
  //float elapsed_time= melapsed_timef();
  lvelocity = ((-countlc/358)*1.58*PI*2*10) /lelapsed;
  
  
  float dem = demand();
  if (dem >= -0.02 && dem <= 0.02 && sensor_read[1]>=1300) {
    dem = 0.1;
    demm = 0;
  } 
  lpf_lvelocity = (lpf_lvelocity * alpha) + (lvelocity * (1.0 - alpha));
  if (demm==0){
    lpf_lvelocity=-lpf_lvelocity;
  }
  pwml= l_pid.lmotorupdate(dem, 0-lpf_lvelocity);
  

  motors.setlMotorforward(fabs(pwml), dem, demm);
  //l_pid.reset();
  }
}
void rmotorpid() {
  relapsed=millis()-rupdate_ts;
  if(relapsed>20){
    rupdate_ts=millis();
  float demm = 1;
  countrt = countr;
  countr = count_e0;
  countrc = countr - countrt;
  //float elapsed_time= melapsed_timef();
  rvelocity = -((countrc/356)*PI*2*10*1.58)/ relapsed ;
  //Serial.print(rvelocity);
  //Serial.print("\n");
  float dem = demand();
   if (dem >= -0.02 && dem <= 0.02 && sensor_read[1]>=1300) {
    dem = 0.12;
    demm = 0;
   }
  lpf_rvelocity = (lpf_rvelocity * alpha) + (rvelocity * (1.0 - alpha));
  pwmr= r_pid.rmotorupdate(dem, lpf_rvelocity);

  
  motors.setrMotorforward(fabs(pwmr), dem, demm);
   //r_pid.reset();
  }
}
float getLineError() {
  float e_line;
  float w_left;
  float w_right;
  float sum;
  float Lprop;
  float Rprop;
  float Cprop;

  linesensor.Linesensors_read();
  
  sum = sensor_read[0] + sensor_read[1] + sensor_read[2];
  Lprop = (sensor_read[0]) / sum;
  Rprop = (sensor_read[2]) / sum;
  Cprop = (sensor_read[1]) / sum;
  w_left = Lprop + 0.5 * Cprop;
  w_right = Rprop + 0.5 * Cprop;
  e_line = w_left - w_right;
  
  
  return e_line;
}
float demand() {
  float e_line = getLineError();
  ;
  float dem = h_pid.headupdate(e_line);
  
  //                     0.9                   0.1 = trust old information, but big delay in signal.
  //                     0.1                   0.9 = trust new information, maybe lots of noise (small delay)
  //  So a big alpha value will create a strong low pass filter (reject high frequency noise)
  lpf_dem = (lpf_dem * alpha) + (dem * (1.0 - alpha));
  //Serial.print(lpf_dem);
  //Serial.print("\n");

  return lpf_dem;

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

void createFictionalResults() {

  // Loop through array to create some
  kinematics.update();


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


  // int i, j;

  // for (j = 0; j < 10; j++) {    // row
  //   for (i = 0; i < 10; i++) {  // col
  //     results[i][j] = angle;
  //   }
  // }
}
void setup() {
  setupEncoder0();
  setupEncoder1();
  l_pid.initialise();
  r_pid.initialise();
  h_pid.initialise();
  Serial.begin(9600);
  delay(1000);
  if (SERIAL_ACTIVE) Serial.println("***RESET***");
  motors.initialise();
  linesensor.initialise();
  // Set LED pin as an output
  pinMode(LED_PIN, OUTPUT);

  // Set initial state of the LED
  led_state = false;
  state = STATE_RUNNING_EXPERIMENT;

  // Record start timestamp
 
  l_pid.reset();
  r_pid.reset();
  h_pid.reset();
  mstart_time = millis();
   update_ts = millis();
   lupdate_ts = millis();
   rupdate_ts = millis();



}


// put your main code here, to run repeatedly:
void loop() {

  if (state == STATE_RUNNING_EXPERIMENT) {

    // Robot active, spinning on the spot.
    //melapsed_timef();
  
    kinematics.update();
    linesensor.Leftsidesensor_read();
    melapsed_timef();
    lmotorpid();
    rmotorpid();
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