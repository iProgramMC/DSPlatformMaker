/*---------------------------------------------------------------------------------
							GAME PROJECT
						  [ July 29, 2020 ]
					 Programmed by iProgramInCpp
					 
						  TestGame module
---------------------------------------------------------------------------------*/

#include "TestGame.hpp"
#include <math.h>
#include <chrono>
#include <vector>
#include <memory>
#include "character.h"
#include "tiles.h"
#include "clapperboard.h"
#include "undo.h"
#include "reset.h"
#include "erase.h"
#include "itemsel.h"
#include "item.h"
#include "lr_arrows.h"
#include "erase_mode.h"
#include "copy_mode.h"
#include "dialog_bg.h"
#include "dialogbtn.h"
#include "logo.h"
#include "Enemy.hpp"

// todo: add save
/*
short tiles[128*128];
short character[128*16];
short clapperboard[128*64];
short undoBmp[32*32];
short eraseBmp[32*32];
short lrArrowsBmp[32*32];
short resetBmp[32*32];
short itemSelBmp[32*32];
short itemBmp[32*32];
short ermodBmp[64*16];
*/

short textureCacheBmp[256*256];
bool hasPressedLROrStart = false;


bool isInCopyMode = false;
bool isDraggingStuff = false;
float worldDragPosX, worldDragPosY;
float lastWorldTouchPosX, lastWorldTouchPosY;
int draggedRectX1 = -1, draggedRectY1;
int draggedRectX2 = -1, draggedRectY2;

#include "Levels.inc.h"

char* sLevel, *sLevelBeforePlay;
int nLevelWidth, nLevelHeight;

bool bPlayerOnGround = false;

int nDirModX = 0, nDirModY = 0;

float fCameraPosX = 0.0f, fCameraPosY = 0.0f;
float fPlayerPosX = 0.0f, fPlayerPosY = 0.0f;
float fPlayerVelX = 0.0f, fPlayerVelY = 0.0f;

glImage tilesImage[64];
glImage playrImage[9];
glImage cbordImage[2];
glImage resetImage[1];
glImage eraseImage[1];
glImage itemSImage[1];
glImage itemImage[1];
glImage lrArrowImage[2];
glImage undoImage[1];
glImage ermodImage[1];
glImage cpmodImage[1];
glImage dlgBgImage[1];
glImage dlgBtnImage[1];
glImage logoImage[1];
int tileSpriteID, plyrSpriteID, cbrdSpriteID;
int lrArSpriteID;
int resetSpriteID;
int eraseSpriteID;
int itemSelSpriteID;
int itemSpriteID;
int undoSpriteID;
int ermodSpriteID;
int cpmodSpriteID;
int dlgBgSpriteID;
int dlgBtnSpriteID;
int logoSpriteID;

enum GameMode { 
	Mode_Game,
	Mode_Title,
	Mode_Maker
};
int gameMode = Mode_Title;

const char* dialogRow1, *dialogRow2, *dialogRow3, *dialogYesText, *dialogNoText;
DialogResultFunction *dialogResultFunc;
bool dialogShown;
int dialogPosition = 192;

void ShowDialog(DialogResultFunction* f, const char* dialogYes, const char* dialogNo, const char* dlgRow1, const char* dlgRow2, const char* dlgRow3) {
	dialogYesText = dialogYes;
	dialogNoText = dialogNo;
	dialogRow1 = dlgRow1;
	dialogRow2 = dlgRow2;
	dialogRow3 = dlgRow3;
	dialogResultFunc = f;
	dialogShown = true;
	dialogPosition = 192;
}

void ResetLevel(bool yes);

bool dialogButtonsClicked[2];

void DrawDialog(bool processTouch = true) {
	DrawImage(dlgBgImage, 256, 192, 0, dialogPosition);
	Rectangle rBtn[2] = { 
		{ 20, 136, 96, 32 }, 
		{ 138, 136, 96, 32 }
	};
	
	// draw text
	int m = dialogPosition;
	m += 50;
	DrawString((SCREEN_WIDTH - strlen(dialogRow1) * 6) / 2, m, BLACK, dialogRow1, 0);
	m += 9;
	DrawString((SCREEN_WIDTH - strlen(dialogRow2) * 6) / 2, m, BLACK, dialogRow2, 0);
	m += 9;
	DrawString((SCREEN_WIDTH - strlen(dialogRow3) * 6) / 2, m, BLACK, dialogRow3, 0);
	
	bool clicked[2] = { 0, 0 };
	Point pTouch = { touch.px, touch.py };
	for (int i = 0; i < 2; i++) {
		uint16_t color = 0xFFFF;
		rBtn[i].y += dialogPosition;
		if (processTouch) {
			if (RectangleContains(rBtn[i], pTouch)) {
				color &= 0xbdef;
				clicked[i] = true;
			}
		}
		DrawImageColored(dlgBtnImage, 128, 32, rBtn[i].x, rBtn[i].y, 0, color);
	}
	DrawString((96 - strlen(dialogNoText) * 6) / 2 + rBtn[0].x, rBtn[0].y + 12, BLACK, dialogNoText, 0);
	DrawString((96 - strlen(dialogYesText) * 6) / 2 + rBtn[1].x, rBtn[1].y + 12, BLACK, dialogYesText, 0);
	if (processTouch) {
		for (int i = 0; i < 2; i++) {
			if (!clicked[i] && dialogButtonsClicked[i]) {
				ResetLevel(i);
				dialogShown = false;
			}
		}
		memcpy(dialogButtonsClicked, clicked, sizeof(clicked));
	}
}

char GetTile(int x, int y){
	int cy = y;
	if (y < 0) y = 0;
	if(x >= 0 && x < nLevelWidth && y >= 0 && y < nLevelHeight){
		if (cy < y && sLevel[y*nLevelWidth+x] == 'o') return '.';
		return sLevel[y*nLevelWidth+x];
	}else{
		return ' ';
	}
}
	
void SetTile(char c, int x, int y){
	if(x >= 0 && x < nLevelWidth && y >= 0 && y < nLevelHeight){
		sLevel[y*nLevelWidth+x] = c;
	}
}

int GetDirModX() {
	return nDirModX;
}
const char* makeText = "Make", *playText = "Play";
bool hasClickedPreviously[2];

// enemy list
std::vector<std::unique_ptr<Enemy>> enemies;

float maxTime = 500;
int gems = 0, score = 0;
float inGameTime = maxTime;
bool isOnOffSwitchOn = true;
bool CanBeBumped(char c) {
	switch (c) {
		case '?':
		case '^':
		case 'Q':
		case 'A':
		case 'L':
		case 'B':
		case 'E':
			return true;
	}
	return false;
}
bool ending = false;
int endingStage = 0;
bool IsSolid(char c, int directions) {
	switch (c) {
		case 'o':
		case '(':
		case ')':
		case '{':
		case '}':
		case '&':
		case '.':
		case ' ':
		case 'S':
		case '0':
		case '8':
			return false;
		case '=':
			if (directions & DIR_DOWN) return true;
			return false;
		case 'F':
			return !isOnOffSwitchOn;
		case 'H':
			return isOnOffSwitchOn;
		default:
			return true;
	}
}

void SpawnEnemy(const Enemy enemy) {
	if (enemies.size() >= 100) {
		//LogMsg("Enemy limit reached");
		return;
	}
	auto u = std::make_unique<Enemy>(enemy);
	enemies.push_back(std::move(u));
}

void SpawnMushTypeRise(float x, float y, int type) {
	Enemy mush;
	mush.type = type;
	mush.posX = mush.homeX = floorf(x); mush.posY = mush.homeY = floorf(y);
	mush.mushroom.risingTimer = 1.0f;
	SpawnEnemy(mush);
}

int hp = 1, maxHP = 1;
float lastHP = 0.0f;
void IncreaseMaxHP() {
	maxHP++;
	hp = maxHP;
}

void AddToMaxHP(int i) {
	while (i > 0) {
		IncreaseMaxHP();
		i--;
	}
}

#define State_Idle 0
#define State_Jump 1
#define State_Walk 2
#define State_Run 3
#define State_Skid 4
#define State_Dead 5
#define State_Crouch 6
int playerState = State_Idle;

float gameTimer = 0;
float deathTimer = 0.0f;
bool canEditLevel = false;
int dieStage = 0;
bool doPlayerFallAfterDeath = false;

