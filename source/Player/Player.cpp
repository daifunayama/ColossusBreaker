#include "Player.h"
#include "../Enemy/Enemys/E1.h"
#include "../Camera/Camera.h"
#include "../Parameter.h"
#include "../Utility.h"
#include "../SSPlayer/SS5Player.h"
#include "../Effekseer/AnimationController.h"
#include "DxLib.h"

void Player::Load() {
	//プレイヤーの作成
	mSprite = ss::Player::create();

	//プレイヤーにリソースを割り当て
	mSprite->setData("chisel");// ssbpファイル名

	mSprite->play("chisel/idle");// アニメーション名を指定

	//表示位置を設定
	mSprite->setPosition(400, 500);
	//スケール設定
	mSprite->setScale(0.3f, 0.3f);

	mSprite->setStep(0.6f);

	mGraphShadow = LoadGraph("Data/heroine/shadow.png");
	mGraphStopper = LoadGraph("Data/heroine/stop.png");

	mAnimeFire = LoadGraph("Data/graphic/animation/hibana.png");
	mAnimeCatch = LoadGraph("Data/graphic/animation/catch.png");

	mSoundCatch = LoadSoundMem("Data/heroine/robot-footstep2.mp3");
	mSoundBreakBolt = LoadSoundMem("Data/heroine/電気ドリル02.mp3");
	mSoundStep = LoadSoundMem("Data/heroine/se_maoudamashii_se_footstep01.mp3");
	mSoundJump = LoadSoundMem("Data/heroine/highspeed-movement1.mp3");
	mSoundLand = LoadSoundMem("Data/heroine/landing1.mp3");

	mState = Parameter::S_PLAYER_NORMAL;
	mPositionDX = 100;
	mPositionDY = 100;
	mPositionX = 100;
	mPositionY = 100;
	mAcceleX = 0;
	mAcceleY = 0;
	mGround = false; 
	mRight = true;
	mCatchId = 0;
	mCatchLength = 0;
	mCatchRad = 0;
	mBoltBreakCounter = 0;
}

