#include "mbed.h"
#include "Servo.h"
#include <cstdio>
#include <iterator>
#include "LinkEntry.h"
#include "Pulse.h"

//Servo
Servo serv(D12);

//Motor
DigitalOut LmotorD(D2);
DigitalOut LmotorR(D3);
DigitalOut RmotorD(D4);
DigitalOut RmotorR(D5);

//Lijn detectie
DigitalIn Analijnl(D8);
DigitalIn Analijnr(D9);
DigitalIn Analijnm(D10);

//Sonar
PulseInOut soon(D6);
Timer tmr;

//aan/uit
AnalogIn aanuit(A0);

//check leds
DigitalOut lijnled(D7);
DigitalOut sonarled(D14);
DigitalOut aanled(D15);

AnalogIn lijnL(A5);
AnalogIn lijnR(A4);
AnalogIn lijnM(A3);


int turnspeed = 30;
int heenservo = 450;
int terugservo = 500;
Timer t;
Timer heen;
bool heenofterug = 1;


void Servloop(){
    while (1) {
    
       while(aanuit>0.90) {
           //heen
           heenofterug = 1;
           heen.start();
           serv = 0.30;
           ThisThread::sleep_for(heenservo*1ms);
           heen.stop();
           heen.reset();
           
           //terug
           heenofterug = 0;
           heen.start();
           serv=0.650;
           ThisThread::sleep_for(terugservo*1ms);
           heen.stop();
           heen.reset();
           
           //serv=0.5;
           //ThisThread::sleep_for(2000ms);
           }
        serv=0.5;
    }   
}

void vooruit(){
    LmotorD = 1;
    RmotorD = 1;
    LmotorR = 0;
    RmotorR = 0;
}
void achteruit(){
    LmotorR = 1;
    RmotorR = 1;
    LmotorD = 0;
    RmotorD = 0;
    
}
void stop(){
    LmotorD = 0;
    RmotorD = 0;
    LmotorR = 0;
    RmotorR = 0;
}

void linksdanvooruit(){
    LmotorD = 1;
    RmotorD = 0;
    LmotorR = 0;
    RmotorR = 1;
    ThisThread::sleep_for(turnspeed*1ms);
    LmotorD = 1;
    RmotorD = 1;
    LmotorR = 0;
    RmotorR = 0;
}
void rechtsdanvooruit(){
    LmotorD = 0;
    RmotorD = 1;
    LmotorR = 1;
    RmotorR = 0;
    ThisThread::sleep_for(turnspeed*1ms);
    LmotorD = 1;
    RmotorD = 1;
    LmotorR = 0;
    RmotorR = 0;
    
}

double sonar(){
    soon.write(0);
    wait_us(2);
    soon.write(1);
    wait_us(5);
    soon.write(0);
    
    long duration;
    duration = soon.read_high_us();

    //omrekenen naar cm
    long RangeInCentimeters;
    RangeInCentimeters = duration / 29 / 2;
    return RangeInCentimeters;
}


Thread Servthread;
//EventQueue queue(32 * EVENTS_EVENT_SIZE);
Thread tr;



int main()
{
    //tr.start(callback(&queue, &EventQueue::dispatch_forever));
   int a = 0;
   
   

    while(1){
        
        printf("lijnL: %f \n", lijnL.read());
        printf("lijnR: %f \n", lijnR.read());
        printf("lijnM: %f \n", lijnM.read());
        printf("aanuit: %f \n", aanuit.read());
        printf("sonar: %f \n", sonar());
        ThisThread::sleep_for(2s);
        

        while(aanuit>0.90){
            aanled =1;
            tr.start(Servloop);
            
            printf("lijnL: %f \n", lijnL.read());
            printf("lijnR: %f \n", lijnR.read());
            printf("lijnM: %f \n", lijnM.read());
            printf("aanuit: %f \n", aanuit.read());
            printf("sonar: %f \n", sonar());
            
            while(Analijnm){
                lijnled = 1;
                if (a==0) {
                    LmotorD = 0;
                    RmotorD = 1;
                    LmotorR = 1;
                    RmotorR = 0;
                    ThisThread::sleep_for(1000ms);
                    a++;
                }else {while(aanuit>0.90)stop();}
            }

            if (Analijnl){
                lijnled =1;
                //ThisThread::sleep_for(turnspeed*1ms);
                rechtsdanvooruit();
                lijnled= 0;
            }

            if(Analijnr){
                lijnled =1;
                //ThisThread::sleep_for(turnspeed*1ms);
                linksdanvooruit();
                lijnled = 0;
            }

            int afstand = sonar();
            printf("turnspeeds: %d \n", afstand);

            if(afstand<20){
                sonarled=1;
                //turnspeed = 500/afstand;
                

                if(heenofterug){
                    if(heen.read_ms()<(terugservo/2.0)){
                        linksdanvooruit();
                        printf("rechts obstakel heen \n");
                        ThisThread::sleep_for(1s);
                        }//rechts obstackel
                    if(heen.read_ms()>(terugservo/2.0)){
                        rechtsdanvooruit();
                        printf("links obstakel heen\n");
                        ThisThread::sleep_for(1s);
                        }//links obstackel
                }
                else {
                    if(heen.read_ms()<(heenservo/2.0)){
                        rechtsdanvooruit();
                        printf("links obstakel terug \n");
                        ThisThread::sleep_for(1s);
                        }//links obstackel
                    if(heen.read_ms()>(heenservo/2.0)){
                        linksdanvooruit();
                        printf("rechts obstakel terug \n");
                        ThisThread::sleep_for(1s);
                        }//rechts obstackel
                }

                turnspeed = 200;
                sonarled = 0;
            }

            vooruit();
            ThisThread::sleep_for(20ms);
            aanled = 0;
        }  
        stop();           
    }
}
