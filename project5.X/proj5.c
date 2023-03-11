/*
 * project 4 
 * Name: Roel Castillo-Hernandez

   
  @Description
        This file groups the functions that implement the servos.

 */

#include <xc.h>
#include <stdlib.h>
#include <string.h>
#include <proc/p32mx370f512l.h>
#include <sys/attribs.h>
#include "lcd.h"
#include "config.h"
#include "srv.h"
#include "ssd.h"
#include "adc.h"

#define btnc PORTFbits.RF0

#define led0 LATAbits.LATA0
#define led1 LATAbits.LATA1
#define led2 LATAbits.LATA2
#define led3 LATAbits.LATA3
#define led4 LATAbits.LATA4
#define led5 LATAbits.LATA5
#define led6 LATAbits.LATA6
#define led7 LATAbits.LATA7

//configure IR sensor to top of PMODB -- pins 1-4 (pin 5 is ground pin and pin 6 in input voltage(least i think))

#define S1 PORTDbits.RD9
#define S2 PORTDbits.RD11
#define S3 PORTDbits.RD10
#define S4 PORTDbits.RD8

#define SYS_FREQ (8000000L)
#define TMR_TIME .0000001
#define PBCLK 8000000

int record_size = 11000;

unsigned short recording[11000];

#pragma config OSCIOFNC = ON
#pragma config FPLLIDIV = DIV_2         // PLL Input Divider (2x Divider)
#pragma config FPLLMUL = MUL_20         // PLL Multiplier (20x Multiplier)
#pragma config FPLLODIV = DIV_1         // System PLL Output Clock Divider (PLL Divide by 1)
#pragma config FNOSC = PRIPLL           // Oscillator Selection Bits (Primary Osc w/PLL (XT+,HS+,EC+PLL))
#pragma config FSOSCEN = OFF            // Secondary Oscillator Enable (Disabled)
#pragma config POSCMOD = XT             // Primary Oscillator Configuration (XT osc mode)
#pragma config FPBDIV = DIV_2           // Peripheral Clock Divisor (Pb_Clk is Sys_Clk/2)


void configLed();
void configIR();
void delay_ms(int ms);

int key = 0;
int mode = 0;
int d1 = 20; 
int d2 = 20;
int d3 = 20;
int d4 = 20;;
int running = 0;
int blklock = 0;
int clap = 0;
int sound = 0;
int threshold;
int buttonlock = 0;
int start = 0;
int idxAudioBuf;
int seeblk = 0;
/*
 * note:  SRV_SetPulseMicroseconds0 -- goes to OC5RS (servo 0)
 *        SRV_SetPulseMicroseconds1 -- goes to OC4RS (servo 1)
 */

void __ISR(_TIMER_2_VECTOR, IPL7AUTO) Timer2ISR(void) 
{  
    IEC0bits.T2IE = 0;      // enable Timer3 interrupt 
    /*
     * some code between here
     */ 
    //PR2 = (int)((PBCLK /(float)(8 * 11000)) - 1); 
    //recording[(idxAudioBuf++)] = ADC_AnalogRead(4);
    
    IEC0bits.T2IE = 1;      // enable Timer3 interrupt    
    IFS0bits.T2IF = 0;      // clear Timer3 interrupt flag
} 
 // SRV_SetPulseMicroseconds0(); -- right motor
 // SRV_SetPulseMicroseconds1(); -- left motor
