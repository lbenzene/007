
#line 1 "Src\main.c" /0
  
#line 1 "F:\Keil\Program\C51\Inc\Atmel\reg2051.h" /0






 
 
 
 
 


 
 sfr SP      = 0x81;
 sfr DPL     = 0x82;
 sfr DPH     = 0x83;
 sfr PCON    = 0x87;
 sfr TCON    = 0x88;
 sfr TMOD    = 0x89;
 sfr TL0     = 0x8A;
 sfr TL1     = 0x8B;
 sfr TH0     = 0x8C;
 sfr TH1     = 0x8D;
 sfr P1      = 0x90;
 sfr SCON    = 0x98;
 sfr SBUF    = 0x99;
 sfr IE      = 0xA8;
 sfr P3      = 0xB0;
 sfr IP      = 0xB8;
 sfr PSW     = 0xD0;
 sfr ACC     = 0xE0;
 sfr B       = 0xF0;
 


 
 
 
 
 
 
 
 
 
 


 
 sbit IT0  = 0x88;
 sbit IE0  = 0x89;
 sbit IT1  = 0x8A;
 sbit IE1  = 0x8B;
 sbit TR0  = 0x8C;
 sbit TF0  = 0x8D;
 sbit TR1  = 0x8E;
 sbit TF1  = 0x8F;
 


 
 
 
 
 
 
 
 
 
 
 
 
 


 
 sbit P1_0 = 0x90;
 sbit P1_1 = 0x91;
 sbit P1_2 = 0x92;
 sbit P1_3 = 0x93;
 sbit P1_4 = 0x94;
 sbit P1_5 = 0x95;
 sbit P1_6 = 0x96;
 sbit P1_7 = 0x97;
 
 sbit AIN0 = 0x90;        
 sbit AIN1 = 0x91;        
 


 
 sbit RI   = 0x98;
 sbit TI   = 0x99;
 sbit RB8  = 0x9A;
 sbit TB8  = 0x9B;
 sbit REN  = 0x9C;
 sbit SM2  = 0x9D;
 sbit SM1  = 0x9E;
 sbit SM0  = 0x9F;
 


 
 sbit EX0  = 0xA8;        
 sbit ET0  = 0xA9;        
 sbit EX1  = 0xAA;        
 sbit ET1  = 0xAB;        
 sbit ES   = 0xAC;        
 sbit ET2  = 0xAD;        
 
 sbit EA   = 0xAF;        
 


 
 sbit P3_0 = 0xB0;
 sbit P3_1 = 0xB1;
 sbit P3_2 = 0xB2;
 sbit P3_3 = 0xB3;
 sbit P3_4 = 0xB4;
 sbit P3_5 = 0xB5;
 
 sbit P3_7 = 0xB7;
 
 sbit RXD  = 0xB0;        
 sbit TXD  = 0xB1;        
 sbit INT0 = 0xB2;        
 sbit INT1 = 0xB3;        
 sbit T0   = 0xB4;        
 sbit T1   = 0xB5;        
 sbit AOUT = 0xB6;        
 


 
 sbit PX0  = 0xB8;
 sbit PT0  = 0xB9;
 sbit PX1  = 0xBA;
 sbit PT1  = 0xBB;
 sbit PS   = 0xBC;
 


 
 sbit P    = 0xD0;
 sbit F1   = 0xD1;
 sbit OV   = 0xD2;
 sbit RS0  = 0xD3;
 sbit RS1  = 0xD4;
 sbit F0   = 0xD5;
 sbit AC   = 0xD6;
 sbit CY   = 0xD7;
 



 
 
 
 
 
 
 
 
#line 1 "Src\main.c" /0
 
  
#line 1 "F:\Keil\Program\C51\Inc\stdio.h" /0






 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 typedef unsigned int size_t;
 
 
 #pragma SAVE
 #pragma REGPARMS
 extern char _getkey (void);
 extern char getchar (void);
 extern char ungetchar (char);
 extern char putchar (char);
 extern int printf   (const char *, ...);
 extern int sprintf  (char *, const char *, ...);
 extern int vprintf  (const char *, char *);
 extern int vsprintf (char *, const char *, char *);
 extern char *gets (char *, int n);
 extern int scanf (const char *, ...);
 extern int sscanf (char *, const char *, ...);
 extern int puts (const char *);
 
 #pragma RESTORE
 
 
 
