#include "encoder.h"
#include <xc.h>

static int encoder_command(int read){
    SPI4BUF = read;
    while(!SPI4STATbits.SPIRBF){
        ;
    }
    SPI4BUF;
    SPI4BUF = 5;
    while(!SPI4STATbits.SPIRBF){
        ;
    }
    return SPI4BUF;
}

int encoder_counts(void){
    return encoder_command(1);
}

int encoder_reset(void){
    return encoder_command(0); // resets to 32768 (65536 total counts)
}

int encoder_angle(void){
    float ratio, counts, angle;

    counts = encoder_command(1);

    angle = ((counts-32768)/384)*360;
    
    return angle;
}

void encoder_init(void){
    SPI4CON = 0;
    SPI4BUF;
    SPI4BRG = 0x4;
    SPI4STATbits.SPIROV = 0;
    SPI4CONbits.MSTEN = 1;
    SPI4CONbits.MSSEN = 1;
    SPI4CONbits.MODE16 = 1;
    SPI4CONbits.MODE32 = 0;
    SPI4CONbits.SMP = 1;
    SPI4CONbits.ON = 1;
}