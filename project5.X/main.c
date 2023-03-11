/*
 * Name: Roel Castillo-Hernandez
 * 
 */
/*
#include <xc.h>
#include <stdlib.h>
#include <string.h>
#include <proc/p32mx370f512l.h>
#include <sys/attribs.h>
#include "lcd.h"
#include "config.h"
#include "srv.h"

#define sw0 PORTFbits.RF3 // read SW0
#define sw1 PORTFbits.RF5 // read SW1
#define sw2 PORTFbits.RF4 // read SW2
#define sw3 PORTDbits.RD15 // read SW3
#define sw4 PORTDbits.RD14 // read SW4
#define sw5 PORTBbits.RB11 // read SW5
#define sw6 PORTBbits.RB10 // read SW6
#define sw7 PORTBbits.RB9 // read SW7

#define led0 LATAbits.LATA0 
#define led1 LATAbits.LATA1
#define led2 LATAbits.LATA2
#define led3 LATAbits.LATA3
#define led4 LATAbits.LATA4
#define led5 LATAbits.LATA5
#define led6 LATAbits.LATA6
#define led7 LATAbits.LATA7


#pragma config OSCIOFNC = ON
#pragma config FPLLIDIV = DIV_2         // PLL Input Divider (2x Divider)
#pragma config FPLLMUL = MUL_20         // PLL Multiplier (20x Multiplier)
#pragma config FPLLODIV = DIV_1         // System PLL Output Clock Divider (PLL Divide by 1)
#pragma config FNOSC = PRIPLL           // Oscillator Selection Bits (Primary Osc w/PLL (XT+,HS+,EC+PLL))
#pragma config FSOSCEN = OFF            // Secondary Oscillator Enable (Disabled)
#pragma config POSCMOD = XT             // Primary Oscillator Configuration (XT osc mode)
#pragma config FPBDIV = DIV_2           // Peripheral Clock Divisor (Pb_Clk is Sys_Clk/2)


void configSw();
void configLed();

int ver;
int rev;
*/

/*
 * note:  SRV_SetPulseMicroseconds0 -- goes to OC5RS (servo 0)
 *        SRV_SetPulseMicroseconds1 -- goes to OC4RS (servo 1)
 */

/*
int main(){
    DDPCONbits.JTAGEN = 0;
    configSw();
    configLed();
    SRV_Init();
    LCD_Init();
    LCD_DisplayClear();
    
    while(1){
        LCD_WriteStringAtPos("Team 24:Harambe", 0, 0);
        if(!sw1 && !sw0){//stop
            led0 = led1 = led2 = led3 = 0;
            SRV_SetPulseMicroseconds0(1500);  
            ver = 1;
        }
        if(!sw1 && sw0){//cw
            led1 = led0 = 0;
            led3 = led2 = 1;
            SRV_SetPulseMicroseconds0(1000);
            ver = 2;
        }
        if(sw1 && !sw0){//ccw
            led0 = led1 = 1;
            led3 = led2 = 0;
            SRV_SetPulseMicroseconds0(2000); 
            ver = 3;
        }
        if(sw1 && sw0){//stop
            led0 = led1 = led2 = led3 = 0;
            SRV_SetPulseMicroseconds0(1500); 
            ver = 1;
        }
        if(!sw7 && !sw6){
            led4 = led5 = led6 = led7 = 0;
            SRV_SetPulseMicroseconds1(1500);
            rev = 1;      
        }
        if(!sw7 && sw6){
            led5 = led4 = 1;
            led7 = led6 = 0; 
            SRV_SetPulseMicroseconds1(2000);
            rev = 2;      
        }
        if(sw7 && !sw6){
            led5 = led4 = 0;
            led7 = led6 = 1; 
            SRV_SetPulseMicroseconds1(1000);
            rev = 3;      
        }
        if(sw7 && sw6){
            led4 = led5 = led6 = led7 = 0;
            SRV_SetPulseMicroseconds1(1500);
            rev = 1;      
        } 
        if(ver == 1 && rev == 1){
            LCD_WriteStringAtPos("STP         STP", 1, 0);
        }
        if(ver == 1 && rev == 2){
            LCD_WriteStringAtPos("FWD         STP", 1, 0);           
        }
        if(ver == 1 && rev == 3){
            LCD_WriteStringAtPos("REV         STP", 1, 0);           
        }
        if(ver == 2 && rev == 1){
            LCD_WriteStringAtPos("STP         FWD", 1, 0);           
        }
        if(ver == 2 && rev == 2){
            LCD_WriteStringAtPos("FWD         FWD", 1, 0);           
        }
        if(ver == 2 && rev == 3){
            LCD_WriteStringAtPos("REV         FWD", 1, 0);           
        }
        if(ver == 3 && rev == 1){
            LCD_WriteStringAtPos("STP         REV", 1, 0);           
        }
        if(ver == 3 && rev == 2){
            LCD_WriteStringAtPos("FWD         REV", 1, 0);           
        }
        if(ver == 3 && rev == 3){
            LCD_WriteStringAtPos("REV         REV", 1, 0);           
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

void configSw(){
    TRISFbits.TRISF3 = 1; // RF3 (SW0) configured as input
    TRISFbits.TRISF5 = 1; // RF5 (SW1) configured as input
    TRISFbits.TRISF4 = 1; // RF4 (SW2) configured as input
    TRISDbits.TRISD15 = 1; // RD15 (SW3) configured as input
    TRISDbits.TRISD14 = 1; // RD14 (SW4) configured as input
    TRISBbits.TRISB11 = 1; // RB11 (SW5) configured as input
    ANSELBbits.ANSB11 = 0; // RB11 (SW5) disabled analog
    TRISBbits.TRISB10 = 1; // RB10 (SW6) configured as input
    ANSELBbits.ANSB10 = 0; // RB10 (SW6) disabled analog
    TRISBbits.TRISB9 = 1; // RB9 (SW7) configured as input
    ANSELBbits.ANSB9 = 0; // RB9 (SW7) disabled analog
}
*/