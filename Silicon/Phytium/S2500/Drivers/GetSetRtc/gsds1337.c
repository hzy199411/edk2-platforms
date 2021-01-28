/*
 * (C) Copyright 2001-2008
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 * Keith Outwater, keith_outwater@mvis.com`
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/*
 * Date & Time support (no alarms) for Dallas Semiconductor (now Maxim)
 * DS1337 Real Time Clock (RTC).
 */

#include <GetSetRtcDxe.h>

#if defined(CONFIG_CMD_DATE)

#define printf(arg...)		DEBUG((EFI_D_BLKIO,## arg))
//#define printf(arg...)
#if 1
void i2c_reg_write(uchar addr, uchar reg, uchar val);
uchar i2c_reg_read(uchar addr, uchar reg);
#endif

/*
 * RTC register addresses
 */
#if defined CONFIG_RTC_DS1337
#define RTC_SEC_REG_ADDR	0x0
#define RTC_MIN_REG_ADDR	0x1
#define RTC_HR_REG_ADDR		0x2
#define RTC_DAY_REG_ADDR	0x3
#define RTC_DATE_REG_ADDR	0x4
#define RTC_MON_REG_ADDR	0x5
#define RTC_YR_REG_ADDR		0x6
#define RTC_CTL_REG_ADDR	0x0e
#define RTC_STAT_REG_ADDR	0x0f
#define RTC_TC_REG_ADDR		0x10
#elif defined CONFIG_RTC_DS1388
#define RTC_SEC_REG_ADDR	0x1
#define RTC_MIN_REG_ADDR	0x2
#define RTC_HR_REG_ADDR		0x3
#define RTC_DAY_REG_ADDR	0x4
#define RTC_DATE_REG_ADDR	0x5
#define RTC_MON_REG_ADDR	0x6
#define RTC_YR_REG_ADDR		0x7
#define RTC_CTL_REG_ADDR	0x0c
#define RTC_STAT_REG_ADDR	0x0b
#define RTC_TC_REG_ADDR		0x0a
#endif

/*
 * RTC control register bits
 */
#define RTC_CTL_BIT_A1IE	0x1	/* Alarm 1 interrupt enable	*/
#define RTC_CTL_BIT_A2IE	0x2	/* Alarm 2 interrupt enable	*/
#define RTC_CTL_BIT_INTCN	0x4	/* Interrupt control		*/
#define RTC_CTL_BIT_RS1		0x8	/* Rate select 1		*/
#define RTC_CTL_BIT_RS2		0x10	/* Rate select 2		*/
#define RTC_CTL_BIT_DOSC	0x80	/* Disable Oscillator		*/

/*
 * RTC status register bits
 */
#define RTC_STAT_BIT_A1F	0x1	/* Alarm 1 flag			*/
#define RTC_STAT_BIT_A2F	0x2	/* Alarm 2 flag			*/
#define RTC_STAT_BIT_OSF	0x80	/* Oscillator stop flag		*/

static uchar rtc_read (uchar reg);
static void rtc_write (uchar reg, uchar val);


static unsigned int bcd2bin(UINT8 val)
{
	return ((val) & 0x0f) + ((val) >> 4) * 10;
}

static UINT8 bin2bcd (unsigned int val)
{
	return (((val / 10) << 4) | (val % 10));
}

/*
 * Utility routines to read/write registers.
 */
u8 i2c_reg_read(u8 addr, u8 reg)
{
	u8 buf;

	i2c_read(addr, reg, 1, &buf, 1);

	return buf;
}

void i2c_reg_write(u8 addr, u8 reg, u8 val)
{

	i2c_write(addr, reg, 1, &val, 1);
}

uchar rtc_read (uchar reg)
{
	return (i2c_reg_read (CONFIG_SYS_I2C_RTC_ADDR, reg));
}


void rtc_write (uchar reg, uchar val)
{
	i2c_reg_write (CONFIG_SYS_I2C_RTC_ADDR, reg, val);
}


/*
 * Get the current time from the RTC
 */
