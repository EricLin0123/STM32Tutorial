// Copyright statement: This article is an original article by CSDN blogger "freemote" and follows the CC 4.0 BY - SA copyright agreement.
// Link：https: // blog.csdn.net/freemote/article/details/102584923
#include "i2c.h"
#include "screen.h"
const unsigned char F6x8[][6];
const unsigned char F8X16[];
const char Hzk[][32];

void SSD1306_WriteCmd(uint8_t cmd)
{
    // control byte: Co D/C# 0 0 0 0 0 0
    uint8_t command[2];
    command[0] = 0x00;
    command[1] = cmd;
    HAL_I2C_Master_Transmit(&hi2c1, SSD1306_I2C_ADDR, &command[0], 2, 500);
}

void SSD1306_WriteData(uint8_t data)
{
    uint8_t datab[2];
    datab[0] = 0x40;
    datab[1] = data;
    HAL_I2C_Master_Transmit(&hi2c1, SSD1306_I2C_ADDR, &datab[0], 2, 500);
}

void OLED_Init(void)
{
    SSD1306_WriteCmd(0xAE); //--display off

    SSD1306_WriteCmd(0xD5); // set display clock divide ratio/oscillator frequency
    SSD1306_WriteCmd(0x80); //

#if (SSD1306_HEIGHT == 128)
    // Found in the Luma Python lib for SH1106.
    SSD1306_WriteCmd(0xFF);
#else
    SSD1306_WriteCmd(0xA8); //--set multiplex ratio(1 to 64) - CHECK
#endif

#if (SSD1306_HEIGHT == 32)
    SSD1306_WriteCmd(0x1F); //
#elif (SSD1306_HEIGHT == 64)
    SSD1306_WriteCmd(0x3F); //
#elif (SSD1306_HEIGHT == 128)
    SSD1306_WriteCmd(0x3F); // Seems to work for 128px high displays too.
#else
#error "Only 32, 64, or 128 lines of height are supported!"
#endif

    SSD1306_WriteCmd(0xD3); // set display offset
    SSD1306_WriteCmd(0x00); // no offset

    SSD1306_WriteCmd(0x40); //--set start line address

    SSD1306_WriteCmd(0x8D); // set charge pump enable
    SSD1306_WriteCmd(0x14); //

    SSD1306_WriteCmd(0xA1); // set segment re-map  0 to 127

    SSD1306_WriteCmd(0xC8); // Com scan direction

    SSD1306_WriteCmd(0xDA); // set com pin  hardware configuartion
#if (SSD1306_HEIGHT == 32)
    SSD1306_WriteCmd(0x02);
#elif (SSD1306_HEIGHT == 64)
    SSD1306_WriteCmd(0x12);
#elif (SSD1306_HEIGHT == 128)
    SSD1306_WriteCmd(0x12);
#else
#error "Only 32, 64, or 128 lines of height are supported!"
#endif

    SSD1306_WriteCmd(0x81); // contract control
    SSD1306_WriteCmd(0xFF); //--128

    SSD1306_WriteCmd(0x00); //--set low column address
    SSD1306_WriteCmd(0x10); //--set high column address

    SSD1306_WriteCmd(0xB0); //--set page address

    SSD1306_WriteCmd(0xD9); // Set Pre-Charge Period
    SSD1306_WriteCmd(0xF1); //

    SSD1306_WriteCmd(0xDB); // set Vcomh
    SSD1306_WriteCmd(0x30); // 0x20,0.77xVcc

    SSD1306_WriteCmd(0xD8); // set area color mode off
    SSD1306_WriteCmd(0x05); //

    SSD1306_WriteCmd(0xA6); // set normal  display
    HAL_Delay(100);

    SSD1306_WriteCmd(0xAF); //--turn on oled panel
}
typedef unsigned char uint8;

void OLED_SetPos(uint8 x, uint8 y)
{
    SSD1306_WriteCmd(0xb0 + y);                 // page addr     0xb0~0xb7
    SSD1306_WriteCmd(((x & 0xf0) >> 4) | 0x10); // high side addr
    SSD1306_WriteCmd((x & 0x0f));               // low side addr
}

void OLED_DisplayOn(void)
{
    SSD1306_WriteCmd(0X8D); // SET DCDC
    SSD1306_WriteCmd(0X14); // DCDC ON
    SSD1306_WriteCmd(0XAF); // DISPLAY ON
}

void OLED_DisplayOff(void)
{
    SSD1306_WriteCmd(0X8D); // SET DCDC
    SSD1306_WriteCmd(0X10); // DCDC OFF
    SSD1306_WriteCmd(0XAE); // DISPLAY OFF
}

void OLED_Clear(void)
{
    uint8 i, n;

    for (i = 0; i < 8; i++)
    {
        SSD1306_WriteCmd(0xb0 + i); // page addr（0~7）
        SSD1306_WriteCmd(0x00);     // set low side addr
        SSD1306_WriteCmd(0x10);     // set high side addr
        for (n = 0; n < 128; n++)
            SSD1306_WriteData(0);
    }
}

void OLED_ShowChar(uint8 x, uint8 y, uint8 chr, uint8 Char_Size)
{
    uint8 c = 0, i = 0;

    c = chr - ' ';
    if (x > MAX_COLUMN - 1)
    {
        x = 0;
        y = y + 2;
    }
    if (Char_Size == 16)
    {
        OLED_SetPos(x, y);
        for (i = 0; i < 8; i++)
        {
            SSD1306_WriteData(F8X16[c * 16 + i]); // upper half
        }

        OLED_SetPos(x, y + 1);
        for (i = 0; i < 8; i++)
        {
            SSD1306_WriteData(F8X16[c * 16 + i + 8]); // lower half
        }
    }
    else
    {
        OLED_SetPos(x, y);
        for (i = 0; i < 6; i++)
        {
            SSD1306_WriteData(F6x8[c][i]);
        }
    }
}

