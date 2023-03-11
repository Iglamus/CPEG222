/*
 * CPEG222
 * PROJECT4
 * names: Roel Castillo-Hernandez
 *        Christopher McGlinn
 * Description: 
 *  this is the code for project4. in mode 1, various sounds are being played according to the switch value. For mode2, we record a sound. and
 *  in mode 3 the voice is played back, 
 */
#include <xc.h>
#include <stdlib.h>
#include <string.h>
#include <sys/attribs.h>
#include <proc/p32mx370f512l.h>
#include "config.h"
#include "lcd.h"
#include "adc.h"


#define BtnC PORTFbits.RF0
#define BtnR PORTBbits.RB8

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

#define SYS_FREQ (8000000L)
#define TMR_TIME .0000001
#define PBCLK 8000000

int record_size = 55000;


#pragma config OSCIOFNC = ON
#pragma config FPLLIDIV = DIV_2         // PLL Input Divider (2x Divider)
#pragma config FPLLMUL = MUL_20         // PLL Multiplier (20x Multiplier)
#pragma config FPLLODIV = DIV_1         // System PLL Output Clock Divider (PLL Divide by 1)
#pragma config FNOSC = PRIPLL           // Oscillator Selection Bits (Primary Osc w/PLL (XT+,HS+,EC+PLL))
#pragma config FSOSCEN = OFF            // Secondary Oscillator Enable (Disabled)
#pragma config POSCMOD = XT             // Primary Oscillator Configuration (XT osc mode)
#pragma config FPBDIV = DIV_8           // Peripheral Clock Divisor (Pb_Clk is Sys_Clk/8)

// Forward Declarations
void shutdown();
void configLed();
void configSW();
void configBtn();
void delay_ms(int ms);

// global variables
int x = 0;
int ms = 0;
int sec = 0;
char q[20];
char s[20];
int playbackposition, recordingposition;
int mode = 1;
int duty;
int switchVal;
int hold = 0;
int duty;
int key2;
int key3;
int buttonLock = 0;
int ledTimer = 0;
int recorded = 0;

// Values for different notes
unsigned short A5[] = {512,767,954,1023,954,767,512,256,69,0,69,256,512};
unsigned short G[] = {512,733,911,1010,1010,911,733,512,290,112,13,13,112,290,512};
unsigned short F[] = {512,720,892,998,1020,954,812,618,405,211,69,3,25,131,303,511};
unsigned short E[] = {512,707,873,984,1023,984,873,707,512,316,150,39,0,39,150,316};
unsigned short D[] = {512,678,826,940,1007,1021,980,888,755,596,427,268,135,43,2,16,83,197,345,512};
unsigned short C[] = {512,670,812,925,998,1023,998,925,812,670,512,353,211,98,25,0,25,98,211,353,512};
unsigned short B[] = {512,656,788,898,977,1018,1018,977,898,788,656,512,367,235,125,46,5,5,46,125,235,367,511};
unsigned short A4[] = {512,639,758,862,943,998,1022,1014,974,906,812,700,576,447,323,211,117,49,9,1,25,80,161,265,384,512};
unsigned short recording[55000];

#define A4_SIZE  (sizeof(A4) / sizeof(A4[0]))
#define B_SIZE  (sizeof(B) / sizeof(B[0]))
#define C_SIZE  (sizeof(C) / sizeof(C[0]))
#define D_SIZE  (sizeof(D) / sizeof(D[0]))
#define E_SIZE  (sizeof(E) / sizeof(E[0]))
#define F_SIZE  (sizeof(F) / sizeof(F[0]))
#define G_SIZE  (sizeof(G) / sizeof(G[0]))
#define A5_SIZE  (sizeof(A5) / sizeof(A5[0]))

