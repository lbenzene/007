#include <reg2051.h>
#include <stdio.h>
#include "common.h"
/*============================================================================
*=============================Main.c==========================================
*=============================================================================*/
#define PLEASE_LOG_IT
#define DEBUG_LED_DISPLAY

sys_struct      timetable;
external_struct externaldata;
output_struct   outputGroup;
/*=============================================================================
=============================Function Prototype Declear========================
=============================================================================*/
void DetectSquareWave(void);
void GenerateTarget(void);

void          LedDisplayLoop   (void);//unsigned   char led[3]
unsigned char LedDisplaySeg(const unsigned char ledstr);

unsigned MusicRegisterFlash(void);
void SquareFrequency2OutptFreq (unsigned int inputfreq);

void Delay_ms(const unsigned char wait_time);
void LedScanLine(unsigned char index);
/*=============================================================================
=============================Interrupt part====================================
=============================================================================*/

void ex0_isr (void)         interrupt IE0_VECTOR {
    externaldata.ex0_isr_counter++;   // Increment the count
}
void ex1_isr_pushkey(void)  interrupt IE1_VECTOR{
    //External Key push made
    //change timetable.mode
    
    //judge Vailid Press Key
    
    //wait for 50 ms
    Delay_ms(60);
    if (0==Key_Port_Line)
    {
        timetable.mode^=1;//reverse
    }
 
}
void system_tick_isr(void)  interrupt TF0_VECTOR {
    //here are timer0 used as system tick counter;
    //oscillator is 12Mhz,so 1us/per sentence;
    //use 100us to set systick;
    timetable.realLoadTime++;

}
void music_change(void)     interrupt TF1_VECTOR {
    //here are timer1 used as music changer
    //toggle level
    //MusicCounterReg(outputGroup.musicoutput.reg_high,outputGroup.musicoutput.reg_low);
	TH1=outputGroup.musicoutput.regCache.reg_8bit.reg_high;
	TL1=outputGroup.musicoutput.regCache.reg_8bit.reg_low ;

    MUSIC_Port_line^=1;//reverse port.
}
/*=============================================================================
  ============================Main loop=======================================
=============================================================================*/
void main (void) {

    /*-----------------------------------------------
    Configure INT0 (external interrupt 0) to generate
    an interrupt on the falling-edge of /INT0 (P3.2).
    Enable the EX0 interrupt and then enable the
    global interrupt flag.
    -----------------------------------------------*/
    IT0 = 1;   // Configure interrupt 0 for falling edge on /INT0 (P3.2)
    EX0 = 1;   // Enable EX0 Interrupt For SquareWave
    //-----------------------------------------------
    //-----------------------------------------------
    IT1 = 1;   // Configure interrupt 0 for falling edge on /INT1 (P3.3)
    EX1 = 1;   // Enable EX1 Interrupt For Key
    //-----------------------------------------------
    TMOD =0x12; //Timer0 mode2(8bit autoreload),Timer1 mode1(16bit),disable gate,Used as Counter
    //SystemTickCounter            Music wave
    TH1=0;TL1=0x66; //Music channel Output,200us,initialize value,which'll be changed later
    SystemTickCounterReg = systemtickTimeConstant;//TL0
    SystemTickPeriodReg  = systemtickTimeConstant;//TH0

    //Priority Config
    //Timer0, EX_INT0,FirstLevel
    IP =0;
    PT0=1;//systick
    PX0=1;//SquareWave
    //
    //-----------------------------------------------
    //------------Interrupt--------------------------
    ET1= 1;
    ET0=1;//music channel,systicker,
    //music use isr to reload
    ES = 0;//serial disable
    //EX0= 1;
    //EX1= 1;
    EA = 1;    // Enable Global Interrupt Flag

    //------------Init timer-------------------------

    //timeTable
    TR1=1;
    TR0=1;
    //-------------Mode Init--------------------------
    timetable.mode=MODE_Freq;
    //------------------------------------------------
    while (1) {
	#ifndef DEBUG_LED_DISPLAY
        DetectSquareWave();
        //set Extwave For Partial Examine Function
	#else
		//During Debug Test ,check led display and music ferq generate.
		externaldata.extwavePeroid=1234;        //just 1Khz
		externaldata.extwaveFreq=1e6/externaldata.extwavePeroid;
        externaldata.finishedFlag=1;
        timetable.mode=MODE_Freq;
	#endif
        //release Input sqrtwave info

        //Generate Output Data
        //1.calculate the target music freq for out ;
        //2.Confirm the Figure to display;
        if(externaldata.finishedFlag){
            GenerateTarget();

            //in interrupt music freq will change
            MusicRegisterFlash();
            externaldata.finishedFlag=0;
        }
        LedDisplayLoop();//outputGroup.ledoutput.ledString
    }
}