void OLED_ShowString(uint8 x, uint8 y, char *str, uint8 Char_Size)
{
    unsigned char j = 0;

    while (str[j] != '\0')
    {
        OLED_ShowChar(x, y, str[j], Char_Size);
        x += 8;
        if (x > 120)
        {
            x = 0;
            y += 2;
        }
        j++; // next page
    }
}

void OLED_ShowCN(uint8 x, uint8 y, uint8 index, const char (*matrix)[32])
{
    uint8 t;

    OLED_SetPos(x, y);
    for (t = 0; t < 16; t++)
    {
        SSD1306_WriteData(matrix[index][t]);
    }

    OLED_SetPos(x, y + 1);
    for (t = 0; t < 16; t++)
    {
        SSD1306_WriteData(matrix[index][t + 16]);
    }
}

void OLED_ShowName(uint8 x, uint8 y, const char (*matrix)[32])
{
    for (int i = 0; i < 3; i++)
    {
        uint8 t;

        OLED_SetPos(x + 16 * i, y);
        for (t = 0; t < 16; t++)
        {
            SSD1306_WriteData(matrix[i][t]);
        }

        OLED_SetPos(x + 16 * i, y + 1);
        for (t = 0; t < 16; t++)
        {
            SSD1306_WriteData(matrix[i][t + 16]);
        }
    }
}

void OLED_ShowCN_Big(uint8 x, uint8 y, uint8 index, const char (*matrix)[128])
{
    uint8 t;

    OLED_SetPos(x, y);
    for (t = 0; t < 32; t++)
    {
        SSD1306_WriteData(matrix[index][t]);
    }

    OLED_SetPos(x, y + 1);
    for (t = 0; t < 32; t++)
    {
        SSD1306_WriteData(matrix[index][t + 32]);
    }

    OLED_SetPos(x, y + 2);
    for (t = 0; t < 32; t++)
    {
        SSD1306_WriteData(matrix[index][t + 64]);
    }

    OLED_SetPos(x, y + 3);
    for (t = 0; t < 32; t++)
    {
        SSD1306_WriteData(matrix[index][t + 96]);
    }
}

void OLED_ShowEN(uint8 x, uint8 y, uint8 index, const char (*matrix)[16])
{
    uint8 t;

    OLED_SetPos(x, y);
    for (t = 0; t < 8; t++)
    {
        SSD1306_WriteData(matrix[index][t]);
    }

    OLED_SetPos(x, y + 1);
    for (t = 0; t < 8; t++)
    {
        SSD1306_WriteData(matrix[index][t + 8]);
    }
}

void OLED_DisplayTest(void)
{
    OLED_Clear();

    OLED_ShowCN(40, 0, 0, Hzk);
    OLED_ShowCN(56, 0, 1, Hzk);
    OLED_ShowCN(72, 0, 2, Hzk);

    OLED_ShowCN(24, 2, 3, Hzk);
    OLED_ShowCN(40, 2, 4, Hzk);
    OLED_ShowCN(56, 2, 5, Hzk);
    OLED_ShowCN(72, 2, 6, Hzk);
    OLED_ShowCN(88, 2, 7, Hzk);

    OLED_ShowString(20, 4, (char *)("This is Eric"), 16);
    OLED_ShowString(0, 6, (char *)("esp32...IoT."), 16);
}