void ResetEverything();
void KillPlayer(bool doInstantly) {
	// play jingle, then wait a few seconds
	deathTimer = 0.0f;
	playerState = State_Dead;
	dieStage = 0;
	doPlayerFallAfterDeath = !doInstantly;
	fPlayerVelY = fPlayerVelX = 0;
}
void LoadLevelToMem(char* c, int w, int h);
void DoLevelInitStuff();

bool doSetToMakerMode = false;

float stunTimer = 0.0f;

void DamagePlayer() {
	if (stunTimer > 0.0f) return;
	hp--;
	if (hp <= 0) {
		hp = 0;
		KillPlayer(false);
		return;
	}
	stunTimer = 4.0f;
}
void HealPlayer() {
	hp++;
	while (hp > maxHP) {
		maxHP++;
	}
}

float BlockBumpTimer = 0.0f;
int BlockBumpedX = -1, BlockBumpedY = -1;
float endingTimer = 0;
void ResetEverything() {
	isOnOffSwitchOn = true;
	memcpy(sLevel, sLevelBeforePlay, nLevelHeight * nLevelWidth);
	enemies.clear();
	hp = 1;
	maxHP = 1;
	inGameTime = 500;
	gems = 0;
	score = 0;
	stunTimer = 0;
	lastHP = 1;
    ending = false;
	endingStage = 0;
	endingTimer = 0;
}
void SetToMakerMode() {
	doSetToMakerMode = true;
	BlockBumpTimer = 0.0f;
	BlockBumpedX = BlockBumpedY = -1;
	ResetEverything();
}

void SpawnBump(int x, int y) {
	if (!CanBeBumped(GetTile(x,y))) return;
	BlockBumpTimer = 0.0f;
	BlockBumpedX = x;
	BlockBumpedY = y;
}
void ResetLevel(bool yes) {
	if (!yes) return;
	//gameMode = Mode_Maker;
	SetToMakerMode();
	LoadLevelToMem(level_1, level_1_width, level_1_height);
	canEditLevel = true;
}

