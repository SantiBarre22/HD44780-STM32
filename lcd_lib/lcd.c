/*
 * lcd.c
 *
 *  Created on: 10/06/2018
 *      Author: Olivier Van den Eede
 *  Modified on: 18/05/2026
 *      Author: Santiago Barrenechea
 */

#include "lcd_command_register.h"
#include "lcd_config.h"
#include "lcd.h"
#include "systick.h"

#ifdef LCD_4_BIT_MODE
    #define LCD_BUS_LEN 4
#else
    #define LCD_BUS_LEN 8
#endif

/************************************** Private constants ****************************************/

/************************************** Static declarations **************************************/

static void lcd_write(uint8_t mode, uint8_t data);
static void lcd_bus(uint8_t data);

/************************************** Function definitions **************************************/

/**
 * @brief  Initialize the LCD controller (HD44780 compatible).
 * @note   Configures the LCD in 4-bit or 8-bit mode depending on the defined
 *         macro. Display is turned on with no cursor and no blink.
 *         Ensure power-on delay requirements are met before calling this function.
 */
void Lcd_init(void)
{
	HAL_Delay(LCD_DELAY_POWER_ON_MS);
    lcd_write(LCD_COMMAND_REG, LCD_FUNCTION_SET | LCD_8BIT);
	HAL_Delay(LCD_DELAY_AFTER_RESET_1_MS);
    lcd_write(LCD_COMMAND_REG, LCD_FUNCTION_SET | LCD_8BIT);
    systick_delay_us(LCD_DELAY_AFTER_RESET_2_US);
    lcd_write(LCD_COMMAND_REG, LCD_FUNCTION_SET | LCD_8BIT);

#ifdef LCD_4_BIT_MODE
	lcd_write(LCD_COMMAND_REG, LCD_FUNCTION_SET | LCD_4BIT);
	lcd_write(LCD_COMMAND_REG, LCD_FUNCTION_SET | LCD_4BIT | LCD_2LINE | LCD_5X8);
#else
    lcd_write(LCD_COMMAND_REG, LCD_FUNCTION_SET | LCD_8BIT | LCD_2LINE | LCD_5X8);
#endif

    lcd_write(LCD_COMMAND_REG, LCD_DISPLAY_CONTROL | LCD_DISPLAY_ON | LCD_CURSOR_OFF | LCD_BLINK_OFF);
    systick_delay_us(LCD_DELAY_AFTER_RESET_2_US);
    lcd_write(LCD_COMMAND_REG, LCD_CLEAR_DISPLAY);
	HAL_Delay(LCD_DELAY_CLEAR_MS);
	lcd_write(LCD_COMMAND_REG, LCD_ENTRY_MODE_SET | LCD_ENTRY_DECREMENT | LCD_ENTRY_SHIFT_OFF);

}

/**
 * @brief  Write a single character at the current cursor position.
 * @param  c: Character to display.
 */
void Lcd_char(const char c)
{
    lcd_write(LCD_DATA_REG, c);
}

/**
 * @brief  Set the cursor to a specific position on the LCD.
 * @param  row: Row index (0-based).
 * @param  col: Column index (0-based).
 */
void Lcd_cursor(uint8_t row, uint8_t col)
{
    uint8_t addr;
    switch (row)
    {
        case 0:  addr = LCD_LINE1_ADDR(col); break;
        case 1:  addr = LCD_LINE2_ADDR(col); break;
        default: return; // fila inválida
    }
    lcd_write(LCD_COMMAND_REG, addr);
}

/************************************** Static function definition **************************************/

/**
 * @brief  Write a byte to the LCD, selecting data or command register.
 * @param  mode: Register select — LCD_DATA_REG or LCD_COMMAND_REG.
 * @param  data: Byte to send.
 * @note   In 4-bit mode the byte is split into two nibbles (high then low).
 */
static void lcd_write(uint8_t mode, uint8_t data)
{
    HAL_GPIO_WritePin(rs_port, rs_pin, mode);

#ifdef LCD_4_BIT_MODE
    lcd_bus(data >> 4);lcd_bus(data & 0x0F);
#else
    lcd_bus(data);
#endif
}

/**
 * @brief  Put data on the bus lines and pulse the enable line to latch it.
 * @param  data: Nibble or byte to write, depending on LCD bus width.
 * @note   Enable pulse width must be at least 450ns per HD44780 datasheet.
 */
static void lcd_bus(uint8_t data)
{
    for (uint8_t i = 0; i < LCD_BUS_LEN; i++)
        HAL_GPIO_WritePin(data_port[i], data_pin[i], (data >> i) & 0x01);

    systick_delay_us(LCD_DELAY_CMD_US);
    HAL_GPIO_WritePin(en_port, en_pin, GPIO_PIN_SET);
    systick_delay_us(1);
    HAL_GPIO_WritePin(en_port, en_pin, GPIO_PIN_RESET);
}
