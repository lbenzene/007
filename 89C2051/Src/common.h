#ifndef __COMMON_H__
#define __COMMON_H__



/*
*Port Define
*P1.0-P1.7 LEDData
*
* P3.5 LED bit High
* P3.1         Mid
* P3.0         Low
*
 */

#define OscillatorFreq      12*1000*1000
#define LED_DATA 				P1
							  //P1 port P1.0-P1.7,
#define LED_SCAN_Line0          P3_0  //low
#define LED_SCAN_Line1          P3_1  //mid
#define LED_SCAN_Line2          P3_5  //high
#define LED_SCAN_Array(a,b,c)   LED_SCAN_Line0=!(a);\
                                LED_SCAN_Line1=!(b);\
                                LED_SCAN_Line2=!(c);

#define Key_Port_Line           P3_3

#define SystemTickPeriodReg     TH0
#define SystemTickCounterReg    TL0
#define OneMilisecondCostSysTick 10

#define MUSIC_Port_line         P3_7
#define MUSIC_SWITCH            TR1
#define MUSIC_RYTHM_LEN 		21
#define MUSIC_ZONE_LEVEL_MAX	7
#define systemtickTimeConstant 0xb3//100us
#define baudRateConstant       0xf6//baud 3215.0

#define MODE_Music 				0
#define MODE_Freq  				1
#define LEDSEG_NONE				0xFF
enum modelist {mode_music=MODE_Music,mode_freq=MODE_Freq,};
typedef struct __internal_time_data__ {
   
    unsigned int  realLoadTime;//16bit
    
    //for easy to use in oter application
    enum modelist mode;
} sys_struct;

typedef struct __internal_music_data__ {
    unsigned int  targetPeriod;//output ?????? period
    unsigned char musicZone,musicZoneLevel,musicZoneID;
    union{
        struct __reg_low_high__{
            unsigned char reg_low; //TL1 cached data
            unsigned char reg_high;//TH1 cached data
            //low byte
            //   |
            //   |
            //   \/
            //high byte
        }reg_8bit;
        unsigned int reg_16bit;//uint==16bit
    }regCache;
    
    
} music_struct;

typedef struct __internal_exint0_data__ {
    unsigned char ex0_isr_counter;
    unsigned int  lastFull,nowFull;
    unsigned int extwavePeroid,extwaveFreq;//???????,??
    unsigned char finishedFlag;
} external_struct;

typedef struct __ledseg_struct__{
	unsigned char ledString[3];
    unsigned char ledobject[3];
    unsigned char chosedGroupindex;
    
}ledseg;
//led output
typedef struct __output__{
    music_struct musicoutput;
    ledseg 		 ledoutput;
}output_struct;







#endif