void Player::Move(Enemy &enemy) {
	Quad mQuad;
	ss::ResluteState pState, state;
	string pass;

	mController.ConvertInput();


	//静止・歩行状態のとき
	if (mState == Parameter::S_PLAYER_NORMAL) {

		//右方向への移動
		if (mController.getRight() > 0) {
			if (mAcceleX < 10)mAcceleX += 2;
		}

		//左方向への移動
		if (mController.getLeft() > 0) {
			if (mAcceleX > -10)mAcceleX -= 2;
		}
	}

	if (mState == Parameter::S_PLAYER_NORMAL) {
		//ジャンプする
		if (mController.getKey(4) == 1) {

			if (mGround) {
				mState = Parameter::S_PLAYER_NORMAL;
				DoJump();
			}
		}
	}

	//機体をつかむ
	if (mState == Parameter::S_PLAYER_NORMAL) {

		if (mController.getKey(5) == 1)mCatching = true;
		if (!mController.getKey(5)) mCatching = false;

		mSprite->getPartState(pState, "root");

		for (int n = 0; n < enemy.getNumHit(); n++) {
			if (enemy.getHitExist(n)) {
				pass.clear();
				pass = "hit" + Utility::IntToString(n + 1);
				mQuad = enemy.getQuad(pass.c_str());

				if (Utility::CheckQuadPointHit(mQuad, pState.x, Parameter::WINDOW_HEIGHT - pState.y-150) && mCatching && mCatchId == 0) {
				
					enemy.getSprite().getPartState(state, pass.c_str());

					mState = Parameter::S_PLAYER_CATCH;
					mCatchRad = -atan2f(state.y - pState.y-150, state.x - pState.x) + state.rotationZ * Parameter::PI / 180;
					mCatchLength = sqrtf(pow(state.x - pState.x, 2.0) + pow(state.y - pState.y-150, 2.0));

					mCatchId = n+1;

					mAcceleX = 0;
					mAcceleY = 0;

					PlaySoundMem(mSoundCatch, DX_PLAYTYPE_BACK);

					mAnimeCatchKey = AnimationController::getInstance().Create(mAnimeCatch, 2, mPositionDX, mPositionDY, 300, 300, 0.6, 0, 4, 1, 20, -1, 0, 1);
				}
			}
		}

		for (int n = 0; n < enemy.getNumBolt(); n++) {
			if (enemy.getBoltExist(n)) {
				pass.clear();
				pass = "bolt" + Utility::IntToString(n + 1);
				mQuad = enemy.getQuad(pass.c_str());

				if (Utility::CheckQuadPointHit(mQuad, pState.x, Parameter::WINDOW_HEIGHT - pState.y-150) && mCatching && mCatchId == 0) {

					mState = Parameter::S_PLAYER_CATCH;
					mCatchRad = 0;
					mCatchLength = 0;

					mCatchId = n + 101;

					mAcceleX = 0;
					mAcceleY = 0;

					PlaySoundMem(mSoundCatch, DX_PLAYTYPE_BACK);
				}
			}
		}
	}
	
	//つかみ状態
	if (mState == Parameter::S_PLAYER_CATCH) {
		AnimationController::getInstance().SetPosition(mAnimeCatchKey, mPositionDX, mPositionDY-150);

		if (!mController.getKey(5)) {
			mCatching = false;
			mGround = false;
			mState = Parameter::S_PLAYER_NORMAL;
			mCatchId = 0;
			mBoltBreakCounter = 0;
		}
		else {
			if (mController.getKey(4) == 1) {
				mState = Parameter::S_PLAYER_NORMAL;
				mCatching = false;
				DoJump();
				mCatchId = 0;
				mBoltBreakCounter = 0;
			}
			else {
				if (mCatchId != 0) {

					//はりつきポイント
					if (enemy.getHitExist(mCatchId-1)) {
						pass.clear();
						pass = "hit" + Utility::IntToString(mCatchId);

						enemy.getSprite().getPartState(state, pass.c_str());

						mSprite->setPosition(state.x - mCatchLength * cosf(mCatchRad - state.rotationZ * Parameter::PI / 180),
							state.y -150 + mCatchLength * sinf(mCatchRad - state.rotationZ * Parameter::PI / 180));

						mSprite->getPartState(pState, "root");
						mPositionDX = pState.x + Camera::getInstance().getPositonX();
						mPositionDY = Parameter::WINDOW_HEIGHT - pState.y + Camera::getInstance().getPositonY();
					}

					//ボルト
					if (mCatchId > 100) {
						if (enemy.getBoltExist(mCatchId - 101)) {
							pass.clear();
							pass = "bolt" + Utility::IntToString(mCatchId-100);

							enemy.getSprite().getPartState(state, pass.c_str());

							mSprite->setPosition(state.x,state.y - 150);

							mSprite->getPartState(pState, "root");
							mPositionDX = pState.x + Camera::getInstance().getPositonX();
							mPositionDY = Parameter::WINDOW_HEIGHT - pState.y + Camera::getInstance().getPositonY();

							if (mController.getKey(6) == 1) {
								mBoltBreakCounter = 1;
								
								mAnimeFireKey = AnimationController::getInstance().Create(mAnimeFire, 11, mPositionDX, mPositionDY, 100, 250, 4, 0, 16, 2, 250, -1, 0, 1);
							}
							if (mController.getKey(6) && mBoltBreakCounter) {
								mBoltBreakCounter++;
								AnimationController::getInstance().SetPosition(mAnimeFireKey, mPositionDX, mPositionDY+250);
							}
							if (!mController.getKey(6)) {
								mBoltBreakCounter = 0;
								if (CheckSoundMem(mSoundBreakBolt))StopSoundMem(mSoundBreakBolt);
								AnimationController::getInstance().Remove(mAnimeFireKey);
							}

							if (mBoltBreakCounter == 10) {
								PlaySoundMem(mSoundBreakBolt, DX_PLAYTYPE_BACK);
							}

							if (mBoltBreakCounter >= 150) {
								enemy.BrokenBolt(mCatchId - 100);
								mCatching = false;
								mState = Parameter::S_PLAYER_NORMAL;
								mCatchId = 0;
								mBoltBreakCounter = 0;
								mAcceleY = 42;
								if (CheckSoundMem(mSoundBreakBolt))StopSoundMem(mSoundBreakBolt);
								AnimationController::getInstance().Remove(mAnimeFireKey);
							}
						}
					}
				}
			}
		}
	}

	//加速度を0にする
	if ((!mController.getRight() && !mController.getLeft())
		&& mGround && mState == Parameter::S_PLAYER_NORMAL)
	{
		mAcceleX = 0;
	}


	//重力の処理
	if (!mGround && mState == Parameter::S_PLAYER_NORMAL)mAcceleY -= 2;

	if (mAcceleX > 0)mRight = true;
	if (mAcceleX < 0)mRight = false;

	//プレイヤーの移動
	mPositionDX += mAcceleX;
	mPositionDY -= mAcceleY;

	mPositionX = (int)mPositionDX;
	mPositionY = (int)mPositionDY;

	//壁と床の接触判定
	CheckWallHit();


	if (mState == Parameter::S_PLAYER_NORMAL) {
		mSprite->setPosition(mPositionX - Camera::getInstance().getPositonX(),
			Parameter::WINDOW_HEIGHT - mPositionY + Camera::getInstance().getPositonY());
	}
	else {

	}

	if (mCatchId == 0) {

		if (!mGround){
			if (mAcceleY == 40) {
				mSprite->play("chisel/jump");
				mSprite->setStep(0.8f);
				mSprite->setLoop(1);
			}
			else {
				if (mSprite->getPlayAnimeName() != "jump") {
					mSprite->play("chisel/jump");
					mSprite->setStep(0.8f);
					mSprite->setFrameNo(20);
					mSprite->setLoop(1);
				}
			}
		}
		else {
			if (mAcceleX == 0) {
				if (mSprite->getPlayAnimeName() != "idle") {
					mSprite->play("chisel/idle");
					mSprite->setStep(0.6f);
				}
			}
			else {
				if (mSprite->getPlayAnimeName() != "dash") {
					mSprite->play("chisel/dash");
					mSprite->setStep(0.75f);
				}
				if (mSprite->getFrameNo() == 14 || mSprite->getFrameNo() == 40)PlaySoundMem(mSoundStep, DX_PLAYTYPE_BACK);
			}
		}

		mSprite->setScale(mRight ? 0.25f : -0.25f, 0.25f);
	}
	else {
		//ボルトを回すとき
		if (mBoltBreakCounter > 0){
			if (mSprite->getPlayAnimeName() != "catch2") {
				mSprite->play("chisel/catch2");
				mSprite->setStep(0.7f);
				mSprite->setLoop(1);
			}
		}
		//それ以外
		else if (mSprite->getPlayAnimeName() != "catch") {
			mSprite->play("chisel/catch");
			mSprite->setStep(0.5f);
		}
		mSprite->setScale(0.25f, 0.25f);
	}

	mSprite->update((float)30 / 1000);
}

