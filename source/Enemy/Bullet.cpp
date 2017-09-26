#include "Bullet.h"
#include "../Parameter.h"
#include "../Camera/Camera.h"
#include <math.h>

void Bullet::Load() {
	mGraph = LoadGraph("Data/graphic/bullet.png");
	mSE = LoadSoundMem("Data/se/shot1.mp3");
}

void Bullet::CreateShot(int x, int y, float angle, int speed, int r) {
	for (int i = 0; i < 200; i++) {
		if (!mShot[i].exist) {
			mShot[i].exist = true;
			mShot[i].positionX = x;
			mShot[i].positionY = y;
			mShot[i].angle = angle;
			mShot[i].speed = speed;
			mShot[i].r = r;
			
			PlaySoundMem(mSE, DX_PLAYTYPE_BACK);
			break;
		}
	}
}


void Bullet::Process() {
	for (int i = 0; i < 200; i++) {
		if (mShot[i].exist) {
			mShot[i].positionX += mShot[i].speed * cosf(mShot[i].angle);
			mShot[i].positionY += mShot[i].speed * sinf(mShot[i].angle);
		}
	}
}


void Bullet::Draw() {
	for (int i = 0; i < 200; i++) {
		if (mShot[i].exist) {
			DrawRotaGraph(mShot[i].positionX - Camera::getInstance().getPositonX(), 
				mShot[i].positionY - Camera::getInstance().getPositonY(), 1, 0, mGraph,1, 0);
		}
	}
}