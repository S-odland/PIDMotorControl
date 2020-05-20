#ifndef delay__H__
#define delay__H__

#include "NU32.h"
#include <stdio.h>

void delay(void);

void delay(void) {
  int j;
  for (j = 0; j < 1000000; j++) { // number is 1 million
    while(!PORTDbits.RD7) {
        ;   // Pin D7 is the USER switch, low (FALSE) if pressed.
    }
  }
}

#endif