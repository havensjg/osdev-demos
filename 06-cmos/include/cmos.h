#pragma once

#include <stdint.h>

/* CMOS IO ports */
#define CMOS_IDX  0x70
#define CMOS_DATA 0x71

/* https://stanislavs.org/helppc/cmos_ram.html */

/* CMOS register */
#define CMOS_RTC_SECONDS 0x00
#define CMOS_RTC_MINUTES 0x02
#define CMOS_RTC_HOURS   0x04
#define CMOS_RTC_WEEKDAY 0x06
#define CMOS_RTC_DAY     0x07
#define CMOS_RTC_MONTH   0x08
#define CMOS_RTC_YEAR    0x09
#define CMOS_RTC_STAT_A  0x0A
#define CMOS_RTC_STAT_B  0x0B

/* CMOS_RTC_STAT_A fields */
#define CMOS_RTC_RATE_SEL(x)  (x & 0xf)
#define CMOS_RTC_TIME_BASE(x) ((x & 0x7) << 4)
#define CMOS_RTC_UPDATING     0x80

/* CMOS_RTC_STAT_B fields */
#define CMOS_RTC_DST            0x01
#define CMOS_RTC_24HR           0x02
#define CMOS_RTC_BCD            0x04
#define CMOS_RTC_SQ_WAVE        0x08
#define CMOS_RTC_UPDATE_IRQ     0x10
#define CMOS_RTC_ALARM_IRQ      0x20
#define CMOS_RTC_PERIODIC_IRQ   0x40
#define CMOS_RTC_DISABLE_UPDATE 0x80

#define CMOS_RTC_PM 0x80

#define CMOS_NMI_DISABLE 0x80

/* Convert BCD to binary */
uint8_t cmos_bcd_to_bin(uint8_t bcd);

/* Read a register in the CMOS */
uint8_t cmos_read(uint8_t reg);

/* Retrieves the 24-hour flag from the RTC */
uint8_t cmos_rtc_24hr(void);

/* Retrieves the BCD flag from the RTC */
uint8_t cmos_rtc_bcd(void);

/* Retrieve the day of the month from the RTC and convert to binary */
uint8_t cmos_rtc_day(void);

/* Retrieve the hours from the RTC and convert to 24-hour binary (0 = 12am ... 23 = 11pm)*/
uint8_t cmos_rtc_hours(void);

/* Retrieve the minutes from the RTC and convert to binary */
uint8_t cmos_rtc_minutes(void);

/* Retrieve the month from the RTC and convert to binary*/
uint8_t cmos_rtc_month(void);

/* Retrieve the seconds from the RTC and convert to binary */
uint8_t cmos_rtc_seconds(void);

/* Check if the RTC is updating its registers */
uint8_t cmos_rtc_updating(void);

/* Wait for the next RTC update to complete. If one is not in progress, it will wait for one to begin. This will take a long time (relatively speaking) and is not the optimal way to do it */
void cmos_rtc_wait(void);

/* Retrieve the day of the week from the RTC (1 = Sunday ... 7 = Saturday) */
uint8_t cmos_rtc_weekday(void);

/* Retrieve the year from the RTC and convert to binary along with century */
uint16_t cmos_rtc_year(void);