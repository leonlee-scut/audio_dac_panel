#include "iwi_SPI_RS0010.h"

#include <stdlib.h>
#include "gpio.h"
#include "delay.h"

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif

#ifndef _clk_out
#define _clk_out() { pinWrite(_clk, LOW); pinWrite(_clk, HIGH); }
#endif

SPI_RS0010::SPI_RS0010()
{
    gfxFont = NULL;
    _cp437 = 1;
}

SPI_RS0010::~SPI_RS0010()
{
}

/**
  * @brief Configure the pins used by the software SPI interface.
  * @param miso Pin ID for the serial data input.
  * @param mosi Pin ID for the serial data output.
  * @param clk Pin ID for the serial clock.
  * @param cs Pin ID for chip select.
  */
void SPI_RS0010::initSPI(uint8_t miso, uint8_t mosi, uint8_t clk, uint8_t cs)
{
    _miso = miso;
    _mosi = mosi;
    _clk = clk;
    _cs = cs;

    pinMode(_miso, INPUT_PULLUP);
    pinMode(_mosi, OUTPUT);
    pinMode(_clk, OUTPUT);
    pinMode(_cs, OUTPUT);

    // normal state of these pins should be high. We must bring them high or the first
    // command will not be captured by the display module.
    pinWrite(_cs, HIGH);
    pinWrite(_clk, HIGH);

}


/**
  * @brief Initialize the RS0010 display.
  * @param cols Display width in character columns, max 16.
  * @param lines Number of display lines, 1 or 2.
  * @param mode Display mode; 1 selects graphic mode.
  */
void SPI_RS0010::begin(uint8_t cols, uint8_t lines, uint8_t mode)
{
    (void)cols;
    
    delay_ms(500U);
    
    // set number of lines 
    if (lines == 2) 
        command(RS_2LINE); //
    else 
        command(RS_1LINE);

    // Function set: 2 lines, 5x8, Western European II
    // delay_ms(7U);

    if (mode == 1) 
        command(RS_GRAPHIC);//Graphic mode; RS_GRAPHIC
    else 
        command(RS_CHARACTER); //Alphanumeric mode

    // delay_ms(7U);

    command(0x0E);//Display on, Cursor ON, Cursor Blinking
    // delay_ms(7U);

    command(RS_CLEARDISPLAY);//clear display
    delay_ms(7U);

    command(RS_RETURNHOME);//Display Home
    // delay_ms(7U);

    command(RS_ENTRYSHIFTINCREMENT);//Increment and Scroll
    // delay_ms(7U);

    command(RS_CLEARDISPLAY);//clear display
    delay_ms(7U);

    command(0x0c);//display on

    // set the brightness and push the linecount with RS_SETFUNCTION
    /*
    _numlines = lines;
    _currline = 0;

    // Initialize to default text direction (for romance languages#include "SPI_RS0010.h"
    _displaymode = RS_ENTRYLEFT | RS_ENTRYSHIFTDECREMENT;
    // set the entry mode
    command(RS_ENTRYMODESET | _displaymode);

    command(RS_SETDDRAMADDR);  // go to address 0

    // turn the display on with no cursor or blinking default
    _displaycontrol = RS_DISPLAYON;
    display();

    clear();
    home();

    */
}


/**
  * @brief Clear the display and return the cursor to the origin.
  */
void SPI_RS0010::clear()
{
    command(RS_CLEARDISPLAY);  // clear display, set cursor position to zero
    delay_ms(7U);  // this command takes a long time!
}


/**
  * @brief Return the display cursor to its home position.
  */
void SPI_RS0010::home()
{
    command(RS_RETURNHOME);  // set cursor position to zero
    // delay_ms(2U);  // this command takes a long time!
}


/**
  * @brief Set the display cursor position.
  * @param x Horizontal cursor position.
  * @param y Display line index.
  */
void SPI_RS0010::setCursor(uint8_t x, uint8_t y)
{

    // command(RS_SETDDRAMADDR | (col + row_offsets[row]));
    command(0x80 | x);
    // delay_ms(7U);
    if (y == 0)  command(0x40);
    else command(0x41);
}


// Turn the display on/off (quickly)
/**
  * @brief Turn the display off without changing its contents.
  */
