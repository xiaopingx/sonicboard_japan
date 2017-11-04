#ifndef _PRO_H
#define _PRO_H
#include "gpio.h"
#include "sys.h"
#define KEY1_1 P12
#define KEY1_2 P13
#define KEY1_1_OUTMODE()    {P1->PMD&=~(3<<4);P1->PMD|=1<<4;}
#define KEY1_1_INMODE()     {P1->PMD&=~(3<<4);}
#define KEY1_2_OUTMODE()    {P1->PMD&=~(3<<6);P1->PMD|=1<<6;}
#define KEY1_2_INMODE()     {P1->PMD&=~(3<<6);}

#define LIGHT_SPITE         (1<<0)
#define LIGHT_XHD           (1<<1)
#define LIGHT_A             (1<<2)
#define LIGHT_B             (1<<3)
#define LIGHT_C             (1<<4)
#define LIGHT_CUP           (1<<8)
#define LIGHT_WATER         (1<<9)
#define LIGHT_HEAT          (1<<10)
#define LIGHT_S0            (1<<11)
#define LIGHT_SET           (1<<12)
#define LIGHT_S1            (1<<16)
#define LIGHT_S2            (1<<17)
#define LIGHT_S3            (1<<18)
#define LIGHT_WATCH         (1<<19)
#define LIGHT_LIGHT         (1<<20)

/***********************************************/
#define KEYROW1         P14
#define KEYROW2         P15
#define KEYROW3         P32
#define KEYROW4         P34

#define LIGHTROW1       P24
#define LIGHTROW2       P35
#define LIGHTROW3       P51

#define SCANCOL1        P04
#define SCANCOL2        P05
#define SCANCOL3        P06
#define SCANCOL4        P07
#define SCANCOL5        P25
/***********************************************/

//JZ:键值有效值
//JZ0:键值当前值
//JZ1:键值前一次值
typedef struct
{
    u32 JZ0;
	u32 JZ1;
    u32 JZ;
    u32 DZ_temp;
    u32 DZ;
    u32 stat_flag;
	u8 flag;
    u8 notsame_flag;   
}TypeDef_PRO;

extern TypeDef_PRO PRO_dev;
u32  injz(u16 jpn);
void outdz(u16 jpn);
void key_init(void);
u8 main_to_slave(void);
u8 slave_to_main(void);
void reciv_deal(void);
#endif

