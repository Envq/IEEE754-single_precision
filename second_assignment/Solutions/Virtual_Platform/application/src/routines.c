#include "routines.h"

// Multiplication Routine
uint32_t mul(uint16_t op1, uint16_t op2) {
  // Prepare the variable used to read from pready.
  uint32_t result = 0x0000;

  // reset FLAGS
  reset_flags();
  // Prepare the data to send.
  set_pwdata_16(op1, op2);
  // Select the peripheral.
  set_psel(PSEL1);
  // Enable the operation.
  set_penable(1);
  // stay here untile prdata_rdy is ready.
  while (get_pready() == 0)
    __asm__("nop");
  // Get the result.
  result = get_prdata();
  // Disable the operation.
  set_penable(0);
  // Disable data stream from the bus.
  set_psel(NO_PSEL);
  // Clear Data
  // set_pwdata(0);
  // Return the result.
  return result;
}

// IO Write Routine
void io_write(uint32_t data) {

  reset_flags();
  // Set data to write on IO Module
  set_pwdata(data);
  // Set Write operation
  set_pwrite(1);
  // Select Peripheral 2
  set_psel(PSEL2);

  set_psel(NO_PSEL);
  set_penable(0);
  set_pwdata(0x00000000);
  set_pwrite(0);
}

// IO Read Routine
uint32_t io_read(void) {
  uint32_t data;
  reset_flags();
  // Set Read Operation
  set_pwrite(0);
  // Select Peripheral 2 = IO Module!
  set_psel(PSEL2);

  // stay here untile prdata_rdy is ready.
  while (get_pready() == 0)
    __asm__("nop");

  data = get_prdata();
  set_psel(NO_PSEL);
  set_penable(0);
  set_pwdata(0x00000000);
  set_pwrite(0);

  return data;
}

// Multiplier Routine
uint32_t float_multiplier(uint32_t op1, uint32_t op2) {
  // Prepare the variable used to read from pready.
  uint32_t result = 0x00000000;

  // Reset FLAGS.
  reset_flags();

  // Select the peripheral.
  set_psel(PSEL3);
  // Prepare the first operand to send.

  set_pwdata(op1);
  // Enable the first operand read.
  set_penable(1);
  // Disable the first operand read.
  set_penable(0);

  // Prepare the second operand to send.
  set_pwdata(op2);
  // Enable the operation.
  set_penable(1);

  // Wait here until prdata_rdy is ready.
  while (get_pready() == 0)
    __asm__("nop");

  // Get the result.
  result = get_prdata();
  // Disable the operation.
  set_penable(0);

  // Disable data stream from the bus.
  set_psel(NO_PSEL);
  // Return the result.
  return result;
}

// Double multiplier Routine
void double_multiplier(uint32_t op1, uint32_t op2, uint32_t op3, uint32_t op4,
                       uint32_t *res1, uint32_t *res2) {
  // Prepare the variables used to read from pready.
  *res1 = 0x00000000;
  *res2 = 0x00000000;

  // reset FLAGS
  reset_flags();

  // Select the peripheral.
  set_psel(PSEL4);

  // Prepare op1 to send.
  set_pwdata(op1);
  // Enable op1 read.
  set_penable(1);
  // Disable op1 read.
  set_penable(0);

  // Prepare op2 to send.
  set_pwdata(op2);
  // Enable op2 read.
  set_penable(1);
  // Disable op2 read.
  set_penable(0);

  // Prepare op3 to send.
  set_pwdata(op3);
  // Enable op3 read.
  set_penable(1);
  // Disable op3 read.
  set_penable(0);

  // Prepare op4 to send.
  set_pwdata(op4);
  // Enable op4 read.
  set_penable(1);

  // stay here until prdata_rdy is ready.
  while (get_pready() == 0)
    __asm__("nop");

  // Get the result.
  *res1 = get_prdata();
  // Disable the operation.
  set_penable(0);

  // Enable op4 read.
  set_penable(1);
  
  // stay here until prdata_rdy is ready.
  while (get_pready() == 0)
    __asm__("nop");

  // Get the result.
  *res2 = get_prdata();
  // Disable the operation.
  set_penable(0);

  // Disable data stream from the bus.
  set_psel(NO_PSEL);
}
