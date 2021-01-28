#include "mcu_info.h"


//#define MCU_USE_MANUAL
static void get_mcu_up_info_default(mcu_config_t *pm)
{
	pm->magic = PARAMETER_MCU_MAGIC;
	pm->version = PARAM_MCU_VERSION;
	pm->size = PARAM_MCU_SIZE;
	pm->ch_enable = PARAM_CH_ENABLE;
	pm->ecc_enable = PARAM_ECC_ENABLE;
	pm->dm_enable = PARAM_DM_ENABLE;
	pm->force_spd_enable = PARAM_FORCE_SPD_DISABLE;
	pm->mcu_misc_enable = PARAM_MCU_MISC_ENABLE;
	pm->train_debug = PARAM_TRAIN_DEBUF;
	pm->train_recover = 0;
}

static void print_ddr_info(uint8_t ch, mcu_config_t *pm)
{
	uint8_t temp;
	uint16_t sdram_density;		//Mb
	uint64_t dimm_capacity;		//G

#if 1
	printf("  tRCDmin = %dps\n", pm->ddr_spd_info[ch].tRCDmin);
    printf("  tRPmin = %dps\n", pm->ddr_spd_info[ch].tRPmin);
    printf("  tRASmin = %dps\n", pm->ddr_spd_info[ch].tRASmin);
    printf("  tRCmin = %dps\n", pm->ddr_spd_info[ch].tRCmin);
    printf("  tFAWmin = %dps\n", pm->ddr_spd_info[ch].tFAWmin);
    if(pm->ddr_spd_info[ch].dram_type == DDR3_TYPE){
        printf("  tRRDmin = %dps\n", pm->ddr_spd_info[ch].tRRDmin);
        printf("  tWRmin = %dps\n", pm->ddr_spd_info[ch].tWRmin);
        printf("  tWTRmin = %dps\n", pm->ddr_spd_info[ch].tWTRmin);
    }else{
        printf("  tRRD_Smin = %dps\n", pm->ddr_spd_info[ch].tRRD_Smin);
        printf("  tRRD_Lmin = %dps\n", pm->ddr_spd_info[ch].tRRD_Lmin);
        printf("  tCCD_Lmin = %dps\n", pm->ddr_spd_info[ch].tCCD_Lmin);
        printf("  tWRmin = %dps\n", pm->ddr_spd_info[ch].tWRmin);
        printf("  tWTR_Smin = %dps\n", pm->ddr_spd_info[ch].tWTR_Smin);
        printf("  tWTR_Lmin = %dps\n", pm->ddr_spd_info[ch].tWTR_Lmin);
    }
#endif

	temp = 4<<pm->ddr_spd_info[ch].data_width;
	if(pm->ddr_spd_info[ch].dram_type == DDR3_TYPE){
		sdram_density =
            (1UL<<(pm->ddr_spd_info[ch].row_num + pm->ddr_spd_info[ch].col_num)) * temp
            * pm->ddr_spd_info[ch].bank_num >> 20; //units: Mb
	}else{
		sdram_density =
            (1UL<<(pm->ddr_spd_info[ch].row_num + pm->ddr_spd_info[ch].col_num)) * temp
            * pm->ddr_spd_info[ch].bg_num * pm->ddr_spd_info[ch].bank_num >> 20; //units: Mb
	}
	dimm_capacity = sdram_density * pm->ddr_spd_info[ch].rank_num * 64 / temp >> 13;   //units: GB
	printf("\tDimm_Capacity = %lldGB\n", dimm_capacity );

	switch(pm->ddr_spd_info[ch].dram_type){
		case 0xb:
			printf("\tDDR3"); break;
		case 0xc:
			printf("\tDDR4"); break;
		default:
			printf("\tdram_type=0x%x", pm->ddr_spd_info[ch].dram_type);
	}
	switch(pm->ddr_spd_info[ch].dimm_type){
		case 1:
			printf("\tRDIMM"); break;
		case 2:
			printf("\tUDIMM"); break;
		case 3:
		case 9:
			printf("\tSODIMM"); break;
		case 4:
			printf("\tLRDIMM"); break;
		default:
			printf("\tdimm_type=0x%x", pm->ddr_spd_info[ch].dimm_type);
	}
	if(pm->ddr_spd_info[ch].dram_type == DDR4_TYPE){
		printf("/%d Bank Groups", pm->ddr_spd_info[ch].bg_num);
		printf("/%d Banks", pm->ddr_spd_info[ch].bank_num);
	}else{
		printf("/%d Banks", pm->ddr_spd_info[ch].bank_num);
		printf("/sdram_density=%dGb", sdram_density >> 10);
	}
	printf("/Column %d", pm->ddr_spd_info[ch].col_num);
	printf("/Row %d", pm->ddr_spd_info[ch].row_num);
	switch(pm->ddr_spd_info[ch].data_width){
		case DIMM_x4:
			printf("/X4"); break;
		case DIMM_x8:
			printf("/X8"); break;
		case DIMM_x16:
			printf("/X16"); break;
		case DIMM_x32:
			printf("/X32"); break;
		default:
			printf("/data_width=0x%x", pm->ddr_spd_info[ch].data_width);
	}
	printf("/%d Rank", pm->ddr_spd_info[ch].rank_num);
	switch(pm->ddr_spd_info[ch].ecc_type){
		case 0:
			printf("/NO ECC"); break;
		case 1:
			printf("/ECC"); break;
		default:
			printf("/ecc_type=0x%x", pm->ddr_spd_info[ch].ecc_type);
	}
	if(pm->ddr_spd_info[ch].mirror_type == 0)
		printf("/Standard\n");
	else
		printf("/Mirror\n");
	printf("\tModual:");
	switch(pm->ddr_spd_info[ch].module_manufacturer_id){
		case SAMSUNG_VENDOR:
			printf("Samsung"); break;
		case MICRON_VENDOR:
			printf("Micron"); break;
		case HYNIX_VENDOR:
			printf("Hynix"); break;
		case KINGSTON_VENDOR:
			printf("KingSton"); break;
		case RAMAXEL_VENDOR:
			printf("Ramaxel"); break;
		case LANQI_VENDOR:
			printf("Lanqi"); break;
		default:
			printf("Unknown=0x%x", pm->ddr_spd_info[ch].module_manufacturer_id);
	}
}