void SPI_RS0010::noDisplay()
{
    _displaycontrol &= ~RS_DISPLAYON;
    command(RS_DISPLAYCONTROL | _displaycontrol);
}


/**
  * @brief Turn the display on.
  */
void SPI_RS0010::display()
{
    _displaycontrol |= RS_DISPLAYON;
    command(RS_DISPLAYCONTROL | _displaycontrol);
}


// Turns the underline cursor on/off
/**
  * @brief Disable the display cursor.
  */
void SPI_RS0010::noCursor()
{
    _displaycontrol &= ~RS_CURSORON;
    command(RS_DISPLAYCONTROL | _displaycontrol);
}


/**
  * @brief Enable the display cursor.
  */
void SPI_RS0010::cursor()
{
    _displaycontrol |= RS_CURSORON;
    command(RS_DISPLAYCONTROL | _displaycontrol);
}


// Turn on and off the blinking cursor
/**
  * @brief Disable cursor blinking.
  */
void SPI_RS0010::noBlink()
{
    _displaycontrol &= ~RS_BLINKON;
    command(RS_DISPLAYCONTROL | _displaycontrol);
}


/**
  * @brief Enable cursor blinking.
  */
void SPI_RS0010::blink()
{
    _displaycontrol |= RS_BLINKON;
    command(RS_DISPLAYCONTROL | _displaycontrol);
}


// These commands scroll the display without changing the RAM
/**
  * @brief Shift the displayed contents one position to the left.
  */
void SPI_RS0010::scrollDisplayLeft(void)
{
    command(RS_CURSORSHIFT | RS_DISPLAYMOVE | RS_MOVELEFT);
}


/**
  * @brief Shift the displayed contents one position to the right.
  */
void SPI_RS0010::scrollDisplayRight(void)
{
    command(RS_CURSORSHIFT | RS_DISPLAYMOVE | RS_MOVERIGHT);
}


// This is for text that flows Left to Right
/**
  * @brief Set the cursor to advance from left to right.
  */
void SPI_RS0010::leftToRight(void)
{
    _displaymode |= RS_ENTRYLEFT;
    command(RS_ENTRYMODESET | _displaymode);
}


// This is for text that flows Right to Left
/**
  * @brief Set the cursor to advance from right to left.
  */
void SPI_RS0010::rightToLeft(void)
{
    _displaymode &= ~RS_ENTRYLEFT;
    command(RS_ENTRYMODESET | _displaymode);
}


// This will 'right justify' text from the cursor
/**
  * @brief Enable automatic display shifting when writing characters.
  */
void SPI_RS0010::autoscroll(void)
{
    _displaymode |= RS_ENTRYSHIFTINCREMENT;
    command(RS_ENTRYMODESET | _displaymode);
}


// This will 'left justify' text from the cursor
/**
  * @brief Disable automatic display shifting when writing characters.
  */
void SPI_RS0010::noAutoscroll(void)
{
    _displaymode &= ~RS_ENTRYSHIFTINCREMENT;
    command(RS_ENTRYMODESET | _displaymode);
}


// Allows us to fill the first 8 CGRAM locations
// with custom characters
/**
  * @brief Write custom character data to the display.
  * @param charmap Array containing the character data.
  * @param length Number of data bytes to write.
  */
void SPI_RS0010::createChar(uint8_t charmap[], uint8_t length)
{
    for (int i = 0; i < length; i++)
    {
        write(charmap[i]);
    }
}


// Draw two lines at a time
/**
  * @brief Draw character data across two display lines.
  * @param charmap Array containing the character data.
  * @param x Starting column.
  * @param length Number of data bytes to draw.
  */
void SPI_RS0010::drawGraphic(uint8_t charmap[], uint8_t x, uint8_t length)
{
    for (int i = 0; i < (length / 2); i++)
    {
        setCursor(x + i, 0);
        write(charmap[i]);
    }
    for (int i = (length / 2); i < length; i++)
    {
        setCursor(x + i - (length / 2), 1);
        write(charmap[i]);
    }
}


//==================== DRAW PIXEL =====================
/**
  * @brief Set or clear one pixel in the framebuffer.
  * @param x Horizontal pixel coordinate.
  * @param y Vertical pixel coordinate.
  * @param color Nonzero to set the pixel; zero to clear it.
  */
