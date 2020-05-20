#include "NU32.h"
#include "encoder.h"
#include "utilities.h"
#include <stdio.h>

#define BUF_SIZE 200
#define NUMSAMPS 100


void periph_init(void);
int read_ADC(void);

volatile int pwm = 0;
volatile int adcvalue;
volatile int angle;
volatile int refCurrent;
volatile int refCurrentpos;
volatile int reflen;

volatile float ref[10000];
volatile float measuredPos[10000];

static volatile int kptemp, kitemp, kip, kpp, kdp;
static volatile int ADCarray[NUMSAMPS];
static volatile int REFarray[NUMSAMPS];


void __ISR(_TIMER_4_VECTOR, IPL5SOFT) Position(void){
    char buffer[BUF_SIZE];
    int a, s, r, e = 0,eint = 0,eprev = 0,edot = 0,pos;

    switch(get_mode()){
        case 3:
        {
            a = encoder_angle();
            r = angle;

            e = r - a;
            eint = eint+e;
            edot = e-eprev;
            refCurrent = kpp*e+kip*eint+kdp*edot;
            eprev = e;

            sprintf(buffer, "%d\r\n",refCurrent);
            NU32_WriteUART3(buffer);

            break;
        }
        case 4:
        {
            static int counter = 0;

            pos = ref[counter];
            a = encoder_angle();

            e = pos - a;
            eint = eint+e;
            edot = e-eprev;
            refCurrentpos = kpp*e + kip*eint + kdp*edot;
            eprev = e;

            if (counter >= reflen){
                angle = refCurrentpos;
                set_mode(3);
                counter = 0;
            }

            measuredPos[counter] = a;

            counter++;

            break;
        }
    }
      
    IFS0bits.T4IF = 0;
}

void __ISR(_TIMER_2_VECTOR, IPL4SOFT) Controller(void) {
    char buffer[BUF_SIZE];
    char direction[BUF_SIZE];

    static int count = 0;

    int u = 0, s, r, e,eint=0;

    switch (get_mode()){
        case 0: //IDLE
        {
            OC1RS = 0;
            break;
        }
        case 1: //PWM
        {           
            OC1RS = abs(pwm)*PR3/100;
            if (pwm<0){
                LATDbits.LATD1 = 1; 
            }else{
                LATDbits.LATD1 = 0;     
            }
            break;
        }
        case 2: //ITEST
        {
            
            int A = 500;
      
            if(count < 25){
                REFarray[count] = 1646 + A;
            }else if(count < 50){
                REFarray[count] = 1646 - A;
            }
            else if(count < 75){
                REFarray[count]= 1646 + A;
            }else{
                REFarray[count]= 1646 - A;
            }
            
            s = read_ADC()*3300/1024;
            r = REFarray[count];
            e = r-s;
            eint = eint + e;
            u = kptemp*e + kitemp*eint;

            if(u<0){
                LATDbits.LATD1 = 1; 
            }else{
                LATDbits.LATD1 = 0; 
            }
            

            if(u<-A){
                u = -A;
            }else if(u>A){
                u = A;
            }else{
                ;
            }

            OC1RS = abs(u)*PR3/175;

            if(OC1RS<0){
                OC1RS = 0;
            }else if(OC1RS>3999){
                OC1RS = 3999;
            }else{
                ;
            }

            ADCarray[count] = s;
            
            if(count==99){
                set_mode(0);
                count = 0;
            }

            count++;
            break;

        }
        case 3: //HOLD
        {

            s = read_ADC()*3300/1024;
            r = refCurrent;
            e = r-s;
            eint = eint + e;
            u = kptemp*e + kitemp*eint;

            if(u<0){
                LATDbits.LATD1 = 1; 
            }else{
                LATDbits.LATD1 = 0; 
            }

            OC1RS = abs(u)*PR3/175;

            if(OC1RS<0){
                OC1RS = 0;
            }else if(OC1RS>3999){
                OC1RS = 3999;
            }else{
                ;
            }

            break;
        }
        case 4: //TRACK
        {

            s = read_ADC()*3300/1024;
            r = refCurrentpos;
            e = r-s;
            eint = eint + e;
            u = kptemp*e + kitemp*eint;

            if(u<0){
                LATDbits.LATD1 = 1; 
            }else{
                LATDbits.LATD1 = 0; 
            }
           
            OC1RS = abs(u)*PR3/175;

            if(OC1RS<0){
                OC1RS = 0;
            }else if(OC1RS>3999){
                OC1RS = 3999;
            }else{
                ;
            }

            break;
        }

    }
    IFS0bits.T2IF = 0;
}

