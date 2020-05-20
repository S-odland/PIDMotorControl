#include "utilities.h"

enum op_mode mode = IDLE;
char buffer[100];

void set_mode(enum op_mode m){
    mode = m;
}
int get_mode(void){
    return mode;
}