const unsigned char F6x8[][6] =
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // sp
        0x00, 0x00, 0x00, 0x2f, 0x00, 0x00, // !
        0x00, 0x00, 0x07, 0x00, 0x07, 0x00, // "
        0x00, 0x14, 0x7f, 0x14, 0x7f, 0x14, // #
        0x00, 0x24, 0x2a, 0x7f, 0x2a, 0x12, // $
        0x00, 0x62, 0x64, 0x08, 0x13, 0x23, // %
        0x00, 0x36, 0x49, 0x55, 0x22, 0x50, // &
        0x00, 0x00, 0x05, 0x03, 0x00, 0x00, // '
        0x00, 0x00, 0x1c, 0x22, 0x41, 0x00, // (
        0x00, 0x00, 0x41, 0x22, 0x1c, 0x00, // )
        0x00, 0x14, 0x08, 0x3E, 0x08, 0x14, // *
        0x00, 0x08, 0x08, 0x3E, 0x08, 0x08, // +
        0x00, 0x00, 0x00, 0xA0, 0x60, 0x00, // ,
        0x00, 0x08, 0x08, 0x08, 0x08, 0x08, // -
        0x00, 0x00, 0x60, 0x60, 0x00, 0x00, // .
        0x00, 0x20, 0x10, 0x08, 0x04, 0x02, // /
        0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E, // 0
        0x00, 0x00, 0x42, 0x7F, 0x40, 0x00, // 1
        0x00, 0x42, 0x61, 0x51, 0x49, 0x46, // 2
        0x00, 0x21, 0x41, 0x45, 0x4B, 0x31, // 3
        0x00, 0x18, 0x14, 0x12, 0x7F, 0x10, // 4
        0x00, 0x27, 0x45, 0x45, 0x45, 0x39, // 5
        0x00, 0x3C, 0x4A, 0x49, 0x49, 0x30, // 6
        0x00, 0x01, 0x71, 0x09, 0x05, 0x03, // 7
        0x00, 0x36, 0x49, 0x49, 0x49, 0x36, // 8
        0x00, 0x06, 0x49, 0x49, 0x29, 0x1E, // 9
        0x00, 0x00, 0x36, 0x36, 0x00, 0x00, // :
        0x00, 0x00, 0x56, 0x36, 0x00, 0x00, // ;
        0x00, 0x08, 0x14, 0x22, 0x41, 0x00, // <
        0x00, 0x14, 0x14, 0x14, 0x14, 0x14, // =
        0x00, 0x00, 0x41, 0x22, 0x14, 0x08, // >
        0x00, 0x02, 0x01, 0x51, 0x09, 0x06, // ?
        0x00, 0x32, 0x49, 0x59, 0x51, 0x3E, // @
        0x00, 0x7C, 0x12, 0x11, 0x12, 0x7C, // A
        0x00, 0x7F, 0x49, 0x49, 0x49, 0x36, // B
        0x00, 0x3E, 0x41, 0x41, 0x41, 0x22, // C
        0x00, 0x7F, 0x41, 0x41, 0x22, 0x1C, // D
        0x00, 0x7F, 0x49, 0x49, 0x49, 0x41, // E
        0x00, 0x7F, 0x09, 0x09, 0x09, 0x01, // F
        0x00, 0x3E, 0x41, 0x49, 0x49, 0x7A, // G
        0x00, 0x7F, 0x08, 0x08, 0x08, 0x7F, // H
        0x00, 0x00, 0x41, 0x7F, 0x41, 0x00, // I
        0x00, 0x20, 0x40, 0x41, 0x3F, 0x01, // J
        0x00, 0x7F, 0x08, 0x14, 0x22, 0x41, // K
        0x00, 0x7F, 0x40, 0x40, 0x40, 0x40, // L
        0x00, 0x7F, 0x02, 0x0C, 0x02, 0x7F, // M
        0x00, 0x7F, 0x04, 0x08, 0x10, 0x7F, // N
        0x00, 0x3E, 0x41, 0x41, 0x41, 0x3E, // O
        0x00, 0x7F, 0x09, 0x09, 0x09, 0x06, // P
        0x00, 0x3E, 0x41, 0x51, 0x21, 0x5E, // Q
        0x00, 0x7F, 0x09, 0x19, 0x29, 0x46, // R
        0x00, 0x46, 0x49, 0x49, 0x49, 0x31, // S
        0x00, 0x01, 0x01, 0x7F, 0x01, 0x01, // T
        0x00, 0x3F, 0x40, 0x40, 0x40, 0x3F, // U
        0x00, 0x1F, 0x20, 0x40, 0x20, 0x1F, // V
        0x00, 0x3F, 0x40, 0x38, 0x40, 0x3F, // W
        0x00, 0x63, 0x14, 0x08, 0x14, 0x63, // X
        0x00, 0x07, 0x08, 0x70, 0x08, 0x07, // Y
        0x00, 0x61, 0x51, 0x49, 0x45, 0x43, // Z
        0x00, 0x00, 0x7F, 0x41, 0x41, 0x00, // [
        0x00, 0x55, 0x2A, 0x55, 0x2A, 0x55, // 55
        0x00, 0x00, 0x41, 0x41, 0x7F, 0x00, // ]
        0x00, 0x04, 0x02, 0x01, 0x02, 0x04, // ^
        0x00, 0x40, 0x40, 0x40, 0x40, 0x40, // _
        0x00, 0x00, 0x01, 0x02, 0x04, 0x00, // '
        0x00, 0x20, 0x54, 0x54, 0x54, 0x78, // a
        0x00, 0x7F, 0x48, 0x44, 0x44, 0x38, // b
        0x00, 0x38, 0x44, 0x44, 0x44, 0x20, // c
        0x00, 0x38, 0x44, 0x44, 0x48, 0x7F, // d
        0x00, 0x38, 0x54, 0x54, 0x54, 0x18, // e
        0x00, 0x08, 0x7E, 0x09, 0x01, 0x02, // f
        0x00, 0x18, 0xA4, 0xA4, 0xA4, 0x7C, // g
        0x00, 0x7F, 0x08, 0x04, 0x04, 0x78, // h
        0x00, 0x00, 0x44, 0x7D, 0x40, 0x00, // i
        0x00, 0x40, 0x80, 0x84, 0x7D, 0x00, // j
        0x00, 0x7F, 0x10, 0x28, 0x44, 0x00, // k
        0x00, 0x00, 0x41, 0x7F, 0x40, 0x00, // l
        0x00, 0x7C, 0x04, 0x18, 0x04, 0x78, // m
        0x00, 0x7C, 0x08, 0x04, 0x04, 0x78, // n
        0x00, 0x38, 0x44, 0x44, 0x44, 0x38, // o
        0x00, 0xFC, 0x24, 0x24, 0x24, 0x18, // p
        0x00, 0x18, 0x24, 0x24, 0x18, 0xFC, // q
        0x00, 0x7C, 0x08, 0x04, 0x04, 0x08, // r
        0x00, 0x48, 0x54, 0x54, 0x54, 0x20, // s
        0x00, 0x04, 0x3F, 0x44, 0x40, 0x20, // t
        0x00, 0x3C, 0x40, 0x40, 0x20, 0x7C, // u
        0x00, 0x1C, 0x20, 0x40, 0x20, 0x1C, // v
        0x00, 0x3C, 0x40, 0x30, 0x40, 0x3C, // w
        0x00, 0x44, 0x28, 0x10, 0x28, 0x44, // x
        0x00, 0x1C, 0xA0, 0xA0, 0xA0, 0x7C, // y
        0x00, 0x44, 0x64, 0x54, 0x4C, 0x44, // z
        0x14, 0x14, 0x14, 0x14, 0x14, 0x14, // horiz lines
};