void Game::UpdateGameLogic(float fElapsedTime) {
	gameTimer += fElapsedTime;
	stunTimer -= fElapsedTime;
	if (stunTimer < 0) stunTimer = 0;
	if (inGameTime >= -1 && playerState != State_Dead && !ending) {
		inGameTime -= fElapsedTime;
	}
	if (inGameTime < 0 && !ending) {
		inGameTime = 0;
		KillPlayer(false);
	}
	/*
	if (GetKeyState(KEY_UP)) {
		fPlayerVelY = -6.0f;
	}
	if (GetKeyState(KEY_DOWN)) {
		fPlayerVelY = 6.0f;
	}*/
	
	BlockBumpTimer += fElapsedTime;
	if (BlockBumpTimer > .25f) {
		BlockBumpTimer = 0.0f;
		BlockBumpedX = BlockBumpedY = -1;
	}
	
	if (gameMode == Mode_Game) {
		if (canEditLevel) {
			if (GetKeyDownState(KEY_SELECT) || GetKeyDownState(KEY_START)) {
				SetToMakerMode();
			}
		}
	}
	
	if (endingStage == 1) {
		if (inGameTime >= 10) {
			inGameTime -= 10;
			score += 100;
		}
		if (inGameTime >= 1) {
			inGameTime--;
			score += 10;
		} else {
			endingStage++;
			endingTimer = 3;
		}
	}
	if (endingStage == 2) {
		endingTimer -= fElapsedTime;
		if (endingTimer <= 0.0f) {
			if (canEditLevel) {
				SetToMakerMode();
			}
			ResetEverything();
			DoLevelInitStuff();
		}
	}
	
	if (playerState == State_Dead) {
		deathTimer += fElapsedTime;
		if (deathTimer >= 1.0f && doPlayerFallAfterDeath) {
			if (dieStage == 0) {
				dieStage++;
				fPlayerVelY = -12.0f;
			} else {
				fPlayerVelY += 20.0f * fElapsedTime;
			}
		}
		fPlayerPosX += fPlayerVelX * fElapsedTime;
		fPlayerPosY += fPlayerVelY * fElapsedTime; // no collision
		if (deathTimer >= 5.0f) {
			if (canEditLevel) {
				SetToMakerMode();
			}
			ResetEverything();
			DoLevelInitStuff();
		}
	} else {
		if (fPlayerPosY > nLevelHeight + 3.0f) {
			KillPlayer(true);
		}
		float addSpeed = 6.0f;
		bool holdingRun = false;
		if(GetKeyState(KEY_Y) || GetKeyState(KEY_X)){
			addSpeed += 8.0f;
			holdingRun = true;
		}
		
		float groundFriction = 6.0f;
		bool hitDirKey = false;
		if (!ending) {
			if (playerState == State_Crouch) playerState = State_Idle;
			if(GetKeyState(KEY_DOWN)){
				playerState = State_Crouch;
			}
			if(GetKeyState(KEY_LEFT) && playerState != State_Crouch){
				hitDirKey = true;
				bool isSkidding = false;
				if (playerState == State_Idle || playerState == State_Walk || playerState == State_Run || playerState == State_Skid) {
					if (fPlayerVelX > 0) {
						playerState = State_Skid;
						isSkidding = true;
					} else {
						playerState = holdingRun ? State_Run : State_Walk;
					}
				}
				if (bPlayerOnGround && !isSkidding) nDirModY = 1;
				fPlayerVelX += -addSpeed * fElapsedTime;
				groundFriction = 2.0f;
			}
			if(GetKeyState(KEY_RIGHT) && playerState != State_Crouch){
				hitDirKey = true;
				bool isSkidding = false;
				if (playerState == State_Idle || playerState == State_Walk || playerState == State_Run || playerState == State_Skid) {
					if (fPlayerVelX < 0) {
						playerState = State_Skid;
						isSkidding = true;
					} else {
						playerState = holdingRun ? State_Run : State_Walk;
					}
				}
				if (bPlayerOnGround && !isSkidding) nDirModY = 0;
				fPlayerVelX += addSpeed * fElapsedTime;
				groundFriction = 2.0f;
			}
			if (!hitDirKey && (playerState == State_Walk || playerState == State_Run || playerState == State_Skid) && bPlayerOnGround) playerState = State_Idle;
			if (playerState == State_Jump && bPlayerOnGround) playerState = State_Idle;
			if ((GetKeyDownState(KEY_B) || GetKeyDownState(KEY_A)) && playerState != State_Crouch) {
				if(bPlayerOnGround){
					playerState = State_Jump;
					fPlayerVelY = -14.0f;
					nDirModX = 1;
				}
			}
			if (GetKeyDownState(KEY_SELECT)) {
				//KillPlayer(false);
			}
		} else {
			// go right
			if (endingStage == 0 && bPlayerOnGround) {
				fPlayerVelX = 7;
				playerState = State_Run;
			}
		}
		fPlayerVelY += 20.0f * fElapsedTime;
			
		if (bPlayerOnGround) {
			fPlayerVelX += -groundFriction * fPlayerVelX * fElapsedTime;
			if(fabs(fPlayerVelX) < 0.01f){
				fPlayerVelX = 0.0f;
			}
		}
		
		if (fPlayerVelX > 10.0f) fPlayerVelX = 10.0f;
		if (fPlayerVelX < -10.0f) fPlayerVelX = -10.0f;
		if (fPlayerVelY > 100.0f) fPlayerVelY = 100.0f;
		if (fPlayerVelY < -100.0f) fPlayerVelY = -100.0f;
		bPlayerOnGround = false;
		for (int i = 0; i < 5; i++) {
			float fNewPlayerPosX = fPlayerPosX + fPlayerVelX * fElapsedTime * .2f;
			float fNewPlayerPosY = fPlayerPosY + fPlayerVelY * fElapsedTime * .2f;
			if (fNewPlayerPosX < 0) fNewPlayerPosX = 0;
			if (ending) {
				if (fNewPlayerPosX >= nLevelWidth) {
					fNewPlayerPosX = nLevelWidth;
					if (endingStage == 0) endingStage++;
					fPlayerVelX = 0;
				}
			} else {
				if (fNewPlayerPosX >= nLevelWidth - 1) {
					fNewPlayerPosX = nLevelWidth - 1;
				}
			}
			
			if (GetTile(fNewPlayerPosX + 0.0f, fNewPlayerPosY + 0.0f) == 'o')
			{ SetTile('.', fNewPlayerPosX + 0.0f, fNewPlayerPosY + 0.0f); gems++; score += 100; }
			if (GetTile(fNewPlayerPosX + 0.0f, fNewPlayerPosY + 1.0f) == 'o')
			{ SetTile('.', fNewPlayerPosX + 0.0f, fNewPlayerPosY + 1.0f); gems++; score += 100; }
			if (GetTile(fNewPlayerPosX + 1.0f, fNewPlayerPosY + 0.0f) == 'o')
			{ SetTile('.', fNewPlayerPosX + 1.0f, fNewPlayerPosY + 0.0f); gems++; score += 100; }
			if (GetTile(fNewPlayerPosX + 1.0f, fNewPlayerPosY + 1.0f) == 'o')
			{ SetTile('.', fNewPlayerPosX + 1.0f, fNewPlayerPosY + 1.0f); gems++; score += 100; }
			
			if (GetTile(fNewPlayerPosX + 0.0f, fNewPlayerPosY + 0.0f) == '8')
			{ SetTile('0', fNewPlayerPosX + 0.0f, fNewPlayerPosY + 0.0f); ending = true; endingStage = 0; fPlayerVelX = 0; }
			if (GetTile(fNewPlayerPosX + 0.0f, fNewPlayerPosY + 1.0f) == '8')
			{ SetTile('0', fNewPlayerPosX + 0.0f, fNewPlayerPosY + 1.0f); ending = true; endingStage = 0; fPlayerVelX = 0;  }
			if (GetTile(fNewPlayerPosX + 1.0f, fNewPlayerPosY + 0.0f) == '8')
			{ SetTile('0', fNewPlayerPosX + 1.0f, fNewPlayerPosY + 0.0f); ending = true; endingStage = 0; fPlayerVelX = 0;  }
			if (GetTile(fNewPlayerPosX + 1.0f, fNewPlayerPosY + 1.0f) == '8')
			{ SetTile('0', fNewPlayerPosX + 1.0f, fNewPlayerPosY + 1.0f); ending = true; endingStage = 0; fPlayerVelX = 0;  }
				
				
			//Collision
			if(fPlayerVelX < 0){
				if (IsSolid(GetTile(fNewPlayerPosX + 0.0f, fPlayerPosY + 0.0f), DIR_LEFT) ||
					IsSolid(GetTile(fNewPlayerPosX + 0.0f, fPlayerPosY + 0.9f), DIR_LEFT)){
					fNewPlayerPosX = (int)fNewPlayerPosX + 1;
					fPlayerVelX = 0;
				}
			}
			else if (fPlayerVelX > 0) {
				if (IsSolid(GetTile(fNewPlayerPosX + 1.0f, fPlayerPosY + 0.0f), DIR_RIGHT) ||
					IsSolid(GetTile(fNewPlayerPosX + 1.0f, fPlayerPosY + 0.9f), DIR_RIGHT)){
					fNewPlayerPosX = (int)fNewPlayerPosX;
					fPlayerVelX = 0;
					if (ending) {
						if (endingStage == 0) endingStage++; // initializes the countdown etc
						playerState = State_Crouch;
						nDirModY = 1;
					}
				}
			}
			
			if (fPlayerVelY < 0) {
				char c1 = GetTile(fNewPlayerPosX + 0.0f, fNewPlayerPosY), c2 = GetTile(fNewPlayerPosX + 0.9f, fNewPlayerPosY);
				bool s1 = IsSolid(c1, DIR_UP);
				bool s2 = IsSolid(c2, DIR_UP);
				if (s1 || s2){
					if (nDirModY) {
						if (s2) {
							SpawnBump(fNewPlayerPosX + 0.9f, fNewPlayerPosY);
							if (c2 == '?') { gems++; score += 100; SetTile('U', fNewPlayerPosX + 0.9f, fNewPlayerPosY); }
							if (c2 == '^') { SetTile('U', fNewPlayerPosX + 0.9f, fNewPlayerPosY); SpawnMushTypeRise(fNewPlayerPosX + 0.9f, fNewPlayerPosY - .5f, Enemy_Mushroom); }
							if (c2 == 'Q') { SetTile('U', fNewPlayerPosX + 0.9f, fNewPlayerPosY); SpawnMushTypeRise(fNewPlayerPosX + 0.9f, fNewPlayerPosY - .5f, Enemy_Flower); }
							if (c2 == 'A') { SetTile('U', fNewPlayerPosX + 0.9f, fNewPlayerPosY); SpawnMushTypeRise(fNewPlayerPosX + 0.9f, fNewPlayerPosY - .5f, Enemy_Time); }
							if (c2 == 'L') { SetTile('U', fNewPlayerPosX + 0.9f, fNewPlayerPosY); SpawnMushTypeRise(fNewPlayerPosX + 0.9f, fNewPlayerPosY - .5f, Enemy_LifeUp); }
							if (c2 == 'E') { isOnOffSwitchOn ^= 1;}
							c1 = GetTile(fNewPlayerPosX + 0.0f, fNewPlayerPosY); c2 = GetTile(fNewPlayerPosX + 0.9f, fNewPlayerPosY);
						}
						if (s1)  {
							SpawnBump(fNewPlayerPosX + 0.0f, fNewPlayerPosY);
							if (c1 == '?') { gems++; score += 100; SetTile('U', fNewPlayerPosX + 0.0f, fNewPlayerPosY); }
							if (c1 == '^') { SetTile('U', fNewPlayerPosX + 0.0f, fNewPlayerPosY); SpawnMushTypeRise(fNewPlayerPosX + 0.0f, fNewPlayerPosY - .5f, Enemy_Mushroom); }
							if (c1 == 'Q') { SetTile('U', fNewPlayerPosX + 0.0f, fNewPlayerPosY); SpawnMushTypeRise(fNewPlayerPosX + 0.0f, fNewPlayerPosY - .5f, Enemy_Flower); }
							if (c1 == 'A') { SetTile('U', fNewPlayerPosX + 0.0f, fNewPlayerPosY); SpawnMushTypeRise(fNewPlayerPosX + 0.0f, fNewPlayerPosY - .5f, Enemy_Time); }
							if (c1 == 'L') { SetTile('U', fNewPlayerPosX + 0.0f, fNewPlayerPosY); SpawnMushTypeRise(fNewPlayerPosX + 0.0f, fNewPlayerPosY - .5f, Enemy_LifeUp); }
							if (c1 == 'E' && c2 != 'E') { isOnOffSwitchOn ^= 1;}
							c1 = GetTile(fNewPlayerPosX + 0.0f, fNewPlayerPosY); c2 = GetTile(fNewPlayerPosX + 0.9f, fNewPlayerPosY);
						}
					}
					else {
						if (s1)  {
							SpawnBump(fNewPlayerPosX + 0.0f, fNewPlayerPosY);
							if (c1 == '?') { gems++; score += 100; SetTile('U', fNewPlayerPosX + 0.0f, fNewPlayerPosY); }
							if (c1 == '^') { SetTile('U', fNewPlayerPosX + 0.0f, fNewPlayerPosY); SpawnMushTypeRise(fNewPlayerPosX + 0.0f, fNewPlayerPosY - .5f, Enemy_Mushroom); }
							if (c1 == 'Q') { SetTile('U', fNewPlayerPosX + 0.0f, fNewPlayerPosY); SpawnMushTypeRise(fNewPlayerPosX + 0.0f, fNewPlayerPosY - .5f, Enemy_Flower); }
							if (c1 == 'A') { SetTile('U', fNewPlayerPosX + 0.0f, fNewPlayerPosY); SpawnMushTypeRise(fNewPlayerPosX + 0.0f, fNewPlayerPosY - .5f, Enemy_Time); }
							if (c1 == 'L') { SetTile('U', fNewPlayerPosX + 0.0f, fNewPlayerPosY); SpawnMushTypeRise(fNewPlayerPosX + 0.0f, fNewPlayerPosY - .5f, Enemy_LifeUp); }
							if (c1 == 'E') { isOnOffSwitchOn ^= 1;}
							c1 = GetTile(fNewPlayerPosX + 0.0f, fNewPlayerPosY); c2 = GetTile(fNewPlayerPosX + 0.9f, fNewPlayerPosY);
						}
						if (s2) {
							SpawnBump(fNewPlayerPosX + 0.9f, fNewPlayerPosY);
							if (c2 == '?') { gems++; score += 100; SetTile('U', fNewPlayerPosX + 0.9f, fNewPlayerPosY); }
							if (c2 == '^') { SetTile('U', fNewPlayerPosX + 0.9f, fNewPlayerPosY); SpawnMushTypeRise(fNewPlayerPosX + 0.9f, fNewPlayerPosY - .5f, Enemy_Mushroom); }
							if (c2 == 'Q') { SetTile('U', fNewPlayerPosX + 0.9f, fNewPlayerPosY); SpawnMushTypeRise(fNewPlayerPosX + 0.9f, fNewPlayerPosY - .5f, Enemy_Flower); }
							if (c2 == 'A') { SetTile('U', fNewPlayerPosX + 0.9f, fNewPlayerPosY); SpawnMushTypeRise(fNewPlayerPosX + 0.9f, fNewPlayerPosY - .5f, Enemy_Time); }
							if (c2 == 'L') { SetTile('U', fNewPlayerPosX + 0.9f, fNewPlayerPosY); SpawnMushTypeRise(fNewPlayerPosX + 0.9f, fNewPlayerPosY - .5f, Enemy_LifeUp); }
							if (c2 == 'E' && c1 != 'E') { isOnOffSwitchOn ^= 1;}
							c1 = GetTile(fNewPlayerPosX + 0.0f, fNewPlayerPosY); c2 = GetTile(fNewPlayerPosX + 0.9f, fNewPlayerPosY);
						}
					}
					fNewPlayerPosY = (int)fNewPlayerPosY + 1;
					fPlayerVelY = 0;
				}
			}
			else if (fPlayerVelY > 0) {
				if (IsSolid(GetTile(fNewPlayerPosX + 0.0f, fNewPlayerPosY+1), DIR_DOWN) ||
					IsSolid(GetTile(fNewPlayerPosX + 0.9f, fNewPlayerPosY+1), DIR_DOWN)){
					fNewPlayerPosY = (int)fNewPlayerPosY;
					fPlayerVelY = 0;
					bPlayerOnGround = true;
					nDirModX = 0;
				}
			}
			fPlayerPosX = fNewPlayerPosX;
			fPlayerPosY = fNewPlayerPosY;
		}
		// update enemies
		for (uint32_t i = 0; i < enemies.size(); i++) {
			enemies.at(i)->Update(fElapsedTime);
		}
begin_deleting_shit:
		for (uint32_t i = 0; i < enemies.size(); i++) {
			if (enemies.at(i)->markForRemoval) {
				// here we have the mark for removal shit, remove 
				// and restart the loop (not most efficient but it will do!)
				enemies.erase(enemies.begin() + i);
				goto begin_deleting_shit;
			}
		}
	}
	switch (playerState) {
		case State_Idle: nDirModX = 0; break;
		case State_Walk: {
			int r = (int)(gameTimer * 48.f); // every second represents 48 ticks, caps at 24 (.5s)
			r %= 24;
			nDirModX = 2 + r / 8;
			break;
		}
		case State_Run: {
			int r = (int)(gameTimer * 24.f); // every second represents 24 ticks, caps at 6 (.25s)
			r %= 6;
			nDirModX = 2 + r / 2;
			break;
		}
		case State_Jump: nDirModX = 1; break;
		case State_Skid: nDirModX = 5; break;
		case State_Dead: nDirModX = 6; break;
		case State_Crouch: nDirModX = 7; break;
	}
	while (gameTimer > 2048) {
		gameTimer -= 2048.f;
	}
	fCameraPosX = fPlayerPosX + .5f;
	fCameraPosY = fPlayerPosY + .5f;
	
}
void DrawTile(char sTileID, int x, int y, char sTileIDAbove, bool renderInInventory, int tileX, int tileY) {
	//^QAS
	switch(sTileID){
		case '.': break;
		case '#':DrawImage(&tilesImage[2], 16, 16,x, y);break;
		case 'o':DrawImage(&tilesImage[3], 16, 16,x, y);break;
		case '?':DrawImage(&tilesImage[9], 16, 16,x, y);break;
		case '^':DrawImage(&tilesImage[gameMode == Mode_Maker ? 41 : 9], 16, 16,x, y); break;
		case 'Q':DrawImage(&tilesImage[gameMode == Mode_Maker ? 44 : 9], 16, 16,x, y); break;
		case 'A':DrawImage(&tilesImage[gameMode == Mode_Maker ? 45 : 9], 16, 16,x, y); break;
		case 'L':DrawImage(&tilesImage[gameMode == Mode_Maker ? 46 : 9], 16, 16,x, y); break;
		case 'B':DrawImage(&tilesImage[8], 16, 16,x, y);break;
		case 'G': {
			if (sTileIDAbove == 'G') 
				DrawImage(&tilesImage[0], 16, 16,x, y);
			else
				DrawImage(&tilesImage[16], 16, 16,x, y);
			break;
		}
		case 'P': {
			if (sTileIDAbove == 'P') 
				DrawImage(&tilesImage[35], 16, 16,x, y);
			else
				DrawImage(&tilesImage[34], 16, 16,x, y);
			break;
		}
		case 'D':DrawImage(&tilesImage[0], 16, 16,x, y);break;
		case 'U':DrawImage(&tilesImage[1], 16, 16,x, y);break;
		case '1':DrawImage(&tilesImage[18], 16, 16,x, y);break;
		case '!':DrawImage(&tilesImage[19], 16, 16,x, y);break;
		case '~':DrawImage(&tilesImage[20], 16, 16,x, y);break;
		case '`':DrawImage(&tilesImage[21], 16, 16,x, y);break;
		case '(':DrawImage(&tilesImage[4], 16, 16,x, y);break;
		case ')':DrawImage(&tilesImage[5], 16, 16,x, y);break;
		case '|': {
			if (sTileIDAbove == '|') 
				DrawImage(&tilesImage[7], 16, 16,x, y);
			else
				DrawImage(&tilesImage[6], 16, 16,x, y);
			break;
		}
		case 'S':
			if (gameMode == Mode_Maker) {
				DrawImage(&tilesImage[13], 16, 16,x, y);break;
			} else return;
		case '{':DrawImage(&tilesImage[12], 16, 16,x, y);break;
		case '}':DrawImage(&tilesImage[10], 16, 16,x, y);break;
		case '&':DrawImage(&tilesImage[11], 16, 16,x, y);break;
		case '=':DrawImage(&tilesImage[24], 16, 16,x, y);break;
		case '>':DrawImage(&tilesImage[22], 16, 16,x, y);break;
		case 'E':DrawImage(&tilesImage[isOnOffSwitchOn?25:26], 16, 16,x, y);break;
		case 'F':DrawImage(&tilesImage[isOnOffSwitchOn?27:29], 16, 16,x, y);break;
		case 'H':DrawImage(&tilesImage[isOnOffSwitchOn?30:28], 16, 16,x, y);break;
		case '0':DrawImage(&tilesImage[52], 16, 16,x, y);break;
		case '8':{
			// Draw the end marker
			int frame = (int)(gameTimer * 8) % 4;
			DrawImage(&tilesImage[48 + frame], 16, 16, x, y);
			break;
		}
		case 'W': {
			// spawn flower
			if (renderInInventory || gameMode == Mode_Maker) {
				DrawImage(&tilesImage[36], 16, 16,x, y);
			} else {
				Enemy mush;
				mush.type = Enemy_Flower;
				mush.posX = tileX; mush.posY = tileY; mush.homeX = 0; mush.homeY = 0; mush.mushroom.risingTimer = 0.0f;
				SpawnEnemy(mush); SetTile('.',  tileX, tileY); DrawTile('.', x, y, sTileIDAbove, renderInInventory);
			}
			break;
		}
		case 'X': {
			// spawn life up
			if (renderInInventory || gameMode == Mode_Maker) {
				DrawImage(&tilesImage[38], 16, 16,x, y);
			} else {
				Enemy mush;
				mush.type = Enemy_LifeUp;
				mush.posX = tileX; mush.posY = tileY; mush.homeX = 0; mush.homeY = 0; mush.mushroom.risingTimer = 0.0f;
				SpawnEnemy(mush); SetTile('.',  tileX, tileY); DrawTile('.', x, y, sTileIDAbove, renderInInventory);
			}
			break;
		}
		case 'T': {
			// spawn extra clock?
			if (renderInInventory || gameMode == Mode_Maker) {
				DrawImage(&tilesImage[37], 16, 16,x, y);
			} else {
				Enemy mush;
				mush.type = Enemy_Time;
				mush.posX = tileX; mush.posY = tileY; mush.homeX = 0; mush.homeY = 0; mush.mushroom.risingTimer = 0.0f;
				SpawnEnemy(mush); SetTile('.',  tileX, tileY); DrawTile('.', x, y, sTileIDAbove, renderInInventory);
			}
			break;
		}
		case 'M': {
			// spawn mushroom?
			if (renderInInventory || gameMode == Mode_Maker) {
				DrawImage(&tilesImage[33], 16, 16,x, y);
			} else {
				Enemy mush;
				mush.type = Enemy_Mushroom;
				mush.posX = tileX; mush.posY = tileY; mush.homeX = 0; mush.homeY = 0; mush.mushroom.risingTimer = 0.0f;
				SpawnEnemy(mush); SetTile('.',  tileX, tileY); DrawTile('.', x, y, sTileIDAbove, renderInInventory);
			}
			break;
		}
		default: DrawImage(&tilesImage[63], 16, 16,x, y);break;
			//FillRectangle(RGBA16(0, 12, 31, 1), x, y, 16, 16);
			//break;
	}
}

