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
void showNumber(int display_value);
void configLed();
void configBtn();
void configSSD();
/* --------------------------Definitions------------------------------------ */
#define SYS_FREQ    (80000000L) // 80MHz system clock
int counter = 0;    // a 4-digit counter keeping track of value to display (0~9999)
int smallcount = 0; // delay count for counter
int flashcount = 0; // counter to create led flashing
int mode = 0;       // keep track of current mode
int position = 0;   // position for setting the hours and minutes
int REFRESH_RATE = SYS_FREQ / 16000;   // refresh rate for SSD
int buttonlock = 0;     // lock for buttons
int alarm = 1200;    // display for the alarm //Roel: was supposed be initialized to 1200
int running = 0;    // Indicates whether clock is running
int blank;          // Int for showing nothing on display

const unsigned char digitSeg[]= 
{
    0b1000000, // 0
    0b1111001, // 1
    0b0100100, // 2
    0b0110000, // 3
    0b0011001, // 4
    0b0010010, // 5
    0b0000010, // 6
    0b1111000, // 7
    0b0000000, // 8
    0b0010000, // 9
    0b0001000, // A
    0b0000011, // b
    0b1000110, // C
    0b0100001, // d
    0b0000110, // E
    0b0001110,  // F
    0b0001001,  // H
    0b0111111,  // -symbol
    0b1111111   // blank display (all off)
};

//Segments
#define SegCA   LATGbits.LATG12
#define SegCB   LATAbits.LATA14
#define SegCC   LATDbits.LATD6
#define SegCD   LATGbits.LATG13
#define SegCE   LATGbits.LATG15
#define SegCF   LATDbits.LATD7
#define SegCG   LATDbits.LATD13
#define SegDF   LATGbits.LATG14

//Display Select
#define AN0     LATBbits.LATB12
#define AN1     LATBbits.LATB13
#define AN2     LATAbits.LATA9
#define AN3     LATAbits.LATA10

//Buttons
#define BtnC    PORTFbits.RF0
#define BtnU    PORTBbits.RB1
#define BtnL    PORTBbits.RB0
#define BtnR    PORTBbits.RB8
#define BtnD    PORTAbits.RA15

//LEDs
#define led0 LATAbits.LATA0
#define led1 LATAbits.LATA1
#define led2 LATAbits.LATA2
#define led3 LATAbits.LATA3
#define led4 LATAbits.LATA4
#define led5 LATAbits.LATA5
#define led6 LATAbits.LATA6
#define led7 LATAbits.LATA7

