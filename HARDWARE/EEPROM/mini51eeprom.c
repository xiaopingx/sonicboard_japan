#include "sys.h"
#include "mini51eeprom.h"
static void FMC_init(u32 dfba_startaddr);
static u8 FMC_EraseSection(u32 page);
static u32 FMC_ReadWord(u8 offset,u32 page);
static void FMC_WriteWord(u32 addr,u32 dat);
static void FMC_Dataflash_Save(u16 offset,u32 dat,u32 page);
Typedef_DATAFLASH DATAFLASH={
    FMC_EraseSection,
    FMC_ReadWord,
    FMC_WriteWord,
    FMC_Dataflash_Save,
    FMC_init,
};
static u8 FMC_EraseSection(u32 page)//擦除扇区
{
//     SYS_UnlockReg();//这两句也不能放在这里
//     FMC->ISPCON|=1<<0;   
    FMC->ISPADR=page;//写入要擦除的地址
    FMC->ISPCMD=0X22;//写入擦除指令
    FMC->ISPTRG|=1<<0;//启动ISP操作
    while((FMC->ISPTRG&(1<<0)));//等待操作完成  
    if(FMC->ISPCON&(1<<6))//检测是否出现错误
    {
        FMC->ISPCON|=1<<6;//清除错误
        return 1;
    }
    FMC->ISPCON&=~(1<<0);//关闭ISP
    SYS_LockReg();
    return 0;
}
static u32 FMC_ReadWord(u8 offset,u32 page)
{
    SYS_UnlockReg();            //这两句一定得存在，不然的画不能读出
    FMC->ISPCON|=1<<0;          //使能ISP    
    FMC->ISPADR=(offset+page);  //写入地址
    FMC->ISPCMD=0X00;           //写入指令       
    FMC->ISPTRG|=1<<0;          //开始ISP操作
    while(FMC->ISPTRG&(1<<0));  //等待操作完成
//     FMC->ISPCON&=~(1<<0);//这两句不能放在此处，
//     SYS_LockReg();
    return FMC->ISPDAT;         //返回数据
}
static void FMC_WriteWord(u32 addr,u32 dat)
{
    SYS_UnlockReg();        //这两句也得存在
    FMC->ISPCON|=1<<0;//使能ISP 
    FMC->ISPADR=addr;
    FMC->ISPCMD=0X21;
    FMC->ISPDAT=dat;
    FMC->ISPTRG|=1<<0;
    while(FMC->ISPTRG&(1<<0));
    FMC->ISPCON&=~(1<<0);//关闭ISP
    SYS_LockReg();
}
// static u8 FMC_WriteConfig(u32 *config,u32 count)
// {
// //     FMC_EnableConfigUpdate();
// //     FMC_EraseSection(0X00300000);
// //     USER_CONFIG0=0XA00000C2;
// //     USER_CONFIG1=0X0001EA00;
//     
//     FMC_WriteWord(DF_CONFIG0,config[0]);
//     FMC_WriteWord(DF_CONFIG1,config[1]);
// //     FMC_DisableConfigUpdate();
//     return 0;
// }
// static u8 FMC_ReadConfig(u32 *config,u32 count)//返回值1操作失败，返回值0成功
// {
//     config[0]=FMC_ReadWord(DF_CONFIG0);
//     if(count<2)
//         return 1;
//     config[1]=FMC_ReadWord(DF_CONFIG1);
//         return 0;
// }
static void FMC_Dataflash_Save(u16 offset,u32 dat,u32 page)//这里设置dataflash为512kb,即为一个扇区地址从0X00000E00~0X00000FFF
{
    u16 i = 0;
    for(i=0;i<128;i++)
        DATA_TEMP->temp[i]=FMC_ReadWord(i*4,page);    //先把原来的数据读出来暂存   
    FMC_EraseSection(page); 
    DATA_TEMP->temp[offset/4]=dat;               //把所要改变的值放入相应的暂存器中保存
    for(i=0;i<128;i++)
        FMC_WriteWord(page+4*i,DATA_TEMP->temp[i]);   //把改变后的值再写回扇区
}
static void FMC_init(u32 dfba_startaddr)//
{
    u32 i;
    CLK->AHBCLK|=1<<2;//开启ISP时钟
    SYS_UnlockReg();//解开写保护
//     FMC->ISPCON|=1<<0;//打开ISP
    FMC->ISPCON&=~(1<<1);//从APROM启动
    FMC->ISPCON|=1<<4;//可以通过ISP更新CONFIG BITS   
    for(i=0;i<(DATAFLASH_16K*1024);i+=512)//清除APROM,因为在修改CONFIG BITS之前必须要修改
        FMC_EraseSection(0X00000000+i);
    FMC_EraseSection(DF_CONFIG0);//清除CONFIG所在的扇区
    FMC_WriteWord(DF_CONFIG0,0XA00000C2);//设置CONFIG0
    FMC_WriteWord(DF_CONFIG1,dfba_startaddr);//设置CONFIG1    
    SYS_LockReg();
}



