#ifndef _MINI51_EEPROM_H
#define _MINI51_EEPROM_H
#include "sys.h"
/*******************DATAFLASH配置**********************/
//#define DATAFLASH_4K    4
//#define DATAFLASH_8K    8//默认为8KB的内存
#define DATAFLASH_16K   16//默认为16KB的内存
#define DF_CONFIG0      0X00300000
#define DF_CONFIG1      0X00300004
#ifdef DATAFLASH_4K
#define DF_STARTADDR    0X00000C00
#define DF_PAGE1        0X00000C00
#define DF_PAGE2        0X00000DFF
#define DF_ENDADDR      0X00000FFF
#endif
#ifdef DATAFLASH_8K
#define DF_STARTADDR    0X00001C00
#define DF_PAGE1        0X00001C00
#define DF_PAGE2        0X00001DFF
#define DF_ENDADDR      0X00001FFF
#endif
#ifdef DATAFLASH_16K
#define DF_STARTADDR    0X00003C00
#define DF_PAGE1        0X00003C00
#define DF_PAGE2        0X00003DFF
#define DF_ENDADDR      0X00003FFF
#endif
/******************************************************/
typedef struct
{
    u8 (*erase)(u32);
    u32 (*read)(u8,u32);
    void (*write)(u32,u32);
    void (*save)(u16,u32,u32);
    void (*init)(u32);
}Typedef_DATAFLASH;
extern Typedef_DATAFLASH DATAFLASH;
typedef struct
{
    u32 temp[128];
} DATA_TEMP;	    
#define DATA_TEMP_BASE        ((u32)(0X20000600))
#define DATA_TEMP             ((DATA_TEMP *)DATA_TEMP_BASE)









#endif






