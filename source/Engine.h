#ifndef DS_GAME_ENGINE_INCL
#define DS_GAME_ENGINE_INCL

#include "EngineDec.h"
#include "Map.h"
#include <gl2d.h>

/*
int round(float f){
	float decpt = f - (int)f;
	if(decpt < 0.5f){
		return (int)f;
	}else{
		return (int)f+1;
	}
}*/

short screenbuffer_maindisplay[49152];
short screen_width = 256;
short screen_height = 192;

uint32_t input_keys = 0;
uint32_t input_keysUp = 0;
uint32_t input_keysDown = 0;
uint32_t input_keysHeld = 0;

uint8_t GraphScreen;

touchPosition touch;
touchPosition prevTouch;
PrintConsole bottomScreen;

#define PlotPixel2(x,y,c) screenbuffer_maindisplay[(y)*256+(x)]=c

bool GetKeyState(uint32_t button){
	uint32_t s = input_keys & button;
	return s;
}
bool GetKeyDownState(uint32_t button){
	uint32_t s = input_keysDown & button;
	return s;
}
bool GetKeyUpState(uint32_t button){
	uint32_t s = input_keysUp & button;
	return s;
}
bool GetKeyHeldState(uint32_t button){
	uint32_t s = input_keysHeld & button;
	return s;
}
/*
void PushFrame(){
	if(GraphScreen == SCREEN_BOTTOM)
	{
		dmaCopy(&screenbuffer_maindisplay, BG_GFX_SUB, 49152*2);
	}
	else
	{
		dmaCopy(&screenbuffer_maindisplay, BG_GFX, 49152*2);
	}
}

void SwitchScreenBottom()
{
	GraphScreen = SCREEN_TOP;
	// Console is on bottom screen
	videoSetMode(MODE_5_2D);
	//videoSetModeSub(MODE_0_2D);

	vramSetBankA(VRAM_A_MAIN_BG);
	//vramSetBankC(VRAM_C_SUB_BG);

	bgInit(2, BgType_Bmp16, BgSize_B16_256x256, 0,0);
	
	//consoleInit(&topScreen, 3,BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
	consoleInit(&bottomScreen, 3,BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);
	ClearScreen(0x0000);
	PushFrame();
}
void SwitchScreenTop()
{
	GraphScreen = SCREEN_BOTTOM;
	// Console is on top screen
	//videoSetModeSub(MODE_5_2D);
	videoSetMode(MODE_0_2D);

	vramSetBankA(VRAM_A_MAIN_BG);
	//vramSetBankC(VRAM_C_SUB_BG);

	//bgInitSub(2, BgType_Bmp16, BgSize_B16_256x256, 0,0);
	
	//consoleInit(&topScreen, 3,BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
	consoleInit(&bottomScreen, 3,BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
	ClearScreen(0x0000);
	PushFrame();
}

void PlotPixel (int x, int y, short colour)
{
	if((x) >= 0 && (x) < 256 && (y) >= 0 && (y) < 192){
		screenbuffer_maindisplay[y*screen_width+x] = colour;
	}
}

void ClearScreen(short colour){
	for(int x = 0; x < 49152; x+=1){
		//PlotPixel(x % 256, x / 256, colour);
		screenbuffer_maindisplay[x]=colour;
	}
}

void FillRectangle(short colour, int dx, int dy, int w, int h){
	for(int y = 0; y < h; y++){
		for(int x = 0; x < w; x++){
			if((x+dx) >= 0 && (x+dx) < 256 && (y+dy) >= 0 && (y+dy) < 192){
				PlotPixel2(x+dx, dy+y, colour);
			}
		}
	}
}
void FillRectangleLerp(short colour1, short colour2, int dx, int dy, int w, int h){
	for(int y = 0; y < h; y++){
		for(int x = 0; x < w; x++){
			short colour = color_lerp(colour1, colour2, x * 255 / w);
			if((x+dx) >= 0 && (x+dx) < 256 && (y+dy) >= 0 && (y+dy) < 192){
				PlotPixel2(x+dx, dy+y, colour);
			}
		}
	}
}
void DrawRectangle(short colour, int dx, int dy, int w, int h){
	int x = 0;
	for(x = 0; x < w; x++){
		if((x+dx) >= 0 && (x+dx) < 256){
			PlotPixel2((x + dx), dy, colour);
		}
	}
	for(x = 0; x < w; x++){
		if((x+dx) >= 0 && (x+dx) < 256){
			PlotPixel2((x + dx), (dy - 1 + h), colour);
		}
	}
	for(x = 0; x < h; x++){
		if((x+dx) >= 0 && (x+dx) < 256){
			PlotPixel2((dx), (x + dy), colour);
		}
	}
	for(x = 0; x < h; x++){
		if((x+dx) >= 0 && (x+dx) < 256){
			PlotPixel2((w + dx - 1), (x + dy), colour);
		}
	}
}

void DrawLine (int x1, int y1, int x2, int y2, short col){
	int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
		dx = x2 - x1; dy = y2 - y1;
		dx1 = abs(dx); dy1 = abs(dy);
		px = 2 * dy1 - dx1;	py = 2 * dx1 - dy1;
		if (dy1 <= dx1)
		{
			if (dx >= 0)
				{ x = x1; y = y1; xe = x2; }
			else
				{ x = x2; y = y2; xe = x1;}

			PlotPixel2(x, y, col);
			
			for (i = 0; x<xe; i++)
			{
				x = x + 1;
				if (px<0)
					px = px + 2 * dy1;
				else
				{
					if ((dx<0 && dy<0) || (dx>0 && dy>0)) y = y + 1; else y = y - 1;
					px = px + 2 * (dy1 - dx1);
				}
				PlotPixel2(x, y, col);
			}
		}
		else
		{
			if (dy >= 0)
				{ x = x1; y = y1; ye = y2; }
			else
				{ x = x2; y = y2; ye = y1; }

			PlotPixel2(x, y, col);

			for (i = 0; y<ye; i++)
			{
				y = y + 1;
				if (py <= 0)
					py = py + 2 * dx1;
				else
				{
					if ((dx<0 && dy<0) || (dx>0 && dy>0)) x = x + 1; else x = x - 1;
					py = py + 2 * (dx1 - dy1);
				}
				PlotPixel2(x, y, col);
			}
		}
}

void DrawTriangle (int x1, int y1, int x2, int y2, int x3, int y3, short colour){
	DrawLine (x1, y1, x2, y2, colour);
	DrawLine (x1, y1, x3, y3, colour);
	DrawLine (x3, y3, x2, y2, colour);
}

void FillTriangle (int x1, int y1, int x2, int y2, int x3, int y3, short col){
	/ * Ripped from OneLoneCoder/ConsoleGameEngine * /
	int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
		dx = x2 - x1; dy = y2 - y1;
		dx1 = abs(dx); dy1 = abs(dy);
		px = 2 * dy1 - dx1;	py = 2 * dx1 - dy1;
		if (dy1 <= dx1)
		{
			if (dx >= 0)
				{ x = x1; y = y1; xe = x2; }
			else
				{ x = x2; y = y2; xe = x1;}

			PlotPixel2(x, y, col);
			DrawLine(x, y, x3, y3, col);
			
			for (i = 0; x<xe; i++)
			{
				x = x + 1;
				if (px<0)
					px = px + 2 * dy1;
				else
				{
					if ((dx<0 && dy<0) || (dx>0 && dy>0)) y = y + 1; else y = y - 1;
					px = px + 2 * (dy1 - dx1);
				}
				PlotPixel2(x, y, col);
			DrawLine(x, y, x3, y3, col);
			}
		}
		else
		{
			if (dy >= 0)
				{ x = x1; y = y1; ye = y2; }
			else
				{ x = x2; y = y2; ye = y1; }

			PlotPixel2(x, y, col);
			DrawLine(x, y, x3, y3, col);

			for (i = 0; y<ye; i++)
			{
				y = y + 1;
				if (py <= 0)
					py = py + 2 * dx1;
				else
				{
					if ((dx<0 && dy<0) || (dx>0 && dy>0)) x = x + 1; else x = x - 1;
					py = py + 2 * (dx1 - dy1);
				}
				PlotPixel2(x, y, col);
				DrawLine(x, y, x3, y3, col);
			}
		}
}

void DrawImageSizable(short* image, int w, int h, int dx, int dy, int dw, int dh){
	/ *float pixel_width = dw / w;
	float pixel_height= dh / h;
	
	for(int y = 0; y < dh; y++){
		for(int x = 0; x < dw; x++){
			if((x+dx) >= 0 && (x+dx) < 256 && (y+dy) >= 0 && (y+dy) < 192){
				int src_y = round(y * pixel_height);
				int src_x = round(x * pixel_width);
				screenbuffer_maindisplay[(dy+y)*screen_width+(x+dx)] = image[(src_y*w+src_x)];
			}
		}
	}* /
	
	int w2 = dw, w1 = w, h2 = dh, h1 = h;
	
    // EDIT: added +1 to account for an early rounding problem
    int x_ratio = (int)((w1<<16)/w2) +1;
    int y_ratio = (int)((h1<<16)/h2) +1;
	
    int x2, y2 ;
    for (int i=0;i<h2;i++) {
        for (int j=0;j<w2;j++) {
            x2 = ((j*x_ratio)>>16) ;
            y2 = ((i*y_ratio)>>16) ;
			if((j+dx) >= 0 && (j+dx) < 256 && (i+dy) >= 0 && (i+dy) < 192){
				PlotPixel2(j+dx,i+dy,image[(y2*w1)+x2]);
			}
        }                
    }            
}

void DrawImage(short* image, int w, int h, int dx, int dy){
	for(int y = 0; y < h; y++){
		for(int x = 0; x < w; x++){
			if((x+dx) >= 0 && (x+dx) < 256 && (y+dy) >= 0 && (y+dy) < 192){
				if(image[y*w+x] == -993){}else{
					PlotPixel2(x+dx,y+dy,image[y*w+x]);
				}
			}
		}
	}
}

void DrawImageCrop(short* image, int w, int h, int dx, int dy, int sx, int sy, int dw, int dh){
	for(int y = 0; y < dh; y++){
		for(int x = 0; x < dw; x++){
			if((x+dx) >= 0 && (x+dx) < 256 && (y+dy) >= 0 && (y+dy) < 192){
				if(image[(sy+y)*w+(sx+x)] == -993){}else{
					PlotPixel2(x+dx,y+dy,image[(sy+y)*w+(sx+x)]);
				}
			}
		}
	}
}

void DrawImageCropFlipH(short* image, int w, int h, int dx, int dy, int sx, int sy, int dw, int dh){
	for(int y = 0; y < dh; y++){
		for(int x = 0; x < dw; x++){
			if((x+dx) >= 0 && (x+dx) < 256 && (y+dy) >= 0 && (y+dy) < 192){
				if(image[(sy+y)*w+(sx+dw-1-x)] == -993){}else{
					PlotPixel2(x+dx,y+dy,image[(sy+y)*w+(sx+dw-1-x)]);
				}
			}
		}
	}
}

void DrawImageFlippedV(short* image, int w, int h, int dx, int dy){
	for(int y = 0; y < h; y++){
		for(int x = 0; x < w; x++){
			if((x+dx) >= 0 && (x+dx) < 256 && (y+dy) >= 0 && (y+dy) < 192){
				if(image[(h-y-1)*w+x] == -993){}else{
					PlotPixel2(x+dx,y+dy,image[(h-y-1)*w+x]);
				}
			}
		}
	}
}
void DrawImageFlippedHV(short* image, int w, int h, int dx, int dy){
	for(int y = 0; y < h; y++){
		for(int x = 0; x < w; x++){
			if((x+dx) >= 0 && (x+dx) < 256 && (y+dy) >= 0 && (y+dy) < 192){
				if(image[(h-y)*w+(w-x-1)] == -993){}else{
					PlotPixel2(x+dx,y+dy,image[(h-y)*w+(w-x-1)]);
				}
			}
		}
	}
}
void DrawImageFlippedH(short* image, int w, int h, int dx, int dy){
	for(int y = 0; y < h; y++){
		for(int x = 0; x < w; x++){
			if((x+dx) >= 0 && (x+dx) < 256 && (y+dy) >= 0 && (y+dy) < 192){
				if(image[y*w+(w-x-1)] == -993){}else{
					PlotPixel2(x+dx,y+dy,image[y*w+(w-x-1)]);
				}
			}
		}
	}
}*/

