/* we */
/* gv_location: 20501-21500 */
/* ********************************************************************
*  file: weather.c , Weather and time module          Part of DIKUMUD *
*  Usage: Performing the clock and the weather                        *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete info.    *
********************************************************************* */

#include "structs.h"
#include "utility.h"
#include "comm.h"
#include "globals.h"
#include "weather.h"

/* uses */

struct time_info_data time_info;
struct weather_data weather_info;

/* In this part. */

void weather_and_time(int mode);
void another_hour(int mode);
void weather_change(void);
void send_current_lunar_phase(void);


int dice(int number, int size);
void co3100_send_to_outdoor(char *messg);


/* Here comes the code */

void weather_and_time(int mode)
{
	another_hour(mode);
	if (mode)
		weather_change();
}

void another_hour(int mode)
{
	time_info.hours++;
	weather_info.current_hour_in_phase++;

	if (mode) {
		switch (time_info.hours) {
		case 5:
			{
				weather_info.sunlight = SUN_RISE;
				co3100_send_to_outdoor("The sky brightens as dawn begins.\n\r");
				break;
			}
		case 6:
			{
				weather_info.sunlight = SUN_LIGHT;
				co3100_send_to_outdoor("Light fills the land as the day begins.\n\r");
				break;
			}
		case 21:
			{
				weather_info.sunlight = SUN_SET;
				co3100_send_to_outdoor(
						       "The sky darkens as the sun begins to set.\n\r");
				break;
			}
		case 22:
			{
				weather_info.sunlight = SUN_DARK;
				co3100_send_to_outdoor("A darkness falls across the land as the night begins.\n\r");
				send_current_lunar_phase();
				break;
			}
		default:
			break;
		}
	}

	// Advance day, month, year.
	if (time_info.hours > 23) {	/* Changed by HHS due to bug ??? */
		time_info.hours = 0;
		time_info.day++;

		if (time_info.day > 34) {
			time_info.day = 0;
			time_info.month++;

			if (time_info.month > 16) {
				time_info.month = 0;
				time_info.year++;
			}
		}
	}
	
	// Advance lunar cycle. -- Relic
	if (weather_info.current_hour_in_phase > HOURS_PER_PHASE)
	{
		weather_info.current_hour_in_phase = 0;
		weather_info.lunar_phase++;
		if (weather_info.lunar_phase > MAX_LUNAR_PHASES)
		{
			weather_info.lunar_phase = 0;
		}
		if (weather_info.sunlight == SUN_DARK)
		{
			send_current_lunar_phase();
		}
	}
}

// Relic
void send_current_lunar_phase(void)
{
	switch (weather_info.lunar_phase)
	{
		case PHASE_NEW:
		co3100_send_to_outdoor("The moon is now new.\n\r");
		break;
		case PHASE_CRESCENT_1:
		co3100_send_to_outdoor("The moon is now crescent.\n\r");
		break;
		case PHASE_FIRST_QUARTER:
		co3100_send_to_outdoor("The moon is now in the first quarter.\n\r");
		break;
		case PHASE_WAXING_GIBBOUS:
		co3100_send_to_outdoor("THe moon is now waxing gibbous.\n\r");
		break;
		case PHASE_FULL_MOON:
		co3100_send_to_outdoor("The moon is now full.\n\r");
		break;
		case PHASE_WANING_GIBBOUS:
		co3100_send_to_outdoor("The moon is now waning gibbous.\n\r");
		break;
		case PHASE_LAST_QUARTER:
		co3100_send_to_outdoor("The moon is now in the last quarter.\n\r");
		break;
		case PHASE_CRESCENT_2:
		co3100_send_to_outdoor("The moon is now crescent.\n\r");
		break;
		default:
		co3100_send_to_outdoor("The sky brightens as dawn begins.\n\r");
		break;
	}
}

void weather_change(void)
{
	int diff, change;

	if ((time_info.month >= 9) && (time_info.month <= 16))
		diff = (weather_info.pressure > 985 ? -2 : 2);
	else
		diff = (weather_info.pressure > 1015 ? -2 : 2);

	weather_info.change += (dice(1, 4) * diff + dice(2, 6) - dice(2, 6));

	weather_info.change = MINV(weather_info.change, 12);
	weather_info.change = MAXV(weather_info.change, -12);

	weather_info.pressure += weather_info.change;

	weather_info.pressure = MINV(weather_info.pressure, 1040);
	weather_info.pressure = MAXV(weather_info.pressure, 960);

	change = 0;

	switch (weather_info.sky) {
	case SKY_CLOUDLESS:
		{
			if (weather_info.pressure < 990)
				change = 1;
			else if (weather_info.pressure < 1010)
				if (dice(1, 4) == 1)
					change = 1;
			break;
		}
	case SKY_CLOUDY:
		{
			if (weather_info.pressure < 970)
				change = 2;
			else if (weather_info.pressure < 990)
				if (dice(1, 4) == 1)
					change = 2;
				else
					change = 0;
			else if (weather_info.pressure > 1030)
				if (dice(1, 4) == 1)
					change = 3;

			break;
		}
	case SKY_RAINING:
		{
			if (weather_info.pressure < 970)
				if (dice(1, 4) == 1)
					change = 4;
				else
					change = 0;
			else if (weather_info.pressure > 1030)
				change = 5;
			else if (weather_info.pressure > 1010)
				if (dice(1, 4) == 1)
					change = 5;

			break;
		}
	case SKY_LIGHTNING:
		{
			if (weather_info.pressure > 1010)
				change = 6;
			else if (weather_info.pressure > 990)
				if (dice(1, 4) == 1)
					change = 6;

			break;
		}
	default:
		{
			change = 0;
			weather_info.sky = SKY_CLOUDLESS;
			break;
		}
	}

	switch (change) {
	case 0:
		break;
	case 1:
		{
			co3100_send_to_outdoor(
					    "The sky is getting cloudy.\n\r");
			weather_info.sky = SKY_CLOUDY;
			break;
		}
	case 2:
		{
			co3100_send_to_outdoor(
					       "It starts to rain.\n\r");
			weather_info.sky = SKY_RAINING;
			break;
		}
	case 3:
		{
			co3100_send_to_outdoor(
					  "The clouds slowly disappear.\n\r");
			weather_info.sky = SKY_CLOUDLESS;
			break;
		}
	case 4:
		{
			co3100_send_to_outdoor(
				  "Lightning starts to show in the sky.\n\r");
			weather_info.sky = SKY_LIGHTNING;
			break;
		}
	case 5:
		{
			co3100_send_to_outdoor(
					       "The rain stopped.\n\r");
			weather_info.sky = SKY_CLOUDY;
			break;
		}
	case 6:
		{
			co3100_send_to_outdoor(
					    "The lightning has stopped.\n\r");
			weather_info.sky = SKY_RAINING;
			break;
		}
	default:
		break;
	}
}