struct i2c_regs* chose_i2c(int i2c_id)
{
	struct i2c_regs *i2c0_regs_p = (struct i2c_regs *)I2C0_BASE;
//	struct i2c_regs *i2c1_regs_p = (struct i2c_regs *)I2C1_BASE;
//	if (0x0 == i2c_id)
		return i2c0_regs_p;
//	else
//		return i2c1_regs_p;
}

static void set_speed(int i2c_spd, int i2c_id)
{
	unsigned int cntl;
	unsigned int hcnt, lcnt;
	struct i2c_regs *i2c_regs_p;

	i2c_regs_p = chose_i2c(i2c_id);

	cntl = (mmio_read_32((uintptr_t)&i2c_regs_p->ic_con) & (~IC_CON_SPD_MSK));

	switch (i2c_spd) {
	case IC_SPEED_MODE_MAX:
		cntl |= IC_CON_SPD_HS;
		hcnt = (IC_CLK * MIN_HS_SCL_HIGHTIME) / NANO_TO_MICRO;
		mmio_write_32((uintptr_t)&i2c_regs_p->ic_hs_scl_hcnt , hcnt);
		lcnt = (IC_CLK * MIN_HS_SCL_LOWTIME) / NANO_TO_MICRO;
		mmio_write_32((uintptr_t)&i2c_regs_p->ic_hs_scl_lcnt, lcnt);
		break;

	case IC_SPEED_MODE_STANDARD:
		cntl |= IC_CON_SPD_SS;
		hcnt = (IC_CLK * MIN_SS_SCL_HIGHTIME) / NANO_TO_MICRO;
		mmio_write_32((uintptr_t)&i2c_regs_p->ic_ss_scl_hcnt , hcnt);
		lcnt = (IC_CLK * MIN_SS_SCL_LOWTIME) / NANO_TO_MICRO;
		mmio_write_32((uintptr_t)&i2c_regs_p->ic_ss_scl_lcnt , lcnt);
		break;

	case IC_SPEED_MODE_FAST:
	default:
		cntl |= IC_CON_SPD_FS;
		hcnt = (IC_CLK * MIN_FS_SCL_HIGHTIME) / NANO_TO_MICRO;
		mmio_write_32((uintptr_t)&i2c_regs_p->ic_fs_scl_hcnt , hcnt);
		lcnt = (IC_CLK * MIN_FS_SCL_LOWTIME) / NANO_TO_MICRO;
		mmio_write_32((uintptr_t)&i2c_regs_p->ic_fs_scl_lcnt , lcnt);
		break;
	}

	mmio_write_32((uintptr_t)&i2c_regs_p->ic_con , cntl);
}

