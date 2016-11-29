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

void iwdog_init(void)
{
    IWDG_Enable();

    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    IWDG_SetPrescaler(IWDG_Prescaler_128);

    /* Set counter reload value to obtain 250ms IWDG Timeout.
     * Counter Reload Value = 250ms / IWDG counter clock period
     *                      = 250ms / (LsiFreq / 128)
     *                      = LsiFreq / (128 * 4)
     *                      = LsiFreq / 512
     */
    IWDG_SetReload((uint8_t)(128000 / 512));

    IWDG_ReloadCounter();
}

void watchdog()
{
    if (jiffies_to_msecs(jiffies) < (7UL * 24 * 60 * 60 * 1000))
        IWDG_ReloadCounter();
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

void reset_status()
{
    bool por = TRUE;

    printf("Reset source:");

    if (RST_GetFlagStatus(RST_FLAG_WWDGF) != RESET) {
        printf(" <Window watchdog> ");
        RST_ClearFlag(RST_FLAG_WWDGF);
        por = FALSE;
    }

    if (RST_GetFlagStatus(RST_FLAG_IWDGF) != RESET) {
        printf(" <independent watchdog> ");
        RST_ClearFlag(RST_FLAG_IWDGF);
        por = FALSE;
    }

    if (RST_GetFlagStatus(RST_FLAG_ILLOPF) != RESET) {
        printf(" <Illigal opcode> ");
        RST_ClearFlag(RST_FLAG_ILLOPF);
        por = FALSE;
    }

    if (RST_GetFlagStatus(RST_FLAG_SWIMF) != RESET) {
        printf(" <SWIM reset> ");
        RST_ClearFlag(RST_FLAG_SWIMF);
        por = FALSE;
    }

    if (RST_GetFlagStatus(RST_FLAG_EMCF) != RESET) {
        printf(" <EMC reset> ");
        RST_ClearFlag(RST_FLAG_EMCF);
        por = FALSE;
    }

    if (por) {
        printf(" <Powerup reset> ");
    }

    printf("\n");
}

void display_banner(void)
{
    printf("\n\n");
    printf("----------------------------\n");
    printf("| STM8 Blinky Demo Program |\n");
    printf("----------------------------\n");

    reset_status();
}

int main()
{
    uint32_t curr_time = jiffies;

    iwdog_init();
    uart_init();
    gpio_init();
    tim_init();

    display_banner();

    enableInterrupts();

    for (;;) {
        if (time_after_eq(jiffies, curr_time + msecs_to_jiffies(500))) {
            curr_time = jiffies;
            GPIO_WriteReverse(GPIOB, (GPIO_Pin_TypeDef)GPIO_PIN_5);
        }
        watchdog();
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