void PushFrame() {
	glFlush(0);
}

void PlotPixel(int x, int y, short color) {
	// todo
	glPutPixel(x, y, 0xffff);
}

typedef struct {
	union {
		struct {
			u8 r,g,b,a;
		};
		u32 c;
	};
} color_t;

inline float MapX(int x) {
	double f = (double)x / 128.0 * .935;
	f -= .935;
	return (float)f;
}
inline float MapY(int y) {
	double f = (191 - y) / 96.0 * .7;
	f -= .7;
	return (float)f;
}
color_t MakeColort(short color) {
	color_t c;
	c.r = (color & 0x1f) * 8;
	c.g = ((color >> 5) & 0x3f) >> 2;
	c.b = ((color >> 11) & 0x1f);
	c.a = ((color & 0x8000) ? 31 : 0) << 3;
	return c;
}

void ClearScreen(short color) {
	color_t c = MakeColort(color);
	glClearColor(c.r, c.g, c.b, c.a);
}
#define fv floattov16
void FillRectangle(short color, int dx, int dy, int w, int h) {
	glBoxFilled(dx,dy,dx+w,dy+h,color);
}
void FillRectangleLerp(short co1, short co2, int dx, int dy, int w, int h)
{
	glBoxFilledGradient(dx,dy,dx+w,dy+h,co1,co1,co2,co2);
}
void DrawRectangle(short color, int dx, int dy, int w, int h) {
	glBox(dx,dy,dx+w,dy+h,color);
}
void DrawLine(int x1, int y1, int x2, int y2, short col) {
	glLine(x1,y1,x2,y2,MakeColort(col).c);
}
int GetGLTexSizeEnum(int w) {
	/**/ if (w <=    8) return TEXTURE_SIZE_8;
	else if (w <=   16) return TEXTURE_SIZE_16;
	else if (w <=   32) return TEXTURE_SIZE_32;
	else if (w <=   64) return TEXTURE_SIZE_64;
	else if (w <=  128) return TEXTURE_SIZE_128;
	else if (w <=  256) return TEXTURE_SIZE_256;
	else if (w <=  512) return TEXTURE_SIZE_512;
	else if (w <= 1024) return TEXTURE_SIZE_1024;
	return TEXTURE_SIZE_1024;
}
int imageIDsReserved[TEXTURE_COUNT];
int nextImageIdx;
int LoadImageVRAM(short* image, int w, int h) {
	bool b = glTexImage2D(imageIDsReserved[nextImageIdx], 0, GL_RGBA, GetGLTexSizeEnum(w),GetGLTexSizeEnum(h), 0, 0, image);
	LogMsg("Loading image with size %dx%d (it got id %d) was %s", w, h,imageIDsReserved[nextImageIdx], b ? "successful":"unsuccessful");
	return imageIDsReserved[nextImageIdx++];
}
void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, short colour) {
	glTriangleFilled(x1,y1,x2,y2,x3,y3,colour);
}
void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, short colour) {
	glTriangle(x1,y1,x2,y2,x3,y3,colour);
}
void DrawImageSizable(glImage* image, int w, int h, int dx, int dy, int dw, int dh){}
void SwitchScreenTop(){}
void SwitchScreenBottom(){}

