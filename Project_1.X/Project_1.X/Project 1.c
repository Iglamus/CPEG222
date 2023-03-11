/*===================================CPEG222====================================
 * Program:		CPEG222_demo_sample.c
 * Authors: 	Chengmo Yang
 * Date: 		8/30/2019
 * Description: demo_sample uses on-board BTNs to control the lighting sequence of
 * the on-board LEDs 0 thru 7.  The initial state is LD0 ON.
 * The program waits for a BTN press with the following results:
 * -BTNC turns on the next LED and turns off current one from LD0 - LD7
 * Input: Button press
 * Output: LED is turned on and off one by one.
==============================================================================*/
/*------------------ Board system settings. PLEASE DO NOT MODIFY THIS PART ----------*/
#ifndef _SUPPRESS_PLIB_WARNING          //suppress the plib warning during compiling
    #define _SUPPRESS_PLIB_WARNING      
#endif
#pragma config FPLLIDIV = DIV_2         // PLL Input Divider (2x Divider)
#pragma config FPLLMUL = MUL_20         // PLL Multiplier (20x Multiplier)
#pragma config FPLLODIV = DIV_1         // System PLL Output Clock Divider (PLL Divide by 1)
#pragma config FNOSC = PRIPLL           // Oscillator Selection Bits (Primary Osc w/PLL (XT+,HS+,EC+PLL))
#pragma config FSOSCEN = OFF            // Secondary Oscillator Enable (Disabled)
#pragma config POSCMOD = XT             // Primary Oscillator Configuration (XT osc mode)
#pragma config FPBDIV = DIV_8           // Peripheral Clock Divisor (Pb_Clk is Sys_Clk/8)
/*----------------------------------------------------------------------------*/
     
#include <xc.h>   //Microchip XC processor header which links to the PIC32MX370512L header
// #include "config.h" // Basys MX3 configuration header
// #include "led.h"    // Digilent Library for using the on-board LEDs
// #include "btn.h"    // Digilent Library for using the on-board BTNs

/* --------------------------- Forward Declarations-------------------------- */
void delay_ms(int ms);

/* -------------------------- Definitions------------------------------------ */
#define SYS_FREQ    (80000000L) // 80MHz system clock
#define led0        LATAbits.LATA0  //define led0
#define led1        LATAbits.LATA1  //define led1
#define led2        LATAbits.LATA2  //define led2
#define led3        LATAbits.LATA3  //define led3
#define led4        LATAbits.LATA4  //define led4
#define led5        LATAbits.LATA5  //define led5
#define led6        LATAbits.LATA6  //define led6
#define led7        LATAbits.LATA7  //define led7
#define btnc        PORTFbits.RF0   //define button C
#define sw0         PORTFbits.RF3   //define switch 0
int LD_position = 0x01;         // Initially turn on LD0
int buttonLock = 0;             // Variable to "lock" the button
int delay = 500;                // Variable to set delay to 100 ms
int mode = 0;                   // Variable to set current mode
int swval = 0;              // Variable to store sw0 value
/* ----------------------------- Main --------------------------------------- */

