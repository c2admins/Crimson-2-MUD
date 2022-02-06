/* ************************************************************************
*  file: weather.h , Weather and time module              Part of DIKUMUD *
*  Usage: Performing the clock and the weather                            *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete information. *
************************************************************************* */

extern struct time_info_data time_info;
extern struct weather_data weather_info;

/* In this part. */

/* 89 hours/phase.  This is within 0.5% of the actual average. */
#define HOURS_PER_PHASE		89

void weather_and_time(int mode);
void another_hour(int mode);
void weather_change(void);
void send_current_lunar_phase(void);
void boot_lunar_cycles(void);