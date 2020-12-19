/*---------------------------------------------------------------------------------
							GAME PROJECT
						  [ July 29, 2020 ]
					 Programmed by iProgramInCpp
					 
							Main module
---------------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <fat.h>

#include "Engine.h"
#include "Utils.hpp"
#include "TestGame.hpp"
/* 
	DS Game Engine Function Definitions
	-- to keep in mind
	
	bool GetKeyState(uint32_t)
	bool GetKeyDownState(uint32_t)
	bool GetKeyUpState(uint32_t)
	bool GetKeyHeldState(uint32_t)
	void PushFrame();
	void PlotPixel(int x, int y, short color)
	void ClearScreen(short color)
	void FillRectangle(short color, int dx, int dy, int w, int h)
	void FillRectangleLerp(short c1, short c2, int dx, int dy, int w, int h)
	void DrawRectangle(short color, int dx, int dy, int w, int h)
	void DrawLine(int x1, int y1, int x2, int y2, short col)
	void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, short colour)
	void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, short colour)
	void DrawImageSizable(short* image, int w, int h, int dx, int dy, int dw, int dh)
	void SwitchScreenTop()
	void SwitchScreenBottom()
	void DrawImageFlippedH(short* image, int w, int h, int dx, int dy)
	void DrawImageFlippedHV(short* image, int w, int h, int dx, int dy)
	void DrawImageFlippedV(short* image, int w, int h, int dx, int dy)
	void PrintChar(char x, char y, char c);
	void InitConsoleScreen();
*/
bool isFileSystemEnabled = false;
void OnUserCreate()
{
	/*isFileSystemEnabled = fatInitDefault();
	FILE* f = fopen("/test.txt", "r");
	char s [15];
	memset(s,0,15);
	if (!f) {
		LogMsg("Unable to open file");
	} else {
		fread(s, 1, 14, f);
		fclose(f);
		LogMsg("Got string: %s", s);
	}*/
	//SwitchScreenBottom();
	//SwitchScreenTop();
	//InitConsoleScreen();
	printf("\n\n\n\n\n\n\n\n");
	printf("     Growalone Maker V%.2f      ", 0.10); // game version
	printf("     For Nintendo DS / DSi      ");
	//printf(" This is intended for use with  ");
	//printf("the touch screen. Please look at");
	//printf("     the bottom screen now.     ");
	//printf(" There is NOTHING to see here...");
	printf("\n");
	printf("NOTE: You can't save. This is a ");
	printf("prototype. Saving will be added ");
	printf("           later....            ");
	
	Game::Init();
	Game::LoadContent();
}


void OnUserUpdate()
{
	rng_update();
	//ClearScreen(RGBA16(0, 12, 31, 1));
	Game::Update();
}
