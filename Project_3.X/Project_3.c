/*===================================CPEG222=================================
 * Program:        CPEG222_project_2_template
 * Authors:     Chengmo Yang
 * Date:         9/13/2019
 * Description: This project 2 template uses on-board SSD to implement a counter (0-9999).
 * The speed of the counter can be adjusted and the when the SSD display 9999,
 * it will come back to 0 automatically.
 =============================================================================*/
/*------------------------ System setting part please DO NOT CHANGE ---------*/
#ifndef _SUPPRESS_PLIB_WARNING          //suppress the plib warning during compiling
    #define _SUPPRESS_PLIB_WARNING
#endif

#include <xc.h>   //Microchip XC processor header which links to the PIC32MX370512L header
#include "config.h" // Basys MX3 configuration header
#include "ssd.h"
#include "lcd.h"
#include "utils.h"
#include <plib.h>
#include <math.h>

#define R4 LATCbits.LATC14
#define R3 LATDbits.LATD0
#define R2 LATDbits.LATD1
#define R1 LATCbits.LATC13

#define C4 PORTDbits.RD9
#define C3 PORTDbits.RD11
#define C2 PORTDbits.RD10
#define C1 PORTDbits.RD8

#define led0 LATAbits.LATA0
#define led1 LATAbits.LATA1
#define led2 LATAbits.LATA2
#define led3 LATAbits.LATA3
#define led4 LATAbits.LATA4
#define led5 LATAbits.LATA5
#define led6 LATAbits.LATA6
#define led7 LATAbits.LATA7

#pragma config OSCIOFNC =	ON
#pragma config FPLLIDIV = DIV_2         // PLL Input Divider (2x Divider)
#pragma config FPLLMUL = MUL_20         // PLL Multiplier (20x Multiplier)
#pragma config FPLLODIV = DIV_1         // System PLL Output Clock Divider (PLL Divide by 1)
#pragma config FNOSC = PRIPLL           // Oscillator Selection Bits (Primary Osc w/PLL (XT+,HS+,EC+PLL))
#pragma config FSOSCEN = OFF            // Secondary Oscillator Enable (Disabled)
#pragma config POSCMOD = XT             // Primary Oscillator Configuration (XT osc mode)
#pragma config FPBDIV = DIV_8           // Peripheral Clock Divisor (Pb_Clk is Sys_Clk/8)
/*---------------------------------------------------------------------------*/
/* ---------------------------Forward Declarations-------------------------- */
void delay_ms(int ms);
void KEY_Init();
void CNConfig();
void __ISR(_CHANGE_NOTICE_VECTOR) CN_Handler(void);
void display_Numbers(int number);
void numberSub(int n1,int n2);

