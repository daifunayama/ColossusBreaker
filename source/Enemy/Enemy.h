#pragma once
#include "../SSPlayer/SS5Player.h"

/*
敵を制御するクラス
*/
class Enemy {
public:
	ss::Quad getQuad(const char* name);

	void LoadGraphic();
	void Process();
	void Draw();

private:
	ss::Player *mSprite;
};