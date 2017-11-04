#include "sys.h"
#include "clk.h"
#include "delay.h"
#include "hardware.h"
#include "mini51eeprom.h"
#include "watchdog.h"
#include "timer.h"
int main()
{     

	/*******************************************************/
    u32 time = 0;
    u32 ledtime = 0;
    MINI51_Clock_Init();
    delay_init(22);
    IOCUP_Init();
    DATAFLASH_init(DF_STARTADDR);
    delay_ms(1000);
    WATCHDOG_init(WDT_2POW18,IN_10K);
    while(1)
    { 
        /******************************/
        SONIC_Signal_detect(&SONIC);
        SONIC_cmd_parse(&SONIC);
        SONIC_overall_control();
        SONIC_send_cmd(&SONIC);
        Sonic_cupset();
        /******************************/
        if(time <= 3){
            if(FLAG.sendplus == 1)
                TRIG = 1;
        } 
        else TRIG = 0;
        if(time == 4){
            if(FLAG.sendplus == 1){
                Sonic_cappre_finish_clear();            
            }
        }
        if(time > 40 && time < 600){
            if(FLAG.sendplus == 1){
                Sonic_cappre_filter();
            }
        }    
        if(time == 600){
             if(FLAG.sendplus == 1){
                Sonic_timeout();
             }  
        }
        time ++;
        if(time == 1400){
            time = 0;
            WATCHDOG_feeddog();
            TIMEPARAM.pre.filtered = 0;
        }
        /******************************/
        ledtime ++;
        if(ledtime > 10000){
            LED = ~LED;
            ledtime = 0;
        }
        /******************************/
        delay_us(10);        
    }//while½áÊø   
}//main()½áÊø