/* -------------------------Main----------------------------------- */
int main(void) 
{
    led0 = led1 = led2 = led3 = led4 = led5 = led6 = led7 = 0;
    DDPCONbits.JTAGEN = 0;      // Statement is required to use Pin RA0 as IO  
    configLed();
    configBtn();
    configSSD();
    /*---------------Initiate state of clock (00:00) and alarm (12:00) ---------*/
    //configLed();
    /*
    TRISBbits.TRISB12 = 0; //RB12 set as output 
    ANSELBbits.ANSB12 = 0; //RB12 analog functionality disabled 
    TRISBbits.TRISB13 = 0; //RB13 set as output 
    ANSELBbits.ANSB13 = 0; //RB13 analog functionality disabled 
    TRISAbits.TRISA9 = 0; //RA9 set as output 
    TRISAbits.TRISA10 = 0; //RA10 set as output 
    TRISGbits.TRISG12 = 0; //RG12 set as output 
    TRISAbits.TRISA14 = 0; //RA14 set as output 
    TRISDbits.TRISD6 = 0; //RD6 set as output 
    TRISGbits.TRISG13 = 0; //RG13 set as output 
    TRISGbits.TRISG15 = 0; //RG15 set as output 
    TRISDbits.TRISD7 = 0; //RD7 set as output 
    TRISDbits.TRISD13 = 0; //RD13 set as output         
    TRISGbits.TRISG14 = 0; //RG14 set as output
    */    
    //TRISA = 0x0000;       //Set Port A bits to output pins
    /*    
    TRISBbits.TRISB1 = 1; // RB1 (BTNU) configured as input
    ANSELBbits.ANSB1 = 0; // RB1 (BTNU) disabled analog
    TRISBbits.TRISB0 = 1; // RB1 (BTNL) configured as input
    ANSELBbits.ANSB0 = 0; // RB1 (BTNL) disabled analog
    TRISFbits.TRISF4 = 1; // RF0 (BTNC) configured as input 
    TRISBbits.TRISB8 = 1; // RB8 (BTNR) configured as input
    ANSELBbits.ANSB8 = 0;// RB8 (BTNR) disabled analog
    TRISAbits.TRISA15 = 1;// RA15 (BTND) configured as input
    */
    while (1)
    {
        switch(mode)
        {
            case 0:
                led0 = 1;
                led1 = 0;
                showNumber(counter);
                 if (BtnR && !buttonlock)
                {
                    buttonlock = 1;
                    counter = 0;
                }
                if (BtnD && !buttonlock)
                {
                    buttonlock = 1;
                    if (position && counter > 99)
                    {
                        counter = counter - 100;
                    }
                    else if (position && counter < 100)
                    {
                        counter = counter % 100 + 2300;
                    }
                    else if (!position && counter > 0)
                    {
                        counter = counter - 1;
                    }
                    else 
                    {
                        counter = 2359;
                    }
                }
                if (BtnU && !buttonlock)
                {
                    buttonlock = 1;                    
                    if (position && counter < 2300)
                    {
                        counter = counter + 100;
                    }
                    else if (position && counter > 2300)
                    {
                        counter = counter % 100;
                    }
                    else if (!position && counter < 2400)
                    {
                        counter = counter + 1;
                    }
                    else 
                    {
                        counter = 1;
                    }
                }
                if (BtnL && !buttonlock)
                {
                    buttonlock = 1;
                    position = 1 - position;
                }
                
                if (BtnC && !buttonlock)
                {
                    buttonlock = 1;
                    position = 0;
                    mode = 1;
                }
                break;
            case 1:
                led0 = 0;
                led1 = 1;
                showNumber(alarm);
                if (BtnR && !buttonlock)
                {
                    buttonlock = 1;
                    counter = 0;
	                mode = 0; 
                }
                if (BtnD && !buttonlock)
                {
                    buttonlock = 1;
                    if (position && alarm > 99)
                    {
                        alarm = alarm - 100;
                    }
                    else if (position && alarm < 100)
                    {
                        alarm = alarm % 100 + 2300;
                    }
                    else if (!position && alarm > 0)
                    {
                        alarm = alarm - 1;
                    }
                    else 
                    {
                        alarm = 2359;
                    }
                }
                if (BtnU && !buttonlock)
                {
                    buttonlock = 1;
                    if (position && alarm < 2300)
                    {
                        alarm = alarm + 100;
                    }
                    else if (position && alarm > 2300)
                    {
                        alarm = alarm % 100;
                    }
                    else if (!position && alarm < 2400)
                    {
                        alarm = alarm + 1;
                    }
                    else 
                    {
                        alarm = 1;
                    }
                }
                if (BtnL && !buttonlock)
                {
                    buttonlock = 1;
                    position = 1 - position;
                }
                if (BtnC && !buttonlock)
                {
                    buttonlock = 1;
                    position = 0;
                    mode = 2;
                }
                break;
            case 2:
                running = 1;
                led0 = 0;
                led1 = 0;
                showNumber(counter);
                if ((counter % 100) > 59 )
                {
                    counter = (counter / 100) * 100 + 100;
                }
                if (counter > 2359)
                {
                    counter = 0;
                }
                if (running)
                {
                    smallcount++;
                }
                if (smallcount > 62)
                {
                    counter++;
                    smallcount = 0;
                }
                if(BtnR && !buttonlock){
                    buttonlock = 1;
                    mode = 0; 
                }
                if(BtnC && !buttonlock){ 
                    buttonlock = 1;
                    mode = 1;
                }
                if (counter == alarm)
                {
                    mode = 3;
                }
                break;
            case 3:
                flashcount++;
                if (flashcount < 6)
                {
                    led0 = 1;
                    led1 = 1;
                    showNumber(blank);
                }
                if (flashcount > 6)
                {
                    led0 = 0;
                    led1 = 0;
                    showNumber(alarm);
                }
                if (flashcount > 12)
                {
                    flashcount = 0;
                }
                if(alarm != counter){
                    mode = 2;
                }
                if (BtnC && BtnR && !buttonlock)
                {
                    buttonlock = 1;
                    mode = 2;
                }
                if ((counter % 100) > 59 )
                {
                    counter = (counter / 100) * 100 + 100;
                }
                if (counter > 2359)
                {
                    counter = 0;
                }
                if (running)
                {
                    smallcount++;
                }
                if (smallcount > 62)
                {
                    counter++;
                    smallcount = 0;
                }
                break;
        }
        if (!BtnC && !BtnL && !BtnD && !BtnU && !BtnR && buttonlock)
        {
            buttonlock = 0;
        }
    }
}
/*-------------------------------------------------------------
**  displayDigit
**  Parameters:
**      value - value to convert to segments
**  Return Value:
**      none
**  Description:
**      Drive the segments for certain values
 ------------------------------------------------------------*/