static int i2c_set_bus_speed(int speed, int i2c_id)
{
	if (speed >= I2C_MAX_SPEED)
		set_speed(IC_SPEED_MODE_MAX, i2c_id);
	else if (speed >= I2C_FAST_SPEED)
		set_speed(IC_SPEED_MODE_FAST, i2c_id);
	else
		set_speed(IC_SPEED_MODE_STANDARD, i2c_id);

	return 0;
}

void init_spd_i2c(int i2c_id)
{
	struct i2c_regs *i2c_regs_p;
	unsigned int enbl;

	i2c_regs_p = chose_i2c(i2c_id);

	/* Disable i2c */
	enbl = mmio_read_32((uintptr_t)&i2c_regs_p->ic_enable);
	enbl &= ~IC_ENABLE_0B;
	mmio_write_32((uintptr_t)&i2c_regs_p->ic_enable , enbl);

	mmio_write_32((uintptr_t)&i2c_regs_p->ic_con ,(IC_CON_SD | IC_CON_RE | IC_CON_SPD_FS | IC_CON_MM));

	//mmio_write_32(&i2c_regs_p->ic_rx_tl , IC_RX_TL);
	//mmio_write_32(&i2c_regs_p->ic_tx_tl , IC_TX_TL);

	i2c_set_bus_speed(I2C_FAST_SPEED, i2c_id);
	//mmio_write_32(&i2c_regs_p->ic_intr_mask , IC_STOP_DET);


	mmio_write_32((uintptr_t)&i2c_regs_p->ic_sar , 0);

	/* Enable i2c */
	enbl = mmio_read_32((uintptr_t)&i2c_regs_p->ic_enable);
	enbl |= IC_ENABLE_0B;
	mmio_write_32((uintptr_t)&i2c_regs_p->ic_enable , enbl);
}

static void spd_i2c_init(void)
{
	init_spd_i2c(0x0);
//	init_spd_i2c(0x1);
}

int i2c_wait_for_bb(int i2c_id)
{
	struct i2c_regs *i2c_regs_p;

	i2c_regs_p = chose_i2c(i2c_id);

	while ((mmio_read_32((uintptr_t)&i2c_regs_p->ic_status) & IC_STATUS_MA) ||
	       !(mmio_read_32((uintptr_t)&i2c_regs_p->ic_status) & IC_STATUS_TFE)) {
	}

	return 0;
}

int i2c_xfer_init(unsigned char chip, int addr, int i2c_id)
{
	struct i2c_regs *i2c_regs_p;

	if (i2c_wait_for_bb(i2c_id))
		return 1;

	i2c_regs_p = chose_i2c(i2c_id);

	/* set chip address */
	mmio_write_32((uintptr_t)&i2c_regs_p->ic_tar, chip);

	/* set register address */
	mmio_write_32((uintptr_t)&i2c_regs_p->ic_cmd_data , addr);

	return 0;
}

int i2c_xfer_finish(int i2c_id)
{
	struct i2c_regs *i2c_regs_p;

	i2c_regs_p = chose_i2c(i2c_id);

	while (1) {
		if ((mmio_read_32((uintptr_t)&i2c_regs_p->ic_raw_intr_stat) & IC_STOP_DET)) {
			mmio_read_32((uintptr_t)&i2c_regs_p->ic_clr_stop_det);
			break;
		}
	}

	if (i2c_wait_for_bb(i2c_id)) {
		printf("Timed out waiting for bus\n");
		return 1;
	}

	/* flush_rxfifo */
	while (mmio_read_32((uintptr_t)&i2c_regs_p->ic_status) & IC_STATUS_RFNE)
		mmio_read_32((uintptr_t)&i2c_regs_p->ic_cmd_data);

	return 0;
}

static void check_dram_type(uint8_t ch, uint8_t *buffer, mcu_config_t *mcu_ptr)
{
	uint8_t temp_val;

	printf("Check Dram Type ...\n");

	//Key Byte /DRAM Device Type
	temp_val = buffer[2] & 0xf;
	mcu_ptr->ddr_spd_info[ch].dram_type = temp_val;

}

