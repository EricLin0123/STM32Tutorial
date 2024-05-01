#ifndef __SCREEN_H
#define __SCREEN_H

#include "main.h"

#define SSD1306_I2C_ADDR (uint8_t)0x3C << 1

#define MAX_COLUMN 128
#define SSD1306_HEIGHT 128
#define MAX_ROW 64
extern const unsigned char F6x8[][6];
extern const unsigned char F8X16[];
extern const char Hzk[][32];
extern const char Hzk2[][16];
extern const char Hzk3[][16];
extern const char imu_title[][16];
extern const char auth_name1[][32];
extern const char auth_name2[][128];
typedef uint8_t uint8;
void SSD1306_WriteCmd(uint8 cmd);
void SSD1306_WriteData(uint8 data);
void OLED_Init(void);
void OLED_DisplayTest(void);
void OLED_SetPos(uint8 x, uint8 y);
void OLED_DisplayOn(void);
void OLED_DisplayOff(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8 x, uint8 y, uint8 chr, uint8 Char_Size);
void OLED_ShowString(uint8 x, uint8 y, char *str, uint8 Char_Size);
void OLED_ShowCN(uint8 x, uint8 y, uint8 index, const char (*matrix)[32]);
void OLED_ShowEN(uint8 x, uint8 y, uint8 index, const char (*matrix)[16]);
void OLED_ShowCN_Big(uint8 x, uint8 y, uint8 index, const char (*matrix)[128]);
void OLED_ShowName(uint8 x, uint8 y, const char (*matrix)[32]);

#endif /* __SCREEN_H */