int main(){
    DDPCONbits.JTAGEN = 0;
    TRISFbits.TRISF0 = 1; //make btnc read to i/o
    /*configuring the timer2 and output compare module*/
    TMR2 = 0;
    T2CONbits.TCKPS = 8;     //1:1 prescale value 
    T2CONbits.TGATE = 0;     //not gated input (the default)
    T2CONbits.TCS = 0;       //PCBLK input (the default)
    T2CONbits.ON = 1;        //turn on Timer3
 
    OC2CONbits.ON = 0;       // Turn off OC1 while doing setup.
    OC2CONbits.OCM = 6;      // PWM mode on OC1; Fault pin is disabled
    OC2CONbits.OCTSEL = 0;   // Timer3 is the clock source for this Output Compare module
    OC2CONbits.ON = 1;       // Start the OC1 module  
    TRISBbits.TRISB14 = 0;
    ANSELBbits.ANSB14 = 0;
    RPB14R = 0xB;
    IPC2bits.T2IP = 7;      // interrupt priority
    IPC2bits.T2IS = 3;      // interrupt subpriority
    IEC0bits.T2IE = 1;      // enable Timer3 interrupt    
    IFS0bits.T2IF = 0;      // clear Timer3 interrupt flag
    macro_enable_interrupts();  // enable interrupts at CPU
    /*end of configuring the timer2 and output compare module*/ 
    ADC_Init();  
    configLed();
    SRV_Init();
    SSD_Init();
    LCD_Init();
    configIR();
    LCD_DisplayClear();
    //led0 = led1 = led2 = led3 = led4 = led5 = led6 = led7 = 0;
    while(1){
        LCD_WriteStringAtPos("Team 24:harambe", 0, 0);    
        switch(mode){
            case 0:{
                SSD_WriteDigits(d1, d2, d3, d4,0,1,0,0);
                LCD_WriteStringAtPos("waiting", 1, 0);
                if(btnc && buttonlock == 0){
                    start = 1;
                    buttonlock = 1;
                    LCD_DisplayClear();
                }
                if(!btnc && buttonlock == 1){
                    buttonlock = 0;
                }
                if(ADC_AnalogRead(4) > 700){
                    delay_ms(100);
                    int i = 0;
                    for (i = 0; i < 1426; i++){
                        if (ADC_AnalogRead(4) > 700){
                            start = 1;
                            SRV_SetPulseMicroseconds0(1000);
                            SRV_SetPulseMicroseconds1(2000);
                        }
                    }
                }
                while(start == 1){
                    LCD_WriteStringAtPos("Team 24:harambe", 0, 0);
                    d1++;
                    delay_ms(90);
                    if(d1 == 10){
                        d1 = 0;
                        d2++; 
                    }
                    if(d2 == 10){
                        d2 = 0;
                        d3++; 
                    }
                    if(d3 == 10){
                        d3 = 0;
                        d4++; 
                    }
                    if(d4 == 10){
                        d4 = 0;
                        d4++; 
                    }
                    if(d4 == 21){
                        d4 = 1;
                    } 
                    if(d3 == 21){
                        d3 = 1;
                    }
                    if(d1 == 21){
                        d1 = 1;
                    } 
                    if(d2 == 21){
                        d2 = 1;
                    }
                    /*
                    if(S4 == 0 && S3 == 0 && S2 == 0 && S1 == 0){                        
                        led0 = led1 = led2 = led3 = led4 = led5 = led6 = led7 = 1;
                        LCD_WriteStringAtPos("FWD         FWD", 1, 0); 
                        SRV_SetPulseMicroseconds0(1000);
                        SRV_SetPulseMicroseconds1(2000);                        
                    }
                    */
                    SSD_WriteDigits(d1, d2, d3, d4,0,1,0,0);                        
                    if(S4 == 1 && S3 == 0 && S2 == 0 && S1 == 1){//straight (1001) --------
                        led0 = led1 = led2 = led3 = led4 = led5 = led6 = led7 = 1;
                        LCD_WriteStringAtPos("FWD         FWD", 1, 0); 
                        SRV_SetPulseMicroseconds0(1000);
                        SRV_SetPulseMicroseconds1(2000);
                    }
                    if(S4 == 0 && S3 == 0 && S2 == 0 && S1 == 1){//sharp left (0001)
                        led0 = led1 = led2 = led3 = 1;
                        led4 = led5 = led6 = led7 = 0;
                        LCD_WriteStringAtPos("FWD         STP", 1, 0);  
                        SRV_SetPulseMicroseconds0(1000);
                        SRV_SetPulseMicroseconds1(1000);
                    }        
                    if(S4 == 0 && S3 == 0 && S2 == 1 && S1 == 1){//right on, left off(1) (0011)
                        led0 = led1 = led2 = led3 = 0;
                        led4 = led5 = led6 = led7 = 1;
                        LCD_WriteStringAtPos("STP         FWD", 1, 0);    
                        SRV_SetPulseMicroseconds0(1000);
                        SRV_SetPulseMicroseconds1(1500);
                    }
                    if(S4 == 0 && S3 == 1 && S2 == 1 && S1 == 1){//right on; left off (0111)
                        led0 = led1 = led2 = led3 = 0;
                        led4 = led5 = led6 = led7 = 1;
                        LCD_WriteStringAtPos("STP         FWD", 1, 0);    
                        SRV_SetPulseMicroseconds0(1000);
                        SRV_SetPulseMicroseconds1(1500);
                    }
                    if(S4 == 1 && S3 == 0 && S2 == 0 && S1 == 0){//sharp right(2) (1000)
                        led0 = led1 = led2 = led3 = 0;
                        led4 = led5 = led6 = led7 = 1;
                        LCD_WriteStringAtPos("STP         FWD", 1, 0);    
                        SRV_SetPulseMicroseconds0(2000);
                        SRV_SetPulseMicroseconds1(2000);
                    }
                    if(S4 == 1 && S3 == 1 && S2 == 0 && S1 == 0){//right off, left on(2) (1100)
                        led0 = led1 = led2 = led3 = 0;
                        led4 = led5 = led6 = led7 = 1;
                        LCD_WriteStringAtPos("FWD         STP", 1, 0);    
                        SRV_SetPulseMicroseconds0(1500);
                        SRV_SetPulseMicroseconds1(2000);
                    }
                    if(S4 == 1 && S3 == 1 && S2 == 1 && S1 == 0){//left on;right off (1110)
                        led0 = led1 = led2 = led3 = 0;
                        led4 = led5 = led6 = led7 = 1;
                        LCD_WriteStringAtPos("STP         FWD", 1, 0);    
                        SRV_SetPulseMicroseconds0(1500);
                        SRV_SetPulseMicroseconds1(2000);
                    }
                    if(S4 == 1 && S3 == 1 && S2 == 1 && S1 == 1){//reverse. off track(1111) --------
                        led0 = led1 = led2 = led3 = 0;
                        led4 = led5 = led6 = led7 = 1;
                        LCD_WriteStringAtPos("REV         REV", 1, 0);    
                        SRV_SetPulseMicroseconds0(2000);
                        SRV_SetPulseMicroseconds1(1000);
                    }   
                    /*
                    if(S4 == 0 && S3 == 0 && S2 == 0 && S1 == 0){//tries to stop
                        int i = 0;
                        for (i = 0; i < 2139; i++){
                        }
                        if(S4 == 0 && S3 == 0 && S2 == 0 && S1 == 0){
                            seeblk ++;
                        }
                        if(seeblk == 2){
                            SRV_SetPulseMicroseconds0(1500);
                            SRV_SetPulseMicroseconds1(1500);                            
                        }
                        
                    }
                    */
                    if(d3 < 5 && (S4 == 0 && S3 == 0 && S2 == 0 && S1 == 0)){
                        SRV_SetPulseMicroseconds0(1000);
                        SRV_SetPulseMicroseconds1(2000); 
                        led0 = led1 = led2 = led3 = led4 = led5 = led6 = led7 = 1;
                        LCD_WriteStringAtPos("FWD         FWD", 1, 0);
                    }  
                    if(d3 > 4 && d3 < 20 && (S4 == 0 && S3 == 0 && S2 == 0 && S1 == 0)){
                        SRV_SetPulseMicroseconds0(1500);
                        SRV_SetPulseMicroseconds1(1500); 
                        start = 0;
                        LCD_DisplayClear();
                        led0 = led1 = led2 = led3 = led4 = led5 = led6 = led7 = 1;
                    }  
                }
            break;
            }
        }
    }
    return 0;
}

void configLed(){
    TRISAbits.TRISA0 = 0;
    TRISAbits.TRISA1 = 0;
    TRISAbits.TRISA2 = 0;
    TRISAbits.TRISA3 = 0;
    TRISAbits.TRISA4 = 0;
    TRISAbits.TRISA5 = 0;
    TRISAbits.TRISA6 = 0;
    TRISAbits.TRISA7 = 0;
}

void configIR(){
    TRISDbits.TRISD8 = 1;
    TRISDbits.TRISD9 = 1;
    TRISDbits.TRISD10 = 1;
    TRISDbits.TRISD11 = 1;
}

void delay_ms(int ms){
	int		i,counter;
	for (counter=0; counter<ms; counter++)
    {
        for(i=0;i<1426;i++)
        {
        }   //software delay 1 millisec
    }
}