#ifndef _MCU_INFO_H
#define _MCU_INFO_H

#include <Library/DebugLib.h>

typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint64_t;
typedef unsigned long uintptr_t;
typedef unsigned long u64;
typedef signed char int8_t;

#define printf(arg...) DEBUG((EFI_D_ERROR,## arg))
#define PARAMETER_CPU_MAGIC                 0x54460010

typedef struct ddr_spd {
    /******************* read from spd *****************/
    uint8_t  dimm_type;     /* 1: RDIMM;2: UDIMM;3: SODIMM;4: LRDIMM */
    uint8_t  data_width;      /* 0: x4; 1: x8; 2: x16 */
    uint8_t  mirror_type;     /* 0: stardard; 1: mirror */
    uint8_t  ecc_type;       /* 0: no-ecc; 1:ecc */
    uint8_t  dram_type;       /* 0xB: DDR3; 0xC: DDR4 */
    uint8_t  rank_num;
    uint8_t  row_num;
    uint8_t  col_num;

    uint8_t  bg_num;		/*only DDR4*/
    uint8_t  bank_num;
    uint16_t module_manufacturer_id;
    uint16_t tAAmin;
    uint16_t tRCDmin;

    uint16_t tRPmin;
    uint16_t tRASmin;
    uint16_t tRCmin;
    uint16_t tFAWmin;

    uint16_t tRRD_Smin;		/*only DDR4*/
    uint16_t tRRD_Lmin;		/*only DDR4*/
    uint16_t tCCD_Lmin;		/*only DDR4*/
    uint16_t tWRmin;

    uint16_t tWTR_Smin;		/*only DDR4*/
    uint16_t tWTR_Lmin;		/*only DDR4*/
    uint16_t tWTRmin;		/*only DDR3*/
    uint16_t tRRDmin;		/*only DDR3*/

    /******************* RCD control words *****************/
    uint8_t  F0RC03; /*bit[3:2]:CS         bit[1:0]:CA  */
    uint8_t  F0RC04; /*bit[3:2]:ODT        bit[1:0]:CKE */
    uint8_t  F0RC05; /*bit[3:2]:CLK-A side bit[1:0]:CLK-B side */
    uint8_t  BC00;
    uint8_t  BC01;
    uint8_t  BC02;
    uint8_t  BC03;
    uint8_t  BC04;

    uint8_t  BC05;
    uint8_t  F5BC5x;
    uint8_t  F5BC6x;
/******************* LRDIMM special *****************/
    uint8_t  vrefDQ_PR0;  /*Byte 140 (0x08C) (Load Reduced): DRAM VrefDQ for Package Rank 0*/
    uint8_t  vrefDQ_MDRAM;/* Byte 144 (0x090) (Load Reduced): Data Buffer VrefDQ for DRAM Interface*/
    uint8_t  RTT_MDRAM_1866; /*Byte 145 (0x091) (Load Reduced): Data Buffer MDQ Drive Strength and RTT for data rate < 1866 */
    uint8_t  RTT_MDRAM_2400; /*Byte 146 (0x092) (Load Reduced): Data Buffer MDQ Drive Strength and RTT for 1866<datarate<2400 */
    uint8_t  RTT_MDRAM_3200; /*Byte 147 (0x093) (Load Reduced): Data Buffer MDQ Drive Strength and RTT for 2400<datarate<3200 */

    uint8_t  Drive_DRAM;  /*Byte 148 (0x094) (Load Reduced): DRAM Drive Strength */
    uint8_t  ODT_DRAM_1866; /* Byte 149 (0x095) (Load Reduced): DRAM ODT (RTT_WR and RTT_NOM) for data rate < 1866 */
    uint8_t  ODT_DRAM_2400; /* Byte 150 (0x096) (Load Reduced): DRAM ODT (RTT_WR and RTT_NOM) for 1866 < data rate < 2400 */
    uint8_t  ODT_DRAM_3200; /* Byte 151 (0x097) (Load Reduced): DRAM ODT (RTT_WR and RTT_NOM) for 2400 < data rate < 3200 */
    uint8_t  PARK_DRAM_1866; /* Byte 152 (0x098) (Load Reduced): DRAM ODT (RTT_PARK) for data rate < 1866 */
    uint8_t  PARK_DRAM_2400; /* Byte 153 (0x099) (Load Reduced): DRAM ODT (RTT_PARK) for 1866 < data rate < 2400 */
    uint8_t  PARK_DRAM_3200; /* Byte 154 (0x09A) (Load Reduced): DRAM ODT (RTT_PARK) for 2400 < data rate  < 3200 */
    uint8_t  rcd_num;   /* Registers used on RDIMM */

}__attribute__((aligned(sizeof(unsigned long)))) ddr_spd_info_t;

