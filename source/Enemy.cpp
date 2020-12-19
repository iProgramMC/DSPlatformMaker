#include "Enemy.hpp"


#include "zombie.h"

glImage* mushroomImage;
glImage zombieImage[8];
short zombieBmp[128*16];
int zombSpriteID;

Enemy::Enemy() {
	
}
Enemy::~Enemy() {
	
}

void Enemy::Update (float fElapsedTime) {
	if (this->posY > nLevelHeight) return;
	if (this->posX > nLevelWidth) return;
	if (this->posX < 0) return;
	if (this->markForRemoval) return;
	switch (type) {
		case Enemy_Mushroom:
		case Enemy_LifeUp: {
			if (mushroom.risingTimer > 0.0f) {
				if ((BlockBumpedX != (int)homeX || BlockBumpedY != (int)homeY)) {
					mushroom.risingTimer -= fElapsedTime;
					posY -= fElapsedTime;
					if (mushroom.risingTimer <= 0.0f) {
						mushroom.risingTimer = 0.0f;
					}
				}
				velX = 5;
			} else {
				// gravity
				velY += 20.0f * fElapsedTime;
				isOnGround = false;
				for (int i = 0; i < 5; i++) {
					float newX = posX + fElapsedTime * velX * .2f;
					float newY = posY + fElapsedTime * velY * .2f;
					
					if (velX < 0) {
						if (IsSolid(GetTile(newX + 0.0f, posY + 0.0f), DIR_LEFT) || 
							IsSolid(GetTile(newX + 0.0f, posY + 0.9f), DIR_LEFT)) {
							velX = -velX;
							newX = (int)newX + 1;
						}
					} else if (velX > 0) {
						if (IsSolid(GetTile(newX + 1.0f, posY + 0.0f), DIR_RIGHT) || 
							IsSolid(GetTile(newX + 1.0f, posY + 0.9f), DIR_RIGHT)) {
							velX = -velX;
							newX = (int)newX;
						}
					}
					if (velY < 0) {
						// no triggering blocks
						if (IsSolid(GetTile(newX + 0.0f, newY), DIR_UP) || 
							IsSolid(GetTile(newX + 0.9f, newY), DIR_UP)) {
							velY = 0;
							newY = (int)newY + 1;
						}
					}
					if (velY > 0) {
						if (IsSolid(GetTile(newX + 0.0f, newY + 1.0f), DIR_DOWN) || 
							IsSolid(GetTile(newX + 0.9f, newY + 1.0f), DIR_DOWN)) {
							velY = 0;
							newY = (int)newY;
						}
					}
					posX = newX; posY = newY;
				}
				if (isOnGround) {
					if (velX == 0) velX = 5;
				}
				Rectangle ri = { (int)(posX * 100), (int)(posY * 100), 100, 100};
				Rectangle rp = { (int)(fPlayerPosX * 100 + 20), (int)(fPlayerPosY * 100), 100, 80 };
				if (RectangleIntersect(ri, rp)) {
					if (type == Enemy_LifeUp)
						IncreaseMaxHP();
					else 
						HealPlayer();
					// have to be overlapping with the mushroom quite a bit
					this->markForRemoval = true;
					score += 1000; // todo: increase lives
				}
			}
			break;
		}
		case Enemy_Flower: 
		case Enemy_Time: {
			if (mushroom.risingTimer > 0.0f) {
				if ((BlockBumpedX != (int)homeX || BlockBumpedY != (int)homeY)) {
					mushroom.risingTimer -= fElapsedTime;
					posY -= fElapsedTime;
					if (mushroom.risingTimer <= 0.0f) {
						mushroom.risingTimer = 0.0f;
					}
				}
			} else {
				// gravity
				velY += 20.0f * fElapsedTime;
				isOnGround = false;
				for (int i = 0; i < 5; i++) {
					float newX = posX + fElapsedTime * velX * .2f;
					float newY = posY + fElapsedTime * velY * .2f;
					
					if (velX < 0) {
						if (IsSolid(GetTile(newX + 0.0f, posY + 0.0f), DIR_LEFT) || 
							IsSolid(GetTile(newX + 0.0f, posY + 0.9f), DIR_LEFT)) {
							velX = -velX;
							newX = (int)newX + 1;
						}
					} else if (velX > 0) {
						if (IsSolid(GetTile(newX + 1.0f, posY + 0.0f), DIR_RIGHT) || 
							IsSolid(GetTile(newX + 1.0f, posY + 0.9f), DIR_RIGHT)) {
							velX = -velX;
							newX = (int)newX;
						}
					}
					if (velY < 0) {
						// no triggering blocks
						if (IsSolid(GetTile(newX + 0.0f, newY), DIR_UP) || 
							IsSolid(GetTile(newX + 0.9f, newY), DIR_UP)) {
							velY = 0;
							newY = (int)newY + 1;
						}
					}
					if (velY > 0) {
						if (IsSolid(GetTile(newX + 0.0f, newY + 1.0f), DIR_DOWN) || 
							IsSolid(GetTile(newX + 0.9f, newY + 1.0f), DIR_DOWN)) {
							velY = 0;
							newY = (int)newY;
						}
					}
					posX = newX; posY = newY;
				}
				Rectangle ri = { (int)(posX * 100), (int)(posY * 100), 100, 100};
				// offset the player rectangle a bit to avoid collecting while inside the box
				Rectangle rp = { (int)(fPlayerPosX * 100 + 20), (int)(fPlayerPosY * 100), 100, 80 };
				if (RectangleIntersect(ri, rp)) {
					// have to be overlapping with the mushroom quite a bit
					this->markForRemoval = true;
					score += 1000;
					if (type == Enemy_Time) {
						inGameTime += 10.f;
					} else {
						DamagePlayer();
					}
				}
			}
			break;
		}
	}
}

