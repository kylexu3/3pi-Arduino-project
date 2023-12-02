// this #ifndef stops this file
// from being included mored than
// once by the compiler. 
#ifndef _MOTORS_H
#define _MOTORS_H

# define L_PWM_PIN 10
# define L_DIR_PIN 16
# define R_PWM_PIN 9
# define R_DIR_PIN 15

# define FWD LOW
# define REV HIGH
// Class to operate the motor(s).
class Motors_c {
  public:

    // Constructor, must exist.
    Motors_c() {


    } 

    // Use this function to 
    // initialise the pins and 
    // state of your motor(s).
    void initialise() {
      pinMode (L_PWM_PIN, OUTPUT);
      pinMode (R_PWM_PIN, OUTPUT);
      pinMode (L_DIR_PIN, OUTPUT);
      pinMode (R_DIR_PIN, OUTPUT);
      Serial.begin(9600);
      delay(1000);
      Serial.println("***RESET***");

    }
    
  // 
     void Motorturnleft() {

       digitalWrite(L_DIR_PIN, REV);
       digitalWrite(R_DIR_PIN, FWD);
       analogWrite(L_PWM_PIN,20);
       analogWrite(R_PWM_PIN,20);

      
       }
     void Motorturnright() {

       digitalWrite(L_DIR_PIN, FWD);
       digitalWrite(R_DIR_PIN, REV);
       analogWrite(L_PWM_PIN,20);
       analogWrite(R_PWM_PIN,20);
       }

     void Motorforward() {

       digitalWrite(L_DIR_PIN, FWD);
       digitalWrite(R_DIR_PIN, FWD);
       analogWrite(L_PWM_PIN,15);
       analogWrite(R_PWM_PIN,15.7);
       }

     void Motorstop() {
       digitalWrite(L_DIR_PIN, FWD);
       digitalWrite(R_DIR_PIN, FWD);
       analogWrite(L_PWM_PIN,0);
       analogWrite(R_PWM_PIN,0);
       }  
     void setMotorValues(float leftm , float rightm , float diss, float disst){
       float disa=diss-disst;
       Serial.print(disa);
       Serial.print("\n");
       if (leftm<-5 && disa>0.01){
         digitalWrite(L_DIR_PIN, REV);
         digitalWrite(R_DIR_PIN, FWD);
         analogWrite(L_PWM_PIN,0-leftm);
         analogWrite(R_PWM_PIN,rightm);
       } else if (leftm>5 && disa>0.01) {
         digitalWrite(L_DIR_PIN, FWD);
         digitalWrite(R_DIR_PIN, REV);
         analogWrite(L_PWM_PIN,leftm);
         analogWrite(R_PWM_PIN,0-rightm);

       } else Motorforward();
         
      
      
     }
 
  
    // Write a function to operate
    // your motor(s)
    // ...

    
};



#endif