void SPI_RS0010::drawPixel(int8_t x, int8_t y, uint8_t color)
{

    if ((x < 0) || (x >= RS0010_WIDTH) || (y < 0) || (y >= RS0010_HEIGHT))
        return;
    if (color)
    {
        rs0010_buf[x + (y / 8) * RS0010_WIDTH] |= (1 << (y % 8));
    }
    else
    {
        rs0010_buf[x + (y / 8) * RS0010_WIDTH] &= ~(1 << (y % 8));
    }

}


//=================== DISPLAY BUFFER ON LCD ================================
/**
  * @brief Transfer the framebuffer contents to the display.
  */
void SPI_RS0010::displayBuf(void)
{
    setCursor(0, 0);
    for (uint16_t i = 0; i < (RS0010_WIDTH * RS0010_HEIGHT / 8); i++)
    {
        write(rs0010_buf[i]);
    }
}


//=================== CLEAR BUFFER =========================================
/**
  * @brief Clear the framebuffer.
  */
void SPI_RS0010::clearBuf(void)
{
    memset(rs0010_buf, 0x00, (BUF_SIZE));
}


//=================== DRAW GRAPHICS
/**
  * @brief Draw a monochrome bitmap into the framebuffer.
  * @param x Horizontal starting coordinate.
  * @param y Vertical starting coordinate.
  * @param bitmap Bitmap data, stored one bit per pixel.
  * @param w Bitmap width in pixels.
  * @param h Bitmap height in pixels.
  * @param color Nonzero to set bitmap pixels; zero to clear them.
  */
void SPI_RS0010::drawBitmap_P(int x, int y, const uint8_t* bitmap, uint8_t w, uint8_t h, uint8_t color)
{

    int i, j, byteWidth = (w + 7) / 8;
    for (j = 0; j < h; j++)
    {
        for (i = 0; i < w; i++)
        {
            if (bitmap[j * byteWidth + i / 8] & (128 >> (i & 7)))
            {
                drawPixel(x + i, y + j, color);
            }
        }
    }
}


//====================== DRAW CHARACTER
/**
  * @brief Draw one character into the framebuffer.
  * @param x Horizontal character origin.
  * @param y Vertical character origin.
  * @param c Character code.
  * @param color Foreground pixel value.
  * @param bg Background pixel value for the built-in font.
  * @param size Character scale factor.
  */
void SPI_RS0010::drawChar(int x, int y, unsigned char c, uint8_t color, uint8_t bg, uint8_t size)
{

    if (!gfxFont)
    { // 'Classic' built-in font

        if ((x >= RS0010_WIDTH) || // Clip right
            (y >= RS0010_HEIGHT) || // Clip bottom
            ((x + 6 * size - 1) < 0) || // Clip left
            ((y + 8 * size - 1) < 0))   // Clip top
            return;

        if (!_cp437 && (c >= 176)) c++; // Handle 'classic' charset behavior

        for (int8_t i = 0; i < 6; i++)
        {
            uint8_t line;
            if (i < 5) line = font[(c * 5) + i];
            else      line = 0x0;
            for (int8_t j = 0; j < 8; j++, line >>= 1)
            {
                if (line & 0x1)
                {
                    if (size == 1) drawPixel(x + i, y + j, color);
                    else          fillRect(x + (i * size), y + (j * size), size, size, color);
                }
                else if (bg != color)
                {
                    if (size == 1) drawPixel(x + i, y + j, bg);
                    else          fillRect(x + i * size, y + j * size, size, size, bg);
                }
            }
        }

    }
    else
    { // Custom font

 // Character is assumed previously filtered by write() to eliminate
 // newlines, returns, non-printable characters, etc.  Calling drawChar()
 // directly with 'bad' characters of font may cause mayhem!

        c -= gfxFont->first;
        GFXglyph* glyph = &gfxFont->glyph[c];
        uint8_t* bitmap = gfxFont->bitmap;

        uint16_t bo = glyph->bitmapOffset;
        uint8_t  w = glyph->width;
        uint8_t  h = glyph->height;
        int8_t   xo = glyph->xOffset;
        int8_t   yo = glyph->yOffset;
        uint8_t  xx, yy, bits, bit = 0;
        int16_t  xo16, yo16;

        if (size > 1)
        {
            xo16 = xo;
            yo16 = yo;
        }

        // Todo: Add character clipping here

        // NOTE: THERE IS NO 'BACKGROUND' COLOR OPTION ON CUSTOM FONTS.
        // THIS IS ON PURPOSE AND BY DESIGN.  The background color feature
        // has typically been used with the 'classic' font to overwrite old
        // screen contents with new data.  This ONLY works because the
        // characters are a uniform size; it's not a sensible thing to do with
        // proportionally-spaced fonts with glyphs of varying sizes (and that
        // may overlap).  To replace previously-drawn text when using a custom
        // font, use the getTextBounds() function to determine the smallest
        // rectangle encompassing a string, erase the area with fillRect(),
        // then draw new text.  This WILL infortunately 'blink' the text, but
        // is unavoidable.  Drawing 'background' pixels will NOT fix this,
        // only creates a new set of problems.  Have an idea to work around
        // this (a canvas object type for MCUs that can afford the RAM and
        // displays supporting setAddrWindow() and pushColors()), but haven't
        // implemented this yet.

        for (yy = 0; yy < h; yy++)
        {
            for (xx = 0; xx < w; xx++)
            {
                if (!(bit++ & 7))
                {
                    bits = bitmap[bo++];
                }
                if (bits & 0x80)
                {
                    if (size == 1)
                    {
                        drawPixel(x + xo + xx, y + yo + yy, color);
                    }
                    else
                    {
                        fillRect(x + (xo16 + xx) * size, y + (yo16 + yy) * size, size, size, color);
                    }
                }
                bits <<= 1;
            }
        }

    } // End classic vs custom font
}


