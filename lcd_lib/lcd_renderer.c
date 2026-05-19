#include "lcd_renderer.h"
#include "lcd.h"
#include <string.h>
#include <stdio.h>

// ---------------------------------------------------------------------------
// Constantes
// ---------------------------------------------------------------------------

#define CURSOR_INVALID 0xFF

// ---------------------------------------------------------------------------
// Estado interno
// ---------------------------------------------------------------------------

static char back   [LCD_ROWS][LCD_COLS + 1]; // escribe la app
static char front  [LCD_ROWS][LCD_COLS + 1]; // lee el render
static char current[LCD_ROWS][LCD_COLS + 1]; // lo que hay en pantalla

static bool    dirty_rows[LCD_ROWS];
static bool    swap_pending;

static uint8_t scan_row, scan_col;
static uint8_t cursor_row, cursor_col;
static bool    writing; // false = fase cursor, true = fase escritura

// ---------------------------------------------------------------------------
// Helpers privados
// ---------------------------------------------------------------------------

static void advance_scan(void)
{
    if (++scan_col >= LCD_COLS) { scan_col = 0; scan_row++; }
}

static void invalidate_cursor(void)
{
    cursor_row = cursor_col = CURSOR_INVALID;
}

// ---------------------------------------------------------------------------
// Privadas
// ---------------------------------------------------------------------------

static void mark_dirty_from_front(void)
{
    for (uint8_t r = 0; r < LCD_ROWS; r++)
        for (uint8_t c = 0; c < LCD_COLS; c++)
            if (front[r][c] != current[r][c]) { dirty_rows[r] = true; break; }
}

static void maybe_swap_buffers(void)
{
    if (!swap_pending) return;

    memcpy(front, back, sizeof(front));
    swap_pending = false;
    mark_dirty_from_front();
    scan_row = scan_col = 0;
}

static bool find_next_dirty(void)
{
    while (scan_row < LCD_ROWS)
    {
        if (dirty_rows[scan_row])
        {
            while (scan_col < LCD_COLS)
            {
                if (front[scan_row][scan_col] != current[scan_row][scan_col])
                    return true;
                scan_col++;
            }
            dirty_rows[scan_row] = false;
        }
        scan_col = 0;
        scan_row++;
    }

    // Frame terminado — hacer swap si hay uno pendiente y re-buscar
    scan_row = scan_col = 0;
    maybe_swap_buffers();

    // Solo re-buscar si el swap dejó trabajo pendiente
    for (uint8_t r = 0; r < LCD_ROWS; r++)
        if (dirty_rows[r]) return find_next_dirty();

    return false;
}

// ---------------------------------------------------------------------------
// Publicas
// ---------------------------------------------------------------------------

void LcdRenderer_init(void)
{
    memset(back,    ' ',  sizeof(back));
    memset(front,   ' ',  sizeof(front));
    memset(current, '\0', sizeof(current)); // distinto a front → render inicial completo

    for (uint8_t r = 0; r < LCD_ROWS; r++)
    {
        back[r][LCD_COLS] = front[r][LCD_COLS] = current[r][LCD_COLS] = '\0';
        dirty_rows[r] = true;
    }

    swap_pending = false;
    scan_row = scan_col = 0;
    invalidate_cursor(); // fuerza el primer Lcd_cursor
    writing = false;

    Lcd_init();
}

void LcdRenderer_set(uint8_t row, uint8_t col, const char *str)
{
    if (row >= LCD_ROWS || col >= LCD_COLS || !str) return;

    for (uint8_t i = 0; str[i] && (col + i) < LCD_COLS; i++)
        back[row][col + i] = str[i];
}

void LcdRenderer_printf(uint8_t row, uint8_t col, const char *fmt, ...)
{
    char tmp[LCD_COLS + 1];
    va_list args;
    va_start(args, fmt);
    vsnprintf(tmp, sizeof(tmp), fmt, args);
    va_end(args);
    LcdRenderer_set(row, col, tmp);
}

void LcdRenderer_commit(void)
{
    swap_pending = true;
}

void LcdRenderer_tick(void)
{
    if (!writing)
    {
        if (!find_next_dirty()) return;

        if (cursor_row != scan_row || cursor_col != scan_col)
        {
            Lcd_cursor(scan_row, scan_col);
            cursor_row = scan_row;
            cursor_col = scan_col;
            writing = true;
            return; // Mover el cursor consume el tick; la escritura ocurre en el siguiente
        }
    }

    // Fase escritura
    current[scan_row][scan_col] = front[scan_row][scan_col];
    Lcd_char(current[scan_row][scan_col]);

    invalidate_cursor();

    advance_scan();
    writing = false;
}