void Player::Catch() {

}

/*ジャンプする*/
void Player::DoJump() {
	//右向き
	if (mController.getRight())mAcceleX = 8;
	//左向き
	else if (mController.getLeft())mAcceleX = -8;
	//垂直
	else mAcceleX = 0;

	//Y軸加速度
	mAcceleY = 42;

	//接地フラグをfalseに
	mGround = false;

	PlaySoundMem(mSoundJump, DX_PLAYTYPE_BACK);
}

/*壁・床とのヒットチェック*/
void Player::CheckWallHit() {
	/*
	//右向きのとき
	if (mRight) {
		//左側
		if (mPositionX + mHitBox[mState].getPositionX() < 0) {
			mPositionDX = -mHitBox[mState].getPositionX();
		}
		//右側
		if (mPositionX + mHitBox[mState].getPositionX() + mHitBox[mState].getWidth() > Parameter::FIELD_WIDTH) {
			mPositionDX = Parameter::FIELD_WIDTH - mHitBox[mState].getWidth() - mHitBox[mState].getPositionX();
		}
	}

	//左向きのとき
	else {
		//左側
		if (mPositionX - mHitBox[mState].getPositionX() - mHitBox[mState].getWidth() < 0) {
			mPositionDX = mHitBox[mState].getPositionX() + mHitBox[mState].getWidth();
		}
		//右側
		if (mPositionX - mHitBox[mState].getPositionX() > Parameter::FIELD_WIDTH) {
			mPositionDX = Parameter::FIELD_WIDTH + mHitBox[mState].getPositionX();
		}
	}
	*/
	//床
	if (mPositionDY > Parameter::GROUND_LINE - 100) {
		mPositionDY = Parameter::GROUND_LINE - 100;
		mAcceleY = 0;
		mAcceleX = 0;
		mGround = true;

		PlaySoundMem(mSoundLand, DX_PLAYTYPE_BACK);
	}

	mPositionX = (int)mPositionDX;
	mPositionY = (int)mPositionDY;
}