void __ISR(_TIMER_2_VECTOR, IPL7AUTO) Timer2ISR(void)
{  
    unsigned short play;
    IEC0bits.T2IE = 0;      // enable Timer2 interrupt                  
    if(sw0 && mode == 1){
        PR2 = (int)((PBCLK /(float)(8 * 11000)) - 1);
        led0 = 1;
        duty = A4[(++recordingposition) % A4_SIZE];    
        OC2RS = (int)((PR2 + 1) * ((float)duty) / 1023);
    }
    if(sw1 && mode == 1){
        PR2 = (int)((PBCLK /(float)(8 * 11000)) - 1);
        led1 = 1;
        duty = B[(++recordingposition) % B_SIZE];    
        OC2RS = (int)((PR2 + 1) * ((float)duty) / 1023);
    }
    if(sw2 && mode == 1){
        PR2 = (int)((PBCLK /(float)(8 * 11000)) - 1);
        led2 = 1;
        duty = C[(++recordingposition) % C_SIZE];    
        OC2RS = (int)((PR2 + 1) * ((float)duty) / 1023);
    }
    if(sw3 && mode == 1){
        PR2 = (int)((PBCLK /(float)(8 * 11000)) - 1);
        led3 = 1;
        duty = D[(++recordingposition) % D_SIZE];    
        OC2RS = (int)((PR2 + 1) * ((float)duty) / 1023);
    }
     if(sw4 && mode == 1){
        PR2 = (int)((PBCLK /(float)(8 * 11000)) - 1);
        led4 = 1;
        duty = E[(++recordingposition) % E_SIZE];    
        OC2RS = (int)((PR2 + 1) * ((float)duty) / 1023);
    }
    if(sw5 && mode == 1){  
        PR2 = (int)((PBCLK /(float)(8 * 11000)) - 1);
        led5 = 1;
        duty = F[(++recordingposition) % F_SIZE];    
        OC2RS = (int)((PR2 + 1) * ((float)duty) / 1023);
    }
    if(sw6 && mode == 1){  
        PR2 = (int)((PBCLK /(float)(8 * 11000)) - 1);
        led6 = 1;
        duty = G[(++recordingposition) % G_SIZE];    
        OC2RS = (int)((PR2 + 1) * ((float)duty) / 1023);
    }
    if(sw7 && mode == 1){  
        PR2 = (int)((PBCLK /(float)(8 * 11000)) - 1);        
        led7 = 1;
        duty = A5[(++recordingposition) % A5_SIZE];    
        OC2RS = (int)((PR2 + 1) * ((float)duty) / 1023);
    }
    if(!sw0 && mode == 1){led0 = 0; }  
    if(!sw1 && mode == 1){led1 = 0; }      
    if(!sw2 && mode == 1){led2 = 0; }
    if(!sw3 && mode == 1){led3 = 0; }
    if(!sw4 && mode == 1){led4 = 0; }
    if(!sw5 && mode == 1){led5 = 0; }
    if(!sw6 && mode == 1){led6 = 0; }
    if(!sw7 && mode == 1){led7 = 0; }    
    if(key2 == 1 && hold == 1){
        PR2 = (int)((PBCLK /(float)(8 * 11000)) - 1);        
        led2 = 1;
        recording[(recordingposition++)] = ADC_AnalogRead(4);  
        x++;
        if(x  == 1200){
            if (ms == 9){
                ms = 0;
                sec++;
                x = 0;
            }
            else{
                ms++;
                x = 0;
            }
        }
       
        if(sec == 5){
            key2 = 0;    
        }
        if(BtnR && recorded && hold){
            sec = 0;
            ms = 0;
            recorded = 0;
            int i = 0;
            recordingposition = 0;
        }
           
    }
    
    if(key3 == 1){    
        //PR2 = (int)((PBCLK /(float)(1 * 4000)) - 1);
        PR2 = (int)((PBCLK /(float)(3 * 5500)) - 1);
        OC2RS = recording[playbackposition++];
     
        x++;
        if(x  == 1300){ 
            if(ms == 0){
                ms = 9;
                sec--;
                x = 0;
            }
            else{
                ms--;
                x = 0;
            }
        }
       
    }
    IEC0bits.T2IE = 1;      // enable Timer2 interrupt    
    IFS0bits.T2IF = 0;      // clear Timer2 interrupt flag
}


