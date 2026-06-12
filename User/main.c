#include "main.h"

uint32_t g_delay_tick = 0;

int main(void)
{
    LED_Init();
    KEY_Init();
    TIM6_Init();
    usart1_init();
    ADC_Logger_Init();

    LoggerApp_Start();

    while (1)
    {
        
    }
}
