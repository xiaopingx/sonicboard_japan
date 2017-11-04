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
static u8 FMC_EraseSection(u32 page)//��������
{
//     SYS_UnlockReg();//������Ҳ���ܷ�������
//     FMC->ISPCON|=1<<0;   
    FMC->ISPADR=page;//д��Ҫ�����ĵ�ַ
    FMC->ISPCMD=0X22;//д�����ָ��
    FMC->ISPTRG|=1<<0;//����ISP����
    while((FMC->ISPTRG&(1<<0)));//�ȴ��������  
    if(FMC->ISPCON&(1<<6))//����Ƿ���ִ���
    {
        FMC->ISPCON|=1<<6;//�������
        return 1;
    }
    FMC->ISPCON&=~(1<<0);//�ر�ISP
    SYS_LockReg();
    return 0;
}
static u32 FMC_ReadWord(u8 offset,u32 page)
{
    SYS_UnlockReg();            //������һ���ô��ڣ���Ȼ�Ļ����ܶ���
    FMC->ISPCON|=1<<0;          //ʹ��ISP    
    FMC->ISPADR=(offset+page);  //д���ַ
    FMC->ISPCMD=0X00;           //д��ָ��       
    FMC->ISPTRG|=1<<0;          //��ʼISP����
    while(FMC->ISPTRG&(1<<0));  //�ȴ��������
//     FMC->ISPCON&=~(1<<0);//�����䲻�ܷ��ڴ˴���
//     SYS_LockReg();
    return FMC->ISPDAT;         //��������
}
static void FMC_WriteWord(u32 addr,u32 dat)
{
    SYS_UnlockReg();        //������Ҳ�ô���
    FMC->ISPCON|=1<<0;//ʹ��ISP 
    FMC->ISPADR=addr;
    FMC->ISPCMD=0X21;
    FMC->ISPDAT=dat;
    FMC->ISPTRG|=1<<0;
    while(FMC->ISPTRG&(1<<0));
    FMC->ISPCON&=~(1<<0);//�ر�ISP
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
// static u8 FMC_ReadConfig(u32 *config,u32 count)//����ֵ1����ʧ�ܣ�����ֵ0�ɹ�
// {
//     config[0]=FMC_ReadWord(DF_CONFIG0);
//     if(count<2)
//         return 1;
//     config[1]=FMC_ReadWord(DF_CONFIG1);
//         return 0;
// }
static void FMC_Dataflash_Save(u16 offset,u32 dat,u32 page)//��������dataflashΪ512kb,��Ϊһ��������ַ��0X00000E00~0X00000FFF
{
    u16 i = 0;
    for(i=0;i<128;i++)
        DATA_TEMP->temp[i]=FMC_ReadWord(i*4,page);    //�Ȱ�ԭ�������ݶ������ݴ�   
    FMC_EraseSection(page); 
    DATA_TEMP->temp[offset/4]=dat;               //����Ҫ�ı��ֵ������Ӧ���ݴ����б���
    for(i=0;i<128;i++)
        FMC_WriteWord(page+4*i,DATA_TEMP->temp[i]);   //�Ѹı���ֵ��д������
}
static void FMC_init(u32 dfba_startaddr)//
{
    u32 i;
    CLK->AHBCLK|=1<<2;//����ISPʱ��
    SYS_UnlockReg();//�⿪д����
//     FMC->ISPCON|=1<<0;//��ISP
    FMC->ISPCON&=~(1<<1);//��APROM����
    FMC->ISPCON|=1<<4;//����ͨ��ISP����CONFIG BITS   
    for(i=0;i<(DATAFLASH_16K*1024);i+=512)//���APROM,��Ϊ���޸�CONFIG BITS֮ǰ����Ҫ�޸�
        FMC_EraseSection(0X00000000+i);
    FMC_EraseSection(DF_CONFIG0);//���CONFIG���ڵ�����
    FMC_WriteWord(DF_CONFIG0,0XA00000C2);//����CONFIG0
    FMC_WriteWord(DF_CONFIG1,dfba_startaddr);//����CONFIG1    
    SYS_LockReg();
}



