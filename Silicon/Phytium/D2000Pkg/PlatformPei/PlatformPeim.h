#ifndef _PLATFORM_PEIM_H
#define _PLATFORM_PEIM_H

#include "mcu_info.h"
#if 1
typedef struct cpu_config {
    uint32_t magic;
    uint32_t version;
    uint32_t size;
    uint8_t rev1[4];
    uint32_t core_pll;
    uint32_t res1;
    uint32_t lmu_pll;
    uint32_t res2;
    uint32_t res3;
    uint32_t res4;
    uint32_t res5;
}__attribute__((aligned(sizeof(unsigned long)))) cpu_config_t;


#define PARAMETER_CPU_MAGIC                 0x54460010
cpu_config_t const cpu_base_info = {
    .magic = PARAMETER_CPU_MAGIC,
    .version = 0x1,
    .size = 0x30,
    .core_pll = 2600,
    .lmu_pll = 667,
};
#endif
#define PARAMETER_COMMON_MAGIC                  0x54460013

typedef struct common_config {
    uint32_t magic;
    uint32_t version;
    uint32_t size;
    uint8_t rev1[4];
    uint64_t core_bit_map;
}__attribute__((aligned(sizeof(unsigned long)))) common_config_t;

common_config_t const common_base_info = {
    .magic = PARAMETER_COMMON_MAGIC,
    .version = 0x1,
    .core_bit_map = 0x3333,
};

/******************************************************************/
#define PARAMETER_PCIE_MAGIC                0x54460011

/**********PEU*************/
#define CONFIG_PCI_PEU0         0x1
#define CONFIG_PCI_PEU1         0x1  /* 0x0 : disable peu1,  x01 : enable peu1 configuartions */
/*0:单根    1:多根*/
#define CONFIG_INDEPENDENT_TREE 0x0

/* splie mode of peu, both PEU0 and PEU1 should be the same */
#define X16                     0x0
#define X8X8                    0x1
#define CONFIG_PEU0_SPLIT_MODE  X8X8
#define CONFIG_PEU1_SPLIT_MODE  X8X8

/* peu device mode: 0 ep, 1 rc default */
#define EP_MODE                 0x0
#define RC_MODE                 0x1

#define PEU1_OFFSET 16

#define PEU_C_OFFSET_MODE   16
#define PEU_C_OFFSET_SPEED  0

#define GEN3    3
#define GEN2    2
#define GEN1    1

typedef struct pcu_ctr{
    uint32_t base_config[3];
    uint32_t equalization[3];
    uint8_t rev[80];
}__attribute__((aligned(sizeof(unsigned long)))) peu_ctr_t;

typedef struct pcu_config {
    uint32_t magic;
    uint32_t version;
    uint32_t size;
    uint8_t rev1[4];
    uint32_t independent_tree;
    uint32_t base_cfg;
    uint8_t rev2[16];
    peu_ctr_t ctr_cfg[2];
}__attribute__((aligned(sizeof(unsigned long)))) peu_config_t;

peu_config_t const peu_base_info  = {
    .magic = PARAMETER_PCIE_MAGIC,
    .version = 0x1,
    .size = 0x100,
    .independent_tree = CONFIG_INDEPENDENT_TREE,
    .base_cfg = ((CONFIG_PCI_PEU1 | (CONFIG_PEU1_SPLIT_MODE << 1)) << PEU1_OFFSET | \
                (CONFIG_PCI_PEU0 | (CONFIG_PEU0_SPLIT_MODE << 1))),
    .ctr_cfg[0].base_config[0] = (RC_MODE << PEU_C_OFFSET_MODE) | (GEN3 << PEU_C_OFFSET_SPEED),
    .ctr_cfg[0].base_config[1] = (RC_MODE << PEU_C_OFFSET_MODE) | (GEN3 << PEU_C_OFFSET_SPEED),
    .ctr_cfg[0].base_config[2] = (RC_MODE << PEU_C_OFFSET_MODE) | (GEN3 << PEU_C_OFFSET_SPEED),
    .ctr_cfg[1].base_config[0] = (RC_MODE << PEU_C_OFFSET_MODE) | (GEN3 << PEU_C_OFFSET_SPEED),
    .ctr_cfg[1].base_config[1] = (RC_MODE << PEU_C_OFFSET_MODE) | (GEN3 << PEU_C_OFFSET_SPEED),
    .ctr_cfg[1].base_config[2] = (RC_MODE << PEU_C_OFFSET_MODE) | (GEN3 << PEU_C_OFFSET_SPEED),
    .ctr_cfg[0].equalization[0] = 0x48484848,
    .ctr_cfg[0].equalization[1] = 0x48484848,
    .ctr_cfg[0].equalization[2] = 0x48484848,
    .ctr_cfg[1].equalization[0] = 0x48484848,
    .ctr_cfg[1].equalization[1] = 0x48484848,
    .ctr_cfg[1].equalization[2] = 0x48484848,
};

#endif