const unsigned char F8X16[] =
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //
        0x00, 0x00, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 0x30, 0x00, 0x00, 0x00, //!
        0x00, 0x10, 0x0C, 0x06, 0x10, 0x0C, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //"
        0x40, 0xC0, 0x78, 0x40, 0xC0, 0x78, 0x40, 0x00, 0x04, 0x3F, 0x04, 0x04, 0x3F, 0x04, 0x04, 0x00, // #
        0x00, 0x70, 0x88, 0xFC, 0x08, 0x30, 0x00, 0x00, 0x00, 0x18, 0x20, 0xFF, 0x21, 0x1E, 0x00, 0x00, //$
        0xF0, 0x08, 0xF0, 0x00, 0xE0, 0x18, 0x00, 0x00, 0x00, 0x21, 0x1C, 0x03, 0x1E, 0x21, 0x1E, 0x00, //%
        0x00, 0xF0, 0x08, 0x88, 0x70, 0x00, 0x00, 0x00, 0x1E, 0x21, 0x23, 0x24, 0x19, 0x27, 0x21, 0x10, //&
        0x10, 0x16, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //'
        0x00, 0x00, 0x00, 0xE0, 0x18, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00, 0x07, 0x18, 0x20, 0x40, 0x00, //(
        0x00, 0x02, 0x04, 0x18, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x40, 0x20, 0x18, 0x07, 0x00, 0x00, 0x00, //)
        0x40, 0x40, 0x80, 0xF0, 0x80, 0x40, 0x40, 0x00, 0x02, 0x02, 0x01, 0x0F, 0x01, 0x02, 0x02, 0x00, //*
        0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x1F, 0x01, 0x01, 0x01, 0x00, //+
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xB0, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, //,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, //-
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, //.
        0x00, 0x00, 0x00, 0x00, 0x80, 0x60, 0x18, 0x04, 0x00, 0x60, 0x18, 0x06, 0x01, 0x00, 0x00, 0x00, ///
        0x00, 0xE0, 0x10, 0x08, 0x08, 0x10, 0xE0, 0x00, 0x00, 0x0F, 0x10, 0x20, 0x20, 0x10, 0x0F, 0x00, // 0
        0x00, 0x10, 0x10, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x20, 0x3F, 0x20, 0x20, 0x00, 0x00, // 1
        0x00, 0x70, 0x08, 0x08, 0x08, 0x88, 0x70, 0x00, 0x00, 0x30, 0x28, 0x24, 0x22, 0x21, 0x30, 0x00, // 2
        0x00, 0x30, 0x08, 0x88, 0x88, 0x48, 0x30, 0x00, 0x00, 0x18, 0x20, 0x20, 0x20, 0x11, 0x0E, 0x00, // 3
        0x00, 0x00, 0xC0, 0x20, 0x10, 0xF8, 0x00, 0x00, 0x00, 0x07, 0x04, 0x24, 0x24, 0x3F, 0x24, 0x00, // 4
        0x00, 0xF8, 0x08, 0x88, 0x88, 0x08, 0x08, 0x00, 0x00, 0x19, 0x21, 0x20, 0x20, 0x11, 0x0E, 0x00, // 5
        0x00, 0xE0, 0x10, 0x88, 0x88, 0x18, 0x00, 0x00, 0x00, 0x0F, 0x11, 0x20, 0x20, 0x11, 0x0E, 0x00, // 6
        0x00, 0x38, 0x08, 0x08, 0xC8, 0x38, 0x08, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00, // 7
        0x00, 0x70, 0x88, 0x08, 0x08, 0x88, 0x70, 0x00, 0x00, 0x1C, 0x22, 0x21, 0x21, 0x22, 0x1C, 0x00, // 8
        0x00, 0xE0, 0x10, 0x08, 0x08, 0x10, 0xE0, 0x00, 0x00, 0x00, 0x31, 0x22, 0x22, 0x11, 0x0F, 0x00, // 9
        0x00, 0x00, 0x00, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x00, //:
        0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x60, 0x00, 0x00, 0x00, 0x00, //;
        0x00, 0x00, 0x80, 0x40, 0x20, 0x10, 0x08, 0x00, 0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x00, //<
        0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x00, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x00, //=
        0x00, 0x08, 0x10, 0x20, 0x40, 0x80, 0x00, 0x00, 0x00, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, 0x00, //>
        0x00, 0x70, 0x48, 0x08, 0x08, 0x08, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x30, 0x36, 0x01, 0x00, 0x00, //?
        0xC0, 0x30, 0xC8, 0x28, 0xE8, 0x10, 0xE0, 0x00, 0x07, 0x18, 0x27, 0x24, 0x23, 0x14, 0x0B, 0x00, //@
        0x00, 0x00, 0xC0, 0x38, 0xE0, 0x00, 0x00, 0x00, 0x20, 0x3C, 0x23, 0x02, 0x02, 0x27, 0x38, 0x20, // A
        0x08, 0xF8, 0x88, 0x88, 0x88, 0x70, 0x00, 0x00, 0x20, 0x3F, 0x20, 0x20, 0x20, 0x11, 0x0E, 0x00, // B
        0xC0, 0x30, 0x08, 0x08, 0x08, 0x08, 0x38, 0x00, 0x07, 0x18, 0x20, 0x20, 0x20, 0x10, 0x08, 0x00, // C
        0x08, 0xF8, 0x08, 0x08, 0x08, 0x10, 0xE0, 0x00, 0x20, 0x3F, 0x20, 0x20, 0x20, 0x10, 0x0F, 0x00, // D
        0x08, 0xF8, 0x88, 0x88, 0xE8, 0x08, 0x10, 0x00, 0x20, 0x3F, 0x20, 0x20, 0x23, 0x20, 0x18, 0x00, // E
        0x08, 0xF8, 0x88, 0x88, 0xE8, 0x08, 0x10, 0x00, 0x20, 0x3F, 0x20, 0x00, 0x03, 0x00, 0x00, 0x00, // F
        0xC0, 0x30, 0x08, 0x08, 0x08, 0x38, 0x00, 0x00, 0x07, 0x18, 0x20, 0x20, 0x22, 0x1E, 0x02, 0x00, // G
        0x08, 0xF8, 0x08, 0x00, 0x00, 0x08, 0xF8, 0x08, 0x20, 0x3F, 0x21, 0x01, 0x01, 0x21, 0x3F, 0x20, // H
        0x00, 0x08, 0x08, 0xF8, 0x08, 0x08, 0x00, 0x00, 0x00, 0x20, 0x20, 0x3F, 0x20, 0x20, 0x00, 0x00, // I
        0x00, 0x00, 0x08, 0x08, 0xF8, 0x08, 0x08, 0x00, 0xC0, 0x80, 0x80, 0x80, 0x7F, 0x00, 0x00, 0x00, // J
        0x08, 0xF8, 0x88, 0xC0, 0x28, 0x18, 0x08, 0x00, 0x20, 0x3F, 0x20, 0x01, 0x26, 0x38, 0x20, 0x00, // K
        0x08, 0xF8, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x3F, 0x20, 0x20, 0x20, 0x20, 0x30, 0x00, // L
        0x08, 0xF8, 0xF8, 0x00, 0xF8, 0xF8, 0x08, 0x00, 0x20, 0x3F, 0x00, 0x3F, 0x00, 0x3F, 0x20, 0x00, // M
        0x08, 0xF8, 0x30, 0xC0, 0x00, 0x08, 0xF8, 0x08, 0x20, 0x3F, 0x20, 0x00, 0x07, 0x18, 0x3F, 0x00, // N
        0xE0, 0x10, 0x08, 0x08, 0x08, 0x10, 0xE0, 0x00, 0x0F, 0x10, 0x20, 0x20, 0x20, 0x10, 0x0F, 0x00, // O
        0x08, 0xF8, 0x08, 0x08, 0x08, 0x08, 0xF0, 0x00, 0x20, 0x3F, 0x21, 0x01, 0x01, 0x01, 0x00, 0x00, // P
        0xE0, 0x10, 0x08, 0x08, 0x08, 0x10, 0xE0, 0x00, 0x0F, 0x18, 0x24, 0x24, 0x38, 0x50, 0x4F, 0x00, // Q
        0x08, 0xF8, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00, 0x20, 0x3F, 0x20, 0x00, 0x03, 0x0C, 0x30, 0x20, // R
        0x00, 0x70, 0x88, 0x08, 0x08, 0x08, 0x38, 0x00, 0x00, 0x38, 0x20, 0x21, 0x21, 0x22, 0x1C, 0x00, // S
        0x18, 0x08, 0x08, 0xF8, 0x08, 0x08, 0x18, 0x00, 0x00, 0x00, 0x20, 0x3F, 0x20, 0x00, 0x00, 0x00, // T
        0x08, 0xF8, 0x08, 0x00, 0x00, 0x08, 0xF8, 0x08, 0x00, 0x1F, 0x20, 0x20, 0x20, 0x20, 0x1F, 0x00, // U
        0x08, 0x78, 0x88, 0x00, 0x00, 0xC8, 0x38, 0x08, 0x00, 0x00, 0x07, 0x38, 0x0E, 0x01, 0x00, 0x00, // V
        0xF8, 0x08, 0x00, 0xF8, 0x00, 0x08, 0xF8, 0x00, 0x03, 0x3C, 0x07, 0x00, 0x07, 0x3C, 0x03, 0x00, // W
        0x08, 0x18, 0x68, 0x80, 0x80, 0x68, 0x18, 0x08, 0x20, 0x30, 0x2C, 0x03, 0x03, 0x2C, 0x30, 0x20, // X
        0x08, 0x38, 0xC8, 0x00, 0xC8, 0x38, 0x08, 0x00, 0x00, 0x00, 0x20, 0x3F, 0x20, 0x00, 0x00, 0x00, // Y
        0x10, 0x08, 0x08, 0x08, 0xC8, 0x38, 0x08, 0x00, 0x20, 0x38, 0x26, 0x21, 0x20, 0x20, 0x18, 0x00, // Z
        0x00, 0x00, 0x00, 0xFE, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x40, 0x40, 0x40, 0x00, //[
        0x00, 0x0C, 0x30, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x06, 0x38, 0xC0, 0x00, //\ 
  0x00,0x02,0x02,0x02,0xFE,0x00,0x00,0x00,0x00,0x40,0x40,0x40,0x7F,0x00,0x00,0x00,//]
        0x00, 0x00, 0x04, 0x02, 0x02, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //^
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, //_
        0x00, 0x02, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //`
        0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x19, 0x24, 0x22, 0x22, 0x22, 0x3F, 0x20, // a
        0x08, 0xF8, 0x00, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x11, 0x20, 0x20, 0x11, 0x0E, 0x00, // b
        0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x0E, 0x11, 0x20, 0x20, 0x20, 0x11, 0x00, // c
        0x00, 0x00, 0x00, 0x80, 0x80, 0x88, 0xF8, 0x00, 0x00, 0x0E, 0x11, 0x20, 0x20, 0x10, 0x3F, 0x20, // d
        0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x1F, 0x22, 0x22, 0x22, 0x22, 0x13, 0x00, // e
        0x00, 0x80, 0x80, 0xF0, 0x88, 0x88, 0x88, 0x18, 0x00, 0x20, 0x20, 0x3F, 0x20, 0x20, 0x00, 0x00, // f
        0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x6B, 0x94, 0x94, 0x94, 0x93, 0x60, 0x00, // g
        0x08, 0xF8, 0x00, 0x80, 0x80, 0x80, 0x00, 0x00, 0x20, 0x3F, 0x21, 0x00, 0x00, 0x20, 0x3F, 0x20, // h
        0x00, 0x80, 0x98, 0x98, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x20, 0x3F, 0x20, 0x20, 0x00, 0x00, // i
        0x00, 0x00, 0x00, 0x80, 0x98, 0x98, 0x00, 0x00, 0x00, 0xC0, 0x80, 0x80, 0x80, 0x7F, 0x00, 0x00, // j
        0x08, 0xF8, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0x20, 0x3F, 0x24, 0x02, 0x2D, 0x30, 0x20, 0x00, // k
        0x00, 0x08, 0x08, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x20, 0x3F, 0x20, 0x20, 0x00, 0x00, // l
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x20, 0x3F, 0x20, 0x00, 0x3F, 0x20, 0x00, 0x3F, // m
        0x80, 0x80, 0x00, 0x80, 0x80, 0x80, 0x00, 0x00, 0x20, 0x3F, 0x21, 0x00, 0x00, 0x20, 0x3F, 0x20, // n
        0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x1F, 0x20, 0x20, 0x20, 0x20, 0x1F, 0x00, // o
        0x80, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x00, 0x80, 0xFF, 0xA1, 0x20, 0x20, 0x11, 0x0E, 0x00, // p
        0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x0E, 0x11, 0x20, 0x20, 0xA0, 0xFF, 0x80, // q
        0x80, 0x80, 0x80, 0x00, 0x80, 0x80, 0x80, 0x00, 0x20, 0x20, 0x3F, 0x21, 0x20, 0x00, 0x01, 0x00, // r
        0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x33, 0x24, 0x24, 0x24, 0x24, 0x19, 0x00, // s
        0x00, 0x80, 0x80, 0xE0, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x20, 0x20, 0x00, 0x00, // t
        0x80, 0x80, 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x1F, 0x20, 0x20, 0x20, 0x10, 0x3F, 0x20, // u
        0x80, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0x01, 0x0E, 0x30, 0x08, 0x06, 0x01, 0x00, // v
        0x80, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x80, 0x0F, 0x30, 0x0C, 0x03, 0x0C, 0x30, 0x0F, 0x00, // w
        0x00, 0x80, 0x80, 0x00, 0x80, 0x80, 0x80, 0x00, 0x00, 0x20, 0x31, 0x2E, 0x0E, 0x31, 0x20, 0x00, // x
        0x80, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x81, 0x8E, 0x70, 0x18, 0x06, 0x01, 0x00, // y
        0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x21, 0x30, 0x2C, 0x22, 0x21, 0x30, 0x00, // z
        0x00, 0x00, 0x00, 0x00, 0x80, 0x7C, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x40, 0x40, //{
        0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, //|
        0x00, 0x02, 0x02, 0x7C, 0x80, 0x00, 0x00, 0x00, 0x00, 0x40, 0x40, 0x3F, 0x00, 0x00, 0x00, 0x00, //}
        0x00, 0x06, 0x01, 0x01, 0x02, 0x02, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //~
};