static v16 g_depth;
static inline void gxVertex3i_re(v16 x, v16 y, v16 z)
{
	GFX_VERTEX16 = (y << 16) | (x & 0xFFFF);
	GFX_VERTEX16 = ((uint32)(uint16)z);
}
static inline void gxTexcoord2i_re(t16 u, t16 v)
{
	GFX_TEX_COORD = (v << 20) | ( (u << 4) & 0xFFFF );
}
static inline void gxVertex2i_re(v16 x, v16 y)
{
	GFX_VERTEX_XY = (y << 16) | (x & 0xFFFF);	
}
void DrawImageColored(glImage* spr, int w, int h, int dx, int dy, int flipmode, u16 color) {
	// Cannot use glSprite, gotta recreate it
	/*
	int x1 = dx;
	int y1 = dy;
	int x2 = dx + spr->width;
	int y2 = dy + spr->height;

	int	u1 = spr->u_off + (( flipmode & GL_FLIP_H ) ? spr->width-1  : 0);
 	int	u2 = spr->u_off + (( flipmode & GL_FLIP_H ) ? 0			    : spr->width);
	int v1 = spr->v_off + (( flipmode & GL_FLIP_V ) ? spr->height-1 : 0);
 	int v2 = spr->v_off + (( flipmode & GL_FLIP_V ) ? 0 		    : spr->height);

	
 
    if ( spr->textureID != gCurrentTexture )
    {
        glBindTexture( GL_TEXTURE_2D, spr->textureID );
        gCurrentTexture = spr->textureID;
    }

	glBegin( GL_QUADS );
		glColor(color);gxTexcoord2i_re( u1, v1 ); gxVertex3i_re( x1, y1, g_depth );	
		glColor(color);gxTexcoord2i_re( u1, v2 ); gxVertex2i_re( x1, y2 );
		glColor(color);gxTexcoord2i_re( u2, v2 ); gxVertex2i_re( x2, y2 );
		glColor(color);gxTexcoord2i_re( u2, v1 ); gxVertex2i_re( x2, y1 );
	glEnd();
	g_depth++;
	glColor(0x7FFF);*/
	glColor(color);
	glSprite(dx, dy, flipmode, spr);
	glColor(0x7FFF);
}
void DrawImageINT(glImage* image, int w, int h, int dx, int dy, int fm) {
	glSprite(dx, dy, fm, image);
}
void DrawImageFlippedH (glImage* image, int w, int h, int dx, int dy) {
	DrawImageINT(image, w, h, dx, dy, GL_FLIP_H);
}
void DrawImageFlippedHV(glImage* image, int w, int h, int dx, int dy) {
	DrawImageINT(image, w, h, dx, dy, GL_FLIP_H|GL_FLIP_V);
}
void DrawImageFlippedV (glImage* image, int w, int h, int dx, int dy) {
	DrawImageINT(image, w, h, dx, dy,GL_FLIP_V);
}
void DrawImage(glImage* image, int w, int h, int dx, int dy) {
	DrawImageINT(image,w,h,dx,dy,0);
}
void DrawImageCrop(glImage* image, int w, int h, int dx, int dy, int sx, int sy, int dw, int dh) {
	DrawImage(image, w, h, dx, dy);
	//DrawRectangle(RED, dx, dy, dw, dh);
}
void DrawImageCropFlipH(glImage* image, int w, int h, int dx, int dy, int sx, int sy, int dw, int dh) {
	DrawImageFlippedH(image, w, h, dx, dy);
	//DrawRectangle(RED, dx, dy, dw, dh);
}
#undef fv
uint16_t GlobalTimer = 0;