void displayDigit(unsigned char value)
{
    SegCA = value & 1;
    SegCB = (value >> 1) & 1;
    SegCC = (value >> 2) & 1;
    SegCD = (value >> 3) & 1;
    SegCE = (value >> 4) & 1;
    SegCF = (value >> 5) & 1;
    SegCG = (value >> 6) & 1;
}
/*-------------------------------------------------------------
**  showNumber
**  Parameters:
**      display_value - minutes to display
**  Return Value:
**      none
**  Description:
**      Display minutes on an SSD
 ------------------------------------------------------------*/
void showNumber(int display_value)
{
    int i;
    AN0 = 0; AN1 = AN2 = AN3 = 1;
    displayDigit(digitSeg[display_value%10]);
    for (i = 0; i < REFRESH_RATE; i++);
    
    AN1 = 0; AN0 = AN2 = AN3 = 1;
    displayDigit(digitSeg[(display_value/10)%10]);
    for (i = 0; i < REFRESH_RATE; i++);
    
    AN2 = 0; AN1 = AN3 = AN0 = 1;
    displayDigit(digitSeg[(display_value/100)%10]);
    for (i = 0; i < REFRESH_RATE; i++);
    
    AN3 = 0; AN0 = AN2 = AN1 = 1;
    displayDigit(digitSeg[(display_value/1000)%10]);
    for (i = 0; i < REFRESH_RATE; i++);
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

void configLed(){
    TRISAbits.TRISA0 = 0x0000;
    TRISAbits.TRISA1 = 0x0000;
    TRISAbits.TRISA2 = 0x0000;
    TRISAbits.TRISA3 = 0x0000;
    TRISAbits.TRISA4 = 0x0000;
    TRISAbits.TRISA5 = 0x0000;
    TRISAbits.TRISA6 = 0x0000;
    TRISAbits.TRISA7 = 0x0000;

}

void configBtn(){
    TRISBbits.TRISB1 = 1; // RB1 (BTNU) configured as input
    ANSELBbits.ANSB1 = 0; // RB1 (BTNU) disabled analog
    TRISBbits.TRISB0 = 1; // RB1 (BTNL) configured as input
    ANSELBbits.ANSB0 = 0; // RB1 (BTNL) disabled analog
    TRISFbits.TRISF4 = 1; // RF0 (BTNC) configured as input 
    TRISBbits.TRISB8 = 1; // RB8 (BTNR) configured as input
    ANSELBbits.ANSB8 = 0;// RB8 (BTNR) disabled analog
    TRISAbits.TRISA15 = 1;// RA15 (BTND) configured as input
}

void configSSD(){
    TRISBbits.TRISB12 = 0; //RB12 set as output 
    ANSELBbits.ANSB12 = 0; //RB12 analog functionality disabled 
    TRISBbits.TRISB13 = 0; //RB13 set as output 
    ANSELBbits.ANSB13 = 0; //RB13 analog functionality disabled 
    TRISAbits.TRISA9 = 0; //RA9 set as output 
    TRISAbits.TRISA10 = 0; //RA10 set as output 
    TRISGbits.TRISG12 = 0; //RG12 set as output 
    TRISAbits.TRISA14 = 0; //RA14 set as output 
    TRISDbits.TRISD6 = 0; //RD6 set as output 
    TRISGbits.TRISG13 = 0; //RG13 set as output 
    TRISGbits.TRISG15 = 0; //RG15 set as output 
    TRISDbits.TRISD7 = 0; //RD7 set as output 
    TRISDbits.TRISD13 = 0; //RD13 set as output         
    TRISGbits.TRISG14 = 0; //RG14 set as output
}