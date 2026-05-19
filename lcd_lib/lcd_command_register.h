#ifndef LCD_COMMAND_REGISTER_H_
#define LCD_COMMAND_REGISTER_H_
/* ─────────────────────────────────────────────
   1. CLEAR DISPLAY
   RS=0 RW=0 | DB7-DB0 = 0000 0001
   Fills all DDRAM with space (0x20), sets AC=0,
   returns display to original position.
   ───────────────────────────────────────────── */
#define LCD_CLEAR_DISPLAY           0b00000001    // Clear display, cursor home (~1.52ms)

/* ─────────────────────────────────────────────
   2. RETURN HOME
   RS=0 RW=0 | DB7-DB0 = 0000 001x
   Sets AC=0, returns shifted display to origin.
   DDRAM contents unchanged.
   ───────────────────────────────────────────── */
#define LCD_RETURN_HOME             0b00000010    // Cursor to home, DDRAM unchanged (~1.52ms)

/* ─────────────────────────────────────────────
   3. ENTRY MODE SET
   RS=0 RW=0 | DB7-DB0 = 0000 01 I/D S
   Sets cursor move direction and display shift
   on each DDRAM/CGRAM read or write.
   ───────────────────────────────────────────── */
#define LCD_ENTRY_MODE_SET          0b00000100    // Base: decrement, no shift
#define LCD_ENTRY_DECREMENT         0b00000000    // I/D=0: cursor moves left  (decrement AC)
#define LCD_ENTRY_INCREMENT         0b00000010    // I/D=1: cursor moves right (increment AC)
#define LCD_ENTRY_SHIFT_OFF         0b00000000    // S=0: display does not shift
#define LCD_ENTRY_SHIFT_ON          0b00000001    // S=1: display shifts on write

/* ─────────────────────────────────────────────
   4. DISPLAY ON/OFF CONTROL
   RS=0 RW=0 | DB7-DB0 = 0000 1 D C B
   D: display on/off
   C: cursor on/off
   B: cursor position character blink on/off
   ───────────────────────────────────────────── */
#define LCD_DISPLAY_CONTROL   0b00001000     // Base value
#define LCD_DISPLAY_OFF       0b00000000     // D=0: turn display off
#define LCD_DISPLAY_ON        0b00000100     // D=1: turn display on
#define LCD_CURSOR_OFF        0b00000000     // C=0: hide cursor
#define LCD_CURSOR_ON         0b00000010     // C=1: show cursor
#define LCD_BLINK_OFF         0b00000000     // B=0: no blink
#define LCD_BLINK_ON          0b00000001     // B=1: blink character at cursor position

/* ─────────────────────────────────────────────
   5. CURSOR / DISPLAY SHIFT
   RS=0 RW=0 | DB7-DB0 = 0001 S/C R/L xx
   Moves cursor or shifts display without changing DDRAM contents.
   S/C: 0=cursor move, 1=display shift
   R/L: 0=left,        1=right
   ───────────────────────────────────────────── */
#define LCD_CURSOR_DISPLAY_SHIFT     0b00010000    // Base value
#define LCD_CURSOR_SHIFT      0b00000000    // S/C=0 
#define LCD_DISPLAY_SHIFT     0b00001000    // S/C=1
#define LCD_LEFT              0b00000000    // R/L=0
#define LCD_RIGHT             0b00000100    // R/L=1

/* ─────────────────────────────────────────────
   6. FUNCTION SET
   RS=0 RW=0 | DB7-DB0 = 001 DL N F xx
   DL: 0=4-bit bus, 1=8-bit bus
   N:  0=1 line,    1=2 lines
   F:  0=5x8 dots,  1=5x10 dots
   NOTE: must be the first instruction sent;
   cannot change DL or N after initialization.
   ───────────────────────────────────────────── */
#define LCD_FUNCTION_SET   0b00100000    // Base value
#define LCD_4BIT           0b00000000     // DL=0
#define LCD_8BIT           0b00010000     // DL=1
#define LCD_1LINE          0b00000000     // N=0
#define LCD_2LINE          0b00001000     // N=1
#define LCD_5X8            0b00000000     // F=0
#define LCD_5X10           0b00000100     // F=1

