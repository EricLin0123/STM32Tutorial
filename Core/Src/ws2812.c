#include "ws2812.h"
#include "tim.h"
#include <string.h>
#include <stdlib.h>
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2)
        HAL_TIM_PWM_Stop_DMA(&htim2, TIM_CHANNEL_1);
}

void ws2812_set_RGB(uint8_t R, uint8_t G, uint8_t B, uint16_t num)
{
    // skip the first N zeros of the reset signal
    uint16_t *p = (RGB_buffur + RESET_PULSE) + (num * LED_DATA_LEN);

    for (uint16_t i = 0; i < 8; i++)
    {
        p[i] = (G << i) & (0x80) ? ONE_PULSE : ZERO_PULSE;
        p[i + 8] = (R << i) & (0x80) ? ONE_PULSE : ZERO_PULSE;
        p[i + 16] = (B << i) & (0x80) ? ONE_PULSE : ZERO_PULSE;
    }
}

void ws2812_example(void)
{
    // #1.填充数组
    ws2812_set_RGB(0x22, 0x00, 0x00, 0);
    ws2812_set_RGB(0x00, 0x22, 0x00, 1);
    ws2812_set_RGB(0x00, 0x00, 0x22, 2);
    ws2812_set_RGB(0x22, 0x22, 0x22, 3);
    // #2.传输数据
    HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_1, (uint32_t *)RGB_buffur, RESET_PULSE + (4 * LED_DATA_LEN));
    HAL_Delay(300);
    // #3.延时：使效果可以被观察
    ws2812_set_RGB(0x22, 0x00, 0x00, 1);
    ws2812_set_RGB(0x00, 0x22, 0x00, 2);
    ws2812_set_RGB(0x00, 0x00, 0x22, 3);
    ws2812_set_RGB(0x22, 0x22, 0x22, 0);

    HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_1, (uint32_t *)RGB_buffur, RESET_PULSE + (4 * LED_DATA_LEN));
    HAL_Delay(300);
}

void ws2812_test(void)
{
    updateLength(numLEDs);
    set_color(0x22, 0x22, 0x22, 0);
    set_color(0x22, 0x00, 0x00, 1);
    set_color(0x00, 0x22, 0x00, 2);
    set_color(0x00, 0x00, 0x22, 3);
    HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_1, (uint32_t *)pixels, RESET_PULSE + (numLEDs * LED_DATA_LEN));
}

void updateLength(uint16_t n)
{
    free(pixels); // Free existing data (if any)

    // Allocate new data -- note: ALL PIXELS ARE CLEARED
    if ((pixels = (uint8_t *)malloc(RESET_PULSE + (n * LED_DATA_LEN))))
    {
        memset(pixels, 0, RESET_PULSE + (n * LED_DATA_LEN));
        numLEDs = n;
    }
    else
    {
        numLEDs = 0;
    }
}

void set_color(uint8_t R, uint8_t G, uint8_t B, uint16_t num)
{
    uint8_t *p = pixels + RESET_PULSE + (num * LED_DATA_LEN);
    for (uint16_t i = 0; i < 8; i++)
    {
        p[i] = (G << i) & (0x80) ? ONE_PULSE : ZERO_PULSE;
        p[i + 8] = (R << i) & (0x80) ? ONE_PULSE : ZERO_PULSE;
        p[i + 16] = (B << i) & (0x80) ? ONE_PULSE : ZERO_PULSE;
    }
}

void theaterChaseRainbow(uint8_t wait)
{
    for (int j = 0; j < 256; j++)
    { // cycle all 256 colors in the wheel
        for (int q = 0; q < 3; q++)
        {
            for (uint16_t i = 0; i < numLEDs; i = i + 3)
            {
                setPixelColor(i + q, Wheel((i + j) % 255)); // turn every third pixel on
            }
            show();

            HAL_Delay(wait);

            for (uint16_t i = 0; i < numLEDs; i = i + 3)
            {
                setPixelColor(i + q, 0); // turn every third pixel off
            }
        }
    }
}

uint32_t Wheel(uint8_t WheelPos)
{
    WheelPos = 255 - WheelPos;
    if (WheelPos < 85)
    {
        return Color(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    if (WheelPos < 170)
    {
        WheelPos -= 85;
        return Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
    WheelPos -= 170;
    return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void setPixelColor(uint16_t n, uint32_t c)
{
    if (n < numLEDs)
    {
        uint8_t *p, r = (uint8_t)(c >> 16), g = (uint8_t)(c >> 8), b = (uint8_t)c;
        if (brightness)
        { // See notes in setBrightness()
            r = (r * brightness) >> 8;
            g = (g * brightness) >> 8;
            b = (b * brightness) >> 8;
        }
        p = &pixels[n * 3];
        p[rOffset] = r;
        p[gOffset] = g;
        p[bOffset] = b;
    }
}

void show(void)
{
}