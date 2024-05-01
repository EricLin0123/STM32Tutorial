#ifndef _H_WS2182
#define _H_WS2182

#include "main.h"

#define ONE_PULSE (48)
#define ZERO_PULSE (27)
#define RESET_PULSE (40)
#define LED_DATA_LEN (24)
#define brightness (0)
#define rOffset (1)
#define gOffset (0)
#define bOffset (2)

static uint32_t Color(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

static uint8_t *pixels;
static uint16_t numLEDs = 4;
static uint16_t numBytes;

static uint16_t RGB_buffur[RESET_PULSE + 4 * LED_DATA_LEN] = {0};
static uint8_t RESET_buffur[RESET_PULSE] = {0};

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim);

void WS2812_RESET(void);

void ws2812_set_RGB(uint8_t R, uint8_t G, uint8_t B, uint16_t num);

void updateLength(uint16_t n);
void theaterChaseRainbow(uint8_t wait);
uint32_t Wheel(uint8_t WheelPos);
void setPixelColor(uint16_t n, uint32_t c);

void show(void);
void set_color(uint8_t R, uint8_t G, uint8_t B, uint16_t num);
void ws2812_test(void);
#endif // _H_WS2182
