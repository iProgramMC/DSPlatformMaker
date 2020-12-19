/*---------------------------------------------------------------------------------
							GAME PROJECT
						  [ July 29, 2020 ]
					 Programmed by iProgramInCpp
					 
						  TestGame module
---------------------------------------------------------------------------------*/
#include "EngineDec.h"
#include "Utils.hpp"

#ifndef TESTGAME_HPP
#define TESTGAME_HPP

//extern short tiles[128*128];
//extern short character[128*16];
//extern short clapperboard[128*64];
extern glImage tilesImage[64];
extern int BlockBumpedX, BlockBumpedY;
extern float fPlayerPosX, fPlayerPosY;
extern float maxTime;
extern int gems, score;
extern float inGameTime;
extern int nLevelWidth, nLevelHeight;
namespace Game {
	void UpdateGameLogic(float fElapsedTime);
	void Update();
	void Draw();
	void Init();
	void LoadContent();
};
namespace Title {
	void Update();
	void Init();
	void LoadContent();
};
void DrawTile(char sTileID, int x, int y, char sTileIDAbove = '.', bool renderInInventory = true, int tileX = 0, int tileY = 0);
#define DIR_UP 0x01
#define DIR_DOWN 0x02
#define DIR_LEFT 0x04
#define DIR_RIGHT 0x08
bool IsSolid(char c, int directions);
char GetTile(int x, int y);
void SetTile(char c, int x, int y);
void IncreaseMaxHP();
void DamagePlayer();
void HealPlayer();
typedef void DialogResultFunction(bool); // the bool represents saidYes
void ShowDialog(DialogResultFunction* f, const char* dialogYes, const char* dialogNo, const char* dlgRow1, const char* dlgRow2 = nullptr, const char* dlgRow3 = nullptr);
#endif//TESTGAME_HPP