//====================== DRAW STRING
/**
  * @brief Draw a null-terminated string into the framebuffer.
  * @param x Horizontal starting coordinate.
  * @param y Vertical starting coordinate.
  * @param cstring String to draw.
  * @param color Foreground pixel value, WHITE or BLACK.
  * @param bg Background pixel value, WHITE or BLACK.
  * @param text_size Character scale factor.
  */
void SPI_RS0010::drawString(int x, int y, const char* cstring, uint8_t color, uint8_t bg, uint8_t text_size)
{
    //int cursor_x = x; int cursor_y = y;

    while (*cstring)
    {

        if (*cstring == 46)
        {
            drawChar(x - 1, y, *cstring++, color, bg, text_size);
            x += 1 * 5;
        }
        else
        {
            drawChar(x, y, *cstring++, color, bg, text_size);
            x += 1 * 6;
        }
    }
}


/**
  * @brief Convert an integer to decimal text and draw it.
  * @param x Horizontal starting coordinate.
  * @param y Vertical starting coordinate.
  * @param data Integer value to draw.
  * @param color Foreground pixel value, WHITE or BLACK.
  * @param bg Background pixel value, WHITE or BLACK.
  * @param text_size Character scale factor.
  */
void SPI_RS0010::drawInteger(int x, int y, int data, uint8_t color, uint8_t bg, uint8_t text_size)
{
    char buf[20];
    snprintf(buf, sizeof(buf), "%d", data);
    drawString(x, y, buf, color, bg, text_size);

}


/**
  * @brief Format a floating-point value and draw it.
  * @param x Horizontal starting coordinate.
  * @param y Vertical starting coordinate.
  * @param data Floating-point value to draw.
  * @param places Number of digits after the decimal point.
  * @param color Foreground pixel value, WHITE or BLACK.
  * @param bg Background pixel value, WHITE or BLACK.
  * @param text_size Character scale factor.
  */
void SPI_RS0010::drawFloat(int x, int y, float data, uint8_t places, uint8_t color, uint8_t bg, uint8_t text_size)
{
    char buf[6];
    floatToString(buf, data, places);
    drawString(x, y, buf, color, bg, text_size);

}


/**
  * @brief Draw one character using the bold font.
  * @param x Horizontal character origin.
  * @param y Vertical character origin.
  * @param c Character code; digits and colon are supported by this font.
  * @param color Foreground pixel value, WHITE or BLACK.
  * @param bg Background pixel value, WHITE or BLACK.
  */