bool isInEraseMode = false;
bool isDrawing = false;
bool multigrabMode = false; // unused, todo
void Game::Draw() {
	
	//Draw Level
	int nTileWidth = 16;
	int nTileHeight = 16;
	int nVisibleTilesX = SCREEN_WIDTH / nTileWidth;
	int nVisibleTilesY = SCREEN_HEIGHT/nTileHeight;
	
	float fOffsetX = fCameraPosX - (float)nVisibleTilesX / 2.0f;
	float fOffsetY = fCameraPosY - (float)nVisibleTilesY / 2.0f;
	
	if(fOffsetX <= 0.0f) fOffsetX = 0.0f;
	if(fOffsetY <= 0.0f) fOffsetY = 0.0f;
	if(fOffsetX > (float)(nLevelWidth - nVisibleTilesX)) fOffsetX = nLevelWidth - nVisibleTilesX;
	if(fOffsetY > (float)(nLevelHeight- nVisibleTilesY)) fOffsetY = nLevelHeight- nVisibleTilesY;
	
	float fTileOffsetX = (fOffsetX - (int)fOffsetX) * nTileWidth;
	float fTileOffsetY = (fOffsetY - (int)fOffsetY) * nTileHeight;
	
					
	for(int x = 0; x <= nVisibleTilesX; x++){
		for(int y = 0; y <= nVisibleTilesY; y++){
			int tileX = floorf((float)x + fOffsetX), tileY = floorf((float)y+fOffsetY);
			char sTileID = GetTile(tileX, tileY);
			char sTileIDAbove = GetTile(tileX, tileY - 1);
			int BumpedBlockDrawY = 0;
			if (tileX == BlockBumpedX && tileY == BlockBumpedY) {
				BumpedBlockDrawY = (int)(sinf(BlockBumpTimer * 12.f) * -8.f);
			}
			bool shouldDraw = true;
			if (gameMode == Mode_Maker) 
			{
				Rectangle r1 = { draggedRectX1, draggedRectY1, draggedRectX2 - draggedRectX1 + 1, draggedRectY2 - draggedRectY1 + 1 };
				Point p = { tileX, tileY };
				shouldDraw = !isDraggingStuff || !RectangleContains(r1,p) || isInCopyMode;
			}
			if (shouldDraw) 
				DrawTile(sTileID, x*nTileWidth-fTileOffsetX, y*nTileHeight-fTileOffsetY + BumpedBlockDrawY, sTileIDAbove, false, tileX, tileY);
		}
	}
	
	if (multigrabMode) {
		int px1 = (draggedRectX1 - fOffsetX) * nLevelWidth;
		int py1 = (draggedRectY1 - fOffsetY) * nLevelWidth;
		int px2 = (draggedRectX2 - fOffsetX) * nLevelWidth;
		int py2 = (draggedRectY2 - fOffsetY) * nLevelWidth;
		DrawRectangle(YELLOW, px1, py1, px2, py2);
	}
	
	// draw the dragged tiles?
	if (isDraggingStuff) {
		for(int x = draggedRectX1; x <= draggedRectX2; x++){
			for(int y = draggedRectY1; y <= draggedRectY2; y++){
				int posX = ((float)x + (float)worldDragPosX / (float)nTileWidth - (float)draggedRectX1 - fOffsetX) * nTileWidth;
				int posY = ((float)y + (float)worldDragPosY / (float)nTileWidth - (float)draggedRectY1 - fOffsetY) * nTileHeight;
				DrawTile(GetTile(x, y), posX, posY, '.', false, x, y);
			}
		}
	}
	
	bool doDisplayChar = true;
	if (stunTimer > 0) {
		doDisplayChar = GlobalTimer % 2;
	}
	if (doDisplayChar) {
		if (nDirModY)
			DrawImageFlippedH(&playrImage[nDirModX], nTileWidth, nTileHeight, (fPlayerPosX - fOffsetX)*nTileWidth, (fPlayerPosY - fOffsetY)*nTileHeight);
		else
			DrawImage(&playrImage[nDirModX], nTileWidth, nTileHeight, (fPlayerPosX - fOffsetX)*nTileWidth, (fPlayerPosY - fOffsetY)*nTileHeight);
	}
	for (uint32_t i = 0; i < enemies.size(); i++) {
		enemies.at(i)->Draw(fOffsetX, fOffsetY);
	}
}
bool hasInventoryOpen = false;
bool hasRightMenuOpen = false;
bool hasClickedPreviouslyMaker[15];

