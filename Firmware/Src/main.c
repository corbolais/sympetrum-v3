/**
 */
#include "platform_hw.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_gpio.h"
#include "stm32f0xx_hal_i2c.h"
#include "stm32f0xx_hal_tim.h"
#include "iprintf.h"

#include "pattern.h"
#include "lighting.h"
//TODO prune these
#include "led.h"
#include "color.h"
#include "als.h"
#include "board_id.h"
#include "test.h"
#include "version.h"
#include "test_patterns.h"

#include "ir.h"

#include <string.h>
#include <stdlib.h>

#define BUTTON_DEBOUNCE_MS    150
static uint32_t lastUserButton = 0;

// indicate if we are in test mode
static bool TestMode = false;
static bool TestModeLED = false;

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

   pattern_Init();
   lighting_Init();

   //TODO enter on boot if button held down
   // fall into LED test patterns forever
   //TestModeLED = true;
   //TestPatterns_Start();

   while(true) {
      // handle the button
      if(lastUserButton && (HAL_GetTick() - lastUserButton > BUTTON_DEBOUNCE_MS)) {
         lastUserButton = 0;

         iprintf("Button Pressed\n");
         //TODO do something else
         pattern_DoSendBeacon();
      }

      pattern_Timeslice(HAL_GetTick());
      lighting_Timeslice(HAL_GetTick());
   }

   return 0;
}

void main_DoButton(bool const buttonPressed) {
   if(TestMode) {
      test_UserButton(buttonPressed);
   }
   else if(TestModeLED) {
      if(buttonPressed) {
         TestPatterns_Button();
      }
   }
   else {
      // running the normal program
      if(lastUserButton == 0 && buttonPressed) {
         lastUserButton = HAL_GetTick();
      }
   }
}