void SPI_RS0010::drawBoldChar(int x, int y, unsigned char c, uint8_t color, uint8_t bg)
{


    if ((x >= RS0010_WIDTH) || // Clip right
        (y >= RS0010_HEIGHT) || // Clip bottom
        ((x + 11 - 1) < 0) || // Clip left
        ((y + 16 - 1) < 0))   // Clip top
        return;

    uint8_t line1, line2;
    if (c == 58)
    {
        for (int8_t i = 0; i < 6; i++)
        {
            if (i == 6)
            {
                line1 = 0;
            }
            else
            {
                line1 = boldfont[((c - 48) * 20) + i];
            }

            for (int8_t j = 0; j < 8; j++)
            {
                if (line1 & 1)
                {
                    drawPixel(x + i, y + j, color);

                }
                else if (bg != color)
                {
                    drawPixel(x + i, y + j, bg);
                }
                line1 >>= 1;
            }
        }
        for (int8_t i = 6; i < 12; i++)
        {
            if (i == 12)
            {
                line2 = 0;
            }
            else
            {
                line2 = boldfont[((c - 48) * 20) + i];
            }

            for (int8_t j = 8; j < 16; j++)
            {
                if (line2 & 1)
                {
                    drawPixel(x + i - 6, y + j, color);

                }
                else if (bg != color)
                {
                    drawPixel(x + i, y + j, bg);
                }
                line2 >>= 1;
            }
        }
    }
    else
    {
        for (int8_t i = 0; i < 10; i++)
        {
            if (i == 10)
            {
                line1 = 0;
            }
            else
            {
                line1 = boldfont[((c - 48) * 20) + i];
            }

            for (int8_t j = 0; j < 8; j++)
            {
                if (line1 & 1)
                {
                    drawPixel(x + i, y + j, color);

                }
                else if (bg != color)
                {
                    drawPixel(x + i, y + j, bg);
                }
                line1 >>= 1;
            }
        }
        for (int8_t i = 10; i < 20; i++)
        {
            if (i == 20)
            {
                line2 = 0;
            }
            else
            {
                line2 = boldfont[((c - 48) * 20) + i];
            }

            for (int8_t j = 8; j < 16; j++)
            {
                if (line2 & 1)
                {
                    drawPixel(x + i - 10, y + j, color);

                }
                else if (bg != color)
                {
                    drawPixel(x + i, y + j, bg);
                }
                line2 >>= 1;
            }
        }
    }
}


/**
  * @brief Draw a null-terminated string using the bold font.
  * @param x Horizontal starting coordinate.
  * @param y Vertical starting coordinate.
  * @param cstring String to draw.
  * @param color Foreground pixel value, WHITE or BLACK.
  * @param bg Background pixel value, WHITE or BLACK.
  */
void SPI_RS0010::drawBoldString(int x, int y, const char* cstring, uint8_t color, uint8_t bg)
{
    //int cursor_x = x; int cursor_y = y;

    while (*cstring)
    {
        if (*cstring == 58)
        {
            drawBoldChar(x - 2, y, *cstring++, color, bg);
            x += 1 * 3;
        }
        else
        {
            drawBoldChar(x, y, *cstring++, color, bg);
            x += 1 * 11;
        }
    }

}


/**
  * @brief Convert an integer to decimal text and draw it using the bold font.
  * @param x Horizontal starting coordinate.
  * @param y Vertical starting coordinate.
  * @param data Integer value to draw.
  * @param color Foreground pixel value, WHITE or BLACK.
  * @param bg Background pixel value, WHITE or BLACK.
  */
void SPI_RS0010::drawBoldInteger(int x, int y, int data, uint8_t color, uint8_t bg)
{
    char buf[12];
    snprintf(buf, sizeof(buf), "%d", data);
    drawBoldString(x, y, buf, color, bg);

}


//====================== DRAW FAST LINES
/**
  * @brief Draw a vertical line.
  * @param x Horizontal coordinate.
  * @param y Starting vertical coordinate.
  * @param h Line height in pixels.
  * @param color Pixel value to draw, WHITE or BLACK.
  */
void SPI_RS0010::drawFastVLine(int8_t x, int8_t y, int8_t h, uint8_t color)
{
    // Update in subclasses if desired!
    drawLine(x, y, x, y + h - 1, color);
}


