#include <reg2051.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

/*=============================================================================
=============================================================================*/
#define PLEASE_LOG_IT

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
#define LED_SCAN_Array(a,b,c)   LED_SCAN_Line0=(a);\
                                LED_SCAN_Line1=(b);\
                                LED_SCAN_Line2=(c);

#define Key_Port_Line           P3_3

#define SystemTickPeriodReg     TH0
#define SystemTickCounterReg    TL0
#define OneMilisecondCostSysTick 10

#define MUSIC_Port_line         P3_0
#define MUSIC_SWITCH            TR1
#define MUSIC_RYTHM_LEN 		21
#define MUSIC_ZONE_LEVEL_MAX	7
#define systemtickTimeConstant 0xb3//100us
#define baudRateConstant       0xf6//baud 3215.0

#define MODE_Music 				0
#define MODE_Freq  				1
#define LEDSEG_NONE				0xFF

typedef struct __internal_time_data__ {
   
    unsigned int  realLoadTime;//16bit
    //for easy to use in oter application
    unsigned char mode;
} sys_struct;

typedef struct __internal_music_data__ {
    unsigned int  targetPeriod;//output 离散的音频值 period
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
    unsigned int extwavePeroid,extwaveFreq;//外部信号的周期,频率
    unsigned char finishedFlag;
} external_struct;

typedef struct __ledseg_struct__{
	unsigned char ledString[3];
    unsigned char ledobject[3];

    
}ledseg;
//led output
typedef struct __output__{
    music_struct musicoutput;
    ledseg 		 ledoutput;
}output_struct;


sys_struct timetable;
external_struct externaldata;
output_struct outputGroup;
/*=============================================================================
=============================Function Prototype Declear========================
=============================================================================*/
void DetectSquareWave(void);
void GenerateTarget(void);

void          LedDisplayLoop   (unsigned   char led[3]);
unsigned char LedDisplaySeg(const unsigned char ledstr);

unsigned MusicRegisterFlash(void);
void SquareFrequency2OutptFreq (unsigned int inputfreq);

void Delay_ms(const unsigned char wait_time);
/*=============================================================================
=============================Interrupt part====================================
=============================================================================*/

void ex0_isr (void)         interrupt IE0_VECTOR {
    externaldata.ex0_isr_counter++;   // Increment the count
}
void ex1_isr_pushkey(void)  interrupt IE1_VECTOR{
    //External Key push made
    //change     timetable.mode
    
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




    while (1) {
        DetectSquareWave();
        //release Input sqrtwave info

        //Generate Output Data
        //1.calculate the target music freq for out ;
        //2.Confirm the Figure to display;
        GenerateTarget();

        //in interrupt music freq will change
        MusicRegisterFlash();
        
        //led flash 3 times
        LedDisplayLoop(outputGroup.ledoutput.ledString);
    }
}

