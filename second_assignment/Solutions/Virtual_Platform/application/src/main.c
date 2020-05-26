//----------------------------------------------------------
//-- TITLE: Example of C main code rom for mos6502
//--
//-- Author:	Stefano Centomo, Enrico Sgarbanti
//-- Date:	26/05/2020
//----------------------------------------------------------
//---------------------------------------
//---------------------------------------
//
//	M6502
//	SUPPORTED TYPES & DIMENSIONS
//
//  int 		=>	16 bits
//  char		=>	8 bits
//  uint8_t/int8_t	=>	8 bits
//  uint16_t/int16_t	=>	16 bits
//  uint32_t/int32_t	=>	32 bits
//  float/double	=> not supported
//
//---------------------------------------
//---------------------------------------

#include "custom_stdlib.h"
#include "routines.h"
#include <stdbool.h>

int main() {
  // VARIABLES
  uint32_t op1 = 0x3f800000; // 1.0
  uint32_t op2 = 0x40000000; // 2.0
  uint32_t op3 = 0x40400000; // 3.0
  uint32_t op4 = 0x00000000; // to read
  uint32_t res1 = 0;
  uint32_t res2 = 0;
  int mode = 2;

  // Read op4 data from IO Module
  op4 = io_read();

  // MULTIPLIERS #####################################################
  if (mode == 1) {
    // Reset multiplier
    set_psel(PSEL3);
    set_presetn(1);
    set_presetn(0);

    // Exec first float multiplication and write result on IO module
    res1 = float_multiplier(op1, op2);
    io_write(res1);

    // Exec second float multiplication and write result on IO module
    res2 = float_multiplier(op3, op4);
    io_write(res2);
  }

  // DOUBLE MULTIPLIER ###############################################
  if (mode == 2) {
    // Reset multiplier
    set_psel(PSEL4);
    set_presetn(1);
    set_presetn(0);
    // Exec double multiplication and write result on IO module
    double_multiplier(op1, op2, op3, op4, &res1, &res2);
    io_write(res1);
    io_write(res2);
  }

  return 0;
}