/*=============================================================================
*===========================Front   Part=======================================
=============================================================================*/
#define FUllCountValue 255
void DetectSquareWave(void) {

    //SqrtWave Period Calculate
    if(externaldata.ex0_isr_counter==FUllCountValue) {

        //Finished FUllCountValue Flag
        externaldata.finishedFlag=1;
        
        externaldata.nowFull=timetable.realLoadTime;
        if(externaldata.lastFull>externaldata.nowFull) { //just between the systicker's Zero
            externaldata.extwavePeroid=(65535- externaldata.lastFull+externaldata.nowFull)*100.0/FUllCountValue ;
        } else { //100us
            externaldata.extwavePeroid=(externaldata.nowFull- externaldata.lastFull )*100.0/FUllCountValue;
        }//us单位
        externaldata.extwaveFreq=1e6/externaldata.extwavePeroid;
        externaldata.lastFull=externaldata.nowFull;
    } else {
        //externaldata.finishedFlag=0;
    }
}

/*=============================================================================
*===========================GenerateTarget   Part==============================
=============================================================================*/
void GenerateTarget(void) {
    unsigned int freq_cache;
	//Only Operate in VariablesLevel
	//unsigned int cache;
	/*
	In Music Mode 
	*/
    //Gen what music freq to display
    //set variables "musicoutput.targetPeriod"
    if(MODE_Music==timetable.mode){
    	SquareFrequency2OutptFreq( externaldata.extwavePeroid );
    	//-----------------------------------
    	//        Left--------->Right
		//LEDstring   0   1   2
    	outputGroup.ledoutput.ledString[0]='0'+outputGroup.musicoutput.musicZoneLevel;
    	outputGroup.ledoutput.ledString[1]='.';
    	outputGroup.ledoutput.ledString[2]='0'+outputGroup.musicoutput.musicZoneID;
    	
    }
	/*
	In FreqDisplay Mode 
	*/
	else{
		//MODE_Freq==timetable.mode
		
        if (externaldata.extwaveFreq>=1000)
        {
            freq_cache=externaldata.extwaveFreq/10;
        }else{
            freq_cache=externaldata.extwaveFreq;
        }
        outputGroup.ledoutput.ledString[2]=freq_cache%10+'0';
        freq_cache/=10;
        outputGroup.ledoutput.ledString[1]=freq_cache%10+'0';
        freq_cache/=10;
        outputGroup.ledoutput.ledString[0]=freq_cache%10+'0';        
		
		
	}
    outputGroup.ledoutput.ledobject[0]=LedDisplaySeg(outputGroup.ledoutput.ledString[0]);
 	outputGroup.ledoutput.ledobject[1]=LedDisplaySeg(outputGroup.ledoutput.ledString[1]);
 	outputGroup.ledoutput.ledobject[2]=LedDisplaySeg(outputGroup.ledoutput.ledString[2]);

}
//-----------------------------------
//----------LED----------------------
//-----------------------------------
#define USEABLE_LED_SEG_LEN 23
unsigned char LedDisplaySeg(const unsigned char ledstr) {
    //LED segment set what to play.
    //
	const unsigned char code LedSegments_Dict[ USEABLE_LED_SEG_LEN]={
		'0',
		'1',
		'2',
		'3',
		'4',
		'5',
		'6',
		'7',
		'8',
		'9',
		'A',
		'b',
		'C',
		'd',
		'E',
		'F',
		'.',
		'P',
		'U',
		'T',
		'Y',
		'M',//8.
		'N',//None
		};
	const unsigned char code LedSegments[ USEABLE_LED_SEG_LEN ]={
        //显示字符	共阳极段码
			0x28,//0
            0xed,//1
            0x34,//2
            0xa4,//3
            0xe1,//4
            0xa2,//5
            0x22,//6
            0xec,//7
            0x20,//8
            0xa0,//9
            0x60,//A
            0x23,//b
            0x3a,//C
            0x25,//d
            0x32,//E
            0x72,//F
            0xdf,//.
            0x22,//P
            0x29,//U
            0x7a,//T
            0xa1,//Y
            0x0,//8.
            0xff,//N
			};
	unsigned char i;
	
	for (i=0;i<USEABLE_LED_SEG_LEN;i++){
		if(ledstr==LedSegments_Dict[i]){
			return LedSegments[i];
		}
	}
	//if not found after full loop,return None
	return 0xFF;
    
}

 void LedDisplayLoop(void){
 	
 	
 	//Judge if Condition is Satisfied,Flash
    //judge  outputGroup.ledoutput.chosedGroupindex
    //#define FURTURE_CODE_TEST
    
 	//if(timetable.realLoadTime%10==0){//10ms
    if(1){
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
      
        switch(outputGroup.ledoutput.chosedGroupindex){
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
 
/*
void LedScanLine(unsigned char index){
     switch(index){
        case 0:LED_SCAN_Array(0,1,1);
            break;       
        case 1:LED_SCAN_Array(1,0,1);
            break;
        case 2:LED_SCAN_Array(1,1,0);
            break;
        default:break;
    }
}
*/
//-----------------------------------
//----------Music part---------------
//-----------------------------------
unsigned MusicRegisterFlash(void) {
    unsigned int _half_period;
    unsigned int TC;
    //use musicoutput.targetPeriod to set TH1,TL1 in next circle
    if(MODE_Music==timetable.mode)
        MUSIC_SWITCH=1;
    else{
        MUSIC_SWITCH=0;
        return MODE_Freq;
    }
    //calculate reg
    
//    #error //Left reg Configure
    //use data from outputGroup
    _half_period=outputGroup.musicoutput.targetPeriod/2;
    //TC=65535-_half_period;
    //TC= (65535-(OscillatorFreq/1000000)/12*_half_period);
    //actually, equals to TC=65535-1*_half_period;
    /*                             ^
    *                              |  
    *------Warning----Here---------|---
    *C51Compiler shows a bug when  | this C format description  used huge Constant.
    *
    */
    outputGroup.musicoutput.regCache.reg_16bit=65535-_half_period;
    //music Reg TC;
    
    return MODE_Music;
}

void SquareFrequency2OutptFreq(unsigned int inputfreq){
	//intput freq MAP to Output freq
	unsigned char i;
	unsigned int code MusicRhythm[ MUSIC_RYTHM_LEN ]= {
    //hard code to programe memory array;
    //period,MusicZone,index 
        3822,// 低1Do	0
        3405,// 低2Re	1
        3034,// 低3Mi	2
        2864,// 低4Fa	3
        2551,// 低5So	4
        2272,// 低6La	5
        2024,// 低7Si	6
        1911,// 中1Do	7
        1703,// 中2Re	8
        1517,// 中3Mi	9
        1432,// 中4Fa	10
        1275,// 中5So	11
        1136,// 中6La	12
        1012,// 中7Si	13
        955,//  高1Do	14
        851,//  高2Re	15
        758,//  高3Mi	16
        716,//  高4Fa	17
        638,//  高5So	18
        568,//  高6La	19
        506,//  高7Si	20

	};
	for (i=MUSIC_RYTHM_LEN-1;i>=0;i--){
		//From Period Small ->Large
		if(inputfreq<=MusicRhythm[i]){
			//set out put to the i index data
			outputGroup.musicoutput.targetPeriod = MusicRhythm[i];
			break;
		}
	}
	outputGroup.musicoutput.musicZone     =i;
	outputGroup.musicoutput.musicZoneLevel=i/MUSIC_ZONE_LEVEL_MAX +1;
	outputGroup.musicoutput.musicZoneID   =i%MUSIC_ZONE_LEVEL_MAX +1;
}

void Delay_ms(const unsigned char wait_time){
   
    unsigned int i,j;
    //unsigned int  lastTimeStamp=timetable.realLoadTime;
    //assert(wait_time<256);
    for(i=0;i<wait_time;i++ ){
        //for(j=0;j<10;j++);
        //while((timetable.realLoadTime- lastTimeStamp)<OneMilisecondCostSysTick);
        for(j=0;j<1000;j++);
    }
}

#ifdef PLEASE_LOG_IT

struct __we_are_history__ {
    char Date[15];
    char Time[10];
    char File[20];
    unsigned  Line;
    unsigned StdCVersion;
    
};
struct __we_are_history__ code Logging={__DATE__,
                                        __TIME__,
                                        __FILE__,
                                        __LINE__,
                                        __STDC__,
                                        };
#endif

#ifdef COMPILE_DEPRE_FUNC
//------------------------------------------------------------------------------
//----------------------------Deprecated Function-------------------------------
//------------------------------------------------------------------------------
unsigned int ConfigBaudRate(unsigned long designed_baud_rate,unsigned int SMOD) {
    assert(designed_baud_rate<=65535);
    if(SMOD==1)
        PCON&=0x7f;
    else
        PCON|=0x80;
    BaudCounter=256 - OscillatorFreq/12/32/designed_baud_rate;

}
#endif
