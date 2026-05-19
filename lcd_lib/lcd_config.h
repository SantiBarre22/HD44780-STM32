#ifndef LCD_CONFIG_H
#define LCD_CONFIG_H

#include "main.h"

// ─── Bus Width ───────────────────────────────────────────────
// #define LCD_4_BIT_MODE

// ─── Bus Width Len ───────────────────────────────────────────

#define LCD_COMMAND_REG 	GPIO_PIN_RESET
#define LCD_DATA_REG 		GPIO_PIN_SET


// ─── Data Pins ───────────────────────────────────────────────
static GPIO_TypeDef *data_port[] = {GPIOB,       GPIOB,      GPIOB,      GPIOA,      GPIOA,      LCD_D5_GPIO_Port,      GPIOB,      GPIOA};
static uint16_t      data_pin[]  = {LCD_D0_Pin,  LCD_D1_Pin, LCD_D2_Pin, LCD_D3_Pin, LCD_D4_Pin, LCD_D5_Pin, LCD_D6_Pin, LCD_D7_Pin};

// ─── Enable Pin ──────────────────────────────────────────────
static GPIO_TypeDef *en_port = GPIOB;
static uint16_t      en_pin  = LCD_E_Pin;

// ─── Register Select Pin ─────────────────────────────────────
static GPIO_TypeDef *rs_port = GPIOA;
static uint16_t      rs_pin  = LCD_RS_Pin;

#endif // LCD_CONFIG_H
