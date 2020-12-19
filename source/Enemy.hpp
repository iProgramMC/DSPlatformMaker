#include "TestGame.hpp"

#define Enemy_Mushroom 1
#define Enemy_Zombie 2
#define Enemy_Flower 3
#define Enemy_Time 4
#define Enemy_LifeUp 5

class Enemy {
public:
	int type;
	float posX = 0, posY = 0;
	float homeX = 0, homeY = 0;
	float velX = 0, velY = 0;
	bool isOnGround = false;
	bool markForRemoval = false;
	union {
		struct {
			bool movingLeft = false;
			float risingTimer = 0.0f;
		} mushroom;
		struct {
			bool movingLeft = false;
		} zombie;
	};
	Enemy(); ~Enemy();
	void CommonDrawAtOffset(glImage* img, float pX, float pY, float oX, float oY, int flipMode);
	void Update(float deltaTime);
	void Draw(float fOffsetX, float fOffsetY);
	static void CommonLoad();
};