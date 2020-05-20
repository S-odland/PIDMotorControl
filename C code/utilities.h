#ifndef UTILITIES__H__
#define UTILITIES__H__

enum op_mode{IDLE = 0, PWM = 1, ITEST = 2, HOLD = 3, TRACK = 4};

void set_mode(enum op_mode m);
int get_mode(void);


#endif

