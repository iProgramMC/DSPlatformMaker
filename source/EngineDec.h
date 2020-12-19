/*---------------------------------------------------------------------------------
							GAME PROJECT
						  [ July 29, 2020 ]
					 Programmed by iProgramInCpp
					 
						  EngineDec module
---------------------------------------------------------------------------------*/
#ifndef ENGINEDEC_H
#define ENGINEDEC_H

#include "Utils.hpp"
#include <stddef.h>

#define TIMER_SPEED (BUS_CLOCK/1024)

// 16 bit color is structured in 1555 format
//								 EBGR
// E - enable bit / alpha
// RGBA16

#define WHITE 0xFFFF
#define BLACK 0x0000
#define SKY 0xEEEF
#define BLUE 0xFC00
#define MAGENTA 0xFC1F
#define CYAN 0xFFE0
#define RED 0x801F
#define YELLOW 0x83FF

//#define RGBA16(r,g,b,a) ((r << 11) | (g << 6) | (b << 1) | a)
#define RGBA16(r,g,b,a)  ((a << 15) | (b << 10) | (g << 5) | r)
#define MakeColor5B(r,g,b) RGBA16(r,g,b,1)
#define MakeColor(r,g,b) RGBA16(r/8,g/8,b/8,1)

#include <nds.h>
#include <gl2d.h>

#define SCREEN_BOTTOM 1
#define SCREEN_TOP 2

#define STYLE_BOLD 0x80
#define STYLE_ITALIC 0x40
#define STYLE_UNDERLINE 0x20

#define LAYER_FORE 0
#define LAYER_BACK 1

#define TEXTURE_COUNT 16
extern int imageIDsReserved[TEXTURE_COUNT];

bool GetKeyState(uint32_t button);
bool GetKeyDownState(uint32_t button);
bool GetKeyUpState(uint32_t button);
bool GetKeyHeldState(uint32_t button);
void PushFrame();
void PlotPixel(int x, int y, short color);
void ClearScreen(short color);
void FillRectangle(short color, int dx, int dy, int w, int h);
void FillRectangleLerp(short c1, short c2, int dx, int dy, int w, int h);
void DrawRectangle(short color, int dx, int dy, int w, int h);
void DrawLine(int x1, int y1, int x2, int y2, short col);
void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, short colour);
void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, short colour);
void DrawImageSizable(glImage* image, int w, int h, int dx, int dy, int dw, int dh);
void SwitchScreenTop();
void SwitchScreenBottom();
void DrawImageFlippedH (glImage* image, int w, int h, int dx, int dy);
void DrawImageFlippedHV(glImage* image, int w, int h, int dx, int dy);
void DrawImageFlippedV (glImage* image, int w, int h, int dx, int dy);
void PrintChar(char x, char y, char c);
void InitConsoleScreen();
uint16_t map(uint16_t min, uint16_t max, uint8_t h);
uint16_t color_lerp(uint16_t min, uint16_t max, uint16_t i);
void DrawChar(uint16_t x, uint16_t y, char ch, uint16_t color, uint8_t styling);
void DrawString(uint8_t x, uint8_t y, uint16_t color, const char* str, uint8_t styling);
void DrawImage(glImage* image, int w, int h, int dx, int dy);
void DrawImageCrop(glImage* image, int w, int h, int dx, int dy, int sx, int sy, int dw, int dh);
void DrawImageCropFlipH(glImage* image, int w, int h, int dx, int dy, int sx, int sy, int dw, int dh);
void DrawImageColored(glImage* spr, int w, int h, int dx, int dy, int flipmode, u16 color);

#define GetBlueComponent(i) (i & 0x7c00) >> 10
#define GetGreenComponent(i) (i & 0x03e0) >> 5
#define GetRedComponent(i) (i & 0x001f)

#define Unite(r, g, b) 0x8000 | (b << 10) | (g << 5) | r

extern touchPosition touch, prevTouch;
extern int fps;
extern uint16_t GlobalTimer;

struct Rectangle {
	int x, y, width, height;
};
struct Point {
	int x, y;
};
typedef Point Point2d;
struct Point3d {
	int x, y, z;
};
struct Vec3f {
	float x, y, z;
};
struct Vec2f {
	float x, y;
};
bool RectangleContains(Rectangle r, Point p);
bool RectangleIntersect(Rectangle r1, Rectangle r2);

#endif//ENDINEDEC_H