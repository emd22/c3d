#ifndef F3D_TIME_H
#define F3D_TIME_H

extern float delta_time;
extern unsigned long frames_rendered;

void time_init(void);
void time_tick(void);
float ar_time_get_delta(void);
void time_end(void);

#endif
