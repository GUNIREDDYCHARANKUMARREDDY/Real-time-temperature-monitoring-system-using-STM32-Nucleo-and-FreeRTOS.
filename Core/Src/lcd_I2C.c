#include "lcd_i2c.h"
#define RS 0x01
#define EN 0x04
#define BL 0x08

static void lcd_send_internal(uint8_t data, uint8_t flags)
{
    uint8_t up, lo;
    uint8_t data_arr[4];

    up = data & 0xF0;
    lo = (data << 4) & 0xF0;

    data_arr[0] = up | flags | EN | BL;
    data_arr[1] = up | flags | BL;
    data_arr[2] = lo | flags | EN | BL;
    data_arr[3] = lo | flags | BL;

    HAL_I2C_Master_Transmit(&hi2c1,
                            LCD_ADDR,
                            data_arr,
                            4,
                            HAL_MAX_DELAY);
}

void lcd_send_cmd(char cmd)
{
    lcd_send_internal(cmd, 0);
}

void lcd_send_data(char data)
{
    lcd_send_internal(data, RS);
}

void lcd_clear(void)
{
    lcd_send_cmd(0x01);
    HAL_Delay(2);
}

void lcd_put_cur(int row, int col)
{
    uint8_t pos;

    switch(row)
    {
        case 0:
            pos = 0x80 + col;
            break;

        case 1:
            pos = 0xC0 + col;
            break;

        default:
            pos = 0x80 + col;
    }

    lcd_send_cmd(pos);
}

void lcd_send_string(char *str)
{
    while(*str)
    {
        lcd_send_data(*str++);
    }
}

void lcd_init(void)
{
    HAL_Delay(50);

    lcd_send_cmd(0x30);
    HAL_Delay(5);

    lcd_send_cmd(0x30);
    HAL_Delay(1);

    lcd_send_cmd(0x30);
    HAL_Delay(10);

    lcd_send_cmd(0x20);
    HAL_Delay(10);

    lcd_send_cmd(0x28); // 4-bit, 2 line
    HAL_Delay(1);

    lcd_send_cmd(0x08); // display off
    HAL_Delay(1);

    lcd_send_cmd(0x01); // clear
    HAL_Delay(2);

    lcd_send_cmd(0x06); // entry mode
    HAL_Delay(1);

    lcd_send_cmd(0x0C); // display on
    HAL_Delay(1);
}