static void parse_spd_byte_ddr3(uint8_t ch, uint8_t *buffer, mcu_config_t *mcu_ptr)
{
	uint16_t temp_val;

	printf("Parse SPD Data ...\n");

	//Key Byte / Module Type
	temp_val = buffer[3] & 0xf;
	mcu_ptr->ddr_spd_info[ch].dimm_type = temp_val;

//	temp_val = buffer[4] & 0xf;
//	mcu_ptr->ddr_spd_info[ch].sdram_density = 256 * (1<<temp_val); //units: Mb

	mcu_ptr->ddr_spd_info[ch].bank_num = 8;

	temp_val = buffer[5] & 0x7;
	mcu_ptr->ddr_spd_info[ch].col_num = temp_val + 9;

	temp_val = buffer[5]>>3 & 0x7;
	mcu_ptr->ddr_spd_info[ch].row_num = temp_val + 12;

	//Module Organization
	temp_val = buffer[7] & 0x7;
	mcu_ptr->ddr_spd_info[ch].data_width = temp_val;

	temp_val = buffer[7]>>3 & 0x7;
	mcu_ptr->ddr_spd_info[ch].rank_num= temp_val + 1;

	//Module Organization
	temp_val = buffer[8]>>3 & 0x7;
	mcu_ptr->ddr_spd_info[ch].ecc_type = temp_val;

	//(RDIMM/LRDIMM): Address Mapping from Register to DRAM
	if(mcu_ptr->ddr_spd_info[ch].dimm_type == UDIMM_TYPE){
		temp_val = buffer[63] & 0x1;
		mcu_ptr->ddr_spd_info[ch].mirror_type = temp_val;
	}

	mcu_ptr->ddr_spd_info[ch].F0RC03 = (buffer[70]>>4) & MASK_4BIT;
	mcu_ptr->ddr_spd_info[ch].F0RC04 = buffer[71] & MASK_4BIT;
	mcu_ptr->ddr_spd_info[ch].F0RC05 = (buffer[71]>>4) & MASK_4BIT;

	mcu_ptr->ddr_spd_info[ch].module_manufacturer_id = ((uint16_t)buffer[117] << 8) + buffer[118];

/*	mcu_ptr->ddr_spd_info[ch].dram_manufacturer_id = ((uint16_t)buffer[147] << 8) + buffer[149];

	mcu_ptr->ddr_spd_info[ch].module_serial_number = ((uint32_t)buffer[122]<<24) + ((uint32_t)buffer[123]<<16)
												+ ((uint32_t)buffer[124]<<8) + buffer[125];
*/

	//tRCDmin
	mcu_ptr->ddr_spd_info[ch].tRCDmin = buffer[18] * SPD_MTB;
	//tRPmin
	mcu_ptr->ddr_spd_info[ch].tRPmin = buffer[20] * SPD_MTB;
	//tRASmin
	mcu_ptr->ddr_spd_info[ch].tRASmin = (((((uint16_t)buffer[21]) & 0xf)<<8) + buffer[23]) * SPD_MTB;
	//tRCmin
	mcu_ptr->ddr_spd_info[ch].tRCmin = (((((uint16_t)buffer[21]>>4) & 0xf)<<8) + buffer[23]) * SPD_MTB;
	//tFAWmin
	mcu_ptr->ddr_spd_info[ch].tFAWmin = ((((uint16_t)buffer[28] & 0xf) <<8) + buffer[29]) * SPD_MTB;
	//tRRDmin
	mcu_ptr->ddr_spd_info[ch].tRRDmin = buffer[19] * SPD_MTB;
	//tWRmin
	mcu_ptr->ddr_spd_info[ch].tWRmin = 15000;
	//tWTRmin, 7.5ns
	mcu_ptr->ddr_spd_info[ch].tWTRmin = 7500;

	print_ddr_info(ch, mcu_ptr);
	temp_val = ((uint16_t)buffer[147] << 8) + buffer[149];		//dram_manufacturer_id
	printf("/Dram:");
	switch(temp_val){
		case SAMSUNG_VENDOR:
			printf("Samsung"); break;
		case MICRON_VENDOR:
			printf("Micron"); break;
		case HYNIX_VENDOR:
			printf("Hynix"); break;
		case KINGSTON_VENDOR:
			printf("KingSton"); break;
		case RAMAXEL_VENDOR:
			printf("Ramaxel"); break;
		case LANQI_VENDOR:
			printf("Lanqi"); break;
		case CXMT_VENDOR:
			printf("CXMT"); break;
		case UNILC_VENDOR:
			printf("Unilc"); break;
		default:
			printf("Unknown=0x%x", temp_val);
	}

	temp_val = ((uint32_t)buffer[122]<<24) + ((uint32_t)buffer[123]<<16)
				+ ((uint32_t)buffer[124]<<8) + buffer[125];			//module_serial_number
	printf("/Serial:0x%x\n", temp_val);

}