void OnUserCreate();
void OnUserUpdate();

void InitConsoleScreen() {consoleSelect(&bottomScreen);}
void PrintChar(char x, char y, char c) { printf("\x1b[%d;%dH%c", y, x, c); }

void DoEverySecond() {
	LogMsg("test");
}
int game_ticks = 0;
int millisSinceRun = 0;

int fpsCounting, fps, prevS;

#include "bmpfont.h"
short font_bmp[1024*8];
int fontSpriteID;
glImage fontImage[128];

int main(void) {
	//videoSetModeSub(MODE_5_2D);
	lcdMainOnBottom();
	videoSetMode(MODE_5_3D);
	videoSetModeSub(MODE_0_2D);

	// init gl2d
	glScreen2D();
	glEnable(GL_ANTIALIAS);
	vramSetBankA( VRAM_A_TEXTURE );
	vramSetBankB( VRAM_B_TEXTURE );
	
	soundEnable();
	
	bgInitSub(2, BgType_Bmp16, BgSize_B16_256x256, 0,0);
	
	consoleInit(&bottomScreen, 3,BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);

	InitConsoleScreen();
	
	timerStart(0, ClockDivider_1024, 0, NULL);
	
	glGenTextures(TEXTURE_COUNT, imageIDsReserved);
	// load font shit
	decompress(bmpfontBitmap,font_bmp,LZ77);
	fontSpriteID = glLoadTileSet(fontImage, 6,8,1024,8, GL_RGBA, TEXTURE_SIZE_1024, TEXTURE_SIZE_8, GL_TEXTURE_WRAP_S|GL_TEXTURE_WRAP_T|TEXGEN_OFF|GL_TEXTURE_COLOR0_TRANSPARENT, 0, NULL, (u8*)font_bmp);
	
	OnUserCreate();
	while(1) {
		glBegin2D();
		fpsCounting++;
		game_ticks += timerElapsed(0);
		uint64_t r = game_ticks * 1000llu;
		r /= TIMER_SPEED;
		millisSinceRun = r;
		
		int s = millisSinceRun / 1000;
		if (prevS != s) {
			fps = fpsCounting;
			fpsCounting = 0;
		}
		prevS = s;
		
		prevTouch = touch;
		touchRead(&touch);
		scanKeys();
		
		input_keys = keysCurrent();
		input_keysUp = keysUp();
		input_keysDown = keysDown();
		input_keysHeld = keysHeld();
		
		OnUserUpdate();
		char str[100];
		memset(str,0,100);
		sprintf(str, "fps: %d", fps);
		DrawString(2, 184, BLACK, str, 0);
		DrawString(1, 183, WHITE, str, 0);
		
		GlobalTimer++;
		glEnd2D();
		PushFrame();
		g_depth = 4000;
		swiWaitForVBlank();
	}

	return 0;
}

