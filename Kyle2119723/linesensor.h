// this #ifndef stops this file
// from being included mored than
// once by the compiler. 
#ifndef _LINESENSOR_H
#define _LINESENSOR_H
#define LS_LEFT_IN_PIN A0
#define LS_CENTRE_IN_PIN A2
#define LS_RIGHT_IN_PIN A3
#define IRLED_IN_PIN 11
#define NB_LS_PINS 3

int  Is_pin[NB_LS_PINS]= {LS_LEFT_IN_PIN,LS_CENTRE_IN_PIN,LS_RIGHT_IN_PIN };
int which;
unsigned long start_time;
unsigned long sensor_read[ NB_LS_PINS ];
int remaining = NB_LS_PINS;
int timeout=5000;
unsigned long current_time;
unsigned long elapsed_time;
// Class to operate the linesensor(s).
class LineSensor_c {
  public:
  
    // Constructor, must exist.
    LineSensor_c() {

    } 

    void initialise(){
      pinMode(LS_LEFT_IN_PIN, INPUT);
      pinMode(LS_RIGHT_IN_PIN, INPUT);
      pinMode(LS_CENTRE_IN_PIN, INPUT);
      pinMode(IRLED_IN_PIN, OUTPUT);
      digitalWrite(IRLED_IN_PIN, HIGH);
      Serial.begin(9600);
      delay(1000);
     Serial.println("***RESET***");

    }
    void Linesensors_read(){
      
        
       for(which=0; which< NB_LS_PINS; which++){
          sensor_read[which]=0;
       }
  
       for(which=0; which< NB_LS_PINS; which++){
         pinMode(Is_pin[which], OUTPUT);
         digitalWrite(Is_pin[which], HIGH);
         delayMicroseconds(10);
         pinMode(Is_pin[which], INPUT);

       } 
       remaining= NB_LS_PINS;
       start_time=micros();
       while(remaining >0) {
          for(which=0; which< NB_LS_PINS; which++){  
            if(digitalRead( Is_pin[ which ])==LOW ) {
            current_time=micros();
            elapsed_time= current_time-start_time;
        
             if (sensor_read[ which ]==0){
               sensor_read[which]= elapsed_time;
              
               Serial.print( sensor_read[which] );
               Serial.print("\n");
          
          //  delay(100);
                remaining= remaining-1;
                
              }   
            }
        
       
          }
          if (elapsed_time >= timeout)  {
           Serial.print( which );
           Serial.print(" line sensor: ");
           Serial.print( timeout );
           Serial.print("\n");
           remaining=0;
           }
         }

     
      
     }

 };



#endif
