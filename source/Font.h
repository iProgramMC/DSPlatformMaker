#include "Typeface.h"
void DrawCharOld(uint16_t x, uint16_t y, char ch, uint16_t color, uint8_t styling)
{
    uint16_t address = ch * 5;
    uint8_t n = 0;
    uint16_t new_x = x + n;
    uint16_t new_y;
    uint8_t a;
	uint16_t m = 4;
	if((styling & STYLE_ITALIC) == 0)
	{
		m = 0;
	}
    for (n = 0; n < 5; n++)
    {
        new_x = x + n;
        a = MainFont[address + n];
		if((styling & STYLE_ITALIC) != 0)
		{
			m = 4;
		}
        for(new_y = 0; new_y < 8; new_y++)
        {
            if(a & (0x01 << new_y))
            {
				if((styling & STYLE_BOLD) != 0) 
				{
					PlotPixel(new_x + m + 1, new_y + y, color);
				}
                PlotPixel(new_x + m, y + new_y, color);
            }
			if((styling & STYLE_ITALIC) != 0 && new_y % 3 == 0) { m--; }
        }
		if((styling & STYLE_UNDERLINE) != 0)
		{
			PlotPixel(new_x - 1, y + 7, color);
			PlotPixel(new_x, y + 7, color);
			PlotPixel(new_x + 1, y + 7, color);
		}
    }
}

void DrawChar(uint16_t x, uint16_t y, char ch, uint16_t color, uint8_t styling) {
	// italic and uline ignored for now, might add it later
	if (ch >= 128) ch = 127;
	DrawImageColored(&fontImage[(int)ch], 6, 8, x, y, 0, color);
	if (styling & STYLE_BOLD) {
		DrawImageColored(&fontImage[(int)ch], 6, 8, x+1, y, 0, color);
	}
}
void DrawString(uint8_t x, uint8_t y, uint16_t color, const char* str, uint8_t styling)
{
	const char* m = str;
	uint8_t ny = y;
	uint8_t nx = x;
	while(*m != 0)
	{
		if(*m == '\n')
		{
			nx = x;
			ny += 8;
		}
		else
		{
			DrawChar(nx, ny, *m, color, styling);
			if((styling & STYLE_BOLD) != 0) 
			{
				nx += 7;
			}
			else
			{
				nx += 6;
			}
		}
		m++;
	}
}