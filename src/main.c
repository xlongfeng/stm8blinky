#include <stdint.h>

#include "stm8s.h"

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1) {
    }
}
#endif

void delay(uint16_t nCount)
{
    /* Decrement nCount value */
    while (nCount != 0) {
        nCount--;
    }
}

int main()
{
    /* Initialize I/Os in Output Mode */
    GPIO_Init(GPIOB, (GPIO_Pin_TypeDef)GPIO_PIN_5, GPIO_MODE_OUT_OD_LOW_SLOW);

    while (1) {
        GPIO_WriteReverse(GPIOB, (GPIO_Pin_TypeDef)GPIO_PIN_5);
        delay(0xFFFF);
    }

    return 0;
}