const char Hzk[][32] =
    {
        {0x00, 0x80, 0x40, 0x20, 0x18, 0x06, 0x80, 0x00, 0x07, 0x18, 0x20, 0x40, 0x80, 0x00, 0x00, 0x00,
         0x01, 0x00, 0x20, 0x70, 0x28, 0x26, 0x21, 0x20, 0x20, 0x24, 0x38, 0x60, 0x00, 0x01, 0x01, 0x00}, /*"公",0*/

        {0x80, 0x80, 0x40, 0x20, 0xD0, 0x08, 0x04, 0x03, 0x04, 0x08, 0xD0, 0x20, 0x40, 0x80, 0x80, 0x00,
         0x80, 0x40, 0x20, 0x18, 0x07, 0x08, 0xB0, 0x40, 0x20, 0x18, 0x07, 0x18, 0x20, 0x40, 0x80, 0x00}, /*"众",1*/

        {0x80, 0x80, 0x80, 0xBE, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xBE, 0x80, 0x80, 0x80, 0x00,
         0x00, 0x00, 0x00, 0x06, 0x05, 0x04, 0x04, 0x04, 0x44, 0x84, 0x44, 0x3C, 0x00, 0x00, 0x00, 0x00}, /*"号",2*/

        {0x40, 0x3C, 0x10, 0xFF, 0x10, 0x10, 0x20, 0x10, 0x8F, 0x78, 0x08, 0xF8, 0x08, 0xF8, 0x00, 0x00,
         0x02, 0x06, 0x02, 0xFF, 0x01, 0x01, 0x04, 0x42, 0x21, 0x18, 0x46, 0x81, 0x40, 0x3F, 0x00, 0x00}, /*"物",0*/

        {0x02, 0xFE, 0x92, 0x92, 0xFE, 0x02, 0x00, 0x10, 0x11, 0x16, 0xF0, 0x14, 0x13, 0x10, 0x00, 0x00,
         0x10, 0x1F, 0x08, 0x08, 0xFF, 0x04, 0x81, 0x41, 0x31, 0x0D, 0x03, 0x0D, 0x31, 0x41, 0x81, 0x00}, /*"联",1*/

        {0x00, 0xFE, 0x02, 0x22, 0x42, 0x82, 0x72, 0x02, 0x22, 0x42, 0x82, 0x72, 0x02, 0xFE, 0x00, 0x00,
         0x00, 0xFF, 0x10, 0x08, 0x06, 0x01, 0x0E, 0x10, 0x08, 0x06, 0x01, 0x4E, 0x80, 0x7F, 0x00, 0x00}, /*"网",2*/

        {0x00, 0x00, 0xFE, 0x92, 0x92, 0x92, 0x92, 0xFE, 0x92, 0x92, 0x92, 0x92, 0xFE, 0x00, 0x00, 0x00,
         0x40, 0x38, 0x01, 0x00, 0x3C, 0x40, 0x40, 0x42, 0x4C, 0x40, 0x40, 0x70, 0x05, 0x08, 0x30, 0x00}, /*"思",0*/

        {0x20, 0x20, 0x24, 0x24, 0x24, 0x24, 0xBF, 0x64, 0x24, 0x34, 0x28, 0x24, 0x22, 0x20, 0x20, 0x00,
         0x10, 0x08, 0x04, 0x02, 0x01, 0x0D, 0x0B, 0x09, 0x49, 0x89, 0x49, 0x39, 0x01, 0x00, 0x00, 0x00}, /*"考",1*/

};

