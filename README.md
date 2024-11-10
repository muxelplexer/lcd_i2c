# i2c_lcd - platform agnostic LCD2004 library

This library was written in an effort to make setup and interfacing of HD44780
displays through a LCD2004 i2c board easier.

This library has been tested with Zephyr on a STM32H563ZI Nucleo board.

## Usage

When instantiating the `lcd_t` structure you will need to fill it's `i2c_funcs`
member. In it is the function table and optional device reference for the
boards `i2c` interface. These will need to be proviced by the user of the library.
```c
struct lcd_i2c
{
    void* device;
    init_i2c_fn* init_fn;
    write_i2c_fn* write_fn;
    delay_us_i2c_fn* delay_fn;
};
```

### Usage with zephyr

When using zephyr you will need to link the library with the `zephyr_interface`
target. This makes sure that the same compilation and linking options are used.

```cmake
target_link_libraries(lcd_i2c
    PUBLIC zephyr_interface
)
```

## Known Issues

- Not compatible with non 2-line mode.
- Likely incompatible with 16x2 interfaces.
- Unable to upload custom character graphs.
- Timing is not adjusted to datasheet yet.
- Unable to adjust writing speed or other LCD settings.

## License

See `LICENSE` for information about licensing.
