#include "motors.h"
#include "linesensor.h"
#include "encoders.h"
#include "kinematics.h"
#include "pid.h"

#define LED_PIN 13  
#define STATE_INITIAL 0
#define STATE_DRIVEF  1 
#define STATE_FINDLINE 2
#define STATE_FOLLOWLINE 3
#define STATE_RETURN 4
#define STATE_COMPLETE 5
#define BUZZER_PIN 6
// Pin to activate the orange LED
boolean led_state;  // Variable to "remember" the state
                    // of the LED, and toggle it.
LineSensor_c linesensors;
Motors_c motors;
Kinematics_c kinematics;

int state;
int STATE_INITIAL_END;
int STATE_DRIVEF_END;
int STATE_FINDLINE_END;
int STATE_FOLLOWLINE_END;
int STATE_RETURN_END;
int RETURNBACKX=1;
int RETURNBACKY;
float distt=0;
float getLineError(){
  float e_line;
  float w_left;
  float w_right;
  float sum;
  float Lprop;
  float Rprop;
  float Cprop;

  linesensors.Linesensors_read();
  
  sum= sensor_read[0]+sensor_read[1]+sensor_read[2];
  Lprop= sensor_read[0]/sum;
  Rprop= sensor_read[2]/sum;
  Cprop= sensor_read[1]/sum;
  w_left= Lprop+ 0.5*Cprop;
  w_right= Rprop+ 0.5*Cprop;
  e_line= w_left-w_right;
  
  return e_line;
}
void updateState(){
  if(STATE_INITIAL_END==0){
    state=STATE_DRIVEF;
  } else if(STATE_DRIVEF_END==0){ 
    state=STATE_FINDLINE; 
  } else if(STATE_FINDLINE_END==0){
    state=STATE_FOLLOWLINE;
  } else if(STATE_FOLLOWLINE_END==0){
    state=STATE_RETURN;
  } else if(STATE_RETURN_END==0){
    state=STATE_COMPLETE; 
  }
 }
void driveForwards(){
  linesensors.Linesensors_read();
  Serial.print( "STATE2");
  if (sensor_read[1]<=1400){
    motors.Motorforward();
  } else { 
     motors.Motorstop();
     STATE_INITIAL_END=1;
     STATE_DRIVEF_END=0;
   } 
}

void foundLineBeeps(){
  int i;
  for(i=0; i<5; i++){
    digitalWrite( BUZZER_PIN, HIGH);
    delay(1000);
    digitalWrite( BUZZER_PIN, LOW);
  }
  
  digitalWrite( BUZZER_PIN, LOW);
  STATE_DRIVEF_END=1;
  STATE_FINDLINE_END=0;

}

void returnbackx(){
    if(angle<170){
     motors.Motorturnright();
     }else if(angle>=170 && angle<=171){
     motors.Motorforward();
     } else motors.Motorturnleft();
     if(disx<=0.5 ){
       motors.Motorstop();
       RETURNBACKX=0;
       RETURNBACKY=1;
     }
}

void returnbacky(){
  if(disy>10){
    motors.Motorturnright();
      if(angle>=270&& angle<=271){
        motors.Motorforward();
      } else if (angle>271)  {
        motors.Motorturnleft();
      }  
  }else if (disy<=10 && angle>=270){ 
      motors.Motorstop();
      RETURNBACKY=0;

    }
     

 if(disy<9.9){
    motors.Motorturnleft();
     if(angle<=90&& angle>=89){
      motors.Motorforward(); 
     }else if (angle<89) {
      motors.Motorturnright();
     }
 }else if (disy>=9.9 && angle<=90){ 
      motors.Motorstop();
      RETURNBACKY=0;
    }
}
// put your setup code here, to run once:
void setup() {
  motors.initialise();
  linesensors.initialise();
  setupEncoder0();
  setupEncoder1();
  pinMode( BUZZER_PIN, OUTPUT );
  Serial.begin(9600);
  delay(1000);
  Serial.println("***RESET***");

  // Set LED pin as an output
  pinMode( LED_PIN, OUTPUT );
  state= STATE_INITIAL;

  // Set initial state of the LED
  led_state = false;
}


// put your main code here, to run repeatedly:
void loop() {

  // Using an if statement to toggle a variable
  // with each call of loop()
  if( led_state == true ) {
    led_state = false;
  } else {
    led_state = true;
  }
  countl=count_e1;
  countr=count_e0;
  dist=dis;
  kinematics.update();
  kinematics.total();
  
  

  // We use the variable to set the
  // debug led on or off on the 3Pi+
  digitalWrite( LED_PIN, led_state );

  Serial.println("loop");
  Serial.print("\n");
  Serial.print("encoder:");
  Serial.print( count_e0 );
  Serial.print("  ");
  Serial.print( count_e1 );
  Serial.print("\n");
  Serial.print(distt);
  Serial.print("\n");
  Serial.print(angle);
  Serial.print("\n");
  Serial.print(disx);
  Serial.print("\n");
  Serial.print(disy);
  Serial.print("\n");
  delayMicroseconds(1000);



  updateState();

  if ( state== STATE_INITIAL ){
    Serial.print(" STATE_INITIAL");
    STATE_INITIAL_END=0;

  } else if(state== STATE_DRIVEF){

     driveForwards();
     
  } else if(state== STATE_FINDLINE){
     foundLineBeeps();
     Serial.print("STATE3");
  } else if (state==STATE_FOLLOWLINE){
  
     float e_line;
     e_line = getLineError();
     Serial.print(e_line);
     float turn_pwm;
     turn_pwm = 200;

     turn_pwm = turn_pwm*e_line;
     if(turn_pwm>30) {
       turn_pwm = 20;
     } else if(turn_pwm<-30){
       turn_pwm = -20;
     } else if(turn_pwm<20 && turn_pwm>5){
       turn_pwm = 20 ;
     } else if(turn_pwm>-20 && turn_pwm<-5){
        turn_pwm = -20 ;
     } 
       if (sensor_read[1]>=1500 && sensor_read[1]<=2560){
         motors.Motorforward(); 
       } else if ( sensor_read[1]<1000 && sensor_read[0]<1200&& sensor_read[2]<900){
           if(disx>30){
           motors.Motorforward();
           distt=(dis-dist)+distt;
              if(distt>20){
               
                motors.Motorstop();
                delay(1000);
                STATE_FOLLOWLINE_END=0;
                STATE_FINDLINE_END=1;
              }
           } else motors.Motorturnleft();
       } else motors.setMotorValues((0 - turn_pwm),(0 + turn_pwm) , dis, dist);
  } else if(state==STATE_RETURN){
      if (RETURNBACKX==1) {
         returnbackx();
      } else if(RETURNBACKY==1){
         returnbacky();
      } else if(RETURNBACKY==0 && RETURNBACKX==0){
         STATE_RETURN_END=0;
         STATE_FOLLOWLINE_END=1;
      }
  } else if(state==STATE_COMPLETE){
      motors.Motorstop();
      digitalWrite( BUZZER_PIN, HIGH);
  }else {
     Serial.print("System Error, Unknown state:");
     Serial.println( state );

     Serial.print ("Error code:");

     motors.Motorstop();
  }
  
}
