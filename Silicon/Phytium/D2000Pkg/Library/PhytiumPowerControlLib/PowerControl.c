#include <Library/ArmPlatformLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/ArmSmcLib.h>
#include "../ec/ec.h"

#define TICKS_PER_MS   48000
/*
*		CPLD interface
*/
#define PWR_CTR0_REG	0x28180480
#define PWR_CTR1_REG	0x28180484
UINTN get_ticks(void)
{
   UINTN cntpct;

   asm volatile("mrs %0, cntpct_el0" : "=r" (cntpct));

   return cntpct;
}

void mdelay(UINT32 time)
{
    UINTN start, end;

    start = get_ticks();
    end = start + time * TICKS_PER_MS;
    while(get_ticks() <= end) {

    }
}


void send_cpld_ctr(UINT32 cmd)
{
	UINT32 i;
	DEBUG((EFI_D_INFO, "send cmd to cpld : %d \n", cmd));
	//start
	MmioWrite32(PWR_CTR0_REG, 0x1);
	mdelay(2);
	for(i = 0; i < cmd; i++){
		MmioWrite32(PWR_CTR1_REG, 0x1);
		mdelay(1);
		MmioWrite32(PWR_CTR1_REG, 0x0);
		mdelay(1);
	}
	//end
	mdelay(2);
	MmioWrite32(PWR_CTR0_REG, 0x0);
}

void send_ec_ctr(UINT32 cmd)
{
	DEBUG((EFI_D_INFO,"send cmd to ec : %d \n", cmd));
    MmioWrite32(0x27ffffcc, 1);
    MmioWrite32(0x27ffffe8, 0x80000000);
    MmioWrite32(0x27ffffd8, 0);
    MmioWrite32(0x27ffffd4, 0);
    mdelay(50);
        if(cmd==4)
        EcWriteMem(0x0A,0x08);
    else if(cmd==8)
        EcWriteMem(0x0A,0x01);
    else if(cmd==12)
        EcWriteMem(0x0A,0x04);
    mdelay(50);
}

#define S3_CLEAN_CPLD			1
#define REBOOT_CPLD				4
#define VTT_DISABLE_CPLD        5
#define VTT_ENABLE_CPLD         6
#define S3_SETUP_CPLD			8
#define SHUTDOWN_CPLD			12

void cpld_power_off(void)
{
	send_cpld_ctr(SHUTDOWN_CPLD);
}

void cpld_reboot(void)
{
	send_cpld_ctr(REBOOT_CPLD);
}

void ec_power_off(void)
{
	send_ec_ctr(SHUTDOWN_CPLD);
}

void ec_reboot(void)
{
	send_ec_ctr(REBOOT_CPLD);
}
/*
 *
*/
typedef struct pwr_ctr_fun{
	void (*power_off)(void);
	void (*reboot)(void);
	UINT32 (*get_s3_flag)(void);
	void (*set_s3_flag)(void);
	void (*clean_s3_flag)(void);
	void (*enable_vtt)(void);
	void (*disable_vtt)(void);
}pwr_ctr_fun_t;

/*GPIO*/
typedef struct pad_info{
	UINT32 pad_sel_reg;
	UINT32 bit_offset;
	UINT32 gpio_swport_ddr;
	UINT32 gpio_ext_port;
	UINT32 port;
}pad_info_t;

#define GPIO0_BASE 0x28004000
#define SWPORTA_DDR_OFFSET 	 0x4
#define SWPORTB_DDR_OFFSET 	 0x10
#define EXT_PORTA_OFFSET 	 0x8
#define EXT_PORTB_OFFSET 	 0x14

#ifdef	CONFIG_CHANGCHENG_XINAN
pad_info_t s3_flag_gpio = {
	.pad_sel_reg = 0x2818020c,
	.bit_offset = 24,
	.gpio_swport_ddr =GPIO1_BASE + SWPORTB_DDR_OFFSET,
	.gpio_ext_port = GPIO1_BASE + EXT_PORTB_OFFSET,
	.port = 3,
};

