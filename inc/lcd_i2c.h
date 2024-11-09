/*
 * Copyright (c) 2024 Sebastian Muxel
 * For license check LICENSE in project root.
 */
#pragma once
#include <stddef.h>
#include <stdint.h>

/*
 * @brief board specific function used to initialize the i2c device.
 * @params device - allows to pass an arbitrary pointer for device data.
 * @params addr - the address of the i2c client.
 */
typedef int (init_i2c_fn)(void* device, const uint8_t addr);

/*
 * @brief board specific function used to write to the i2c device
 *        it is assumed that there is no pre-configures delay on writes.
 * @params device - allows to pass an arbitrary pointer for device data.
 * @params addr - the address of the i2c client.
 * @params val - the value to write to the i2c client.
 */
typedef int (write_i2c_fn)(void* device, const uint8_t addr, const uint8_t val);

/*
 * @brief board specific function used to delay operations
 * @params device - allows to pass an arbitrary pointer for device data.
 * @params delay_us - amount of µseconds to wait.
 */
typedef int (delay_us_i2c_fn)(void* device, size_t delay_us);

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

typedef struct lcd
{
    struct lcd_i2c i2c_funcs;
    lcd_bl_t backlight;
    uint8_t addr;
    uint8_t rows;
    uint8_t cols;
} lcd_t;

int lcd_init(lcd_t* lcd, const uint8_t addr, const uint8_t cols, const uint8_t rows);
int lcd_write(lcd_t* lcd, const uint8_t val, const uint8_t mode);
int lcd_print(struct lcd* lcd, const char* text, const size_t text_len, uint8_t line);
int lcd_clear(lcd_t* lcd);