int rtc_get (rtc_time *tmp)
{
	uchar sec, min, hour, mday, mon_cent, year;//, status;

  // status = rtc_read (RTC_STAT_REG_ADDR);
    sec = rtc_read (RTC_SEC_REG_ADDR);
    min = rtc_read (RTC_MIN_REG_ADDR);
    hour = rtc_read (RTC_HR_REG_ADDR);
    mday = rtc_read (RTC_DATE_REG_ADDR);
    mon_cent = rtc_read (RTC_MON_REG_ADDR);
    year = rtc_read (RTC_YR_REG_ADDR);


	// if (status & RTC_STAT_BIT_OSF) {
	// 	//printf ("### Warning: RTC oscillator has stopped\n");
	// 	/* clear the OSF flag */
	// 	rtc_write (RTC_STAT_REG_ADDR,
	// 		   rtc_read (RTC_STAT_REG_ADDR) & ~RTC_STAT_BIT_OSF);
	// 	// This will make setup unhappy.  rel = -1;
	// }

	tmp->tm_sec  = bcd2bin (sec & 0x7F);
	tmp->tm_min  = bcd2bin (min & 0x7F);
	tmp->tm_hour = bcd2bin (hour & 0x3F);
	tmp->tm_mday = bcd2bin (mday & 0x3F);
	tmp->tm_mon  = bcd2bin (mon_cent & 0x1F);
	tmp->tm_year = bcd2bin (year) + 2000;
	// tmp->tm_wday = bcd2bin ((wday - 1) & 0x07);
	tmp->tm_yday = 0;
	tmp->tm_isdst= 0;

	return 0;
}


/*
 * Set the RTC
 */
int rtc_set (EFI_TIME *tmp, rtc_time *current)
{

  if (tmp->Year != current->tm_year) {
    rtc_write (RTC_YR_REG_ADDR, bin2bcd (tmp->Year % 100));
  }

  if (tmp->Month != current->tm_mon) {
    rtc_write (RTC_MON_REG_ADDR, bin2bcd (tmp->Month) | 0x80);
  }

  if (tmp->Day != current->tm_mday) {
    rtc_write (RTC_DATE_REG_ADDR, bin2bcd (tmp->Day));
  }
  if (tmp->Hour != current->tm_hour) {
    rtc_write (RTC_HR_REG_ADDR, bin2bcd (tmp->Hour));
  }
  if (tmp->Minute != current->tm_min) {
    rtc_write (RTC_MIN_REG_ADDR, bin2bcd (tmp->Minute));
  }
  if (tmp->Second != current->tm_sec)
    rtc_write (RTC_SEC_REG_ADDR, bin2bcd (tmp->Second));

  return 0;
}


/*
 * Reset the RTC.  We also enable the oscillator output on the
 * SQW/INTB* pin and program it for 32,768 Hz output. Note that
 * according to the datasheet, turning on the square wave output
 * increases the current drain on the backup battery from about
 * 600 nA to 2uA. Define CONFIG_SYS_RTC_DS1337_NOOSC if you wish to turn
 * off the OSC output.
 */

#ifdef CONFIG_SYS_RTC_DS1337_NOOSC
 #define RTC_DS1337_RESET_VAL \
	(RTC_CTL_BIT_INTCN | RTC_CTL_BIT_RS1 | RTC_CTL_BIT_RS2)
#else
 #define RTC_DS1337_RESET_VAL (RTC_CTL_BIT_RS1 | RTC_CTL_BIT_RS2)
#endif
void rtc_reset (void)
{
#ifdef CONFIG_SYS_RTC_DS1337
	rtc_write (RTC_CTL_REG_ADDR, RTC_DS1337_RESET_VAL);
#elif defined CONFIG_SYS_RTC_DS1388
	rtc_write(RTC_CTL_REG_ADDR, 0x0); /* hw default */
#endif
#ifdef CONFIG_SYS_DS1339_TCR_VAL
	rtc_write (RTC_TC_REG_ADDR, CONFIG_SYS_DS1339_TCR_VAL);
#endif
#ifdef CONFIG_SYS_DS1388_TCR_VAL
	rtc_write(RTC_TC_REG_ADDR, CONFIG_SYS_DS1388_TCR_VAL);
#endif
}

#endif
