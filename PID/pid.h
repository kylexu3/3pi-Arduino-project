// this #ifndef stops this file
// from being included mored than
// once by the compiler.
#ifndef _PID_H
#define _PID_H

// Class to contain generic PID algorithm.
class PID_c {
public:
  float Pl;
  float Pr;
  float e_lmotor;
  float e_rmotor;
  float Kp = 200;
  float Ki = 0.04;
  unsigned long lltime;
 
  unsigned long rltime ;
  unsigned long hltime ;
 
  float Il;
  float Ir;
  float Ptt = 0;
  float Pt = 0;
  float Kd =-200;
  float Dl;
  float Dr;
  float Pls; //
  float Ils; //
  float Dls; //
  float Pttt;
  float Kpls = 0.25;  // gains for line following
  float Kils=0.0005;
  float Kdls=-0.3;
  float heading_i_sum = 0.0;
  float last_eline = 0.0;
  float last_ermotor = 0.0;
  float last_elmotor= 0.0;
  float rmotor_i_sum = 0.0;
  float lmotor_i_sum = 0.0;
  float feedbackl;
  float feedbackr;
   float feedbackh;
  // Constructor, must exist.
  PID_c() {
  }
  void initialise() {
    Pl = 0;
    Pr = 0;
    e_lmotor = 0;
    e_rmotor = 0;
    Ir = 0;
    Dl = 0;
    Dr = 0;
    Pls = 0;
    Ils = 0;
    Dls = 0;
    heading_i_sum = 0.00;
    last_eline = 0.0;
    last_ermotor = 0.0;
    last_elmotor = 0.0;
    rmotor_i_sum = 0.0;
    lmotor_i_sum = 0.0;
    feedbackl=0;
    feedbackr=0;
    feedbackh=0;
  }
  void reset() {

    Pl = 0;
    Pr = 0;
    e_lmotor = 0;
    e_rmotor = 0;
    Il = 0;
    Ir = 0;
    Dl = 0;
    Dr = 0;
    Pls = 0;
    Ils = 0;
    Dls = 0;
    lltime = millis();

    rltime = millis();

    hltime = millis();

    last_eline = 0.0;
    heading_i_sum = 0.00;
    last_ermotor = 0.0;
    last_elmotor = 0.0;
    rmotor_i_sum = 0.0;
    lmotor_i_sum = 0.0;
    feedbackl=0;
    feedbackr=0;
    feedbackh=0;
  }
  float elapsedtime() {
    
    //etime = millis() - ctime;
   // if(etime>20){
       //ctime = millis();
       //etime=etime;
   // } 
    
    
   // return etime;
  }
  float lmotorupdate(float demand, float lmeasurement) {
       unsigned long lctime;
       unsigned long letime;
       lctime=millis();
       letime=lctime-lltime;
       lltime=millis();
       if(letime==0) return feedbackl;
       e_lmotor = demand - lmeasurement;
       Pl = Kp * e_lmotor;
       lmotor_i_sum=lmotor_i_sum + (e_lmotor*letime);
       Il = lmotor_i_sum*Ki;
       float diff_elmotor= e_lmotor- last_elmotor;
       diff_elmotor = diff_elmotor/letime ;
       last_elmotor=e_lmotor;

       Dl = Kd * diff_elmotor;
       feedbackl=Il+Pl+Dl;
       return feedbackl ;
       
  }
  float rmotorupdate(float demand, float rmeasurement) {
     unsigned long rctime;
     unsigned long retime;
     rctime=millis();
     retime=rctime-rltime;
     rltime=millis();
     if(retime==0) return feedbackr;
    e_rmotor = demand - rmeasurement;
    Pr = Kp * e_rmotor;
    rmotor_i_sum=rmotor_i_sum + (e_rmotor*retime);
    Ir = rmotor_i_sum*Ki;
    float diff_ermotor= e_rmotor- last_ermotor;
    diff_ermotor = diff_ermotor/retime ;
    last_ermotor=e_rmotor;

    Dr = Kd * diff_ermotor;
    feedbackr=Pr+Ir+Dr;
    return feedbackr;
  }
  float headupdate(float eline) {
      unsigned long hctime;
       unsigned long hetime;
       hctime=millis();
       hetime=hctime-hltime;
       hltime=millis();
    Pls = (-eline)*Kpls; // p-term


    heading_i_sum = heading_i_sum + ( -eline * hetime); // integration.

    Ils = heading_i_sum * Kils;// i-term

    float diff_eline = -eline - last_eline; // work out change
    diff_eline = diff_eline / hetime;
    last_eline = -eline;

    Dls = Kdls * diff_eline; // d-term
    feedbackh= Pls+Ils+Dls;
    return feedbackh;
    //Pttt = Pls;
  }
  void dupdate() {
  }
};



#endif