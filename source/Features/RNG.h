unsigned short rng;

short sky_colour = 0xEEEF;

unsigned short rng_function(unsigned short input){
	if(input == 0x560A) input=0;
	unsigned short S0 = (unsigned char)input<<8;
	S0 = S0 ^ input;
	input = ((S0 & 0xFF) << 8) | ((S0 & 0xFF00) >> 8);
	S0 = ((unsigned char)S0 << 1) ^ input;
	short S1 = (S0 >> 1) ^ 0xFF80;
	if((S0 & 1) == 0){
		if(S1 == 0xAA55){input = 0;}else{input=S1 ^ 0x1FF4;}
	}else{
		input = S1 ^ 0x1FF4;
	}
	return (unsigned short)input;
}

void rng_update() {
	rng = rng_function(GlobalTimer);
}
