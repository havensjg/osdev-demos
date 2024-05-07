#include <stdio.h>

#include <cmos.h>
#include <terminal.h>

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif
 
/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

const char *days[7] = {
	"Sunday",
	"Monday",
	"Tuesday",
	"Wednesday",
	"Thursday",
	"Friday",
	"Saturday"
};
 
void kernel_main(void) 
{
	/* Initialize terminal interface */
	terminal_initialize();

	/* Print some things on the screen */
	printf("Hello, kernel World!\n");

	/* Wait for the next RTC update to complete so there's almost a whole second to read the value */
	/* A better way to do this would be to use IRQ8 and enable the update IRQ on the RTC */
	cmos_rtc_wait();

	/* Print the time and date */
	printf("The current time (h:m:s): %d:%d:%d\n", cmos_rtc_hours(), cmos_rtc_minutes(), cmos_rtc_seconds());
	printf("The current date (y/m/d): %d/%d/%d\n", cmos_rtc_year(), cmos_rtc_month(), cmos_rtc_day());
	printf("The day of the week: %s\n",days[cmos_rtc_weekday() - 1]);
}