int main(){
    char buffer[BUF_SIZE];
    char read[100];

    NU32_Startup();
    set_mode(0);
    
    NU32_LED1 = 1;
    NU32_LED2 = 1;

    __builtin_disable_interrupts();
    
    encoder_init();
    periph_init();

    IPC2bits.T2IP = 4;
    IFS0bits.T2IF = 0;
    IEC0bits.T2IE = 1;

    IPC4bits.T4IP = 5;
    IFS0bits.T4IF = 0;
    IEC0bits.T4IE = 1;

    __builtin_enable_interrupts();

    while(1){

        NU32_ReadUART3(buffer,BUF_SIZE);
        

        switch (buffer[0]){
            case 'a':
            {
                sprintf(buffer, "%d\r\n", read_ADC());
                NU32_WriteUART3(buffer);
                break;
            } 
            case 'b':
            {
                sprintf(buffer, "%d\r\n", read_ADC()*3300/1024);
                NU32_WriteUART3(buffer);
                break;
            }
            case 'c':
            {
                int enc;
                enc = encoder_counts();
                sprintf(buffer, "%d\r\n", enc);
                NU32_WriteUART3(buffer);
                break;
            }
            case 'd':
            {
                sprintf(buffer, "%d\r\n", encoder_angle());
                NU32_WriteUART3(buffer);
                break;
            }
            case 'e':
            {
                sprintf(buffer, "%d\r\n", encoder_reset());
                NU32_WriteUART3(buffer);
                break;
            }   
            case 'f':
            {
                set_mode(1);
                NU32_ReadUART3(buffer,BUF_SIZE);
                sscanf(buffer, "%d\n",&pwm);

                sprintf(buffer, "%d\r\n",pwm);
                NU32_WriteUART3(buffer);
                break;
            }
            case 'g':
            {
                NU32_ReadUART3(buffer, BUF_SIZE);
                sscanf(buffer, "%d %d\n",&kptemp,&kitemp);

                break;
            }
            case 'h':
            {
                sprintf(buffer, "%d %d\r\n",kptemp,kitemp);
                NU32_WriteUART3(buffer);
                break;
            }
            case 'i':
            {
                NU32_ReadUART3(buffer, BUF_SIZE);
                sscanf(buffer, "%d %d %d\n",&kpp,&kip,&kdp);

                break;
            }
            case 'j':
            {
                sprintf(buffer, "%d %d %d\r\n",kpp,kip,kdp);
                NU32_WriteUART3(buffer);
                break;
            }
            case 'k':
            {

                sprintf(buffer, "%d\r\n",NUMSAMPS);
                NU32_WriteUART3(buffer);

                set_mode(2);

                while(get_mode()==2){
                    ;
                }

                int i;
                for (i=0;i<NUMSAMPS;i++){
                    sprintf(buffer, "%d %d\r\n",REFarray[i],ADCarray[i]);
                    NU32_WriteUART3(buffer);
                }
                break;
            }
            case 'l':
            {

                NU32_ReadUART3(buffer, BUF_SIZE);
                sscanf(buffer, "%d\n",&angle);

                set_mode(3);

            
                break;
            }
            case 'm':
            {
                int i;

                NU32_ReadUART3(buffer, BUF_SIZE);
                sscanf(buffer,"%d\n",&reflen);

                for(i=0;i<reflen;i++){
                    NU32_ReadUART3(buffer,100);
                    sscanf(buffer,"%d\n",&ref[i]);
                }

                break;
            }
            case 'n':
            {
                int i;

                NU32_ReadUART3(buffer, BUF_SIZE);
                sscanf(buffer,"%d\n",&reflen);

                for(i=0;i<reflen;i++){
                    NU32_ReadUART3(buffer, BUF_SIZE);
                    sscanf(buffer,"%f\n",&ref[i]);
                }

                break;

            }
            case 'o':
            {
                set_mode(4); //TRACK

                while(get_mode()==4){
                    ;
                }

                sprintf(buffer, "%d\r\n",reflen);
                NU32_WriteUART3(buffer);

                int i;
                for (i=0;i<=reflen;i++){
                    sprintf(buffer, "%d %d\r\n",measuredPos[i],ref[i]);
                    NU32_WriteUART3(buffer);
                }
                break;
                
            }
            case 'p':
            {
                set_mode(0);

                break;
            }
            case 'q':
            {
                set_mode(0);
                break;
            }
            case 'r':
            {
                sprintf(buffer, "%d\r\n", get_mode());
                NU32_WriteUART3(buffer);
                break;
            }
            default:
            {
                NU32_LED1 = 0;

                break;
            }
        }
    }
}

void periph_init(void){
  
    T3CONbits.TCKPS = 0;
    PR3 = 3999;             // 20kHz PWM 
    TMR3 = 0;
    OC1CONbits.OC32 = 0;
    OC1CONbits.OCM = 0b110;
    OC1CONbits.OCTSEL = 1;
    T3CONbits.ON = 1;
    OC1CONbits.ON = 1;

    T2CONbits.TCKPS = 0;
    PR2 = 15999;            // 5kHz current control loop
    TMR2 = 0;
    T2CONbits.ON = 1;

    T4CONbits.TCKPS = 3;
    PR4 = 49999;            // 200 Hz position control loop
    TMR4 = 0;
    T4CONbits.ON = 1;

    AD1PCFGbits.PCFG0 = 0;
    AD1CON3bits.ADCS = 2;
    AD1CHSbits.CH0SA = 0;
    AD1CON1bits.ON = 1;
    
    TRISDbits.TRISD1 = 0;
    LATDbits.LATD1 = 1;
}

int read_ADC(void){

    int elapsed = 0;



    AD1CHSbits.CH0SA = 0;
    AD1CON1bits.SAMP = 1;

    elapsed = _CP0_GET_COUNT() + 16;
    
    while(_CP0_GET_COUNT()< elapsed){      
            ;
    }
    AD1CON1bits.SAMP = 0;
    while(!AD1CON1bits.DONE){
        ;
    }
    adcvalue= ADC1BUF0;


    return adcvalue;
}
