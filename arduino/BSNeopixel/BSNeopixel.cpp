#include "BSNeopixel.h"


uint8_t BSNeopixel::pins[5] = {5, 6, 7, 8, 9};

BSNeopixel::BSNeopixel(uint8_t* p)
{
  if (p != NULL) {
    for (int i = 0; i < shelfRows; i++) {
      pins[i] = p[i];
    }
  }

  strips = new Adafruit_NeoPixel*[shelfRows];
  for (int i = 0; i < shelfRows; i++) {
    strips[i] = new Adafruit_NeoPixel(shelfCols * shelfLEDs, pins[i], NEO_GRB + NEO_KHZ800);
  }
}

BSNeopixel::~BSNeopixel()
{
	for (int i = 0; i < shelfRows; i++) {
		delete strips[i];
	}
	delete[] strips;
}

void BSNeopixel::begin(void)
{
	for (int i = 0; i < shelfRows; i++) {
		strips[i]->begin();
	}
}

void BSNeopixel::show(void)
{
	for (int i = 0; i < shelfRows; i++) {
		strips[i]->show();
	}
}

void BSNeopixel::setShelfColor(uint8_t row, uint8_t col, uint32_t color)
{
	uint16_t firstLED = col * shelfLEDs;
	for (uint16_t n = firstLED; n < firstLED + shelfLEDs; n++) {
		strips[row]->setPixelColor(n, color);
	}
}

uint32_t BSNeopixel::getShelfColor(uint8_t row, uint8_t col)
{
	uint16_t firstLED = col * shelfLEDs;
    return strips[row]->getPixelColor(firstLED);
}

void BSNeopixel::setPixelColor(uint8_t r, uint8_t c, uint8_t p, uint32_t color)
{
	strips[r]->setPixelColor(c * shelfLEDs + p, color);
}

void BSNeopixel::setPixelColor(uint8_t r, uint8_t c, uint32_t color)
{
	strips[r]->setPixelColor(c, color);
}

void BSNeopixel::setRowHeight(uint8_t r, uint8_t h, uint32_t color)
{
	for (int i = 0; i < h; i++) {
		strips[r]->setPixelColor(i, color);
	}
}

void BSNeopixel::setBrightness(uint8_t level)
{
	for (int i = 0; i < shelfRows; i++) {
		strips[i]->setBrightness(level);
	}
}

void BSNeopixel::colorWipe(uint32_t color)
{
	for (int i = 0; i < shelfRows; i++) {
		setRowHeight(i, shelfCols * shelfLEDs, color);
	}
}

void BSNeopixel::clear()
{
	for (int i = 0; i < shelfRows; i++) {
		strips[i]->clear();
	}
}

uint32_t BSNeopixel::Color(uint8_t r, uint8_t g, uint8_t b)
{
	return Adafruit_NeoPixel::Color(r,g,b);
}

uint32_t BSNeopixel::Color(uint8_t rgb[])
{
	uint8_t r = rgb[0];
	uint8_t g = rgb[1];
	uint8_t b = rgb[2];
	return Adafruit_NeoPixel::Color(r,g,b);
}