bool RectangleContains(Rectangle r, Point p) {
	return (r.x <= p.x && r.y <= p.y && r.x + r.width > p.x && r.y + r.height > p.y);
}

bool RectangleIntersect(Rectangle r1, Rectangle r2) {
	int playerX1 = r1.x, playerX2 = r1.x + r1.width, playerY1 = r1.y, playerY2 = r1.y + r1.height;
	int tileRectX1 = r2.x, tileRectX2 = r2.x + r2.width, tileRectY1 = r2.y, tileRectY2 = r2.y + r2.height;
	bool noOverlap = tileRectX1 > playerX2 ||
					 playerX1 > tileRectX2 ||
					 tileRectY1 > playerY2 ||
					 playerY1 > tileRectY2;
	return !noOverlap;
}
 
/*int dx = x2 - x1;
	int dy = y2 - y1;
		
	if (dx >= dy){
		float y = 0.0f;
		if(x1 < x2){
			for (int x = x1; x <= x2; x++) {
				y = y1 + dy * (x - x1) / dx;
				PlotPixel(x, y, colour);
			}
		}else{
			for (int x = x2; x <= x1; x++) {
				y = y1 + dy * (x - x1) / dx;
				PlotPixel(x, y, colour);
			}
		}
	}else{
		float x = 0.0f;
		if(dx == 0){
			if(y1 < y2){
				for (int y = y1; y <= y2; y++) {
					PlotPixel(x1, y, colour);
				}
			}else{
				for (int y = y2; y <= y1; y++) {
					PlotPixel(x2, y, colour);
				}
			}
		}else{
			if(y1 < y2){
				for (int y = y1; y <= y2; y++) {
					x = x1 + dx * (y - y1) / dy;
					PlotPixel(x, y, colour);
				}
			}else{
				for (int y = y2; y <= y1; y++) {
					x = x1 + dx * (y - y1) / dy;
					PlotPixel(x, y, colour);
				}
			}
		}
	}*/

#include "Features/RNG.h"
#include "Font.h"

#endif
