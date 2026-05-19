# LCD Driver & Renderer for HD44780

A lightweight, non-blocking LCD driver and renderer for HD44780-compatible character LCDs, designed for embedded systems with STM32 MCUs.

## Features

- **HD44780 Compatible** - Supports both 4-bit and 8-bit bus modes
- **Double Buffering** - Write to back buffer, commit atomically to front buffer
- **Non-blocking Rendering** - Progressive rendering with `tick()` function
- **Cursor Management** - Automatic cursor positioning during rendering
- **Convenient APIs** - `printf`-style formatting support

## Hardware Configuration

### Pin Mapping

Configure your pin assignments in `lcd_config.h`:

```c
// Data pins (order matters - D0 to D7)
static GPIO_TypeDef *data_port[] = {GPIOB, GPIOB, GPIOB, GPIOA, GPIOA, GPIOB, GPIOB, GPIOA};
static uint16_t      data_pin[]  = {D0_Pin, D1_Pin, D2_Pin, D3_Pin, D4_Pin, D5_Pin, D6_Pin, D7_Pin};

// Control pins
static GPIO_TypeDef *en_port = GPIOB;
static uint16_t      en_pin  = E_Pin;

static GPIO_TypeDef *rs_port = GPIOA;
static uint16_t      rs_pin  = RS_Pin;
```

### Bus Mode Selection

Uncomment this line in `lcd_config.h` to use 4-bit mode (saves I/O pins):

```c
#define LCD_4_BIT_MODE
```

By default, 8-bit mode is used.

## API Reference

### Initialization

```c
#include "lcd_renderer.h"

// Call once at startup
LcdRenderer_init();
```

### Writing to Display

**Set string at position:**
```c
LcdRenderer_set(0, 0, "Hello World");  // row 0, col 0
LcdRenderer_set(1, 0, "Line 2");
```

**Print formatted text:**
```c
uint32_t value = 42;
LcdRenderer_printf(0, 0, "Value: %lu", value);
```

### Commit Changes

Write to back buffer, then commit to front buffer:

```c
LcdRenderer_set(0, 0, "New Text");
LcdRenderer_commit();  // Schedule swap to front buffer
```

### Render Loop

Call `LcdRenderer_tick()` repeatedly in your main loop:

```c
while (1) {
    LcdRenderer_tick();  // Non-blocking - renders one character per call
    // Your application code here...
}
```

## Complete Example

```c
#include "lcd_renderer.h"
#include "main.h"

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    
    // Initialize LCD renderer
    LcdRenderer_init();
    
    // Initial display content
    LcdRenderer_set(0, 0, "STM32 LCD Demo");
    LcdRenderer_set(1, 0, "Count: ");
    LcdRenderer_commit();
    
    uint32_t counter = 0;
    
    while (1)
    {
        LcdRenderer_tick();  // Keep rendering
        
        // Update counter every second
        static uint32_t last_tick = 0;
        if (HAL_GetTick() - last_tick > 1000)
        {
            last_tick = HAL_GetTick();
            LcdRenderer_printf(1, 7, "%5lu", counter++);
            LcdRenderer_commit();
        }
    }
}
```

## Timing Requirements

The driver implements all necessary delays per HD44780 datasheet:

| Operation                 | Delay Required |
| ------------------------- | -------------- |
| Power-on to first command | ≥40 ms         |
| After first Function Set  | ≥4.1 ms        |
| After second Function Set | ≥100 µs        |
| Clear display             | ≥1.52 ms       |
| Other commands            | ≥37 µs         |

The `systick_delay_us()` function (assumed provided by your platform) is used for microsecond precision.

## Internal Architecture

### Double Buffering

```
Back Buffer    →    Front Buffer    →    Physical LCD
  (write)          (render source)      (current state)
```

- **Back buffer**: Application writes here
- **Front buffer**: Snapshot after `commit()`
- **Current buffer**: Actual LCD contents
- Changes detected by comparing front vs current buffers

### Rendering Algorithm

Each `tick()` call:
1. If not currently writing, find next dirty character position
2. Move cursor to dirty position (if needed)
3. Write character to LCD
4. Mark position as clean
5. Advance to next position

## Dependencies

- STM32 HAL drivers (GPIO)
- `systick.h` - Provides `systick_delay_us()`
- Standard C libraries: `string.h`, `stdio.h`, `stdarg.h`

## Buffer Size Configuration

Modify in `lcd_renderer.h`:

```c
#define LCD_ROWS 2   // Number of display rows
#define LCD_COLS 16  // Number of columns per row
```

## Notes

- The renderer assumes a 2x16 LCD by default (modify as needed)
- All functions are non-blocking except the initial `LcdRenderer_init()`
- The `printf` function uses a stack buffer sized to `LCD_COLS + 1`
- Cursor position is tracked automatically - no manual cursor management needed
