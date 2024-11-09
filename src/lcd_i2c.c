/*
 * Copyright (c) 2024 Sebastian Muxel
 * For license check LICENSE in project root.
 */
#include "lcd_i2c.h"
#include <stdint.h>

#define LCD_NOBACKLIGHT         0x00
#define LCD_BACKLIGHT           0x08

#define LCD_EN                  0b00000100  // Enable bit
#define LCD_RW                  0b00000010  // Read/Write bit
#define LCD_RS                  0b00000001  // Register select bit

/* commands */
#define LCD_CMD_CLS             0x01
#define LCD_CMD_RET             0x02
#define LCD_CMD_EMSET           0x04
#define LCD_CMD_DCTRL           0x08
#define LCD_CMD_CSHFT           0x10
#define LCD_CMD_FSET            0x20
#define LCD_CMD_SCGRAMADDR      0x40
#define LCD_CMD_SDDRAMADDR      0x80

// flags for display entry mode
#define LCD_EM_RIGHT            0x00
#define LCD_EM_LEFT             0x02
#define LCD_EM_SHIFTINCREMENT   0x01
#define LCD_EM_SHIFTDECREMENT   0x00

// flags for display on/off control
#define LCD_DCTRL_ON            0x04
#define LCD_DCTRL_OFF           0x00

/* cursor definitions */
#define LCD_DCTRL_CON           0x02
#define LCD_DCTRL_COFF          0x00

/* blink definitions */
#define LCD_DCTRL_BON           0x01
#define LCD_DCTRL_BOFF          0x00

/* function set controls */
#define LCD_FSET_8BITMODE       0x10
#define LCD_FSET_4BITMODE       0x00
#define LCD_FSET_2LINE          0x08
#define LCD_FSET_1LINE          0x00
#define LCD_FSET_5x10DOTS       0x04
#define LCD_FSET_5x8DOTS        0x00

static int lcd_write4b(lcd_t* lcd, uint8_t val);
static int lcd_strobe(lcd_t* lcd, const uint8_t val);

int lcd_init(lcd_t* lcd, const uint8_t addr, const uint8_t cols, const uint8_t rows)
{
    lcd->i2c_funcs.init_fn(lcd->i2c_funcs.device, addr);
    lcd->addr = addr;
    lcd->cols = cols;
    lcd->rows = rows;
    lcd->backlight = LCD_BL_ON;

    lcd_write(lcd, 0x3, 0x0);
    lcd_write(lcd, 0x3, 0x0);
    lcd_write(lcd, 0x3, 0x0);
    lcd_write(lcd, 0x2, 0x0);

    lcd_write(lcd, LCD_CMD_FSET | LCD_FSET_2LINE | LCD_FSET_5x8DOTS | LCD_FSET_4BITMODE, 0x0);
    lcd_write(lcd, LCD_CMD_DCTRL | LCD_DCTRL_ON | LCD_DCTRL_BON, 0x0);
    lcd_write(lcd, LCD_CMD_CLS, 0x0);
    lcd_write(lcd, LCD_CMD_EMSET | LCD_EM_LEFT, 0x0);

    return 0;
}


int lcd_write(lcd_t* lcd, uint8_t val, const uint8_t mode)
{
    lcd_write4b(lcd, mode | (val & 0xF0));
    lcd_write4b(lcd, mode | ((val << 4) & 0xF0));
    return 0;
}

int lcd_set_line(lcd_t* lcd, uint8_t line)
{
    switch(line)
    {
    case 0:
        lcd_write(lcd, 0x80, 0);
        break;
    case 1:
        lcd_write(lcd, 0xC0, 0);
        break;
    case 2:
        lcd_write(lcd, 0x94, 0);
        break;
    case 3:
        lcd_write(lcd, 0xD4, 0);
        break;
    default:
        return -1;
    }
    return 0;
}

static int lcd_clear_line(lcd_t* lcd, const uint8_t line)
{
    static const unsigned char empty[] = {0xFE, 0xFE,0xFE, 0xFE,0xFE, 0xFE,0xFE, 0xFE,0xFE, 0xFE,0xFE, 0xFE,0xFE, 0xFE,0xFE, 0xFE,0xFE, 0xFE,0xFE, 0xFE, 0x00};
    lcd_set_line(lcd, line);
    for (int i = 0; i < lcd->cols; ++i)
    {
        lcd_write(lcd, empty[i], 0);
        lcd->i2c_funcs.delay_fn(lcd->i2c_funcs.device, 1000);
    }
    return 0;
}

int lcd_print(lcd_t* lcd, const char* text, const size_t text_len, uint8_t line)
{
    lcd_set_line(lcd, line);

    int text_pos = 0;
    for (size_t i = 0; i < text_len; ++i)
    {
        if (text[i] == '\r') continue;
        if (text[i] == '\n')
        {
            if (text_pos != 0)
            {
                lcd_set_line(lcd, ++line);
                text_pos = 0;
            }
            continue;
        }

        lcd_write(lcd, (uint8_t)text[i], LCD_RS);

        if (text_pos == 20 - 1)
        {
            if (line == 3)
            {
                line = 0;
                lcd_clear_line(lcd, 0);
            } else
            {
                lcd_set_line(lcd, ++line);
            }
            text_pos = 0;
        } else
        {
            ++text_pos;
        }
        lcd->i2c_funcs.delay_fn(lcd->i2c_funcs.device, 75000);
    }
    return 0;
}

int lcd_clear(lcd_t* lcd)
{
    lcd_write(lcd, LCD_CMD_CLS, 0);
    lcd_write(lcd, LCD_CMD_RET, 0);
    return 0;
}

static int lcd_write4b(lcd_t* lcd, uint8_t val)
{
    val = val | lcd->backlight;
    int ret = lcd->i2c_funcs.write_fn(lcd->i2c_funcs.device, lcd->addr, val);
    lcd->i2c_funcs.delay_fn(lcd->i2c_funcs.device, 100);
    lcd_strobe(lcd, val);
    return ret;
}

static int lcd_strobe(lcd_t* lcd, const uint8_t val)
{
    uint8_t ena = val | LCD_EN | lcd->backlight;
    int ret = lcd->i2c_funcs.write_fn(lcd->i2c_funcs.device, lcd->addr, ena);

    uint8_t dis = (val & ~LCD_EN) | lcd->backlight;
    ret = lcd->i2c_funcs.write_fn(lcd->i2c_funcs.device, lcd->addr, dis);
    return ret;
}