/* ─────────────────────────────────────────────
   7. SET CGRAM ADDRESS
   RS=0 RW=0 | DB7-DB0 = 01 ACG5 ACG4 ACG3 ACG2 ACG1 ACG0
   Sets CGRAM address (6 bits) in AC.
   5x8 dots:  8 custom chars, addresses 0x00-0x3F
   5x10 dots: 4 custom chars, addresses 0x00-0x1F
   Data written/read after this goes to CGRAM.
   ───────────────────────────────────────────── */
#define LCD_SET_CGRAM_ADDR          0x40    // Base; OR with 6-bit address (0x00-0x3F)
// Usage: LCD_SET_CGRAM_ADDR | (char_index << 3)  for 5x8 (char 0-7)
// Usage: LCD_SET_CGRAM_ADDR | (char_index << 4)  for 5x10 (char 0-3)

/* ─────────────────────────────────────────────
   8. SET DDRAM ADDRESS
   RS=0 RW=0 | DB7-DB0 = 1 ADD6 ADD5 ADD4 ADD3 ADD2 ADD1 ADD0
   Sets DDRAM address (7 bits) in AC.
   1-line (N=0): 0x00-0x4F
   2-line (N=1): line 1 = 0x00-0x27
                 line 2 = 0x40-0x67
   ───────────────────────────────────────────── */
#define LCD_SET_DDRAM_ADDR          0x80    // Base; OR with 7-bit address
#define LCD_LINE1_START             0x80    // DDRAM address 0x00 (first char, line 1)
#define LCD_LINE2_START             0xC0    // DDRAM address 0x40 (first char, line 2)
#define LCD_LINE1_ADDR(col)         (0x80 + (col))        // col: 0-39
#define LCD_LINE2_ADDR(col)         (0xC0 + (col))        // col: 0-39

/* ─────────────────────────────────────────────
   9. READ BUSY FLAG + ADDRESS COUNTER
   RS=0 RW=1 | DB7=BF, DB6-DB0=AC
   BF=1: controller busy, do not send instructions.
   BF=0: ready to accept next instruction.
   AC reflects last CGRAM or DDRAM address set.
   Execution time: 0 µs (immediate read)
   ───────────────────────────────────────────── */
#define LCD_BUSY_FLAG               0x80    // Mask for DB7 in read result
// Usage: if (lcd_read_bf() & LCD_BUSY_FLAG) { /* wait */ }

/* ─────────────────────────────────────────────
   10. WRITE DATA TO CGRAM / DDRAM
   RS=1 RW=0 | DB7-DB0 = data byte
   Destination is determined by the last
   SET_CGRAM_ADDR or SET_DDRAM_ADDR issued.
   AC auto-increments or decrements per entry mode.
   ───────────────────────────────────────────── */
// No fixed opcode — send any 8-bit data byte with RS=1, RW=0.
// Example: lcd_write_data('A');  lcd_write_data(0x41);

/* ─────────────────────────────────────────────
   11. READ DATA FROM CGRAM / DDRAM
   RS=1 RW=1 | DB7-DB0 = data byte
   Must execute SET_CGRAM/DDRAM_ADDR first.
   First read after address set is invalid —
   discard it; valid data starts from 2nd read.
   ───────────────────────────────────────────── */
// No fixed opcode — read any 8-bit data byte with RS=1, RW=1.
// Example: uint8_t ch = lcd_read_data();

/* ─────────────────────────────────────────────
   INITIALIZATION TIMING (by instruction)
   Required delays when NOT using Busy Flag check
   ───────────────────────────────────────────── */
#define LCD_DELAY_POWER_ON_MS       50      // Wait >= 40ms after VCC >= 2.7V
#define LCD_DELAY_AFTER_RESET_1_MS  5       // Wait >= 4.1ms after first Function Set
#define LCD_DELAY_AFTER_RESET_2_US  150     // Wait >= 100µs after second Function Set
#define LCD_DELAY_CLEAR_MS          2        // Wait >= 1.52ms after Clear Display
#define LCD_DELAY_HOME_MS           2        // Wait >= 1.52ms after Return Home
#define LCD_DELAY_CMD_US            50      // Wait >= 37µs after all other instructions


#endif /* LCD_COMMAND_REGISTER_H_ */
