#include <reg2051.h>
void delay(int t){
    unsigned int i,j;
    for(i=0;i<t;i++){
        for(j=0;j<1000;j++);
    }
}
void main(void){
    unsigned char Ledcode[]={
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
    int i;
    while(1)
    for(i=0;i<15;i++){
        P1=Ledcode[i];
        delay(500);//500ms
    }
    
}