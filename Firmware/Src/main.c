/**
 */
#include "platform_hw.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_gpio.h"
#include "stm32f0xx_hal_i2c.h"
#include "stm32f0xx_hal_tim.h"
#include "iprintf.h"

#include "led.h"
#include "color.h"
#include "als.h"
#include "board_id.h"
#include "test.h"
#include "version.h"

#include "ir.h"

#include <string.h>
#include <stdlib.h>

union Interrupts {
   uint32_t mask;
   struct {
      //FIXME update with real peripherals
      uint8_t     accelerometer  : 1;
      uint8_t     charger        : 1;
   };
};

// indicate if we are in test mode
static bool TestMode = false;

static void testDarknetIR(void) {
   iprintf("Darknet TX\n");

   /*
   uint8_t b = 0x00;
   IRTxBuff(&b, 1);
   */

   uint8_t buf[] = "Test Str";
   iprintf("TX...");
   IRTxBuff(buf, sizeof(buf) - 1);
   iprintf("done\n");
   iprintf("have %d bytes\n", IRBytesAvailable());
}

// just paint the display white
static void fillWhite(void) {
   struct color_ColorHSV c = {.h = 10, .s = 0, .v = 255};
   int row, col;

   iprintf("Fill display white forever\n");
   while(true) {
      for(row = 0; row < 4; row++) {
         for(col = 0; col < 12; col++) {
            iprintf("[%d,%d]\n", row, col);

            //led_ClearDisplay();
            led_DrawPixel(row, col, &c);

            //HAL_Delay(500);
         }
      }
   }
}

static uint8_t HueTable[] = {0
};

int main(void)
{
   HAL_Init();

   platformHW_Init();

   iprintf("\r\nStarting... (v%d | #0x%x / 0x%x | Built "__DATE__":"__TIME__")\r\n", FW_VERSION, bid_GetID(), bid_GetIDCrc());

   // if we should enter test mode, do that
   if(test_EnterTestMode()) {
      TestMode = true;

      // this should never return
      //test_DoTests();
   }
   TestMode = false;

   //FIXME mv? into LED?
   als_Init();

   IRInit();

   iprintf("Init LEDs\n");
   led_Init();

   //FIXME rm
   //testDarknetIR();
   //iprintf(">> DONE TEST DARKNET IR<<\n");

   //FIXME rm?
   led_SetGlobalBrightness(255);

   //FIXME move
   struct color_ColorHSV color = {.h = 10, .s = 255, .v = 255};

   int huePhase = 0;
   int count = 0;
   uint32_t bytes = 0;
   while(true) {
      if(IRDataReady()) {
         iprintf("Got Full Message! ");

         uint8_t* buf = IRGetBuff(&bytes);
         iprintf("%d bytes: [%s]\n", bytes, (char*)buf);
      }

      //permute
      /*
      if(count % 50 == 0) {
         for(x = 0; x < 4; x++) {
            for(y = 0; y < 12; y++) {
               c.h = off;
               led_DrawPixel(x, y, &c);
            }
         }
      }
      */
      /*
      if(count % 1000) {
         for(int i = 0; i < ChasingSize; i++) {
            struct coord const * c = &chasingArray[i];
            color.h = huePhase + (i * 10);
            led_DrawPixel(c->x, c->y, &color);
         }
      }
      */
      if(count % 1000) {
         for(int i = 0; i < 12 * 2; i++) {
            color.h = huePhase + (i * 10);
            led_DrawPixelLinear(i, &color);
            led_DrawPixelLinear(47 - i, &color);
         }
      }

      if(count % 9000) {
         huePhase += 1;
      }

      /*
      //FIXME rm
      if(count > 100) {
         count = 0;

         uint32_t lux;
         als_GetLux(&lux);

         iprintf("Light Counts = %d\n", lux);
      }
      */
      count++;
   }

   return 0;
}

void main_DoButton(bool const buttonPressed) {
   iprintf("Button to %d\n", buttonPressed);

   if(TestMode) {
      test_UserButton(buttonPressed);
   }
   else {
   }
}

