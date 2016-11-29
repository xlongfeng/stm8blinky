#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "stm8s.h"
#include "stm8s_it.h"
#include "jiffies.h"

#define CONFIG_CLK_HSI_RC       (16000000)
#define CONFIG_CLK_HSI          (CONFIG_CLK_HSI_RC / 8)
#define CONFIG_CLK_MASTER       (CONFIG_CLK_HSI)
#define CONFIG_HZ               (HZ)

void tim2_update_interrupt(void)
{
    if (TIM2_GetITStatus(TIM2_IT_UPDATE) != RESET) {
        TIM2_ClearITPendingBit(TIM2_IT_UPDATE);
        tick();
    }
}

void uart_init(void)
{
    UART1_DeInit();
    UART1_Init(115200, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO, UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TX_ENABLE);
}

void gpio_init(void)
{
    GPIO_Init(GPIOB, (GPIO_Pin_TypeDef)GPIO_PIN_5, GPIO_MODE_OUT_OD_LOW_SLOW);
}

void tim_init(void)
{
    TIM2_DeInit();
    TIM2_TimeBaseInit(TIM2_PRESCALER_2, CONFIG_HZ);
    TIM2_ITConfig(TIM2_IT_UPDATE, ENABLE);
    TIM2_Cmd(ENABLE);
}

int main()
{
    uint32_t curr_time = jiffies;

    uart_init();
    gpio_init();
    tim_init();

    printf("\n\n");
    printf("----------------------------\n");
    printf("| STM8 Blinky Demo Program |\n");
    printf("----------------------------\n");

    enableInterrupts();

    for (;;) {
        if (time_after_eq(jiffies, curr_time + msecs_to_jiffies(500))) {
            curr_time = jiffies;
            GPIO_WriteReverse(GPIOB, (GPIO_Pin_TypeDef)GPIO_PIN_5);
        }
    }
}

void putchar(char c)
{
    if (c == '\n')
        putchar('\r');

    UART1_SendData8(c);

    while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
    printf("Assert failed @ file:%s, line %d\n", file, line);

    /* Infinite loop */
    while (1) {
    }
}
#endif