/* --------------------------Definitions------------------------------------ */
#define SYS_FREQ    (80000000L) // 80MHz system clock
int counter = 0;    // a 4-digit counter keeping track of value to display (0~9999)
int key;          
int count = 0;      // holder for number of keys hit
int buttonlock = 0; // button lock for debouncing
int mode = 1;       // mode of the calculator
int sign = 0;       // indicator for the sign of the sum
int left[4];        // number holder for the left side of the equation
int display[4];     // number holder for the sum
int sum = 0;        // total for operations
int summed = 0;     // check for if numbers have been summed yet
int version = 0;    // check for decimal or hexadecimal
int overflow;       // sign for overflow
int operator = 0;   // addition or subtraction
int wait = 0;
int done = 0;
int quotient = 0;
/* -------------------------Main----------------------------------- */
int main(void) 
{

    DDPCONbits.JTAGEN = 0;
    KEY_Init();
    LCD_Init();
    LED_Init();
    SSD_Init();
    CNConfig();
    
    while (1)
    {
        switch (mode) {
            case 1 : {
                LCD_DisplayClear();
                led0=led1=led2=led3=led4=led5=led6=led7=0;
                display[0] = display[1] = display[2] = display[3] = display[4] = 0;
                SSD_WriteDigits(display[0],display[1],display[2],display[3],0,0,0,0);
                key = 0;
                count = 0;
                left[0] = left[1] = left[2] = left[3] = left[4] = 0;
                delay_ms(100);
                buttonlock = 0;
                sum = 0;
                sign = 0;
                wait = 0;
                done = 0;
                mode = 2;
                break;
            }
            case 2 : {
                if (count == 1 && buttonlock == 1 && key < 10 && version == 0){
                    display[0] = key;
                    key = 0;
                    buttonlock = 0;
                }
                if (count == 2 && buttonlock == 1 && key < 10 && version == 0){
                    display[1] = display[0];
                    display[0] = key;
                    key = 0;
                    buttonlock = 0;
                }
                if (count == 3 && buttonlock == 1 && key < 10 && version == 0){
                    display[2] = display[1];
                    display[1] = display[0];
                    display[0] = key;
                    key = 0;
                    buttonlock = 0;
                }
                if (count > 3 && buttonlock == 1 && key < 10 && version == 0){
                    display[3] = display[2];
                    display[2] = display[1];
                    display[1] = display[0];
                    display[0] = key;
                    count = 3;
                    key = 0;
                    buttonlock = 0;
                }
                if (sign == 1){ // Negative Check
                    led0=led1=led2=led3=led4=led5=led6=led7=1;
                }
                if (sign == 0){ // Positive Check
                    led0=led1=led2=led3=led4=led5=led6=led7=0;
                }
                if (version == 0){  // Display Decimal
                    LCD_DisplayClear();
                    LCD_WriteStringAtPos("Calculation", 0, 0);
                    LCD_WriteStringAtPos("Decimal",1,0);
                }
                if (version == 1){  // Display Hex
                    LCD_DisplayClear();
                    LCD_WriteStringAtPos("Calculation", 0, 0);
                    LCD_WriteStringAtPos("Hexadecimal",1,0);
                }
                if (sum > 9999){ // Upper overflow
                    overflow = 0;
                    mode = 3;
                }
                if (sum < -9999){ // Under overflow
                    overflow = 1;
                    mode = 3;
                }
                if (key == 10 && wait == 0 && version == 0 && buttonlock == 1){ // A
                    operator = 0;
                    wait = 1;
                    done = 0;
                    left[0] = display[0];
                    left[1] = display[1];
                    left[2] = display[2];
                    left[3] = display[3];
                    display[0] = display[1] = display[2] = display[3] = 0;
                    key = 0;
                    buttonlock = 0;
                    count = 0;
                }
                if (key == 10 && wait == 1 && version == 0 && buttonlock == 1){ // A
                    operator = 0;
                    display[0] = display[1] = display[2] = display[3] = 0;
                    key = 14;
                    buttonlock = 0;
                }
                if (key == 11 && wait == 0 && version == 0 && buttonlock == 1){ // B
                    operator = 1;
                    wait = 1;
                    done = 0;
                    left[0] = display[0];
                    left[1] = display[1];
                    left[2] = display[2];
                    left[3] = display[3];
                    display[0] = display[1] = display[2] = display[3] = 0;
                    key = 0;
                    buttonlock = 0;
                    count = 0;
                }
                if (key == 11 && wait == 1 && version == 0 && buttonlock == 1){ // B
                    operator = 1;
                    display[0] = display[1] = display[2] = display[3] = 0;
                    key = 14;
                    buttonlock = 0;
                }
                if (key == 12 && buttonlock == 1 && version == 0){ // C
                    mode = 1;
                    buttonlock = 0;
                }
                if (key == 13 && buttonlock == 1 && version == 0){ // D
                    display[0] = display[1];
                    display[1] = display[2];
                    display[2] = display[3];
                    display[3] = 0;
                    if (count > 0){
                        count--;
                    }
                    key = 0;
                    buttonlock = 0;
                }
                if (key == 14 && buttonlock == 1 && version == 0){ // E
                    int i = 0;
                    int number1 = 0;
                    int number2 = 0;
                    if (operator == 0){
                        int i;
                        for (i = 0; i < 4; i++){
                            number1 = number1 + left[i]*pow(10,i);
                        }
                        for (i = 0; i < 4; i++){
                            number2 = number2 + display[i]*pow(10,i);
                        }
                        sum = number1 + number2;
                        if (sum < 0 && wait == 0){
                            sign = 1;
                            sum = sum *(-1);
                            display[0] = sum%10;
                            display[1] = (sum/10)%10;
                            display[2] = (sum/100)%10;
                            display[3] = (sum/1000);
                            sum = sum *(-1);
                        }
                        else if (sum > -1 && wait == 0){
                            sign = 0;
                            display[0] = sum%10;
                            display[1] = (sum/10)%10;
                            display[2] = (sum/100)%10;
                            display[3] = (sum/1000);

                       }
                    }
                    if (operator == 1){
                        int i;
                        for (i = 0; i < 4; i++){
                            number1 = number1 + left[i]*pow(10,i);
                        }
                        for (i = 0; i < 4; i++){
                            number2 = number2 + display[i]*pow(10,i);
                        }
                        sum = number1 - number2;
                        if (sum < 0 && wait == 0){
                            sign = 1;
                            sum = sum *(-1);
                            delay_ms(100);
                            display[0] = sum%10;
                            display[1] = (sum/10)%10;
                            display[2] = (sum/100)%10;
                            display[3] = (sum/1000);
                            sum = sum *(-1);
                        }
                        else if (sum > -1 && wait == 0){
                            sign = 0;
                            display[0] = sum%10;
                            display[1] = (sum/10)%10;
                            display[2] = (sum/100)%10;
                            display[3] = (sum/1000);
                        }
                    }
                    key = 0;
                    buttonlock = 0;
                }
                if (key == 15 && buttonlock == 1){ // F
                    version = 1 - version;
                    key = 0;
                    buttonlock = 0;
                }
                if (version == 0){ // SSD Display Parameters
                    SSD_WriteDigits(display[0],display[1],display[2],display[3],0,0,0,0);
                }
                if (version == 1){
                    int d[4];
                    if (sum == 0){
                        quotient = display[3] * 1000 + display[2] * 100 + display[1]*10 + display[0];
                    }
                    else quotient = sum;
                    d[0] = quotient % 16;
                    quotient = quotient / 16;
                    d[1] = quotient % 16;
                    quotient = quotient / 16;
                    d[2] = quotient % 16;
                    quotient = quotient / 16;
                    d[3] = quotient % 16;
                    quotient = quotient / 16;
                    SSD_WriteDigits(d[0],d[1],d[2],d[3],0,0,0,0);
                }
                break;
            }
            case 3 : {
                LCD_DisplayClear();
                LCD_WriteStringAtPos("Overflow",0,3);
                if (overflow == 1){                  
                    while (key != 12){
                        SSD_WriteDigits(16,16,16,16,0,0,0,0); //CHECK THESE NUMBERS
                        delay_ms(500);
                        SSD_WriteDigits(0,0,0,0,0,0,0,0);
                        delay_ms(500);
                    }
                }
                if (overflow == 0){
                    while (key != 12){
                        SSD_WriteDigits(16,16,16,16,0,0,0,0); //CHECK THESE NUMBERS
                        delay_ms(500);
                        SSD_WriteDigits(0,0,0,0,0,0,0,0);
                        delay_ms(500);
                    }
                }
                if (key == 12){
                    mode = 1;
                }
                break;
            }
        }
    }
}

