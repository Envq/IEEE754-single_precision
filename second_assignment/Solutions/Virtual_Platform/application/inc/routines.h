//-------------------------------------------------------------
// Title:	Driver Library for m6502-Splatters platform
// Author:	Stefano Centomo, Enrico Sgarbanti
// Date:	26/05/2020
//-------------------------------------------------------------
#include "mmio.h"
#include "stdbool.h"

// Send op1 and op2 to multiplier connected to psel2, and return the result.
uint32_t mul(uint16_t op1, uint16_t op2);

// WRITE to IO module
void io_write(uint32_t data);

// READ from IO module
uint32_t io_read(void);

// Send op1 and op2 to multiplier connected to psel3, and return the result.
uint32_t float_multiplier(uint32_t op1, uint32_t op2);

// Send op1, op2, op3, op4 to double_multiplier connected to psel4, and update
// res1 and res2.
void double_multiplier(uint32_t op1, uint32_t op2, uint32_t op3, uint32_t op4,
                       uint32_t *res1, uint32_t *res2);