#line 2 "Src\main.c" /0
 
  
#line 1 "Src\common.h" /0
 
 
 
 
 
 
 
 








 
 
 
 
 
 
 
 
 


 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 typedef struct __internal_time_data__ {
 
 unsigned int  realLoadTime; 
 
 
 unsigned char mode;
 } sys_struct;
 
 typedef struct __internal_music_data__ {
 unsigned int  targetPeriod; 
 unsigned char musicZone,musicZoneLevel,musicZoneID;
 union{
 struct __reg_low_high__{
 unsigned char reg_low;  
 unsigned char reg_high; 
 
 
 
 
 
 }reg_8bit;
 unsigned int reg_16bit; 
 }regCache;
 
 
 } music_struct;
 
 typedef struct __internal_exint0_data__ {
 unsigned char ex0_isr_counter;
 unsigned int  lastFull,nowFull;
 unsigned int extwavePeroid,extwaveFreq; 
 unsigned char finishedFlag;
 } external_struct;
 
 typedef struct __ledseg_struct__{
 unsigned char ledString[3];
 unsigned char ledobject[3];
 
 
 }ledseg;
 
 typedef struct __output__{
 music_struct musicoutput;
 ledseg 		 ledoutput;
 }output_struct;
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
#line 3 "Src\main.c" /0
 


 
 
 
 sys_struct      timetable;
 external_struct externaldata;
 output_struct   outputGroup;


 
 void DetectSquareWave(void);
 void GenerateTarget(void);
 
 void          LedDisplayLoop   (unsigned   char led[3]);
 unsigned char LedDisplaySeg(const unsigned char ledstr);
 
 unsigned MusicRegisterFlash(void);
 void SquareFrequency2OutptFreq (unsigned int inputfreq);
 
 void Delay_ms(const unsigned char wait_time);


 
 
 void ex0_isr (void)         interrupt 0 {
 externaldata.ex0_isr_counter++;    
 }
 void ex1_isr_pushkey(void)  interrupt 2{
 
 
 
 
 
 
 Delay_ms(60);
 if (0==P3_3)
 {
 timetable.mode^=1; 
 }
 
 }
 void system_tick_isr(void)  interrupt 1 {
 
 
 
 timetable.realLoadTime++;
 
 }
 void music_change(void)     interrupt 3 {
 
 
 
 TH1=outputGroup.musicoutput.regCache.reg_8bit.reg_high;
 TL1=outputGroup.musicoutput.regCache.reg_8bit.reg_low ;
 
  P3_0^=1; 
 }


 
 void main (void) {
 





 
 IT0 = 1;    
 EX0 = 1;    
 
 
 IT1 = 1;    
 EX1 = 1;    
 
 TMOD =0x12;  
 
 TH1=0;TL1=0x66;  
  TL0 = 0xb3; 
  TH0  = 0xb3; 
 
 
 
 IP =0;
 PT0=1; 
 PX0=1; 
 
 
 
 ET1= 1;
 ET0=1; 
 
 ES = 0; 
 
 
 EA = 1;     
 
 
 
 
 TR1=1;
 TR0=1;
 
 while (1) {
 DetectSquareWave();
 
 
 
 
 
 GenerateTarget();
 
 
 MusicRegisterFlash();
 
 
 LedDisplayLoop(outputGroup.ledoutput.ledString);
 }
 }
 

 
 
 void DetectSquareWave(void) {
 
 
 if(externaldata.ex0_isr_counter==255) {
 
 
 
 
 externaldata.nowFull=timetable.realLoadTime;
 if(externaldata.lastFull>externaldata.nowFull) {  
 externaldata.extwavePeroid=(65535- externaldata.lastFull+externaldata.nowFull)*100.0/255 ;
 } else {  
 externaldata.extwavePeroid=(externaldata.nowFull- externaldata.lastFull )*100.0/255;
 } 
 externaldata.extwaveFreq=1e6/externaldata.extwavePeroid;
 externaldata.lastFull=externaldata.nowFull;
 } else {
 
 }
 }
 void GenerateTarget(void) {
 unsigned int freq_cache;
 
 


 
 
 
 if(0==timetable.mode){
 SquareFrequency2OutptFreq( externaldata.extwavePeroid );
 
 
 
 outputGroup.ledoutput.ledString[0]='0'+outputGroup.musicoutput.musicZoneLevel;
 outputGroup.ledoutput.ledString[1]='P';
 outputGroup.ledoutput.ledString[2]='0'+outputGroup.musicoutput.musicZoneID;
 
 
 }


 
 else{
 
 
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
 
 
 
 
 unsigned char LedDisplaySeg(const unsigned char ledstr) {
 
 
 const unsigned char code LedSegments_Dict[ 23]={
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
 'M', 
 'N', 
 };
 const unsigned char code LedSegments[ 23 ]={
 
 0x28, 
 0xed, 
 0x34, 
 0xa4, 
 0xe1, 
 0xa2, 
 0x22, 
 0xec, 
 0x20, 
 0xa0, 
 0x60, 
 0x23, 
 0x3a, 
 0x25, 
 0x32, 
 0x72, 
 0xdf, 
 0x22, 
 0x29, 
 0x7a, 
 0xa1, 
 0x0, 
 0xff, 
 };
 unsigned char i;
 
 for (i=0;i<23;i++){
 if(ledstr==LedSegments_Dict[i]){
 return LedSegments[i];
 }
 }
 
 return 0xFF;
 
 }
 
 void LedDisplayLoop(unsigned	char led[3]){
 outputGroup.ledoutput.ledobject[0]=LedDisplaySeg(led[0]);
 outputGroup.ledoutput.ledobject[1]=LedDisplaySeg(led[1]);
 outputGroup.ledoutput.ledobject[2]=LedDisplaySeg(led[2]);
 
 
 if(timetable.realLoadTime%100==0){ 
 
   P3_0=(0); P3_1=(1); P3_5=(1);;
  P1=outputGroup.ledoutput.ledobject[0];
 
 Delay_ms(1);
  P1=0xFF; 
 
 
   P3_0=(1); P3_1=(0); P3_5=(1);;    
  P1=outputGroup.ledoutput.ledobject[1];
 
 Delay_ms(1);
  P1=0xFF;
 
 
   P3_0=(1); P3_1=(1); P3_5=(0);;
  P1=outputGroup.ledoutput.ledobject[2];
 
 Delay_ms(1);
  P1=0xFF;
 }
 
 }
 
 
 
 unsigned MusicRegisterFlash(void) {
 unsigned int _half_period;
 
 if(0==timetable.mode)
  TR1=1;
 else{
  TR1=0;
 return 1;
 }
 
 
 
 
 _half_period=outputGroup.musicoutput.targetPeriod/2;
 outputGroup.musicoutput.regCache.reg_16bit= 65536- (12*1000*1000/1000000 )*_half_period /12;
 
 
 return 0;
 }
 
 void SquareFrequency2OutptFreq(unsigned int inputfreq){
 
 unsigned char i;
 unsigned int code MusicRhythm[ 21 ]= {
 
 
 3822, 
 3405, 
 3034, 
 2864, 
 2551, 
 2272, 
 2024, 
 1911, 
 1703, 
 1517, 
 1432, 
 1275, 
 1136, 
 1012, 
 955, 
 851, 
 758, 
 716, 
 638, 
 568, 
 506, 
 };
 for (i=21;i>=0;i--){
 
 if(inputfreq<=MusicRhythm[i]){
 
 outputGroup.musicoutput.targetPeriod = MusicRhythm[i];
 break;
 }
 }
 outputGroup.musicoutput.musicZone     =i;
 outputGroup.musicoutput.musicZoneLevel=i/7;
 outputGroup.musicoutput.musicZoneID   =i%7;
 }
 
 void Delay_ms(const unsigned char wait_time){
 
 unsigned int i,j;
 
 
 for(i=0;i<wait_time;i++ ){
 
 
 for(j=0;j<1000;j++);
 }
 }
 
 
 
 struct __we_are_history__ {
 char Date[15];
 char Time[10];
 char File[20];
 unsigned  Line;
 unsigned StdCVersion;
 
 };
 struct __we_are_history__ code Logging={"May 17 2016",
  "00:06:39",
  "Src\\main.c",
  379,
  1,
 };
 
 
 
#line 385 "Src\main.c" /1
 
 
 
 
 
 
 
 
 
 
 
 
 
#line 397 "Src\main.c" /0