/**
  * @brief Draw a horizontal line.
  * @param x Starting horizontal coordinate.
  * @param y Vertical coordinate.
  * @param w Line width in pixels.
  * @param color Pixel value to draw, WHITE or BLACK.
  */
void SPI_RS0010::drawFastHLine(int8_t x, int8_t y, int8_t w, uint8_t color)
{
    // Update in subclasses if desired!
    drawLine(x, y, x + w - 1, y, color);
}


/**
  * @brief Fill a rectangular area in the framebuffer.
  * @param x Left coordinate.
  * @param y Top coordinate.
  * @param w Rectangle width in pixels.
  * @param h Rectangle height in pixels.
  * @param color Pixel value to fill with, WHITE or BLACK.
  */
void SPI_RS0010::fillRect(int8_t x, int8_t y, int8_t w, int8_t h, uint8_t color)
{
    // Update in subclasses if desired!
    for (int16_t i = x; i < x + w; i++)
    {
        drawFastVLine(i, y, h, color);
    }
}


/**
  * @brief Draw a line between two points in the framebuffer.
  * @param x0 Starting point horizontal coordinate.
  * @param y0 Starting point vertical coordinate.
  * @param x1 Ending point horizontal coordinate.
  * @param y1 Ending point vertical coordinate.
  * @param color Pixel value to draw, WHITE or BLACK.
  */
void SPI_RS0010::drawLine(int8_t x0, int8_t y0, int8_t x1, int8_t y1, uint8_t color)
{
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep)
    {
        _swap_int16_t(x0, y0);
        _swap_int16_t(x1, y1);
    }

    if (x0 > x1)
    {
        _swap_int16_t(x0, x1);
        _swap_int16_t(y0, y1);
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1)
    {
        ystep = 1;
    }
    else
    {
        ystep = -1;
    }

    for (; x0 <= x1; x0++)
    {
        if (steep)
        {
            drawPixel(y0, x0, color);
        }
        else
        {
            drawPixel(x0, y0, color);
        }
        err -= dy;
        if (err < 0)
        {
            y0 += ystep;
            err += dx;
        }
    }
}


/************ Convert Float to String    **********/
/**
  * @brief Format a floating-point value as a null-terminated string.
  * @param outstr Destination buffer; it must be large enough for the result.
  * @param value Floating-point value to format.
  * @param places Number of digits after the decimal point.
  * @param minwidth Minimum output width excluding the null terminator.
  * @param rightjustify If true, pad the output on the left; otherwise pad it on the right.
  * @return The destination buffer containing the formatted string.
  */
char* SPI_RS0010::floatToString(char* outstr, float value, int places, int minwidth, bool rightjustify)
{
    // this is used to write a float value to string, outstr.  oustr is also the return value.
    int digit;
    float tens = 0.1;
    int tenscount = 0;
    int i;
    float tempfloat = value;
    int c = 0;
    int charcount = 1;
    int extra = 0;
    // make sure we round properly. this could use pow from <math.h>, but doesn't seem worth the import
    // if this rounding step isn't here, the value  54.321 prints as 54.3209

    // calculate rounding term d:   0.5/pow(10,places)
    float d = 0.5;
    if (value < 0)
        d *= -1.0;
    // divide by ten for each decimal place
    for (i = 0; i < places; i++)
        d /= 10.0;
    // this small addition, combined with truncation will round our values properly
    tempfloat += d;

    // first get value tens to be the large power of ten less than value
    if (value < 0)
        tempfloat *= -1.0;
    while ((tens * 10.0) <= tempfloat)
    {
        tens *= 10.0;
        tenscount += 1;
    }

    if (tenscount > 0)
        charcount += tenscount;
    else
        charcount += 1;

    if (value < 0)
        charcount += 1;
    charcount += 1 + places;

    minwidth += 1; // both count the null final character
    if (minwidth > charcount)
    {
        extra = minwidth - charcount;
        charcount = minwidth;
    }

    if (extra > 0 and rightjustify)
    {
        for (int i = 0; i < extra; i++)
        {
            outstr[c++] = ' ';
        }
    }

    // write out the negative if needed
    if (value < 0)
        outstr[c++] = '-';

    if (tenscount == 0)
        outstr[c++] = '0';

    for (i = 0; i < tenscount; i++)
    {
        digit = (int)(tempfloat / tens);
        outstr[c++] = (char)('0' + digit);
        tempfloat = tempfloat - ((float)digit * tens);
        tens /= 10.0;
    }

    // if no places after decimal, stop now and return

    // otherwise, write the point and continue on
    if (places > 0)
        outstr[c++] = '.';


    // now write out each decimal place by shifting digits one by one into the ones place and writing the truncated value
    for (i = 0; i < places; i++)
    {
        tempfloat *= 10.0;
        digit = (int)tempfloat;
        outstr[c++] = (char)('0' + digit);
        // once written, subtract off that digit
        tempfloat = tempfloat - (float)digit;
    }
    if (extra > 0 and not rightjustify)
    {
        for (int i = 0; i < extra; i++)
        {
            outstr[c++] = ' ';
        }
    }


    outstr[c++] = '\0';
    return (outstr);
}