/* ------------------------------------------------------------ 
**	delay_ms
**	Parameters:
**		ms - amount of milliseconds to delay (based on 80 MHz SSCLK)
**	Return Value:
**		none
**	Description:
**		Create a delay by counting up to counter variable
** ------------------------------------------------------------ */
void delay_ms(int ms){
	int		i,counter;
	for (counter=0; counter<ms; counter++)
    {
        for(i=0;i<1426;i++)
        {
        }   //software delay 1 millisec
    }
}
void KEY_Init() {
    TRISDbits.TRISD0 = 0;
    TRISDbits.TRISD1 = 0;
    ANSELDbits.ANSD1 = 0;
    
    TRISCbits.TRISC14 = 0;
    TRISCbits.TRISC13 = 0;
    TRISDbits.TRISD8 = 1;
    TRISDbits.TRISD9 = 1;
    TRISDbits.TRISD10 = 1;
    TRISDbits.TRISD11 = 1;
}
void CNConfig() {
    /* Make sure vector interrupts is disabled prior to configuration*/
    CNCONDbits.ON= 1;   //allport Dpins to trigger CN interrupts
    CNEND = 0xF00;      //configure PORTD pins 8-11asCNpins
    CNPUD = 0xF00;      //enable pullups on PORTD pins 8-11
    
    PORTD;//readporttoclearmismatchonCNpins
    IPC8bits.CNIP = 6;  // set CN priority to  6
    IPC8bits.CNIS = 3;  // set CN sub-priority to 3
    
    IFS1bits.CNDIF = 0;   //Clear interrupt flag status bit
    IEC1bits.CNDIE = 1;   //Enable CNinterrupt on port D
    
    INTEnableSystemMultiVectoredInt();
}
void __ISR(_CHANGE_NOTICE_VECTOR) CN_Handler(void) {
    
    // Disable CNinterrupts
    IEC1bits.CNDIE = 0; 
    
    // Debounce keys
    int i = 0;
    for(i=0;i<10000;i++){}
    
    R1 = 0; R2 = R3 = R4 = 1;
    if(C1 == 0 && buttonlock == 0){buttonlock = 1; key = 1;count++;}
    else if(C2 == 0  && buttonlock == 0){buttonlock = 1; key = 2;count++;}
    else if(C3 == 0  && buttonlock == 0){buttonlock = 1; key = 3;count++;}
    else if(C4 == 0  && buttonlock == 0){buttonlock = 1; key = 10;}
    
    R2 = 0; R1 = R3 = R4 = 1;
    if(C1 == 0  && buttonlock == 0){buttonlock = 1; key = 4;count++;}
    else if(C2 == 0  && buttonlock == 0){buttonlock = 1; key = 5;count++;}
    else if(C3 == 0  && buttonlock == 0){buttonlock = 1; key = 6;count++;}
    else if(C4 == 0  && buttonlock == 0){buttonlock = 1; key = 11;}
    
    R3 = 0; R1 = R4 = R2 = 1;
    if(C1 == 0  && buttonlock == 0){buttonlock = 1; key = 7;count++;}
    else if(C2 == 0  && buttonlock == 0){buttonlock = 1; key = 8;count++;}
    else if(C3 == 0  && buttonlock == 0){buttonlock = 1; key = 9;count++;}
    else if(C4 == 0  && buttonlock == 0){buttonlock = 1; key = 12;}
    
    R4 = 0; R1 = R3 = R2 = 1;
    if(C1 == 0  && buttonlock == 0){buttonlock = 1; key = 0;count++;}
    else if(C2 == 0  && buttonlock == 0){buttonlock = 1; key = 15;}
    else if(C3 == 0  && buttonlock == 0){buttonlock = 1; key = 14;wait = 0;}
    else if(C4 == 0  && buttonlock == 0){buttonlock = 1; key = 13;}
    
    R1 = R2 = R3 = R4 = 0;
    
    // Clear the interrupt flag
    IFS1bits.CNDIF = 0;
    
    // Reenable CNinterrupts
    IEC1bits.CNDIE = 1;
}