void Player::Draw() {
	ss::ResluteState pState;
	mSprite->getPartState(pState, "root");

	if(mGround)DrawGraph(pState.x-60,Parameter::WINDOW_HEIGHT - pState.y +95, mGraphShadow, 1);

	if (mBoltBreakCounter > 0) {
		if (mBoltBreakCounter < 10)DrawRotaGraph(pState.x, Parameter::WINDOW_HEIGHT - pState.y - 150, 0.08*mBoltBreakCounter, 0, mGraphStopper, 1, 0);
		else DrawRotaGraph(pState.x, Parameter::WINDOW_HEIGHT - pState.y - 150, 0.8, -mBoltBreakCounter*0.1, mGraphStopper, 1, 0);
	}

	mSprite->draw();

	DrawBox(pState.x - 15, Parameter::WINDOW_HEIGHT - pState.y - 150 - 1, pState.x + 15, Parameter::WINDOW_HEIGHT - pState.y - 150 + 1, Parameter::COLOR_RED, 1);
	DrawBox(pState.x - 1, Parameter::WINDOW_HEIGHT - pState.y - 150 - 15, pState.x + 1, Parameter::WINDOW_HEIGHT - pState.y - 150 + 15, Parameter::COLOR_RED, 1);
	DrawCircle(pState.x, Parameter::WINDOW_HEIGHT - pState.y - 150, 3, Parameter::COLOR_RED, 1, 1);

	//DrawCircle(mPositionX - Camera::getInstance().getPositonX(),
	//	mPositionY - Camera::getInstance().getPositonY(), 3, Parameter::COLOR_RED, true, true);

	//DrawFormatString(10, 50, Parameter::COLOR_RED, "state :%d  catchlength:%5f  catchrad:%5f  catchId:%d", mState, mCatchLength,mCatchRad,mCatchId);

	//ボルトをつかんでいる間ゲージを表示
	if (mCatchId > 100) {
		DrawBox(pState.x - 75, Parameter::WINDOW_HEIGHT - pState.y - 200, pState.x + 75, Parameter::WINDOW_HEIGHT - pState.y - 220, Parameter::COLOR_GRAY, 1);
		DrawBox(pState.x - 75, Parameter::WINDOW_HEIGHT - pState.y - 200, pState.x - 75 + mBoltBreakCounter, Parameter::WINDOW_HEIGHT - pState.y - 220, Parameter::COLOR_RED, 1);
	}
}