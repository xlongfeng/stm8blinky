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

void watchdog();

void tim2_update_interrupt(void)
{
    if (TIM2_GetITStatus(TIM2_IT_UPDATE) != RESET) {
        TIM2_ClearITPendingBit(TIM2_IT_UPDATE);
        tick();
    }
}

void mdelay(uint32_t ms)
{
    uint32_t delay = jiffies + msecs_to_jiffies(ms);

    while (time_before(jiffies, delay))
        watchdog();
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

void keyboard_init(void )
{
    GPIO_Init(GPIOA, (GPIO_Pin_TypeDef)GPIO_PIN_2, GPIO_MODE_IN_PU_NO_IT);
}

int keyboard_press(void)
{
    uint32_t debouncing = jiffies + msecs_to_jiffies(1);
    int pressed = -1;

    if (GPIO_ReadInputPin(GPIOA, (GPIO_Pin_TypeDef)GPIO_PIN_2) == RESET) {
        do {
            if (GPIO_ReadInputPin(GPIOA, (GPIO_Pin_TypeDef)GPIO_PIN_2) != RESET)
                return -1;
            watchdog();
        } while (time_before(jiffies, debouncing));

        /* wait for button release */
        while (GPIO_ReadInputPin(GPIOA, (GPIO_Pin_TypeDef)GPIO_PIN_2) == RESET)
            watchdog();

        pressed = 0;
    }

    return pressed;
}

void pulse_output_init(void)
{
    GPIO_Init(GPIOA, (GPIO_Pin_TypeDef)GPIO_PIN_1, GPIO_MODE_OUT_PP_LOW_SLOW);
}

void pulse_output(uint8_t value)
{
    GPIO_WriteHigh(GPIOA, (GPIO_Pin_TypeDef)GPIO_PIN_1);
    mdelay(value * 5);
    GPIO_WriteLow(GPIOA, (GPIO_Pin_TypeDef)GPIO_PIN_1);
}

void sensor_input_disable(void)
{
    GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_3, GPIO_MODE_IN_FL_NO_IT);
    GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_2, GPIO_MODE_IN_FL_NO_IT);
    GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_1, GPIO_MODE_IN_FL_NO_IT);
    GPIO_Init(GPIOC, (GPIO_Pin_TypeDef)GPIO_PIN_7, GPIO_MODE_IN_FL_NO_IT);
    GPIO_Init(GPIOC, (GPIO_Pin_TypeDef)GPIO_PIN_6, GPIO_MODE_IN_FL_NO_IT);
    GPIO_Init(GPIOC, (GPIO_Pin_TypeDef)GPIO_PIN_5, GPIO_MODE_IN_FL_NO_IT);
    GPIO_Init(GPIOC, (GPIO_Pin_TypeDef)GPIO_PIN_4, GPIO_MODE_IN_FL_NO_IT);
    GPIO_Init(GPIOC, (GPIO_Pin_TypeDef)GPIO_PIN_3, GPIO_MODE_IN_FL_NO_IT);

    GPIO_Init(GPIOB, (GPIO_Pin_TypeDef)GPIO_PIN_4, GPIO_MODE_IN_FL_NO_IT);
}

void sensor_input_enable(void)
{
    GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_3, GPIO_MODE_IN_PU_NO_IT);
    GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_2, GPIO_MODE_IN_PU_NO_IT);
    GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_1, GPIO_MODE_IN_PU_NO_IT);
    GPIO_Init(GPIOC, (GPIO_Pin_TypeDef)GPIO_PIN_7, GPIO_MODE_IN_PU_NO_IT);
    GPIO_Init(GPIOC, (GPIO_Pin_TypeDef)GPIO_PIN_6, GPIO_MODE_IN_PU_NO_IT);
    GPIO_Init(GPIOC, (GPIO_Pin_TypeDef)GPIO_PIN_5, GPIO_MODE_IN_PU_NO_IT);
    GPIO_Init(GPIOC, (GPIO_Pin_TypeDef)GPIO_PIN_4, GPIO_MODE_IN_PU_NO_IT);
    GPIO_Init(GPIOC, (GPIO_Pin_TypeDef)GPIO_PIN_3, GPIO_MODE_IN_PU_NO_IT);

    GPIO_Init(GPIOB, (GPIO_Pin_TypeDef)GPIO_PIN_4, GPIO_MODE_OUT_OD_LOW_SLOW);
}

uint8_t sensor_input(void)
{
    uint8_t value = 1;

    sensor_input_enable();

    if (GPIO_ReadInputPin(GPIOC, (GPIO_Pin_TypeDef)GPIO_PIN_3) == RESET)
        value = 2;

    if (GPIO_ReadInputPin(GPIOC, (GPIO_Pin_TypeDef)GPIO_PIN_4) == RESET)
        value = 3;

    if (GPIO_ReadInputPin(GPIOC, (GPIO_Pin_TypeDef)GPIO_PIN_5) == RESET)
        value = 4;

    if (GPIO_ReadInputPin(GPIOC, (GPIO_Pin_TypeDef)GPIO_PIN_6) == RESET)
        value = 5;

    if (GPIO_ReadInputPin(GPIOC, (GPIO_Pin_TypeDef)GPIO_PIN_7) == RESET)
        value = 6;

    if (GPIO_ReadInputPin(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_1) == RESET)
        value = 7;

    if (GPIO_ReadInputPin(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_2) == RESET)
        value = 8;

    if (GPIO_ReadInputPin(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_3) == RESET)
        value = 9;

    sensor_input_disable();

    return value;
}

void sensor_init(void)
{
    sensor_input_disable();
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

    keyboard_init();
    pulse_output_init();
    sensor_init();

    for (;;) {
        if (time_after_eq(jiffies, curr_time + msecs_to_jiffies(500))) {
            curr_time = jiffies;
            GPIO_WriteReverse(GPIOB, (GPIO_Pin_TypeDef)GPIO_PIN_5);
        }

        if (keyboard_press() != -1) {
            GPIO_WriteHigh(GPIOB, (GPIO_Pin_TypeDef)GPIO_PIN_5);
            pulse_output(sensor_input());
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
