/*
 * lcd.h
 *
 *  Created on: May 18, 2026
 *      Author: santi
 */

#ifndef LCD_H_
#define LCD_H_

void Lcd_init(void);
void Lcd_char(const char c);
void Lcd_cursor(uint8_t row, uint8_t col);

#endif /* LCD_H_ */
