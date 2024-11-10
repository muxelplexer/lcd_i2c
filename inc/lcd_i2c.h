/*
 * Copyright (c) 2024 Sebastian Muxel
 * For license check LICENSE in project root.
 */
#pragma once
#include <stddef.h>
#include <stdint.h>

/**
 ** @brief board specific function used to initialize the i2c device.
 ** @params device - allows to pass an arbitrary pointer for device data.
 ** @params addr - the address of the i2c client.
 **/
typedef int (init_i2c_fn)(void* device, const uint8_t addr);

/**
 ** @brief board specific function used to write to the i2c device
 **        it is assumed that there is no pre-configures delay on writes.
 ** @params device - allows to pass an arbitrary pointer for device data.
 ** @params addr - the address of the i2c client.
 ** @params val - the value to write to the i2c client.
 **/
typedef int (write_i2c_fn)(void* device, const uint8_t addr, const uint8_t val);

/**
 ** @brief board specific function used to delay operations
 ** @params device - allows to pass an arbitrary pointer for device data.
 ** @params delay_us - amount of µseconds to wait.
 **/
typedef int (delay_us_i2c_fn)(void* device, size_t delay_us);


/**
 ** @brief struct storing all data needed for i2c interfacing.
 **
 ** device is optional and can be used by user code.
 ** All function pointers need to be set.
 **/
struct lcd_i2c
{
    void* device;
    init_i2c_fn* init_fn;
    write_i2c_fn* write_fn;
    delay_us_i2c_fn* delay_fn;
};

typedef enum lcd_backlight : uint8_t
{
    LCD_BL_OFF = 0x00,
    LCD_BL_ON = 0x08,
} lcd_bl_t;

/**
 ** @brief main structure storing all data needed by the lcd system.
 ** @param write_delay - use to adjust the delay after each character that is being printed.
 **/
typedef struct lcd
{
    struct lcd_i2c i2c_funcs;
    lcd_bl_t backlight;
    uint8_t addr;
    uint8_t rows;
    uint8_t cols;
    uint32_t write_delay_us;
} lcd_t;

/**
 ** @brief initializes a lcd structure
 ** @param lcd - the structure to initialize
 ** @param addr - the i2c client address
 ** @param cols - amount of columns of the LCD screen
 ** @param rows - amount of rows of the LCD screen
 **/
int lcd_init(lcd_t* lcd, const uint8_t addr, const uint8_t cols, const uint8_t rows);

/**
 ** @brief writes a byte to the LCD's parallel bus in 4-bit mode.
 ** @param lcd - the structure to initialize
 ** @param val - value to write
 ** @param mode - write mode to use
 **/
int lcd_write(lcd_t* lcd, const uint8_t val, const uint8_t mode);

/**
 ** @brief prints a string to the LCD-screen. Line-Wrap is enabled on per-character basis.
 ** @param lcd - the structure to initialize
 ** @param text - pointer to string in memory
 ** @param text_len - length of string
 ** @param line - LCD line to start printing on
 **/
int lcd_print(struct lcd* lcd, const char* text, const size_t text_len, uint8_t line);

/**
 ** @brief prints an character on the screen.
 ** @param lcd - the structure to initialize
 ** @param ch - character to print
 ** @param line - LCD line to reset to. If 0xFF it will continue.
 **/
int lcd_put_char(lcd_t* lcd, const char ch, uint8_t line);

/**
 ** @brief refreshes the LCD screen and clears all content from it
 ** @param lcd - the structure to initialize
 **/
int lcd_clear(lcd_t* lcd);

/**
 ** @brief turns the backlight on or off
 **        this will only set the internal flag for the backlight level
 **        and will only take effect on the next write
 ** @param lcd - the structure to initialize
 ** @param bl_level - set to LCD_BL_OFF or LCD_BL_ON.
 **/
int lcd_set_backlight(lcd_t* lcd, const lcd_bl_t bl_level);
