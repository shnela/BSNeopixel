#include "Effect.h"
#include "Animation.h"


uint32_t Effect::hsvToRgb(uint16_t h, uint8_t s, uint8_t v)
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

uint32_t Effect::randomize()
{
  uint32_t h, color;
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      if (!(random(200) % 5)) {
        h = random(255);
        color = hsvToRgb(h, 250, 205);
        bs.setShelfColor(i, j, color);
      }
    }
  }
  bs.show();
}

inline bool Effect::make_step()
{
    if (millis() - last_step_time >= step_duration) {
        last_step_time = millis();
        return true;
    }
    return false;
}


/* equalizer */

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


/* rainbow */

void Rainbow::step(uint8_t data[]) {
    if (!make_step())
        return;
    uint16_t time = millis() >> 2;
    uint8_t box = 0;

    for (int col = 0; col < 5; col++) {
        for (int row = 0; row < 5; row++) {
            byte x = (time >> 2) - (box << 3);
            uint32_t color = hsvToRgb((uint32_t)x * 359 / 256, 255, 255);

            bs.setShelfColor(row, col, color);
            box++;
        }
    }

    bs.show();
}


/* animation */

void Animation::step(uint8_t data[]) {
    if (!make_step())
        return;
    static int animation_step = 0;
    ++animation_step %= NUMBER_OF_STEPS;

    for (int i = 0; i < bs.shelfRows; i++)
        for (int j = 0; j < bs.shelfCols; j++) {
            bs.setShelfColor(i, j, background_color);
            if ((ANIMATION[animation_step][j] >> (BSNeopixel::shelfRows - i - 1)) & 1)
                bs.setShelfColor(i, j, animation_color);
        }

    bs.show();
}


/* dots */

void Dots::step(uint8_t data[])
{
    if (!make_step())
        return;

    bs.colorWipe(bgColor);
    int number_of_dots = countDotsNumber(data);
    for (int i=0; i < number_of_dots; i++) {
        setRandomDot();
    }
    bs.show();
}

void Dots::setRandomDot()
{
    int col = random(bs.shelfCols);
    int row = random(bs.shelfRows);
    bs.setShelfColor(row, col, fgColor);
}

int Dots::countDotsNumber(uint8_t data[])
{
    return 4; //TODO: temporary solution
    int sum = 0;
    for (int i=0; i < 5 ; i++)
    {
        sum += data[i];
    }
    return sum / (39 * bs.shelfCols / MAX_DOTS) + 1;
}


/* snake */

void Snake::reset()
{
    snake_row[0] = 2;
    snake_col[0] = 2;
    for (int i = 1; i < max_len; i++) {
        snake_row[i] = -1;
        snake_row[i] = -1;
    }
}

void Snake::step(uint8_t data[])
{
    if (!data[5])
        return;

    bs.colorWipe(GREEN);
    bs.setShelfColor(snake_row[0], snake_col[0], bs.Color(255,0,0));
    for (int i = 1; i < max_len; i++) {
        if (snake_row[i] != -1) {
            bs.setShelfColor(snake_row[i], snake_col[i], RED);
        }
    }
    bs.show();

    shift();

    bool collision = true;
    int step = 0;
    const int MAX_STEPS = 20;
    while(collision) {
        if (step++ >= MAX_STEPS) {
            reset();
            break;
        }

        collision = false;
        int dx = 0, dy = 0;
        if (random(2)==0) {
            dx = random(2)*2 - 1;
        } else {
            dy = random(2)*2 - 1;
        }

        // uint8_t align = bs.shelfRows;
        // snake_row[0] = (snake_row[1] + dx + align) % align;
        // snake_col[0] = (snake_col[1] + dy + align) % align;
        snake_row[0] = snake_row[1] + dx;
        if (snake_row[0] < 0 || snake_row[0] > 4) {
            collision = true;
            continue;
        }
        snake_col[0] = snake_col[1] + dy;
        if (snake_col[0] < 0 || snake_col[0] > 4) {
            collision = true;
            continue;        
        }

        for (int i = 1; i < max_len; i++) {
            if (snake_row[i] != -1) {
                if (snake_row[0] == snake_row[i] && snake_col[0] == snake_col[i]) {
                    collision = true;
                }
            }
        }
    }
    delay(400);
}

void Snake::nextShelf(int8_t snake[])
{
    // snake[0] = ((random(2)*2 - 1) + snake[1] + max_len - 1)%max_len;
    int randx = random(2)*2 - 1;
    snake[0] = (randx + snake[1]);
    snake[0] = snake[0] + bs.shelfRows;
    snake[0] = snake[0] % bs.shelfRows;
}

void Snake::shift()
{
    for (int i = max_len-1; i > 0; i--) {
        snake_row[i] = snake_row[i-1];
        snake_col[i] = snake_col[i-1];
    }
}


void Blur::reset_shelf(int i, int j)
{
    struct BlurState * shelf_state = &bs_state[i][j];

    shelf_state->rising = 0;
    uint8_t min_br = 0;
    uint8_t max_br = random(10, 100);
    uint32_t color = hsvToRgb(h_lvl, s_lvl, min_br);

    shelf_state->min_brightness = min_br;
    shelf_state->max_brightness = max_br;
    shelf_state->current_v_lvl = min_br;
    bs.setShelfColor(i, j, color);
}


void Blur::step(uint8_t data[])
{
    if (!make_step())
        return;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            struct BlurState * shelf_state = &bs_state[i][j];

            bool rising = shelf_state->rising;
            uint8_t min_br = shelf_state->min_brightness;
            uint8_t max_br = shelf_state->max_brightness;
            uint8_t color_intensity = shelf_state->current_v_lvl;
            if (rising && color_intensity >= max_br - step_size) {
                /* max color value for cell rised, stop rising
                and chose min value for the cell to fall */
                shelf_state->rising = rising = 0;
                shelf_state->min_brightness = random(0, max_br - step_size);
            } else if (!rising && color_intensity <= min_br + step_size) {
                /* min color value for cell rised, start rising
                and chose max value for the cell to rise */
                shelf_state->rising = rising = 1;
                shelf_state->max_brightness = random(min_br + step_size, 255);
            }
            /* continue rising/falling choosen color */
            color_intensity += (shelf_state->rising ? 1 : -1) * step_size;
            uint32_t color = hsvToRgb(h_lvl, s_lvl, color_intensity);
            shelf_state->current_v_lvl = color_intensity;
            bs.setShelfColor(i, j, color);
        }
    }
    bs.show();
}


void SpaceConsole::step(uint8_t data[])
{
    if (!make_step())
        return;
    uint32_t h, color;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (!(random() % this->change_density)) {
                h = random(this->colors) * (250 / this->colors);
                color = hsvToRgb(h, 250, 205);
                bs.setShelfColor(i, j, color);
          }
        }
    }
    bs.show();
}


int get_all_effects(BSNeopixel &bs, Effect*** effects)
{
    const int number_of_effects = 5;
    *effects = new Effect*[number_of_effects];
    (*effects)[0] = new Rainbow(bs);
    (*effects)[1] = new Animation(bs);
    (*effects)[2] = new Dots(bs);
    (*effects)[3] = new Blur(bs);
    (*effects)[4] = new SpaceConsole(bs);
    return number_of_effects;
}