void SPI_RS0010::drawStringAligned(int left, int top, int right, int bottom, const char* cstring, uint8_t color, uint8_t bg, uint8_t alignment)
{
    int x = right - left;
    int y = bottom - top;
    int w = strlen(cstring) * 6;
    int h = 8;

    if (x < 0) x = 0;
    if (y < 0) y = 0;

    if (w >= x)
    {
        alignment &= 0xF0; // force left alignment if string is too long
    }

    if (h >= y)
    {
        alignment &= 0x0F; // force top alignment if string is too tall
    }
    
    switch (alignment & 0x0F)
    {
        case ALIGN_LEFT:
            x = left;
            break;

        case ALIGN_CENTER:
            x = left + (x - w) / 2;
            break;

        case ALIGN_RIGHT:
            x = right - w;
            break;

        default:
            x = left;
            break;
    }

    switch (alignment & 0xF0)
    {
        case ALIGN_TOP:
            y = top;
            break;

        case ALIGN_MIDDLE:
            y = top + (y - h) / 2;
            break;

        case ALIGN_BOTTOM:
            y = bottom - h;
            break;

        default:
            y = top;
            break;
    }

    drawString(x, y, cstring, color, bg, 1);
}




/*********** mid level commands, for sending data/cmds, init */

/**
  * @brief Send an instruction byte to the display controller.
  * @param value Instruction byte.
  */
void SPI_RS0010::command(uint8_t value)
{
    pinWrite(_cs, LOW);

    pinWrite(_mosi, LOW);
    _clk_out();

    // pinWrite(_mosi, LOW);
    _clk_out();

    send(value);

    pinWrite(_cs, HIGH);

    /*
    Serial.print(RS_SPICOMMAND, HEX);
    Serial.print('\t');
    Serial.println(value, HEX);
    */
}


void SPI_RS0010::updateScreen(uint8_t update_lines)
{
    uint8_t *p, buf_len;

    if (update_lines >= UPDATE_ALL)
    {
        p = rs0010_buf;
        buf_len = BUF_SIZE;
        command(0x40);
    }
    else
    {
        p = rs0010_buf + (update_lines * RS0010_WIDTH);
        buf_len = RS0010_WIDTH;
        command(0x40 | update_lines);
    }
    command(0x80);

    for (uint8_t i = 0; i < buf_len; i++)
    {
        write(*p++);
    }
}

/**
  * @brief Send one data byte to the display controller.
  * @param value Data byte.
  */
void SPI_RS0010::write(uint8_t value)
{
    pinWrite(_cs, LOW);

    pinWrite(_mosi, HIGH);
    _clk_out();

    pinWrite(_mosi, LOW);
    _clk_out();

    send(value);
    
    pinWrite(_cs, HIGH);

    /*
    Serial.print(RS_SPIDATA, HEX);
    Serial.print('\t');
    Serial.println(value, HEX);
    */
}


/************ low level data pushing commands **********/

/**
  * @brief Shift one byte to the display, most-significant bit first.
  * @param dat Byte to transmit.
  */
inline void SPI_RS0010::send(uint8_t dat)
{
    uint8_t i;

    for (i = 0x80; i; i >>= 1)
    {
        pinWrite(_mosi, (dat & i) != 0U ? HIGH : LOW);
        _clk_out();
    }
}