const char Hzk2[][16] = {
    {0x00, 0xF8, 0xF8, 0x88, 0x88, 0xF8, 0x70, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00}, /*"P",0*/

    {0x00, 0xF0, 0xF8, 0x08, 0x08, 0xF8, 0xF0, 0x00, 0x00, 0x07, 0x0F, 0x08, 0x08, 0x0F, 0x07, 0x00}, /*"O",1*/

    {0x00, 0xF8, 0xF8, 0x00, 0xC0, 0x00, 0xF8, 0xF8, 0x00, 0x01, 0x0F, 0x0E, 0x01, 0x0E, 0x0F, 0x01}, /*"W",2*/

    {0x00, 0xF8, 0xF8, 0x88, 0x88, 0x88, 0x08, 0x00, 0x00, 0x0F, 0x0F, 0x08, 0x08, 0x08, 0x08, 0x00}, /*"E",3*/

    {0x00, 0xF8, 0xF8, 0x88, 0x88, 0xF8, 0x70, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x01, 0x0F, 0x0E, 0x00}, /*"R",4*/

    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*" ",5*/

    {0x00, 0xF8, 0xF8, 0x20, 0xC0, 0x20, 0xF8, 0xF8, 0x00, 0x0F, 0x0F, 0x00, 0x01, 0x00, 0x0F, 0x0F}, /*"M",6*/

    {0x00, 0xF8, 0xF8, 0x88, 0x88, 0x88, 0x08, 0x00, 0x00, 0x0F, 0x0F, 0x08, 0x08, 0x08, 0x08, 0x00}, /*"E",7*/

    {0x00, 0x08, 0x08, 0xF8, 0xF8, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00, 0x00}, /*"T",8*/

    {0x00, 0xF8, 0xF8, 0x88, 0x88, 0x88, 0x08, 0x00, 0x00, 0x0F, 0x0F, 0x08, 0x08, 0x08, 0x08, 0x00}, /*"E",9*/

    {0x00, 0xF8, 0xF8, 0x88, 0x88, 0xF8, 0x70, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x01, 0x0F, 0x0E, 0x00}, /*"R",10*/

    {0x00, 0x00, 0x70, 0xF8, 0xF8, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0D, 0x0D, 0x00, 0x00, 0x00}, /*"!",11*/
};