/*=============================================================================
=============================================================================*/
void DetectSquareWave(void) {
#define FUllCountValue 255
    //SqrtWave Period Calculate
    if(externaldata.ex0_isr_counter==FUllCountValue) {

        //Finished FUllCountValue Flag
        externaldata.finishedFlag=1;
        
        externaldata.nowFull=timetable.realLoadTime;
        if(externaldata.lastFull>externaldata.nowFull) { //just between the systicker's Zero
            externaldata.extwavePeroid=(65535- externaldata.lastFull+externaldata.nowFull)*100.0/FUllCountValue ;
        } else { //100us
            externaldata.extwavePeroid=(externaldata.nowFull- externaldata.lastFull )*100.0/FUllCountValue;
        }
        externaldata.extwaveFreq=1e6/externaldata.extwavePeroid;
        externaldata.lastFull=externaldata.nowFull;
    } else {
        externaldata.finishedFlag=0;
    }
}
void GenerateTarget(void) {
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
    	outputGroup.ledoutput.ledString[1]='P';
    	outputGroup.ledoutput.ledString[2]='0'+outputGroup.musicoutput.musicZoneID;
    	
    	
    }
	/*
	In FreqDisplay Mode 
	*/
	else{
		//MODE_Freq==timetable.mode
		unsigned int freq_cache;
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
			0xC0,	//0
			0xF9,	//1
			0xA4,	//2
			0xB0,	//3
			0x99,	//4
			0x92,	//5
			0x82,	//6
			0xF8,	//7
			0x80,	//8
			0x90,	//9
			0x88,	//A
			0x83,	//b
			0xC6,	//C
			0xA1,	//d
			0x86,	//E
			0x8E,	//F
			0x7F,	//·
			0x82,	//P
			0xC1,	//U
			0xCE,	//T
			0x91,	//Y
			0x00,	//8.
			0xFF,	//None
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

 void LedDisplayLoop(unsigned	char led[3]){
 	outputGroup.ledoutput.ledobject[0]=LedDisplaySeg(led[0]);
 	outputGroup.ledoutput.ledobject[1]=LedDisplaySeg(led[1]);
 	outputGroup.ledoutput.ledobject[2]=LedDisplaySeg(led[2]);
 	
 	//Judge if Condition is Satisfied,Flash
 	if(timetable.realLoadTime%50==0){
 		//scan line 0,Choose ledgroup_1
        LED_SCAN_Array(1,0,0);
	 	LED_DATA=outputGroup.ledoutput.ledobject[0];
	 	LED_DATA=LEDSEG_NONE;//remove Left shadow
	 	
	 	//scan line 1
        LED_SCAN_Array(0,1,0);   //
	 	LED_DATA=outputGroup.ledoutput.ledobject[1];
	 	LED_DATA=LEDSEG_NONE;
	 	
	 	//scan line 2
        LED_SCAN_Array(0,0,1);
	 	LED_DATA=outputGroup.ledoutput.ledobject[2];
	 	LED_DATA=LEDSEG_NONE;
	 }
 	
 }
//-----------------------------------
//----------Music part---------------
//-----------------------------------
unsigned MusicRegisterFlash(void) {
    unsigned int _half_period;
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
    outputGroup.musicoutput.regCache.reg_16bit= 65536- (OscillatorFreq/1000000 )*_half_period /12;
    //music Reg TC;
    
    return MODE_Music;
}

void SquareFrequency2OutptFreq(unsigned int inputfreq){
	//intput freq MAP to Output freq
	unsigned char i;
	unsigned int code MusicRhythm[ MUSIC_RYTHM_LEN ]= {
    //hard code to programe memory array;
    //period,Description
	    3822,// 低1Do
	    3405,// 低2Re
	    3034,// 低3Mi
	    2864,// 低4Fa
	    2551,// 低5So
	    2272,// 低6La
	    2024,// 低7Si
	    1911,// 中1Do
	    1703,// 中2Re
	    1517,// 中3Mi
	    1432,// 中4Fa
	    1275,// 中5So
	    1136,// 中6La
	    1012,// 中7Si
	    955,//  高1Do
	    851,//  高2Re
	    758,//  高3Mi
	    716,//  高4Fa
	    638,//  高5So
	    568,//  高6La
	    506,//  高7Si
	};
	for (i=MUSIC_RYTHM_LEN;i>=0;i--){
		//From Period Small ->Large
		if(inputfreq<=MusicRhythm[i]){
			//set out put to the i index data
			outputGroup.musicoutput.targetPeriod = MusicRhythm[i];
			break;
		}
	}
	outputGroup.musicoutput.musicZone     =i;
	outputGroup.musicoutput.musicZoneLevel=i/MUSIC_ZONE_LEVEL_MAX;
	outputGroup.musicoutput.musicZoneID   =i%MUSIC_ZONE_LEVEL_MAX;
}

void Delay_ms(const unsigned char wait_time){
   
    unsigned char i;
    unsigned int  lastTimeStamp=timetable.realLoadTime;
    //assert(wait_time<256);
    for(i=0;i<wait_time;i++ ){
        //for(j=0;j<10;j++);
        while((timetable.realLoadTime- lastTimeStamp)<OneMilisecondCostSysTick);
        
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