static void parse_spd_byte(uint8_t ch, uint8_t *buffer, mcu_config_t *mcu_ptr)
{
	uint16_t temp_val;

	printf("Parse SPD Data ...\n");

	//Key Byte / Module Type
	temp_val = buffer[3] & 0xf;
	mcu_ptr->ddr_spd_info[ch].dimm_type = temp_val;

	//SDRAM Density and Banks
	temp_val = buffer[4]>>6 & 0x3;
	if(!temp_val)
		mcu_ptr->ddr_spd_info[ch].bg_num = 0x0;
	else
		mcu_ptr->ddr_spd_info[ch].bg_num = 0x1<<temp_val;


	temp_val = buffer[4]>>4 & 0x3;
	if(temp_val == 1)
		mcu_ptr->ddr_spd_info[ch].bank_num = 8;
	else
		mcu_ptr->ddr_spd_info[ch].bank_num = 4;

//	temp_val = buffer[4] & 0xf;
//	mcu_ptr->ddr_spd_info[ch].sdram_density = 256 * (1<<temp_val); //units: Mb	//可以算出来,不用读了

	//SDRAM Addressing
	temp_val = buffer[5] & 0x7;
	mcu_ptr->ddr_spd_info[ch].col_num = temp_val + 9;

	temp_val = buffer[5]>>3 & 0x7;
	mcu_ptr->ddr_spd_info[ch].row_num = temp_val + 12;

	//Module Organization
	temp_val = buffer[12] & 0x7;
	mcu_ptr->ddr_spd_info[ch].data_width = temp_val;

	temp_val = buffer[12]>>3 & 0x7;
	mcu_ptr->ddr_spd_info[ch].rank_num= temp_val + 1;

	//Module Organization
	temp_val = buffer[13]>>3 & 0x7;
	mcu_ptr->ddr_spd_info[ch].ecc_type = temp_val;

	//(Registered): Address Mapping from Register to DRAM
	if((mcu_ptr->ddr_spd_info[ch].dimm_type == RDIMM_TYPE)
		|| (mcu_ptr->ddr_spd_info[ch].dimm_type == LRDIMM_TYPE) )
		temp_val = buffer[136] & 0x1;
	else//(Unbuffered): Address Mapping from Edge Connector to DRAM
		temp_val = buffer[131] & 0x1;
	mcu_ptr->ddr_spd_info[ch].mirror_type = temp_val;
	mcu_ptr->ddr_spd_info[ch].F0RC03 = (buffer[137]>>4) & MASK_4BIT;
	mcu_ptr->ddr_spd_info[ch].F0RC04 = buffer[137] & MASK_4BIT;
	mcu_ptr->ddr_spd_info[ch].F0RC05 = buffer[138] & MASK_4BIT;

	mcu_ptr->ddr_spd_info[ch].vrefDQ_PR0 = buffer[140];
	mcu_ptr->ddr_spd_info[ch].vrefDQ_MDRAM = buffer[144];
	mcu_ptr->ddr_spd_info[ch].RTT_MDRAM_1866 = buffer[145];
	mcu_ptr->ddr_spd_info[ch].RTT_MDRAM_2400 = buffer[146];
	mcu_ptr->ddr_spd_info[ch].RTT_MDRAM_3200 = buffer[147];
	mcu_ptr->ddr_spd_info[ch].Drive_DRAM = buffer[148];
	mcu_ptr->ddr_spd_info[ch].ODT_DRAM_1866 = buffer[149];
	mcu_ptr->ddr_spd_info[ch].ODT_DRAM_2400 = buffer[150];
	mcu_ptr->ddr_spd_info[ch].ODT_DRAM_3200 = buffer[151];
	mcu_ptr->ddr_spd_info[ch].PARK_DRAM_1866 = buffer[152];
	mcu_ptr->ddr_spd_info[ch].PARK_DRAM_2400 = buffer[153];
	mcu_ptr->ddr_spd_info[ch].PARK_DRAM_3200 = buffer[154];

	mcu_ptr->ddr_spd_info[ch].rcd_num = (uint16_t)buffer[256];
	mcu_ptr->ddr_spd_info[ch].module_manufacturer_id = ((uint16_t)buffer[320] << 8) + buffer[321];

/*
	mcu_ptr->ddr_spd_info[ch].dram_manufacturer_id = ((uint16_t)buffer[350] << 8) + buffer[351];

	mcu_ptr->ddr_spd_info[ch].module_serial_number = ((uint32_t)buffer[325]<<24) + ((uint32_t)buffer[326]<<16)
												+ ((uint32_t)buffer[327]<<8) + buffer[328];
*/
	//tAAmin , buffer[123] may be positive/negative
	mcu_ptr->ddr_spd_info[ch].tAAmin = buffer[24] * SPD_MTB + (int8_t)buffer[123] * SPD_FTB;
	//tRCDmin , buffer[122] may be positive/negative
	mcu_ptr->ddr_spd_info[ch].tRCDmin = buffer[25] * SPD_MTB + (int8_t)buffer[122] * SPD_FTB;
	//tRPmin
	mcu_ptr->ddr_spd_info[ch].tRPmin = buffer[26] * SPD_MTB + (int8_t)buffer[121] * SPD_FTB;
	//tRASmin
	mcu_ptr->ddr_spd_info[ch].tRASmin = (((((uint16_t)buffer[27]) & 0xf)<<8) + buffer[28]) * SPD_MTB;
	//tRCmin
	mcu_ptr->ddr_spd_info[ch].tRCmin = (((((uint16_t)buffer[27]>>4) & 0xf)<<8) + buffer[29]) * SPD_MTB + (int8_t)buffer[120] * SPD_FTB;
	//tFAWmin
	mcu_ptr->ddr_spd_info[ch].tFAWmin = ((((uint16_t)buffer[36] & 0xf) <<8) + buffer[37]) * SPD_MTB;
	//tRDD_Smin
	mcu_ptr->ddr_spd_info[ch].tRRD_Smin = buffer[38] * SPD_MTB + (int8_t)buffer[119] * SPD_FTB;
	//tRDD_Lmin
	mcu_ptr->ddr_spd_info[ch].tRRD_Lmin = buffer[39] * SPD_MTB + (int8_t)buffer[118] * SPD_FTB;
	//tCCD_Lmin
	mcu_ptr->ddr_spd_info[ch].tCCD_Lmin = buffer[40] * SPD_MTB + (int8_t)buffer[117] * SPD_FTB;
	//tWRmin
	if((buffer[42] == 0x0) && (buffer[41] == 0x0) ){
		printf("Error! spd byte42 = 0\n");
		mcu_ptr->ddr_spd_info[ch].tWRmin = 15000;
	}else
		mcu_ptr->ddr_spd_info[ch].tWRmin = buffer[41] * SPD_MTB * 256 + buffer[42] * SPD_MTB;
	//tWTR_Smin, 2.5ns
	if( (buffer[43] == 0) && (buffer[44] == 0) ){
		printf("Error! spd byte43 = 0\n");
		mcu_ptr->ddr_spd_info[ch].tWTR_Smin = 2500;
	}else
		mcu_ptr->ddr_spd_info[ch].tWTR_Smin = ((((uint16_t)buffer[43] & 0xf) << 8) + buffer[44]) * SPD_MTB;
	//tWTR_Lmin, 7.5ns
	if( (buffer[43] == 0) && (buffer[45] == 0) )
		mcu_ptr->ddr_spd_info[ch].tWTR_Lmin = 7500;
	else
		mcu_ptr->ddr_spd_info[ch].tWTR_Lmin = ((((uint16_t)buffer[43] >> 4) & 0xf) + buffer[45]) * SPD_MTB;

	print_ddr_info(ch, mcu_ptr);
	temp_val = ((uint16_t)buffer[350] << 8) + buffer[351];	//dram_manufacturer_id
	printf("/Dram:");
	switch(temp_val){
		case SAMSUNG_VENDOR:
			printf("Samsung"); break;
		case MICRON_VENDOR:
			printf("Micron"); break;
		case HYNIX_VENDOR:
			printf("Hynix"); break;
		case KINGSTON_VENDOR:
			printf("KingSton"); break;
		case RAMAXEL_VENDOR:
			printf("Ramaxel"); break;
		case LANQI_VENDOR:
			printf("Lanqi"); break;
		case CXMT_VENDOR:
			printf("CXMT"); break;
		case UNILC_VENDOR:
			printf("Unilc"); break;
		default:
			printf("Unknown=0x%x", temp_val);
	}
	temp_val = ((uint32_t)buffer[325]<<24) + ((uint32_t)buffer[326]<<16)
				+ ((uint32_t)buffer[327]<<8) + buffer[328];				//module_serial_number
	printf("/Serial:0x%x\n", temp_val);

}