void Enemy::CommonDrawAtOffset(glImage* img, float pX, float pY, float oX, float oY, int flipMode) {
	int sx = (posX - oX) * 16;
	int sy = (posY - oY) * 16 + 1;
	//glSprite(sx, sy, flipMode, img);
	DrawImage(img, img->width, img->height, sx, sy);
}

void Enemy::Draw (float fOffsetX, float fOffsetY) {
	glImage* toDraw = nullptr;
	int flipMode = 0;
	switch (type) {
		case Enemy_Mushroom: {
			flipMode = this->mushroom.movingLeft ? GL_FLIP_H : 0;
			// This should move the mushroom to behind the blocks during its rise anim
			if ((BlockBumpedX != (int)homeX || BlockBumpedY != (int)homeY)) {
				CommonDrawAtOffset(&tilesImage[33], posX, posY, fOffsetX, fOffsetY, flipMode);
				if (homeX != 0 || homeY != 0) {
					int mx = (homeX - fOffsetX) * 16;
					int my = (homeY - fOffsetY) * 16;
					DrawTile('U',mx,my,'.');
				}
			}
			break;
		}
		case Enemy_Flower: {
			flipMode = this->mushroom.movingLeft ? GL_FLIP_H : 0;
			// This should move the mushroom to behind the blocks during its rise anim
			if ((BlockBumpedX != (int)homeX || BlockBumpedY != (int)homeY)) {
				CommonDrawAtOffset(&tilesImage[36], posX, posY, fOffsetX, fOffsetY, flipMode);
				if (homeX != 0 || homeY != 0) {
					int mx = (homeX - fOffsetX) * 16;
					int my = (homeY - fOffsetY) * 16;
					DrawTile('U',mx,my,'.');
				}
			}
			break;
		}
		case Enemy_Time: {
			flipMode = this->mushroom.movingLeft ? GL_FLIP_H : 0;
			// This should move the mushroom to behind the blocks during its rise anim
			if ((BlockBumpedX != (int)homeX || BlockBumpedY != (int)homeY)) {
				CommonDrawAtOffset(&tilesImage[37], posX, posY, fOffsetX, fOffsetY, flipMode);
				if (homeX != 0 || homeY != 0) {
					int mx = (homeX - fOffsetX) * 16;
					int my = (homeY - fOffsetY) * 16;
					DrawTile('U',mx,my,'.');
				}
			}
			break;
		}
		case Enemy_LifeUp: {
			flipMode = this->mushroom.movingLeft ? GL_FLIP_H : 0;
			// This should move the mushroom to behind the blocks during its rise anim
			if ((BlockBumpedX != (int)homeX || BlockBumpedY != (int)homeY)) {
				CommonDrawAtOffset(&tilesImage[38], posX, posY, fOffsetX, fOffsetY, flipMode);
				if (homeX != 0 || homeY != 0) {
					int mx = (homeX - fOffsetX) * 16;
					int my = (homeY - fOffsetY) * 16;
					DrawTile('U',mx,my,'.');
				}
			}
			break;
		}
		case Enemy_Zombie:
			flipMode = this->zombie.movingLeft ? GL_FLIP_H : 0;
			toDraw = &zombieImage[1];
			CommonDrawAtOffset(toDraw, posX, posY, fOffsetX, fOffsetY, flipMode);
			break;
	}
}

void Enemy::CommonLoad() {
	mushroomImage = &tilesImage[33];
	decompress(zombieBitmap, zombieBmp, LZ77);
	zombSpriteID = glLoadTileSet(zombieImage,16,16,128,16, GL_RGBA, TEXTURE_SIZE_128, TEXTURE_SIZE_16, GL_TEXTURE_WRAP_S|GL_TEXTURE_WRAP_T|TEXGEN_OFF|GL_TEXTURE_COLOR0_TRANSPARENT, 0, NULL, (u8*)zombieBmp);
}