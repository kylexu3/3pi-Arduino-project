// this #ifndef stops this file
// from being included mored than
// once by the compiler. 
#ifndef _KINEMATICS_H
#define _KINEMATICS_H
#include "math.h"
float dis;
float countl;
float countr;
float rotl;
float rotr;
float angle;
float arotl;
float arotr;
float disx;
float disy;
float dist;


// Class to track robot position.
class Kinematics_c {
  public:
  
    // Constructor, must exist.
    Kinematics_c() {

    } 

    // Use this function to update
    // your kinematics
    void update() {
      rotl=-countl/358;
      rotr=-countr/354;
      dis= (rotl+rotr)*1.58*PI;
      arotl=-countl/357*360;
      arotr=-countr/354*360;
      angle=(arotl-arotr)*1.58/8.45;
      
     

    }
    void total(){
          disx=disx+(dis-dist)*cos(angle/180*PI);
          disy= disy+(dis-dist)*sin(angle/180*PI);
      

    }

};



#endif
