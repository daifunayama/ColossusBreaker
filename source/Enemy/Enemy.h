#pragma once
#include "../SSPlayer/SS5Player.h"
#include "../Quad/Quad.h"

/*
敵を制御するクラス
*/
class Enemy {
public:
	Quad getQuad(const char* name);

	void LoadGraphic();
	void Process();
	void Draw();

private:
	ss::Player *mSprite;
};