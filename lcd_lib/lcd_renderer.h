// lcd_renderer.h

#ifndef LCD_RENDERER_H
#define LCD_RENDERER_H

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#define LCD_ROWS 2
#define LCD_COLS 16

void LcdRenderer_init   (void);
void LcdRenderer_set    (uint8_t row, uint8_t col, const char *str);
void LcdRenderer_printf (uint8_t row, uint8_t col, const char *fmt, ...);
void LcdRenderer_commit (void);
void LcdRenderer_tick   (void);

#endif
