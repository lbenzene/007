#include <reg2051.h>
#include <stdio.h>
#include "common.h"

sys_struct      timetable;
external_struct externaldata;
output_struct   outputGroup;
void LedDisplayLoop(const unsigned char led[3]);
unsigned char code ledseg_test[]= {
    0x34,//2
    0xa4,//3
    0xe1,//4
};

void system_tick_isr(void)  interrupt TF0_VECTOR {
    //here are timer0 used as system tick counter;
    //oscillator is 12Mhz,so 1us/per sentence;
    //use 100us to set systick;
    timetable.realLoadTime++;

}
void main(void) {
    TMOD =0x12; //Timer0 mode2(8bit autoreload),Timer1 mode1(16bit),disable gate,Used as Counter
    //SystemTickCounter            Music wave
    //TH1=0;TL1=0x66; //Music channel Output,200us,initialize value,which'll be changed later
    SystemTickCounterReg = systemtickTimeConstant;//TL0
    SystemTickPeriodReg  = systemtickTimeConstant;//TH0
    ET1= 1;
    ET0=1;//music channel,systicker,
    //music use isr to reload
    ES = 0;//serial disable
    //EX0= 1;
    //EX1= 1;
    EA = 1;    // Enable Global Interrupt Flag

    //------------Init timer-------------------------

    //timeTable
    //TR1=1;
    TR0=1;

    // Write your code here
    while (1){
        //LedDisplayLoop(outputGroup.ledoutput.ledString);
        LedDisplayLoop(ledseg_test);
    }

}

void LedDisplayLoop(const unsigned char led[3]) {
    outputGroup.ledoutput.ledobject[0]=led[0];
    outputGroup.ledoutput.ledobject[1]=led[1];
    outputGroup.ledoutput.ledobject[2]=led[2];

    //Judge if Condition is Satisfied,Flash
    //judge  outputGroup.ledoutput.chosedGroupindex
    //#define FURTURE_CODE_TEST

    if(timetable.realLoadTime%50==0) { //10ms

        /*         if(outputGroup.ledoutput.chosedGroupindex>=2){
                    outputGroup.ledoutput.chosedGroupindex=0;
                }else{
                    outputGroup.ledoutput.chosedGroupindex++;
                }
                //LED_DATA=LEDSEG_NONE;
                LED_SCAN_Array(1,1,1);//close BitChoose
                LED_DATA=outputGroup.ledoutput.ledobject[outputGroup.ledoutput.chosedGroupindex];
                LedScanLine(outputGroup.ledoutput.chosedGroupindex);
                 */

        switch(outputGroup.ledoutput.chosedGroupindex) {
        case 0://line 0
            LED_SCAN_Array(1,1,1);//close BitChoose
            LED_DATA=outputGroup.ledoutput.ledobject[0];
            LED_SCAN_Array(0,1,1);
            //next scan line
            outputGroup.ledoutput.chosedGroupindex=1;
            break;
        case 1://line1
            LED_SCAN_Array(1,1,1);

            LED_DATA=outputGroup.ledoutput.ledobject[1];
            LED_SCAN_Array(1,0,1);
            outputGroup.ledoutput.chosedGroupindex=2;
            break;
        case 2:
            LED_SCAN_Array(1,1,1);

            LED_DATA=outputGroup.ledoutput.ledobject[2];
            LED_SCAN_Array(1,1,0);
            outputGroup.ledoutput.chosedGroupindex=0;
            break;
        default:
            LED_SCAN_Array(0,0,0);
            LED_DATA=0x32;//StandsFor "Error"s E
            break;
        }

    }

}