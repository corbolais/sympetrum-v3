/**
 */
#include "platform_hw.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_gpio.h"
#include "stm32f0xx_hal_i2c.h"
#include "stm32f0xx_hal_tim.h"
#include "iprintf.h"

#include "led.h"
#include "als.h"
#include "board_id.h"
#include "version.h"

#include "ir.h"

#include <string.h>
#include <stdlib.h>

union Interrupts {
   uint32_t mask;
   struct {
      uint8_t     accelerometer  : 1;
      uint8_t     charger        : 1;
   };
};

static void testDarknetIR(void) {
   iprintf("Darknet TX\n");

   IRInit();

   iprintf("dnet IR init done\n");

   uint8_t buf[] = "Test Str";

   iprintf("TX...");
   IRTxBuff(buf, sizeof(buf) - 1);
   iprintf("done\n");
   iprintf("have %d bytes\n", IRBytesAvailable());
}

int main(void)
{
   HAL_Init();

   platformHW_Init();

   iprintf("\r\nStarting... (v%d | #0x%x / 0x%x | Built "__DATE__":"__TIME__")\r\n", FW_VERSION, bid_GetID(), bid_GetIDCrc());

   //FIXME mv?
   als_Init();

   //FIXME en
   led_Init();

   //testDarknetIR();

   led_SetGlobalBrightness(100);

   IRInit();

   //FIXME rm?
   uint32_t lux;
   als_GetLux(&lux);
   led_SetGlobalBrightness(10 + (lux / 10));

   //FIXME move
   struct color_ColorHSV c = {.h = 10, .s = 255, .v = 255};
   int x, y;
   uint8_t off = 0;

   int count = 0;

   uint32_t bytes = 0;
   while(true) {
      if(IRDataReady()) {
         iprintf("Got Full Message! ");

         uint8_t* buf = IRGetBuff(&bytes);
         iprintf("%d bytes: [%s]\n", bytes, (char*)buf);
      }

      //permute
      for(y = 0; y < 4; y++) {
         for(x = 0; x < 12; x++) {
            c.h = (x * 5) + (y * 8) + off;
            led_DrawPixel(x, y, &c);
         }
      }
      off++;

      //FIXME rm
      if(count > 100) {
         count = 0;

         als_GetLux(&lux);
         //calculate the brightness to set
         if(60 + (lux / 10) > 255) {
            led_SetGlobalBrightness(255);
         }
         else {
            led_SetGlobalBrightness(60 + (lux / 10));
         }

         iprintf("counts = %d\n", lux);
      }
      count++;
   }

   return 0;
}