const char Hzk3[][16] = {
    // I(0) M(1) U(2)(3) D(4) A(5) T(6) A(7)(8)(9)(10)(11)

    {0x00, 0x00, 0x08, 0xF8, 0xF8, 0x08, 0x00, 0x00, 0x00, 0x00, 0x08, 0x0F, 0x0F, 0x08, 0x00, 0x00}, /*"I",0*/

    {0x00, 0xF8, 0xF8, 0x20, 0xC0, 0x20, 0xF8, 0xF8, 0x00, 0x0F, 0x0F, 0x00, 0x01, 0x00, 0x0F, 0x0F}, /*"M",1*/

    {0x00, 0xF8, 0xF8, 0x00, 0x00, 0xF8, 0xF8, 0x00, 0x00, 0x07, 0x0F, 0x08, 0x08, 0x0F, 0x07, 0x00}, /*"U",2*/

    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*" ",3*/

    {0x00, 0xF8, 0xF8, 0x08, 0x18, 0xF0, 0xE0, 0x00, 0x00, 0x0F, 0x0F, 0x08, 0x0C, 0x07, 0x03, 0x00}, /*"D",4*/

    {0x00, 0xE0, 0xF0, 0x18, 0x18, 0xF0, 0xE0, 0x00, 0x00, 0x0F, 0x0F, 0x01, 0x01, 0x0F, 0x0F, 0x00}, /*"A",5*/

    {0x00, 0x08, 0x08, 0xF8, 0xF8, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00, 0x00}, /*"T",6*/

    {0x00, 0xE0, 0xF0, 0x18, 0x18, 0xF0, 0xE0, 0x00, 0x00, 0x0F, 0x0F, 0x01, 0x01, 0x0F, 0x0F, 0x00}, /*"A",7*/

    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*" ",8*/

    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*" ",9*/

    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*" ",10*/

    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /*" ",11*/
};

const char imu_title[][16] = {
    // A(0) C(1) C(2) E(3) L(4) E(5) R(6) O(7) M(8) E(9) T(10) E(11) R(12)

    {0x00, 0xE0, 0xF0, 0x18, 0x18, 0xF0, 0xE0, 0x00, 0x00, 0x0F, 0x0F, 0x01, 0x01, 0x0F, 0x0F, 0x00}, /*"A",0*/

    {0x00, 0xF0, 0xF8, 0x08, 0x08, 0x38, 0x30, 0x00, 0x00, 0x07, 0x0F, 0x08, 0x08, 0x0E, 0x06, 0x00}, /*"C",1*/

    {0x00, 0xF0, 0xF8, 0x08, 0x08, 0x38, 0x30, 0x00, 0x00, 0x07, 0x0F, 0x08, 0x08, 0x0E, 0x06, 0x00}, /*"C",2*/

    {0x00, 0xF8, 0xF8, 0x88, 0x88, 0x88, 0x08, 0x00, 0x00, 0x0F, 0x0F, 0x08, 0x08, 0x08, 0x08, 0x00}, /*"E",3*/

    {0x00, 0xF8, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x08, 0x08, 0x08, 0x08, 0x00}, /*"L",4*/

    {0x00, 0xF8, 0xF8, 0x88, 0x88, 0x88, 0x08, 0x00, 0x00, 0x0F, 0x0F, 0x08, 0x08, 0x08, 0x08, 0x00}, /*"E",5*/

    {0x00, 0xF8, 0xF8, 0x88, 0x88, 0xF8, 0x70, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x01, 0x0F, 0x0E, 0x00}, /*"R",6*/

    {0x00, 0xF0, 0xF8, 0x08, 0x08, 0xF8, 0xF0, 0x00, 0x00, 0x07, 0x0F, 0x08, 0x08, 0x0F, 0x07, 0x00}, /*"O",7*/

    {0x00, 0xF8, 0xF8, 0x20, 0xC0, 0x20, 0xF8, 0xF8, 0x00, 0x0F, 0x0F, 0x00, 0x01, 0x00, 0x0F, 0x0F}, /*"M",8*/

    {0x00, 0xF8, 0xF8, 0x88, 0x88, 0x88, 0x08, 0x00, 0x00, 0x0F, 0x0F, 0x08, 0x08, 0x08, 0x08, 0x00}, /*"E",9*/

    {0x00, 0x08, 0x08, 0xF8, 0xF8, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00, 0x00}, /*"T",10*/

    {0x00, 0xF8, 0xF8, 0x88, 0x88, 0x88, 0x08, 0x00, 0x00, 0x0F, 0x0F, 0x08, 0x08, 0x08, 0x08, 0x00}, /*"E",11*/

    {0x00, 0xF8, 0xF8, 0x88, 0x88, 0xF8, 0x70, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x01, 0x0F, 0x0E, 0x00}, /*"R",12*/
};