char hotbarSlots[8];
int inventoryHotbarSelectedIdx = 0;
int hotbarSlotPage = 0;

#define HOTBAR_SLOT_PAGE_COUNT 3
char hotbarSlotPages[] = //{
	//"#o?^BG{}"
	//"P()+|S&="
	//"EFH....."
//};
"#oS8BG{}"
"P(?MTWX)"
"|&=EFH..";

extern bool isFileSystemEnabled;
void EmptyFunctionForInfoboxes(bool t) {}

bool hasShowedFatUnusableDlg = false;
void Game::Update() {
	ClearScreen(RGBA16(1, 27, 31, 1));
	if (dialogShown) {
		// TODO
		dialogPosition -= dialogPosition / 2; // todo: make it independent of fps
		if (dialogPosition <= 1) {
			dialogPosition = 0;
		} else {
			//glPolyFmt(POLY_ALPHA(3));
			//FillRectangle(BLACK, 0, 0, 256, 192);
			//glPolyFmt(POLY_ALPHA(31));
		}
	} else {
		// go back down
		if (dialogPosition == 0) dialogPosition = 1;
		dialogPosition *= 2;
		if (dialogPosition > 192) 
			dialogPosition = 192;
	}
	if (dialogShown) goto drawDialogAndReturn; // pause the game
	
	//
	{
		float fElapsedTime = 0.0166667f;
		if (gameMode != Mode_Maker) UpdateGameLogic(fElapsedTime);
		Draw();
		
		switch (gameMode) {
			case Mode_Maker: {
				Point pTouch = { touch.px, touch.py };
				bool canTouchLevel = true;
				if (hasInventoryOpen) {
					Rectangle r = { 220, 5, 32, 16 };
					u16 color = 0xffff;
					bool click = false;
					click = RectangleContains(r, pTouch);
					if (click) {
						color &= 0xBDEF; // 127,127,127
						canTouchLevel = false;
					}
					DrawImageColored(&lrArrowImage[1], r.width, r.height, r.x, r.y, 0, color);
					if (hasClickedPreviouslyMaker[0] && !click) {
						hasInventoryOpen = false;
					}
					hasClickedPreviouslyMaker[0] = click;
					
					for (int i = 0; i < 9; i++) {
						Rectangle r = { 2 + i * 24, 1, 24, 28 };
						u16 color = 0xffff;
						click = RectangleContains(r, pTouch);
						if (click) {
							color &= 0xBDEF; // 127,127,127
							canTouchLevel = false;
						}
						// draw inventory slot shit
						char c;
						if (i < 8) c = hotbarSlots[i]; else  c = '>';
						if (inventoryHotbarSelectedIdx == i) {
							DrawImageColored(&itemSImage[0], r.width, r.height, r.x, r.y, 0, color);
						} else {
							DrawImageColored(&itemImage[0], r.width, r.height, r.x, r.y, 0, color);
						}
						// draw the tile itself slightly lower and to the right
						DrawTile(c, r.x + 4, r.y + 4);
						if (hasClickedPreviouslyMaker[i+1] && !click) {
							if (i < 8) inventoryHotbarSelectedIdx = i; else {
								hotbarSlotPage++;
								if (hotbarSlotPage >= HOTBAR_SLOT_PAGE_COUNT) {
									hotbarSlotPage = 0;
								}
								memcpy(hotbarSlots, 8 * hotbarSlotPage + hotbarSlotPages, 8);
							}
						}
						hasClickedPreviouslyMaker[i+1] = click;
					}
				} else {
					Rectangle r = { 0, 5, 32, 16 };
					u16 color = 0xffff;
					bool click = false;
					click = RectangleContains(r, pTouch);
					if (click) {
						color &= 0xBDEF; // 127,127,127
						canTouchLevel = false;
					}
					DrawImageColored(&lrArrowImage[0], r.width, r.height, r.x, r.y, 0, color);
					if (hasClickedPreviouslyMaker[0] && !click) {
						hasInventoryOpen = true;
					}
					hasClickedPreviouslyMaker[0] = click;
				}
				if (hasRightMenuOpen) {
					Rectangle r = { 200, 25, 32, 16 };
					u16 color = 0xffff;
					bool click = false;
					click = RectangleContains(r, pTouch);
					if (click) {
						color &= 0xBDEF; // 127,127,127
						canTouchLevel = false;
					}
					DrawImageColored(&lrArrowImage[0], r.width, r.height, r.x, r.y, 0, color);
					if (hasClickedPreviouslyMaker[10] && !click) {
						hasRightMenuOpen = false;
					}
					hasClickedPreviouslyMaker[10] = click;
					
					r = { 256 - 34, 23, 34, 70 };
					FillRectangle(RGBA16(0x1A, 0x1A, 0x1B, 1), r.x, r.y, r.width, r.height);
					if (RectangleContains(r, pTouch)) {
						canTouchLevel = false;
					}
					r = { 226, 26, 28, 30 };
					color = 0xffff;
					click = false;
					if (RectangleContains(r, pTouch)) {
						color &= 0xBDEF; // 127,127,127
						canTouchLevel = false;
						click = true;
					}
					DrawImageColored(&resetImage[0], r.width, r.height, r.x, r.y, 0, color);
					if (!click && hasClickedPreviouslyMaker[11]) {
						//LogMsg("Tried to reset");
						ShowDialog(ResetLevel, "Yes", "No", "Do you really want to", "reset this level? You will", "lose all the progress you've made.");
					}
					hasClickedPreviouslyMaker[11] = click;
					r = { 226, 60, 28, 30 };
					color = 0xffff;
					click = false;
					if (RectangleContains(r, pTouch)) {
						color &= 0xBDEF; // 127,127,127
						canTouchLevel = false;
						click = true;
					}
					DrawImageColored(&eraseImage[0], r.width, r.height, r.x, r.y, 0, color);
					if (!click && hasClickedPreviouslyMaker[12]) {
						// too bad we wont add dialog yet
						isInEraseMode ^= 1;
					}
					hasClickedPreviouslyMaker[12] = click;
				} else {
					Rectangle r = { 230, 25, 32, 16 };
					u16 color = 0xffff;
					bool click = false;
					click = RectangleContains(r, pTouch);
					if (click) {
						color &= 0xBDEF; // 127,127,127
						canTouchLevel = false;
					}
					DrawImageColored(&lrArrowImage[1], r.width, r.height, r.x, r.y, 0, color);
					if (hasClickedPreviouslyMaker[10] && !click) {
						hasRightMenuOpen = true;
					}
					hasClickedPreviouslyMaker[10] = click;
				}
				if (isInEraseMode) isInCopyMode = false;
				/**/ if (isInEraseMode) DrawImage(&ermodImage[0], 64, 16, 0, hasInventoryOpen ? 25 : 0);
				else if (isInCopyMode)  DrawImage(&cpmodImage[0], 64, 16, 0, hasInventoryOpen ? 25 : 0);
				if (GetKeyDownState(KEY_START)) {
					gameMode = Mode_Game;
					DoLevelInitStuff();
				}
				if (GetKeyDownState(KEY_SELECT)) {
					gameMode = Mode_Game;
					memcpy(sLevelBeforePlay, sLevel, nLevelHeight * nLevelWidth);
				}
				
				int nTileWidth = 16;
				int nTileHeight = 16;
				int nVisibleTilesX = SCREEN_WIDTH / nTileWidth;
				int nVisibleTilesY = SCREEN_HEIGHT/nTileHeight;
				if (canTouchLevel) {
					float fOffsetX = fCameraPosX - (float)nVisibleTilesX / 2.0f;
					float fOffsetY = fCameraPosY - (float)nVisibleTilesY / 2.0f;
					
					if(fOffsetX <= 0.0f) fOffsetX = 0.0f;
					if(fOffsetY <= 0.0f) fOffsetY = 0.0f;
					if(fOffsetX > (float)(nLevelWidth - nVisibleTilesX)) fOffsetX = nLevelWidth - nVisibleTilesX;
					if(fOffsetY > (float)(nLevelHeight- nVisibleTilesY)) fOffsetY = nLevelHeight- nVisibleTilesY;
					
					int worldPosX = pTouch.x + fOffsetX * 16;
					int worldPosY = pTouch.y + fOffsetY * 16;
					int tilePosX = worldPosX / 16;
					int tilePosY = worldPosY / 16;
					
					if (pTouch.x != 0 && pTouch.y != 0) { // has touched screen
						if (isInEraseMode) {
							SetTile('.', tilePosX, tilePosY);
							isDraggingStuff = false;
						} else if (multigrabMode) {
							if (isDrawing) {
								draggedRectX2 = tilePosX;
								draggedRectY2 = tilePosY;
							} else {
								if (draggedRectX1 != -1) {
									isDraggingStuff = true;
									worldDragPosX = draggedRectX1 * 16;
									worldDragPosY = draggedRectY1 * 16;
								} else {
									isDrawing = true;
									draggedRectX1 = tilePosX;
									draggedRectY1 = tilePosY;
								}
							}
						} else {
							if (isDraggingStuff) {
								worldDragPosX += worldPosX - lastWorldTouchPosX;
								worldDragPosY += worldPosY - lastWorldTouchPosY;
							} else {
								char c = hotbarSlots[inventoryHotbarSelectedIdx];
								if (c != '.') {
									if (GetTile(tilePosX, tilePosY) == '.' || isDrawing) {
										SetTile(c, tilePosX, tilePosY);
										isDrawing = true;
									} else {
										// drag
										isDraggingStuff = true;
										draggedRectX1 = tilePosX;
										draggedRectX2 = tilePosX;
										draggedRectY1 = tilePosY;
										draggedRectY2 = tilePosY;
										
										worldDragPosX = tilePosX * 16;
										worldDragPosY = tilePosY * 16;
									}
								}
							}
						}
					} else {
						isDrawing = false;
						if (isDraggingStuff) {
							isDraggingStuff = false;
							/*
							// release?
							bool goRightToLeft = worldDragPosX > draggedRectX1;
							bool goDownToUp = worldDragPosY > draggedRectY1;
							
							
							if (goDownToUp) {
								if (goRightToLeft) {
									for (int i = draggedRectX2; i >= draggedRectX1; i--) {
										for (int j = draggedRectY2; j >= draggedRectY1; j--) {
											int offsetX = (int)roundf(worldDragPosX / 16.f) + i - draggedRectX1;
											int offsetY = (int)roundf(worldDragPosY / 16.f) + j - draggedRectY1;
											char c = GetTile(i, j);
											SetTile('.', i, j);
											SetTile(c, offsetX, offsetY);
										}
									}
								} else {
									for (int i = draggedRectX2; i >= draggedRectX1; i--) {
										for (int j = draggedRectY1; j <= draggedRectY1; j++) {
											int offsetX = (int)roundf(worldDragPosX / 16.f) + i - draggedRectX1;
											int offsetY = (int)roundf(worldDragPosY / 16.f) + j - draggedRectY1;
											char c = GetTile(i, j);
											SetTile('.', i, j);
											SetTile(c, offsetX, offsetY);
										}
									}
								}
							} else {
								if (goRightToLeft) {
									for (int i = draggedRectX1; i <= draggedRectX2; i++) {
										for (int j = draggedRectY2; j >= draggedRectY1; j--) {
											int offsetX = (int)roundf(worldDragPosX / 16.f) + i - draggedRectX1;
											int offsetY = (int)roundf(worldDragPosY / 16.f) + j - draggedRectY1;
											char c = GetTile(i, j);
											SetTile('.', i, j);
											SetTile(c, offsetX, offsetY);
										}
									}
								} else {
									for (int i = draggedRectX1; i <= draggedRectX2; i++) {
										for (int j = draggedRectY1; j <= draggedRectY1; j++) {
											int offsetX = (int)roundf(worldDragPosX / 16.f) + i - draggedRectX1;
											int offsetY = (int)roundf(worldDragPosY / 16.f) + j - draggedRectY1;
											char c = GetTile(i, j);
											SetTile('.', i, j);
											SetTile(c, offsetX, offsetY);
										}
									}
								}
							}*/
							int i = draggedRectX1;
							int j = draggedRectY1;
							int offsetX = (int)roundf(worldDragPosX / 16.f) + i - draggedRectX1;
							int offsetY = (int)roundf(worldDragPosY / 16.f) + j - draggedRectY1;
							char c = GetTile(i, j);
							char cDest = GetTile(offsetX, offsetY);
							if (!isInCopyMode) SetTile('.', i, j);
							switch (cDest) {
								// here we do special actions, for example, placing
								// a mushroom on top of a ? block
								case '?':
									if (c == 'M') c = '^'; // mushroom
									if (c == 'T') c = 'A'; // extra clock
									if (c == 'W') c = 'Q'; // flower
									if (c == 'X') c = 'L'; // life up
									break; // todo
							}
							SetTile(c, offsetX, offsetY);
							
							draggedRectX1 = -1;
							draggedRectX2 = -1;
							draggedRectY1 = -1;
							draggedRectY2 = -1;
							
							worldDragPosX = 0;
							worldDragPosY = 0;
						}
					}
					lastWorldTouchPosX = worldPosX;
					lastWorldTouchPosY = worldPosY;
				}
				
				fCameraPosY = nVisibleTilesY / 2.f;
				float moveSpeed = 10;
				bool b = GetKeyState(KEY_Y);
				if (b) moveSpeed *= 2;
				bool hitDirKey = false;
				if (GetKeyState(KEY_UP)) {
					fPlayerPosY -= moveSpeed * fElapsedTime;
					if (fPlayerPosY < 0) fPlayerPosY = 0;
					hitDirKey = true;
				}
				if (GetKeyState(KEY_DOWN)) {
					fPlayerPosY += moveSpeed * fElapsedTime;
					if (fPlayerPosY >= nLevelHeight) fPlayerPosY = nLevelHeight - 1;
				}
				if (GetKeyState(KEY_LEFT)) {
					fPlayerPosX -= moveSpeed * fElapsedTime;
					if (fPlayerPosX < 0) fPlayerPosX = 0;
					while (fPlayerPosX < fCameraPosX - 6) {
						fCameraPosX -= moveSpeed * fElapsedTime;
					}
					hitDirKey = true;
				}
				if (GetKeyState(KEY_RIGHT)) {
					fPlayerPosX += moveSpeed * fElapsedTime;
					if (fPlayerPosX >= nLevelWidth - 1) fPlayerPosX = nLevelWidth - 1;
					while (fPlayerPosX > fCameraPosX + 6) {
						fCameraPosX += moveSpeed * fElapsedTime;
					}
					hitDirKey = true;
				}
				if (GetKeyDownState(KEY_L)) {
					isInEraseMode ^= 1;
					isInCopyMode = 0;
				}
				if (GetKeyDownState(KEY_R)) {
					isInCopyMode ^= 1;
					isInEraseMode = 0;
				}
				if (GetKeyDownState(KEY_Y) && !hitDirKey) {
					// call player
					fPlayerPosX = fCameraPosX;
					fPlayerPosY = fCameraPosY;
				}
				
				if (playerState == State_Dead && fPlayerPosY > nLevelHeight) {
					fPlayerPosX = fCameraPosX;
					fPlayerPosY = fCameraPosY;
				}
				playerState = State_Idle;
				
				break; // todo
			}
			case Mode_Title: {
				//DrawString((SCREEN_WIDTH - 20 * 6) / 2, 40, WHITE, "GAME MAKER PROTOTYPE", 0);
				DrawImage(logoImage, 256, 32, (SCREEN_WIDTH - 192) / 2, 40);
				if (hasPressedLROrStart) {
					DrawString((SCREEN_WIDTH - 26 * 6) / 2, 100, WHITE, "Touch the buttons to play!", 0);
					Rectangle r[2] = { { (SCREEN_WIDTH - 96) / 2, 110, 48, 36}, {(SCREEN_WIDTH - 96) / 2 + 48, 110, 48, 36}};
					Point pTouch = { touch.px, touch.py };
					bool clicked[2] = { RectangleContains(r[0], pTouch), RectangleContains(r[1], pTouch)};
					const char* texts[2] = { makeText, playText };
					for (int i = 0; i < 2; i++) {
						DrawImage(&cbordImage[!clicked[i]], 48, 36, r[i].x, r[i].y);
						DrawString(r[i].x + 25 - strlen(texts[i]) * 3, r[i].y + 21, BLACK, texts[i], 0);
						DrawString(r[i].x + 24 - strlen(texts[i]) * 3, r[i].y + 20, WHITE, texts[i], 0);
					}
					if (!clicked[0] && hasClickedPreviously[0]) {
						hasPressedLROrStart = false;
						//gameMode = Mode_Maker;
						SetToMakerMode();
						LoadLevelToMem(level_1, level_1_width, level_1_height);
						canEditLevel = true;
					}
					if (!clicked[1] && hasClickedPreviously[1]) {
						hasPressedLROrStart = false;
						gameMode = Mode_Game;
					}
					memcpy(hasClickedPreviously, clicked, sizeof(clicked));
				} else {
					DrawString((SCREEN_WIDTH - 13 * 6) / 2, 140, WHITE, "Press [L]/[R]", 0);
					if (GetKeyState(KEY_L) && GetKeyState(KEY_R)) {
						hasPressedLROrStart = true;
					}
				}
			case Mode_Game: {
				// draw game UI?
				char ch[100];
				memset(ch, 0, 100);
				sprintf(ch, "%09d \xb%03d", score, (int)inGameTime);
				DrawString(SCREEN_WIDTH - 6 - 6 * strlen(ch), 5, WHITE, ch, 0);
				memset(ch, 0, 100);
				sprintf(ch, "\x9x%02d", gems);
				DrawString(6, 5, WHITE, ch, 0);
				
				int hpBarWidth = 184, hpBarX = 36;
				if (lastHP == 0) lastHP = hp;
				else lastHP = (lastHP * 7.f + (float)hp) / 8.f;
				int curHPWidth = (int)((float)lastHP * (float)hpBarWidth / (float)maxHP);
				FillRectangle(RED & 0xBDEF, hpBarX, 15, hpBarWidth, 3);
				FillRectangle(RED, hpBarX, 15, curHPWidth, 3);
				
				memset(ch, 0, 100);
				sprintf(ch, "%d", 0);
				DrawString(hpBarX - 3 - 6 * strlen(ch), 13, WHITE, ch, 0);
				memset(ch, 0, 100);
				sprintf(ch, "%d", maxHP);
				DrawString(hpBarX + hpBarWidth + 3, 13, WHITE, ch, 0);
				
				break; // todo
			}
			}
		}
		if (doSetToMakerMode) {
			doSetToMakerMode = false;
			gameMode = Mode_Maker;
			ResetEverything();
		}
	}
drawDialogAndReturn:
	if (dialogPosition < 191) DrawDialog(dialogShown);
	
	if (!hasShowedFatUnusableDlg && !isFileSystemEnabled) {
		hasShowedFatUnusableDlg = true;
		//ShowDialog(EmptyFunctionForInfoboxes, "OK", "OK", "The file system could not", "be initialized, you can't save", "levels.");	
	}
	
}
void LoadLevelToMem(char* c, int w, int h) {
	if (sLevel) delete[] sLevel;
	if (sLevelBeforePlay) delete[] sLevelBeforePlay;
	nLevelWidth = w;
	nLevelHeight = h;
	sLevel = new char[nLevelWidth * nLevelHeight];
	sLevelBeforePlay = new char[nLevelWidth * nLevelHeight];
	memcpy(sLevel, c, nLevelHeight * nLevelWidth);
	memcpy(sLevelBeforePlay, c, nLevelHeight * nLevelWidth);
}
void DoLevelInitStuff() {
	memcpy(sLevelBeforePlay, sLevel, nLevelHeight * nLevelWidth);
	playerState = State_Idle;
	int count = nLevelWidth * nLevelHeight;
	fPlayerPosX = fPlayerPosY = fPlayerVelX = fPlayerVelY = 0;
	for (int i = 0; i < count; i++) {
		/*
		if (sLevel[i] == '&') {
			if (sLevel[i + nLevelWidth] == 'G') 
				sLevel[i + nLevelWidth] = '}';
			else 
				sLevel[i + nLevelWidth] = '{';
		}
		*/
		
		if (sLevel[i] == 'S') {
			fPlayerPosX = i % nLevelWidth;
			fPlayerPosY = i / nLevelWidth;
			//sLevel[i] = '{';
		}
	}
	gems = 0;
	score = 0;
	inGameTime = maxTime;
	hp = 1;
	maxHP = 1;
	inGameTime = 500;
	gems = 0;
	score = 0;
	stunTimer = 0;
	lastHP = 1;
    ending = false;
	endingStage = 0;
}
void Game::Init() {
	LoadLevelToMem(level_1_old, level_1_width, level_1_height);
	DoLevelInitStuff();
}
void Game::LoadContent() {
	LogMsg("Loading content");
	decompress(tilesBitmap, textureCacheBmp,  LZ77);
	tileSpriteID = glLoadTileSet(tilesImage, 16, 16, 128, 128, GL_RGBA, TEXTURE_SIZE_128, TEXTURE_SIZE_128, GL_TEXTURE_WRAP_S|GL_TEXTURE_WRAP_T|TEXGEN_OFF|GL_TEXTURE_COLOR0_TRANSPARENT, 256, NULL, (u8*)textureCacheBmp);
	decompress(characterBitmap, textureCacheBmp,  LZ77);
	plyrSpriteID = glLoadTileSet(playrImage, 16, 16, 128, 16, GL_RGBA, TEXTURE_SIZE_128, TEXTURE_SIZE_16, GL_TEXTURE_WRAP_S|GL_TEXTURE_WRAP_T|TEXGEN_OFF|GL_TEXTURE_COLOR0_TRANSPARENT, 256, NULL, (u8*)textureCacheBmp);
	decompress(clapperboardBitmap, textureCacheBmp,  LZ77);
	cbrdSpriteID = glLoadTileSet(cbordImage, 48, 36, 128, 64, GL_RGBA, TEXTURE_SIZE_128, TEXTURE_SIZE_64, GL_TEXTURE_WRAP_S|GL_TEXTURE_WRAP_T|TEXGEN_OFF|GL_TEXTURE_COLOR0_TRANSPARENT, 0, NULL, (u8*)textureCacheBmp);
	decompress(itemselBitmap, textureCacheBmp,  LZ77);
	itemSelSpriteID = glLoadTileSet(itemSImage, 32,32,32,32, GL_RGBA, TEXTURE_SIZE_32, TEXTURE_SIZE_32, GL_TEXTURE_WRAP_S|GL_TEXTURE_WRAP_T|TEXGEN_OFF|GL_TEXTURE_COLOR0_TRANSPARENT, 0, NULL, (u8*)textureCacheBmp);
	decompress(itemBitmap, textureCacheBmp,  LZ77);
	itemSpriteID = glLoadTileSet(itemImage, 32,32,32,32, GL_RGBA, TEXTURE_SIZE_32, TEXTURE_SIZE_32, GL_TEXTURE_WRAP_S|GL_TEXTURE_WRAP_T|TEXGEN_OFF|GL_TEXTURE_COLOR0_TRANSPARENT, 0, NULL, (u8*)textureCacheBmp);
	decompress(lr_arrowsBitmap, textureCacheBmp,  LZ77);
	lrArSpriteID = glLoadTileSet(lrArrowImage, 32,16,32,32, GL_RGBA, TEXTURE_SIZE_32, TEXTURE_SIZE_32, GL_TEXTURE_WRAP_S|GL_TEXTURE_WRAP_T|TEXGEN_OFF|GL_TEXTURE_COLOR0_TRANSPARENT, 0, NULL, (u8*)textureCacheBmp);
	decompress(undoBitmap, textureCacheBmp,  LZ77);
	undoSpriteID = glLoadTileSet(undoImage, 32,32,32,32, GL_RGBA, TEXTURE_SIZE_32, TEXTURE_SIZE_32, GL_TEXTURE_WRAP_S|GL_TEXTURE_WRAP_T|TEXGEN_OFF|GL_TEXTURE_COLOR0_TRANSPARENT, 0, NULL, (u8*)textureCacheBmp);
	decompress(eraseBitmap, textureCacheBmp,  LZ77);
	eraseSpriteID = glLoadTileSet(eraseImage, 32,32,32,32, GL_RGBA, TEXTURE_SIZE_32, TEXTURE_SIZE_32, GL_TEXTURE_WRAP_S|GL_TEXTURE_WRAP_T|TEXGEN_OFF|GL_TEXTURE_COLOR0_TRANSPARENT, 0, NULL, (u8*)textureCacheBmp);
	decompress(resetBitmap, textureCacheBmp,  LZ77);
	resetSpriteID = glLoadTileSet(resetImage, 32,32,32,32, GL_RGBA, TEXTURE_SIZE_32, TEXTURE_SIZE_32, GL_TEXTURE_WRAP_S|GL_TEXTURE_WRAP_T|TEXGEN_OFF|GL_TEXTURE_COLOR0_TRANSPARENT, 0, NULL, (u8*)textureCacheBmp);
	decompress(erase_modeBitmap, textureCacheBmp,  LZ77);
	ermodSpriteID = glLoadTileSet(ermodImage,64,16,64,16, GL_RGBA, TEXTURE_SIZE_64, TEXTURE_SIZE_16, GL_TEXTURE_WRAP_S|GL_TEXTURE_WRAP_T|TEXGEN_OFF|GL_TEXTURE_COLOR0_TRANSPARENT, 0, NULL, (u8*)textureCacheBmp);
	decompress(copy_modeBitmap, textureCacheBmp,  LZ77);
	cpmodSpriteID = glLoadTileSet(cpmodImage,64,16,64,16, GL_RGBA, TEXTURE_SIZE_64, TEXTURE_SIZE_16, GL_TEXTURE_WRAP_S|GL_TEXTURE_WRAP_T|TEXGEN_OFF|GL_TEXTURE_COLOR0_TRANSPARENT, 0, NULL, (u8*)textureCacheBmp);
	decompress(dialog_bgBitmap, textureCacheBmp,  LZ77);
	dlgBgSpriteID = glLoadTileSet(dlgBgImage,256,256,256,256, GL_RGBA, TEXTURE_SIZE_256, TEXTURE_SIZE_256, GL_TEXTURE_WRAP_S|GL_TEXTURE_WRAP_T|TEXGEN_OFF|GL_TEXTURE_COLOR0_TRANSPARENT, 0, NULL, (u8*)textureCacheBmp);
	decompress(dialogbtnBitmap, textureCacheBmp,  LZ77);
	dlgBtnSpriteID = glLoadTileSet(dlgBtnImage,128,32,128,32, GL_RGBA, TEXTURE_SIZE_128, TEXTURE_SIZE_32, GL_TEXTURE_WRAP_S|GL_TEXTURE_WRAP_T|TEXGEN_OFF|GL_TEXTURE_COLOR0_TRANSPARENT, 0, NULL, (u8*)textureCacheBmp);
	decompress(logoBitmap, textureCacheBmp,  LZ77);
	logoSpriteID = glLoadTileSet(logoImage,256,32,256,32, GL_RGBA, TEXTURE_SIZE_256, TEXTURE_SIZE_32, GL_TEXTURE_WRAP_S|GL_TEXTURE_WRAP_T|TEXGEN_OFF|GL_TEXTURE_COLOR0_TRANSPARENT, 0, NULL, (u8*)textureCacheBmp);
	
	memcpy(hotbarSlots, hotbarSlotPages, 8);
	
	Enemy::CommonLoad();
}
time_t currentTime;
void Title::Update(){
	DrawString((SCREEN_WIDTH - 20 * 6) / 2, 30, WHITE, "GAME MAKER PROTOTYPE", 0);
	DrawString((SCREEN_WIDTH - 30 * 6) / 2, 90, WHITE, "Touch the Touch Screen to play", 0);
	if ((touch.px != 0 || touch.py != 0) && !(prevTouch.px != 0 || prevTouch.py != 0)) {
		LogMsg("Touch2 at x:%d, y:%d", touch.px, touch.py);
		//mode = PlayMode::Mode::Game;
	}
	char s[100];
	memset(s,0,100);
	currentTime = time(nullptr);
	auto m = gmtime(&currentTime);
	strftime(s, 99, "TOD: %a, %d %b %Y at %H:%M:%S", m);
	DrawString(3, 3, WHITE, s, 0);
}
void Title::Init() {
	
}
void Title::LoadContent() {
	
}