int main(void) 
{
    DDPCONbits.JTAGEN = 0;      // Statement is required to use Pin RA0 as IO
    // LED_Init();         //	This function initializes the hardware involved in the LED module. 
                        //  The pins corresponding to LEDs are initialized as digital outputs.
    // SWT_Init();         // Initialize the switches.
    // BTN_Init();         //  This function initializes the hardware involved in the BTN module.
    // LED_SetGroupValue(LD_position); // Set the initial LD0 on
    ANSELA |= 0xFFFF;       //Set all A ports as digital
    TRISA = 0x0000;         //Set Port A bits to output pins
    TRISF |= 0x80;          //Set PortA bit 15 (BTNC) to digital inputs
    
    led0 = led1 = led2 = led3 = led4 = led5 = led6 = led7 = 0;  //set leds to off
    while (1) 
    {
/*-------------------- Main logic and actions start --------------------------*/
        if(buttonLock && !btnc) // Check button status and in this case upper button(BTNU)) is used.
                                             // Please check the btn.c if you want to use other button
        {
            delay_ms(delay);        // wait 100ms or you can change the value to
                                    // make it more sensitive to the button by changing the variable delay
            buttonLock = 0; //unlock buttons if all off
        }
        if (btnc && !buttonLock && mode==0)
        {
            
            delay_ms(100); // delay
            
            led0 = 1;  // initial conditions for mode 1
            led1 = 1;
            led2 = 1;
            led3 = 1;
            led4 = 1;
            led5 = 1;
            led6 = 1;
            led7 = 1;
            
            delay_ms(100);
            mode = 1; //assign mode 1
            
        }
        if (btnc && !buttonLock && mode==1) // check for button lock and current mode
        {
                delay_ms(100);  //wait 100ms
                swval = sw0; //assign switch value
                buttonLock = 1;   //lock buttons
                mode = 2; //enter mode 2
                
                int counter = 0;
                
                if (swval == 0)
                {
                    delay_ms(delay);
                    led7 = 0;
                    delay_ms(delay);
                    led6 = 0;
                    delay_ms(delay);
                    led5 = 0;
                    delay_ms(delay);
                    led4 = 0;
                    delay_ms(delay);
                    led3 = 0;
                    delay_ms(delay);
                    led2 = 0;
                    delay_ms(delay);
                    led1 = 0;
                    delay_ms(delay);
                    led0 = 0;
                }
                else 
                {
                    delay_ms(delay);
                    led0 = 0;
                    delay_ms(delay);
                    led1 = 0;
                    delay_ms(delay);
                    led2 = 0;
                    delay_ms(delay);
                    led3 = 0;
                    delay_ms(delay);
                    led4 = 0;
                    delay_ms(delay);
                    led5 = 0;
                    delay_ms(delay);
                    led6 = 0;
                    delay_ms(delay);
                    led7 = 0;
                }
                
                mode = 3; //enter mode 3
        }          
        if (btnc && !buttonLock && mode==3) // check for button lock and current mode
        {
                delay_ms(100);  //wait 100ms
                swval = sw0; //assign switch value
                buttonLock = 1;   //lock buttons
                mode = 4; //enter mode 4
                
                int counter = 0;  //initialize counter
                
                if (swval == 0)
                {
                    delay_ms(delay);
                    led7 = 1;
                    delay_ms(delay);
                    led6 = 1;
                    delay_ms(delay);
                    led5 = 1;
                    delay_ms(delay);
                    led4 = 1;
                    delay_ms(delay);
                    led3 = 1;
                    delay_ms(delay);
                    led2 = 1;
                    delay_ms(delay);
                    led1 = 1;
                    delay_ms(delay);
                    led0 = 1;
                }
                else 
                {
                    delay_ms(delay);
                    led0 = 1;
                    delay_ms(delay);
                    led1 = 1;
                    delay_ms(delay);
                    led2 = 1;
                    delay_ms(delay);
                    led3 = 1;
                    delay_ms(delay);
                    led4 = 1;
                    delay_ms(delay);
                    led5 = 1;
                    delay_ms(delay);
                    led6 = 1;
                    delay_ms(delay);
                    led7 = 1;
                }
                
                mode = 1; //enter mode 1
    }

/*--------------------- Action and logic end ---------------------------------*/
}
}
/* ----------------------------------------------------------------------------- 
**	delay_ms
**	Parameters:
**		ms - amount of milliseconds to delay (based on 80 MHz SSCLK)
**	Return Value:
**		none
**	Description:
**		Create a delay by counting up to counter variable
** -------------------------------------------------------------------------- */
void delay_ms(int ms){
    int i,counter;
	for (counter=0; counter<ms; counter++){
        for(i=0;i<1426;i++){}   //software delay 1 millisec
    }
}