int main()
{  
    DDPCONbits.JTAGEN = 0;
    configSw();
    configLed();
    LCD_Init();
    TRISFbits.TRISF4 = 1;   // RF0 (BTNC) configured as input
    TRISBbits.TRISB8 = 1;   // RB8 (BTNR) configured as input
    ANSELBbits.ANSB8 = 0;   // RB8 (BTNR) disabled analog
    TMR2 = 0;
    T2CONbits.TCKPS = 8;     //1:256 prescale value
    T2CONbits.TGATE = 0;     //not gated input (the default)
    T2CONbits.TCS = 0;       //PCBLK input (the default)
    T2CONbits.ON = 1;        //turn on Timer2
    OC2CONbits.ON = 0;       // Turn off OC2 while doing setup.
    OC2CONbits.OCM = 6;      // PWM mode on OC2; Fault pin is disabled
    OC2CONbits.OCTSEL = 0;   // Timer2 is the clock source for this Output Compare module
    OC2CONbits.ON = 1;       // Start the OC2 module  
    TRISBbits.TRISB14 = 0;
    ANSELBbits.ANSB14 = 0;
    RPB14R = 0xB;
    IPC2bits.T2IP = 7;          // interrupt priority
    IPC2bits.T2IS = 3;          // interrupt subpriority
    IEC0bits.T2IE = 1;          // enable Timer2 interrupt    
    IFS0bits.T2IF = 0;          // clear Timer2 interrupt flag
    macro_enable_interrupts();  // enable interrupts at CPU
    ADC_Init();  
    LCD_DisplayClear();
    
    while(1){
        switch (mode)
        {
            case 1: {
                sec = 0;
                ms = 0;
                playbackposition = 0;
                recordingposition = 0;
                led0=led1=led2=led3=led4=led5=led6=led7=0;
                LCD_WriteStringAtPos("1: Tone Play", 0, 0);
                // play sine
                if(BtnC && buttonLock == 0){
                    buttonLock = 1;
                    mode = 2;
                    LCD_DisplayClear();
                }
                if(!BtnC && buttonLock == 1){
                    buttonLock = 0;                    
                }
                break;
            }
            case 2: {
                LCD_WriteStringAtPos("2: Voice Record", 0, 0);
                sprintf(s, "%d.%d sec recorded", sec, ms);                  
                LCD_WriteStringAtPos(s, 1, 0);
                if(BtnC && buttonLock == 0){
                    buttonLock = 1;
                    mode = 3;
                    key3 = 1;
                    LCD_DisplayClear();
                    delay_ms(100);
                    buttonLock = 0;
                }
                if(!BtnC && buttonLock == 1){
                    buttonLock = 0;                    
                }
                if(BtnR && hold == 0 && recorded == 0){
                    key2 = 1;
                    hold = 1;
                    recorded = 1;
                }
                                
                if(!BtnR && hold == 1){  
                    hold = 0;
                    key2 = 0;
                }              
                break;
            }
            case 3: {
                led0=led1=led2=led4=led5=led6=led7=0;
                led3 = 1;
                LCD_WriteStringAtPos("3: Playback", 0, 0);
                sprintf(q, "%d.%d sec remain", sec, ms);  
                LCD_WriteStringAtPos(q, 1, 0);
                if(BtnC && buttonLock == 0){
                    mode = 1;
                    buttonLock = 1;
                    LCD_DisplayClear();
                    delay_ms(100);
                    buttonLock = 0;
                }
                if(sec == 0 && ms == 0){
                    key3 = 0;
                }
                if(!BtnC && buttonLock == 1){
                    buttonLock = 0;                    
                }  

            }
        }
    }
}

void delay_ms(int ms){
int i,counter;
for (counter=0; counter<ms; counter++)
    {
        for(i=0;i<1426;i++)
        {
        }   //software delay 1 millisec
    }
}


void shutdown()
{
    T2CONbits.ON = 0;       // turn off Timer2
    OC2CONbits.ON = 0;      // Turn off OC2      
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