static int spd_i2c_read(uint8_t chip, int addr, uint8_t *val, int i2c_id, int page_num)
{
	struct i2c_regs *i2c_regs_p;
	unsigned char *buffer = (unsigned char *)val;
	uint32_t i;
	uint32_t cnt = 0;

	if(page_num == 1)
		buffer = val + 256;

	if (i2c_xfer_init(chip, addr, i2c_id))
		return 1;

	i2c_regs_p = chose_i2c(i2c_id);


	for(i = 0; i < SPD_NUM; i++ )
	{
		cnt = 0;
		mmio_write_32((uintptr_t)&i2c_regs_p->ic_cmd_data , IC_CMD | IC_STOP);
		while (!(mmio_read_32((uintptr_t)&i2c_regs_p->ic_status) & IC_STATUS_RFNE)) {
			cnt++;
			if(cnt >= 5000)
				break;
		}
		if(cnt >= 5000){
			if(mmio_read_32((uintptr_t)&i2c_regs_p->ic_raw_intr_stat) & IC_TX_ABRT)
				mmio_read_32((uintptr_t)&i2c_regs_p->ic_clr_tx_abrt);
			return 1;
		}

		*(buffer + i) = (unsigned char)mmio_read_32((uintptr_t)&i2c_regs_p->ic_cmd_data);
		if(i == 0){
			if(((*val & 0x70) == 0x10) || ((*val & 0x70) == 0x20)){
			}else{
				printf("spd BYTE0 = 0x%x\n", *buffer);
				return 1;
			}
		}
	}

	return i2c_xfer_finish(i2c_id);
}

