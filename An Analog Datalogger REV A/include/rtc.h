/*
 * rtc.h
 *
 *  Created on: 12-apr.-2015
 *      Author: Robbe Van Assche
 */

#ifndef RTC_H_
#define RTC_H_

#include "stm32f4xx.h"
#include "stm32f4xx_rtc.h"

#include <stdio.h>

#include "projectconfig.h"
#include "rcc.h"
#include "delay.h"
#include "ssd1306.h"

void
initialize_RTC(void);

void
rtc_printTimeTerminal(void);

void
rtc_setTimeToOLED(void);

#endif /* RTC_H_ */