#else
pad_info_t s3_flag_gpio = {
	.pad_sel_reg = 0x28180200,
	.bit_offset = 20,
	.gpio_swport_ddr =GPIO0_BASE + SWPORTA_DDR_OFFSET,
	.gpio_ext_port = GPIO0_BASE + EXT_PORTA_OFFSET,
	.port = 1,
};
#endif
//复用gpio0_a1位gpio，且为输入模式（默认为输入模式），读取对应状态
UINT32 gpio_get_s3_flag(void)
{
	DEBUG((EFI_D_INFO, "s3 flag form gpio\n"));
	UINT32 temp, temp1, ret;

	UINT32 pad_sel_reg = s3_flag_gpio.pad_sel_reg;
	UINT32 bit_offset = s3_flag_gpio.bit_offset;

	UINT32 gpio_swport_ddr = s3_flag_gpio.gpio_swport_ddr;
	UINT32 gpio_ext_port = s3_flag_gpio.gpio_ext_port;
	UINT32 port = s3_flag_gpio.port;

	//读取之前配置方向寄存器
	temp1 = MmioRead32(gpio_swport_ddr);
	//配置为输入模式
	MmioWrite32(gpio_swport_ddr, temp1 & (1 << port));

	temp = MmioRead32(pad_sel_reg);
	MmioWrite32(pad_sel_reg, (temp & (~(0xf << bit_offset))) | (0x1 << bit_offset));
	mdelay(10);
	//读取当前控io寄存器的值
	ret = MmioRead32(gpio_ext_port);
	//恢复配置
	MmioWrite32(gpio_swport_ddr, temp1);
	MmioWrite32(pad_sel_reg, temp);
	//判断对应io引脚是否为高电平
	if((1 << port) == ret)
		return	1;
	else
		return	0;
}
void gpio_set_s3_flag(void)
{
	DEBUG((EFI_D_INFO, "s3 set flag by gpio\n"));
	send_cpld_ctr(S3_SETUP_CPLD);
}

void gpio_clean_s3_flag(void)
{
	DEBUG((EFI_D_INFO,"s3 clean flag by gpio\n"));
	send_cpld_ctr(S3_CLEAN_CPLD);
}

void gpio_enable_vtt(void)
{
  DEBUG((EFI_D_INFO, "s3 enable vtt by gpio"));
  send_cpld_ctr(VTT_ENABLE_CPLD);
}

void gpio_disable_vtt(void)
{
  DEBUG((EFI_D_INFO, "s3 disable vtt by gpio"));
  send_cpld_ctr(VTT_DISABLE_CPLD);
}


static pwr_ctr_fun_t pwr_ctr_gpio = {
	.power_off = cpld_power_off,
	.reboot = cpld_reboot,
	.get_s3_flag = gpio_get_s3_flag,
	.set_s3_flag = gpio_set_s3_flag,
	.clean_s3_flag = gpio_clean_s3_flag,
	.enable_vtt = gpio_enable_vtt,
	.disable_vtt = gpio_disable_vtt
};
/*RTC*/
#define	RTC_BASE			0x2800D000
#define	RTC_AES_SEL			(RTC_BASE + 0x08)
#define	RTC_AES_KEY7_L		(RTC_BASE + 0x68)
#define	RTC_AES_KEY7_H		(RTC_BASE + 0x6c)

#define	RTC_S3_FLAG_MAGIC	0xFF005566
UINT32 rtc_get_s3_flag(void)
{
	UINT32 rtc_h,rtc_l;

	DEBUG((EFI_D_INFO,"s3 flag form rtc\n"));
	MmioWrite32(RTC_AES_SEL, 0x80);
	mdelay(1);
	//先读高位 再读低位
	rtc_h = MmioRead32(RTC_AES_KEY7_H);
	rtc_l = MmioRead32(RTC_AES_KEY7_L);
	if(RTC_S3_FLAG_MAGIC == rtc_h && RTC_S3_FLAG_MAGIC == rtc_l)
		return 1;
	else
		return 0;
}
void rtc_set_s3_flag(void)
{
	DEBUG((EFI_D_INFO, "s3 set flag by rtc\n"));
	MmioWrite32(RTC_AES_SEL, 0x80);
	mdelay(1);
	//先写低位 再写高位
	MmioWrite32(RTC_AES_KEY7_L, RTC_S3_FLAG_MAGIC);
	MmioWrite32(RTC_AES_KEY7_H, RTC_S3_FLAG_MAGIC);

	send_cpld_ctr(S3_SETUP_CPLD);

}

void rtc_clean_s3_flag(void)
{
	DEBUG((EFI_D_INFO, "s3 clean flag by rtc\n"));
	MmioWrite32(RTC_AES_SEL, 0x80);
	mdelay(1);
	MmioWrite32(RTC_AES_KEY7_L, 0);
	MmioWrite32(RTC_AES_KEY7_H, 0);

	send_cpld_ctr(S3_CLEAN_CPLD);
}

static pwr_ctr_fun_t pwr_ctr_rtc = {
	.power_off = cpld_power_off,
	.reboot = cpld_reboot,
	.get_s3_flag = rtc_get_s3_flag,
	.set_s3_flag = rtc_set_s3_flag,
	.clean_s3_flag = rtc_clean_s3_flag,
};

#define LPC_BASE            0x20000000
#define PM2_CMD             0x66
#define PM2_DATA            0x62
#define PM2_CMD_R           0x80
#define PM2_CMD_W           0x81
#define EC_S3_FLAG_MAGIC    0x55
#define EC_S3S4_FLAG_MAGIC  0xAA

