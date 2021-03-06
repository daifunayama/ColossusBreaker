#include "Application.h"
#include "Input/Input.h"
#include "SSPlayer/SS5Player.h"
#include "Effekseer/AnimationController.h"
#include "DxLib.h"

int Application::mOldSceneId = Parameter::SCENE_TITLE;
int Application::mNextSceneId = Parameter::SCENE_TITLE;
int Application::mChapterId = 1;

ss::ResourceManager* Application::resman = nullptr;

/*ゲームのメインループの処理*/
void Application::MainLoop() {
	Input::getInstance().Update();

	//シーンIDが異なる場合はシーン切り替え
	if (mNextSceneId != mOldSceneId) {

		delete mScene;
		AnimationController::getInstance().Reset();

		switch (mNextSceneId) {

		case Parameter::SCENE_TITLE:
			mScene = new TitleScene();
			break;

		case Parameter::SCENE_NOVEL:
			mScene = new NovelScene();
			break;

		case Parameter::SCENE_BATTLE :
			mScene = new BattleScene();
			break;
		}

		mOldSceneId = mNextSceneId;
		
	}
	
	mScene->Process();
	mScene->Drawing();
}

/*キャラスプライトのリソース追加*/
void  Application::AddSpriteResource() {
	Application::resman->addData("Data/E0/mill.ssbp");
	Application::resman->addData("Data/E1/spider.ssbp");
	//Application::resman->addData("Data/E2/armor.ssbp");
	Application::resman->addData("Data/heroine/chisel.ssbp");
}