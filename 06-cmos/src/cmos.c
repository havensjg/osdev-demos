#include <stdint.h>

#include <cmos.h>
#include <io.h>

/* Set to 1 to disable non-maskable interrupts */
int cmos_nmi_disable = 0;

/* Convert BCD to binary */
uint8_t cmos_bcd_to_bin(uint8_t bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

/* Read a register in the CMOS */
uint8_t cmos_read(uint8_t reg) {
    /* append the NMI disable flag */
    if (cmos_nmi_disable) {
        reg &= 0x7F;
        reg |= CMOS_NMI_DISABLE;
    }

    /* perform the read */
    outb(CMOS_IDX, reg);
    uint8_t v = inb(CMOS_DATA);

    return v;
}

/* Retrieves the 24-hour flag from the RTC */
uint8_t cmos_rtc_24hr(void) {
    return cmos_read(CMOS_RTC_STAT_B) & CMOS_RTC_24HR;
}

/* Retrieves the BCD flag from the RTC */
uint8_t cmos_rtc_bcd(void) {
    uint8_t b = cmos_read(CMOS_RTC_STAT_B);

    /* CMOS_RTC_BCD bit set means the counter is in binary */
    if (b & CMOS_RTC_BCD) {
        return 0;
    } else {
        return 1;
    }
}

/* Retrieve the day of the month from the RTC and convert to binary */
uint8_t cmos_rtc_day(void) {
    uint8_t v = cmos_read(CMOS_RTC_DAY);

    /* Is RTC in BCD? */
    if (cmos_rtc_bcd()) {
        return cmos_bcd_to_bin(v);
    } else {
        /* RTC is in binary */
        return v;
    }
}

/* Retrieve the hours from the RTC and convert to 24-hour binary (0 = 12am ... 23 = 11pm)*/
uint8_t cmos_rtc_hours(void) {
    uint8_t v = cmos_read(CMOS_RTC_HOURS);

    /* Is RTC in 24 hour mode? */
    if (cmos_rtc_24hr()) {
        /* Is RTC in BCD mode? */
        if (cmos_rtc_bcd()) {
            return cmos_bcd_to_bin(v);
        } else {
            /* RTC is in 24 hours binary mode already */
            return v;
        }
    } else {
        /* RTC is in 12 hour mode */

        /* Save the PM flag */
        uint8_t pm = v & CMOS_RTC_PM;

        /* Is RTC in BCD mode? */
        if (cmos_rtc_bcd()) {
            /* Mask the PM flag before conversion */
            v = cmos_bcd_to_bin(v & ~CMOS_RTC_PM);
        } else {
            /* Mask the PM flag */
            v &= ~CMOS_RTC_PM;
        }
        
        /* Is it PM? */
        if (pm != 0) {
            v += 12;
        }

        return v;
    }
}

/* Retrieve the minutes from the RTC and convert to binary */
uint8_t cmos_rtc_minutes(void) {
    uint8_t v = cmos_read(CMOS_RTC_MINUTES);

    /* Is RTC in BCD? */
    if (cmos_rtc_bcd()) {
        return cmos_bcd_to_bin(v);
    } else {
        /* RTC is in binary */
        return v;
    }
}

/* Retrieve the month from the RTC and convert to binary*/
uint8_t cmos_rtc_month(void) {
    uint8_t v = cmos_read(CMOS_RTC_MONTH);

    /* Is RTC in BCD? */
    if (cmos_rtc_bcd()) {
        return cmos_bcd_to_bin(v);
    } else {
        /* RTC is in binary */
        return v;
    }
}

/* Retrieve the seconds from the RTC and convert to binary */
uint8_t cmos_rtc_seconds(void) {
    /* This register doesn't need to wait for an update to complete */
    uint8_t v = cmos_read(CMOS_RTC_SECONDS);

    /* Is RTC in BCD? */
    if (cmos_rtc_bcd()) {
        return cmos_bcd_to_bin(v);
    } else {
        /* RTC is in binary */
        return v;
    }
}

/* Check if the RTC is updating its registers */
uint8_t cmos_rtc_updating(void) {
    return cmos_read(CMOS_RTC_STAT_A & CMOS_RTC_UPDATING);
}

/* Wait for the next RTC update to complete. If one is not in progress, it will wait for one to begin. This will take a long time (relatively speaking) and is not the optimal way to do it */
void cmos_rtc_wait(void) {
    /* Is waiting even necessary? The update only happens when seconds are rolling over */
    if (cmos_rtc_seconds() < 59) {
        return;
    }

    /* Wait for it to become set */
    while (!cmos_rtc_updating()) {
        io_wait();
    }

    /* Then wait for it to become clear */
    while (cmos_rtc_updating()) {
        io_wait();
    }
}

/* Retrieve the day of the week from the RTC (1 = Sunday ... 7 = Saturday) */
uint8_t cmos_rtc_weekday(void) {
    return cmos_read(CMOS_RTC_WEEKDAY);
}

/* Retrieve the year from the RTC and convert to binary along with century */
uint16_t cmos_rtc_year(void) {
    uint8_t v = cmos_read(CMOS_RTC_YEAR);

    /* Is RTC in BCD? */
    if (cmos_rtc_bcd()) {
        v = cmos_bcd_to_bin(v);
    }

    /* Assume the 21st century. However, a better way one might do this is to locate the century register using ACPI and read it from there. */
    return 2000 + v;
}