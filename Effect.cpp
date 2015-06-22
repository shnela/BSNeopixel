#include "Effect.h"

void Equalizer::step(uint8_t data[])
{
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            bs.setShelfColor(i, j, BLACK);
        }
    }

    for (int row = 0; row < 5; row++) {
        for (int i = 0; i < data[row]+1; i++) {
            // tricolors(row, i);
            gradient(row, i);
        }
    }

    bs.show();
}

void Equalizer::tricolors(int row, int i)
{
    uint32_t color = BLUE;
    if (i > 20) color = YELLOW;
    if (i > 31) color = RED; 
    bs.setPixelColor(row, i, color);
}

void Equalizer::gradient(int row, int i)
{
    uint8_t red = map(i, 0, 39, 0, 128);
    uint8_t green = map(i, 0, 39, 128, 0);
    bs.setPixelColor(row, i, bs.Color(red, green, 0));
}

void Rainbow::step(uint8_t data[]) {
    uint16_t time = millis() >> 2;
    uint8_t box = 0;

    for (int col = 0; col < 5; col++) {
        for (int row = 0; row < 5; row++) {
            byte x = (time >> 2) - (box << 3);
            uint32_t color = hsvToRgb((uint32_t)x * 359 / 256, 255, 255);

            Serial.print(row);
            Serial.print(" ");
            Serial.print(col);
            Serial.print(" ");
            Serial.println(color);
            bs.setShelfColor(row, col, color);
            box++;
        }
    }

    bs.show();
}


uint32_t Rainbow::hsvToRgb(uint16_t h, uint8_t s, uint8_t v)
{
    uint8_t f = (h % 60) * 255 / 60;
    uint8_t p = (255 - s) * (uint16_t)v / 255;
    uint8_t q = (255 - f * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t t = (255 - (255 - f) * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t r = 0, g = 0, b = 0;
    switch((h / 60) % 6){
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    }
    return bs.Color(r, g, b);
}