static void spd_setpage(int i2c_id, int page_num)
{
	struct i2c_regs *i2c_regs_p;

	i2c_regs_p = chose_i2c(i2c_id);

	if(page_num == 0x1)
		mmio_write_32((uintptr_t)&i2c_regs_p->ic_tar , 0x37);
	else
		mmio_write_32((uintptr_t)&i2c_regs_p->ic_tar , 0x36);

	mmio_write_32((uintptr_t)&i2c_regs_p->ic_cmd_data , 0x0);
	mmio_write_32((uintptr_t)&i2c_regs_p->ic_cmd_data , 0x0|IC_STOP);

	/*******************For NoAck Response************************/

	while(mmio_read_32((uintptr_t)&i2c_regs_p->ic_status) != 0x6);

	if(mmio_read_32((uintptr_t)&i2c_regs_p->ic_intr_stat) & 0x40){
		volatile int i = 0;
		for(i = 0; i < 100000; i++);
		mmio_read_32((uintptr_t)&i2c_regs_p->ic_intr_stat);
		mmio_read_32((uintptr_t)&i2c_regs_p->ic_status);
		mmio_read_32((uintptr_t)&i2c_regs_p->ic_clr_tx_abrt);
		mmio_read_32((uintptr_t)&i2c_regs_p->ic_intr_stat);
		mmio_read_32((uintptr_t)&i2c_regs_p->ic_status);
	}
	/**************************************************************/

}
int dimm_probe(uint8_t ch, mcu_config_t *mcu_ptr)
{
	uint8_t temp[SPD_NUM*2] = {0};
	int chip_id[2] = {LMU0_I2C_ID, LMU1_I2C_ID};
	uint8_t slave_addr[2] = {LMU0_I2C_SLAVE_ADDR, LMU1_I2C_SLAVE_ADDR};

	//init I2C0
	spd_i2c_init();

	printf("Read Spd Begin...\n");
	/* read page 0 data*/
	if(spd_i2c_read(slave_addr[ch], 0, temp, chip_id[ch], 0))
		return 1;
    printf("aa");
	check_dram_type(ch, temp, mcu_ptr);

	if(mcu_ptr->ddr_spd_info[ch].dram_type == DDR3_TYPE){
		parse_spd_byte_ddr3(ch, temp, mcu_ptr);
	}else{
		/*
		 * according to <JEDEC Standard No.21-C>, divied to 2 page
		 */
		spd_setpage(chip_id[ch], 1); /* set to page 1 */

		/* read page 1 data*/
		spd_i2c_read(slave_addr[ch], 0, temp, chip_id[ch], 1);

		parse_spd_byte(ch, temp, mcu_ptr);

		/* must rechanged to page 0 */
		spd_setpage(chip_id[ch], 0); /* set to page 0 */

	}

	return 0;
}

