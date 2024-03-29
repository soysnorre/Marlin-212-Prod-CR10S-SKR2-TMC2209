/*

  u8g_com_std_sw_spi.c

  Universal 8bit Graphics Library

  Copyright (c) 2015, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification,
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list
    of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice, this
    list of conditions and the following disclaimer in the documentation and/or other
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "u8g.h"

#ifdef U8G_WITH_PINLIST

  static void u8g_sw_spi_shift_out(uint8_t dataPin, uint8_t clockPin, uint8_t val) {
    uint8_t i = 8;
    do {
      if (val & 128)
        u8g_SetPinLevel(dataPin, 1);
      else
        u8g_SetPinLevel(dataPin, 0);
      val <<= 1;
      u8g_MicroDelay(); // 23 Sep 2012
      // delay(1);
      u8g_SetPinLevel(clockPin, 1);
      u8g_MicroDelay(); // 23 Sep 2012
      // delay(1);
      u8g_SetPinLevel(clockPin, 0);
      u8g_MicroDelay(); // 23 Sep 2012
      // delay(1);
      i--;
    } while (i != 0);
  }

  uint8_t u8g_com_std_sw_spi_fn(u8g_t *u8g, uint8_t msg, uint8_t arg_val, void *arg_ptr) {
    switch (msg) {
      case U8G_COM_MSG_INIT:
        u8g_SetPIOutput(u8g, U8G_PI_SCK);
        u8g_SetPIOutput(u8g, U8G_PI_MOSI);
        u8g_SetPIOutput(u8g, U8G_PI_RESET);
        u8g_SetPIOutput(u8g, U8G_PI_CS);
        u8g_SetPIOutput(u8g, U8G_PI_A0);
        u8g_SetPILevel(u8g, U8G_PI_SCK, 0);
        u8g_SetPILevel(u8g, U8G_PI_MOSI, 0);
        break;

      case U8G_COM_MSG_STOP:
        break;

      case U8G_COM_MSG_RESET:
        u8g_SetPILevel(u8g, U8G_PI_RESET, arg_val);
        break;

      case U8G_COM_MSG_CHIP_SELECT:
        if (arg_val == 0) {
          // disable
          u8g_SetPILevel(u8g, U8G_PI_CS, 1);
        }
        else {
          // enable
          u8g_SetPILevel(u8g, U8G_PI_SCK, 0);
          u8g_SetPILevel(u8g, U8G_PI_CS, 0);
        }
        break;

      case U8G_COM_MSG_WRITE_BYTE:
        u8g_sw_spi_shift_out(u8g->pin_list[U8G_PI_MOSI], u8g->pin_list[U8G_PI_SCK], arg_val);
        break;

      case U8G_COM_MSG_WRITE_SEQ: {
        register uint8_t *ptr = arg_ptr;
        while (arg_val > 0) {
          u8g_sw_spi_shift_out(u8g->pin_list[U8G_PI_MOSI], u8g->pin_list[U8G_PI_SCK], *ptr++);
          arg_val--;
        }
      }
      break;

      case U8G_COM_MSG_WRITE_SEQ_P: {
        register uint8_t *ptr = arg_ptr;
        while (arg_val > 0) {
          u8g_sw_spi_shift_out(u8g->pin_list[U8G_PI_MOSI], u8g->pin_list[U8G_PI_SCK], u8g_pgm_read(ptr));
          ptr++;
          arg_val--;
        }
      }
      break;

      case U8G_COM_MSG_ADDRESS:                   // define cmd (arg_val = 0) or data mode (arg_val = 1)
        u8g_SetPILevel(u8g, U8G_PI_A0, arg_val);
        break;
    }
    return 1;
  }

#else // ifdef U8G_WITH_PINLIST

  uint8_t u8g_com_std_sw_spi_fn(u8g_t *u8g, uint8_t msg, uint8_t arg_val, void *arg_ptr) {
    return 1;
  }

#endif // ifdef U8G_WITH_PINLIST