typedef struct mcu_config {
    uint32_t magic;
	uint32_t version;
	uint32_t size;
	uint8_t rev1[4];

    uint8_t ch_enable;
    uint8_t ecc_enable;
    uint8_t dm_enable;
	uint8_t force_spd_enable;
    uint8_t mcu_misc_enable;	/*0:read spd req 1:use margin 2: s3 back devinit 3: cmd 2T mode 4:dual dimm*/
    uint8_t train_debug;
    uint8_t train_recover;

    uint8_t rev2[9];

    ddr_spd_info_t ddr_spd_info[2];
}__attribute__((aligned(sizeof(unsigned long)))) mcu_config_t;

void get_mcu_param_info(mcu_config_t *mcu_info);

#define MISC_READ_SPD_FREQ_EN	(0x1<<0)
#define MISC_USE_MARGIN_EN		(0x1<<1)
#define MISC_S3_DEVINIT_EN		(0x1<<2)
#define MISC_2T_MODE_EN			(0x1<<3)
#define MISC_DUAL_DIMM_EN		(0x1<<4)


#define PARAMETER_MCU_MAGIC		0x54460014
#define PARAM_MCU_VERSION		0x1
#define PARAM_MCU_SIZE			0x100
#define PARAM_CH_ENABLE			0x3
#define PARAM_ECC_ENABLE		0x3
#define PARAM_DM_ENABLE			0x3
#define	PARAM_FORCE_SPD_DISABLE	0x0
#define PARAM_MCU_MISC_ENABLE	0x0
#define PARAM_TRAIN_DEBUF		0xff

//#define MCU_USE_MANUAL
//#define MCU_USE_PARAMETER

#define     MASK_4BIT               0xF

#define     SPD_MTB                 125     //125ps
#define     SPD_FTB                 1       //1ps

#define     RDIMM_TYPE              1
#define     UDIMM_TYPE              2
#define     SODIMM_TYPE             3
#define     LRDIMM_TYPE             4
#define     DIMM_x4                 0x0
#define     DIMM_x8                 0x1
#define     DIMM_x16                0x2
#define     DIMM_x32                0x3
#define     MIRROR                  0x1
#define     NO_MIRROR               0x0
#define     ECC_TYPE                1
#define     NO_ECC_TYPE             0
#define     ROW_NUM                 16
#define     COL_NUM                 10
#define     DDR3_TYPE               0xB
#define     DDR4_TYPE               0xC

#define SAMSUNG_VENDOR      0x80CE
#define HYNIX_VENDOR        0x80AD
#define MICRON_VENDOR       0x802C
#define KINGSTON_VENDOR     0x0198
#define RAMAXEL_VENDOR      0x0443
#define LANQI_VENDOR        0x8632
#define CXMT_VENDOR			0x8A91
#define UNILC_VENDOR		0x081A

//----------------------i2c---------------------//
static inline void mmio_write_32(uintptr_t addr, uint32_t value)
{
	*(volatile uint32_t*)addr = value;
}

static inline uint32_t mmio_read_32(uintptr_t addr)
{
	return *(volatile uint32_t*)addr;
}
#define		SPD_NUM			256

#define		LMU0_I2C_ID		0
#define		LMU1_I2C_ID		0
#define		LMU0_I2C_SLAVE_ADDR	0x50
#define		LMU1_I2C_SLAVE_ADDR	0x51

struct i2c_regs {
	uint32_t ic_con;
	uint32_t ic_tar;
	uint32_t ic_sar;
	uint32_t ic_hs_maddr;
	uint32_t ic_cmd_data;
	uint32_t ic_ss_scl_hcnt;
	uint32_t ic_ss_scl_lcnt;
	uint32_t ic_fs_scl_hcnt;
	uint32_t ic_fs_scl_lcnt;
	uint32_t ic_hs_scl_hcnt;
	uint32_t ic_hs_scl_lcnt;
	uint32_t ic_intr_stat;
	uint32_t ic_intr_mask;
	uint32_t ic_raw_intr_stat;
	uint32_t ic_rx_tl;
	uint32_t ic_tx_tl;
	uint32_t ic_clr_intr;
	uint32_t ic_clr_rx_under;
	uint32_t ic_clr_rx_over;
	uint32_t ic_clr_tx_over;
	uint32_t ic_clr_rd_req;
	uint32_t ic_clr_tx_abrt;
	uint32_t ic_clr_rx_done;
	uint32_t ic_clr_activity;
	uint32_t ic_clr_stop_det;
	uint32_t ic_clr_start_det;
	uint32_t ic_clr_gen_call;
	uint32_t ic_enable;
	uint32_t ic_status;
	uint32_t ic_txflr;
	uint32_t ix_rxflr;
	uint32_t reserved_1;
	uint32_t ic_tx_abrt_source;
};