static uint8_t init_dimm_param(uint8_t ch, mcu_config_t *pm)
{
#ifdef MCU_USE_MANUAL
	//1. 代码手动配置参数
	printf("manual config dimm info...\n");
	pm->ddr_spd_info[ch].dimm_type = RDIMM_TYPE;
    pm->ddr_spd_info[ch].data_width = DIMM_x8;
    pm->ddr_spd_info[ch].mirror_type = MIRROR;
    pm->ddr_spd_info[ch].ecc_type = ECC_TYPE;
    pm->ddr_spd_info[ch].dram_type = DDR4_TYPE;
    pm->ddr_spd_info[ch].rank_num = 2;
    pm->ddr_spd_info[ch].row_num = 16;
    pm->ddr_spd_info[ch].col_num = 10;
    pm->ddr_spd_info[ch].bg_num = 4;
    pm->ddr_spd_info[ch].bank_num = 4;
    pm->ddr_spd_info[ch].tRCDmin = 13750;
    pm->ddr_spd_info[ch].tRPmin = 13750;
    pm->ddr_spd_info[ch].tRASmin = 32000;
    pm->ddr_spd_info[ch].tRCmin = 45750;
    pm->ddr_spd_info[ch].tFAWmin = 21000;
    pm->ddr_spd_info[ch].tRRD_Smin = 3300;
    pm->ddr_spd_info[ch].tRRD_Lmin = 4900;
    pm->ddr_spd_info[ch].tCCD_Lmin = 5000;
    pm->ddr_spd_info[ch].tWRmin = 15000;
    pm->ddr_spd_info[ch].tWTR_Smin = 2500;
    pm->ddr_spd_info[ch].tWTR_Lmin = 7500;
#else
#ifdef MCU_USE_PARAMETER
	//2. 读取参数表
#else
	//3. i2c读取spd
	printf("read spd...\n");
	if(dimm_probe(ch, pm))
		return 1;
#endif
#endif
	return 0;
}

void get_mcu_param_info(mcu_config_t *mcu_info)
{
	get_mcu_up_info_default(mcu_info);

#if 1
	uint8_t ch;
	for(ch=0; ch<2; ch++){
		if(!((mcu_info->ch_enable>>ch)&0x1))
			continue;
		printf("  CH %d : \n", ch);
		if(init_dimm_param(ch, mcu_info)){
			printf("DIMM %d Don't Probed!\n", ch);
			mcu_info->ch_enable &= ~(0x1<<ch);
		}else{
			if(!((mcu_info->ecc_enable>>ch) & 0x1))
				printf("set ecc disable!\n");
			if(!((mcu_info->dm_enable>>ch) & 0x1))
				printf("set dm disable!\n");
		}
	}
#endif
#if 0
	printf("MCU INFO: \n");
	printf("  magic = 0x%x\n", mcu_info->magic);
	printf("  version = 0x%x\n", mcu_info->version);
	printf("  size = 0x%x\n", mcu_info->size);
	printf("  ch_enable = 0x%x\n", mcu_info->ch_enable);
	printf("  ecc_enable = 0x%x\n", mcu_info->dm_enable);
	printf("  force_spd_enable = 0x%x\n", mcu_info->force_spd_enable);
	printf("  mcu_misc_enable = 0x%x\n", mcu_info->mcu_misc_enable);
	printf("  train_debug = 0x%x\n", mcu_info->train_debug);
#endif

}
