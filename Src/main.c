/**
 */
#include "platform_hw.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_gpio.h"
#include "stm32f0xx_hal_i2c.h"
#include "iprintf.h"

#include "led.h"
#include "board_id.h"
#include "version.h"

#include <string.h>
#include <stdlib.h>

union Interrupts {
   uint32_t mask;
   struct {
      uint8_t     accelerometer  : 1;
      uint8_t     charger        : 1;
   };
};

int main(void)
{
   HAL_Init();

   platformHW_Init();

   iprintf("\r\nStarting... (v%d | #0x%x / 0x%x | Built "__DATE__":"__TIME__")\r\n", FW_VERSION, bid_GetID(), bid_GetIDCrc());

   led_Init();
   HAL_Delay(10);
   led_ClearDisplay();

   int i;
   while(1) {
      iprintf("Start| ");

      for(i = 0; i < 36; i++) {
         led_SetChannel(i, 250);
         HAL_Delay(10);
      }

      HAL_Delay(10);

      for(i = 36; i >= 0; i--) {
         led_SetChannel(i, 0);
         HAL_Delay(10);
      }
   }

   return 0;
}