UINT32 ec_get_s3_flag(void)
{
	UINT8 ec_flag=0;
	DEBUG((EFI_D_INFO,"s3 flag form ec\n"));
    MmioWrite32(0x27ffffcc, 1);
    MmioWrite32(0x27ffffe8, 0x80000000);
    MmioWrite32(0x27ffffd8, 0);
    MmioWrite32(0x27ffffd4, 0);
    mdelay(50);
    EcReadMem(0x0B,&ec_flag);
	if(EC_S3_FLAG_MAGIC == ec_flag || EC_S3S4_FLAG_MAGIC == ec_flag)
		return 1;
	else
		return 0;
}
void ec_set_s3_flag(void)
{
	DEBUG((EFI_D_INFO, "s3 set flag by ec\n"));
    MmioWrite32(0x27ffffcc, 1);
    MmioWrite32(0x27ffffe8, 0x80000000);
    MmioWrite32(0x27ffffd8, 0);
    MmioWrite32(0x27ffffd4, 0);
    mdelay(50);
    EcWriteMem(0x0B,0x55);
    mdelay(50);
    send_ec_ctr(S3_SETUP_CPLD);

}
void ec_clean_s3_flag(void)
{

}

void ec_enable_vtt(void)
{
  DEBUG((EFI_D_INFO, "s3 enable vtt by ec"));
}

void ec_disable_vtt(void)
{
  DEBUG((EFI_D_INFO, "s3 disable vtt by ec"));
}


static pwr_ctr_fun_t pwr_ctr_ec = {
	.power_off = ec_power_off,
	.reboot = ec_reboot,
	.get_s3_flag = ec_get_s3_flag,
	.set_s3_flag = ec_set_s3_flag,
	.clean_s3_flag = ec_clean_s3_flag,
	.enable_vtt = ec_enable_vtt,
	.disable_vtt = ec_disable_vtt,
};

UINT32 pm_get_s3_flag_source(void)
{
	//gpio
    return 1;
	//ec
	//return 2;
}

void * get_flag_fun(void)
{
	switch(pm_get_s3_flag_source())
	{
	case 0:
		return &pwr_ctr_rtc;
	case 1:
		return &pwr_ctr_gpio;
	case 2:
		return &pwr_ctr_ec;
	default:
		return &pwr_ctr_rtc;
	}

}

void SystemOff(void)
{
	pwr_ctr_fun_t *pwr_flag_p = get_flag_fun();

	if(NULL == pwr_flag_p || NULL == pwr_flag_p->power_off){
		DEBUG((EFI_D_ERROR, "%a(),Line=%d\n", __FUNCTION__, __LINE__));
		while(1);
	}

	pwr_flag_p->power_off();
}

void SystemReboot(void)
{
	pwr_ctr_fun_t *pwr_flag_p = get_flag_fun();

	if(NULL == pwr_flag_p || NULL == pwr_flag_p->reboot){
		DEBUG((EFI_D_ERROR, "%a(),Line=%d\n", __FUNCTION__, __LINE__));
		while(1);
	}

	pwr_flag_p->reboot();
}

UINT32 get_s3_flag(void)
 {
	pwr_ctr_fun_t *pwr_flag_p = get_flag_fun();

	if(NULL == pwr_flag_p || NULL == pwr_flag_p->get_s3_flag){
		DEBUG((EFI_D_ERROR, "%a(),Line=%d\n", __FUNCTION__, __LINE__));
		while(1);
	}
	return (pwr_flag_p->get_s3_flag());
}

void set_s3_flag(void)
{
	pwr_ctr_fun_t *pwr_flag_p = get_flag_fun();

	if(NULL == pwr_flag_p || NULL == pwr_flag_p->set_s3_flag){
		DEBUG((EFI_D_ERROR, "%a(),Line=%d\n", __FUNCTION__, __LINE__));
		while(1);
	}

	pwr_flag_p->set_s3_flag();
}

void clean_s3_flag(void)
{
	pwr_ctr_fun_t *pwr_flag_p = get_flag_fun();

	if(NULL == pwr_flag_p || NULL == pwr_flag_p->clean_s3_flag){
		DEBUG((EFI_D_ERROR, "%a(),Line=%d\n", __FUNCTION__, __LINE__));
		while(1);
	}
	pwr_flag_p->clean_s3_flag();
}

void enable_vtt(void)
{
	pwr_ctr_fun_t *pwr_flag_p = get_flag_fun();

	if(NULL == pwr_flag_p || NULL == pwr_flag_p->clean_s3_flag){
		DEBUG((EFI_D_ERROR, "%a(),Line=%d\n", __FUNCTION__, __LINE__));
		while(1);
	}
	pwr_flag_p->enable_vtt();
}

void disable_vtt(void)
{
	pwr_ctr_fun_t *pwr_flag_p = get_flag_fun();

	if(NULL == pwr_flag_p || NULL == pwr_flag_p->clean_s3_flag){
		DEBUG((EFI_D_ERROR, "%a(),Line=%d\n", __FUNCTION__, __LINE__));
		while(1);
	}
	pwr_flag_p->disable_vtt();
}