#define MIO_BASE        0x28000000 /* 1MB: Uart/i2c/wdt/gpio */
#define GPIO0_BASE      (MIO_BASE + 0x4000)
#define GPIO1_BASE      (MIO_BASE + 0x5000)
#define SWPORTA_DDR_OFFSET   0x4
#define SWPORTB_DDR_OFFSET   0x10
#define EXT_PORTA_OFFSET     0x8
#define EXT_PORTB_OFFSET     0x14


#define PWR_CTR0_REG    0x28180480
#define PWR_CTR1_REG    0x28180484

#define S3_CLEAN_CPLD           1
#define REBOOT_CPLD             4
#define S3_SETUP_CPLD           8
#define SHUTDOWN_CPLD           12

#define writel(b,addr) ((*(volatile uint32_t *) (addr)) = (b))
#define readl(addr) (*(volatile uint32_t *) (addr))

#define I2C0_BASE       (MIO_BASE + 0x6000)

#if !defined(IC_CLK)
#define IC_CLK			166
#endif
#define NANO_TO_MICRO		1000

/* High and low times in different speed modes (in ns) */
#define MIN_SS_SCL_HIGHTIME	4000
#define MIN_SS_SCL_LOWTIME	4700
#define MIN_FS_SCL_HIGHTIME	600
#define MIN_FS_SCL_LOWTIME	1300
#define MIN_HS_SCL_HIGHTIME	60
#define MIN_HS_SCL_LOWTIME	160

/* Worst case timeout for 1 byte is kept as 2ms */
#define I2C_BYTE_TO		(CONFIG_SYS_HZ/500)
#define I2C_STOPDET_TO		(CONFIG_SYS_HZ/500)
#define I2C_BYTE_TO_BB		(I2C_BYTE_TO * 16)

/* i2c control register definitions */
#define IC_CON_SD		0x0040
#define IC_CON_RE		0x0020
#define IC_CON_10BITADDRMASTER	0x0010
#define IC_CON_10BITADDR_SLAVE	0x0008
#define IC_CON_SPD_MSK		0x0006
#define IC_CON_SPD_SS		0x0002
#define IC_CON_SPD_FS		0x0004
#define IC_CON_SPD_HS		0x0006
#define IC_CON_MM		0x0001

/* i2c target address register definitions */
#define TAR_ADDR		0x0050

/* i2c slave address register definitions */
#define IC_SLAVE_ADDR		0x0002

/* i2c data buffer and command register definitions */
#define IC_CMD			0x0100
#define IC_STOP			0x0200

/* i2c interrupt status register definitions */
#define IC_GEN_CALL		0x0800
#define IC_START_DET		0x0400
#define IC_STOP_DET		0x0200
#define IC_ACTIVITY		0x0100
#define IC_RX_DONE		0x0080
#define IC_TX_ABRT		0x0040
#define IC_RD_REQ		0x0020
#define IC_TX_EMPTY		0x0010
#define IC_TX_OVER		0x0008
#define IC_RX_FULL		0x0004
#define IC_RX_OVER 		0x0002
#define IC_RX_UNDER		0x0001

/* fifo threshold register definitions */
#define IC_TL0			0x00
#define IC_TL1			0x01
#define IC_TL2			0x02
#define IC_TL3			0x03
#define IC_TL4			0x04
#define IC_TL5			0x05
#define IC_TL6			0x06
#define IC_TL7			0x07
#define IC_RX_TL		IC_TL0
#define IC_TX_TL		IC_TL0

/* i2c enable register definitions */
#define IC_ENABLE_0B		0x0001

/* i2c status register  definitions */
#define IC_STATUS_SA		0x0040
#define IC_STATUS_MA		0x0020
#define IC_STATUS_RFF		0x0010
#define IC_STATUS_RFNE		0x0008
#define IC_STATUS_TFE		0x0004
#define IC_STATUS_TFNF		0x0002
#define IC_STATUS_ACT		0x0001

/* Speed Selection */
#define IC_SPEED_MODE_STANDARD	1
#define IC_SPEED_MODE_FAST	2
#define IC_SPEED_MODE_MAX	3

#define I2C_MAX_SPEED		3400000
#define I2C_FAST_SPEED		400000
#define I2C_STANDARD_SPEED	100000

#endif
