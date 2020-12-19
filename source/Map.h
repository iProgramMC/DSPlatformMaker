

// returns an integer between min and max using range from 0-255
uint16_t map(uint16_t min, uint16_t max, uint8_t h)
{
	uint16_t i = max - min;
	if(h == 0x00) return min;
	else if(h == 255) return max;
	else
	{
		uint16_t m = min + (h * i / 256);
		return m;
	}
}

uint16_t color_lerp(uint16_t min, uint16_t max, uint16_t i)
{
	uint8_t mred = GetRedComponent(min);
	uint8_t mgreen = GetGreenComponent(min);
	uint8_t mblue = GetBlueComponent(min);
	
	uint8_t m2red = GetRedComponent(max);
	uint8_t m2green = GetGreenComponent(max);
	uint8_t m2blue = GetBlueComponent(max);
	
	uint8_t rred = map(mred, m2red, i);
	uint8_t rgreen = map(mgreen, m2green, i);
	uint8_t rblue = map(mblue, m2blue, i);
	
	return Unite(rred, rgreen, rblue);
}