const char auth_name1[][32] = {
    /* Microsoft JhengHei UI [δָ���ֺ�] ����[-1]*/
    /* �L[0xAA4C]   16x16 */
    {0x00, 0x10, 0x10, 0xD0, 0xFE, 0x50, 0x90, 0x00, 0x10, 0x10, 0xD0, 0xFE, 0x90, 0x10, 0x10, 0x00,
     0x00, 0x0C, 0x03, 0x00, 0xFF, 0x00, 0x01, 0x12, 0x0C, 0x03, 0x00, 0xFF, 0x01, 0x06, 0x18, 0x00},
    /* Microsoft JhengHei UI [δָ���ֺ�] ����[-1]*/
    /* �U[0xB855]   16x16 */
    {0x04, 0x04, 0xE4, 0xA4, 0xAE, 0xA4, 0xA4, 0xE0, 0xA4, 0xA4, 0xAE, 0xA4, 0xE4, 0x04, 0x04, 0x00,
     0x08, 0xF8, 0x0B, 0x4A, 0x4A, 0x4A, 0x4A, 0x7F, 0x2A, 0x3A, 0x2A, 0xCA, 0x8B, 0xF8, 0x00, 0x00},
    /* Microsoft JhengHei UI [δָ���ֺ�] ����[-1]*/
    /* ��[0xAFFE]   16x16 */
    {0x00, 0x04, 0x04, 0x04, 0x24, 0xAE, 0x64, 0x24, 0x24, 0x44, 0x8E, 0x04, 0x04, 0x04, 0x04, 0x00,
     0x00, 0x82, 0x92, 0x91, 0x93, 0x92, 0x92, 0x92, 0xFE, 0x92, 0x92, 0x93, 0x93, 0x92, 0x82, 0x00}};

const char auth_name2[][128] = {

    /* �L[0xAA4C]   32x32 */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xF0, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0xF8, 0xF8, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0xD0, 0xFF, 0xFF, 0x08, 0x08, 0x08, 0x08, 0x10, 0x10,
     0x10, 0x10, 0xD8, 0xFF, 0xFF, 0xC8, 0x8C, 0x0C, 0x0C, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x80, 0x40, 0x60, 0x30, 0x1C, 0x0F, 0x03, 0xFF, 0xFF, 0x01, 0x83, 0x46, 0x60, 0x30, 0x18,
     0x0E, 0x07, 0x01, 0xFF, 0xFF, 0x00, 0x03, 0x06, 0x1C, 0x78, 0x78, 0x70, 0x60, 0x60, 0x40, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x1F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x0F, 0x3F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /* �з��� [δָ���ֺ�] ����[-1]*/
    /* �U[0xB855]   32x32 */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80, 0x80, 0xB0, 0xF0, 0x80, 0x80, 0x00, 0x00,
     0x00, 0x80, 0xC0, 0xFC, 0x7C, 0x58, 0x40, 0x60, 0x60, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x03, 0x09, 0x39, 0xF0, 0x4D, 0x4B, 0x48, 0xF8, 0xF8,
     0x48, 0x48, 0x6F, 0x45, 0xFC, 0xFC, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x40, 0x40, 0x70, 0xF0, 0x60, 0x20, 0x27, 0x22, 0x26, 0x22, 0xFF, 0xFF,
     0x12, 0x12, 0x52, 0xD2, 0x97, 0x11, 0x10, 0x18, 0xF8, 0xF0, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x1C, 0x00, 0x02, 0x06, 0x06, 0x02, 0x03, 0x01,
     0x01, 0x01, 0x01, 0x01, 0x0F, 0x13, 0x30, 0x70, 0x7F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /* �з��� [δָ���ֺ�] ����[-1]*/
    /* ��[0xAFFE]   32x32 */
    {0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x88, 0xF8, 0xF8, 0xC0, 0x40, 0x40, 0x00, 0x00, 0x40,
     0x40, 0xC0, 0xFC, 0x7C, 0x68, 0x60, 0x60, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x87, 0xC0, 0xE4, 0x74, 0x3C, 0x0C, 0x38,
     0x74, 0xE3, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x10, 0x10, 0x08, 0x04, 0x06, 0x03, 0x81, 0x8D, 0x8C, 0xCC, 0xFC, 0xFC, 0x44,
     0x46, 0x44, 0x01, 0x07, 0x0F, 0x0E, 0x1C, 0x1C, 0x1C, 0x18, 0x18, 0x10, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x08, 0x18, 0x18, 0x18, 0x18, 0x08, 0x08, 0x08, 0x08, 0x0F, 0x0F, 0x08,
     0x08, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
};